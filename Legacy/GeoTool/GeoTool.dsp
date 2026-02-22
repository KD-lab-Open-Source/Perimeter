# Microsoft Developer Studio Project File - Name="GeoTool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GeoTool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GeoTool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GeoTool.mak" CFG="GeoTool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GeoTool - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GeoTool - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "GeoTool - Win32 Final" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GeoTool - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\Garbage\Perimeter\RELEASE\GeoTool"
# PROP Intermediate_Dir "\Garbage\Perimeter\RELEASE\GeoTool"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "..\visgeneric\inc" /I "..\render\inc" /I "..\pluginmax\inc" /I "..\terra" /I "..\util" /I "..\tx3d" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_PERIMETER_" /D "_GEOTOOL_" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 XutilMT.lib winmm.lib Comctl32.lib ftkvc50.lib /nologo /subsystem:windows /machine:I386 /out:"../GeoTool.exe"
# SUBTRACT LINK32 /incremental:yes /nodefaultlib

!ELSEIF  "$(CFG)" == "GeoTool - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\Garbage\Perimeter\debug\GeoTool"
# PROP Intermediate_Dir "\Garbage\Perimeter\debug\GeoTool"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /I "..\visgeneric\inc" /I "..\render\inc" /I "..\pluginmax\inc" /I "..\terra" /I "..\util" /I "..\tx3d" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_PERIMETER_" /D "_GEOTOOL_" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 XutilDBGMT.lib winmm.lib Comctl32.lib ftkvc50d.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../GeoToolDBG.exe" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "GeoTool - Win32 Final"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GeoTool___Win32_Final"
# PROP BASE Intermediate_Dir "GeoTool___Win32_Final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\Garbage\Perimeter\final\GeoTool"
# PROP Intermediate_Dir "\Garbage\Perimeter\final\GeoTool"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "." /I "..\visgeneric\inc" /I "..\render\inc" /I "..\pluginmax\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_PERIMETER_" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "..\visgeneric\inc" /I "..\render\inc" /I "..\pluginmax\inc" /I "..\terra" /I "..\util" /I "..\tx3d" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_PERIMETER_" /D "_GEOTOOL_" /D "_FINAL_VERSION_" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 XutilMT.lib XGraphMT.lib xmathMT.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib ddraw.lib winmm.lib dsound.lib Comctl32.lib ftkvc50.lib prmeditMT.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBC.lib" /nodefaultlib:"LIBCMT.lib" /nodefaultlib:"libcimt.lib" /out:"../GeoTool.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 XutilMT.lib winmm.lib Comctl32.lib ftkvc50.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"../GeoTool.exe"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "GeoTool - Win32 Release"
# Name "GeoTool - Win32 Debug"
# Name "GeoTool - Win32 Final"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Group "MFCForms"

# PROP Default_Filter "*.h, *.cpp, *.hpp"
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\GeoTool.cpp
# End Source File
# Begin Source File

SOURCE=.\GeoTool.h
# End Source File
# Begin Source File

SOURCE=.\GeoTool.rc
# End Source File
# Begin Source File

SOURCE=.\IndexerDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\IndexerDialog.h
# End Source File
# Begin Source File

SOURCE=.\InstrumentsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\InstrumentsDialog.h
# End Source File
# Begin Source File

SOURCE=.\LoadMapDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadMapDialog.h
# End Source File
# Begin Source File

SOURCE=.\LocationUpdater.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SlotButton.cpp
# End Source File
# Begin Source File

SOURCE=.\SlotButton.h
# End Source File
# Begin Source File

SOURCE=.\SlotMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\SlotMenu.h
# End Source File
# Begin Source File

SOURCE=.\SlotPropertiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SlotPropertiesDialog.h
# End Source File
# Begin Source File

SOURCE=.\Slots.cpp
# End Source File
# Begin Source File

SOURCE=.\Slots.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Render"

# PROP Default_Filter "cpp;h;hpp"
# Begin Source File

SOURCE=.\ColorMapManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorMapManager.h
# End Source File
# Begin Source File

SOURCE=.\GeoToolTerraInterface.h
# End Source File
# Begin Source File

SOURCE=.\InputHandler.h
# End Source File
# Begin Source File

SOURCE=.\Operator.h
# End Source File
# Begin Source File

SOURCE=.\Runtime.cpp
# End Source File
# Begin Source File

SOURCE=.\Runtime.h
# End Source File
# Begin Source File

SOURCE=.\Updater.h
# End Source File
# Begin Source File

SOURCE=.\VMapOperator.cpp
# End Source File
# Begin Source File

SOURCE=.\VMapOperator.h
# End Source File
# End Group
# Begin Group "Logic"

# PROP Default_Filter "cpp;h;hpp"
# Begin Source File

SOURCE=.\ActionNode.cpp
# End Source File
# Begin Source File

SOURCE=.\ActionNode.h
# End Source File
# Begin Source File

SOURCE=.\LocationNode.cpp
# End Source File
# Begin Source File

SOURCE=.\LocationNode.h
# End Source File
# Begin Source File

SOURCE=.\ModelNode.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelNode.h
# End Source File
# Begin Source File

SOURCE=.\Node.cpp
# End Source File
# Begin Source File

SOURCE=.\Node.h
# End Source File
# Begin Source File

SOURCE=.\SimpleNode.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleNode.h
# End Source File
# Begin Source File

SOURCE=.\Slot.cpp
# End Source File
# Begin Source File

SOURCE=.\Slot.h
# End Source File
# Begin Source File

SOURCE=.\SlotManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SlotManager.h
# End Source File
# Begin Source File

SOURCE=.\TextureNode.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureNode.h
# End Source File
# Begin Source File

SOURCE=.\TreeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeManager.h
# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter "cpp;h;hpp"
# Begin Source File

SOURCE=.\Rect4i.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\arrow.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\colorPicker.bmp
# End Source File
# Begin Source File

SOURCE=.\res\default.bmp
# End Source File
# Begin Source File

SOURCE=.\res\GeoTool.ico
# End Source File
# Begin Source File

SOURCE=.\res\GeoTool.rc2
# End Source File
# Begin Source File

SOURCE=.\res\pitBrush.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pitBrushBig.bmp
# End Source File
# Begin Source File

SOURCE=.\res\plane.bmp
# End Source File
# Begin Source File

SOURCE=.\res\scale.bmp
# End Source File
# Begin Source File

SOURCE=.\res\shift.bmp
# End Source File
# Begin Source File

SOURCE=.\res\shift.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# End Group
# End Target
# End Project
