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
    "            \"name\": \"path\","
    "            \"value-type\": \"string\","
    "            \"long-opt\": \"--path-long\","
    "            \"short-opt\": \"\","
    "            \"description\": \"this is asdasd some description about the option and what it does\","
    "            \"optional\": false"
    "        },"
    "        {"
    "            \"name\": \"file\","
    "            \"value-type\": \"string\","
    "            \"long-opt\": \"\","
    "            \"short-opt\": \"-f\","
    "            \"description\": \"this is some description about the option and what it does\","
    "            \"optional\": true"
    "        },"
    "        {"
    "            \"name\": \"flag\","
    "            \"value-type\": \"bool\","
    "            \"long-opt\": \"--flag\","
    "            \"short-opt\": \"-a\","
    "            \"description\": \"this is some description about the option and what it does\","
    "            \"optional\": true"
    "        }"
    "    ]"
    "}";

static const char *BUILD_OPTIONS =
    "{"
    "    \"project\": \"project-build\","
    "    \"version\": \"0.0.1\","
    "    \"options\": ["
    "        {"
    "            \"name\": \"build_tests\","
    "            \"value-type\": \"bool\","
    "            \"long-opt\": \"--build-tests\","
    "            \"short-opt\": \"-t\","
    "            \"description\": \"this optiona will enable test builds\","
    "            \"optional\": true"
    "        },"
    "        {"
    "            \"name\": \"enable_debug\","
    "            \"value-type\": \"bool\","
    "            \"long-opt\": \"--enable-debug\","
    "            \"short-opt\": \"-d\","
    "            \"description\": \"this options will build a debug release\","
    "            \"optional\": true"
    "        },"
    "        {"
    "            \"name\": \"number\","
    "            \"value-type\": \"long\","
    "            \"long-opt\": \"--nuber\","
    "            \"short-opt\": \"-n\","
    "            \"description\": \"this will just print a number\","
    "            \"optional\": true"
    "        }"
    "    ]"
    "}";

STF_TEST_CASE(sclip, able_to_create_config_from_json)
{
    cJSON *json_config = cJSON_Parse(TEST_CONFIG);
    STF_EXPECT(cJSON_GetObjectItem(json_config, "options") != NULL, .return_on_failure = true, .failure_msg = "json_configs options were not provided");
    STF_EXPECT(cJSON_IsArray(cJSON_GetObjectItem(json_config, "options")), .return_on_failure = true, .failure_msg = "json_configs options is not an array");
    STF_EXPECT(cJSON_GetArraySize(cJSON_GetObjectItem(json_config, "options")) == 3, .return_on_failure = true, .failure_msg = "json_configs child is supposed to be a array");
    sclip_config config = { 0 };
    STF_EXPECT(sclip_config_create_from_json(&config, json_config) == 0, .failure_msg = "unable to create config");
    sclip_config_destroy(&config);
    cJSON_Delete(json_config);
}

STF_TEST_CASE(sclip, able_to_create_config_from_json_string)
{
    sclip_config config = { 0 };
    STF_EXPECT(sclip_config_create_from_string(&config, TEST_CONFIG) == 0, .failure_msg = "unable to create config");
    sclip_config_destroy(&config);
}

STF_TEST_CASE(sclip, able_to_create_config_from_json_string_and_output_to_file)
{
    static const char *output_file_path = "project-build.h";
    FILE *file = fopen(output_file_path, "w");
    STF_EXPECT(file != NULL, .return_on_failure = true, .failure_msg = "unable to open file");
    sclip_config config = { 0 };
    STF_EXPECT(sclip_config_create_from_string(&config, BUILD_OPTIONS) == 0, .failure_msg = "unable to create config");
    sclip_generate(&config, file);
    sclip_config_destroy(&config);
    fclose(file);
}


int main(int argc, const char **argv)
{
    (void)argc;
    (void)argv;
    return STF_RUN_TESTS();
}
