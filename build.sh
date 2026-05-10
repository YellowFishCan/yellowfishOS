#!/bin/bash
set -e

echo "  yellowfishOS Build Script"

SRC_DIR="/mnt/d/.yellowfishOS"

cp "$SRC_DIR"/*.asm .
cp "$SRC_DIR"/*.cpp .
cp "$SRC_DIR"/*.h .
cp "$SRC_DIR"/*.ld .

nasm -f bin boot.asm -o boot.bin

nasm -f elf32 keyboard_wrapper.asm -o keyboard_wrapper.o

echo "Compiling kernel..."
g++ -m32 -nostdlib -fno-builtin -ffreestanding -fno-exceptions -fno-rtti -fno-pic \
    -O2 \
    -c *.cpp

echo "Linking..."
ld -m elf_i386 -T linker.ld --oformat binary kernel.o video.o keyboard_wrapper.o -o kernel.bin

cat boot.bin kernel.bin > os.img

qemu-system-x86_64 -drive file=os.img,format=raw

echo "  Done."