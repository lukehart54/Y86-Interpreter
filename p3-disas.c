/*
 * CS 261 PA3: Mini-ELF disassembler
 *
 * Name: Luke Hartley
 */

#include "p3-disas.h"



/**********************************************************************
 *                         REQUIRED FUNCTIONS
 *********************************************************************/

y86_inst_t fetch(y86_t *cpu, byte_t *memory)
{
    y86_inst_t ins;

    if (memory == NULL)
    {
        exit(EXIT_FAILURE);
    }

    ins.valP = cpu->pc;
    ins.icode = memory[ins.valP] >> 4;
    ins.ifun.b = memory[ins.valP] & 0xF;

    switch (ins.icode)
    {
    case HALT:
    case NOP:
    case RET:
        if(ins.ifun.b != 0) {
            ins.icode = INVALID;
            cpu->stat = INS;
        }
        ins.valP += 1;
        break;
    case OPQ:
        ins.ifun.op = ins.ifun.b;
        if (ins.ifun.op >= BADOP || ins.ifun.op < 0)
        {
            ins.icode = INVALID;
            cpu->stat = INS;
        }
    case CMOV:
        if (ins.icode == CMOV){
            ins.ifun.cmov = ins.ifun.b;
        }
        ins.ra = memory[ins.valP + 1] >> 4;
        ins.rb = memory[ins.valP + 1] & 0xF;
        if (ins.ifun.cmov >= BADCMOV || ins.ifun.cmov < 0)
        {
            ins.icode = INVALID;
            cpu->stat = INS;
        }
        ins.valP += 2;
        break;
    case PUSHQ:
        ins.ra = memory[ins.valP + 1] >> 4;
        ins.rb = memory[ins.valP + 1] & 0xF;
                if (ins.ra > 14 || ins.ra < 0 || ins.rb != 15
         || ins.ifun.b != 0)
        {
            ins.icode = INVALID;
            cpu->stat = INS;
        }
        ins.valP += 2;
        break;
    case POPQ:
        ins.ra = memory[ins.valP + 1] >> 4;
        ins.rb = memory[ins.valP + 1] & 0xF;

        if (ins.ra > 14 || ins.ra < 0 || ins.rb != 15
         || ins.ifun.b != 0)
        {
            ins.icode = INVALID;
            cpu->stat = INS;
        }
        ins.valP += 2;
        break;
    case RMMOVQ:
    case MRMOVQ:
        ins.ra = memory[ins.valP + 1] >> 4;
        ins.rb = memory[ins.valP + 1] & 0xF;
        ins.valP += 10;
        ins.valC.d = *(uint64_t*) (&memory[cpu->pc + 2]);
        if (ins.rb == NOREG || ins.ra == NOREG)
        {
            ins.icode = INVALID;
            cpu->stat = ADR;
        } else if(ins.ifun.b != 0) {
            ins.icode = INVALID;
            cpu->stat = INS;
        }
        break;
    case IRMOVQ:
        if (ins.ifun.b != 0) {
            ins.icode = INVALID;
            cpu->stat = INS;
        }
        ins.ra = memory[ins.valP + 1] >> 4;
        ins.rb = memory[ins.valP + 1] & 0xF;
        ins.valC.v = *(uint64_t*) (&memory[cpu->pc + 2]); 
        ins.valP += 10;
        if (ins.ra != 15){
            ins.icode = INVALID;
            cpu->stat = INS;
        }
        break;
    case CALL:
        ins.valC.dest = *(uint64_t*) (&memory[cpu->pc + 1]);
        ins.valP += 9;
        if(ins.ifun.b != 0) {
            ins.icode = INVALID;
            cpu->stat = INS;
        }
        break;
    case JUMP:
        ins.valC.dest = *(uint64_t*) (&memory[cpu->pc + 1]);
        ins.valP += 9;
        ins.ifun.jump = ins.ifun.b;
        if (ins.ifun.jump >= BADJUMP || ins.ifun.cmov < 0)
        {
            ins.icode = INVALID;
            cpu->stat = INS;
        }
        break;
    case IOTRAP:
        if (ins.icode != IOTRAP || ins.ifun.b < 0 || ins.ifun.b > 5) {
            ins.icode = INVALID;
            cpu->stat = INS;
        }
        ins.valP += 1;
        break;
    default:
        ins.valP += 10;
    }

    return ins;
}

/**********************************************************************
 *                         OPTIONAL FUNCTIONS
 *********************************************************************/

void usage_p3(char **argv)
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
    printf("  -d      Disassemble code contents\n");
    printf("  -D      Disassemble data contents\n");
}

bool parse_command_line_p3(int argc, char **argv,
                           bool *print_header, bool *print_segments,
                           bool *print_membrief, bool *print_memfull,
                           bool *disas_code, bool *disas_data, char **filename)
{

    if (argv == NULL || print_header == NULL || filename == NULL)
    {
        return false;
    }

    // parameter parsing w/ getopt()
    int c;
    while ((c = getopt(argc, argv, "hHsmMafdD")) != -1)
    {
        switch (c)
        {
        case 'h':
            usage_p3(argv);
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
        case 'd':
            *disas_code = true;
            break;
        case 'D':
            *disas_data = true;
            break;
        default:
            usage_p3(argv);
            return false;
        }
    }

    if (optind != argc - 1)
    {
        // no filename (or extraneous input)
        usage_p3(argv);
        return false;
    }
    *filename = argv[optind]; // save filename

    return true;
}

void cmov_helper(y86_cmov_t c) {
    if (c == RRMOVQ) {
        printf("rrmovq");
    } else if(c == CMOVLE) {
        printf("cmovle");
    } else if(c == CMOVL) {
        printf("cmovl");
    } else if(c == CMOVE) {
        printf("cmove");
    } else if(c == CMOVNE) {
        printf("cmovne");
    } else if(c == CMOVGE) {
        printf("cmovge");
    } else if(c == CMOVG) {
        printf("cmovg");
    } else if(c == BADCMOV) {
        printf("badcmov");
    }
}

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

void reg_helper(y86_regnum_t reg) {
    if(reg == RAX) {
        printf(" %%rax");
    } else if (reg == RCX) {
        printf(" %%rcx");
    } else if (reg == RDX) {
        printf(" %%rdx");
    } else if (reg == RBX) {
        printf(" %%rbx");
    } else if (reg == RSP) {
        printf(" %%rsp");
    } else if (reg == RBP) {
        printf(" %%rbp");
    } else if (reg == RSI) {
        printf(" %%rsi");
    } else if (reg == RDI) {
        printf(" %%rdi");
    } else if (reg == R8) {
        printf(" %%r8");
    } else if (reg == R9) {
        printf(" %%r9");
    } else if (reg == R10) {
        printf(" %%r10");
    } else if (reg == R11) {
        printf(" %%r11");
    } else if (reg == R12) {
        printf(" %%r12");
    }  else if (reg == R13) {
        printf(" %%r13");
    }  else if (reg == R14) {
        printf(" %%r14");
    }  else if (reg == NOREG) {
        printf(" %%noreg");
    }
}

// void imov_helper(y86_cmov_t mov) {
//         if (mov == RRMOVQ) {
//         printf("rrmovq");
//     } else if(mov == SUB) {
//         printf("subq");
//     } else if(mov == AND) {
//         printf("andq");
//     } else if(mov == XOR) {
//         printf("xorq");
//     } else if(mov == BADOP) {
//         printf("badop");
//     }
// }

void jmp_helper(y86_jump_t jump) {
    if (jump == JMP) {
        printf("jmp");
    } else if (jump == JLE) {
        printf("jle");
    } else if (jump == JL) {
        printf("jl");
    } else if (jump == JE) {
        printf("je");
    } else if (jump == JNE) {
        printf("jne");
    } else if (jump == JGE) {
        printf("jge");
    } else if (jump == JG) {
        printf("jg");
    }
}

void io_helper(y86_iotrap_t io) {
    if (io == CHAROUT) {
        printf("iotrap 0");
    } else if(io == CHARIN) {
        printf("iotrap 1");
    } else if(io == DECOUT) {
        printf("iotrap 2");
    } else if(io == DECIN) {
        printf("iotrap 3");
    } else if(io == STROUT) {
        printf("iotrap 4");
    } else if(io == FLUSH) {
        printf("iotrap 5");
    }
}


void disassemble(y86_inst_t inst)
{
    switch (inst.icode)
    {
    case HALT:
        printf("halt");
        break;
    case NOP:
        printf("nop");
        break;
    case RET:
        printf("ret");
        break;
    case CMOV:
        cmov_helper(inst.ifun.cmov);
        reg_helper(inst.ra);
        printf(",");
        reg_helper(inst.rb);
        break;
    case OPQ:
        opq_helper(inst.ifun.op);
        reg_helper(inst.ra);
        printf(",");
        reg_helper(inst.rb);
        break;
    case PUSHQ:
        printf("pushq");
        reg_helper(inst.ra);
        break;
    case POPQ:
        printf("popq");
        reg_helper(inst.ra);
        break;
    case IRMOVQ:
        printf("irmovq");
        printf(" 0x%lx,", inst.valC.v);
        reg_helper(inst.rb);
        break;
    case RMMOVQ:
        printf("rmmovq");
        reg_helper(inst.rb);
        printf(", 0x%lx", inst.valC.v);
        break;
    case MRMOVQ:
        printf("mrmovq");
        printf(" 0x%lx,", inst.valC.v);
        reg_helper(inst.rb);
        break;
    case JUMP:
        jmp_helper(inst.ifun.jump);
        reg_helper(inst.rb);
        printf(" 0x%lx", inst.valC.v);
        break;
    case CALL:
        printf("call ");
        printf("0x%lx", inst.valC.v);
        break;
    case IOTRAP:
        io_helper(inst.ifun.trap);
        break;
    case INVALID:
        break;
    }
    
}


void disassemble_code(byte_t *memory, elf_phdr_t *phdr, elf_hdr_t *hdr)
{
    y86_t cpu;      // CPU struct to store "fake" PC
    y86_inst_t ins; // struct to hold fetched instruction

    // start at beginning of the segment
    cpu.pc = phdr->p_vaddr;

    // iterate through the segment one instruction at a time
    printf("Disassembly of executable contents:\n");
    printf("  0x%03lx: %-21s| %s\n", cpu.pc,"", ".pos 0x100 code");
    while (cpu.pc < phdr->p_vaddr + phdr->p_filesz)
    {

        ins = fetch(&cpu, memory); // stage 1: fetch instruction

        // TODO: abort with error if instruction was invalid
        if(cpu.pc == hdr->e_entry){
            printf("  0x%03lx: %-21s| %s\n", cpu.pc, "", "_start:");
        }
        // TODO: print current address and raw bytes of instruction
        if(ins.icode == HALT || ins.icode == NOP || ins.icode == RET){
            printf("  0x%03lx: %-21.02x|   ", cpu.pc, memory[cpu.pc]);
        } else if(ins.icode == CMOV || ins.icode == OPQ || ins.icode == PUSHQ || ins.icode == POPQ) {
            printf("  0x%03lx: %02x%-19.02x|   ", cpu.pc, memory[cpu.pc], memory[cpu.pc + 1] );
        } else if (ins.icode == IRMOVQ || ins.icode == RMMOVQ || ins.icode == MRMOVQ ) {
            printf("  0x%03lx: %02x%02x%-17.016lx|   ", cpu.pc, memory[cpu.pc], memory[cpu.pc + 1] , ins.valC.v );
        } else if(ins.icode == IOTRAP) {
            printf("  0x%03lx: %-21.02x|   ", cpu.pc, memory[cpu.pc]);
        } else if(ins.icode == CALL || ins.icode == JUMP) {
            printf("  0x%03lx: %02x%-19.016lx|   ", cpu.pc, memory[cpu.pc] , ins.valC.v );
        }
        disassemble(ins);  // stage 2: print disassembly
        printf("\n");
        cpu.pc = ins.valP; // stage 3: update PC (go to next instruction)
    }
    printf("\n");
}

void disassemble_data(byte_t *memory, elf_phdr_t *phdr)
{
}

void disassemble_rodata(byte_t *memory, elf_phdr_t *phdr)
{
}
