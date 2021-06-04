# Microsoft Developer Studio Project File - Name="WinVG" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WinVG - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinVG.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinVG.mak" CFG="WinVG - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinVG - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "WinVG - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Perimeter/WinVG", MJMAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinVG - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinVG___Win32_Debug"
# PROP BASE Intermediate_Dir "WinVG___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\Garbage/WinVG/DEBUG/WinVG"
# PROP Intermediate_Dir "\Garbage/WinVG/DEBUG/WinVG"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "Render\inc" /I "VisGeneric\inc" /I "VisGeneric\client" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\Render\client" /I "..\Render\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xutilDbgMT.lib xmathDbgMT.lib /nologo /subsystem:windows /debug /machine:I386 /out:"WinVGdbg.exe" /pdbtype:sept
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 xutilDbgMT.lib /nologo /subsystem:windows /debug /machine:I386 /out:"WinVGdbg.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "WinVG - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinVG___Win32_Release"
# PROP BASE Intermediate_Dir "WinVG___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\Garbage/WinVG/Release/WinVG"
# PROP Intermediate_Dir "\Garbage/WinVG/Release/WinVG"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /GX /Zi /O2 /I "Render\inc" /I "VisGeneric\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_FINAL_VERSION_" /Fr /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /Zi /O2 /I "VisGeneric\inc" /I "Render\inc" /I "Render\client" /I "..\Render\client" /I "..\Render\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xutilMT.lib xmathMT.lib /nologo /subsystem:windows /debug /machine:I386 /out:"WinVG.exe"
# ADD LINK32 xutilMT.lib /nologo /subsystem:windows /debug /machine:I386 /out:"WinVG.exe"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "WinVG - Win32 Debug"
# Name "WinVG - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DirectoryTree.cpp
# End Source File
# Begin Source File

SOURCE=.\HierarchyTree.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenShotSize.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TextureDirectory.cpp
# End Source File
# Begin Source File

SOURCE=.\WinVG.cpp
# End Source File
# Begin Source File

SOURCE=.\WinVG.rc
# End Source File
# Begin Source File

SOURCE=.\WinVGDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\WinVGView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DirectoryTree.h
# End Source File
# Begin Source File

SOURCE=.\HierarchyTree.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\ModelInfo.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScreenShotSize.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TextureDirectory.h
# End Source File
# Begin Source File

SOURCE=.\WinVG.h
# End Source File
# Begin Source File

SOURCE=.\WinVGDoc.h
# End Source File
# Begin Source File

SOURCE=.\WinVGView.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WinVG.ico
# End Source File
# Begin Source File

SOURCE=.\res\WinVG.rc2
# End Source File
# Begin Source File

SOURCE=.\res\WinVGDoc.ico
# End Source File
# End Group
# End Target
# End Project
