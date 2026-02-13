# sclip
Simple  CLI Parser

# Description

This repository provides an executable to generate a single header lib that would parse command line interface arguments.
sclip executable has the following interface,
```bash
Usage:
sclip [options]
    -i, --input   <string>    Input json config file. 
    -o, --output  <string>    Output generated header file. 
    -h, --help    <bool>      Shows help menu 
    -v, --version <bool>      Shows version string 

```
As input it accepts a json configuration file and outputs a generated C header file.
This generated C header file then can be used to parse cli args in any C/Cpp application.

# JSON Config

The config is supposed to look like this (you can also find it in assets/example.json without comments)
```json
{
    "project": "sclip", // Project name
    "version": "0.0.1", // Version string
    "options": [
        {
            "name": "input", // This is unique id of this option
            "value-type": "string", // there are several types [string, bool, double, long]
            "long-opt": "--input", // long option string
            "short-opt": "-i", // short option string
            "description": "Input json config file.", // Description that will be in help menu (don't use \n)
            "optional": false // Is this value optional or not
        },
        {
            "name": "output",
            "value-type": "string",
            "long-opt": "--output",
            "short-opt": "-o",
            "description": "Output generated header file.",
            "optional": false
        }
    ]
}
```

# Handling of stdin

Example code,
```c
if (sclip_is_stdin_available()) {
    sclip_stdin_content contents = sclip_get_stdin_contents();
    // ...
    sclip_free_stdin_content(&contents);
}
```

# Dependencies

This project depends on saa and cJSON, cJSON is fetched automatically during the build process.
Build process requires
1) gcc or clang
2) cmake (to configure cJSON) 
3) git (to clone cJSON)
4) curl (to download saa header)

# Build

To build run these commands
```bash
mkdir -p build/deps/nob
curl -Lo build/deps/nob/nob.h https://raw.githubusercontent.com/tsoding/nob.h/refs/heads/main/nob.h
gcc -o project-build project-build.c 
./project-build [--build-tests | [--build-tests && --run-tests]] [--debug] [--install]
```
