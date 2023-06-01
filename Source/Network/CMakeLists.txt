INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Source/Game"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/Render/inc"
        "${PROJECT_SOURCE_DIR}/Source/Terra"
        "${PROJECT_SOURCE_DIR}/Source/UserInterface"
        "${PROJECT_SOURCE_DIR}/Source/Network"
        "${PROJECT_SOURCE_DIR}/Source/Units"
        "${PROJECT_SOURCE_DIR}/Source/Render/client"
        "${PROJECT_SOURCE_DIR}/Source/Physics"
        "${PROJECT_SOURCE_DIR}/Source/AI"
        "${PROJECT_SOURCE_DIR}/Source/Sound"
        .
        "${PROJECT_SOURCE_DIR}/Source/Network/Peer2"
)

SET(Network_SRCS
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

ADD_LIBRARY(Network STATIC ${Network_SRCS})
target_compile_options(Network PRIVATE ${PERIMETER_COMPILE_OPTIONS})
