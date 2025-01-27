#pragma once
#include <elf.h>
#include <stdio.h>

Elf64_Phdr *phdr_load(Elf64_Ehdr, FILE *);

typedef struct
{
    Elf64_Phdr phdr;
    void *base;
} MappedSegment;

MappedSegment phdr_map(Elf64_Phdr);

int phdr_map_prot(Elf64_Phdr);