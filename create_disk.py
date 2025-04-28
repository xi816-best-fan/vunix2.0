import struct 
import random

MAGIC = 0xDEADBABE
FILE_START = 0xFE
FILE_META = 0xFD
DIR_START = 0xFC
DIR_META = 0xFB
DISK_END = 0xFF

def write_string(f, s):
    f.write(s.encode('utf-8')+b'\x00')

def write_file(f, name, path, content):
    f.write(bytes([FILE_START]))
    write_string(f, name)
    f.write(bytes([FILE_META]))
    write_string(f, path)
    f.write(bytes([FILE_META]))
    f.write(content.encode('utf-8'))
    f.write(bytes([FILE_START]))

def write_dir(f, name, path):
    f.write(bytes([DIR_START]))
    write_string(f, name)
    f.write(bytes([DIR_META]))
    write_string(f, path)
    f.write(bytes([DIR_START]))

with open("disk.img", "wb") as f:
    f.write(struct.pack("<IQ", MAGIC, random.getrandbits(64)))
    write_file(f, "hello.txt", "/hello", "Hello world!")
    write_dir(f, "hello", "/hello")
    f.write(bytes([DISK_END]))
