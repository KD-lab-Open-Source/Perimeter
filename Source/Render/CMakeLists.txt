
OPTION(OPTION_DXVK_1 "Use old DXVK-native (DXVK 1.x) instead of DXVK 2.x" OFF)
OPTION(OPTION_DXVK_SOURCE_DIR "Path for predownloaded DXVK source code")

SET(Render_SRCS
        src/Unknown.cpp
        src/umath.cpp
        src/VisError.cpp
        src/Frame.cpp
        src/UnkLight.cpp
        src/UnkObj.cpp
        src/CChaos.cpp
        src/cPlane.cpp
        src/lighting.cpp
        src/Line3d.cpp
        src/SpriteNode.cpp
        src/AnimChannel.cpp
        src/AnimChannelNode.cpp
        src/MeshBank.cpp
        src/MeshTri.cpp
        src/ObjLibrary.cpp
        src/ObjLight.cpp
        src/ObjMesh.cpp
        src/ObjNode.cpp
        src/ShadowVolume.cpp
        src/NParticle.cpp
        src/FileImage.cpp
        src/TexLibrary.cpp
        src/Texture.cpp
        src/LogicGeneric.cpp
        client/ExternalObj.cpp
        client/ForceField.cpp
        client/SceneClient.cpp
        client/Silicon.cpp
        gemsiii/filter.cpp
        saver/Saver.cpp
        src/cCamera.cpp
        src/Font.cpp
        src/Scene.cpp
        src/VisGeneric.cpp
        src/VisGrid2d.cpp
        src/RenderDevice.cpp
        src/DrawBuffer.cpp
        tracker/RenderTracker.cpp
        src/RenderDeviceDraw.cpp
        client/RenderDrawClient.cpp
        tilemap/TileMap.cpp
        tilemap/TileMapRender.cpp
        tilemap/TileMapBumpTile.cpp
        tilemap/TileMapTexturePool.cpp
        tilemap/PoolManager.cpp
        tilemap/ClippingMesh.cpp
)

IF (OPTION_D3D9)
    SET(Render_SRCS ${Render_SRCS}
        shader/shaders.cpp
        D3D/AdapterMode.cpp
        D3D/D3DRender.cpp
        D3D/D3DRenderDraw.cpp
        D3D/D3DRenderTexture.cpp
        D3D/D3DRenderTilemap.cpp
        D3D/DrawType.cpp
    )
    IF(PERIMETER_EXODUS)
        SET(Render_SRCS ${Render_SRCS}
            d3dx9/d3dx9.cpp
        )
    ENDIF()
ENDIF()

add_library(Render STATIC ${Render_SRCS})

IF (OPTION_D3D9)
    target_include_directories(Render PRIVATE "D3D")
ENDIF()

target_include_directories(Render PRIVATE
        .
        "inc"
        "src"
        "client"
        "tracker"
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/PluginMAX/Inc"
        "${PROJECT_SOURCE_DIR}/Source/PluginMAX"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

SET(Render_LINK_LIBS PluginMAX)

if(OPTION_D3D9)
    #D3D9 Renderer enabled
    if(PERIMETER_WINDOWS)
        #DirectX SDK libs
        target_include_directories(Render PUBLIC "${PROJECT_SOURCE_DIR}/Thirdparty/MSDXSDK_02_06/Include")

        #DirectX 64 or 32 libs
        IF(PERIMETER_ARCH_64)
            MESSAGE("Linking against DirectX 64 bits")
            set(LIBS_MSDXSDK ${PROJECT_SOURCE_DIR}/Thirdparty/MSDXSDK_02_06/Lib/x64)
        ELSE()
            MESSAGE("Linking against DirectX 32 bits")
            set(LIBS_MSDXSDK ${PROJECT_SOURCE_DIR}/Thirdparty/MSDXSDK_02_06/Lib/x86)
        ENDIF()
        target_link_directories(Render BEFORE PUBLIC ${LIBS_MSDXSDK})

        set(Render_LINK_LIBS ${Render_LINK_LIBS}
                ${LIBS_MSDXSDK}/d3d9.lib
                ${LIBS_MSDXSDK}/d3dx9.lib
                ${LIBS_MSDXSDK}/DxErr.lib
        )
    else()
        set(NEED_DXVK ON)
    endif()
endif()

if(OPTION_SOKOL AND NOT PERIMETER_WINDOWS AND OPTION_SOKOL_API MATCHES "D3D11")
    MESSAGE(WARNING "Using DXVK for Sokol D3D backend, this is only to check compilation! is NOT supported")
    set(NEED_DXVK ON)
endif()

if(NEED_DXVK)
    #For non windows we use DXVK to provide headers and impl
    MESSAGE("Using DXVK for D3D API")

    set(DXVK_BUILD_OPTIONS -Denable_d3d10=false)
    
    if(PERIMETER_DEBUG AND OFF)
        #Set ON in case one needs to go down a rabbit hole
        MESSAGE(WARNING "Building debug DXVK!")
        set(DXVK_BUILD_OPTIONS ${DXVK_BUILD_OPTIONS} --buildtype=debug)
    else()
        set(DXVK_BUILD_OPTIONS ${DXVK_BUILD_OPTIONS} --buildtype=release)
    endif ()
    if(OPTION_D3D9)
        set(DXVK_BUILD_OPTIONS ${DXVK_BUILD_OPTIONS} -Denable_d3d9=true)
    else()
        set(DXVK_BUILD_OPTIONS ${DXVK_BUILD_OPTIONS} -Denable_d3d9=false)
    endif()
    if(OPTION_SOKOL_API MATCHES "D3D11")
        set(DXVK_BUILD_OPTIONS ${DXVK_BUILD_OPTIONS} -Denable_d3d11=true -Denable_dxgi=true)
    else()
        set(DXVK_BUILD_OPTIONS ${DXVK_BUILD_OPTIONS} -Denable_d3d11=false -Denable_dxgi=false)
    endif()

    #DXVK wrapper lib retrieval and building
    IF (MACOS)
        IF (CMAKE_OSX_ARCHITECTURES)
            #Cross compile script, this assumes CMAKE_OSX_ARCHITECTURES only contains 1 arch
            set(DXVK_BUILD_COMMAND
                    ${PROJECT_SOURCE_DIR}/macos/dxvk-native.sh
                    ${CMAKE_OSX_ARCHITECTURES}
                    ${PROJECT_SOURCE_DIR}/macos/
            )
        ENDIF()
        #TODO change this to upstream if DXVK2 works with MoltenVK and MacOS
        set(OPTION_DXVK_1 ON)
    ENDIF()
    IF(OPTION_DXVK_1)
        if (NOT DXVK_GIT_URL)
            set(DXVK_GIT_URL https://github.com/IonAgorria/dxvk-native)
        endif ()
        if (NOT DXVK_GIT_TAG)
            set(DXVK_GIT_TAG "43aedc756cbd620b9ee8b1cf2c17b17cc49b3781")
        endif ()
    ELSE()
        if (NOT DXVK_GIT_URL)
            set(DXVK_GIT_URL https://github.com/doitsujin/dxvk)
        endif ()
        if (NOT DXVK_GIT_TAG)
            set(DXVK_GIT_TAG "c3dd74be6baec53786d4e064a572185b70347a17") # tag v2.7.1
        endif ()
        #Disable SDL3 for now to avoid issues with game using SDL2
        set(DXVK_BUILD_OPTIONS ${DXVK_BUILD_OPTIONS} -Denable_d3d8=false -Dnative_sdl2=enabled -Dnative_sdl3=disabled)
    ENDIF()
    IF("${DXVK_BUILD_COMMAND}" STREQUAL "")
        set(DXVK_BUILD_COMMAND meson setup)
    ENDIF()
    IF(OPTION_DXVK_SOURCE_DIR)
        MESSAGE("DXVK Source path provided: ${OPTION_DXVK_SOURCE_DIR}")
        UNSET(DXVK_GIT_URL)
        UNSET(DXVK_GIT_TAG)
        SET(DXVK_BUILD_COMMAND ${DXVK_BUILD_COMMAND} ${OPTION_DXVK_SOURCE_DIR})
    ELSE()
        SET(DXVK_BUILD_COMMAND ${DXVK_BUILD_COMMAND} ../dxvk)
    ENDIF()
    ExternalProject_Add(dxvk
            SOURCE_DIR        ${OPTION_DXVK_SOURCE_DIR}
            GIT_REPOSITORY    ${DXVK_GIT_URL}
            GIT_TAG           ${DXVK_GIT_TAG}
            GIT_SHALLOW       OFF
            BUILD_ALWAYS      OFF
            CONFIGURE_HANDLED_BY_BUILD ON
            CONFIGURE_COMMAND ${DXVK_BUILD_COMMAND} ${DXVK_BUILD_OPTIONS}
            BUILD_COMMAND     ninja
            INSTALL_COMMAND   ""
            BUILD_BYPRODUCTS 
                <BINARY_DIR>/src/d3d9/libdxvk_d3d9${CMAKE_SHARED_LIBRARY_SUFFIX}
                <BINARY_DIR>/src/d3d11/libdxvk_d3d11${CMAKE_SHARED_LIBRARY_SUFFIX}
    )
    ExternalProject_Get_property(dxvk SOURCE_DIR BINARY_DIR)
    set(DXVK_BINARY_DIR ${BINARY_DIR})
    set(DXVK_INCLUDE_DIRS
            "${SOURCE_DIR}/include/native/directx"
            "${SOURCE_DIR}/include/native/windows"
    )

    if(OPTION_D3D9)        
        #Make sure our d3dx9 is before dxvk includes!
        target_include_directories(Render PUBLIC "d3dx9")

        #Handle output lib file and also export it during install
        set(DXVK_D3D9_LIB ${DXVK_BINARY_DIR}/src/d3d9/libdxvk_d3d9${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(Render_LINK_LIBS ${Render_LINK_LIBS} ${DXVK_D3D9_LIB})
        install (FILES ${DXVK_D3D9_LIB} TYPE LIB)
    endif()

    #This way dxvk will be fetched and compiled before Render
    add_dependencies(Render dxvk)
    target_include_directories(Render PUBLIC "${DXVK_INCLUDE_DIRS}")
endif()

IF (OPTION_SOKOL)
    INCLUDE("sokol/CMakeLists.txt")
    SET(Render_LINK_LIBS ${Render_LINK_LIBS} RenderSokol)
ENDIF()

OPTION(OPTION_VULKAN "Enable Vulkan renderer" OFF)
IF (OPTION_VULKAN)
    find_package(Vulkan REQUIRED)
    find_program(GLSLC_EXECUTABLE NAMES glslc
        HINTS ENV VULKAN_SDK
        PATH_SUFFIXES Bin bin)
    IF(NOT GLSLC_EXECUTABLE)
        MESSAGE(FATAL_ERROR "glslc not found - required for Vulkan shader compilation")
    ENDIF()
    MESSAGE("Found glslc: ${GLSLC_EXECUTABLE}")

    set(VULKAN_SHADER_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders_vk")
    file(MAKE_DIRECTORY "${VULKAN_SHADER_OUT_DIR}")

    # Helper to compile a GLSL shader pair to SPV
    macro(add_vulkan_shader BASE_NAME VERTEX_GLSL FRAGMENT_GLSL)
        set(_vert_spv "${VULKAN_SHADER_OUT_DIR}/${BASE_NAME}.vert.spv")
        set(_frag_spv "${VULKAN_SHADER_OUT_DIR}/${BASE_NAME}.frag.spv")
        add_custom_command(
            OUTPUT "${_vert_spv}"
            COMMAND "${GLSLC_EXECUTABLE}" -fshader-stage=vert
                    "${CMAKE_CURRENT_SOURCE_DIR}/vulkan/shaders/${VERTEX_GLSL}"
                    -o "${_vert_spv}" ${ARGN}
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/vulkan/shaders/${VERTEX_GLSL}"
            COMMENT "Compiling vertex shader ${VERTEX_GLSL}"
        )
        add_custom_command(
            OUTPUT "${_frag_spv}"
            COMMAND "${GLSLC_EXECUTABLE}" -fshader-stage=frag
                    "${CMAKE_CURRENT_SOURCE_DIR}/vulkan/shaders/${FRAGMENT_GLSL}"
                    -o "${_frag_spv}" ${ARGN}
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/vulkan/shaders/${FRAGMENT_GLSL}"
            COMMENT "Compiling fragment shader ${FRAGMENT_GLSL}"
        )
        list(APPEND VULKAN_SPV_FILES "${_vert_spv}" "${_frag_spv}")
    endmacro()

    set(VULKAN_SPV_FILES "")
    add_vulkan_shader(mesh_color_tex1        mesh_color_tex.vert   mesh_color_tex.frag)
    add_vulkan_shader(mesh_color_tex2        mesh_color_tex.vert   mesh_color_tex.frag  -DHAS_TEX2)
    add_vulkan_shader(mesh_normal_tex1       mesh_normal_tex.vert  mesh_normal_tex.frag)
    add_vulkan_shader(mesh_tex1              mesh_tex1.vert        mesh_tex1.frag)
    add_vulkan_shader(shadow_tex1            shadow_tex.vert       shadow_tex.frag)
    add_vulkan_shader(shadow_normal_tex1     shadow_tex.vert       shadow_tex.frag      -DHAS_NORMAL)
    add_vulkan_shader(tile_map               tile_map.vert         tile_map.frag)
    add_vulkan_shader(mesh_color_tex1_inst   mesh_color_tex.vert   mesh_color_tex.frag   -DUSE_INSTANCING)
    add_vulkan_shader(mesh_color_tex2_inst   mesh_color_tex.vert   mesh_color_tex.frag   -DHAS_TEX2 -DUSE_INSTANCING)
    add_vulkan_shader(mesh_normal_tex1_inst  mesh_normal_tex.vert  mesh_normal_tex.frag  -DUSE_INSTANCING)
    add_vulkan_shader(mesh_tex1_inst         mesh_tex1.vert        mesh_tex1.frag        -DUSE_INSTANCING)
    add_vulkan_shader(shadow_tex1_inst       shadow_tex.vert       shadow_tex.frag       -DUSE_INSTANCING)
    add_vulkan_shader(shadow_normal_tex1_inst shadow_tex.vert      shadow_tex.frag       -DHAS_NORMAL -DUSE_INSTANCING)

    add_custom_target(VulkanShaders DEPENDS ${VULKAN_SPV_FILES})
    add_dependencies(Render VulkanShaders)

    target_sources(Render PRIVATE
        vulkan/VulkanRender.cpp
        vulkan/VulkanRenderState.cpp
        vulkan/VulkanRenderDraw.cpp
        vulkan/VulkanRenderTexture.cpp
        vulkan/VulkanResources.cpp
        vulkan/VulkanPipeline.cpp
    )
    target_compile_definitions(Render PUBLIC PERIMETER_VULKAN)
    target_include_directories(Render PRIVATE "vulkan"
        "${PROJECT_SOURCE_DIR}/Source/Render/tilemap"
    )
    SET(Render_LINK_LIBS ${Render_LINK_LIBS} Vulkan::Vulkan)
ENDIF()

target_link_libraries(Render PRIVATE ${Render_LINK_LIBS})

target_compile_options(Render PRIVATE ${PERIMETER_COMPILE_OPTIONS})
