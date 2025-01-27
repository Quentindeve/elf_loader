#include <elf.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include <stdlib.h>
#include "phdr_load.h"

Elf64_Shdr *shdr_load(FILE *fd, Elf64_Ehdr ehdr)
{
    Elf64_Shdr *shdr_table = malloc(sizeof(Elf64_Shdr) * ehdr.e_shnum);
    ASSERT_NOTNULL(shdr_table);

    fseek(fd, ehdr.e_shoff, SEEK_SET);
    fread(shdr_table, sizeof(Elf64_Shdr), ehdr.e_shnum, fd);
    return shdr_table;
}

int shdr_map(MappedSegment *segments, int seg_count, Elf64_Shdr shdr, FILE *fd)
{
    // This section mustn't be mapped
    if (!(shdr.sh_flags & SHF_ALLOC))
        return 1;

    MappedSegment *segment = NULL;

    for (int i = 0; i < seg_count; i++)
    {
        MappedSegment *current = &segments[i];

        if (current->base == NULL)
            continue;

        if ((Elf64_Addr)(current->base) <= shdr.sh_addr && (Elf64_Addr)(current->base) + current->phdr.p_memsz >= shdr.sh_addr)
        {
            segment = current;
            break;
        }
    }

    if (segment == NULL)
    {
        fprintf(stderr, "Error: section %2lx maps to a non-existent segment.\n", shdr.sh_addr);
        exit(1);
    }

    void *addr = segment->base;
    fseek(fd, shdr.sh_offset, SEEK_SET);
    int read = fread(addr, 1, shdr.sh_size, fd);

    if ((Elf64_Xword)read != shdr.sh_size)
    {
        return 0;
    }

    // If region isn't originally writable
    if (!(shdr.sh_flags & SHF_WRITE))
    {
        int result = mprotect(addr, segment->phdr.p_memsz, phdr_map_prot(segment->phdr));
        if (result < 0)
        {
            fprintf(stderr, "Error: failed to remove write permission at %2lx", (uintptr_t)addr);
            exit(1);
        }
    }
    return 1;
}