/*
 * CS 261 PA1: Mini-ELF header verifier
 *
 * Name: Luke Hartley
 */

#include "p1-check.h"

/**********************************************************************
 *                         REQUIRED FUNCTIONS
 *********************************************************************/

bool read_header(FILE *file, elf_hdr_t *hdr)
{
    bool was_read = false;
    fread(hdr, sizeof(elf_hdr_t), 1, file);
    return was_read;
}

/**********************************************************************
 *                         OPTIONAL FUNCTIONS
 *********************************************************************/

void usage_p1(char **argv)
{
    printf("Usage: %s <option(s)> mini-elf-file\n", argv[0]);
    printf(" Options are:\n");
    printf("  -h      Display usage\n");
    printf("  -H      Show the Mini-ELF header\n");
}

bool parse_command_line_p1(int argc, char **argv, bool *print_header, char **filename)
{
    return true;
}

void dump_header(elf_hdr_t hdr)
{
    printf("%02x %02x ", hdr.e_version & 0xFF, (hdr.e_version >> 8) & 0xFF);
    printf("%02x %02x ", hdr.e_entry & 0xFF, (hdr.e_entry >> 8) & 0xFF);
    printf("%02x %02x ", hdr.e_phdr_start & 0xFF, (hdr.e_phdr_start >> 8) & 0xFF);
    printf("%02x %02x  ", hdr.e_num_phdr & 0xFF, (hdr.e_num_phdr >> 8) & 0xFF);
    printf("%02x %02x ", hdr.e_symtab & 0xFF, (hdr.e_symtab >> 8) & 0xFF);
    printf("%02x %02x ", hdr.e_strtab & 0xFF, (hdr.e_strtab >> 8) & 0xFF);
    printf("%02x %02x %02x %02x%s", hdr.magic & 0xFF, (hdr.magic >> 8) & 0xFF, (hdr.magic >> 16) & 0xFF, (hdr.magic >> 24) & 0xFF, "\n");
    printf("%s %i\n", "Mini-ELF version", hdr.e_version);
    printf("%s%x%s", "Entry point 0x",hdr.e_entry, "\n");
    printf("%s%d %s %i (0x%x)\n", "There are ", hdr.e_phdr_start >> 3, "program headers, starting at offset", hdr.e_phdr_start,hdr.e_phdr_start );
    printf("%s %d (0x%x)\n", "There is a symbol table starting at offset", hdr.e_symtab, hdr.e_symtab);
    printf("%s %d (0x%x)\n", "There is a string table starting at offset", hdr.e_strtab, hdr.e_strtab);
}
