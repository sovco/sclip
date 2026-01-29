#ifndef SCLIP_UTILS_H
#define SCLIP_UTILS_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cjson/cJSON.h>

#include "sclip-statics.h"

typedef enum {
    SCLIP_UTILS_STRING = 0,
    SCLIP_UTILS_LONG,
    SCLIP_UTILS_DOUBLE,
    SCLIP_UTILS_BOOL,
    SCLIP_UTILS_STDIN,
    SCLIP_UTILS_NONTYPE,
} sclip_utils_option_type;

static const char *sclip_config_value_types[] = {
    [SCLIP_UTILS_STRING] = "string",
    [SCLIP_UTILS_LONG] = "long",
    [SCLIP_UTILS_DOUBLE] = "double",
    [SCLIP_UTILS_BOOL] = "bool",
    [SCLIP_UTILS_STDIN] = "stdin",
    NULL
};

static const char *sclip_value_help_menu[] = {
    [SCLIP_UTILS_STRING] = "<string>",
    [SCLIP_UTILS_LONG] = "<long>  ",
    [SCLIP_UTILS_DOUBLE] = "<double>",
    [SCLIP_UTILS_BOOL] = "<bool>  ",
    [SCLIP_UTILS_STDIN] = "<stdin> ",
    NULL
};

static const char *sclip_value_types[] = {
    [SCLIP_UTILS_STRING] = "SCLIP_STRING",
    [SCLIP_UTILS_LONG] = "SCLIP_LONG",
    [SCLIP_UTILS_DOUBLE] = "SCLIP_DOUBLE",
    [SCLIP_UTILS_BOOL] = "SCLIP_BOOL",
    [SCLIP_UTILS_STDIN] = "SCLIP_STDIN",
    NULL
};

static const char *sclip_getter_tail[] = {
    [SCLIP_UTILS_STRING] = "string",
    [SCLIP_UTILS_LONG] = "long",
    [SCLIP_UTILS_DOUBLE] = "double",
    [SCLIP_UTILS_BOOL] = "bool",
    [SCLIP_UTILS_STDIN] = "string",
    NULL
};

typedef struct sclip_utils_option_t sclip_utils_option;
typedef struct sclip_config_t sclip_config;
typedef struct sclip_scenario_item_t sclip_scenario_item;

struct sclip_utils_option_t
{
    sclip_utils_option *next;
    const char *long_opt;
    const char *short_opt;
    const char *name;
    const char *description;
    const sclip_utils_option_type value_type;
    bool optional;
    bool system_option;
};

struct sclip_config_t
{
    sclip_utils_option *options;
    const char *project;
    const char *version;
};

struct sclip_scenario_item_t
{
    const char *static_content;
    void (*generator)(const sclip_config *config, FILE *file);
};

static inline int sclip_config_create_from_file(sclip_config *restrict config, const char *const restrict filepath);
static inline int sclip_config_create_from_string(sclip_config *restrict config, const char *const restrict json_string);
static inline int sclip_config_create_from_json(sclip_config *restrict config, const cJSON *const restrict json);

#define sclip_parse_options(config, json) \
    __sclip_parse_options(config, json, sclip_config_value_types)
static inline int __sclip_parse_options(sclip_config *restrict config, const cJSON *const restrict json, const char **config_value_types);

static inline int sclip_parse_string_attribute(const cJSON *const node, const char *tag, char **restrict ret);
static inline int sclip_parse_bool_attribute(const cJSON *const node, const char *tag, bool *restrict ret);

static inline sclip_utils_option_type sclip_deduct_type(const char *type_str, const char **known_types);

static inline sclip_utils_option *sclip_config_allocate_option(sclip_config *config, const char *name, const char *short_opt, const char *long_opt, const char *description, const sclip_utils_option_type type, const bool optional, const bool system_option);
static inline void sclip_config_destroy(sclip_config *options);
static inline char *sclip_read_file(const char *filepath);
static inline cJSON *sclip_read_config(const char *filepath);
static inline void sclip_print_cjson_error();

static inline void sclip_generate_options_enum_decl(const sclip_config *config, FILE *file);
static inline void sclip_generate_options_declaration(const sclip_config *config, FILE *file);
static inline void sclip_generator_value_getters(const sclip_config *config, FILE *file);
static inline void sclip_generator_presence_checkers(const sclip_config *config, FILE *file);
static inline void sclip_generate_help_string(const sclip_config *config, FILE *file);
static inline void sclip_generate_version_string(const sclip_config *config, FILE *file);

static const sclip_scenario_item SCLIP_GENERATION_SCENARIO[] = {
    { .static_content = __SCLIP_STATIC_BLOCK_INCLUDES_DEFS,
        .generator = NULL },
    { .static_content = NULL, .generator = sclip_generate_help_string },
    { .static_content = NULL, .generator = sclip_generate_version_string },
    { .static_content = NULL, .generator = sclip_generate_options_enum_decl },
    { .static_content = NULL, .generator = sclip_generate_options_declaration },
    { .static_content = __SCLIP_STATIC_BLOCK_FUNC_DECL,
        .generator = NULL },
    { .static_content = NULL, .generator = sclip_generator_presence_checkers },
    { .static_content = NULL, .generator = sclip_generator_value_getters },
    { .static_content = __SCLIP_STATIC_BLOCK_FUNC_BODY,
        .generator = NULL },
};

#define sclip_generate(config, file) \
    __sclip_generate(config, file, &SCLIP_GENERATION_SCENARIO[0], sizeof(SCLIP_GENERATION_SCENARIO) / sizeof(sclip_scenario_item));
static inline void __sclip_generate(const sclip_config *const restrict config, FILE *file, const sclip_scenario_item *scenario, const size_t scenarion_items_cnt);

#ifdef SCLIP_UTILS_IMPL

#include <ctype.h>
#include <assert.h>
#include <regex.h>

#define SAA_IMPL
#include <saa/saa.h>

static inline char *__sclip_copy_string(const char *const restrict str)
{
    if (str == NULL) return NULL;
    char *ret = malloc(strlen(str) + 1);
    memcpy(ret, str, strlen(str) + 1);
    return ret;
}

static inline int sclip_config_create_from_file(sclip_config *restrict config, const char *const restrict filepath)
{
    if (filepath == NULL) return -1;
    char *file_contents = sclip_read_file(filepath);
    if (file_contents == NULL) return -1;
    int res = sclip_config_create_from_string(config, file_contents);
    free(file_contents);
    return res;
}

static inline int sclip_config_create_from_string(sclip_config *restrict config, const char *const restrict json_string)
{
    if (json_string == NULL) return -1;
    cJSON *json_config = cJSON_Parse(json_string);
    if (json_config == NULL) {
        sclip_print_cjson_error();
        return -1;
    }
    const int res = sclip_config_create_from_json(config, json_config);
    cJSON_Delete(json_config);
    return res;
}

static inline int sclip_config_create_from_json(sclip_config *restrict config, const cJSON *const restrict json)
{
    if (json == NULL) return -1;
    char *project_str = NULL;
    char *version_str = NULL;
    if (sclip_parse_string_attribute(json, "project", &project_str) != 0) return -1;
    if (sclip_parse_string_attribute(json, "version", &version_str) != 0) return -1;
    const sclip_config tmp = { .project = __sclip_copy_string(project_str), .version = __sclip_copy_string(version_str), .options = NULL };
    memcpy(config, &tmp, sizeof(*config));
    return sclip_parse_options(config, json);
}

static inline int __sclip_parse_options(sclip_config *restrict config, const cJSON *const restrict json, const char **config_value_types)
{
    const cJSON *const options = cJSON_GetObjectItem(json, "options");
    if (options == NULL || !cJSON_IsArray(options)) {
        return -1;
    }
    for (int i = 0; i < cJSON_GetArraySize(options); i++) {
        cJSON *array_item = cJSON_GetArrayItem(options, i);
        if (array_item != NULL) {
            char *name = NULL;
            char *long_opt = NULL;
            char *short_opt = NULL;
            char *description = NULL;
            char *value_type_string = NULL;
            bool is_optional = false;
            sclip_utils_option_type type = SCLIP_UTILS_NONTYPE;
            if (sclip_parse_string_attribute(array_item, "name", &name) != 0) {
                return -1;
            }
            (void)sclip_parse_string_attribute(array_item, "long-opt", &long_opt);
            (void)sclip_parse_string_attribute(array_item, "short-opt", &short_opt);
            if (long_opt == NULL && short_opt == NULL) {
                return -1;
            }
            if (sclip_parse_string_attribute(array_item, "value-type", &value_type_string) != 0) {
                return -1;
            }
            if ((type = sclip_deduct_type(value_type_string, config_value_types)) == SCLIP_UTILS_NONTYPE) {
                return -1;
            }
            if (sclip_parse_string_attribute(array_item, "description", &description) != 0) {
                return -1;
            }
            if (sclip_parse_bool_attribute(array_item, "optional", &is_optional) != 0) {
                return -1;
            }
            if (sclip_config_allocate_option(config, __sclip_copy_string(name), __sclip_copy_string(short_opt), __sclip_copy_string(long_opt), __sclip_copy_string(description), type, is_optional, false) == NULL) {
                return -1;
            }
        }
    }
    if (sclip_config_allocate_option(config, __sclip_copy_string("help"), __sclip_copy_string("-h"), __sclip_copy_string("--help"), __sclip_copy_string("Shows help menu"), SCLIP_UTILS_BOOL, true, true) == NULL) {
        return -1;
    }
    if (sclip_config_allocate_option(config, __sclip_copy_string("version"), __sclip_copy_string("-v"), __sclip_copy_string("--version"), __sclip_copy_string("Shows version string"), SCLIP_UTILS_BOOL, true, true) == NULL) {
        return -1;
    }
    return 0;
}

static inline int sclip_parse_string_attribute(const cJSON *const node, const char *tag, char **restrict ret)
{
    cJSON *field = NULL;
    if ((field = cJSON_GetObjectItem(node, tag)) == NULL) {
        return -1;
    }
    if (!cJSON_IsString(field)) {
        return -1;
    }
    *ret = cJSON_GetStringValue(field);
    if (*ret == NULL || strlen(*ret) == 0) {
        *ret = NULL;
        return -1;
    }
    return 0;
}

static inline int sclip_parse_bool_attribute(const cJSON *const node, const char *tag, bool *restrict ret)
{
    cJSON *field = NULL;
    if ((field = cJSON_GetObjectItem(node, tag)) == NULL) {
        return -1;
    }
    if (!cJSON_IsBool(field)) {
        return -1;
    }
    *ret = (bool)field->valueint;
    return 0;
}

static inline void __sclip_generate(const sclip_config *const restrict config, FILE *file, const sclip_scenario_item *scenario, const size_t scenarion_items_cnt)
{
    for (register size_t i = 0; i < scenarion_items_cnt; i++) {
        if (scenario[i].generator == NULL) {
            fputs(scenario[i].static_content, file);
            continue;
        }
        scenario[i].generator(config, file);
    }
}

static inline sclip_utils_option *sclip_config_allocate_option(sclip_config *config, const char *name, const char *short_opt, const char *long_opt, const char *description, const sclip_utils_option_type type, const bool optional, const bool system_option)
{
    sclip_utils_option **option = NULL;
    for (option = &config->options; *option != NULL; option = &((*option)->next)) {}
    *option = malloc(sizeof(**option));
    if (*option == NULL) return NULL;
    const sclip_utils_option tmp = (const sclip_utils_option){ .next = NULL, .name = name, .short_opt = short_opt, .long_opt = long_opt, .description = description, .value_type = type, .optional = optional, .system_option = system_option };
    memcpy(*option, &tmp, sizeof(tmp));
    return *option;
}

static inline void sclip_config_destroy(sclip_config *config)
{
    free((char *)config->version);
    free((char *)config->project);
    sclip_utils_option *option = config->options;
    while (option != NULL) {
        sclip_utils_option *tmp = option->next;
        free((char *)option->name);
        free((char *)option->long_opt);
        free((char *)option->short_opt);
        free((char *)option->description);
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

static inline char *__sclip_to_upper(char *restrict string)
{
    if (string == NULL) return NULL;
    for (size_t i = 0; i < strlen(string); i++) {
        string[i] = toupper(string[i]);
    }
    return string;
}

static inline sclip_utils_option_type sclip_deduct_type(const char *type_str, const char **known_types)
{
    if (type_str == NULL) return SCLIP_UTILS_NONTYPE;
    for (size_t i = 0; known_types[i] != NULL; i++) {
        if (strcmp(type_str, known_types[i]) == 0) return (sclip_utils_option_type)i;
    }
    return SCLIP_UTILS_NONTYPE;
}

static inline void sclip_generate_options_enum_decl(const sclip_config *config, FILE *file)
{
    static const size_t page_size = 512;
    static const char *sclip_utils_options_enum_decl = "typedef enum {\n";
    saa_arena arena = saa_arena_create(page_size);
    fprintf(file, sclip_utils_options_enum_decl);
    for (sclip_utils_option *option = config->options; option != NULL; option = option->next) {
        if (option->system_option) continue;
        fprintf(file, "    SCLIP_OPTION_%s_ID,\n", __sclip_to_upper(saa_arena_push_value_string(&arena, option->name)));
    }
    fputs("    SCLIP_OPTION_HELP_ID,\n", file);
    fputs("    SCLIP_OPTION_VERSION_ID\n} sclip_option_id;\n\n", file);
    saa_arena_destroy(&arena);
}

static inline void sclip_generate_options_declaration(const sclip_config *config, FILE *file)
{
    static const size_t page_size = 512;
    static const char *sclip_utils_options_decl = "static sclip_option SCLIP_OPTIONS[] = {\n";
    saa_arena arena = saa_arena_create(page_size);
    fprintf(file, sclip_utils_options_decl);
    for (sclip_utils_option *option = config->options; option != NULL; option = option->next) {
        if (option->system_option) continue;
        fprintf(file, "    [SCLIP_OPTION_%s_ID] = { ", __sclip_to_upper(saa_arena_push_value_string(&arena, option->name)));
        fprintf(file, ".long_opt = %s, ", option->long_opt != NULL ? saa_arena_push_value_strings(&arena, "\"", option->long_opt, "\"") : "\"\"");
        fprintf(file, ".short_opt = %s, ", option->short_opt != NULL ? saa_arena_push_value_strings(&arena, "\"", option->short_opt, "\"") : "\"\"");
        fprintf(file, ".type = %s, ", sclip_value_types[option->value_type]);
        fprintf(file, ".optional = %s, ", option->optional ? "true" : "false");
        fprintf(file, ".value = { .numeric = LONG_MIN } },\n");
    }
    fprintf(file, "    [SCLIP_OPTION_HELP_ID] = { .long_opt = \"--help\", .short_opt = \"-h\", .type = SCLIP_BOOL, .optional = true, .value = { .string = SCLIP_HELP_STR } },\n");
    fprintf(file, "    [SCLIP_OPTION_VERSION_ID] = { .long_opt = \"--version\", .short_opt = \"-v\", .type = SCLIP_BOOL, .optional = true, .value = { .string = SCLIP_VERSION_STR } }\n");
    fprintf(file, "};\n\n");
    saa_arena_destroy(&arena);
}

static inline size_t __sclip_determine_help_width(const sclip_config *const restrict config)
{
    size_t ret = 0;
    for (sclip_utils_option *option = config->options; option != NULL; option = option->next) {
        if (option->long_opt == NULL) continue;
        size_t long_opt_len = strlen(option->long_opt);
        if (long_opt_len > ret) {
            ret = long_opt_len;
        }
    }
    return ret;
}

static inline char *__sclip_make_padding(const saa_arena *restrict arena, const char symbol, const size_t len)
{
    char *ret = saa_arena_push(arena, len + 1);
    memset(ret, symbol, len);
    ret[len] = '\0';
    return ret;
}

static inline char **__sclip_split_description(const saa_arena *const restrict arena, const char *description, const size_t description_max_len)
{
    const size_t len = strlen(description);
    const size_t initial_lines_cnt = sizeof(char *) * (len / description_max_len) * 5;
    char **res = (char **)saa_arena_push(arena, initial_lines_cnt);
    char **res_tmp = res;

    size_t last_stop = 0;
    size_t look_forward = description_max_len;
    while (look_forward <= len) {
        for (; look_forward > 0; look_forward--) {
            if (description[look_forward] == ' ') {
                *(res_tmp++) = saa_arena_push_arbitrary(arena, description + last_stop, look_forward - last_stop);
                (void)saa_arena_push_value_string(arena, "");
                last_stop = look_forward;
                look_forward = look_forward + description_max_len;
                break;
            }
        }
    }
    *(res_tmp++) = saa_arena_push_arbitrary(arena, description + last_stop, len - last_stop);
    (void)saa_arena_push_value_string(arena, "");
    *res_tmp = NULL;
    return res;
}

static inline void sclip_generate_help_string(const sclip_config *config, FILE *file)
{
    static const size_t page_size = 1024;
    static const size_t description_max_len = 50;
    saa_arena arena = saa_arena_create(page_size);
    size_t max_opt_len = __sclip_determine_help_width(config);
    fputs("#define SCLIP_HELP_STR \\\n", file);
    fputs("\"Usage:\\n\"\\\n", file);
    fprintf(file, "\"%s [options]\\n\"\\\n", config->project);
    for (sclip_utils_option *option = config->options; option != NULL; option = option->next) {
        const char *const short_opt = !option->short_opt ? "  " : option->short_opt;
        const char *const long_opt = !option->long_opt ? "" : option->long_opt;
        const char *const option_separator = option->long_opt != NULL && option->short_opt != NULL ? ", " : "  ";
        const char *const padding_long_opt = __sclip_make_padding(&arena, ' ', max_opt_len - strlen(long_opt) + 1);
        const char *const option_line = saa_arena_push_value_strings(&arena, "\"    ", short_opt, option_separator, long_opt, padding_long_opt, sclip_value_help_menu[option->value_type], "    ");
        fputs(option_line, file);

        const size_t description_len = strlen(option->description);
        if (description_len <= description_max_len) {
            fprintf(file, "%s \\n\"\\\n", option->description);
            continue;
        }
        char **split = __sclip_split_description(&arena, option->description, description_max_len);
        const char *const padding_before_description = __sclip_make_padding(&arena, ' ', strlen(option_line) - 2);
        for (int i = 0; split[i] != NULL; i++) {
            if (i == 0) {
                fprintf(file, "%s \\n\"\\\n", split[i]);
                continue;
            }
            fprintf(file, "\"%s%s \\n\"\\\n", padding_before_description, split[i]);
        }
    }
    fputs("\"\"\n\n", file);
    saa_arena_destroy(&arena);
}

static inline void sclip_generate_version_string(const sclip_config *config, FILE *file)
{
    fprintf(file, "#define SCLIP_VERSION_STR \"%s %s\\n\"\n\n", config->project, config->version);
}

static inline void sclip_generator_value_getters(const sclip_config *config, FILE *file)
{
    static const size_t page_size = 512;
    saa_arena arena = saa_arena_create(page_size);
    for (sclip_utils_option *option = config->options; option != NULL; option = option->next) {
        if (option->system_option) continue;
        fprintf(file, "#define sclip_opt_%s_get_value() \\\n", option->name);
        fprintf(file, "    sclip_opt_get_value_%s(&SCLIP_OPTIONS[0], SCLIP_OPTION_%s_ID)\n", sclip_getter_tail[option->value_type], __sclip_to_upper(saa_arena_push_value_string(&arena, option->name)));
    }
    fprintf(file, "\n");
    saa_arena_destroy(&arena);
}

static inline void sclip_generator_presence_checkers(const sclip_config *config, FILE *file)
{
    static const size_t page_size = 512;
    saa_arena arena = saa_arena_create(page_size);
    for (sclip_utils_option *option = config->options; option != NULL; option = option->next) {
        if (option->system_option) continue;
        fprintf(file, "#define sclip_opt_%s_is_provided() \\\n", option->name);
        fprintf(file, "    sclip_opt_is_provided(&SCLIP_OPTIONS[0], SCLIP_OPTION_%s_ID)\n", __sclip_to_upper(saa_arena_push_value_string(&arena, option->name)));
    }
    fprintf(file, "\n");
    saa_arena_destroy(&arena);
}

#endif// SCLIP_UTILS_IMPL
#endif// SCLIP_UTILS_H
