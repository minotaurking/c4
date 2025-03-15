int failed_case;
int ia;
int ib;
int ic;
char ca;
int *pa;

void putchar(char c) {
    char *p;
    p = 0x80000;
    *p = c;
}

void print4(char *s, int p1, int p2, int p3, int p4) {
    int *p;
    int t;
    int d;
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
                if (*p < 0) {
                    putchar('-');
                    *p = -(*p);
                }
                t = 0;
                while (*p > 0) {
                    t = t * 10 + (*p % 10);
                    *p = *p / 10;
                }
                if (t == 0) {
                    putchar('0');
                } else {
                    while (t > 0) {
                        putchar(t % 10 + '0');
                        t = t / 10;
                    }
                }
            } else if (*s == 's') {
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

int main() {
    failed_case = 0;
    test_global();
    test_local();
    test_func_global();
    test_large_number();

    print1("failed_case: %d\n", failed_case);
    return 0;
}