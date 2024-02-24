#Include Sokol
FetchContent_Declare(sokol
        GIT_REPOSITORY    https://github.com/floooh/sokol
        GIT_TAG           "004223737d9f6d07a0aa5c63659662480285de29"
        GIT_SHALLOW       OFF
        )
FetchContent_MakeAvailable(sokol)

IF (OPTION_PROCESS_SHADERS)
    #Download precompiled sokol-shdc
    FetchContent_Declare(sokol-tools-bin
            GIT_REPOSITORY    https://github.com/floooh/sokol-tools-bin
            GIT_TAG           "5b8d524b7faef9d479a48549b3f83511c83e92f0"
            GIT_SHALLOW       OFF
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
        message("Compiling Sokol shader: ${SHADER_FILE} ${OUTPUT_FILE}")
        if(NOT SHADER_DEFINES STREQUAL "")
            set(SHADER_DEFINES "--defines=${SHADER_DEFINES}")
        endif()
        add_custom_command(
                TARGET sokol_shaders PRE_BUILD
                COMMAND ${SOKOL_SHDC_BIN}
                --input "${SHADER_FILE}.glsl" --output "${OUTPUT_FILE}.h" --slang "${PERIMETER_SHADER_LANG}"
                --format=sokol --reflection
                --ifdef ${SHADER_DEFINES} --module "${OUTPUT_FILE}"
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Source/Render/sokol/shaders
                VERBATIM
        )
    ENDMACRO(PERIMETER_SOKOL_SHDC)

    PERIMETER_SOKOL_SHDC(color_texture      color_tex1   "")
    PERIMETER_SOKOL_SHDC(color_texture      color_tex2   "SHADER_TEX_2")
    PERIMETER_SOKOL_SHDC(normal_texture     normal       "")
    PERIMETER_SOKOL_SHDC(terrain            terrain      "")
ENDIF()

#Select Sokol API per platform and options
set(RenderSokol_SokolImpl sokol/SokolImpl.cpp)
if (MACOS)
    if(NOT OPTION_SOKOL_API)
        set(OPTION_SOKOL_API "METAL")
    endif ()
    #Required for compiling some glue as Obj-C
    set(RenderSokol_SokolImpl sokol/SokolImpl.mm)
    if(OPTION_SOKOL_API MATCHES "GLCORE33")
        message("Sokol API: OpenGL Core 3.3")
        add_definitions(-DSOKOL_GLCORE33)
        set(RenderSokol_LINK_LIBS "-framework OpenGL")
    elseif(OPTION_SOKOL_API MATCHES "METAL")
        message("Sokol API: Metal")
        add_definitions(-DSOKOL_METAL)
        set(RenderSokol_LINK_LIBS "-framework Foundation -framework QuartzCore -framework Metal -framework MetalKit")
        #set(RenderSokol_LINK_LIBS "${RenderSokol_LINK_LIBS} -fobjc-arc -framework AudioToolbox")
        #You may wanna use UIKit here instead when porting to iOS in far far future
        set(RenderSokol_LINK_LIBS "${RenderSokol_LINK_LIBS} -framework Cocoa")
    else()
        message(SEND_ERROR "Unknown Sokol API selected: ${OPTION_SOKOL_API}")
    endif()
elseif(PERIMETER_WINDOWS)
    if(NOT OPTION_SOKOL_API)
        set(OPTION_SOKOL_API "D3D11")
    endif ()
    if(OPTION_SOKOL_API MATCHES "D3D11")
        message("Sokol API: DirectX 11")
        add_definitions(-DSOKOL_D3D11)
    elseif(OPTION_SOKOL_API MATCHES "GLCORE33")
        message("Sokol API: OpenGL Core 3.3")
        add_definitions(-DSOKOL_GLCORE33)
        set(RenderSokol_LINK_LIBS opengl32)
    else()
        message(SEND_ERROR "Unknown Sokol API selected: ${OPTION_SOKOL_API}")
    endif()
else()
    if(NOT OPTION_SOKOL_API)
        set(OPTION_SOKOL_API "GLCORE33")
    endif ()
    if(OPTION_SOKOL_API MATCHES "GLES2")
        message("Sokol API: OpenGLES 2.0")
        add_definitions(-DSOKOL_GLES2)
        set(RenderSokol_LINK_LIBS GL)
    elseif(OPTION_SOKOL_API MATCHES "GLES3")
        message("Sokol API: OpenGLES 3.0")
        add_definitions(-DSOKOL_GLES3)
        set(RenderSokol_LINK_LIBS GL)
    elseif(OPTION_SOKOL_API MATCHES "GLCORE33")
        message("Sokol API: OpenGL Core 3.3")
        add_definitions(-DSOKOL_GLCORE33)
        set(RenderSokol_LINK_LIBS GL)
    else()
        message(SEND_ERROR "Unknown Sokol API selected: ${OPTION_SOKOL_API}")
    endif()
endif()

#Required for SDL_GL_* funcs called during render init
set(RenderSokol_LINK_LIBS ${RenderSokol_LINK_LIBS} ${SDL2_LIBRARY} XTool)

add_library(RenderSokol STATIC
    ${RenderSokol_SokolImpl}
    sokol/SokolResources.cpp
    sokol/SokolRender.cpp
    sokol/SokolRenderTexture.cpp
    sokol/SokolRenderState.cpp
    sokol/SokolRenderDraw.cpp
    sokol/SokolRenderPipeline.cpp
    sokol/SokolShaders.cpp
)

IF (OPTION_PROCESS_SHADERS)
    add_dependencies(RenderSokol sokol_shaders)
ENDIF()
add_dependencies(RenderSokol dependencies)

target_include_directories(RenderSokol PRIVATE
        .
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/Render/inc"
        "${PROJECT_SOURCE_DIR}/Source/Render/src"
        "${PROJECT_SOURCE_DIR}/Source/Render/client"
        "${PROJECT_SOURCE_DIR}/Source/Render/tracker"
        "${sokol_SOURCE_DIR}/"
)

#Render also needs headers
target_include_directories(Render PRIVATE "${sokol_SOURCE_DIR}/")

target_link_libraries(RenderSokol PRIVATE ${RenderSokol_LINK_LIBS})

target_compile_options(RenderSokol PRIVATE ${PERIMETER_COMPILE_OPTIONS})
