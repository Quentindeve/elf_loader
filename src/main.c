#include <stdio.h>
#include <fcntl.h>
#include <elf.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "loader.h"
#include <sys/mman.h>
#include <stdlib.h>
#include "utils.h"
#include "shdr_load.h"
#include "phdr_load.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s (-debug) <path to elf>\n ", argv[0]);
        return 1;
    }

    FILE *fd = fopen(argv[1], "r");
    if (fd == NULL)
    {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return 1;
    }

    Elf64_Ehdr ehdr;
    int read = fread(&ehdr, sizeof(ehdr), 1, fd);
    if (read < 0)
    {
        fprintf(stderr, "Error: failed to read ehdr.\n");
        return 1;
    }

    if (!validate_elf(ehdr))
        return 1;

    Elf64_Phdr *phdr_table = phdr_load(ehdr, fd);
    MappedSegment *segments = malloc(sizeof(MappedSegment) * ehdr.e_phnum);
    ASSERT_NOTNULL(segments);

    for (int i = 0; i < ehdr.e_phnum; i++)
    {
        segments[i] = phdr_map(phdr_table[i]);
    }

    Elf64_Shdr *shdr_table = shdr_load(fd, ehdr);
    for (int i = 0; i < ehdr.e_shnum; i++)
    {
        shdr_map(segments, ehdr.e_phnum, shdr_table[i], fd);
    }

    void (*entry_point)(void) = (void (*)(void))(uintptr_t)ehdr.e_entry;
    entry_point();
}

int validate_elf(Elf64_Ehdr ehdr)
{
    if (!ELF_MAGIC_CHECK(ehdr) || ehdr.e_ident[EI_CLASS] != ELFCLASS64)
    {
        fprintf(stderr, "Error: path must be a valid ELF64 file.\n");
        return 0;
    }

    // No entry point
    if (ehdr.e_entry == 0)
    {
        fprintf(stderr, "Error: not an executable file.\n");
        return 0;
    }

    // Bad Arch
    if (ehdr.e_machine != EM_X86_64)
    {
        fprintf(stderr, "Error: bad architecture: %2X.\n", ehdr.e_machine);
        return 0;
    }

    // Bad ABI
    if (ehdr.e_ident[EI_OSABI] != ELFOSABI_LINUX && ehdr.e_ident[EI_OSABI] != ELFOSABI_NONE)
    {
        fprintf(stderr, "Error: weird ABI detected: %2X.\n", ehdr.e_ident[EI_OSABI]);
        return 0;
    }

    // No program header
    if (ehdr.e_phoff == 0)
    {
        fprintf(stderr, "Error: no program header.\n");
        return 0;
    }

    // Too much Section Header
    if (ehdr.e_shnum == SHN_UNDEF)
    {
        fprintf(stderr, "Error: e_shnum >= SHN_LORESERVE is not supported yet.\n");
        return 0;
    }

    return 1;
}