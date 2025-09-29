enum Color {
    RED,
    GREEN,
    BLUE = 5
};


struct Point {
    int x, y;
};


union Data {
    int i;
    float f;
    char str[20];
};


int sum(int a, int b) {
    return a + b;
}


void print_point(struct Point p) {
    printf("Point: (%d, %d)\n", p.x, p.y);
}


void fa(int a, char b);
void fb(int a, char b, ...);
void fc(const char *fmt, ...);


int (*get_op(char op))(int, int) {
    return add;
}


int (*get_op(char op))(int, int);
int add(int x, int y) { return x + y; }


int square(int x) { return x * x; }


int global_var;


int main() {
    int a;
    int b, c;
    int d = 5;
    int *e;
    int ***f;
    int g[10];
    int h[3][3];
    int i[3] = {1,2,3};
    int j = 5, k = 10;
    float l = 3.14;
    char m = 'Z';

    struct Point my_struct = {1, 2};
    union Data my_union;
    enum Color my_color = GREEN;

    static int counter;
    extern int external_var;
    volatile int flag;
    const int max_value = 100;
    register int fast_var;
    static void helper_function(void);

    unsigned int u;
    signed char s;

    int bits = (a & 1) | (b << 8);

    float n = (float) a / b;

    int size = sizeof(struct Point);

    a = 12;
    b = a;
    c = b = 3;

    c = b + 14;
    
    my_union.i = 42;

    int (*function_vector[3])(int);
    function_vector[2](2);

    int (*func_ptr)(int);
    func_ptr = square;
    int result = func_ptr(5);

    if (a < b) {
        printf("a is less than b\n");
    } else {
        printf("a is not less than b\n");
    }
    
    int index;

    for (index = 0; index < 3; index++) {
        printf("Loop %d\n", index);
        i[index] = index * index;
    }

    while (a > 0) {
        a--;
        if (a == 2) break;
    }

    do {
        break;
    } while (1);


    switch (my_color) {
        case RED:
            printf("Color is RED\n");
            break;
        case GREEN:
            printf("Color is GREEN\n");
            break;
        default:
            printf("Unknown color\n");
    }

    struct Point *ptr = &my_struct;
    ptr->x = 0;
    ptr->x += 10;
    ptr->y = 0;
    ptr->y += 20;

    print_point(my_struct);
    printf("Sum = %d\n", sum(3, 4));

    int max = (a > b) ? a : b;
    a += 2;
    a -= 2;
    a *= 3;
    a /= 3;
    a %= 3;

    int value = 10;

    {
        int value = 5;
        printf("%d\n", value);
    }

    label:
        printf("Jumped here\n");
        goto label;

    c = -a;
    printf("Negative of 'a': %d\n", c);
    printf("Sum: %d\n", a + b);
    printf("Sub: %d\n", a - b);
    printf("Mul: %d\n", a * b);
    printf("Div: %d\n", a / b);
    printf("Res: %d\n", a % b);

    printf("a > b: %d\n", a > b);
    printf("a < b: %d\n", a < b);
    printf("a == b: %d\n", a == b);
    printf("a != b: %d\n", a != b);
    printf("a >= b: %d\n", a >= b);
    printf("a <= b: %d\n", a <= b);

    printf("a & b: %d\n", a & b);
    printf("a | b: %d\n", a | b);
    printf("a ^ b: %d\n", a ^ b);
    printf("a >> 1: %d\n", a >> 1);
    printf("a << 1: %d\n", a << 1);
    printf("~a: %d\n", ~a);

    printf("a && b: %d\n", a && b);
    printf("a || b: %d\n", a || b);
    printf("!a: %d\n", !a);

    c = a;
    c += b;
    printf("c += b: %d\n", c);
    c -= b;
    printf("c -= b: %d\n", c);
    c *= b;
    printf("c *= b: %d\n", c);
    c /= b;
    printf("c /= b: %d\n", c);
    c %= b;
    printf("c %%= b: %d\n", c);
    c <<= 1;
    printf("c <<= 1: %d\n", c);
    c >>= 1;
    printf("c >>= 1: %d\n", c);
    
    return 0;
}