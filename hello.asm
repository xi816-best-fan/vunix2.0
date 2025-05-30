xui:
  mov r2, hello
loop:
  ld r1, [r2]
  add r2, r2, r0, 1
  xor r1, r1, r0
  jz end
  int 1
  jmp loop
end:
  hlt

hello: bytes "Hello, Пидорасы!$^@"

