INCLUDE_DIRECTORIES(
        BEFORE .
        "inc"
        "src"
        "client"
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/PluginMAX/Inc"
        "${PROJECT_SOURCE_DIR}/Source/PluginMAX"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

SET(Render_SRCS
        src/umath.cpp
        src/VisError.cpp
        src/Frame.cpp
        src/TileMap.cpp
        src/UnkLight.cpp
        src/UnkObj.cpp
        src/CChaos.cpp
        src/cZPlane.cpp
        src/lighting.cpp
        src/Line3d.cpp
        src/particle.cpp
        src/SpriteNode.cpp
        src/Trail.cpp
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
)

IF (OPTION_D3D9)
    INCLUDE_DIRECTORIES(
        "D3D"
    )
    SET(Render_SRCS ${Render_SRCS}
        client/D3DRenderDrawClient.cpp
        shader/shaders.cpp
        D3D/AdapterMode.cpp
        D3D/D3DRender.cpp
        D3D/D3DRenderDraw.cpp
        D3D/D3DRenderTexture.cpp
        D3D/D3DRenderTilemap.cpp
        D3D/DrawType.cpp
        D3D/OcclusionQuery.cpp
        D3D/PoolManager.cpp
        D3D/clip/ClippingMesh.cpp
    )
ENDIF()

SET(Render_LINK_LIBS PluginMAX)

IF (OPTION_SOKOL)
    INCLUDE("sokol/CMakeLists.txt")
    SET(Render_LINK_LIBS ${Render_LINK_LIBS} RenderSokol)
ENDIF()

ADD_LIBRARY(Render STATIC ${Render_SRCS})
target_compile_options(Render PUBLIC ${PERIMETER_COMPILE_OPTIONS})

TARGET_LINK_LIBRARIES(Render ${Render_LINK_LIBS})
