int a;
int b;
char c;
int *ppp;

void putchar(char c) {
    char *p;
    p = 0x800;
    p = p * 256;
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
    }
}

void test(int t) {
    int *pp;
    pp = &t;
    if (*pp == 3) {
        print0("3\n");
    }
}

int main() {
    a = 12;
    b = 23;
    print4("print %d %d %s %d\n", a, b, "abc", b + 2);

    ASSERT(b == 23);
    ASSERT(a == 13);

    return 0;
}