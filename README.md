# Ciri Path Tracer

## Cloning

This project uses git submodules so make sure to include `--recuse-submodules` when cloning:

    git clone --recurse-submodules https://github.com/cianjinks/Ciri

## Building

### Linux

First run `linux_setup.sh` then:

    cmake --build ./build/release OR cmake --build ./build/debug

The executable will be placed in the corresponding build folder.

### Windows

Running `win_setup.bat` will create a Visual Studio solution in the folder `./build`.