
add_library(Exodus STATIC
        exodus.cpp
        ${pevents_SOURCE_DIR}/src/pevents.cpp
)

target_include_directories(Exodus PRIVATE
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

target_compile_options(Exodus PRIVATE ${PERIMETER_COMPILE_OPTIONS})

IF(OPTION_D3D9)
    add_library(ExodusD3D9 STATIC d3dx9.cpp)
    
    target_include_directories(ExodusD3D9 PRIVATE
            BEFORE .
            "${PROJECT_SOURCE_DIR}/Source/XTool"
    )

    target_compile_options(ExodusD3D9 PRIVATE ${PERIMETER_COMPILE_OPTIONS})
ENDIF()
