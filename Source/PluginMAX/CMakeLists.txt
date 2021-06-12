INCLUDE_DIRECTORIES(
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

SET(PluginMAX_SRCS
        Src/AnimationMaterial.cpp
        Src/AnimationMesh.cpp
        Src/AnimLight.cpp
        Src/ChannelAnimation.cpp
        Src/ChannelMapping.cpp
        Src/DefMeshFile.cpp
        Src/HelperObject.cpp
        Src/LodObject.cpp
        Src/MaterialObject.cpp
        Src/MeshFile.cpp
        Src/NodeObject.cpp
        Src/ObjectCamera.cpp
        Src/ObjectLight.cpp
        Src/ObjectMesh.cpp
        Src/SceneLogic.cpp
        Src/StreamBuffer.cpp
        zip_resource.cpp
        SceneMesh.cpp
        ZIPStream.cpp
)

ADD_LIBRARY(PluginMAX STATIC ${PluginMAX_SRCS})
