#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// If you need to call a function in a shared library you can define it here
// extern "C" void function_do_something(void *param_1);

int main(int argc, char *argv[]) {
    int x = 1;
    printf("[*] Hello we are on android\n");
    for (int i = 0; i < 20; i++) {
        x *= 2;
        printf("[!] x: %d\n", x);
    }

    void *y = malloc(0x10);
    memset(y, 0x61, 0x20);
    // function_do_something(y);

    printf("done");

    return 0;
}


