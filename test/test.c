int failed_case;
int ia;
int ib;
int ic;
char ca;
int *pa;

void putchar(char c) {
    char *p;
    p = 0x20000000;
    *p = c;
}

void print4(char *s, int p1, int p2, int p3, int p4) {
    int *p;
    char t9, t8, t7, t6, t5, t4, t3, t2, t1, t0;
    char *t, *te;
    int tmp, digit;
    char *cp;
    p = &p1;

    while (*s > 0) {
        if (*s != '%') {
            putchar(*s);
        } else {
            s++;
            if (*s == 'c') {
                putchar(*p);
            } else if (*s == 'd') {
                if (*p == 0) {
                    putchar('0');
                } else if (*p == -2147483648) {
                    putchar('-');
                    putchar('2');
                    putchar('1');
                    putchar('4');
                    putchar('7');
                    putchar('4');
                    putchar('7');
                    putchar('3');
                    putchar('6');
                    putchar('4');
                    putchar('8');
                } else {
                    if (*p < 0) {
                        putchar('-');
                        *p = -(*p);
                    }
                    te = &t9;
                    t = &t9;
                    while (*p > 0) {
                        *t-- = (*p % 10) + '0';
                        *p = *p / 10;
                    }
                    while (t <= te) {
                        putchar(*t++);
                    }
                }
            } else if (*s == 'x') {
                if (*p == 0) {
                    putchar('0');
                } else {
                    te = &t9;
                    t = &t9;
                    tmp = *p;
                    while (tmp != 0) {
                        digit = tmp & 15;
                        if (digit < 0) {
                            digit = digit + 16;
                        }
                        *t-- = (digit < 10 ? digit + '0' : digit - 10 + 'a');
                        tmp = tmp >> 4;
                    }
                    while (t <= te) {
                        putchar(*t++);
                    }
                }
            }
            else if (*s == 's') {
                cp = *p;
                while (*cp != 0) {
                    putchar(*cp);
                    cp++;
                }
            }

            // Fix me: should be p--, but pointer in c4 is 64 bit, in target is 32 bit
            // So we expect the code is p = (int)p - 4, but we got p = (int)p - 8 which
            // is generate by c4.
            t = (int)p;
            t = t - 4;
            p = (int*)t;
        }
        s++;
    }
}

void print0(char *s) {
    print4(s, 0, 0, 0, 0);
}

void print1(char *s, int p1) {
    print4(s, p1, 0, 0, 0);
}

void print2(char *s, int p1, int p2) {
    print4(s, p1, p2, 0, 0);
}

void print3(char *s, int p1, int p2, int p3) {
    print4(s, p1, p2, p3, 0);
}

void assert(char *file, int line, char *expr, int cond) {
    if (cond == 0) {
        print3("assert failed!\n%s:%d\n%s\n", file, line, expr);
        failed_case++;
    }
}

void unimplemented_handler() {
    print0("unimplemented handler\n");
    while (1);
}

void abort_handler() {
    int *p;
    int addr, type, pc;
    p = 0x20001000;
    addr = *p;
    p = 0x20001004;
    type = *p;
    p = 0x20001008;
    pc = *p;
    print3("abort: 0x%x, type 0x%x, pc 0x%x\n", addr, type, pc);
    while (1);
}

void init() {
    int addr;
    int *vector;

    addr = 4;
    vector = (int*)addr;
    // undefined instruction
    *vector = unimplemented_handler;
    // abort
    addr = addr + 4;
    vector = (int*)addr;
    *vector = abort_handler;
    // Division by zero
    addr = addr + 4;
    vector = (int*)addr;
    *vector = unimplemented_handler;
    // Reserved exception
    addr = addr + 4;
    vector = (int*)addr;
    *vector = unimplemented_handler;
    // interrupt 0
    addr = addr + 4;
    vector = (int*)addr;
    *vector = unimplemented_handler;
    // interrupt 1
    addr = addr + 4;
    vector = (int*)addr;
    *vector = unimplemented_handler;
    // interrupt 2
    addr = addr + 4;
    vector = (int*)addr;
    *vector = unimplemented_handler;
    // interrupt 3
    addr = addr + 4;
    vector = (int*)addr;
    *vector = unimplemented_handler;
    // interrupt 4
    addr = addr + 4;
    vector = (int*)addr;
    *vector = unimplemented_handler;
    // interrupt 5
    addr = addr + 4;
    vector = (int*)addr;
    *vector = unimplemented_handler;
    // interrupt 6
    addr = addr + 4;
    vector = (int*)addr;
    *vector = unimplemented_handler;
    // interrupt 7
    addr = addr + 4;
    vector = (int*)addr;
    *vector = unimplemented_handler;
}

// Test global variable access
void test_global() {
    ia = 10;
    ib = 20;
    ic = ia + ib;
    ASSERT(ic == 30);
}

// Test local variable access
void test_local() {
    int a;
    int b;
    int c;
    a = 10;
    b = 20;
    c = a + b;
    ASSERT(c == 30);
}

// Test function parameters and return value
int test_func_para(int a, int b) {
    int c;
    c = a + b;
    return c;
}

// Pass global variable as parameters
void test_func_global() {
    ia = 30;
    ib = 40;
    ic = test_func_para(ia, ib);
    ASSERT(ic == 70);
}

// Pass local variable as parameters
void test_func_local() {
    int a;
    int b;
    int c;
    a = 30;
    b = 40;
    c = test_func_para(a, b);
    ASSERT(c == 70);
}

void test_large_number() {
    int a;
    int b;

    a = 65536;
    b = a - 32768;
    ASSERT(b == 32768);
}

void test_negtive_number() {
    int a;
    int b;

    a = -257;
    b = a + 257;
    ASSERT(b == 0);
}

void test_large_negtive_number() {
    int a;
    int b;

    a = -32768;
    b = a + 32768;
    ASSERT(b == 0);
}

void test_abort() {
    int *p;
    p = 0x8000000;
    *p = 97;
}

int main() {
    init();
    failed_case = 0;
    test_global();
    test_local();
    test_func_global();
    test_large_number();
    test_negtive_number();
    test_large_negtive_number();
    test_abort();

    print1("failed_case: %d\n", failed_case);
    return 0;
}