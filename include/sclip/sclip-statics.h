#ifndef SCLIP_STATICS_H
#define SCLIP_STATICS_H

#define __SCLIP_STATIC_BLOCK_INCLUDES_DEFS \
    "#ifndef SCLIP_H\n"                    \
    "#define SCLIP_H\n"                    \
    "\n"                                   \
    "#ifdef __cplusplus\n"                 \
    "extern \"C\" {\n"                     \
    "#endif\n"                             \
    "\n"                                   \
    "#include <stdbool.h>\n"               \
    "#include <string.h>\n"                \
    "#include <limits.h>\n"                \
    "#include <stdlib.h>\n"                \
    "#include <stddef.h>\n"                \
    "#include <stdio.h>\n"                 \
    "#include <stdint.h>\n"                \
    "#include <unistd.h>\n"                \
    "\n"                                   \
    "typedef enum {\n"                     \
    "    SCLIP_STRING,\n"                  \
    "    SCLIP_LONG,\n"                    \
    "    SCLIP_DOUBLE,\n"                  \
    "    SCLIP_BOOL,\n"                    \
    "    SCLIP_STDIN,\n"                   \
    "} sclip_option_type;\n"               \
    "\n"                                   \
    "typedef union {\n"                    \
    "    long numeric;\n"                  \
    "    double real;\n"                   \
    "    const char *string;\n"            \
    "} sclip_value;\n"                     \
    "\n"                                   \
    "typedef struct\n"                     \
    "{\n"                                  \
    "    const void *const data;\n"        \
    "    const size_t lenght;\n"           \
    "} sclip_stdin_content;\n"             \
    "\n"                                   \
    "typedef struct\n"                     \
    "{\n"                                  \
    "    const char *long_opt;\n"          \
    "    const char *short_opt;\n"         \
    "    const sclip_option_type type;\n"  \
    "    sclip_value value;\n"             \
    "    const bool optional;\n"           \
    "} sclip_option;\n\n"

#define __SCLIP_STATIC_BLOCK_FUNC_DECL                                                                                        \
    "#define sclip_parse(argc, argv) \\\n"                                                                                    \
    "    __sclip_parse(argc, argv, &SCLIP_OPTIONS[0])\n"                                                                      \
    "static inline void __sclip_parse(int argc, const char **argv, sclip_option *restrict options);\n"                        \
    "static inline bool sclip_opt_matches(const char *arg, sclip_option *restrict option);\n"                                 \
    "static inline sclip_value sclip_opt_parse_long(const char *arg);\n"                                                      \
    "static inline sclip_value sclip_opt_parse_double(const char *arg);\n"                                                    \
    "static inline double sclip_opt_get_value_double(const sclip_option *restrict options, const sclip_option_id id);\n"      \
    "static inline long sclip_opt_get_value_long(const sclip_option *restrict options, const sclip_option_id id);\n"          \
    "static inline bool sclip_opt_get_value_bool(const sclip_option *restrict options, const sclip_option_id id);\n"          \
    "static inline const char *sclip_opt_get_value_string(const sclip_option *restrict options, const sclip_option_id id);\n" \
    "static inline bool sclip_opt_is_provided(const sclip_option *restrict options, const sclip_option_id id);\n"             \
    "static inline sclip_stdin_content sclip_get_stdin_contents();\n"                                                         \
    "static inline void sclip_free_stdin_content(sclip_stdin_content *restrict const content);\n"                             \
    "\n"                                                                                                                      \
    "#ifdef __cplusplus\n"                                                                                                    \
    "}// extern \"C\"\n"                                                                                                      \
    "#endif\n"                                                                                                                \
    "\n"

#define __SCLIP_STATIC_BLOCK_FUNC_BODY                                                                                                                       \
    "#ifdef SCLIP_IMPL\n"                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "#ifdef __cplusplus\n"                                                                                                                                   \
    "extern \"C\" {\n"                                                                                                                                       \
    "#endif\n"                                                                                                                                               \
    "\n"                                                                                                                                                     \
    "#include <assert.h>\n"                                                                                                                                  \
    "\n"                                                                                                                                                     \
    "static inline double sclip_opt_get_value_double(const sclip_option *restrict options, const sclip_option_id id)\n"                                      \
    "{\n"                                                                                                                                                    \
    "    return options[id].value.real;\n"                                                                                                                   \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "static inline long sclip_opt_get_value_long(const sclip_option *restrict options, const sclip_option_id id)\n"                                          \
    "{\n"                                                                                                                                                    \
    "    return options[id].value.numeric;\n"                                                                                                                \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "static inline bool sclip_opt_get_value_bool(const sclip_option *restrict options, const sclip_option_id id)\n"                                          \
    "{\n"                                                                                                                                                    \
    "    return options[id].value.numeric == 1;\n"                                                                                                           \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "static inline const char *sclip_opt_get_value_string(const sclip_option *restrict options, const sclip_option_id id)\n"                                 \
    "{\n"                                                                                                                                                    \
    "    return options[id].value.string;\n"                                                                                                                 \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "static inline bool sclip_opt_is_provided(const sclip_option *restrict options, const sclip_option_id id)\n"                                             \
    "{\n"                                                                                                                                                    \
    "    return options[id].value.numeric != LONG_MIN;\n"                                                                                                    \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "static inline bool sclip_opt_matches(const char *arg, sclip_option *restrict option)\n"                                                                 \
    "{\n"                                                                                                                                                    \
    "    assert(arg != NULL);\n"                                                                                                                             \
    "    if (arg[0] != '-') return false;\n"                                                                                                                 \
    "    if (option->short_opt != NULL && strcmp(arg, option->short_opt) == 0)\n"                                                                            \
    "        return true;\n"                                                                                                                                 \
    "    else if (option->long_opt != NULL && strcmp(arg, option->long_opt) == 0)\n"                                                                         \
    "        return true;\n"                                                                                                                                 \
    "    return false;\n"                                                                                                                                    \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "static inline sclip_value sclip_opt_parse_long(const char *arg)\n"                                                                                      \
    "{\n"                                                                                                                                                    \
    "    assert(arg != NULL);\n"                                                                                                                             \
    "    static const int base = 10;\n"                                                                                                                      \
    "    char *end_ptr = NULL;\n"                                                                                                                            \
    "    const long ret = strtol(arg, &end_ptr, base);\n"                                                                                                    \
    "    if (end_ptr == arg) return (sclip_value){ .numeric = LONG_MIN };\n"                                                                                 \
    "    return (sclip_value){ .numeric = ret };\n"                                                                                                          \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "static inline sclip_value sclip_opt_parse_double(const char *arg)\n"                                                                                    \
    "{\n"                                                                                                                                                    \
    "    assert(arg != NULL);\n"                                                                                                                             \
    "    char *end_ptr = NULL;\n"                                                                                                                            \
    "    const double ret = strtod(arg, &end_ptr);\n"                                                                                                        \
    "    if (end_ptr == arg) return (sclip_value){ .numeric = LONG_MIN };\n"                                                                                 \
    "    return (sclip_value){ .real = ret };\n"                                                                                                             \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "static inline void __sclip_parse(int argc, const char **argv, sclip_option *restrict options)\n"                                                        \
    "{\n"                                                                                                                                                    \
    "    if(argc == 1) {\n"                                                                                                                                  \
    "        fputs(SCLIP_HELP_STR, stdout);\n"                                                                                                               \
    "        exit(EXIT_SUCCESS);\n"                                                                                                                          \
    "    }\n"                                                                                                                                                \
    "    for (register int j = SCLIP_OPTION_VERSION_ID; j >= 0; j--) {\n"                                                                                    \
    "        for (register int i = 1; i < argc; i++) {\n"                                                                                                    \
    "            if (!sclip_opt_matches(argv[i], &options[j])) continue;\n"                                                                                  \
    "            switch (options[j].type) {\n"                                                                                                               \
    "            case SCLIP_STRING: {\n"                                                                                                                     \
    "                options[j].value = (sclip_value){ .string = argv[i + 1] };\n"                                                                           \
    "            } break;\n"                                                                                                                                 \
    "            case SCLIP_LONG: {\n"                                                                                                                       \
    "                options[j].value = sclip_opt_parse_long(argv[i + 1]);\n"                                                                                \
    "            } break;\n"                                                                                                                                 \
    "            case SCLIP_DOUBLE: {\n"                                                                                                                     \
    "                options[j].value = sclip_opt_parse_double(argv[i + 1]);\n"                                                                              \
    "            } break;\n"                                                                                                                                 \
    "            case SCLIP_STDIN: {\n"                                                                                                                      \
    "                if(!isatty(STDIN_FILENO)) {\n"                                                                                                          \
    "                   options[j].value = (sclip_value){ .numeric = 1 };\n"                                                                                 \
    "                }\n"                                                                                                                                    \
    "            } break;\n"                                                                                                                                 \
    "            case SCLIP_BOOL: {\n"                                                                                                                       \
    "                if (j == SCLIP_OPTION_VERSION_ID || j == SCLIP_OPTION_HELP_ID) {\n"                                                                     \
    "                   puts(options[j].value.string);\n"                                                                                                    \
    "                   exit(EXIT_SUCCESS);\n"                                                                                                               \
    "                }\n"                                                                                                                                    \
    "                options[j].value = (sclip_value){ .numeric = 1 };\n"                                                                                    \
    "            } break;\n"                                                                                                                                 \
    "            default: {\n"                                                                                                                               \
    "                options[j].value = (sclip_value){ .numeric = LONG_MIN };\n"                                                                             \
    "            } break;\n"                                                                                                                                 \
    "            }\n"                                                                                                                                        \
    "        }\n"                                                                                                                                            \
    "        if (!options[j].optional && options[j].value.numeric == LONG_MIN) {\n"                                                                          \
    "            fprintf(stderr, \"Mandatory option/value %s, %s was not provided\\nRefer to --help, -h\\n\", options[j].long_opt, options[j].short_opt);\n" \
    "            exit(EXIT_FAILURE);\n"                                                                                                                      \
    "        }\n"                                                                                                                                            \
    "    }\n"                                                                                                                                                \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "static inline sclip_stdin_content sclip_get_stdin_contents()\n"                                                                                         \
    "{\n"                                                                                                                                                    \
    "    static const size_t default_size = 2;\n"                                                                                                            \
    "    char *data = NULL;\n"                                                                                                                               \
    "    char buffer[default_size];\n"                                                                                                                       \
    "    size_t maximum_size = default_size;\n"                                                                                                              \
    "    size_t total_bytes_read = 0;\n"                                                                                                                     \
    "    size_t bytes_read = 0;\n"                                                                                                                           \
    "\n"                                                                                                                                                     \
    "    if ((data = malloc(default_size)) == NULL) {\n"                                                                                                     \
    "        return (sclip_stdin_content){ .data = NULL, .lenght = 0 };\n"                                                                                   \
    "    }\n"                                                                                                                                                \
    "\n"                                                                                                                                                     \
    "    while ((bytes_read = fread(buffer, 1, default_size, stdin)) > 0) {\n"                                                                               \
    "        if ((total_bytes_read + bytes_read) > maximum_size) {\n"                                                                                        \
    "            maximum_size = 2 * maximum_size;\n"                                                                                                         \
    "            if ((data = realloc(data, maximum_size)) == NULL) {\n"                                                                                      \
    "                return (sclip_stdin_content){ .data = NULL, .lenght = 0 };\n"                                                                           \
    "            }\n"                                                                                                                                        \
    "        }\n"                                                                                                                                            \
    "        memcpy(data + total_bytes_read, buffer, bytes_read);\n"                                                                                         \
    "        total_bytes_read += bytes_read;\n"                                                                                                              \
    "    }\n"                                                                                                                                                \
    "    return (sclip_stdin_content){ .data = data, .lenght = total_bytes_read };\n"                                                                        \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "static inline void sclip_free_stdin_content(sclip_stdin_content *restrict const content)\n"                                                             \
    "{\n"                                                                                                                                                    \
    "    free((void *)content->data);\n"                                                                                                                     \
    "}\n"                                                                                                                                                    \
    "\n"                                                                                                                                                     \
    "#ifdef __cplusplus\n"                                                                                                                                   \
    "}// extern \"C\"\n"                                                                                                                                     \
    "#endif\n"                                                                                                                                               \
    "\n"                                                                                                                                                     \
    "#endif\n"                                                                                                                                               \
    "#endif\n"

#endif// SCLIP_STATICS_H
