# Ciri Path Tracer

## Media
![Sponza Textured](docs/sponza_textured.png)
![Lost Empire Textured](docs/lostempire_textured.png)
![Dragon Normals](docs/dragon_normals.png)
![Hairball Normals](docs/hairball_normals.png)

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

## Resources

Test models contained in the `resources/mesh/` folder were downloaded from Morgan McGuire's [Computer Graphics Archive](https://casual-effects.com/data).