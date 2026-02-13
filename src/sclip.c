#define SCLIP_UTILS_IMPL
#include <sclip/sclip-utils.h>

#define SCLIP_IMPL
#include "sclip.h"

int main(int argc, const char **argv)
{
    if (sclip_parse(argc, argv) == SCLIP_PARSE_VERS_OR_HELP) {
        return 0;
    }
    FILE *output_file = NULL;
    if (sclip_opt_output_is_provided()) {
        output_file = fopen(sclip_opt_output_get_value(), "w");
    } else {
        output_file = stdout;
    }
    if (!output_file) return EXIT_FAILURE;
    sclip_config config = { 0 };
    if (!sclip_is_stdin_available() && !sclip_opt_input_is_provided()) {
        puts("No input was provided.");
        return EXIT_FAILURE;
    } else if (sclip_is_stdin_available()) {
        sclip_stdin_content contents = sclip_get_stdin_contents();
        if (sclip_config_create_from_string(&config, contents.data) < 0) return EXIT_FAILURE;
        sclip_free_stdin_content(&contents);
    } else if (sclip_opt_input_is_provided()) {
        if (sclip_config_create_from_file(&config, sclip_opt_input_get_value()) < 0) return EXIT_FAILURE;
    }
    sclip_generate(&config, output_file);
    sclip_config_destroy(&config);
    fclose(output_file);
    return EXIT_SUCCESS;
}
