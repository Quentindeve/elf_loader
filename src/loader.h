#pragma once
#include <elf.h>
#include <stdio.h>

#define ELF_MAGIC_CHECK(header) (header.e_ident[EI_MAG0] == ELFMAG0 && header.e_ident[EI_MAG1] == ELFMAG1 && header.e_ident[EI_MAG2] == ELFMAG2 && header.e_ident[EI_MAG3] == ELFMAG3)
#define ELF_READ(fd, var) fread(&var, sizeof(char), sizeof(var), fd)

// Checks that the provided buffer is a valid ELF64 and executable on the current machine
int validate_elf(Elf64_Ehdr);