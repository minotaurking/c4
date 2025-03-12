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
            }
            p--;
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

void test(int t) {
    int *pp;
    pp = &t;
    if (*pp == 3) {
        print0("3\n");
    }
}

int main() {
    a = 2;
    b = -1;
    c = 1;
    ppp = &a;

    c = c + b;
    if (*ppp == 2) {
        print1("c is %d\n", c);
    }

    test(b);

    return 0;
}