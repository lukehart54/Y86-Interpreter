/*
 * CS 261 PA2: Mini-ELF loader
 *
 * Name: Luke Hartley
 */

#include "p2-load.h"

/**********************************************************************
 *                         REQUIRED FUNCTIONS
 *********************************************************************/

bool read_phdr(FILE *file, uint16_t offset, elf_phdr_t *phdr)
{
    if (file == NULL)
    {
        return false;
    }
    if (phdr == NULL)
    {
        return false;
    }
    fseek(file, offset, SEEK_SET);

    if (fread(phdr, sizeof(elf_phdr_t), 1, file) != 1)
    {
        return false;
    }
    if (phdr->magic != -559038737)
    {
        return false;
    }
    return true;
}

bool load_segment(FILE *file, byte_t *memory, elf_phdr_t phdr)
{
    if (file == NULL)
    {
        return false;
    }
    if (memory == NULL)
    {
        return false;
    }
    fseek(file, phdr.p_offset, SEEK_SET);
    if (fread(&memory[phdr.p_vaddr], phdr.p_filesz, 1, file) != 1)
    {
        return false;
    }
    return true;
}

/**********************************************************************
 *                         OPTIONAL FUNCTIONS
 *********************************************************************/

void usage_p2(char **argv)
{
    printf("Usage: %s <option(s)> mini-elf-file\n", argv[0]);
    printf(" Options are:\n");
    printf("  -h      Display usage\n");
    printf("  -H      Show the Mini-ELF header\n");
    printf("  -a      Show all with brief memory\n");
    printf("  -f      Show all with full memory\n");
    printf("  -s      Show the program headers\n");
    printf("  -m      Show the memory contents (brief)\n");
    printf("  -M      Show the memory contents (full)\n");
}

bool parse_command_line_p2(int argc, char **argv,
                           bool *print_header, bool *print_segments,
                           bool *print_membrief, bool *print_memfull,
                           char **filename)
{
    if (argv == NULL || print_header == NULL || filename == NULL)
    {
        return false;
    }

    // parameter parsing w/ getopt()
    int c;
    while ((c = getopt(argc, argv, "hHsmMaf")) != -1)
    {
        switch (c)
        {
        case 'h':
            usage_p2(argv);
            return true;
        case 'H':
            *print_header = true;
            break;
        case 's':
            *print_segments = true;
            break;
        case 'm':
            *print_membrief = true;
            break;
        case 'M':
            *print_memfull = true;
            break;
        case 'a':
            *print_header = true;
            *print_segments = true;
            *print_membrief = true;
            break;
        case 'f':
            *print_header = true;
            *print_segments = true;
            *print_memfull = true;
            break;
        default:
            usage_p2(argv);
            return false;
        }
    }

    if (optind != argc - 1)
    {
        // no filename (or extraneous input)
        usage_p2(argv);
        return false;
    }
    *filename = argv[optind]; // save filename

    return true;
}

void dump_phdrs(uint16_t numphdrs, elf_phdr_t phdr[])
{
    printf(" Segment   Offset    VirtAddr  FileSize  Type      Flag\n");
    for (int i = 0; i < numphdrs; i++)
    {
        printf("  %02d       0x%04x    0x%04x    0x%04x   ", i, phdr[i].p_offset, phdr[i].p_vaddr, phdr[i].p_filesz);
        if (phdr[i].p_type == 1)
        {
            printf(" CODE    ");
        }
        else if (phdr[i].p_type == 0)
        {
            printf(" DATA    ");
        }
        else if (phdr[i].p_type == 2)
        {
            printf(" STACK   ");
        }
        else if (phdr[i].p_type == 3)
        {
            printf(" HEAP    ");
        }
        else if (phdr[i].p_type == 5)
        {
            printf(" UNKNOWN    ");
        }

        if (phdr[i].p_flag == 5)
        {
            printf("  R X\n");
        }
        else if (phdr[i].p_flag == 6)
        {
            printf("%s\n", "  RW ");
        }
        else if (phdr[i].p_flag == 4)
        {
            printf("%s\n", "  R  ");
        }
        else if (phdr[i].p_flag == 2)
        {
            printf("%s\n", "   W ");
        }
        else if (phdr[i].p_flag == 3)
        {
            printf("%s\n", "   WX");
        }
        else if (phdr[i].p_flag == 1)
        {
            printf("%s\n", "    X");
        }
        else if (phdr[i].p_flag == 7)
        {
            printf("%s\n", "  RWX");
        }
    }
}

void dump_memory(byte_t *memory, uint16_t start, uint16_t end)
{
    int count = 0;
    int temp_mem = 0;
    printf("%s%04x %s %04x:\n", "Contents of memory from ", start, "to", end);
    for (int i = start; i < 256; i++)
    {
        printf("  0%02x0 ", i);
        while (count < 16)
        {
            if (count == 8)
            {
                printf(" ");
            }
            printf(" %02x", memory[temp_mem]);
            temp_mem++;
            count++;
        }
        count = 0;
        printf("\n");
    }
}
