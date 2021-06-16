IF(GEN_SCRIPTS)
    MESSAGE("Generating Scripts")
    #TODO call XPrm and other stuff
ENDIF()

ADD_SUBDIRECTORY("XTool")
ADD_SUBDIRECTORY("PlayOgg")
ADD_SUBDIRECTORY("PluginMAX")
ADD_SUBDIRECTORY("Terra")
ADD_SUBDIRECTORY("tx3d")
if(NOT FINAL_VERSION)
    ADD_SUBDIRECTORY("TriggerEditor")
ENDIF()
ADD_SUBDIRECTORY("Network/Peer2")
ADD_SUBDIRECTORY("Render")
ADD_SUBDIRECTORY("Sound")

INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Source/Game"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/Source/PlayOgg"
        "${PROJECT_SOURCE_DIR}/Source/XPrm" #For Stadistics.h
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

SET(perimeter_LINK_LIBS
        #Our libs
        XTool
        PlayOgg
        PluginMAX
        Terra
        tx3d
        Peer
        Render
        Sound
)

# Boost dependencies
IF(NOT MSVC_CL_BUILD)
	SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
		${BOOST_LIBRARY}
		${SDL2_LIBRARY}
	)
ENDIF()

#Add Windows and DirectX libs
IF(PERIMETER_WINDOWS)
    #Include headers
    INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Include")
    
    #Select 64 or 32 compiled libs
    IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        MESSAGE("Linking against DirectX 64 bits")
        LINK_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Lib/x64)
    ELSE()
        MESSAGE("Linking against DirectX 32 bits")
        LINK_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Lib/x86)
    ENDIF()

    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
            #DirectX libs
            d3d8
            d3d9
            d3dx9
            d3dx9d
            d3d10
            d3dx10
            d3dx10d
            d3dxof
            ddraw
            dinput
            dsetup
            dinput8
            dplayx
            dsound
            dxgi
            dxtrans
            DxErr
            DxErr8
            DxErr9
            X3DAudio
            XInput
    
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

			Vorbis::vorbis
			Vorbis::vorbisenc
			Vorbis::vorbisfile
		)
	ENDIF()
ELSE()
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS}
		# dl is required for dladdr in stacktrace,
		dl
    )
ENDIF()

SET(perimeter_SRCS
        Util/dplay8guid.c
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
        Network/CommonCommands.cpp
        Network/CommonEvents.cpp
        Network/ConnectionDP.cpp
        Network/EventBufferDP.cpp
        Network/GS_interface.cpp
        Network/HyperSpace.cpp
        Network/P2P_interface1Th.cpp
        Network/P2P_interface2Th.cpp
        Network/P2P_interface2ThDPF.cpp
        Network/P2P_interface3Th.cpp
        Network/P2P_interfaceAnyTh.cpp
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

if(MSVC_CL_BUILD)
    SET(CMAKE_WIN32_EXECUTABLE YES)
ENDIF()

ADD_EXECUTABLE(perimeter ${perimeter_SRCS})

if(NOT FINAL_VERSION)
    SET(perimeter_LINK_LIBS ${perimeter_LINK_LIBS} TriggerEditor)
ENDIF()

TARGET_LINK_LIBRARIES(perimeter ${perimeter_LINK_LIBS})
