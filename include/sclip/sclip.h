#ifndef SCLIP_H
#define SCLIP_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
    SCLIP_STRING,
    SCLIP_LONG,
    SCLIP_DOUBLE,
    SCLIP_PRESENCE,
    SCLIP_STDIN,
} sclip_option_types;

typedef struct
{
    const char *long_opt;
    const char *short_opt;
    char *value;
    sclip_option_types type;
    bool optional;
    const typedef union {
        long numeric;
        double real;
        char *string;
    } value;
} sclip_option;

//!

static inline bool sclip_parse(int argc, const char **argv, sclip_option *options);

#ifdef SCLIP_IMPL
static inline bool sclip_parse(int argc, const char **argv, sclip_option *options)
{
    for (int i = 1; i < argc; i++) {
        for (sclip_option *option = options; options != NULL; option++) {
            switch (option->type) {
            case SCLIP_STRING: {
                if (strcmp(option->long_opt, argv[i]) == 0 || strcmp(option->short_opt, argv[i]) == 0) {
                    option->value = (char *)malloc(strlen(argv[i]));
                    memset(option->value, 0x00, strlen(argv[i]));
                    strcpy(option->value, argv[i + 1]);
                    i++;
                }
            } break;
            case SCLIP_INT: {
                if (strcmp(option->long_opt, argv[i]) == 0 || strcmp(option->short_opt, argv[i]) == 0) {
                    option->value = (char *)malloc(sizeof(int));
                    memset(option->value, 0x00, sizeof(int));
                    *option->value = strtol(argv[i + 1], NULL, 10);
                    i++;
                }
            } break;
            case SCLIP_DOUBLE: {
                if (strcmp(option->long_opt, argv[i]) == 0 || strcmp(option->short_opt, argv[i]) == 0) {
                    option->value = (char *)malloc(sizeof(double));
                    memset(option->value, 0x00, sizeof(double));
                    *option->value = strtod(argv[i + 1], NULL);
                    i++;
                }
            } break;
            case SCLIP_PRESENCE: {
                if (strcmp(option->long_opt, argv[i]) == 0 || strcmp(option->short_opt, argv[i]) == 0) {
                    option->value = (char *)malloc(sizeof(bool));
                    memset(option->value, 0x00, sizeof(bool));
                    *option->value = (char)true;
                }
            } break;
            }
        }
    }
    return false;
}

#endif// SCLI_IMPL
#endif// SCLIP_H
