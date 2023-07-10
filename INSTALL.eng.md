# General notes

Project uses CMake build system.

A clean build may take a while to compile and link, have patience until it completes or errors.

Be aware that some options and parts are experimental or untested.

Some CMake options are available to customize builds:
- `OPTION_DISABLE_STACKTRACE` - Set this to ON to not compile stacktrace support, which can give issues sometimes when building.  
- `OPTION_LINKER` - Set this to custom linker in builds that are not MSVC
More available in CMakeLists.txt
  
Pass `-DCMAKE_BUILD_TYPE=Debug` to cmake command for debug builds with some stuff enabled or -DCMAKE_BUILD_TYPE=Release 

We have several CI in github which can serve as hint and examples.

### Main dependencies

- C++17 compiler
- CMake
- SDL2
- SDL2_image
- SDL2_net
- SDL2_mixer
- FFmpeg (avcodec / avformat / avfilter / avutils / swscale)
- Boost
- ZLIB

### Automatic dependencies

These dependencies are downloaded and compiled automatically if needed by CMake and no intervention is need:
- https://github.com/doitsujin/dxvk - Vulkan translation layer for D3D9 render code.
- https://github.com/neosmart/pevents - Provides Windows compatible events.
- https://github.com/brofield/simpleini - Provides .ini file loading/writing.

---

# Windows + Visual Studio (MSVC)

Recommended method on Windows. 

### Requirements

- CMake
- Microsoft Visual Studio for C/C++

### Instructions

- Run `msvc\generate_msvc.bat` script
- Open generate solution file for project
- Compile project

### Launching the game
Copy generated perimeter.exe executable and required libraries .dll's to the game directory.

---

# Windows + MinGW/MSYS

### Requirements

- CMake
- MinGW/MSYS environment
- Main dependencies +
  - gcc
  - make
  - ninja
  - lld
  - libbacktrace

Packages: `gcc cmake make ninja SDL2 SDL2_image SDL2_net SDL2_mixer boost lld libbacktrace zlib`

### Instructions

- `mkdir build`
- `cd build`
- `cmake -G Ninja ..`
- `ninja dependencies`
- `ninja -j4` (replace 4 with number of logical cores in your computer)

### Launching the game
Copy generated perimeter.exe executable and required libraries .dll's to the game directory.

---

# Linux

Experimental support, it uses dxvk for D3D9 API and some libraries to supply Windows platform stuff.

### Requirements

- Main dependencies +
  - gcc
  - make
  - ninja
  - meson
  - lld
  - libbacktrace
  - vulkan headers (for DXVK)
  - glsllang-tools (for DXVK which needs glslangValidator program) 

Packages in Debian/Ubuntu: `build-essential ninja-build cmake meson libvulkan-dev glslang-tools lld cmake meson zlib1g-dev
libsdl2-dev libsdl2-image-dev libsdl2-net-dev libsdl2-mixer-dev libboost-dev
libavcodec-dev libavformat-dev libavfilter-dev libswscale-dev`

### Instructions

- `mkdir build`
- `cd build`
- `cmake -G Ninja ..`
- `ninja dependencies`
- `ninja -j$(nproc --all)` or `ninja -j4` (replace 4 with number of logical cores in your computer)

### Launching the game
Run executable while current directory is the game directory or pass content= with game path.

---

# MacOS

Experimental support, it uses dxvk for D3D9 API and some libraries to supply Windows platform stuff.

### Requirements

- Main dependencies +
  - gcc
  - make
  - ninja
  - meson
  - libbacktrace
  - Vulkan SDK or MoltenVK (for DXVK)

Packages in MacPorts: `ninja cmake meson ffmpeg boost libsdl2-dev libsdl2-image-dev libsdl2-net-dev libsdl2-mixer-dev zlib`

### Instructions

- `mkdir build`
- `cd build`
- `cmake -G Ninja ..`
- `ninja dependencies`
- `ninja -j$(nproc --all)` or `ninja -j4` (replace 4 with number of logical cores in your computer)

### Launching the game
Run executable while current directory is the game directory or pass content= with game path.

---

# Linux using Docker

This uses Docker container to setup a builder that will compile linux compatible binaries
A old debian image is used so the binary is linked against old GLIBC symbols so it can be run in older OSes

### Requirements

- Docker

### Instructions

- Run `docker/linux/run.sh` script

### Launching the game

Copy generated perimeter executable and required libraries .so's to the game directory.

---

# Windows using Docker dockcross

This uses Docker container to setup a builder that will compile Win32 compatible .exe, originally used during initial
porting and currently is not recommended, but can be useful for compiling in hosts that can run Docker containers.

### Requirements

- Docker

### Instructions

- Run `docker/dockcross/run.sh` script

### Launching the game

Copy generated perimeter.exe executable and required libraries .dll's to the game directory.
