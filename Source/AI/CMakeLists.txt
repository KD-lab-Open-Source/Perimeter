add_library(AI STATIC
        AiBuilding.cpp
        AIMain.cpp
        AITileMap.cpp
        ClusterFind.cpp
)

target_include_directories(AI PRIVATE
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/Render/inc"
        "${PROJECT_SOURCE_DIR}/Source/Render/client"
        "${PROJECT_SOURCE_DIR}/Source/Physics"
        "${PROJECT_SOURCE_DIR}/Source/Terra"
        "${PROJECT_SOURCE_DIR}/Source/tx3d"
        "${PROJECT_SOURCE_DIR}/Source/Network"
        "${PROJECT_SOURCE_DIR}/Source/Units"
        "${PROJECT_SOURCE_DIR}/Source/UserInterface"
        "${PROJECT_SOURCE_DIR}/Source/Sound"
        "${PROJECT_SOURCE_DIR}/Source/Game"
        .
)

target_compile_options(AI PRIVATE ${PERIMETER_COMPILE_OPTIONS})
