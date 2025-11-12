#include <stdlib.h>
#include <stf/stf.h>

#define SCLIP_UTILS_IMPL
#include <sclip/sclip-utils.h>

static const char *TEST_CONFIG =
    "{"
    "    \"project\": \"test_project\","
    "    \"version\": \"1.0.1\","
    "    \"options\": ["
    "        {"
    "            \"name\": \"option_a\","
    "            \"value-type\": \"double\","
    "            \"long-opt\": \"--option\","
    "            \"short-opt\": \"-o\","
    "            \"optional\": false"
    "        },"
    "        {"
    "            \"name\": \"option_b\","
    "            \"value-type\": \"long\","
    "            \"long-opt\": \"--ass\","
    "            \"short-opt\": \"-a\","
    "            \"optional\": true"
    "        }"
    "    ]"
    "}";

STF_TEST_CASE(sclip, generate_options_list_from_config)
{
    cJSON *json_config = cJSON_Parse(TEST_CONFIG);
    STF_EXPECT(cJSON_GetObjectItem(json_config, "options") != NULL, .return_on_failure = true, .failure_msg = "json_configs options were not provided");
    STF_EXPECT(cJSON_IsArray(cJSON_GetObjectItem(json_config, "options")), .return_on_failure = true, .failure_msg = "json_configs options is not an array");
    STF_EXPECT(cJSON_GetArraySize(cJSON_GetObjectItem(json_config, "options")) == 2, .return_on_failure = true, .failure_msg = "json_configs child is supposed to be a array");
    sclip_config config = { 0 };
    STF_EXPECT(sclip_parse_options(json_config, &config) == true, .failure_msg = "unable to parse to config");
    sclip_generate(&config, stdout);
    sclip_config_destroy(&config);
    cJSON_Delete(json_config);
}

int main(int argc, const char **argv)
{
    (void)argc;
    (void)argv;
    return STF_RUN_TESTS();
}
