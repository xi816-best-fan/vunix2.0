#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include "vfs.h"
#include "exec.h"
#include "panic.h"

int cpu_read8(risc_gc* cpu, short address) {
  address = address % MEMORY_SIZE;
  return cpu->memory[address];
}

int cpu_read(risc_gc* cpu, short address) {
  address = address % MEMORY_SIZE;
  return (cpu->memory[address] << 24) | (cpu->memory[address+1] << 16) | (cpu->memory[address+2] << 8) | cpu->memory[address+3];
}

void cpu_write(risc_gc* cpu, short address, char value) {
  cpu->memory[address % MEMORY_SIZE] = value;
}

void update_flags(risc_gc* cpu, short result) {
  if(result == 0) {
    cpu->flags |= FLAG_ZERO;
  } else {
    cpu->flags &= ~FLAG_ZERO;
  }
  if(result & 0x8000) {
    cpu->flags |= FLAG_SIGN;
  } else {
    cpu->flags &= ~FLAG_SIGN;
  }
}

int cpu_step(risc_gc* cpu) {
  cpu->ir = cpu_read(cpu, cpu->pc);
  char opcode = (cpu->ir & 0xFF000000) >> 24;
  char reg1   = (cpu->ir & 0x00F00000) >> 20;
  char reg2   = (cpu->ir & 0x000F0000) >> 16;
  char reg3   = (cpu->ir & 0x0000F000) >> 12;
  short imm   = (cpu->ir & 0x00000FFF);
  // printf("Opcode: \033[93m%02X\033[0m\n", opcode);

    switch(opcode) {
      case 0x00: //NOP
        break;
        
      case 0x01: //MOV R0, R1
        cpu->regs[(int)reg1] = cpu->regs[(int)reg2];
        break;
        
      case 0x02: //MOV R0, imm
        cpu->regs[(int)reg1] = imm;
        break;
        
      case 0x03: //ADD R0, R1, R2
        cpu->regs[(int)reg1] = cpu->regs[(int)reg2] + cpu->regs[(int)reg3] + imm;
        update_flags(cpu, cpu->regs[(int)reg1]);
        break;
        
      case 0x04: //SUB R0, R1, R2
        cpu->regs[(int)reg1] = cpu->regs[(int)reg2] - cpu->regs[(int)reg3] - imm;
        update_flags(cpu, cpu->regs[(int)reg1]);
        break;

      case 0x05: //AND R0, R1, R2
        cpu->regs[(int)reg1] = cpu->regs[(int)reg2] & cpu->regs[(int)reg3];
        update_flags(cpu, cpu->regs[(int)reg1]);
        break;

      case 0x06: //OR R1, R2, R3
        cpu->regs[(int)reg1] = cpu->regs[(int)reg2] | cpu->regs[(int)reg3];
        update_flags(cpu, cpu->regs[(int)reg1]);
         break;

     case 0x07: //XOR R1, R2, R3
        cpu->regs[(int)reg1] = cpu->regs[(int)reg2] ^ cpu->regs[(int)reg3];
        update_flags(cpu, cpu->regs[(int)reg1]);
         break;

       case 0x08: //NOT R1, R2
        cpu->regs[(int)reg1] = ~cpu->regs[(int)reg2];
        update_flags(cpu, cpu->regs[(int)reg1]);
         break;

       case 0x09: //LD R1, [R2+R3+imm]
         cpu->regs[(int)reg1] = cpu_read8(cpu, cpu->regs[(int)reg2]+cpu->regs[(int)reg3]+imm);
         break;

       case 0x0A: //ST [R1+R2+imm], R3
         cpu_write(cpu, cpu->regs[(int)reg1]+cpu->regs[(int)reg2]+imm, cpu->regs[(int)reg3]);
         break;

       case 0x0B: //JMP imm
         cpu->pc = imm-4;
         break;

       case 0x0C: //JMP r0+r1+imm
         cpu->pc = cpu->regs[(int)reg1]+cpu->regs[(int)reg2]+imm-4;
         break;

       case 0x0D: //JZ imm
         if(cpu->flags & FLAG_ZERO) cpu->pc = imm-4;
         break;

       case 0x0E: //JZ r0+r1+imm
         if(cpu->flags & FLAG_ZERO) cpu->pc = cpu->regs[(int)reg1]+cpu->regs[(int)reg2]+imm-4;
      break;

    case 0x0F: //JNZ imm
      if(!(cpu->flags & FLAG_ZERO)) cpu->pc=imm-4;
      break;

    case 0x10: //JNZ r0+r1+imm
      if(!(cpu->flags & FLAG_ZERO)) cpu->pc = cpu->regs[(int)reg1]+cpu->regs[(int)reg2]+imm-4;
      break;

    case 0x11: //INT imm
      switch(imm) {
        case 0x01: //PUTCHAR SUKA
          putchar(cpu->regs[1]);
          break;
        case 0x02: //GETCHAR SUKA
          cpu->regs[1] = getchar();
      }
      break;

    case 0x12: //HLT
      return 0;

    default:
      printf("Unknown command: 0x%02X at 0x%04X\n", opcode, cpu->pc);
      fflush(stdout);
      return -1;
    }
    cpu->pc += 4;
    return 1;
}

void cpu_dump(risc_gc* cpu) {
  for (int i = 0; i < 48; i++) {
    printf("%02X ", cpu->memory[i]);
  }
  printf("\nPC: 0x%04X, IR: 0x%08X, FLAGS: 0x%04X\n", cpu->pc, cpu->ir, cpu->flags);
  printf("Regs:\n");
  for(int i = 0; i < NUM_REGS; i++) {
    printf("R%d: 0x%04X ", i, cpu->regs[i]);
    if((i+1)%4==0) putchar('\n');
  }
  putchar('\n');
}

int exec(char* filename) {
  struct termios oldt;
  struct termios newt;
  tcgetattr(STDIN_FILENO, &oldt);
  memcpy(&newt, &oldt, sizeof(oldt));
  newt.c_iflag &= ~(IXON);
  newt.c_lflag &= ~(ICANON | ECHO | /*ISIG |*/ IEXTEN);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  risc_gc cpu;
  File* f = sys_open(filename);
  if(!f) return -1;
  cpu.pc = 0;
  cpu.ir = 0;
  cpu.flags = 0;
  for(int i = 0; i < NUM_REGS; i++) {
    cpu.regs[i] = 0;
  }
  cpu.memory = malloc(MEMORY_SIZE);
  memcpy(cpu.memory, f->content, MEMORY_SIZE);
  printf("Executing %s\n", filename);
  execcycle:
    // printf("PC: %03X\n", cpu.pc);
    int status = cpu_step(&cpu);
    // cpu_dump(&cpu);
    // getchar();
    if(status == 0) {
      printf("Halted\n");
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
      return 0;
    } else if(status == -1) {
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
      return -1;
    }
    goto execcycle;
}

