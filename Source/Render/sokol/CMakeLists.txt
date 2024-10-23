
OPTION(OPTION_PROCESS_SHADERS "Re-Process game shaders" OFF)

#Include Sokol
FetchContent_Declare(sokol
        GIT_REPOSITORY    https://github.com/floooh/sokol
        GIT_TAG           "4bda1469d3b311af03a34dd956460776c920dc2e"
        GIT_SHALLOW       OFF
)
FetchContent_MakeAvailable(sokol)

#Include ImGui
FetchContent_Declare(imgui
        GIT_REPOSITORY    https://github.com/ocornut/imgui
        GIT_TAG           "cb16568fca5297512ff6a8f3b877f461c4323fbe"
        GIT_SHALLOW       OFF
)
FetchContent_MakeAvailable(imgui)

IF (OPTION_PROCESS_SHADERS)
    #Download precompiled sokol-shdc
    FetchContent_Declare(sokol-tools-bin
            GIT_REPOSITORY    https://github.com/floooh/sokol-tools-bin
            GIT_TAG           "f1af4529746b804d6c79d5a36c333aec489fab27"
            GIT_SHALLOW       OFF
    )
    FetchContent_MakeAvailable(sokol-tools-bin)

    if(NOT DEFINED ENV{PERIMETER_SHADER_LANG})
        #Use all GLSL dialects, HLSL for D3d and WebGPU
        set(PERIMETER_SHADER_LANG "glsl410:glsl300es:hlsl5")
        #Metal for Apple platforms
        set(PERIMETER_SHADER_LANG "${PERIMETER_SHADER_LANG}:metal_macos:metal_ios:metal_sim")
    endif()
    
    add_custom_target(sokol_shaders)

    IF (MACOS)
        IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch")
            set(SOKOL_SHDC_BIN ${sokol-tools-bin_SOURCE_DIR}/bin/osx_arm64/sokol-shdc)
        ELSE()
            set(SOKOL_SHDC_BIN ${sokol-tools-bin_SOURCE_DIR}/bin/osx/sokol-shdc)
        ENDIF()
    ELSEIF(PERIMETER_WINDOWS)
        set(SOKOL_SHDC_BIN ${sokol-tools-bin_SOURCE_DIR}/bin/win32/sokol-shdc.exe)
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

    PERIMETER_SOKOL_SHDC(mesh_color_texture      mesh_color_tex1     "")
    PERIMETER_SOKOL_SHDC(mesh_color_texture      mesh_color_tex2     "SHADER_TEX_2")
    PERIMETER_SOKOL_SHDC(mesh_normal_texture     mesh_normal_tex1    "")
    PERIMETER_SOKOL_SHDC(tile_map                tile_map            "")
    PERIMETER_SOKOL_SHDC(shadow_texture          shadow_tex1         "")
    PERIMETER_SOKOL_SHDC(shadow_texture          shadow_normal_tex1  "SHADER_NORMAL")
    PERIMETER_SOKOL_SHDC(mesh_tex1               mesh_tex1           "")
ENDIF()

#Select Sokol API per platform and options
set(RenderSokol_SokolImpl sokol/SokolImpl.cpp)
if (MACOS)
    if(NOT OPTION_SOKOL_API)
        set(OPTION_SOKOL_API "METAL")
    endif ()
    #Required for compiling some glue as Obj-C
    set(RenderSokol_SokolImpl sokol/SokolImpl.mm)
    if(OPTION_SOKOL_API MATCHES "GLCORE")
        set(RenderSokol_LINK_LIBS "-framework OpenGL")
    elseif(OPTION_SOKOL_API MATCHES "METAL")
        set(RenderSokol_LINK_LIBS "-framework Foundation -framework QuartzCore -framework Metal -framework MetalKit")
        #set(RenderSokol_LINK_LIBS "${RenderSokol_LINK_LIBS} -fobjc-arc -framework AudioToolbox")
        #You may wanna use UIKit here instead when porting to iOS in far far future
        set(RenderSokol_LINK_LIBS "${RenderSokol_LINK_LIBS} -framework Cocoa")
    endif()
elseif(PERIMETER_WINDOWS)
    if(NOT OPTION_SOKOL_API)
        set(OPTION_SOKOL_API "D3D11")
    endif ()
    if(OPTION_SOKOL_API MATCHES "D3D11")
        set(RenderSokol_LINK_LIBS d3d11)
    elseif(OPTION_SOKOL_API MATCHES "GLCORE")
        set(RenderSokol_LINK_LIBS opengl32)
    endif()
else()
    if(NOT OPTION_SOKOL_API)
        set(OPTION_SOKOL_API "GLCORE")
    endif ()
    if(OPTION_SOKOL_API MATCHES "GLES3")
        set(RenderSokol_LINK_LIBS GL)
    elseif(OPTION_SOKOL_API MATCHES "GLCORE")
        set(RenderSokol_LINK_LIBS GL)
    endif()
endif()

if(OPTION_SOKOL_API MATCHES "GLES3")
    message("Sokol API: OpenGLES 3.0")
    add_definitions(-DSOKOL_GLES3)
    set(SOKOL_GL ON)
    set(RenderSokol_ImguiBackend ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
elseif(OPTION_SOKOL_API MATCHES "GLCORE")
    message("Sokol API: OpenGL Core")
    add_definitions(-DSOKOL_GLCORE)
    set(RenderSokol_ImguiBackend ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
elseif(OPTION_SOKOL_API MATCHES "D3D11")
    message("Sokol API: D3D11")
    add_definitions(-DSOKOL_D3D11)
    set(RenderSokol_ImguiBackend ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp)
elseif(OPTION_SOKOL_API MATCHES "METAL")
    message("Sokol API: Metal")
    add_definitions(-DSOKOL_METAL)
    set(RenderSokol_ImguiBackend ${imgui_SOURCE_DIR}/backends/imgui_impl_metal.mm)
else()
    message(SEND_ERROR "Unknown Sokol API selected: ${OPTION_SOKOL_API}")
endif()

#Required for SDL_GL_* and SDL_SetHint/SDL_GetHint funcs called during render init
set(RenderSokol_LINK_LIBS ${RenderSokol_LINK_LIBS} ${SDL2_LIBRARY})

set(RenderSokol_SRCS
    ${RenderSokol_SokolImpl}
    sokol/SokolResources.cpp
    sokol/SokolRender.cpp
    sokol/SokolRenderTexture.cpp
    sokol/SokolRenderState.cpp
    sokol/SokolRenderDraw.cpp
    sokol/SokolRenderPipeline.cpp
    sokol/SokolShaders.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${RenderSokol_ImguiBackend}
)

add_library(RenderSokol STATIC ${RenderSokol_SRCS})

if(NOT PERIMETER_WINDOWS AND OPTION_SOKOL_API MATCHES "D3D11")
    #This way dxvk will be fetched and compiled before RenderSokol
    add_dependencies(RenderSokol dxvk)
    target_include_directories(RenderSokol PRIVATE
            "${DXVK_INCLUDE_DIRS}"
    )

    #Handle output lib file and also export it during install
    set(DXVK_D3D11_LIB ${DXVK_BINARY_DIR}/src/d3d11/libdxvk_d3d11${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(RenderSokol_LINK_LIBS ${RenderSokol_LINK_LIBS} ${DXVK_D3D11_LIB})
    install (FILES ${DXVK_D3D11_LIB} TYPE LIB)
endif()

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
        "${PROJECT_SOURCE_DIR}/Source/Render/tilemap"
        "${sokol_SOURCE_DIR}/"
        "${imgui_SOURCE_DIR}/"
)

#Render also needs headers
target_include_directories(Render PRIVATE "${sokol_SOURCE_DIR}/")

target_link_libraries(RenderSokol PRIVATE ${RenderSokol_LINK_LIBS})

target_compile_options(RenderSokol PRIVATE ${PERIMETER_COMPILE_OPTIONS})
