target_sources(perimeter PRIVATE
        CameraManager.cpp
        MonkManager.cpp
        MusicManager.cpp
        VideoPlayer.cpp
        qd_textdb.cpp
        Texts.cpp
        Player.cpp
        Universe.cpp
        PerimeterDataChannel.cpp
        Runtime.cpp
        CopyToGraph.cpp
        Region.cpp
        GameContent.cpp
        Config.cpp
        "${PROJECT_SOURCE_DIR}/Source/TriggerEditor/TriggerExport.cpp"
)

target_include_directories(perimeter PRIVATE
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/XPrm" #For Statistics.h
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/Render/inc"
        "${PROJECT_SOURCE_DIR}/Source/Render/client"
        "${PROJECT_SOURCE_DIR}/Source/Terra"
        "${PROJECT_SOURCE_DIR}/Source/tx3d"
        "${PROJECT_SOURCE_DIR}/Source/UserInterface"
        "${PROJECT_SOURCE_DIR}/Source/Network"
        "${PROJECT_SOURCE_DIR}/Source/Units"
        "${PROJECT_SOURCE_DIR}/Source/Physics"
        "${PROJECT_SOURCE_DIR}/Source/AI"
        "${PROJECT_SOURCE_DIR}/Source/Sound"
        "${PROJECT_SOURCE_DIR}/Source/Scripts"
        "${PROJECT_SOURCE_DIR}/Source/Game/Scripts"
        .
)

SET(perimeter_LINK_LIBS
        #Our libs
        Units
        UserInterface
        AI
        Physics
        GeoLogic
        Render
        Network
        HT
        PluginMAX
        Terra
        tx3d
        Sound
        Util
        XTool
)

IF(PERIMETER_WINDOWS)
    #Add Windows and DirectX libs

    #DirectX 64 or 32 libs
    IF(OPTION_D3D9)
        IF(PERIMETER_ARCH_64)
            MESSAGE("Linking against DirectX 64 bits")
            SET(LIBS_MSDXSDK ${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Lib/x64)
        ELSE()
            MESSAGE("Linking against DirectX 32 bits")
            SET(LIBS_MSDXSDK ${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Lib/x86)
        ENDIF()
        LINK_DIRECTORIES(BEFORE ${LIBS_MSDXSDK})

        SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
                ${LIBS_MSDXSDK}/d3d9.lib
                ${LIBS_MSDXSDK}/d3dx9.lib
                ${LIBS_MSDXSDK}/DxErr.lib
        )
    ENDIF()

    IF(MSVC_CL_BUILD)
        SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
                legacy_stdio_definitions
        )
    ELSE()
        SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
                mingw32
        )
    ENDIF()
ELSE()
    IF(OPTION_D3D9)
        SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} ${DXVK_D3D9_LIB})
    ENDIF ()
ENDIF()

#Add TriggerEditor
if(NOT OPTION_FINAL_VERSION)
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} TriggerEditor)
ENDIF()


#Add gperftools
IF(OPTION_GPERFTOOLS)
    MESSAGE(WARNING "Linking gperftools")
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} -ltcmalloc -lprofiler)
ENDIF()

#Add XPrmLib if Debug build
IF(PERIMETER_DEBUG)
    ADD_DEPENDENCIES(dependencies XPrmLib)
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} XPrmLib)
ENDIF()

#FFMPEG
IF(FFMPEG_FOUND)
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} ${FFMPEG_LIBRARIES})
    IF(OPTION_FFMPEG_MOVIES)
        SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} ${FFMPEG_LIBSWSCALE_LIBRARIES} ${FFMPEG_LIBAVFILTER_LIBRARIES})
    ENDIF()
    #Optionally link since static ffmpeg builds require libswresample
    IF(FFMPEG_LIBSWRESAMPLE_FOUND)
        SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} ${FFMPEG_LIBSWRESAMPLE_LIBRARIES})
    ENDIF()
ENDIF()

IF(PERIMETER_EXODUS)
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} Exodus)
    IF (OPTION_D3D9)
        SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} ExodusD3D9)
        #Make Exodus depend on lib that provides D3D9 API
        ADD_DEPENDENCIES(ExodusD3D9 dxvk)
        #Make other modules depend on Exodus
        ADD_DEPENDENCIES(dependencies ExodusD3D9)
    ENDIF()
ENDIF()

IF(OPTION_PROCESS_SCRIPTS)
    ADD_DEPENDENCIES(dependencies ScriptsCompile)
    MESSAGE("Scripts will be compiled")
ELSE()
    MESSAGE("Not compiling Scripts")
ENDIF()
IF(OPTION_CHECK_SCRIPTS)
    ADD_DEPENDENCIES(dependencies ScriptsCheck)
    MESSAGE("Scripts will be checked")
ELSE()
    MESSAGE("Not checking Scripts")
ENDIF()

ADD_DEPENDENCIES(Util dependencies)
ADD_DEPENDENCIES(PluginMAX dependencies)
ADD_DEPENDENCIES(Terra dependencies)
ADD_DEPENDENCIES(tx3d dependencies)
ADD_DEPENDENCIES(Render dependencies)
ADD_DEPENDENCIES(Sound dependencies)
ADD_DEPENDENCIES(Network dependencies)
ADD_DEPENDENCIES(Units dependencies)
ADD_DEPENDENCIES(UserInterface dependencies)
ADD_DEPENDENCIES(Physics dependencies)
ADD_DEPENDENCIES(AI dependencies)
ADD_DEPENDENCIES(GeoLogic dependencies)
ADD_DEPENDENCIES(HT dependencies)
ADD_DEPENDENCIES(perimeter dependencies)
target_compile_options(perimeter PRIVATE ${PERIMETER_COMPILE_OPTIONS})

SET(perimeter_LINK_LIBS
        #Libraries
        ${EXE_LINK_LIBS_PRE}
        ${perimeter_LINK_LIBS}
        ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
        ${SDL2_MIXER_LIBRARY}
        ${SDL2_NET_LIBRARY}
        ${EXE_LINK_LIBS_POST}
)

IF(PERIMETER_WINDOWS)
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} ${SDL2MAIN_LIBRARY})
ENDIF()

target_link_libraries(perimeter PRIVATE ${perimeter_LINK_LIBS})

#Install rules
IF(OPTION_D3D9 AND NOT PERIMETER_WINDOWS)
    install (FILES ${DXVK_D3D9_LIB} TYPE LIB)
ENDIF()
install (TARGETS perimeter RUNTIME)
