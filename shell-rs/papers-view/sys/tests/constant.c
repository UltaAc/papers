// Generated by gir (https://github.com/gtk-rs/gir @ 20a5b17f6da9)
// from ../../ev-girs (@ 14c0b6a80649+)
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
    PRINT_CONSTANT((gint) PPS_JOB_N_PRIORITIES);
    PRINT_CONSTANT((gint) PPS_JOB_PRIORITY_HIGH);
    PRINT_CONSTANT((gint) PPS_JOB_PRIORITY_LOW);
    PRINT_CONSTANT((gint) PPS_JOB_PRIORITY_NONE);
    PRINT_CONSTANT((gint) PPS_JOB_PRIORITY_URGENT);
    PRINT_CONSTANT((gint) PPS_JOB_RUN_MAIN_LOOP);
    PRINT_CONSTANT((gint) PPS_JOB_RUN_THREAD);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_ALL);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_ANNOTS);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_FORMS);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_IMAGES);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_LINKS);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_MEDIA);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_NONE);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_TEXT);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_TEXT_ATTRS);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_TEXT_LAYOUT);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_TEXT_LOG_ATTRS);
    PRINT_CONSTANT((guint) PPS_PAGE_DATA_INCLUDE_TEXT_MAPPING);
    PRINT_CONSTANT((gint) PPS_PAGE_LAYOUT_AUTOMATIC);
    PRINT_CONSTANT((gint) PPS_PAGE_LAYOUT_DUAL);
    PRINT_CONSTANT((gint) PPS_PAGE_LAYOUT_SINGLE);
    PRINT_CONSTANT((gint) PPS_SIZING_AUTOMATIC);
    PRINT_CONSTANT((gint) PPS_SIZING_FIT_PAGE);
    PRINT_CONSTANT((gint) PPS_SIZING_FIT_WIDTH);
    PRINT_CONSTANT((gint) PPS_SIZING_FREE);
    return 0;
}
