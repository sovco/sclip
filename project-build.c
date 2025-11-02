#define NOB_IMPLEMENTATION
#include "nob.h"
#include <dirent.h>
#include <errno.h>

static inline bool file_exists(const char *path)
{
    return access(path, F_OK) == 0;
}

static inline bool dir_exists(const char *path)
{
    DIR *dir = opendir(path);
    if (dir) {
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
        return false;
    } else {
        return false;
    }
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    Nob_Cmd cmd = { 0 };

    nob_cmd_append(&cmd, "mkdir", "-p", "build/deps/stf");
    if (!nob_cmd_run(&cmd)) return 1;
    if (!file_exists("build/deps/stf/stf.h")) {
        nob_cmd_append(&cmd, "curl", "-Lo", "build/deps/stf/stf.h", "https://raw.githubusercontent.com/sovco/stf/refs/heads/master/include/stf/stf.h");
        if (!nob_cmd_run(&cmd)) return 1;
    }
    nob_cmd_append(&cmd, "mkdir", "-p", "build/deps/saa");
    if (!nob_cmd_run(&cmd)) return 1;
    if (!file_exists("build/deps/saa/saa.h")) {
        nob_cmd_append(&cmd, "curl", "-Lo", "build/deps/saa/saa.h", "https://raw.githubusercontent.com/sovco/saa/refs/heads/master/include/saa/saa.h");
        if (!nob_cmd_run(&cmd)) return 1;
    }
    if (!file_exists("build/deps/cjson/instal/lib/libcjson.a")) {
        nob_cmd_append(&cmd, "git", "clone", "https://github.com/DaveGamble/cJSON.git", "build/deps/cjson");
        if (!nob_cmd_run(&cmd)) return 1;
        nob_cmd_append(&cmd, "mkdir", "-p", "build/deps/cjson/build");
        if (!nob_cmd_run(&cmd)) return 1;
        nob_cmd_append(&cmd, "mkdir", "-p", "build/deps/cjson/instal");
        if (!nob_cmd_run(&cmd)) return 1;
        nob_cmd_append(&cmd, "cmake", "-DCMAKE_POLICY_VERSION_MINIMUM=3.5", "-DCMAKE_BUILD_TYPE=Release", "-DENABLE_CJSON_TEST=Off", "-DENABLE_CJSON_UTILS=Off", "-DENABLE_TARGET_EXPORT=On", "-DENABLE_CUSTOM_COMPILER_FLAGS=On", "-DENABLE_VALGRIND=Off", "-DENABLE_SAFE_STACK=Off", "-DBUILD_SHARED_LIBS=Off", "-DBUILD_SHARED_AND_STATIC_LIBS=Off", "-DCMAKE_INSTALL_PREFIX=build/deps/cjson/instal", "-DENABLE_LOCALES=Off", "-DCJSON_OVERRIDE_BUILD_SHARED_LIBS=Off", "-DENABLE_CJSON_VERSION_SO=Off", "-B", "build/deps/cjson/build", "-S", "build/deps/cjson");
        if (!nob_cmd_run(&cmd)) return 1;
        nob_cmd_append(&cmd, "cmake", "--build", "build/deps/cjson/build");
        if (!nob_cmd_run(&cmd)) return 1;
        nob_cmd_append(&cmd, "cmake", "--install", "build/deps/cjson/build");
        if (!nob_cmd_run(&cmd)) return 1;
    }
    nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-Wpedantic", "-std=c11", "-ggdb", "-o", "build/sclip-tests", "-Iinclude", "-Ibuild/deps/cjson/instal/include", "-Ibuild/deps/", "-Lbuild/deps/cjson/instal/lib", "test/sclip-tests.c", "-l:libcjson.a");
    if (!nob_cmd_run(&cmd)) return 1;
    // nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-Wpedantic", "-ggdb", "-o", "build/sclip", "-lpthread", "-lrt", "-Iinclude", "-Ibuild/deps/cjson/instal/include", "-Lbuild/deps/cjson/instal/lib", "src/sclip.c", "-l:libcjson.a");
    // if (!nob_cmd_run(&cmd)) return 1;
    return 0;
}
