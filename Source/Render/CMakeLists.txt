INCLUDE_DIRECTORIES(
        BEFORE .
        "inc"
        "src"
        "client"
        "D3D"
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/PluginMAX/Inc"
        "${PROJECT_SOURCE_DIR}/Source/PluginMAX"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Include"
)

SET(Render_SRCS
        D3D/AdapterMode.cpp
        D3D/D3DRender.cpp
        D3D/D3DRenderDraw.cpp
        D3D/D3DRenderTexture.cpp
        D3D/D3DRenderTilemap.cpp
        D3D/DrawType.cpp
        D3D/OcclusionQuery.cpp
        D3D/PoolManager.cpp
        D3D/RenderDevice.cpp
        D3D/clip/ClippingMesh.cpp
        shader/shaders.cpp
        D3D/StdAfxRD.cpp
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
        3dx/Interpolator3dx.cpp
        3dx/Lib3dx.cpp
        3dx/Node3DX.cpp
        3dx/Static3DX.cpp
        src/FileImage.cpp
        src/TexLibrary.cpp
        src/Texture.cpp
        src/LogicGeneric.cpp
        client/D3DRenderDrawClient.cpp
        client/ExternalObj.cpp
        client/ForceField.cpp
        client/SceneClient.cpp
        client/Silicon.cpp
        client/WinVideo.cpp
        gemsiii/filter.c
        saver/Saver.cpp
        src/cCamera.cpp
        src/Font.cpp
        src/Scene.cpp
        src/VisGeneric.cpp
        src/VisGrid2d.cpp
)

SET(Render_LINK_LIBS PluginMAX)

ADD_LIBRARY(Render STATIC ${Render_SRCS})

TARGET_LINK_LIBRARIES(Render ${Render_LINK_LIBS})
