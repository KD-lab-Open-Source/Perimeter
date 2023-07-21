#Include Sokol
FetchContent_Declare(sokol
        GIT_REPOSITORY    https://github.com/floooh/sokol
        GIT_TAG           "c0e54485457b2e1645f2d809394753b53bf50cd4"
        GIT_SHALLOW       OFF
        )
FetchContent_MakeAvailable(sokol)
include_directories("${sokol_SOURCE_DIR}/")

IF (OPTION_PROCESS_SHADERS)
    #Download precompiled sokol-shdc
    FetchContent_Declare(sokol-tools-bin
            GIT_REPOSITORY    https://github.com/floooh/sokol-tools-bin
            GIT_TAG           "e906578942899c5c803bae8de3b722ab4241f768"
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
        message("Sokol shader: ${SHADER_FILE} ${OUTPUT_FILE}")
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
    set(RenderSokol_SokolImpl sokol/SokolImpl.mm)
    if(OPTION_SOKOL_API MATCHES "GLCORE33")
        message("Sokol API: OpenGL Core 3.3")
        add_definitions(-DSOKOL_GLCORE33)
        set(RenderSokol_LINK_LIBS "-framework OpenGL")
    else()
        message("Sokol API: Metal")
        add_definitions(-DSOKOL_METAL)
        set(RenderSokol_LINK_LIBS "-framework Foundation -framework QuartzCore -framework Metal -framework MetalKit")
        #set(RenderSokol_LINK_LIBS "${RenderSokol_LINK_LIBS} -fobjc-arc -framework AudioToolbox")
        #You may wanna use UIKit here instead when porting to iOS in far far future
        set(RenderSokol_LINK_LIBS "${RenderSokol_LINK_LIBS} -framework Cocoa") 
    endif()
elseif(PERIMETER_WINDOWS)
    if(OPTION_SOKOL_API MATCHES "D3D11")
        message("Sokol API: DirectX 11")
        add_definitions(-DSOKOL_D3D11)
    else()
        message("Sokol API: OpenGL Core 3.3")
        add_definitions(-DSOKOL_GLCORE33)
    endif()
else()
    if(OPTION_SOKOL_API MATCHES "GLES2")
        message("Sokol API: OpenGLES 2.0")
        add_definitions(-DSOKOL_GLES2)
        set(RenderSokol_LINK_LIBS GL)
    elseif(OPTION_SOKOL_API MATCHES "GLES3")
        message("Sokol API: OpenGLES 3.0")
        add_definitions(-DSOKOL_GLES3)
        set(RenderSokol_LINK_LIBS GL)
    else()
        message("Sokol API: OpenGL Core 3.3")
        add_definitions(-DSOKOL_GLCORE33)
        set(RenderSokol_LINK_LIBS GL)
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
ADD_DEPENDENCIES(RenderSokol dependencies)

target_link_libraries(RenderSokol PRIVATE ${RenderSokol_LINK_LIBS})
