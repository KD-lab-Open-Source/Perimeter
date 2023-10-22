add_library(tx3d STATIC
        Clouds.cpp
        Colorizer3DFactory.cpp
        Empty.cpp
        Marbles.cpp
        Sands.cpp
        Spots.cpp
        Woods.cpp
        BackCosInterpolator3D.cpp
        BackCubicInterpolator3D.cpp
        CosInterpolator3D.cpp
        CubicInterpolator3D.cpp
        Interpolator3DFactory.cpp
        SphericSimplexInterpolator3D.cpp
        CachingTurbulator3D.cpp
        SimpleCachingTurbulator3D.cpp
        SimpleTurbulator3D.cpp
        Texture3DUtils.cpp
        TGA.cpp
        XMLUtils.cpp
        Clear3D.cpp
        Compound3D.cpp
        FlatModelTexture3D.cpp
        FlatModelXTexture3D.cpp
        FlatModelYTexture3D.cpp
        FlatModelZTexture3D.cpp
        IndexedTexture3D.cpp
        Laying3D.cpp
        ModelTexture3D.cpp
        PerTexture3D.cpp
        Primitive3D.cpp
        Test3D.cpp
        Texture3DFactory.cpp
        ExtendedWorldLocation3D.cpp
        LayingLocation3D.cpp
        Location3DFactory.cpp
        SphereLocation3D.cpp
        StratumLocation3D.cpp
        StratumXLocation3D.cpp
        StratumYLocation3D.cpp
        StratumZLocation3D.cpp
        WorldLocation3D.cpp
)

target_include_directories(tx3d PRIVATE
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/XTool"
)

target_compile_options(tx3d PRIVATE ${PERIMETER_COMPILE_OPTIONS})
