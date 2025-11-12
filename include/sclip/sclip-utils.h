#ifndef SCLIP_UTILS_H
#define SCLIP_UTILS_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

typedef struct sclip_option_t sclip_option;
typedef struct sclip_config_t sclip_config;
typedef struct sclip_scenario_item_t sclip_scenario_item;

struct sclip_option_t
{
    sclip_option *next;
    char *long_opt;
    char *short_opt;
    char *value_type;
    char *name;
    bool optional;
};

struct sclip_config_t
{
    sclip_option *options;
    int count;
};

struct sclip_scenario_item_t
{
    const char *static_content;
    void (*generator)(const sclip_config *config, FILE *file);
};

static inline sclip_option *sclip_config_allocate_option(sclip_config *options);
static inline void sclip_config_destroy(sclip_config *options);
static inline char *sclip_read_file(const char *filepath);
static inline cJSON *sclip_read_config(const char *filepath);
static inline void sclip_print_cjson_error();

static inline bool sclip_parse_options(const cJSON *const json_config, sclip_config *config);

static inline bool sclip_verify_config_attribute_name(const char *const name_str);
static inline bool sclip_verify_config_attribute_short_opt(const char *const short_opt_str);
static inline bool sclip_verify_config_attribute_long_opt(const char *const long_opt_str);
static inline bool sclip_verify_config_attribute_value_type(const char *const value_type_str);

static inline void sclip_generate_options_enum_decl(const sclip_config *config, FILE *file);
static inline void sclip_generate_options_declaration(const sclip_config *config, FILE *file);
static inline void sclip_generator_header_guard(const sclip_config *config, FILE *file);
static inline void sclip_generator_value_getters(const sclip_config *config, FILE *file);
static inline void sclip_generator_presence_checkers(const sclip_config *config, FILE *file);

static const sclip_scenario_item SCLIP_GENERATION_SCENARIO[] = {
    { .static_content = NULL, .generator = sclip_generator_header_guard },
    { .static_content =
            "#ifdef __cplusplus\n"
            "extern \"C\" {\n"
            "#endif\n"
            "\n"
            "#include <stdbool.h>\n"
            "#include <string.h>\n"
            "#include <limits.h>\n"
            "#include <stdlib.h>\n"
            "#include <stddef.h>\n"
            "#include <stdio.h>\n"
            "#include <stdint.h>\n"
            "\n"
            "typedef enum {\n"
            "    SCLIP_STRING,\n"
            "    SCLIP_LONG,\n"
            "    SCLIP_DOUBLE,\n"
            "    SCLIP_PRESENCE,\n"
            "    SCLIP_STDIN,\n"
            "} sclip_option_type;\n"
            "\n"
            "typedef union {\n"
            "    long numeric;\n"
            "    double real;\n"
            "    const char *string;\n"
            "} sclip_value;\n"
            "\n"
            "typedef struct\n"
            "{\n"
            "    const char *long_opt;\n"
            "    const char *short_opt;\n"
            "    const sclip_option_type type;\n"
            "    sclip_value value;\n"
            "    const bool optional;\n"
            "} sclip_option;\n\n",
        .generator = NULL },
    { .static_content = NULL, .generator = sclip_generate_options_enum_decl },
    { .static_content = NULL, .generator = sclip_generate_options_declaration },
    { .static_content =
            "#define sclip_parse(argc, argv) \\n"
            "    __sclip_parse(argc, argv, &SCLIP_OPTIONS[0])\n"
            "static inline void __sclip_parse(int argc, const char **argv, sclip_option *restrict options);\n"
            "static inline bool sclip_is_opt(const char *arg);\n"
            "static inline bool sclip_opt_matches(const char *arg, sclip_option *restrict option);\n"
            "static inline sclip_value sclip_opt_parse_long(const char *arg);\n"
            "static inline sclip_value sclip_opt_parse_double(const char *arg);\n"
            "static inline double sclip_opt_get_value_double(const sclip_option *restrict options, const sclip_option_id id);\n"
            "static inline long sclip_opt_get_value_long(const sclip_option *restrict options, const sclip_option_id id);\n"
            "static inline bool sclip_opt_get_value_bool(const sclip_option *restrict options, const sclip_option_id id);\n"
            "static inline const char *sclip_opt_get_value_string(const sclip_option *restrict options, const sclip_option_id id);\n"
            "static inline bool sclip_opt_is_provided(const sclip_option *restrict options, const sclip_option_id id);\n\n",
        .generator = NULL },
    { .static_content = NULL, .generator = sclip_generator_presence_checkers },
    { .static_content = NULL, .generator = sclip_generator_value_getters },
    { .static_content =
            "#ifdef __cplusplus\n"
            "extern \"C\" {\n"
            "#endif\n"
            "\n"
            "#include <assert.h>\n"
            "\n"
            "static inline double sclip_opt_get_value_double(const sclip_option *restrict options, const sclip_option_id id)\n"
            "{\n"
            "    return options[id].value.real;\n"
            "}\n"
            "\n"
            "static inline long sclip_opt_get_value_long(const sclip_option *restrict options, const sclip_option_id id)\n"
            "{\n"
            "    return options[id].value.numeric;\n"
            "}\n"
            "\n"
            "static inline bool sclip_opt_get_value_bool(const sclip_option *restrict options, const sclip_option_id id)\n"
            "{\n"
            "    return options[id].value.numeric == 1;\n"
            "}\n"
            "\n"
            "static inline const char *sclip_opt_get_value_string(const sclip_option *restrict options, const sclip_option_id id)\n"
            "{\n"
            "    return options[id].value.string;\n"
            "}\n"
            "\n"
            "static inline bool sclip_opt_is_provided(const sclip_option *restrict options, const sclip_option_id id)\n"
            "{\n"
            "    return options[id].value.numeric == LONG_MIN;\n"
            "}\n"
            "\n"
            "static inline bool sclip_is_opt(const char *arg)\n"
            "{\n"
            "    if (arg == NULL || strlen(arg) < 2)\n"
            "        return false;\n"
            "    else if (arg[0] == '-' && arg[1] == '-')\n"
            "        return true;\n"
            "    else if (arg[0] == '-' && arg[1] != '-')\n"
            "        return true;\n"
            "    return false;\n"
            "}\n"
            "\n"
            "static inline bool sclip_opt_matches(const char *arg, sclip_option *restrict option)\n"
            "{\n"
            "    assert(arg != NULL);\n"
            "    if (option->short_opt != NULL && strcmp(arg, option->short_opt) == 0)\n"
            "        return true;\n"
            "    else if (option->long_opt != NULL && strcmp(arg, option->long_opt) == 0)\n"
            "        return true;\n"
            "    return false;\n"
            "}\n"
            "\n"
            "static inline sclip_value sclip_opt_parse_long(const char *arg)\n"
            "{\n"
            "    assert(arg != NULL);\n"
            "    static const int base = 10;\n"
            "    char *end_ptr = NULL;\n"
            "    const long ret = strtol(arg, &end_ptr, base);\n"
            "    if (end_ptr == arg) return (sclip_value){ .numeric = LONG_MIN };\n"
            "    return (sclip_value){ .numeric = ret };\n"
            "}\n"
            "\n"
            "static inline sclip_value sclip_opt_parse_double(const char *arg)\n"
            "{\n"
            "    assert(arg != NULL);\n"
            "    char *end_ptr = NULL;\n"
            "    const double ret = strtod(arg, &end_ptr);\n"
            "    if (end_ptr == arg) return (sclip_value){ .numeric = LONG_MIN };\n"
            "    return (sclip_value){ .real = ret };\n"
            "}\n"
            "\n"
            "static inline void __sclip_parse(int argc, const char **argv, sclip_option *restrict options)\n"
            "{\n"
            "    for (register int j = 0; j < SCLIP_OPTION_HELP_MENU_ID; j++) {\n"
            "        for (register int i = 1; i < argc; i++) {\n"
            "            if (!sclip_is_opt(argv[i])) continue;\n"
            "            if (!sclip_opt_matches(argv[i], &options[j])) continue;\n"
            "            switch (options[j].type) {\n"
            "            case SCLIP_STRING: {\n"
            "                options[j].value = (sclip_value){ .string = argv[i + 1] };\n"
            "            } break;\n"
            "            case SCLIP_LONG: {\n"
            "                options[j].value = sclip_opt_parse_long(argv[i + 1]);\n"
            "            } break;\n"
            "            case SCLIP_DOUBLE: {\n"
            "                options[j].value = sclip_opt_parse_double(argv[i + 1]);\n"
            "            } break;\n"
            "            case SCLIP_PRESENCE: {\n"
            "                options[j].value = (sclip_value){ .numeric = 1 };\n"
            "            } break;\n"
            "            default: {\n"
            "                options[j].value = (sclip_value){ .numeric = LONG_MIN };\n"
            "            } break;\n"
            "            }\n"
            "        }\n"
            "        if (!options[j].optional && options[j].value.numeric == LONG_MIN) {\n"
            // "            fprintf(stderr, \"Mandatory option/value %s, %s was not provided\\nRefer to --help, -h\\n\", options[j].long_opt, options[j].short_opt);\n"
            "            exit(EXIT_FAILURE);\n"
            "        }\n"
            "    }\n"
            "}\n"
            "\n"
            "#ifdef __cplusplus\n"
            "}// extern \"C\"\n"
            "#endif\n"
            "\n"
            "#endif\n"
            "#endif\n",
        .generator = NULL },
};

#define sclip_generate(config, file) \
    __sclip_generate(config, file, &SCLIP_GENERATION_SCENARIO[0], sizeof(SCLIP_GENERATION_SCENARIO) / sizeof(sclip_scenario_item));
static inline void __sclip_generate(const sclip_config *const restrict config, FILE *file, const sclip_scenario_item *scenario, const size_t scenarion_items_cnt);

#ifdef SCLIP_UTILS_IMPL

#include <ctype.h>
#define SAA_IMPL
#include <saa/saa.h>

static inline void __sclip_generate(const sclip_config *const restrict config, FILE *file, const sclip_scenario_item *scenario, const size_t scenarion_items_cnt)
{
    for (register size_t i = 0; i < scenarion_items_cnt; i++) {
        if (scenario[i].generator == NULL) {
            fprintf(file, scenario[i].static_content);
            continue;
        }
        scenario[i].generator(config, file);
    }
}

static inline void sclip_generator_header_guard(const sclip_config *config, FILE *file)
{
    (void)config;
    static const char *header_guard_template =
        "#ifndef SCLIP_%s_H\n"
        "#define SCLIP_%s_H\n\n";
    fprintf(file, header_guard_template, "put the name here later", "put the name here later");
}

static inline sclip_option *sclip_config_allocate_option(sclip_config *config)
{
    sclip_option **option = NULL;
    for (option = &config->options; *option != NULL; option = &((*option)->next)) {}
    *option = malloc(sizeof(**option));
    config->count++;
    return *option;
}

static inline void sclip_config_destroy(sclip_config *config)
{
    sclip_option *option = config->options;
    while (option != NULL) {
        sclip_option *tmp = option->next;
        free(option);
        option = tmp;
    }
}

static inline cJSON *sclip_read_config(const char *filepath)
{
    cJSON *ret = NULL;
    char *file_contents = sclip_read_file(filepath);
    ret = cJSON_Parse(file_contents);
    free(file_contents);
    if (ret == NULL) {
        sclip_print_cjson_error();
        return NULL;
    }
    return ret;
}

static inline bool sclip_parse_options(const cJSON *const json_config, sclip_config *config)
{
    const cJSON *const options = cJSON_GetObjectItem(json_config, "options");
    if (options == NULL || !cJSON_IsArray(options)) {
        return false;
    }
    for (int i = 0; i < cJSON_GetArraySize(options); i++) {
        cJSON *item = cJSON_GetArrayItem(options, i);
        if (item != NULL) {
            sclip_option *option = sclip_config_allocate_option(config);
            for (cJSON *attribute = item->child; attribute != NULL; attribute = attribute->next) {
                if (strcmp(attribute->string, "long-opt") == 0) {
                    option->long_opt = cJSON_GetStringValue(attribute);
                } else if (strcmp(attribute->string, "short-opt") == 0) {
                    option->short_opt = cJSON_GetStringValue(attribute);
                } else if (strcmp(attribute->string, "value-type") == 0) {
                    option->value_type = cJSON_GetStringValue(attribute);
                } else if (strcmp(attribute->string, "name") == 0) {
                    option->name = cJSON_GetStringValue(attribute);
                } else if (strcmp(attribute->string, "optional") == 0) {
                    if (cJSON_IsBool(attribute)) {
                        option->optional = (bool)attribute->valueint;
                        continue;
                    }
                }
            }
        }
    }
    return true;
}

static inline char *sclip_read_file(const char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    char *buffer = NULL;
    size_t file_size = 0;
    size_t bytes_read = 0;

    if (!file) {
        perror("Failed to open file");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer = malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[file_size] = '\0';
    fclose(file);
    return buffer;
}

static inline void sclip_print_cjson_error()
{
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) { fprintf(stderr, "Error before: %s\n", error_ptr); }
}

static inline bool sclip_is_config_valid(const cJSON *config)
{
    (void)config;
    return false;
}

static inline const char *__sclip_null_filter(const char *const restrict string)
{
    return string == NULL ? "NULL" : string;
}

static inline char *__sclip_to_upper(char *restrict string)
{
    if (string == NULL) return NULL;
    for (size_t i = 0; i < strlen(string); i++) {
        string[i] = toupper(string[i]);
    }
    return string;
}

static inline const char *__sclip_type_converter(const char *const restrict string)
{
    static const char *sclip_config_value_types[] = {
        "long",
        "double",
        "string",
        NULL
    };
    static const char *sclip_value_types[] = {
        "SCLIP_LONG",
        "SCLIP_DOUBLE",
        "SCLIP_STRING",
        NULL
    };
    if (string == NULL) return NULL;
    for (size_t i = 0; sclip_config_value_types[i] != NULL; i++) {
        if (strcmp(string, sclip_config_value_types[i]) == 0) return sclip_value_types[i];
    }
    return NULL;
}

static inline void sclip_generate_options_enum_decl(const sclip_config *config, FILE *file)
{
    static const size_t page_size = 512;
    static const char *sclip_options_enum_decl = "typedef enum {\n";
    saa_arena arena = saa_arena_create(page_size);
    fprintf(file, sclip_options_enum_decl);
    for (sclip_option *option = config->options; option != NULL; option = option->next) {
        fprintf(file, "    SCLIP_OPTION_%s_ID,\n", __sclip_to_upper(saa_arena_push_value_string(&arena, option->name)));
    }
    fprintf(file, "    SCLIP_OPTION_HELP_MENU_ID\n} sclip_option_id\n\n");
    saa_arena_destroy(&arena);
}

static inline void sclip_generate_options_declaration(const sclip_config *config, FILE *file)
{
    static const size_t page_size = 512;
    static const char *sclip_options_decl = "static sclip_option SCLIP_OPTIONS[] = {\n";
    saa_arena arena = saa_arena_create(page_size);
    fprintf(file, sclip_options_decl);
    for (sclip_option *option = config->options; option != NULL; option = option->next) {
        fprintf(file, "    [SCLIP_OPTION_%s_ID] = { ", __sclip_to_upper(saa_arena_push_value_string(&arena, option->name)));
        fprintf(file, ".long_opt = %s, ", saa_arena_push_value_strings(&arena, "\"", option->long_opt, "\""));
        fprintf(file, ".short_opt = %s, ", saa_arena_push_value_strings(&arena, "\"", option->short_opt, "\""));
        fprintf(file, ".type = %s, ", __sclip_type_converter(option->value_type));
        fprintf(file, ".optional = %s, ", option->optional ? "true" : "false");
        fprintf(file, ".value = { .numeric = LONG_MIN } },\n");
    }
    fprintf(file, "};\n\n");
    saa_arena_destroy(&arena);
}

static inline void sclip_generator_value_getters(const sclip_config *config, FILE *file)
{
    static const size_t page_size = 512;
    saa_arena arena = saa_arena_create(page_size);
    for (sclip_option *option = config->options; option != NULL; option = option->next) {
        fprintf(file, "#define sclip_opt_%s_get_value() \\\n", option->name);
        fprintf(file, "    sclip_opt_get_value_%s(&SCLIP_OPTIONS[0], SCLIP_OPTION_%s_ID)\n", option->value_type, __sclip_to_upper(saa_arena_push_value_string(&arena, option->name)));
    }
    fprintf(file, "\n");
    saa_arena_destroy(&arena);
}

static inline void sclip_generator_presence_checkers(const sclip_config *config, FILE *file)
{
    static const size_t page_size = 512;
    saa_arena arena = saa_arena_create(page_size);
    for (sclip_option *option = config->options; option != NULL; option = option->next) {
        fprintf(file, "#define sclip_opt_%s_is_provided() \\\n", option->name);
        fprintf(file, "    sclip_opt_is_provided(&SCLIP_OPTIONS[0], SCLIP_OPTION_%s_ID)\n", __sclip_to_upper(saa_arena_push_value_string(&arena, option->name)));
    }
    fprintf(file, "\n");
    saa_arena_destroy(&arena);
}

typedef struct
{
    int arr_index;
    bool name_valid;
    bool long_opt_valid;
    bool short_opt_valid;
    bool value_type_valid;
    bool optional_valid;
} sclip_verify_res;

static inline bool sclip_verify_config_attribute_name(const char *const name_str)
{
    static const char *forbidden_symbols = " /|,.!@#$%^&*()-=+[]{}`~\n\r\t";
    if (name_str == NULL) return false;
    for (size_t i = 0; i < strlen(name_str); i++) {
        for (size_t j = 0; j < strlen(forbidden_symbols); j++) {
            if (name_str[i] == forbidden_symbols[j]) return false;
        }
    }
    return true;
}

static inline bool sclip_verify_config_attribute_short_opt(const char *const short_opt_str)
{
    static const char *forbidden_symbols = " /|,.!@#$%^&*()=+[]{}`~\n\r\t";
    if (short_opt_str == NULL && strlen(short_opt_str) >= 2) return false;
    if (short_opt_str[0] != '-') return false;
    for (size_t i = 0; i < strlen(short_opt_str); i++) {
        for (size_t j = 0; j < strlen(forbidden_symbols); j++) {
            if (short_opt_str[i] == forbidden_symbols[j]) return false;
        }
    }
    return true;
}

static inline bool sclip_verify_config_attribute_long_opt(const char *const long_opt_str)
{
    static const char *forbidden_symbols = " /|,.!@#$%^&*()=+[]{}`~\n\r\t";
    if (long_opt_str == NULL && strlen(long_opt_str) > 5) return false;
    if (long_opt_str[0] != '-' && long_opt_str[1] != '-') return false;
    for (size_t i = 0; i < strlen(long_opt_str); i++) {
        for (size_t j = 0; j < strlen(forbidden_symbols); j++) {
            if (long_opt_str[i] == forbidden_symbols[j]) return false;
        }
    }
    return true;
}

static inline bool sclip_verify_config_attribute_value_type(const char *const value_type_str)
{
    static const char *sclip_acceptable_value_types[] = {
        "long",
        "double",
        NULL
    };
    if (value_type_str == NULL) return false;
    for (size_t i = 0; i < strlen(value_type_str); i++) {
        for (size_t j = 0; sclip_acceptable_value_types[j] != NULL; j++) {
            if (strcmp(value_type_str, sclip_acceptable_value_types[j]) == 0) return true;
        }
    }
    return false;
}


#endif// SCLIP_UTILS_IMPL
#endif// SCLIP_UTILS_H
