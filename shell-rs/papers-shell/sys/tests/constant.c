// Generated by gir (https://github.com/gtk-rs/gir @ 20a5b17f6da9)
// from ../../ev-girs (@ f3f06bcf4a06+)
// from ../../gir-files (@ 20031a537e40)
// DO NOT EDIT

#include "manual.h"
#include <stdio.h>

#define PRINT_CONSTANT(CONSTANT_NAME) \
    printf("%s;", #CONSTANT_NAME); \
    printf(_Generic((CONSTANT_NAME), \
                    char *: "%s", \
                    const char *: "%s", \
                    char: "%c", \
                    signed char: "%hhd", \
                    unsigned char: "%hhu", \
                    short int: "%hd", \
                    unsigned short int: "%hu", \
                    int: "%d", \
                    unsigned int: "%u", \
                    long: "%ld", \
                    unsigned long: "%lu", \
                    long long: "%lld", \
                    unsigned long long: "%llu", \
                    float: "%f", \
                    double: "%f", \
                    long double: "%ld"), \
           CONSTANT_NAME); \
    printf("\n");

int main() {
    PRINT_CONSTANT((gint) PPS_PRINT_PAGE_SET_ALL);
    PRINT_CONSTANT((gint) PPS_PRINT_PAGE_SET_EVEN);
    PRINT_CONSTANT((gint) PPS_PRINT_PAGE_SET_ODD);
    PRINT_CONSTANT((gint) PPS_WINDOW_MODE_ERROR_VIEW);
    PRINT_CONSTANT((gint) PPS_WINDOW_MODE_FULLSCREEN);
    PRINT_CONSTANT((gint) PPS_WINDOW_MODE_NORMAL);
    PRINT_CONSTANT((gint) PPS_WINDOW_MODE_PASSWORD_VIEW);
    PRINT_CONSTANT((gint) PPS_WINDOW_MODE_PRESENTATION);
    PRINT_CONSTANT((gint) PPS_WINDOW_MODE_START_VIEW);
    return 0;
}
