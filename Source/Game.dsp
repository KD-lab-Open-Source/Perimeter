# Microsoft Developer Studio Project File - Name="Game" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Game - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Game.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Game.mak" CFG="Game - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Game - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Game - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Game - Win32 Final" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Perimeter/GAME", AFEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Game - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "\Garbage/Perimeter/Release/Game"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /Gi /GR /GX /Zi /O2 /I "Util" /I "XSocket" /I "Render\INC" /I "Terra" /I "Game" /I "UserInterface" /I "GeoLogic" /I "Network" /I "Units" /I "Render\Client" /I "Physics" /I "AI" /I "Squad" /I "Sound" /I "Sound\Mpp\PlayMpp" /I "Scripts" /I "tx3d" /I "Game\scripts" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_PERIMETER_" /D "_TX3D_LIBRARY_" /FAcs /FR /Yu"stdafx.h" /FD /G7 /O3 /Zm150 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /n
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib Comdlg32.lib PlayOggMT.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib ole32.lib dsound.lib Comctl32.lib dxerr9.lib libboost_serialization.lib /nologo /subsystem:windows /profile /map:"Game.map" /debug /machine:I386

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "\Garbage/Perimeter/DEBUG/Game"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "Util" /I "XSocket" /I "Render\INC" /I "Terra" /I "Game" /I "UserInterface" /I "GeoLogic" /I "Network" /I "Units" /I "Render\Client" /I "Physics" /I "AI" /I "Squad" /I "Sound" /I "Sound\Mpp\PlayMpp" /I "Scripts" /I "tx3d" /I "Game\scripts" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_PERIMETER_" /D _STLP_SHRED_BYTE=0xA3 /D "_TX3D_LIBRARY_" /FR /Yu"StdAfx.h" /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /n
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 PlayOggDbgMT.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib ole32.lib dsound.lib Comctl32.lib dxerr9.lib libboost_serialization_debug.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"GameDBG.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "Game - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Game___Win32_Final"
# PROP BASE Intermediate_Dir "Game___Win32_Final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "\Garbage/Perimeter/Final/Game"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gi /GR /GX /Zi /O2 /I "Util" /I "XSocket" /I "Render\INC" /I "VisGeneric\INC" /I "Terra" /I "Game" /I "UserInterface" /I "GeoLogic" /I "Network" /I "Units" /I "VisGeneric\Client" /I "Physics" /I "AI" /I "Squad" /I "Sound" /I "Sound\Mpp\PlayMpp" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_PERIMETER_" /Fr /Yu"StdAfx.h" /FD /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /MT /W3 /GR /GX /Zi /O2 /I "Util" /I "XSocket" /I "Render\INC" /I "Terra" /I "Game" /I "UserInterface" /I "GeoLogic" /I "Network" /I "Units" /I "Render\Client" /I "Physics" /I "AI" /I "Squad" /I "Sound" /I "Sound\Mpp\PlayMpp" /I "Scripts" /I "tx3d" /I "Game\scripts" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_PERIMETER_" /D "_FINAL_VERSION_" /D "_TX3D_LIBRARY_" /FAcs /FR /Yu"StdAfx.h" /FD /Zm150 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /n /n
# ADD BSC32 /nologo /n /n
LINK32=link.exe
# ADD BASE LINK32 XTool.lib XGraph.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dxguid.lib winmm.lib dsound.lib wsock32.lib prmedit.lib xmath.lib Comctl32.lib XTool.lib XGraph.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dxguid.lib winmm.lib dsound.lib wsock32.lib prmedit.lib xmath.lib Comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"VisPerimeter\LIB"
# SUBTRACT BASE LINK32 /map
# ADD LINK32 shell32.lib Comdlg32.lib PlayOggMT.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib ole32.lib dsound.lib Comctl32.lib dxerr9.lib libboost_serialization.lib /nologo /subsystem:windows /map:"Perimeter.map" /debug /machine:I386 /out:"Perimeter.exe" /FIXED:NO
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Game - Win32 Release"
# Name "Game - Win32 Debug"
# Name "Game - Win32 Final"
# Begin Group "Game"

# PROP Default_Filter ""
# Begin Group "Universe"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Units\Economic.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\Player.h
# End Source File
# Begin Source File

SOURCE=Game\Universe.cpp
# End Source File
# Begin Source File

SOURCE=Game\Universe.h
# End Source File
# End Group
# Begin Group "Runtime"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Game\PerimeterDataChannel.cpp
# End Source File
# Begin Source File

SOURCE=Game\ResourceDispatcher.cpp
# End Source File
# Begin Source File

SOURCE=Game\ResourceDispatcher.h
# End Source File
# Begin Source File

SOURCE=Game\Runtime.cpp
# End Source File
# Begin Source File

SOURCE=Game\Runtime.h
# End Source File
# End Group
# Begin Group "Region"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GAME\CopyToGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\GAME\Region.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\Region.h
# End Source File
# End Group
# Begin Source File

SOURCE=Game\CameraManager.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=Game\CameraManager.h
# End Source File
# Begin Source File

SOURCE=.\GAME\MonkManager.cpp
# End Source File
# Begin Source File

SOURCE=.\GAME\MonkManager.h
# End Source File
# Begin Source File

SOURCE=.\GAME\MusicManager.cpp
# End Source File
# Begin Source File

SOURCE=.\GAME\MusicManager.h
# End Source File
# Begin Source File

SOURCE=.\GAME\PlayBink.cpp
# End Source File
# Begin Source File

SOURCE=.\GAME\PlayBink.h
# End Source File
# Begin Source File

SOURCE=.\GAME\qd_textdb.cpp
# End Source File
# Begin Source File

SOURCE=.\GAME\qd_textdb.h
# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Util\Actions.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\Conditions.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\Conditions.h
# End Source File
# Begin Source File

SOURCE=Game\Config.cpp
# End Source File
# Begin Source File

SOURCE=Game\Config.h
# End Source File
# Begin Source File

SOURCE=.\Game\ConfigPro.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\DebugPrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\DebugPrm.h
# End Source File
# Begin Source File

SOURCE=.\Util\DebugUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\DebugUtil.h
# End Source File
# Begin Source File

SOURCE=.\Util\Dictionary.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\Dictionary.h
# End Source File
# Begin Source File

SOURCE=.\Util\EditArchive.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\EditArchive.h
# End Source File
# Begin Source File

SOURCE=.\Util\EnumWrapper.h
# End Source File
# Begin Source File

SOURCE=UTIL\Grid2D.h
# End Source File
# Begin Source File

SOURCE=UTIL\Handle.h
# End Source File
# Begin Source File

SOURCE=.\Util\Map2D.h
# End Source File
# Begin Source File

SOURCE=.\UTIL\MemoryPool.h
# End Source File
# Begin Source File

SOURCE=.\Util\MissionDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\MissionDescription.h
# End Source File
# Begin Source File

SOURCE=.\Util\ProTool.h
# End Source File
# Begin Source File

SOURCE=UTIL\SafeMath.h
# End Source File
# Begin Source File

SOURCE=.\Util\Save.h
# End Source File
# Begin Source File

SOURCE=.\Units\ScanPoly.h
# End Source File
# Begin Source File

SOURCE=.\Util\Serialization.cpp
# End Source File
# Begin Source File

SOURCE=Game\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=Game\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UTIL\SynchroTimer.cpp
# End Source File
# Begin Source File

SOURCE=UTIL\SynchroTimer.h
# End Source File
# Begin Source File

SOURCE=.\Util\SystemUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\SystemUtil.h
# End Source File
# Begin Source File

SOURCE=UTIL\Timers.h
# End Source File
# Begin Source File

SOURCE=.\UTIL\xmath.h
# End Source File
# Begin Source File

SOURCE=.\Util\XPrmArchive.cpp
# End Source File
# Begin Source File

SOURCE=.\Util\XPrmArchive.h
# End Source File
# End Group
# Begin Group "UserInterface"

# PROP Default_Filter ""
# Begin Group "SCodeInterpreter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Commander.hpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Commands.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Commands.hpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Containers.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Containers.hpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Controller.hpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Frame.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Frame.hpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Interpreter.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Interpreter.hpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Knowledge.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Knowledge.hpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\Object3D.hpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\World.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SCodeInterpreter\World.hpp
# End Source File
# End Group
# Begin Group "HistoryScene"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UserInterface\History3D.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\History3D.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\HistoryScene.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\HistoryScene.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\HistorySceneCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\HistorySceneCamera.h
# End Source File
# End Group
# Begin Group "Menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UserInterface\BattleMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GraphicsOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GraphicsOptions.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LanMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\MainMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\MainMenu.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\MessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\MessageBox.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\MonoSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\MonoSelect.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\OnlineMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\OptionsMenu.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\UserInterface\BGScene.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\BGScene.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\chaos.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\chaos.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\controls.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\Controls.h
# End Source File
# Begin Source File

SOURCE=.\RESOURCE\controls.ini
# End Source File
# Begin Source File

SOURCE=.\UserInterface\ExternalShow.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\ExternalShow.h
# End Source File
# Begin Source File

SOURCE=UserInterface\GameShell.cpp
# End Source File
# Begin Source File

SOURCE=UserInterface\GameShell.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GameShellSq.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\HotKey.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\HotKey.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\Installer.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\Installer.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\InterfaceLogic.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogicData.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogicUpdater.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogicUpdater.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\MissionEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\MissionEdit.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\PerimeterShellDisp.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\PerimeterShellUI.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\PerimeterShellUI.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\ReelManager.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\ReelManager.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SelectManager.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SelectManager.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\ShellCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\ShellLogDisp.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\ShellUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\UniverseInterface.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\UserSingleProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\UserSingleProfile.h
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\CommonCommands.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CommonCommands.h
# End Source File
# Begin Source File

SOURCE=.\Network\CommonEvents.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CommonEvents.h
# End Source File
# Begin Source File

SOURCE=.\Network\ConnectionDP.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\ConnectionDP.h
# End Source File
# Begin Source File

SOURCE=.\Network\EventBufferDP.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\EventBufferDP.h
# End Source File
# Begin Source File

SOURCE=.\Network\GS_interface.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\GS_interface.h
# End Source File
# Begin Source File

SOURCE=.\Network\HyperSpace.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\HyperSpace.h
# End Source File
# Begin Source File

SOURCE=.\Network\NetPlayer.h
# End Source File
# Begin Source File

SOURCE=.\Network\P2P_interface.h
# End Source File
# Begin Source File

SOURCE=.\Network\P2P_interface1Th.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\P2P_interface2Th.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\P2P_interface2ThDPF.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\P2P_interface3Th.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\P2P_interfaceAnyTh.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\P2P_interfaceAux.h
# End Source File
# End Group
# Begin Group "Units"

# PROP Default_Filter ""
# Begin Group "Objects"

# PROP Default_Filter ""
# Begin Group "Trust"

# PROP Default_Filter ""
# Begin Source File

SOURCE=Units\TrustMap.cpp
# End Source File
# Begin Source File

SOURCE=Units\TrustMap.h
# End Source File
# End Group
# Begin Source File

SOURCE=Units\GenericUnit.cpp
# End Source File
# Begin Source File

SOURCE=Units\GenericUnit.h
# End Source File
# End Group
# Begin Group "Realm"

# PROP Default_Filter ""
# Begin Group "IronLegion"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Units\IronBullet.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\IronBullet.h
# End Source File
# Begin Source File

SOURCE=.\Units\IronExplosion.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\IronExplosion.h
# End Source File
# Begin Source File

SOURCE=.\Units\IronLegion.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\IronLegion.h
# End Source File
# Begin Source File

SOURCE=.\Units\LaunchData.h
# End Source File
# Begin Source File

SOURCE=.\Units\SecondGun.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\SecondGun.h
# End Source File
# Begin Source File

SOURCE=.\Units\Squad.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\Squad.h
# End Source File
# End Group
# Begin Group "IronFrame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Units\BuildingBlock.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\BuildingBlock.h
# End Source File
# Begin Source File

SOURCE=.\Units\BuildMaster.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\BuildMaster.h
# End Source File
# Begin Source File

SOURCE=.\Units\FrameChild.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FrameChild.h
# End Source File
# Begin Source File

SOURCE=.\Units\FrameCore.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FrameCore.h
# End Source File
# Begin Source File

SOURCE=.\Units\FrameField.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FrameField.h
# End Source File
# Begin Source File

SOURCE=.\Units\FrameLegion.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FrameLegion.h
# End Source File
# Begin Source File

SOURCE=.\Units\FramePlant.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FramePlant.h
# End Source File
# Begin Source File

SOURCE=.\Units\IronBuilding.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\IronBuilding.h
# End Source File
# Begin Source File

SOURCE=.\Units\IronClusterUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\IronClusterUnit.h
# End Source File
# Begin Source File

SOURCE=.\Units\IronDigger.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\IronDigger.h
# End Source File
# Begin Source File

SOURCE=.\Units\IronFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\IronFrame.h
# End Source File
# Begin Source File

SOURCE=.\Units\IronPort.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\IronPort.h
# End Source File
# Begin Source File

SOURCE=.\Units\TerrainMaster.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\TerrainMaster.h
# End Source File
# Begin Source File

SOURCE=.\Units\WarBuilding.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\WarBuilding.h
# End Source File
# End Group
# Begin Group "Filth"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Units\FilthAnts.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthAnts.h
# End Source File
# Begin Source File

SOURCE=.\Units\FilthCrow.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthCrow.h
# End Source File
# Begin Source File

SOURCE=.\Units\FilthDaemon.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthDaemon.h
# End Source File
# Begin Source File

SOURCE=.\Units\FilthDragon.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthDragon.h
# End Source File
# Begin Source File

SOURCE=.\Units\FilthEye.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthEye.h
# End Source File
# Begin Source File

SOURCE=.\Units\FilthGhost.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthGhost.h
# End Source File
# Begin Source File

SOURCE=.\Units\FilthRat.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthRat.h
# End Source File
# Begin Source File

SOURCE=.\Units\FilthShark.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthShark.h
# End Source File
# Begin Source File

SOURCE=.\Units\FilthVolcano.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthVolcano.h
# End Source File
# Begin Source File

SOURCE=.\Units\FilthWasp.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthWasp.h
# End Source File
# Begin Source File

SOURCE=.\Units\FilthWorm.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\FilthWorm.h
# End Source File
# Begin Source File

SOURCE=.\Units\GenericFilth.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\GenericFilth.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Units\CorpseDynamic.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\CorpseDynamic.h
# End Source File
# Begin Source File

SOURCE=.\Units\GridTools.h
# End Source File
# Begin Source File

SOURCE=.\Units\Nature.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\Nature.h
# End Source File
# Begin Source File

SOURCE=.\Units\RealInterpolation.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\RealInterpolation.h
# End Source File
# Begin Source File

SOURCE=.\Units\RealUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\RealUnit.h
# End Source File
# End Group
# Begin Group "Triggers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Units\EnergyConsumer.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\EnergyConsumer.h
# End Source File
# Begin Source File

SOURCE=.\Units\Triggers.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\Triggers.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Units\DefenceMap.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\DefenceMap.h
# End Source File
# Begin Source File

SOURCE=Units\GenericControls.cpp
# End Source File
# Begin Source File

SOURCE=Units\GenericControls.h
# End Source File
# Begin Source File

SOURCE=.\Units\GeoControl.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\GeoControl.h
# End Source File
# Begin Source File

SOURCE=Units\Interpolation.cpp
# End Source File
# Begin Source File

SOURCE=Units\Interpolation.h
# End Source File
# Begin Source File

SOURCE=Units\MutationEngine.cpp
# End Source File
# Begin Source File

SOURCE=Units\MutationEngine.h
# End Source File
# Begin Source File

SOURCE=.\Units\ToolzerController.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\ToolzerController.h
# End Source File
# Begin Source File

SOURCE=.\Units\TriggersExport.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\UnitAttribute.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\UnitAttribute.h
# End Source File
# Begin Source File

SOURCE=.\Units\UnitAttributeCache.cpp
# End Source File
# Begin Source File

SOURCE=.\Units\UnitAttributeCache.h
# End Source File
# End Group
# Begin Group "Physics"

# PROP Default_Filter ""
# Begin Group "Collision Detection"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Physics\2Do.txt
# End Source File
# Begin Source File

SOURCE=.\Physics\Missile.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics\MultiBodyDispatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics\Mutation.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics\Mutation.h
# End Source File
# Begin Source File

SOURCE=Physics\RigidBody.cpp
# End Source File
# Begin Source File

SOURCE=Physics\RigidBody.h
# End Source File
# Begin Source File

SOURCE=.\Physics\Rocket.cpp
# End Source File
# End Group
# Begin Group "AI"

# PROP Default_Filter ""
# Begin Group "PathFind"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AI\AIAStar.h
# End Source File
# Begin Source File

SOURCE=.\AI\ClusterFind.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\ClusterFind.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AI\AiBuilding.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\AIMain.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\AIMain.h
# End Source File
# Begin Source File

SOURCE=.\AI\AIPrm.h
# End Source File
# Begin Source File

SOURCE=.\AI\AITileMap.cpp
# End Source File
# Begin Source File

SOURCE=.\AI\AITileMap.h
# End Source File
# Begin Source File

SOURCE=.\AI\PlaceOperators.h
# End Source File
# Begin Source File

SOURCE=.\AI\PositionGenerator.h
# End Source File
# End Group
# Begin Group "GeoLogic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GeoLogic\LineAntiAlias.cpp
# End Source File
# Begin Source File

SOURCE=.\GeoLogic\TerraCleft.cpp
# End Source File
# Begin Source File

SOURCE=.\GeoLogic\TerraCleft.h
# End Source File
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SOUND\Scripts\interface.cfg

!IF  "$(CFG)" == "Game - Win32 Release"

# Begin Custom Build
InputPath=.\SOUND\Scripts\interface.cfg
InputName=interface

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

# Begin Custom Build
InputPath=.\SOUND\Scripts\interface.cfg
InputName=interface

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ELSEIF  "$(CFG)" == "Game - Win32 Final"

# Begin Custom Build
InputPath=.\SOUND\Scripts\interface.cfg
InputName=interface

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SOUND\Scripts\mainmenu.cfg

!IF  "$(CFG)" == "Game - Win32 Release"

# Begin Custom Build
InputPath=.\SOUND\Scripts\mainmenu.cfg
InputName=mainmenu

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

# Begin Custom Build
InputPath=.\SOUND\Scripts\mainmenu.cfg
InputName=mainmenu

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ELSEIF  "$(CFG)" == "Game - Win32 Final"

# Begin Custom Build
InputPath=.\SOUND\Scripts\mainmenu.cfg
InputName=mainmenu

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SOUND\Scripts\units.cfg

!IF  "$(CFG)" == "Game - Win32 Release"

# Begin Custom Build
InputPath=.\SOUND\Scripts\units.cfg
InputName=units

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

# Begin Custom Build
InputPath=.\SOUND\Scripts\units.cfg
InputName=units

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ELSEIF  "$(CFG)" == "Game - Win32 Final"

# Begin Custom Build
InputPath=.\SOUND\Scripts\units.cfg
InputName=units

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SOUND\Scripts\Voices.cfg

!IF  "$(CFG)" == "Game - Win32 Release"

# Begin Custom Build
InputPath=.\SOUND\Scripts\Voices.cfg
InputName=Voices

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ELSEIF  "$(CFG)" == "Game - Win32 Debug"

# Begin Custom Build
InputPath=.\SOUND\Scripts\Voices.cfg
InputName=Voices

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ELSEIF  "$(CFG)" == "Game - Win32 Final"

# Begin Custom Build
InputPath=.\SOUND\Scripts\Voices.cfg
InputName=Voices

".\RESOURCE\sounds\$(InputName).dat" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	TextConverter.exe $(InputPath) -W -O.\RESOURCE\sounds\$(InputName).dat

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "HT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HT\ht.cpp
# End Source File
# Begin Source File

SOURCE=.\HT\ht.h
# End Source File
# Begin Source File

SOURCE=.\HT\LagStatistic.cpp
# End Source File
# Begin Source File

SOURCE=.\HT\LagStatistic.h
# End Source File
# Begin Source File

SOURCE=.\HT\StreamInterpolation.cpp
# End Source File
# Begin Source File

SOURCE=.\HT\StreamInterpolation.h
# End Source File
# End Group
# Begin Source File

SOURCE=".\Perim icon XP3.ico"
# End Source File
# Begin Source File

SOURCE=.\Perimeter.ini
# End Source File
# Begin Source File

SOURCE=.\perimeter.rc
# End Source File
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# Begin Source File

SOURCE=.\Rudiments.txt
# End Source File
# End Target
# End Project
