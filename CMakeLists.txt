CMAKE_MINIMUM_REQUIRED(VERSION 3.16.0)

# root
PROJECT(perimeter VERSION 3.0.11)
message("Version ${PROJECT_VERSION}")

SET(CMAKE_CONFIGURATION_TYPES "Release;Debug;MinSizeRel;RelWithDebInfo")

set(CMAKE_C_STANDARD 11)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)
SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
IF("${CMAKE_INSTALL_RPATH}" STREQUAL "")
    SET(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
ENDIF ()

# Options and inputs
OPTION(OPTION_FINAL_VERSION "Generate Final version" ON)
OPTION(OPTION_STATIC_BUILD "Generate static build" OFF)
OPTION(OPTION_STATIC_LIB_STD "Statically link to libstd++" ON)
OPTION(OPTION_CHECK_SCRIPTS "Check game scripts with XPrm" OFF)
OPTION(OPTION_PROCESS_SCRIPTS "Re-Process game scripts with XPrm" OFF)
OPTION(OPTION_DISABLE_STACKTRACE "Disable stacktrace support" OFF)
OPTION(OPTION_LINKER "Use specified linker instead of default" "")
OPTION(OPTION_DEBUG_ASSERT "Enable debug assertions" OFF)
OPTION(OPTION_GPERFTOOLS "Link executable with gperftools for debugging" OFF)
OPTION(OPTION_D3D9 "Enable D3D9 renderer using dxvk in *nix or DirectX in Windows" ON)
OPTION(OPTION_LINK_LIBS "Link executables with provided libraries, placed before the rest" "")
OPTION(OPTION_LINK_LIBS_POST "Link executables with provided libraries, placed after the rest" "")
OPTION(OPTION_DXVK_1 "Use old DXVK-native (DXVK 1.x) instead of DXVK 2.x" OFF)
OPTION(OPTION_DXVK_SOURCE_DIR "Path for predownloaded DXVK source code")
OPTION(OPTION_SOKOL "Enable Sokol GFX based renderer" OFF)
OPTION(OPTION_PROCESS_SHADERS "Re-Process game shaders" OFF)
OPTION(OPTION_FFMPEG_MOVIES "Use FFMPEG to decode ingame movies" ON)
OPTION(OPTION_ASAN "Enable AddressSanitizer" OFF)
OPTION(OPTION_O0 "Disable optimizations" OFF)

# Compiler detections
SET(MSVC_CL_BUILD OFF)
SET(CLANG OFF)
IF(MSVC OR ${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
    MESSAGE("Compiler: MSVC")
    SET(MSVC_CL_BUILD ON)
ENDIF()
IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    MESSAGE("Compiler: clang")
    SET(CLANG ON)
ENDIF()

#Detect 32 or 64 bits compiler
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE("Architecture: 64 bits")
    SET(PERIMETER_ARCH_64 ON)
    ADD_DEFINITIONS(-DPERIMETER_ARCH_64)
ELSE()
    MESSAGE("Architecture: 32 bits")
    SET(PERIMETER_ARCH_64 OFF)
ENDIF()

# Host detections and tweaks
MESSAGE("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
SET(MACOS OFF)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOS ON)
endif()

SET(PERIMETER_EXE_FLAGS "")
SET(PERIMETER_EXODUS OFF)
IF(CMAKE_HOST_WIN32 OR MINGW)
    MESSAGE("Windows or MinGW build")
    SET(PERIMETER_WINDOWS ON)
    #Compile exe as Win32 App instead of Console App
    #Also add icon via .rc
    SET(PERIMETER_EXE_FLAGS WIN32 perimeter.rc)
ELSE()
    MESSAGE("Not Windows or MinGW build - WARNING: EXPERIMENTAL!")
    SET(PERIMETER_WINDOWS OFF)
    SET(PERIMETER_EXODUS ON)
    SET(PERIMETER_EXODUS_SOUND ON)
ENDIF()

IF (OPTION_D3D9)
    MESSAGE("D3D9 renderer: Enabled")
    ADD_DEFINITIONS(-DPERIMETER_D3D9)
ELSE()
    MESSAGE("D3D9 renderer: Disabled")
ENDIF()

IF (OPTION_SOKOL)
    MESSAGE("Sokol renderer: Enabled - WARNING: EXPERIMENTAL!")
    ADD_DEFINITIONS(-DPERIMETER_SOKOL)
ELSE()
    MESSAGE("Sokol renderer: Disabled")
ENDIF()

#To allow downloading deps
INCLUDE(FetchContent)
INCLUDE(ExternalProject)

#Is a debug build?
SET(PERIMETER_DEBUG OFF)
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    SET(PERIMETER_DEBUG ON)
    #On debug mode don't download every time unless its absent
    SET(FETCHCONTENT_UPDATES_DISCONNECTED ON)
ENDIF()

#Linking setup
SET(EXE_LINK_LIBS_PRE "")
SET(EXE_LINK_LIBS_POST "")
SET(BUILD_SHARED_LIBS OFF)
SET(BUILD_STATIC_LIBS ON)
SET(PREFER_STATIC_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
SET(PREFER_DYNAMIC_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
MESSAGE("Original lib suffixes ${CMAKE_FIND_LIBRARY_SUFFIXES}")
IF(OPTION_STATIC_BUILD)
    #TODO: Not properly tested or used in all platforms
    MESSAGE("Static build selected")
    SET(CMAKE_LINK_SEARCH_START_STATIC 1)
    SET(CMAKE_LINK_SEARCH_END_STATIC 1)
    IF(NOT PERIMETER_WINDOWS OR MINGW)
        SET(PREFER_STATIC_LIBRARY_SUFFIXES ".a")
        SET(PREFER_DYNAMIC_LIBRARY_SUFFIXES ".a")
    ENDIF()
ELSE()
    IF(PERIMETER_WINDOWS)
        IF(MINGW)
            SET(PREFER_STATIC_LIBRARY_SUFFIXES
                ".a"
                ".dll.a"
                ".a.lib"
            )
            SET(PREFER_DYNAMIC_LIBRARY_SUFFIXES
                ".a.lib"
                ".dll.a"
                ".a"
            )
        ENDIF()
    ELSEIF(APPLE)
        SET(PREFER_STATIC_LIBRARY_SUFFIXES ".a;.so;.tbd;.dylib")
        SET(PREFER_DYNAMIC_LIBRARY_SUFFIXES ".tbd;.dylib;.so;.a")
    ELSE()
        SET(PREFER_STATIC_LIBRARY_SUFFIXES ".a;.so")
        SET(PREFER_DYNAMIC_LIBRARY_SUFFIXES ".so;.a")
    ENDIF()
ENDIF()
SET(CMAKE_FIND_LIBRARY_SUFFIXES ${PREFER_DYNAMIC_LIBRARY_SUFFIXES})
MESSAGE("Current lib suffixes ${CMAKE_FIND_LIBRARY_SUFFIXES}")
IF(OPTION_LINK_LIBS)
    FOREACH(LINK_LIB ${OPTION_LINK_LIBS})
        MESSAGE("Linking extra library: ${LINK_LIB}")
        SET(EXE_LINK_LIBS_PRE ${EXE_LINK_LIBS_PRE} "${LINK_LIB}")
    ENDFOREACH()
ENDIF()
IF(OPTION_LINK_LIBS_POST)
    FOREACH(LINK_LIB ${OPTION_LINK_LIBS_POST})
        MESSAGE("Linking extra library: ${LINK_LIB}")
        SET(EXE_LINK_LIBS_POST ${EXE_LINK_LIBS_POST} "${LINK_LIB}")
    ENDFOREACH()
ENDIF()
IF(MINGW)
    #Required for static SDL2 linking
    SET(EXE_LINK_LIBS_POST ${EXE_LINK_LIBS_POST} "winmm;imm32;setupapi;version;ws2_32;iphlpapi")
ENDIF()
IF(OPTION_STATIC_LIB_STD)
    IF (NOT MSVC_CL_BUILD)
        MESSAGE("Linking libstd statically")
        set(LINK_FLAGS "-static-libstdc++")
        IF (NOT CLANG)
            set(LINK_FLAGS "${LINK_FLAGS} -static-libgcc")
        ENDIF ()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${LINK_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINK_FLAGS}")
    ENDIF ()
ENDIF()

# paths to search libs
IF(NOT MSVC_CL_BUILD)
    SET(LIBRARY_SEARCH_PATHS
        "${LIBRARY_SEARCH_PATHS}"
        "~/Library/Frameworks" 
        "/Library/Frameworks"
        "/mingw"
        "/usr/pkg"
        "/usr/local"
        "/usr"
        "/sw # Fink"
        "/opt/local # DarwinPorts"
        "/opt/csw # Blastwave"
        "/opt"
        "/local"
        "C:/MinGW"
        "/usr/include/x86_64-linux-gnu"
        "/usr/include/aarch64-linux-gnu"
        "/usr/lib/x86_64-linux-gnu"
        "/usr/lib/aarch64-linux-gnu"
    )
ENDIF()
# Dependencies
SET(CMAKE_FIND_LIBRARY_SUFFIXES ${PREFER_STATIC_LIBRARY_SUFFIXES})
IF(NOT APPLE)
    FIND_PACKAGE(Threads)
ENDIF()
FIND_PACKAGE(ZLIB REQUIRED)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ${PREFER_DYNAMIC_LIBRARY_SUFFIXES})
FIND_PACKAGE(Boost REQUIRED)
IF(MSVC_CL_BUILD)
    #Specifics to MSVC+VCPKG
    FIND_PACKAGE(SDL2 CONFIG REQUIRED)
    SET(SDL2_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/SDL2")
    SET(SDL2_LIBRARY SDL2::SDL2) 
    SET(SDL2MAIN_LIBRARY SDL2::SDL2main)
    FIND_PACKAGE(SDL2_image CONFIG REQUIRED)
    SET(SDL2_IMAGE_LIBRARY $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
    FIND_PACKAGE(SDL2_mixer CONFIG REQUIRED)
    SET(SDL2_MIXER_LIBRARY $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
    FIND_PACKAGE(SDL2_net CONFIG REQUIRED)
    SET(SDL2_NET_LIBRARY $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,SDL2_net::SDL2_net,SDL2_net::SDL2_net-static>)
ELSE()
    SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    
    FIND_PACKAGE(SDL2 REQUIRED)
    FIND_PACKAGE(SDL2-image REQUIRED)
    FIND_PACKAGE(SDL2-mixer REQUIRED)
    FIND_PACKAGE(SDL2-net REQUIRED)
ENDIF()

#FFMPEG used for AVI and video decoding if present
IF (PERIMETER_WINDOWS)
    FIND_PACKAGE(FFMPEG)
ELSE()
    #Required since there is no alternative for loading AVI textures
    FIND_PACKAGE(FFMPEG REQUIRED)
ENDIF()

# globals
INCLUDE_DIRECTORIES(SYSTEM
    ${Boost_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_MIXER_INCLUDE_DIR}
    ${SDL2_NET_INCLUDE_DIR}
)
SET(PERIMETER_COMPILE_OPTIONS)

IF(FFMPEG_FOUND)
    #FFMPEG is found
    ADD_DEFINITIONS(-DPERIMETER_FFMPEG)
    INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIRS})
    IF(OPTION_FFMPEG_MOVIES)
        ADD_DEFINITIONS(-DPERIMETER_FFMPEG_MOVIE)
        INCLUDE_DIRECTORIES(${FFMPEG_LIBSWSCALE_INCLUDE_DIRS})
        INCLUDE_DIRECTORIES(${FFMPEG_LIBAVFILTER_INCLUDE_DIRS})
    ELSE()
        MESSAGE("WARNING: Movies disabled!")
    ENDIF()
ELSE()
    MESSAGE("WARNING: FFMPEG not found!, movies unavailable!")
ENDIF()

#Required for more compliant floating point math
IF(MSVC_CL_BUILD)
    add_compile_options(/fp:precise)
ELSE()
    add_compile_options(-fno-fast-math -ffp-contract=off)
    
    #Arch specific flags
    IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64")
        #Enable SSE/SSE2 in x86 arch
        MESSAGE("CPU is x86 type, using SSE")
        add_compile_options(-mfpmath=sse -msse -msse2)
        IF (NOT PERIMETER_ARCH_64)
            #Might be required for SSE being enabled
            add_compile_options(-march=pentium4)
        ENDIF()
    ELSEIF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch")
        #ARM32/ARM64
        IF (NOT PERIMETER_ARCH_64)
            MESSAGE("CPU is ARM, using VFP WARNING: untested")
            add_compile_options(-mfpu=vfp)
        ENDIF()
    ENDIF()
ENDIF()

IF(NOT MSVC_CL_BUILD)
    #Others
    IF(CLANG)
        add_compile_options(-Werror=unused-command-line-argument)
    ENDIF()
    INCLUDE(CheckCXXCompilerFlag)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder -Werror=delete-non-virtual-dtor -Wno-conversion-null")
    IF(NOT (OPTION_LINKER STREQUAL ""))
        find_program(PROGRAM_LINKER NAMES ${OPTION_LINKER})
        IF(PROGRAM_LINKER)
            MESSAGE("Using custom linker ${PROGRAM_LINKER}")
            SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=${OPTION_LINKER}")
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=${OPTION_LINKER}")
        ENDIF ()
    ENDIF()
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -Wall -Wextra")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}")
    IF (OPTION_O0)
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
        SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O0")
    ELSE()
        SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -ftree-vectorize")
    ENDIF ()
    IF(MACOS)
        SET(CMAKE_BUILD_RPATH "./dxvk-native-prefix/src/dxvk-native-build/src/d3d9/")
    ELSE()
        SET(CMAKE_BUILD_RPATH "$ORIGIN") #let binary find libs on own dir
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -no-pie -fno-pie")
    ENDIF()

    IF (OPTION_ASAN)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
        set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
    ENDIF()

    #Codebase assumes char is signed, some compilers set char as unsigned
    add_compile_options(-fsigned-char)
    
    
    #TODO remove these once code is more tidy
    add_compile_options(-Wno-misleading-indentation)
    add_compile_options(-Wno-unused-parameter)
    add_compile_options(-Wno-unused-function)
    add_compile_options(-Wno-unused-variable)
    add_compile_options(-Wno-comment)
    IF(CLANG)
        add_compile_options(-Wno-missing-braces)
        add_compile_options(-Wno-invalid-source-encoding)
    ELSE()
        add_compile_options(-Wno-unused-but-set-variable)
    ENDIF()
    add_compile_options(-Wno-unused-value)
    add_compile_options(-Wno-deprecated)
    
    #TODO remove these once code is more reasonable
    add_compile_options(-Wno-sign-compare)
    add_compile_options(-Wno-narrowing)
    add_compile_options(-Wno-write-strings)
    add_compile_options(-Wno-return-type)
    IF(CLANG)
        add_compile_options(-Wno-reorder-ctor)
    ENDIF()

    add_compile_options(-Wno-unknown-pragmas) #TODO we should remove these pragmas?
    add_compile_options(-Werror=dangling-else)
    add_compile_options(-Werror=return-type)
    # add_compile_options(-Werror=switch) #TODO enable for strict check of switch block
    # add_compile_options(-Werror=write-strings) #TODO enable this again

    if (CLANG)
        add_compile_options(-Werror=mismatched-new-delete )
    endif()
    
    #These options may cause issues with external code compilation, so we apply only to our own targets
    SET(PERIMETER_COMPILE_OPTIONS -Werror=shift-negative-value)
    SET(PERIMETER_COMPILE_OPTIONS -Werror=parentheses)
ENDIF()

#Pass project version as a definition
SET(PERIMETER_VERSION_STR "\"${PROJECT_VERSION}\"")
ADD_DEFINITIONS(-DPERIMETER_VERSION=${PERIMETER_VERSION_STR})
UNSET(PERIMETER_VERSION_STR)

ADD_DEFINITIONS(-D_PERIMETER_)
IF(OPTION_FINAL_VERSION)
    ADD_DEFINITIONS(-D_FINAL_VERSION_)
ELSE()
    MESSAGE("Not compiling Final version - WARNING: EXPERIMENTAL!")
ENDIF()
IF(PERIMETER_DEBUG)
    ADD_DEFINITIONS(-DPERIMETER_DEBUG)
    MESSAGE("Compiling Debug version")
    IF(OPTION_DEBUG_ASSERT)
        ADD_DEFINITIONS(-DPERIMETER_DEBUG_ASSERT)
        MESSAGE("Debug assertions enabled")
    ENDIF()
ELSE()
    ADD_DEFINITIONS(-DNDEBUG)
ENDIF()
IF(OPTION_DISABLE_STACKTRACE)
    ADD_DEFINITIONS(-DOPTION_DISABLE_STACKTRACE)
ENDIF()
IF(PERIMETER_EXODUS)
    ADD_DEFINITIONS(-DPERIMETER_EXODUS)
ENDIF()
IF(PERIMETER_EXODUS_SOUND)
    ADD_DEFINITIONS(-DPERIMETER_EXODUS_SOUND)
ENDIF()

ADD_SUBDIRECTORY("Source")
