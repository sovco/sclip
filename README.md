# sclip
Simple  CLI Parser

# Description

This repository provides an executable to generate a single header lib that would parse command line interface arguments.
sclip executable has the following interface,
```bash
Usage:
sclip [options]
    -i ,--input   <string>    Input json config file.
    -o ,--output  <string>    Output generated header file.
    -h ,--help    <bool>      Shows help menu.
    -v ,--version <bool>      Shows version string.
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

In order to read the contents of stdin you need to specify an option in the cconfig with type stdin.
Example
```json
{
    "project": "sclip",
    "version": "0.0.1",
    "options": [
        {
            "name": "stdin_input",
            "value-type": "stdin",
            "long-opt": "--stdin",
            "short-opt": "--",
            "description": "Use stdin as input",
            "optional": true 
        }
    ]
}
```
In the particular case of the above mentioned config a function sclip_opt_stdin_input_get_value() will be generated.
With it you can check if the file was provided.
If the file is provided you need to call function sclip_get_stdin_contents() that returns sclip_stdin_content struct,
sclip_stdin_content has two properties data and lenght, data holds the contents of stdin.
To free sclip_stdin_content call sclip_free_stdin_content.
Example code,
```c
if (sclip_opt_stdin_input_is_provided()) {
    sclip_stdin_content contents = sclip_get_stdin_contents();
    // ...
    sclip_free_stdin_content(&contents);
}
```

# Dependencies

This project depends on saa and cJSON and there acquired during the build process.
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
./project-build [--build-tests] [--enable-debug]
```
