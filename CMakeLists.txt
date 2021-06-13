SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment version")
CMAKE_MINIMUM_REQUIRED(VERSION 3.16.0)

# vcpkg should be configured before PROJECT()
IF(USE_VCPKG)
    IF(CUSTOME_VCPKG_INSTALL_DIR)
        SET(_VCPKG_INSTALLED_DIR ${CUSTOME_VCPKG_INSTALL_DIR})
    ENDIF()

    IF(CMAKE_GENERATOR_PLATFORM MATCHES "x64")
        SET(VCPKG_TARGET_TRIPLET "x64-windows")
    ELSE()
        SET(VCPKG_TARGET_TRIPLET "x86-windows")
    ENDIF()
    MESSAGE(STATUS "Using vcpkg triplet: ${VCPKG_TARGET_TRIPLET}")
ENDIF()

# root
PROJECT(perimeter)

# modules
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

SET(CMAKE_CONFIGURATION_TYPES "Release;Debug;MinSizeRel;RelWithDebInfo")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)

# options
OPTION(FINAL_VERSION "Generate Final version" ON)
OPTION(OPTION_STATIC_BUILD "Generate static build" ON)
OPTION(GEN_SCRIPTS "Re-Generates game scripts" OFF)

IF(USE_VCPKG)
    OPTION(OPTION_DISABLE_STACKTRACE "Disable stacktrace support" ON)
ELSE()
    OPTION(OPTION_DISABLE_STACKTRACE "Disable stacktrace support" OFF)
ENDIF()

# depends
FIND_PACKAGE(Boost REQUIRED) # Boost stacktrace was introduced in 1.65 but is bugged
FIND_PACKAGE(VORBIS)

# globals
INCLUDE_DIRECTORIES(SYSTEM
        ${Boost_INCLUDE_DIRS}
)

IF(MSVC)
    #MSVC compiler specifics
ELSE()
    IF(OPTION_STATIC_BUILD)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
        SET(BUILD_SHARED_LIBS OFF)
        SET(CMAKE_EXE_LINKER_FLAGS "-static")
    ENDIF()
    
    #Others
    INCLUDE(CheckCXXCompilerFlag)
    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Werror=delete-non-virtual-dtor -Wno-conversion-null")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -ftree-vectorize -ffast-math")
    CHECK_CXX_COMPILER_FLAG("-msse2" HAVE_SIMDSSE)
    IF(HAVE_SIMDSSE)
        SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -msse2")
    ENDIF()

    # For modern gcc
    #add_definitions(-D_USE_MATH_DEFINES)

    #TODO remove these once code is more tidy
    add_compile_options(-Wno-misleading-indentation)
    add_compile_options(-Wno-unused-parameter)
    add_compile_options(-Wno-unused-function)
    add_compile_options(-Wno-unused-variable)
    add_compile_options(-Wno-unused-but-set-variable)
    add_compile_options(-Wno-unused-value)
    add_compile_options(-Wno-deprecated)
    
    #TODO remove these once code is more reasonable
    add_compile_options(-Wno-sign-compare)
    add_compile_options(-Wno-narrowing)
    add_compile_options(-Wno-write-strings)
    add_compile_options(-Wno-return-type)

    add_compile_options(-Wno-unknown-pragmas) #TODO we should remove these pragmas?
    add_compile_options(-Werror=shift-negative-value)
    add_compile_options(-Werror=dangling-else)
    add_compile_options(-Werror=parentheses)
    #add_compile_options(-Werror=write-strings) #TODO enable this again

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-Werror=mismatched-new-delete )
    endif()
ENDIF()

ADD_DEFINITIONS(-D_PERIMETER_)
#ADD_DEFINITIONS(-D_PERIMETER_ADDON_)
IF(FINAL_VERSION)
    ADD_DEFINITIONS(-DNDEBUG)
    ADD_DEFINITIONS(-D_FINAL_VERSION_)
ELSE()
    MESSAGE("Not compiling Final version")
ENDIF()
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    ADD_DEFINITIONS(-DPERIMETER_DEBUG)
    MESSAGE("Compiling Debug version")
ENDIF()
IF(OPTION_DISABLE_STACKTRACE)
    ADD_DEFINITIONS(-DOPTION_DISABLE_STACKTRACE)
ENDIF()

IF( CMAKE_HOST_WIN32 OR MINGW )
    MESSAGE("Windows or MinGW build")
    SET(PERIMETER_WINDOWS ON)
ELSE()
    MESSAGE("Not Windows or MinGW build")
    SET(PERIMETER_WINDOWS OFF)
ENDIF()

ADD_SUBDIRECTORY("Source")
