#Include Sokol
FetchContent_Declare(sokol
        GIT_REPOSITORY    https://github.com/floooh/sokol
        GIT_TAG           origin/master
        GIT_SHALLOW       ON
        )
FetchContent_MakeAvailable(sokol)
include_directories("${sokol_SOURCE_DIR}/")

IF (OPTION_PROCESS_SHADERS)
    #Download precompiled sokol-shdc
    FetchContent_Declare(sokol-tools-bin
            GIT_REPOSITORY    https://github.com/floooh/sokol-tools-bin
            GIT_TAG           origin/master
            GIT_SHALLOW       ON
            )
    FetchContent_MakeAvailable(sokol-tools-bin)

    if(NOT DEFINED ENV{PERIMETER_SHADER_LANG})
        #Use all GLSL dialects
        set(PERIMETER_SHADER_LANG "glsl330:glsl100:glsl300es:metal_macos")
    endif()
    
    add_custom_target(sokol_shaders)

    IF (MACOS)
        IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch")
            set(SOKOL_SHDC_BIN ${sokol-tools-bin_SOURCE_DIR}/bin/osx_arm64/sokol-shdc)
        ELSE()
            set(SOKOL_SHDC_BIN ${sokol-tools-bin_SOURCE_DIR}/bin/osx/sokol-shdc)
        ENDIF()
    ELSEIF(PERIMETER_WINDOWS)
        set(SOKOL_SHDC_BIN ${sokol-tools-bin_SOURCE_DIR}/bin/win32/sokol-shdc)
    ELSE()
        set(SOKOL_SHDC_BIN ${sokol-tools-bin_SOURCE_DIR}/bin/linux/sokol-shdc)
    ENDIF()
    
    MACRO(PERIMETER_SOKOL_SHDC SHADER_FILE OUTPUT_FILE SHADER_DEFINES_ARG)
        #Workaround since SHADER_DEFINES doesnt seem to be eval correctly otherwise
        set(SHADER_DEFINES "${SHADER_DEFINES_ARG}")
        message("Sokol shader: ${SHADER_FILE} ${OUTPUT_FILE}")
        if(NOT SHADER_DEFINES STREQUAL "")
            set(SHADER_DEFINES "--defines=${SHADER_DEFINES}")
        endif()
        add_custom_command(
                TARGET sokol_shaders PRE_BUILD
                COMMAND ${SOKOL_SHDC_BIN}
                --input "${SHADER_FILE}.glsl" --output "${OUTPUT_FILE}.h" --slang "${PERIMETER_SHADER_LANG}"
                --ifdef ${SHADER_DEFINES} --reflection --module "${OUTPUT_FILE}" --format=sokol
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Source/Render/sokol/shaders
                VERBATIM
        )
    ENDMACRO(PERIMETER_SOKOL_SHDC)

    PERIMETER_SOKOL_SHDC(color_texture      color        "SHADER_COLOR")
    PERIMETER_SOKOL_SHDC(color_texture      color_tex1   "SHADER_COLOR:SHADER_TEX_1")
    PERIMETER_SOKOL_SHDC(color_texture      color_tex2   "SHADER_COLOR:SHADER_TEX_2")
    PERIMETER_SOKOL_SHDC(color_texture      tex1         "SHADER_TEX_1")
    PERIMETER_SOKOL_SHDC(normal_texture     normal       "")
    PERIMETER_SOKOL_SHDC(normal_texture     normal_dot3  "SHADER_DOT3")
ENDIF()

#Use OpenGL Core 3.3 in Windows, Linux and MacOS
if (MACOS)
    #TODO add switch to choose Metal in MacOS?
    add_definitions(-DSOKOL_GLCORE33)
    set(RenderSokol_LINK_LIBS
            "-framework OpenGL"
            )
elseif(PERIMETER_WINDOWS)
    add_definitions(-DSOKOL_GLCORE33)
    #set(RenderSokol_LINK_LIBS opengl32)
else()
    add_definitions(-DSOKOL_GLCORE33)
    set(RenderSokol_LINK_LIBS GL)
endif()

#Required for SDL_GL_* funcs called during render init
set(RenderSokol_LINK_LIBS ${RenderSokol_LINK_LIBS} ${SDL2_LIBRARY})

add_library(RenderSokol STATIC
    sokol/SokolResources.cpp
    sokol/SokolRender.cpp
    sokol/SokolRenderTexture.cpp
    sokol/SokolRenderState.cpp
    sokol/SokolRenderDraw.cpp
    sokol/SokolImpl.cpp
)

IF (OPTION_PROCESS_SHADERS)
    add_dependencies(RenderSokol sokol_shaders)
ENDIF()

target_link_libraries(RenderSokol ${RenderSokol_LINK_LIBS})