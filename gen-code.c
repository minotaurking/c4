#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define int long long

// opcodes
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
    OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
    OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

void gen_ins(char **code, int op, int rd, int rs1, int rs2) {
    char* cur = *code;
    *cur++ = op;
    *cur++ = rd;
    *cur++ = rs1;
    *cur++ = rs2;
    *code = cur;
}

void gen_mov(char **code, int rd, int rs) {
    gen_ins(code, 0x61, rd, rs, 0);
}

void gen_movi(char **code, int rd, int immd) {
    gen_ins(code, 0x62, rd, immd % 256, immd / 256);
}

void gen_ldr(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0x41, rd, rs1, rs2);
}

void gen_ldri(char **code, int rd, int rs, int immd) {
    gen_ins(code, 0x42, rd, rs, immd);
}

void gen_sti(char **code, int rd, int rs1, int immd) {
    gen_ins(code, 0x52, rd, rs1, immd);
}

void gen_push(char **code, int rd) {
    gen_ins(code, 0xa1, rd, 0, 0);
}

void gen_pop(char **code, int rd) {
    gen_ins(code, 0xa2, rd, 0, 0);
}

void gen_add(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0x1, rd, rs1, rs2);
}

void gen_sub(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0x11, rd, rs1, rs2);
}

void gen_b(char **code, int immd) {
    gen_ins(code, 0x91, immd % 256, (immd & 0xFF00) / 0x100, immd / 0x10000);
}

void gen_nop(char **code) {
    gen_ins(code, 0x0, 0, 0, 0);
}

void gen_slli(char **code, int rd, int rs1, int immd) {
    gen_ins(code, 0x72, rd, rs1, immd);
}

void* read_file(char *filename, int size) {
    void *buffer;
    int fd;
    fd = open(filename, 0);
    if (fd < 0) {
        printf("could not open %s\n", filename);
        return NULL;
    }
    buffer = malloc(size);
    if (buffer == NULL) {
        printf("Could not allocate memory\n");
        close(fd);
        return NULL;
    }
    read(fd, buffer, size);
    close(fd);
    return buffer;
}

int main(int argc, char **argv) {
    char code_file[128];
    char addr_file[128];
    int *code, *base_addr;
    int sp, bp, pc, a, r;
    // target address
    int code_base, data_base;
    char *target_code;
    char *cur_ins;
    int target_entry;
    int code_mem_size;

    if (argc < 2) {
        printf("usage: %s prefix\n", argv[0]);
        return -1;
    }

    strcpy(code_file, argv[1]);
    strcat(code_file, ".code");
    strcpy(addr_file, argv[1]);
    strcat(addr_file, ".addr");

    base_addr = read_file(addr_file, 5*sizeof(int));
    if (base_addr == NULL) {
        return -1;
    }
    code = read_file(code_file, base_addr[2]);
    if (code == NULL) {
        return -1;
    }

    sp = 7;
    bp = 6;
    pc = 8;
    a = 4;
    r = 5;

    // round up to 4k alignment
    code_mem_size = base_addr[2] + 0x1000 - (base_addr[2] & 0xFFF);
    target_code = malloc(code_mem_size);
    cur_ins = target_code;
    code_base = 0;
    data_base = code_mem_size;
    target_entry = 0;

    // Reset handler, jump to 0xc
    gen_b(&cur_ins, 0xc);
    // Vectors, nop now
    gen_nop(&cur_ins);
    gen_nop(&cur_ins);
    // Set sp
    gen_movi(&cur_ins, sp, 0x8000);
    gen_slli(&cur_ins, sp, sp, 4);
    // Jump to entry, but the entry address is unknown, will be updated later
    gen_b(&cur_ins, 0);

    for (int i = 1; i < base_addr[2] / sizeof(int); i++) {
        if (i == base_addr[4]) {
            target_entry = cur_ins - target_code;
        }
        switch (code[i]) {
            case LEA:
                // a = *(bp + *pc);
                gen_ldri(&cur_ins, a, bp, code[++i]);
                break;
            case IMM:
                // a = *pc;
                ++i;
                if (code[i] >= base_addr[1] && code[i] < base_addr[1] + base_addr[3]) {
                    code[i] = code[i] - base_addr[1] + (int)data_base;
                }
                gen_movi(&cur_ins, a, code[i]);
                break;
            case JMP:
                break;
            case JSR:
                break;
            case BZ:
                break;
            case BNZ:
                break;
            case ENT:
                // { *--sp = (int)bp; bp = sp; sp = sp - *pc++; }
                gen_push(&cur_ins, bp);
                gen_mov(&cur_ins, bp, sp);
                gen_movi(&cur_ins, r, code[++i] * 4);
                gen_sub(&cur_ins, sp, sp, r);
                break;
            case ADJ:
                // sp = sp + *pc++
                gen_movi(&cur_ins, r, code[++i] * 4);
                gen_add(&cur_ins, sp, sp, r);
                break;
            case LEV:
                break;
            case LI:
                // a = *(int*)a;
                gen_ldr(&cur_ins, a, a, 0);
                break;
            case LC:
                break;
            case SI:
                gen_pop(&cur_ins, r);
                gen_sti(&cur_ins, a, r, 0);
                break;
            case SC:
                break;
            case PSH:
                // *--sp = a
                gen_push(&cur_ins, a);
                break;
            case OR:
                break;
            case XOR:
                break;
            case AND:
                break;
            case EQ:
                break;
            case NE:
                break;
            case LT:
                break;
            case GT:
                break;
            case LE:
                break;
            case GE:
                break;
            case SHL:
                break;
            case SHR:
                break;
            case ADD:
                // a = *sp++ + a
                gen_pop(&cur_ins, r);
                gen_add(&cur_ins, a, a, r);
                break;
            case SUB:
                break;
            case MUL:
                break;
            case DIV:
                break;
            case MOD:
                break;
            case OPEN:
                break;
            case READ:
                break;
            case CLOS:
                break;
            case PRTF:
                break;
            case MALC:
                break;
            case FREE:
                break;
            case MSET:
                break;
            case MCMP:
                break;
            case EXIT:
                break;
            default:
                printf("Unkown instruction 0x%llx at %lld\n", code[i], i);
                break;
        }
    }

    // End of code, add a deadloop
    gen_b(&cur_ins, 0);

    // Update the jump to entry instruction
    cur_ins = target_code + 0x14;
    gen_b(&cur_ins, target_entry - 0x14);

    // open "code.bin" for write
    int fd;
    fd = open("code.bin", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        printf("could not open code.bin\n");
        return -1;
    }
    write(fd, target_code, code_mem_size);
    close(fd);

    return 0;
}