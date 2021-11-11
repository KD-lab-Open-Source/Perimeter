
#simpleini retrieval
FetchContent_Declare(simpleini
        GIT_REPOSITORY    https://github.com/brofield/simpleini
        GIT_TAG           origin/master
        GIT_SHALLOW       ON
        )
FetchContent_MakeAvailable(simpleini)
INCLUDE_DIRECTORIES(
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${simpleini_SOURCE_DIR}"
)

SET(Exodus_SRCS
        exodus.cpp
        ${pevents_SOURCE_DIR}/src/pevents.cpp
)

ADD_LIBRARY(Exodus STATIC ${Exodus_SRCS})
target_compile_options(Exodus PUBLIC ${PERIMETER_COMPILE_OPTIONS})

SET(ExodusD3D9_SRCS
        d3dx9.cpp
        d3dx9math.cpp
)

ADD_LIBRARY(ExodusD3D9 STATIC ${ExodusD3D9_SRCS})
target_compile_options(ExodusD3D9 PUBLIC ${PERIMETER_COMPILE_OPTIONS})
