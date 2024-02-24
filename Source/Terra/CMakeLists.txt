add_library(Terra STATIC
        crc.cpp
        f3d.cpp
        fastmath.cpp
        geo.cpp
        geoNet.cpp
        grid.cpp
        pn.cpp
        pnint.cpp
        pnt.cpp
        stdafxTr.cpp
        sur_scr.cpp
        tgai.cpp
        tmask.cpp
        tools.cpp
        undoDispatcher.cpp
        worldGen.cpp
        VMAP.cpp
        LAND.cpp
)

target_include_directories(Terra PRIVATE
        .
        "${PROJECT_SOURCE_DIR}/Source/tx3d"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

target_compile_options(Terra PRIVATE ${PERIMETER_COMPILE_OPTIONS})
