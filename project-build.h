#ifndef SCLIP_H
#define SCLIP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

typedef enum {
    SCLIP_STRING,
    SCLIP_LONG,
    SCLIP_DOUBLE,
    SCLIP_BOOL,
    SCLIP_STDIN,
} sclip_option_type;

typedef union {
    long numeric;
    double real;
    const char *string;
} sclip_value;

typedef struct
{
    const char *long_opt;
    const char *short_opt;
    const sclip_option_type type;
    sclip_value value;
    const bool optional;
} sclip_option;

#define SCLIP_HELP_STR \
"Usage:\n"\
"project-build [options]\n"\
"    -t ,--build-tests  <bool>      this optiona will enable test builds \n"\
"    -d ,--enable-debug <bool>      this options will build a debug release \n"\
"    -n ,--nuber        <long>      this will just print a number \n"\
"    -h ,--help         <bool>      Shows help menu \n"\
"    -v ,--version      <bool>      Shows version string \n"\
""

#define SCLIP_VERSION_STR "project-build 0.0.1\n"

typedef enum {
    SCLIP_OPTION_BUILD_TESTS_ID,
    SCLIP_OPTION_ENABLE_DEBUG_ID,
    SCLIP_OPTION_NUMBER_ID,
    SCLIP_OPTION_HELP_ID,
    SCLIP_OPTION_VERSION_ID
} sclip_option_id;

static sclip_option SCLIP_OPTIONS[] = {
    [SCLIP_OPTION_BUILD_TESTS_ID] = { .long_opt = "--build-tests", .short_opt = "-t", .type = SCLIP_BOOL, .optional = true, .value = { .numeric = LONG_MIN } },
    [SCLIP_OPTION_ENABLE_DEBUG_ID] = { .long_opt = "--enable-debug", .short_opt = "-d", .type = SCLIP_BOOL, .optional = true, .value = { .numeric = LONG_MIN } },
    [SCLIP_OPTION_NUMBER_ID] = { .long_opt = "--nuber", .short_opt = "-n", .type = SCLIP_LONG, .optional = true, .value = { .numeric = LONG_MIN } },
    [SCLIP_OPTION_HELP_ID] = { .long_opt = "--help", .short_opt = "-h", .type = SCLIP_BOOL, .optional = true, .value = { .string = SCLIP_HELP_STR } },
    [SCLIP_OPTION_VERSION_ID] = { .long_opt = "--version", .short_opt = "-v", .type = SCLIP_BOOL, .optional = true, .value = { .string = SCLIP_VERSION_STR } }
};

#define sclip_parse(argc, argv) \
    __sclip_parse(argc, argv, &SCLIP_OPTIONS[0])
static inline void __sclip_parse(int argc, const char **argv, sclip_option *restrict options);
static inline bool sclip_is_opt(const char *arg);
static inline bool sclip_opt_matches(const char *arg, sclip_option *restrict option);
static inline sclip_value sclip_opt_parse_long(const char *arg);
static inline sclip_value sclip_opt_parse_double(const char *arg);
static inline double sclip_opt_get_value_double(const sclip_option *restrict options, const sclip_option_id id);
static inline long sclip_opt_get_value_long(const sclip_option *restrict options, const sclip_option_id id);
static inline bool sclip_opt_get_value_bool(const sclip_option *restrict options, const sclip_option_id id);
static inline const char *sclip_opt_get_value_string(const sclip_option *restrict options, const sclip_option_id id);
static inline bool sclip_opt_is_provided(const sclip_option *restrict options, const sclip_option_id id);

#ifdef __cplusplus
}// extern "C"
#endif

#define sclip_opt_build_tests_is_provided() \
    sclip_opt_is_provided(&SCLIP_OPTIONS[0], SCLIP_OPTION_BUILD_TESTS_ID)
#define sclip_opt_enable_debug_is_provided() \
    sclip_opt_is_provided(&SCLIP_OPTIONS[0], SCLIP_OPTION_ENABLE_DEBUG_ID)
#define sclip_opt_number_is_provided() \
    sclip_opt_is_provided(&SCLIP_OPTIONS[0], SCLIP_OPTION_NUMBER_ID)

#define sclip_opt_build_tests_get_value() \
    sclip_opt_get_value_bool(&SCLIP_OPTIONS[0], SCLIP_OPTION_BUILD_TESTS_ID)
#define sclip_opt_enable_debug_get_value() \
    sclip_opt_get_value_bool(&SCLIP_OPTIONS[0], SCLIP_OPTION_ENABLE_DEBUG_ID)
#define sclip_opt_number_get_value() \
    sclip_opt_get_value_long(&SCLIP_OPTIONS[0], SCLIP_OPTION_NUMBER_ID)

#ifdef SCLIP_IMPL

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>

static inline double sclip_opt_get_value_double(const sclip_option *restrict options, const sclip_option_id id)
{
    return options[id].value.real;
}

static inline long sclip_opt_get_value_long(const sclip_option *restrict options, const sclip_option_id id)
{
    return options[id].value.numeric;
}

static inline bool sclip_opt_get_value_bool(const sclip_option *restrict options, const sclip_option_id id)
{
    return options[id].value.numeric == 1;
}

static inline const char *sclip_opt_get_value_string(const sclip_option *restrict options, const sclip_option_id id)
{
    return options[id].value.string;
}

static inline bool sclip_opt_is_provided(const sclip_option *restrict options, const sclip_option_id id)
{
    return options[id].value.numeric != LONG_MIN;
}

static inline bool sclip_is_opt(const char *arg)
{
    if (arg == NULL || strlen(arg) < 2)
        return false;
    else if (arg[0] == '-' && arg[1] == '-')
        return true;
    else if (arg[0] == '-' && arg[1] != '-')
        return true;
    return false;
}

static inline bool sclip_opt_matches(const char *arg, sclip_option *restrict option)
{
    assert(arg != NULL);
    if (option->short_opt != NULL && strcmp(arg, option->short_opt) == 0)
        return true;
    else if (option->long_opt != NULL && strcmp(arg, option->long_opt) == 0)
        return true;
    return false;
}

static inline sclip_value sclip_opt_parse_long(const char *arg)
{
    assert(arg != NULL);
    static const int base = 10;
    char *end_ptr = NULL;
    const long ret = strtol(arg, &end_ptr, base);
    if (end_ptr == arg) return (sclip_value){ .numeric = LONG_MIN };
    return (sclip_value){ .numeric = ret };
}

static inline sclip_value sclip_opt_parse_double(const char *arg)
{
    assert(arg != NULL);
    char *end_ptr = NULL;
    const double ret = strtod(arg, &end_ptr);
    if (end_ptr == arg) return (sclip_value){ .numeric = LONG_MIN };
    return (sclip_value){ .real = ret };
}

static inline void __sclip_parse(int argc, const char **argv, sclip_option *restrict options)
{
    for (register int j = 0; j <= SCLIP_OPTION_VERSION_ID; j++) {
        for (register int i = 1; i < argc; i++) {
            if (!sclip_is_opt(argv[i])) continue;
            if (!sclip_opt_matches(argv[i], &options[j])) continue;
            switch (options[j].type) {
            case SCLIP_STRING: {
                options[j].value = (sclip_value){ .string = argv[i + 1] };
            } break;
            case SCLIP_LONG: {
                options[j].value = sclip_opt_parse_long(argv[i + 1]);
            } break;
            case SCLIP_DOUBLE: {
                options[j].value = sclip_opt_parse_double(argv[i + 1]);
            } break;
            case SCLIP_BOOL: {
                if (j == SCLIP_OPTION_VERSION_ID || j == SCLIP_OPTION_HELP_ID) {
                   puts(options[j].value.string);
                   exit(EXIT_SUCCESS);
                }
                options[j].value = (sclip_value){ .numeric = 1 };
            } break;
            default: {
                options[j].value = (sclip_value){ .numeric = LONG_MIN };
            } break;
            }
        }
        if (!options[j].optional && options[j].value.numeric == LONG_MIN) {
            fprintf(stderr, "Mandatory option/value %s, %s was not provided\nRefer to --help, -h\n", options[j].long_opt, options[j].short_opt);
            exit(EXIT_FAILURE);
        }
    }
}

#ifdef __cplusplus
}// extern "C"
#endif

#endif
#endif
