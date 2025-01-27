#pragma once
#include <elf.h>
#include <stdio.h>
#include "phdr_load.h"

// Reads section header
Elf64_Shdr *shdr_load(FILE *, Elf64_Ehdr);

// Maps a section into memory, and copies section content. Returns 1 on success, 0 otherwise.
int shdr_map(MappedSegment *, int, Elf64_Shdr, FILE *);