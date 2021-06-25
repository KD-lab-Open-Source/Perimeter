IF(NOT PERIMETER_WINDOWS)
    #Non Windows target specifics
    
    INCLUDE(FetchContent)
    INCLUDE(ExternalProject)    
    
    #Exodus wrapper libs
    INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Source/Exodus"
        "${pevents_SOURCE_DIR}/src"
    )

    #pevents retrieval
    FetchContent_Declare(pevents
            GIT_REPOSITORY    https://github.com/neosmart/pevents
            GIT_TAG           16967b99954e9fc077e07ea188d7886383bd5459 #latest commit
            GIT_SHALLOW       ON
    )
    FetchContent_MakeAvailable(pevents)

    #DXVK-native wrapper lib retrieval and building
    ExternalProject_Add(dxvk-native
            GIT_REPOSITORY    https://github.com/Joshua-Ashton/dxvk-native
            GIT_TAG           d3e43ed3d52cfabe7dc5c95e165d43b68d65da9a #latest commit
            GIT_SHALLOW       ON
            BUILD_ALWAYS      OFF
            CONFIGURE_COMMAND meson ../dxvk-native
            BUILD_COMMAND     ninja
            INSTALL_COMMAND   ""
            )
    ExternalProject_Get_property(dxvk-native SOURCE_DIR BINARY_DIR)
    set(DXVK_NATIVE_INCLUDE_DIRS
            "${SOURCE_DIR}/include/native/directx"
            "${SOURCE_DIR}/include/native/windows"
    )
    set(DXVK_NATIVE_D3D9_LIB ${BINARY_DIR}/src/d3d9/libdxvk_d3d9.so)
    
    INCLUDE_DIRECTORIES("${DXVK_NATIVE_INCLUDE_DIRS}")
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} ${DXVK_NATIVE_D3D9_LIB})
ELSE()
    #DirectX SDK libs
    INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Include")
ENDIF()


IF(OPTION_GEN_SCRIPTS)
    MESSAGE("Generating Scripts")
    #TODO call XPrm and other stuff
ENDIF()

ADD_SUBDIRECTORY("XTool")
ADD_SUBDIRECTORY("PluginMAX")
ADD_SUBDIRECTORY("Terra")
ADD_SUBDIRECTORY("tx3d")
ADD_SUBDIRECTORY("Render")
ADD_SUBDIRECTORY("Sound")
ADD_SUBDIRECTORY("Network")

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

IF(PERIMETER_EXODUS)
    MESSAGE("Using Exodus wrapper - WARNING: EXPERIMENTAL!")
    ADD_SUBDIRECTORY("Exodus")
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
        Exodus
        Threads::Threads
    )
ELSE()
    ADD_SUBDIRECTORY("PlayOgg")
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} PlayOgg)
ENDIF()
if(NOT OPTION_FINAL_VERSION)
    ADD_SUBDIRECTORY("TriggerEditor")
ENDIF()

IF(NOT PERIMETER_WINDOWS)
    #This is the only way I know to force download of external project from any targets

    ADD_CUSTOM_TARGET(dependencies ALL DEPENDS dxvk-native)

    ADD_DEPENDENCIES(Exodus dependencies)
    ADD_DEPENDENCIES(XTool dependencies)
    ADD_DEPENDENCIES(PluginMAX dependencies)
    ADD_DEPENDENCIES(Terra dependencies)
    ADD_DEPENDENCIES(tx3d dependencies)
    ADD_DEPENDENCIES(Render dependencies)
    ADD_DEPENDENCIES(Sound dependencies)
    ADD_DEPENDENCIES(Network dependencies)
    #ADD_DEPENDENCIES(PlayOgg dependencies)
ENDIF()

SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} ${SDL2_LIBRARY})

#Add Windows and DirectX libs
IF(PERIMETER_WINDOWS)
    #Select 64 or 32 compiled libs
    IF(PERIMETER_ARCH_64)
        MESSAGE("Linking against DirectX 64 bits")
        SET(LIBS_MSDXSDK ${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Lib/x64)
    ELSE()
        MESSAGE("Linking against DirectX 32 bits")
        SET(LIBS_MSDXSDK ${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Lib/x86)
    ENDIF()
    LINK_DIRECTORIES(BEFORE ${LIBS_MSDXSDK})

    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
        #DirectX libs
        ${LIBS_MSDXSDK}/d3d9.lib
        ${LIBS_MSDXSDK}/d3dx9.lib
        ${LIBS_MSDXSDK}/dsound.lib
        ${LIBS_MSDXSDK}/DxErr.lib

        #Windows libs
        winmm
        wsock32
        comctl32
        advapi32
        vfw32
        strmiids
    )

    IF(MSVC_CL_BUILD)
        SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
            legacy_stdio_definitions
        )
    ENDIF()
ELSE()
    #Add non windows dependencies    
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
        # dl is required for dladdr in stacktrace,
        dl
    )
ENDIF()

INCLUDE_DIRECTORIES(
    "${PROJECT_SOURCE_DIR}/Source/Game"
    "${PROJECT_SOURCE_DIR}/Source/XTool"
    "${PROJECT_SOURCE_DIR}/Source/PlayOgg"
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

SET(perimeter_SRCS
        Game/CameraManager.cpp
        Game/MonkManager.cpp
        Game/MusicManager.cpp
        Game/PlayBink.cpp
        Game/qd_textdb.cpp
        Units/Economic.cpp
        Game/Player.cpp
        Game/Universe.cpp
        Game/PerimeterDataChannel.cpp
        Game/Runtime.cpp
        Game/CopyToGraph.cpp
        Game/Region.cpp
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
        Util/SaveSQSH-1251.cpp
        Util/SaveConditions-1251.cpp
        Util/SaveActions-1251.cpp
        Util/SaveOthers-1251.cpp
        Util/Save.cpp
        Game/StdAfx.cpp
        Util/SynchroTimer.cpp
        Util/SystemUtil.cpp
        Util/XPrmArchive.cpp
        UserInterface/BGScene.cpp
        UserInterface/chaos.cpp
        UserInterface/controls.cpp
        UserInterface/ExternalShow.cpp
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
        UserInterface/BattleMenu.cpp
        UserInterface/GraphicsOptions.cpp
        UserInterface/LanMenu.cpp
        UserInterface/MainMenu.cpp
        UserInterface/MessageBox.cpp
        UserInterface/MonoSelect.cpp
        UserInterface/OnlineMenu.cpp
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
)

if(PERIMETER_WINDOWS)
    SET(CMAKE_WIN32_EXECUTABLE YES)
ENDIF()

ADD_EXECUTABLE(perimeter ${perimeter_SRCS})

if(NOT OPTION_FINAL_VERSION)
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} TriggerEditor)
ENDIF()

TARGET_LINK_LIBRARIES(perimeter ${perimeter_LINK_LIBS})
