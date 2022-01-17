/*
 * CS 261 PA4: Mini-ELF interpreter
 *
 * Name: Luke Hartley
 */
#include "p4-interp.h"

/**********************************************************************
 *                         REQUIRED FUNCTIONS
 *********************************************************************/

y86_reg_t decode_execute(y86_t *cpu, y86_inst_t inst, bool *cnd, y86_reg_t *valA)
{
    y86_reg_t valE = 0;
    switch(inst.icode) {
        case HALT:
            cpu->stat = HLT;
            break;
        case NOP:
            cpu->stat = AOK;
            if (valA == NULL)
            {
                cpu->stat = INS;
            }
            break;
        case IRMOVQ:
            valE = inst.valC.v;
            break;
        case OPQ:
            switch(inst.ifun.b){
                case ADD:
                    valE = cpu->reg[inst.ra] + cpu->reg[inst.rb];
                    break;
                case SUB:
                    valE = cpu->reg[inst.rb] - cpu->reg[inst.ra];
                    break;
                case AND:
                    valE = cpu->reg[inst.ra] & cpu->reg[inst.rb];
                    break;
                case XOR:
                    valE = cpu->reg[inst.ra] ^ cpu->reg[inst.rb];
                    break;
                default:
                    cpu->stat = INS;
            }
            break;
        case RMMOVQ:
            valE = inst.valC.d + cpu->reg[inst.rb];
            *valA = cpu->reg[inst.ra];
            break;
        case MRMOVQ:
            valE = inst.valC.d + cpu->reg[inst.rb];
            break;
        default:
            cpu->stat = INS;
            break;
    }
    return valE;
}

void memory_wb_pc(y86_t *cpu, y86_inst_t inst, byte_t *memory,
                  bool cnd, y86_reg_t valA, y86_reg_t valE)
{
    switch (inst.icode)
    {
    case HALT:
        cpu->pc = inst.valP;
        break;
    case NOP:
        cpu->pc = inst.valP;
        break;
    case IRMOVQ:
        cpu->reg[inst.rb] = valE;
        cpu->pc = inst.valP;
        break;
    case OPQ:
        cpu->reg[inst.rb] = valE;
        cpu->pc = inst.valP;
        break;
    default:
        cpu->pc = inst.valP;
        break;
    }
}

/**********************************************************************
 *                         OPTIONAL FUNCTIONS
 *********************************************************************/

void usage_p4(char **argv)
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
    printf("  -e      Execute program\n");
    printf("  -E      Execute program (trace mode)\n");
}

bool parse_command_line_p4(int argc, char **argv,
                           bool *header, bool *segments, bool *membrief, bool *memfull,
                           bool *disas_code, bool *disas_data,
                           bool *exec_normal, bool *exec_trace, char **filename)
{
    if (argv == NULL || header == NULL || filename == NULL)
    {   
        return false;
    }

    // parameter parsing w/ getopt()
    int c;
    while ((c = getopt(argc, argv, "hHsmMafdDeE")) != -1)
    {
        switch (c)
        {
        case 'h':
            usage_p4(argv);
            return true;
        case 'H':
            *header = true;
            break;
        case 's':
            *segments = true;
            break;
        case 'm':
            *membrief = true;
            break;
        case 'M':
            *memfull = true;
            break;
        case 'a':
            *header = true;
            *segments = true;
            *membrief = true;
            break;
        case 'f':
            *header = true;
            *segments = true;
            *memfull = true;
            break;
        case 'd':
            *disas_code = true;
            break;
        case 'D':
            *disas_data = true;
            break;
        case 'e':
            *exec_normal = true;
            break;
        case 'E':
            *exec_trace = true;
            break;
        default:
            usage_p4(argv);
            return false;
        }
    }
    if (optind != argc - 1)
    {
        // no filename (or extraneous input)
        usage_p4(argv);
        return false;
    }
    *filename = argv[optind]; // save filename

    return true;
}

void get_stat(y86_stat_t stat) {
    switch(stat){
        case AOK:
            printf("AOK");
            break;
        case HLT:
            printf("HLT");
            break;
        case ADR:
            printf("ADR");
            break;
        default:
            printf("INS");
            break;
    }
}

    void dump_cpu_state(y86_t cpu)
{
    //cpu.reg[RIP] = cpu.pc;
    printf("Y86 CPU state:");
    printf("\n  %s %016lx   %s%u %s%u %s%u     ", "%rip:", cpu.pc, "flags: Z", cpu.zf, "S", cpu.sf, "O", cpu.sf);
    get_stat(cpu.stat);
    printf("\n  %s %016lx    %s %016lx", "%rax:", cpu.reg[RAX], "%rcx:", cpu.reg[RCX]);
    printf("\n  %s %016lx    %s %016lx", "%rdx:", cpu.reg[RDX], "%rbx:", cpu.reg[RBX]);
    printf("\n  %s %016lx    %s %016lx", "%rsp:", cpu.reg[RSP], "%rbp:", cpu.reg[RBP]);
    printf("\n  %s %016lx    %s %016lx", "%rsi:", cpu.reg[RSI], "%rdi:", cpu.reg[RDI]);
    printf("\n   %s %016lx     %s %016lx", "%r8:", cpu.reg[R8], "%r9:", cpu.reg[R9]);
    printf("\n  %s %016lx    %s %016lx", "%r10:", cpu.reg[R10], "%r11:", cpu.reg[R11]);
    printf("\n  %s %016lx    %s %016lx", "%r12:", cpu.reg[R12], "%r13:", cpu.reg[R13]);
    printf("\n  %s %016lx\n", "%r14:", cpu.reg[R14]);
}
