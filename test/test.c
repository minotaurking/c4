int a;
int b;

void putchar(char c) {
    char *p;
    p = 0x800;
    p = p * 256;
    *p = c;
}

void print(char *s) {
    while (*s > 0) {
        putchar(*s);
        s++;
    }
}

void assert(int c) {
    if (c == 0) {
        print("assert failed");
    } else {
        print("assert passed");
    }
}

int main() {
    a = 2;
    b = 3;

    assert(b == 4);

    return 0;
}