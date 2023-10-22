add_library(PluginMAX STATIC
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

target_compile_options(PluginMAX PRIVATE ${PERIMETER_COMPILE_OPTIONS})

target_include_directories(PluginMAX PRIVATE
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)
