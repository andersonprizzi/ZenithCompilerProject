/*enum Color {
    RED,
    GREEN,
    BLUE = 5
};*/

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
    int a = 5, b = 10;
    float f = 3.14;
    //char c = 'Z';
    int arr[10] = {0};
    //enum Color my_color = GREEN;
    //struct Point p1 = { .x = 1, .y = 2 };
    struct Point p1 = { 1, 2 };
    union Data d;
    
    d.i = 42;

    if (a < b) {
        printf("a is less than b\n");
    } else {
        printf("a is not less than b\n");
    }
    
    int i;
    for (i = 0; i < 3; i++) {
        printf("Loop %d\n", i);
        arr[i] = i * i;
    }

    while (a > 0) {
        a--;
        if (a == 2) break;
    }

    /*switch (my_color) {
        case RED:
            printf("Color is RED\n");
            break;
        case GREEN:
            printf("Color is GREEN\n");
            break;
        default:
            printf("Unknown color\n");
    }*/

    struct Point *ptr = &p1;
    ptr->x += 10;
    ptr->y += 20;

    print_point(p1);
    printf("Sum = %d\n", sum(3, 4));
    printf("Square of 4 = %d\n", SQUARE(4));
    
    return 0;
}
