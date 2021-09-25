# General notes

Project uses CMake build system.

A clean build may take a while to compile and link, have patience until it completes or errors.

Be aware that some options and parts are experimental or untested.

Some CMake options are available to customize builds:
- `OPTION_DISABLE_STACKTRACE` - Set this to ON to not compile stacktrace support, which can give issues sometimes when building.  
- `OPTION_LINKER_LLD` - Set this to OFF to use builtin linker in builds that are not MSVC
More available in CMakeLists.txt
  
Pass `-DCMAKE_BUILD_TYPE=Debug` to cmake command for debug builds with some stuff enabled or -DCMAKE_BUILD_TYPE=Release 

We have several CI in github which can serve as hint and examples.

### Main dependencies

- CMake
- SDL2
- SDL2_image
- SDL2_net
- SDL2_mixer
- libvorbis
- ffmpeg / avcodec / avformat
- boost

### Automatic dependencies

These dependencies are downloaded and compiled automatically if needed by CMake and no intervention is need:
- https://github.com/Joshua-Ashton/dxvk-native - Modified VXVK for inclusion in native Linux applications.
- https://github.com/neosmart/pevents - Provides Windows compatible events.
- https://github.com/brofield/simpleini - Provides .ini file loading/writing.

---

# Windows + Visual Studio (MSVC)

Recommended method.

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

May generate bigger binaries than MSVC. Has support for stack traces which is useful during crashes.

### Requirements

- CMake
- MinGW/MSYS environment
- Main dependencies +
  - gcc
  - make
  - ninja
  - lld
  - libbacktrace

Packages: `gcc cmake make ninja SDL2 libvorbis boost lld libbacktrace`

### Instructions

- `mkdir build`
- `cd build`
- `cmake -G Ninja ..`
- `ninja -j4` (replace 4 with number of logical cores in your computer)

### Launching the game
Copy generated perimeter.exe executable and required libraries .dll's to the game directory.

---

# Linux

Still experimental support, it uses libraries like dxvk-native and some libraries to supply Windows platform stuff.

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

Packages in Debian/Ubuntu: `build-essential ninja cmake meson libvulkan-dev glslang-tools lld libsdl2-dev libogg-dev libvorbis-dev libboost-dev`

### Instructions

- `mkdir build`
- `cd build`
- `cmake -G Ninja ..`
- `ninja -j$(nproc --all)` or `ninja -j4` (replace 4 with number of logical cores in your computer)

### Launching the game
Copy generated perimeter executable to the game directory or run executable while current directory is the game directory.

---

# Docker

This uses Docker container to setup a builder that will compile Win32 compatible .exe, originally used during initial
porting and currently is not recommended, but can be useful for compiling in hosts that can run Docker containers.

### Requirements

- Docker

### Instructions

- Run `docker/run.sh` script

### Launching the game
Copy generated perimeter.exe executable and required libraries .dll's to the game directory.