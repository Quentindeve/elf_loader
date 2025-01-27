#include "phdr_load.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include "utils.h"

Elf64_Phdr *phdr_load(Elf64_Ehdr ehdr, FILE *fd)
{
    Elf64_Phdr *phdr_table = (Elf64_Phdr *)malloc(ehdr.e_phnum * sizeof(Elf64_Phdr));
    ASSERT_NOTNULL(phdr_table);

    fseek(fd, ehdr.e_phoff, SEEK_SET);
    int read = fread(phdr_table, sizeof(Elf64_Phdr), ehdr.e_phnum, fd);

    if (read < 0)
    {
        fprintf(stderr, "Error: failed to read phdr.\n");
        return NULL;
    }

    return phdr_table;
}

MappedSegment phdr_map(Elf64_Phdr phdr)
{
    if ((phdr.p_type & PT_LOAD) != PT_LOAD)
    {
        fprintf(stderr, "Error: not supported phdr\n");
        exit(1);
    }

    // We must init it with WRITE access to fread to it later
    int mmap_prot = PROT_WRITE | phdr_map_prot(phdr);

    void *mapped = mmap((void *)phdr.p_vaddr, phdr.p_filesz, mmap_prot, MAP_ANONYMOUS | MAP_SHARED | MAP_FIXED, 0, 0);
    if (mapped == MAP_FAILED)
    {
        fprintf(stderr, "Error: failed to map segment %2lx-%2lx: %s\n", phdr.p_vaddr, phdr.p_vaddr + phdr.p_memsz, strerror(errno));
        exit(1);
    }

    return (MappedSegment){
        .phdr = phdr,
        .base = mapped};
}

int phdr_map_prot(Elf64_Phdr phdr)
{
    int mmap_prot = 0;
    if (phdr.p_flags & PF_R)
        mmap_prot |= PROT_READ;
    if (phdr.p_flags & PF_X)
        mmap_prot |= PROT_EXEC;

    return mmap_prot;
}