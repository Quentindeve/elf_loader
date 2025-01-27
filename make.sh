#!/bin/env sh
gcc -Wall -Wextra -Werror -g -o loader src/*.c

nasm -f elf64 -o payload.o payload.asm
ld -o payload payload.o
