#define SCLIP_UTILS_IMPL
#include <sclip/sclip-utils.h>

#define SCLIP_IMPL
#include "sclip.h"

int main(int argc, const char **argv)
{
    sclip_parse(argc, argv);
    FILE *const output_file = fopen(sclip_opt_output_get_value(), "w");
    sclip_config config = { 0 };
    if (sclip_config_create_from_file(&config, sclip_opt_input_get_value()) < 0) return EXIT_FAILURE;
    if (!output_file) return EXIT_FAILURE;
    sclip_generate(&config, output_file);

    fclose(output_file);
    return EXIT_SUCCESS;
}
