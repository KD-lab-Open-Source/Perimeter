add_library(Network STATIC
        CommonEvents.cpp
        NetComEventBuffer.cpp
        HyperSpace.cpp
        ServerList.cpp
        P2P_interface1Th.cpp
        P2P_interface2Th.cpp
        P2P_interface2ThSDL.cpp
        P2P_interfaceAnyTh.cpp
        NetConnection.cpp
)

target_include_directories(Network PRIVATE
        "${PROJECT_SOURCE_DIR}/Source/Game"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/Render/inc"
        "${PROJECT_SOURCE_DIR}/Source/Terra"
        "${PROJECT_SOURCE_DIR}/Source/tx3d"
        "${PROJECT_SOURCE_DIR}/Source/UserInterface"
        "${PROJECT_SOURCE_DIR}/Source/Network"
        "${PROJECT_SOURCE_DIR}/Source/Units"
        "${PROJECT_SOURCE_DIR}/Source/Render/client"
        "${PROJECT_SOURCE_DIR}/Source/Physics"
        "${PROJECT_SOURCE_DIR}/Source/AI"
        "${PROJECT_SOURCE_DIR}/Source/Sound"
        .
)

target_compile_options(Network PRIVATE ${PERIMETER_COMPILE_OPTIONS})
