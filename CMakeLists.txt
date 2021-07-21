SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
CMAKE_MINIMUM_REQUIRED(VERSION 3.16.0)

# root
PROJECT(perimeter)

# Compiler / Host detections
IF(MSVC OR ${CMAKE_GENERATOR} STREQUAL "NMake Makefiles")
    SET(MSVC_CL_BUILD ON)
ENDIF()

SET(CMAKE_CONFIGURATION_TYPES "Release;Debug;MinSizeRel;RelWithDebInfo")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)

IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    SET(CLANG ON)
ELSE()
    SET(CLANG OFF)
ENDIF()

SET(PERIMETER_DEBUG OFF)
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    SET(PERIMETER_DEBUG ON)
ENDIF()

SET(PERIMETER_EXODUS OFF)
IF(CMAKE_HOST_WIN32 OR MINGW)
    MESSAGE("Windows or MinGW build")
    SET(PERIMETER_WINDOWS ON)
    SET(PERIMETER_EXE_FLAGS WIN32)
ELSE()
    MESSAGE("Not Windows or MinGW build - WARNING: EXPERIMENTAL!")
    SET(PERIMETER_EXE_FLAGS "")
    SET(PERIMETER_WINDOWS OFF)
    SET(PERIMETER_EXODUS ON)
    SET(PERIMETER_EXODUS_WINDOW ON)
    SET(PERIMETER_EXODUS_SOUND ON)
ENDIF()

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE("Architecture: 64 bits - WARNING: EXPERIMENTAL!")
    SET(PERIMETER_ARCH_64 ON)
    ADD_DEFINITIONS(-DPERIMETER_ARCH_64)
ELSE()
    MESSAGE("Architecture: 32 bits")
    SET(PERIMETER_ARCH_64 OFF)
ENDIF()

# options
OPTION(OPTION_FINAL_VERSION "Generate Final version" ON)
OPTION(OPTION_STATIC_BUILD "Generate static build" OFF)
OPTION(OPTION_PROCESS_SCRIPTS "Re-Process game scripts with XPrm" OFF)
OPTION(OPTION_DISABLE_STACKTRACE "Disable stacktrace support" OFF)
OPTION(OPTION_LINKER_LLD "Use LLD linker in unix based compilers" ON)
OPTION(OPTION_DEBUG_ASSERT "Enable debug assertions" OFF)


# depends
FIND_PACKAGE(Boost REQUIRED)
IF(MSVC_CL_BUILD)
    FIND_PACKAGE(Vorbis CONFIG REQUIRED)
    SET(Vorbis_FOUND TRUE)
    SET(VORBIS_LIBRARIES Vorbis::vorbis Vorbis::vorbisenc Vorbis::vorbisfile)
    FIND_PACKAGE(SDL2 CONFIG REQUIRED)
    SET(SDL2_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/SDL2")
    SET(SDL2_LIBRARY SDL2::SDL2) 
    SET(SDL2MAIN_LIBRARY SDL2::SDL2main)
ELSE()
    # paths to search libs
    SET(LIBRARY_SEARCH_PATHS "
        ${LIBRARY_SEARCH_PATHS}
        ~/Library/Frameworks 
        /Library/Frameworks
        /mingw
        /usr/pkg
        /usr/local
        /usr
        /sw # Fink
        /opt/local # DarwinPorts
        /opt/csw # Blastwave
        /opt
        /local
        C:/MinGW
        /usr/include/x86_64-linux-gnu
        /usr/include/aarch64-linux-gnu
        /usr/lib/x86_64-linux-gnu
        /usr/lib/aarch64-linux-gnu
    ")
    
    SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    
    FIND_PACKAGE(Vorbis)
    #This makes SDL2 not include SDL2main
    SET(SDL2_BUILDING_LIBRARY ON)
    FIND_PACKAGE(SDL2 REQUIRED)
ENDIF()

IF(NOT PERIMETER_WINDOWS)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    FIND_PACKAGE(Threads REQUIRED)
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
)

IF(FFMPEG_FOUND)
    #FFMPEG is found
    ADD_DEFINITIONS(-DPERIMETER_FFMPEG)
    INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIRS})
ENDIF()

IF(NOT MSVC_CL_BUILD)
    # Configure lib suffixes for MinGW unless is static build
    IF(MINGW AND NOT OPTION_STATIC_BUILD)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES
            ".a"
            ".dll.a"
            ".a.lib"
        )
    ENDIF()
    IF(OPTION_STATIC_BUILD)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        SET(BUILD_SHARED_LIBS OFF)
        SET(CMAKE_EXE_LINKER_FLAGS "-static")
    ENDIF()
    
    #Others
    INCLUDE(CheckCXXCompilerFlag)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=delete-non-virtual-dtor -Wno-conversion-null")
    IF(OPTION_LINKER_LLD)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=lld")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=lld")
    ENDIF()
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -no-pie -fno-pie")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -ftree-vectorize -ffast-math")
    CHECK_CXX_COMPILER_FLAG("-msse2" HAVE_SIMDSSE)
    IF(HAVE_SIMDSSE)
        SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -msse2")
    ENDIF()

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
    add_compile_options(-Wno-reorder)
    IF(CLANG)
        add_compile_options(-Wno-reorder-ctor)
    ENDIF()

    add_compile_options(-Wno-unknown-pragmas) #TODO we should remove these pragmas?
    add_compile_options(-Werror=shift-negative-value)
    add_compile_options(-Werror=dangling-else)
    add_compile_options(-Werror=parentheses)
    add_compile_options(-Werror=return-type)
    # add_compile_options(-Werror=switch) #TODO enable for strict check of switch block
    # add_compile_options(-Werror=write-strings) #TODO enable this again

    if (CLANG)
        add_compile_options(-Werror=mismatched-new-delete )
    endif()
ENDIF()

ADD_DEFINITIONS(-D_PERIMETER_)
#ADD_DEFINITIONS(-D_PERIMETER_ADDON_)
IF(OPTION_FINAL_VERSION)
    ADD_DEFINITIONS(-DNDEBUG)
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
ENDIF()
IF(OPTION_DISABLE_STACKTRACE)
    ADD_DEFINITIONS(-DOPTION_DISABLE_STACKTRACE)
ENDIF()
IF(PERIMETER_EXODUS)
    ADD_DEFINITIONS(-DPERIMETER_EXODUS)
ENDIF()
IF(PERIMETER_EXODUS_WINDOW)
    ADD_DEFINITIONS(-DPERIMETER_EXODUS_WINDOW)
ENDIF()
IF(PERIMETER_EXODUS_SOUND)
    ADD_DEFINITIONS(-DPERIMETER_EXODUS_SOUND)
ENDIF()

ADD_SUBDIRECTORY("Source")
