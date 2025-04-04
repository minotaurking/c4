#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

// opcodes
enum { LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
    OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
    OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

int sp, bp, pc, a, r, t;

// Target data area
int64_t *data;
// Target data base
int data_base;
int cur_data_index;

void gen_ins(char **code, int op, int rd, int rs1, int rs2) {
    char* cur = *code;
    *cur++ = op;
    *cur++ = rd;
    *cur++ = rs1;
    *cur++ = rs2;
    *code = cur;
}

void gen_slli(char **code, int rd, int rs1, int immd) {
    gen_ins(code, 0x72, rd, rs1, immd);
}

void gen_add(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0x1, rd, rs1, rs2);
}

void gen_addi(char **code, int rd, int rs, int immd) {
    gen_ins(code, 0x2, rd, rs, immd);
}

void gen_sub(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0x11, rd, rs1, rs2);
}

void gen_mul(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0x21, rd, rs1, rs2);
}

void gen_div(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0x31, rd, rs1, rs2);
}

void gen_ld32(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0x41, rd, rs1, rs2);
}

void gen_ld32i(char **code, int rd, int rs, int immd) {
    gen_ins(code, 0x42, rd, rs, immd);
}

void gen_mov(char **code, int rd, int rs) {
    gen_ins(code, 0x61, rd, rs, 0);
}

void gen_movi(char **code, int rd, int immd) {
    int limit = 32768;
    int cur_data_addr = 0;
    if (immd >= limit || immd <= -limit) {
        data[cur_data_index] = immd;
        cur_data_addr = data_base + cur_data_index * sizeof(int64_t);
        cur_data_index++;
        gen_movi(code, rd, cur_data_addr / limit);
        gen_slli(code, rd, rd, 15);
        gen_movi(code, t, cur_data_addr % limit);
        gen_ld32(code, rd, rd, t);
    } else if (immd < 0) {
        // For example, immd = -257(0xFEFF in 16 bit)
        // As mod rule in c language
        // -257 / 256 = -1 (0xFF in 8 bit)
        // -257 % 256 = -1 (0xFF in 8 bit)
        // So in binary of MOVI instruction, we got a immd 0xFFFF, which is -1 not -257
        // We should minus 1 to -257 / 256, that is what we want
        gen_ins(code, 0x62, rd, immd % 256, immd / 256 - 1);
    } else {
        gen_ins(code, 0x62, rd, immd % 256, immd / 256);
    }
}

void gen_ld8i(char **code, int rd, int rs, int immd) {
    gen_ins(code, 0x44, rd, rs, immd);
}

void gen_st32i(char **code, int rd, int rs1, int immd) {
    gen_ins(code, 0x52, rd, rs1, immd);
}

void gen_st8i(char **code, int rd, int rs1, int immd) {
    gen_ins(code, 0x54, rd, rs1, immd);
}

void gen_push(char **code, int rd) {
    gen_ins(code, 0xa1, rd, 0, 0);
}

void gen_pop(char **code, int rd) {
    gen_ins(code, 0xa2, rd, 0, 0);
}

void gen_and(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0xc1, rd, rs1, rs2);
}

void gen_eq(char **code, int rd, int rs) {
    gen_ins(code, 0xb1, rd, rs, 0);
}

void gen_ne(char **code, int rd, int rs) {
    gen_ins(code, 0xb3, rd, rs, 0);
}

void gen_lt(char **code, int rd, int rs) {
    gen_ins(code, 0xb7, rd, rs, 0);
}

void gen_gt(char **code, int rd, int rs) {
    gen_ins(code, 0xb5, rd, rs, 0);
}

void gen_le(char **code, int rd, int rs) {
    gen_ins(code, 0xbb, rd, rs, 0);
}

void gen_ge(char **code, int rd, int rs) {
    gen_ins(code, 0xb9, rd, rs, 0);
}

void gen_srl(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0x81, rd, rs1, rs2);
}

void gen_mod(char **code, int rd, int rs1, int rs2) {
    gen_ins(code, 0x33, rd, rs1, rs2);
}

void gen_b(char **code, int rd) {
    gen_ins(code, 0x91, rd, 0, 0);
}

void gen_bi(char **code, int immd) {
    uint16_t d = (uint16_t)immd;
    gen_ins(code, 0x92, d % 256, d / 256, 0);
}

void gen_bz(char **code, int rd) {
    gen_ins(code, 0x93, rd, 0, 0);
}

void gen_bl(char **code, int rd) {
    gen_ins(code, 0x95, rd, 0, 0);
}

void gen_nop(char **code) {
    gen_ins(code, 0x0, 0, 0, 0);
}

void gen_sprr(char **code, int rd) {
    gen_ins(code, 0xd1, rd, 0, 0);
}

void unimplemented(int ins) {
    printf("%8.4s unimplemented", &"LEA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,"
        "OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,"
        "OPEN,READ,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,"[ins * 5]);
    printf("\n");
    exit(1);
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
    char data_file[128];
    char addr_file[128];
    int64_t *code, *base_addr;
    // target address
    int code_base;
    char *target_code;
    char *cur_ins;
    int code_mem_size;
    int data_mem_size;
    // Map c4 instruction adress to target code address
    int *address_map;
    // jump(JMP, BZ...) instruction maybe jump to a target which address is larger than itself,
    // the target instruction maybe not compiled, so its address in target machine
    // is unknown.
    uint64_t reloc[1024];
    int reloc_index;
    int ins_index;

    if (argc < 2) {
        printf("usage: %s prefix\n", argv[0]);
        return -1;
    }

    strcpy(code_file, argv[1]);
    strcat(code_file, ".code");
    strcpy(data_file, argv[1]);
    strcat(data_file, ".data");
    strcpy(addr_file, argv[1]);
    strcat(addr_file, ".addr");

    base_addr = read_file(addr_file, 5*sizeof(int64_t));
    if (base_addr == NULL) {
        return -1;
    }
    code = read_file(code_file, base_addr[2]);
    if (code == NULL) {
        return -1;
    }
    // Need more data size
    data_mem_size = 128 * 1024;
    data = read_file(data_file, data_mem_size);
    if (data == NULL) {
        return -1;
    }
    cur_data_index = base_addr[3] / sizeof(int64_t);

    sp = 7;
    bp = 6;
    pc = 8;
    a = 4;
    r = 5;
    t = 3;

    reloc_index = 0;

    // round up to 4k alignment
    code_mem_size = (base_addr[2] + 0x1000) & -0x1000;
    target_code = malloc(code_mem_size);
    cur_ins = target_code;
    code_base = 0;
    data_base = code_mem_size;
    address_map = malloc(base_addr[2]);

    // Reset handler, jump to 0xc
    gen_bi(&cur_ins, 0x30);
    cur_ins = target_code + 0x30;
    // Set sp and bp
    gen_movi(&cur_ins, sp, 0x00100000);
    gen_mov(&cur_ins, bp, sp);
    // Jump to entry point, but the target_entry is unknown, will be updated later
    // jmp_entry_ins = cur_ins;
    reloc[reloc_index++] = (uint64_t)cur_ins;
    reloc[reloc_index++] = base_addr[4];
    gen_movi(&cur_ins, r, 0);
    gen_bl(&cur_ins, r);
    // Save uart address to a
    gen_movi(&cur_ins, a, 0x20000000);
    // Output end
    gen_movi(&cur_ins, r, 'e');
    gen_st32i(&cur_ins, r, a, 0);
    gen_movi(&cur_ins, r, 'n');
    gen_st32i(&cur_ins, r, a, 0);
    gen_movi(&cur_ins, r, 'd');
    gen_st32i(&cur_ins, r, a, 0);
    gen_movi(&cur_ins, r, '\n');
    gen_st32i(&cur_ins, r, a, 0);

    // Dead loop
    gen_bi(&cur_ins, 0);

    for (int i = 1; i < base_addr[2] / sizeof(uint64_t); i++) {
        address_map[i] = cur_ins - target_code;
        switch (code[i]) {
            case LEA:
                // a = (int)(bp + *pc);
                gen_addi(&cur_ins, a, bp, code[++i] * 4);
                break;
            case IMM:
                // a = *pc;
                ++i;
                if (code[i] >= base_addr[1] && code[i] < base_addr[1] + base_addr[3]) {
                    code[i] = code[i] - base_addr[1] + (int)data_base;
                } else if (code[i] >= base_addr[0] && code[i] < base_addr[0] + base_addr[2]) {
                    // This is to get a function address, and the function is defined before
                    code[i] = address_map[(code[i] - base_addr[0]) / sizeof(int64_t)];
                }
                gen_movi(&cur_ins, a, code[i]);
                break;
            case JMP:
                // pc = (int*)*pc
                ins_index = (code[++i] - base_addr[0]) / sizeof(int64_t);
                if (ins_index > i) {
                    // Jump further
                    reloc[reloc_index++] = (uint64_t)cur_ins;
                    reloc[reloc_index++] = ins_index;
                }
                gen_movi(&cur_ins, r, address_map[ins_index]);
                gen_b(&cur_ins, r);
                break;
            case JSR:
                // *--sp = (int)(pc + 1); pc = (int *)*pc;
                gen_movi(&cur_ins, r, address_map[(code[++i] - base_addr[0]) / sizeof(int64_t)]);
                gen_bl(&cur_ins, r);
                break;
            case BZ:
                gen_movi(&cur_ins, r, 0);
                gen_ne(&cur_ins, a, r);
                ins_index = (code[++i] - base_addr[0]) / sizeof(int64_t);
                if (ins_index > i) {
                    // Jump further
                    reloc[reloc_index++] = (uint64_t)cur_ins;
                    reloc[reloc_index++] = ins_index;
                }
                gen_movi(&cur_ins, r, address_map[ins_index]);
                gen_bz(&cur_ins, r);
                break;
            case BNZ:
                unimplemented(code[i]);
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
                // sp = bp
                gen_mov(&cur_ins, sp, bp);
                // bp = (int*)*sp++;
                gen_pop(&cur_ins, bp);
                // pc = (int*)*sp++;
                gen_pop(&cur_ins, r);
                gen_b(&cur_ins, r);
                break;
            case LI:
                // a = *(int*)a;
                gen_ld32i(&cur_ins, a, a, 0);
                break;
            case LC:
                gen_ld8i(&cur_ins, a, a, 0);
                break;
            case SI:
                gen_pop(&cur_ins, r);
                gen_st32i(&cur_ins, a, r, 0);
                break;
            case SC:
                gen_pop(&cur_ins, r);
                gen_st8i(&cur_ins, a, r, 0);
                break;
            case PSH:
                // *--sp = a
                gen_push(&cur_ins, a);
                break;
            case OR:
                unimplemented(code[i]);
                break;
            case XOR:
                unimplemented(code[i]);
                break;
            case AND:
                // a = *sp++ & a
                gen_pop(&cur_ins, r);
                gen_and(&cur_ins, a, r, a);
                break;
            case EQ:
                // a = *sp++ == a
                gen_pop(&cur_ins, r);
                gen_eq(&cur_ins, r, a);
                gen_sprr(&cur_ins, a);
                break;
            case NE:
                // a = *sp++ != a
                gen_pop(&cur_ins, r);
                gen_ne(&cur_ins, r, a);
                gen_sprr(&cur_ins, a);
                break;
            case LT:
                // a = *sp++ < a
                gen_pop(&cur_ins, r);
                gen_lt(&cur_ins, r, a);
                gen_sprr(&cur_ins, a);
                break;
            case GT:
                // a = *sp++ > a
                gen_pop(&cur_ins, r);
                gen_gt(&cur_ins, r, a);
                gen_sprr(&cur_ins, a);
                break;
            case LE:
                // a = *sp++ <= a
                gen_pop(&cur_ins, r);
                gen_le(&cur_ins, r, a);
                gen_sprr(&cur_ins, a);
                break;
            case GE:
                // a = *sp++ >= a
                gen_pop(&cur_ins, r);
                gen_ge(&cur_ins, r, a);
                gen_sprr(&cur_ins, a);
                break;
            case SHL:
                unimplemented(code[i]);
                break;
            case SHR:
                // a = *sp++ >> a
                gen_pop(&cur_ins, r);
                gen_srl(&cur_ins, a, r, a);
                break;
            case ADD:
                // a = *sp++ + a
                gen_pop(&cur_ins, r);
                gen_add(&cur_ins, a, a, r);
                break;
            case SUB:
                // a = *sp++ - a
                gen_pop(&cur_ins, r);
                gen_sub(&cur_ins, a, r, a);
                break;
            case MUL:
                // a = *sp++ * a
                gen_pop(&cur_ins, r);
                gen_mul(&cur_ins, a, r, a);
                break;
            case DIV:
                // a = *sp++ / a
                gen_pop(&cur_ins, r);
                gen_div(&cur_ins, a, r, a);
                break;
            case MOD:
                // a = *sp++ % a
                gen_pop(&cur_ins, r);
                gen_mod(&cur_ins, a, r, a);
                break;
            case OPEN:
                unimplemented(code[i]);
                break;
            case READ:
                unimplemented(code[i]);
                break;
            case CLOS:
                unimplemented(code[i]);
                break;
            case PRTF:
                unimplemented(code[i]);
                break;
            case MALC:
                unimplemented(code[i]);
                break;
            case FREE:
                unimplemented(code[i]);
                break;
            case MSET:
                unimplemented(code[i]);
                break;
            case MCMP:
                unimplemented(code[i]);
                break;
            case EXIT:
                unimplemented(code[i]);
                break;
            default:
                printf("Unkown instruction 0x%lx at %d\n", code[i], i);
                break;
        }
    }

    for (int i = 0; i < reloc_index; i += 2) {
        gen_movi((void*)&reloc[i], r, address_map[reloc[i + 1]]);
    }

    // open "code.bin" for write
    int fd;
    fd = open("code.bin", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        printf("could not open code.bin\n");
        return -1;
    }
    write(fd, target_code, code_mem_size);
    write(fd, data, data_mem_size);
    close(fd);

    return 0;
}