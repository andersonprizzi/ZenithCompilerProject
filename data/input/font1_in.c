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
    //char m = 'Z';
    struct Point my_struct = {1, 2};
    union Data my_union;
    enum Color my_color = GREEN;

    a = 12;
    b = a;
    c = b = 3;
    
    my_union.i = 42;

    int (*n[3])(int);
    n[2](2);

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
    
    return 0;
}
