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
        CommonCommands.cpp
        CommonEvents.cpp
        ConnectionDP.cpp
        EventBufferDP.cpp
        HyperSpace.cpp
        P2P_interface1Th.cpp
        P2P_interface2Th.cpp
        P2P_interfaceAnyTh.cpp
)

IF(PERIMETER_EXODUS)
    SET(Network_SRCS ${Network_SRCS}
        P2P_interface2ThSDL.cpp
        P2P_interface3ThSDL.cpp
    )
ELSE()
    SET(Network_SRCS ${Network_SRCS}
        dplay8guid.cpp
        P2P_interface2ThDPF.cpp
        P2P_interface3ThDP.cpp
    )
ENDIF()

ADD_LIBRARY(Network STATIC ${Network_SRCS})
