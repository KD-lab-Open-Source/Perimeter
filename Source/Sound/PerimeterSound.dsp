# Microsoft Developer Studio Project File - Name="PerimeterSound" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=PerimeterSound - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PerimeterSound.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PerimeterSound.mak" CFG="PerimeterSound - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PerimeterSound - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "PerimeterSound - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "PerimeterSound - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "PerimeterSound - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\Garbage/Perimeter/Release/Sound"
# PROP Intermediate_Dir "\Garbage/Perimeter/Release/Sound"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MT /W3 /Gm /Gi /GR /GX /Zi /O2 /I "..\Util" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /G7 /O3 /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "PerimeterSound - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\Garbage/Perimeter/DEBUG/Sound"
# PROP Intermediate_Dir "\Garbage/Perimeter/DEBUG/Sound"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\Util" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "PerimeterSound - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "PerimeterSound___Win32_Final"
# PROP BASE Intermediate_Dir "PerimeterSound___Win32_Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\Garbage\Perimeter\Final\PerimeterSound"
# PROP Intermediate_Dir "\Garbage\Perimeter\Final\PerimeterSound"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GR /GX /Zi /O2 /Ob2 /I "..\Util" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_FINAL_VERSION_" /Yu"stdafx.h" /FD /O3 /G7 /c
# SUBTRACT CPP /FA<none>
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "PerimeterSound - Win32 Release"
# Name "PerimeterSound - Win32 Debug"
# Name "PerimeterSound - Win32 Final"
# Begin Source File

SOURCE=.\C3D.cpp
# End Source File
# Begin Source File

SOURCE=.\C3D.h
# End Source File
# Begin Source File

SOURCE=.\init.cpp
# End Source File
# Begin Source File

SOURCE=.\paramblock.h
# End Source File
# Begin Source File

SOURCE=.\PerimeterSound.h
# End Source File
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# Begin Source File

SOURCE=.\SoftwareBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\SoftwareBuffer.h
# End Source File
# Begin Source File

SOURCE=.\SoundInternal.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WaveFile.cpp
# End Source File
# End Target
# End Project
