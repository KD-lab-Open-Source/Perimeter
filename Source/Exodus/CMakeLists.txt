
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
        d3dx9.cpp
        d3dx9math.cpp
        ${pevents_SOURCE_DIR}/src/pevents.cpp
)

ADD_LIBRARY(Exodus STATIC ${Exodus_SRCS})
