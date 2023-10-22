add_library(GeoLogic STATIC
        LineAntiAlias.cpp
        TerraCleft.cpp
)

target_include_directories(GeoLogic PRIVATE
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/Render/inc"
        "${PROJECT_SOURCE_DIR}/Source/Terra"
        "${PROJECT_SOURCE_DIR}/Source/tx3d"
        "${PROJECT_SOURCE_DIR}/Source/Game"
        .
)

target_compile_options(GeoLogic PRIVATE ${PERIMETER_COMPILE_OPTIONS})
