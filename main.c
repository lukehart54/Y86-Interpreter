/*
 * CS 261: Main driver
 *
 * Name: Luke Hartley
 */

#include "p1-check.h"
#include "p2-load.h"
#include "p3-disas.h"
#include "p4-interp.h"

void opq_helper(y86_op_t c) {
    if (c == ADD) {
        printf("addq");
    } else if(c == SUB) {
        printf("subq");
    } else if(c == AND) {
        printf("andq");
    } else if(c == XOR) {
        printf("xorq");
    } else if(c == BADOP) {
        printf("badop");
    }
}

void reg_helper(y86_regnum_t reg)
{
    if (reg == RAX)
    {
        printf(" %%rax");
    }
    else if (reg == RCX)
    {
        printf(" %%rcx");
    }
    else if (reg == RDX)
    {
        printf(" %%rdx");
    }
    else if (reg == RBX)
    {
        printf(" %%rbx");
    }
    else if (reg == RSP)
    {
        printf(" %%rsp");
    }
    else if (reg == RBP)
    {
        printf(" %%rbp");
    }
    else if (reg == RSI)
    {
        printf(" %%rsi");
    }
    else if (reg == RDI)
    {
        printf(" %%rdi");
    }
    else if (reg == R8)
    {
        printf(" %%r8");
    }
    else if (reg == R9)
    {
        printf(" %%r9");
    }
    else if (reg == R10)
    {
        printf(" %%r10");
    }
    else if (reg == R11)
    {
        printf(" %%r11");
    }
    else if (reg == R12)
    {
        printf(" %%r12");
    }
    else if (reg == R13)
    {
        printf(" %%r13");
    }
    else if (reg == R14)
    {
        printf(" %%r14");
    }
    else if (reg == NOREG)
    {
        printf(" %%noreg");
    }
}

int main(int argc, char **argv)
{
    bool print_header = false;
    bool print_segments = false;
    bool print_membrief = false;
    bool print_memfull = false;
    bool disas_code = false;
    bool disas_data = false;
    bool exec_normal = false;
    bool exec_trace = false;
    char *filename = NULL;
    
    if (!parse_command_line_p4(argc, argv, &print_header, &print_segments,
                               &print_membrief, &print_memfull, &disas_code,
                               &disas_data, &exec_normal, &exec_trace, &filename))
    {
        exit(EXIT_FAILURE);
    }
    if (filename != NULL)
    {
        FILE *f = fopen(filename, "r");
        // open Mini-ELF binary
        if (!f)
        {
            printf("Failed to read file\n");
            exit(EXIT_FAILURE);
        }

        // P1: load and check Mini-ELF header
        elf_hdr_t hdr;
        read_header(f, &hdr);

        // P2
        elf_phdr_t phdr[hdr.e_num_phdr];
        byte_t *mem = (byte_t *)calloc(MEMSIZE, 1);
        for (int i = 0; i < hdr.e_num_phdr; i++)
        {
            read_phdr(f, 16 + (20 * i), &phdr[i]);
            load_segment(f, mem, phdr[i]);
        }
        if (print_segments)
        {
            dump_phdrs(hdr.e_num_phdr, phdr);
        }

        if (print_memfull)
        {
            dump_memory(mem, 0, MEMSIZE);
        }

        if (print_membrief)
        {
            dump_memory(mem, phdr->p_vaddr, 0x0117);
        }

        y86_t cpu;
        bool cnd;
        y86_reg_t valA, valE;

        // initialize cpu
        cpu.stat = AOK;
        cpu.pc = hdr.e_entry;
        for ( int i = 0; i < 15; i++ ) {
            cpu.reg[i] = 0;
        }
        cpu.zf = 0;
        cpu.sf = 0;
        cpu.of = 0;

        


        if (exec_normal)
        {
            int count = 0;
            while (cpu.stat == AOK)
            {
                y86_inst_t ins = fetch(&cpu, mem);
                if (cpu.pc == hdr.e_entry) {
                    printf("%s 0x%04x\n", "Beginning execution at", hdr.e_entry);
                }
                // P4
                valE = decode_execute(&cpu, ins, &cnd, &valA);
                memory_wb_pc(&cpu, ins, mem, cnd, valA, valE);
                count++;
            }
            dump_cpu_state(cpu);
            printf("%s%i\n","Total execution count: ", count);
        }

        if (exec_trace)
        {
            int count = 0;
            if (cpu.pc == hdr.e_entry) {
                printf("%s 0x%04x\n", "Beginning execution at", hdr.e_entry);
            } 
            dump_cpu_state(cpu);
            while (cpu.stat == AOK)
            {
                y86_inst_t ins = fetch(&cpu, mem);
                // P4
                valE = decode_execute(&cpu, ins, &cnd, &valA);
                memory_wb_pc(&cpu, ins, mem, cnd, valA, valE);
                if (cpu.pc == hdr.e_entry - 1) {
                } else {
                    printf("\n%s","Executing: ");
                    switch(ins.icode) {
                        case HALT:
                            printf("halt\n");
                            break;
                        case NOP:
                            printf("nop\n");
                            break;
                        case IRMOVQ:
                            printf("%s 0x%lx,", "irmovq", cpu.reg[ins.rb]);
                            reg_helper(ins.rb);
                            printf("\n");
                            break;
                        case OPQ:
                            opq_helper(ins.ifun.op);
                            reg_helper(ins.ra);
                            printf(",");
                            reg_helper(ins.rb);
                            printf("\n");
                            break;
                        default: 
                            printf("nop\n");
                            break;
                    }
                }
                dump_cpu_state(cpu);
                count++;
            }
            printf("%s%i\n\n","Total execution count: ", count);
            dump_memory(mem, 0, MEMSIZE);
        }

        // cleanup
        free(mem);
        fclose(f);
    }
}
