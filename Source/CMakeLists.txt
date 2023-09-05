SET(perimeter_SRCS "")
SET(perimeter_LINK_LIBS
        #Our libs
        XTool
        PluginMAX
        Terra
        tx3d
        Render
        Sound
        Network
)

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

#simpleini
FetchContent_Declare(simpleini
        GIT_REPOSITORY    https://github.com/brofield/simpleini
        GIT_TAG           "7350fcc9228f410309734c3fb6dae2bf513cdd98"
        GIT_SHALLOW       OFF
)
FetchContent_MakeAvailable(simpleini)
INCLUDE_DIRECTORIES(${simpleini_SOURCE_DIR})

IF(NOT PERIMETER_WINDOWS)
    #Non Windows target specifics

    #pevents retrieval
    FetchContent_Declare(pevents
            GIT_REPOSITORY    https://github.com/neosmart/pevents
            GIT_TAG           "d6afcbc629cf806f6465238849278e530e1d56fb"
            GIT_SHALLOW       OFF
    )
    FetchContent_MakeAvailable(pevents)
    ADD_DEFINITIONS(-DWFMO)

    #Includes
    INCLUDE_DIRECTORIES(
            "${PROJECT_SOURCE_DIR}/Source/Exodus"
            "${pevents_SOURCE_DIR}/src"
    )

    IF (OPTION_D3D9)
        MESSAGE("Using DXVK for D3D9 API")

        SET(DXVK_BUILD_OPTIONS --buildtype=release -Denable_d3d9=true -Denable_d3d11=false -Denable_d3d10=false -Denable_dxgi=false)
        #DXVK wrapper lib retrieval and building
        IF (MACOS)
            IF (CMAKE_OSX_ARCHITECTURES)
                #Cross compile script, this assumes CMAKE_OSX_ARCHITECTURES only contains 1 arch
                SET(DXVK_BUILD_COMMAND
                        ${PROJECT_SOURCE_DIR}/macos/dxvk-native.sh
                        ${CMAKE_OSX_ARCHITECTURES}
                        ${PROJECT_SOURCE_DIR}/macos/
                )
            ENDIF()
            #TODO change this to upstream once DXVK2 works with MoltenVK and MacOS
            SET(OPTION_DXVK_1 ON)
        ENDIF()
        IF(OPTION_DXVK_1)
            set(DXVK_GIT_URL https://github.com/IonAgorria/dxvk-native)
            set(DXVK_GIT_TAG "43aedc756cbd620b9ee8b1cf2c17b17cc49b3781")
        ELSE()
            set(DXVK_GIT_URL https://github.com/doitsujin/dxvk)
            set(DXVK_GIT_TAG "80e075406b1b7d9d2508c9dc05e9745b3d3cf7e2") # v2.3
            set(DXVK_BUILD_OPTIONS ${DXVK_BUILD_OPTIONS} -Ddxvk_native_wsi=sdl2)
        ENDIF()
        IF("${DXVK_BUILD_COMMAND}" STREQUAL "")
            SET(DXVK_BUILD_COMMAND meson setup)
        ENDIF()
        IF(OPTION_DXVK_SOURCE_DIR)
            MESSAGE("DXVK Source path provided: ${OPTION_DXVK_SOURCE_DIR}")
            ExternalProject_Add(dxvk
                SOURCE_DIR        ${OPTION_DXVK_SOURCE_DIR}
                BUILD_ALWAYS      OFF
                CONFIGURE_HANDLED_BY_BUILD ON
                CONFIGURE_COMMAND ${DXVK_BUILD_COMMAND} ${OPTION_DXVK_SOURCE_DIR} ${DXVK_BUILD_OPTIONS}
                BUILD_COMMAND     ninja
                INSTALL_COMMAND   ""
            )
        ELSE()
            ExternalProject_Add(dxvk
                GIT_REPOSITORY    ${DXVK_GIT_URL}
                GIT_TAG           ${DXVK_GIT_TAG}
                GIT_SHALLOW       OFF
                BUILD_ALWAYS      OFF
                CONFIGURE_HANDLED_BY_BUILD ON
                CONFIGURE_COMMAND ${DXVK_BUILD_COMMAND} ../dxvk ${DXVK_BUILD_OPTIONS}
                BUILD_COMMAND     ninja
                INSTALL_COMMAND   ""
            )
        ENDIF()
        ExternalProject_Get_property(dxvk SOURCE_DIR BINARY_DIR)
        set(DXVK_INCLUDE_DIRS
                "${SOURCE_DIR}/include/native/directx"
                "${SOURCE_DIR}/include/native/windows"
        )
        IF (MACOS)
            set(DXVK_D3D9_LIB ${BINARY_DIR}/src/d3d9/libdxvk_d3d9.dylib)
        ELSE()
            set(DXVK_D3D9_LIB ${BINARY_DIR}/src/d3d9/libdxvk_d3d9.so)
        ENDIF()
    
        SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} ${DXVK_D3D9_LIB})
        INCLUDE_DIRECTORIES("${DXVK_INCLUDE_DIRS}")
    ENDIF()
ELSEIF(OPTION_D3D9)
    #DirectX SDK libs
    INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Include")
ENDIF()

#This is the only way I know to force download of external project from any targets
ADD_CUSTOM_TARGET(dependencies ALL)

ADD_SUBDIRECTORY("XPrm")
ADD_SUBDIRECTORY("Scripts")
ADD_SUBDIRECTORY("XTool")
ADD_SUBDIRECTORY("PluginMAX")
ADD_SUBDIRECTORY("Terra")
ADD_SUBDIRECTORY("tx3d")
ADD_SUBDIRECTORY("Render")
ADD_SUBDIRECTORY("Sound")
ADD_SUBDIRECTORY("Network")

IF(PERIMETER_EXODUS)
    MESSAGE("Using Exodus wrapper - WARNING: EXPERIMENTAL!")
    ADD_SUBDIRECTORY("Exodus")
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} Exodus)
    IF (OPTION_D3D9)
        #Make Exodus depend on lib that provides D3D9 API
        ADD_DEPENDENCIES(Exodus dxvk)
        #Make other modules depend on Exodus
        ADD_DEPENDENCIES(dependencies Exodus)
    ENDIF()
ENDIF()
if(NOT OPTION_FINAL_VERSION)
    ADD_SUBDIRECTORY("TriggerEditor")
ENDIF()

#Add Windows and DirectX libs
IF(PERIMETER_WINDOWS)
    #DirectX 64 or 32 libs
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
    
    #Use these libs as AVI fallback
    IF(NOT FFMPEG_FOUND)
        SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
            vfw32
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
ENDIF()

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}/Source/Game"
    "${PROJECT_SOURCE_DIR}/Source/XTool"
    "${PROJECT_SOURCE_DIR}/Source/XPrm" #For Statistics.h
    "${PROJECT_SOURCE_DIR}/Source/Util"
    "${PROJECT_SOURCE_DIR}/Source/Render/inc"
    "${PROJECT_SOURCE_DIR}/Source/Terra"
    "${PROJECT_SOURCE_DIR}/Source/UserInterface"
    "${PROJECT_SOURCE_DIR}/Source/GeoLogic"
    "${PROJECT_SOURCE_DIR}/Source/Network"
    "${PROJECT_SOURCE_DIR}/Source/Units"
    "${PROJECT_SOURCE_DIR}/Source/Render/client"
    "${PROJECT_SOURCE_DIR}/Source/Physics"
    "${PROJECT_SOURCE_DIR}/Source/AI"
    "${PROJECT_SOURCE_DIR}/Source/Sound"
    "${PROJECT_SOURCE_DIR}/Source/Scripts"
    "${PROJECT_SOURCE_DIR}/Source/tx3d"
    "${PROJECT_SOURCE_DIR}/Source/Game/Scripts"
)

SET(perimeter_SRCS ${perimeter_SRCS}
        Game/CameraManager.cpp
        Game/MonkManager.cpp
        Game/MusicManager.cpp
        Game/VideoPlayer.cpp
        Game/qd_textdb.cpp
        Game/Texts.cpp
        Units/Economic.cpp
        Game/Player.cpp
        Game/Universe.cpp
        Game/PerimeterDataChannel.cpp
        Game/Runtime.cpp
        Game/CopyToGraph.cpp
        Game/Region.cpp
        Game/GameContent.cpp
        Units/DefenceMap.cpp
        Units/GenericControls.cpp
        Units/GeoControl.cpp
        Units/Interpolation.cpp
        Units/MutationEngine.cpp
        Units/ToolzerController.cpp
        Units/UnitAttribute.cpp
        Units/GenericUnit.cpp
        Units/TrustMap.cpp
        Units/CorpseDynamic.cpp
        Units/Nature.cpp
        Units/RealInterpolation.cpp
        Units/RealUnit.cpp
        Units/IronBullet.cpp
        Units/IronExplosion.cpp
        Units/IronLegion.cpp
        Units/SecondGun.cpp
        Units/Squad.cpp
        Units/BuildingBlock.cpp
        Units/BuildMaster.cpp
        Units/FrameChild.cpp
        Units/FrameCore.cpp
        Units/FrameField.cpp
        Units/FrameLegion.cpp
        Units/FramePlant.cpp
        Units/IronBuilding.cpp
        Units/IronClusterUnit.cpp
        Units/IronDigger.cpp
        Units/IronFrame.cpp
        Units/IronPort.cpp
        Units/TerrainMaster.cpp
        Units/WarBuilding.cpp
        Units/FilthAnts.cpp
        Units/FilthCrow.cpp
        Units/FilthDaemon.cpp
        Units/FilthDragon.cpp
        Units/FilthEye.cpp
        Units/FilthGhost.cpp
        Units/FilthRat.cpp
        Units/FilthShark.cpp
        Units/FilthVolcano.cpp
        Units/FilthWasp.cpp
        Units/FilthWorm.cpp
        Units/GenericFilth.cpp
        Util/BinaryArchive.cpp
        Game/Config.cpp
        Util/DebugPrm.cpp
        Util/DebugUtil.cpp
        Util/EditArchive.cpp
        Util/MissionDescription.cpp
        Util/SaveSQSH.cpp
        Util/SaveConditions.cpp
        Util/SaveActions.cpp
        Util/SaveOthers.cpp
        Util/Save.cpp
        Util/AVWrapper.cpp
        Util/SynchroTimer.cpp
        Util/SystemUtil.cpp
        Util/XPrmArchive.cpp
        Util/Localization.cpp
        Util/ANIFile.cpp
        UserInterface/BGScene.cpp
        UserInterface/chaos.cpp
        UserInterface/controls.cpp
        UserInterface/ExternalShow.cpp
        UserInterface/SourceUIResolution.cpp
        UserInterface/GameShell.cpp
        UserInterface/HotKey.cpp
        UserInterface/Installer.cpp
        UserInterface/InterfaceScript.cpp
        UserInterface/LogicUpdater.cpp
        UserInterface/MissionEdit.cpp
        UserInterface/PerimeterShellDisp.cpp
        UserInterface/PerimeterShellUI.cpp
        UserInterface/ReelManager.cpp
        UserInterface/SelectManager.cpp
        UserInterface/ShellCmd.cpp 
        UserInterface/ShellLogDisp.cpp
        UserInterface/ShellUtils.cpp
        UserInterface/UserSingleProfile.cpp
        UserInterface/SCodeInterpreter/Commands.cpp
        UserInterface/SCodeInterpreter/Containers.cpp
        UserInterface/SCodeInterpreter/Frame.cpp
        UserInterface/SCodeInterpreter/Interpreter.cpp
        UserInterface/SCodeInterpreter/Knowledge.cpp
        UserInterface/SCodeInterpreter/World.cpp
        UserInterface/History3D.cpp
        UserInterface/HistoryScene.cpp
        UserInterface/HistorySceneCamera.cpp
        UserInterface/BelligerentSelect.cpp
        UserInterface/BattleMenu.cpp
        UserInterface/GraphicsOptions.cpp
        UserInterface/MainMenu.cpp
        UserInterface/Menu/SingleMenu.cpp
        UserInterface/Menu/AddonsMenu.cpp
        UserInterface/Menu/CommunityMenu.cpp
        UserInterface/Menu/MultiplayerList.cpp
        UserInterface/Menu/MultiplayerHost.cpp
        UserInterface/Menu/MultiplayerJoin.cpp
        UserInterface/Menu/MultiplayerLobby.cpp
        UserInterface/MessageBox.cpp
        UserInterface/MonoSelect.cpp
        UserInterface/OptionsMenu.cpp
        Physics/Missile.cpp
        Physics/MultiBodyDispatcher.cpp
        Physics/Mutation.cpp
        Physics/RigidBody.cpp
        Physics/Rocket.cpp
        AI/AiBuilding.cpp
        AI/AIMain.cpp
        AI/AITileMap.cpp
        AI/ClusterFind.cpp
        GeoLogic/LineAntiAlias.cpp
        GeoLogic/TerraCleft.cpp
        HT/ht.cpp
        HT/LagStatistic.cpp
        HT/StreamInterpolation.cpp
        Units/EnergyConsumer.cpp
        TriggerEditor/TriggerExport.cpp
        Units/Triggers.cpp
        UserInterface/Menu/MultiplayerCommon.cpp)

ADD_EXECUTABLE(perimeter ${PERIMETER_EXE_FLAGS} ${perimeter_SRCS})

#Add TriggerEditor
if(NOT OPTION_FINAL_VERSION)
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} TriggerEditor)
ENDIF()

#Add gperftools
IF(OPTION_GPERFTOOLS)
    MESSAGE("WARNING: Linking gperftools")
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} -ltcmalloc -lprofiler)
ENDIF()

#Add XPrmLib if Debug build
IF(PERIMETER_DEBUG)
    ADD_DEPENDENCIES(dependencies XPrmLib)
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} XPrmLib)
ENDIF()

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
    
TARGET_LINK_LIBRARIES(perimeter PRIVATE ${perimeter_LINK_LIBS})

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

ADD_DEPENDENCIES(PluginMAX dependencies)
ADD_DEPENDENCIES(Terra dependencies)
ADD_DEPENDENCIES(tx3d dependencies)
ADD_DEPENDENCIES(Render dependencies)
ADD_DEPENDENCIES(Sound dependencies)
ADD_DEPENDENCIES(Network dependencies)
ADD_DEPENDENCIES(perimeter dependencies)
target_compile_options(perimeter PRIVATE ${PERIMETER_COMPILE_OPTIONS})

#Install rules
IF(OPTION_D3D9 AND NOT PERIMETER_WINDOWS)
    install (FILES ${DXVK_D3D9_LIB} TYPE LIB)
ENDIF()
install (TARGETS perimeter RUNTIME)
