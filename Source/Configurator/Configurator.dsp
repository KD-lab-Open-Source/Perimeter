# Microsoft Developer Studio Project File - Name="Configurator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Configurator - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Configurator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Configurator.mak" CFG="Configurator - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Configurator - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Configurator - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Configurator - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "/garbage/configurator/Release"
# PROP Intermediate_Dir "/garbage/configurator/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O1 /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 XUtilMT.lib /nologo /subsystem:windows /machine:I386 /out:"../Config.exe"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Configurator - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "/garbage/configurator/debug"
# PROP Intermediate_Dir "/garbage/configurator/debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 XUtilDBGMTDLL.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../Config.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Configurator - Win32 Release"
# Name "Configurator - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CollectLangDirs.cpp
# End Source File
# Begin Source File

SOURCE=.\Configurator.cpp
# End Source File
# Begin Source File

SOURCE=.\Configurator.rc
# End Source File
# Begin Source File

SOURCE=.\ConfiguratorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CurDirSafer.cpp
# End Source File
# Begin Source File

SOURCE=.\CurDirSafer.h
# End Source File
# Begin Source File

SOURCE=.\IniReader.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CollectLangDirs.h
# End Source File
# Begin Source File

SOURCE=.\Configurator.h
# End Source File
# Begin Source File

SOURCE=.\ConfiguratorDlg.h
# End Source File
# Begin Source File

SOURCE=.\file_enumerator.h
# End Source File
# Begin Source File

SOURCE=.\IniReader.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Configurator.ico
# End Source File
# Begin Source File

SOURCE=.\res\Configurator.rc2
# End Source File
# End Group
# Begin Group "Settings Sheets"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Config Sheets\aboutpage.cpp"
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\aboutpage.h"
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\BasePage.cpp"
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\BasePage.h"
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\GamePropertyPage.cpp"
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\GamePropertyPage.h"
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\graphsettingspage.cpp"
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\graphsettingspage.h"
# End Source File
# Begin Source File

SOURCE=.\GrCustomSettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GrCustomSettingsDlg.h
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\MiscSettingsPage.cpp"
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\MiscSettingsPage.h"
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\SettingsSheet.cpp"
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\SettingsSheet.h"
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\soundsettingspage.cpp"
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=".\Config Sheets\soundsettingspage.h"
# End Source File
# End Group
# Begin Group "Settings"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Settings\CheckSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\CheckSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\ColorDepthSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\ColorDepthSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\ComboSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\ComboSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\GraphSettingsSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\GraphSettingsSet.h
# End Source File
# Begin Source File

SOURCE=.\Settings\HTSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\HTSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\ISetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\LanguageSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\LanguageSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\MapLevelLOD.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\MapLevelLOD.h
# End Source File
# Begin Source File

SOURCE=.\Settings\MouseLookRateSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\MouseLookRateSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\RadioSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\RadioSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\ReflectionSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\ReflectionSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\ResolutionSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\ResolutionSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\ScrollRateSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\ScrollRateSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\SettingBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\SettingBase.h
# End Source File
# Begin Source File

SOURCE=.\Settings\ShadowQualitySetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\ShadowQualitySetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\ShadowSamples.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\ShadowSamples.h
# End Source File
# Begin Source File

SOURCE=.\Settings\ShadowTypeSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\ShadowTypeSetting.h
# End Source File
# Begin Source File

SOURCE=.\Settings\SliderSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings\SliderSetting.h
# End Source File
# End Group
# Begin Group "SettingFactory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SettingFactory\ISettingFactory.h
# End Source File
# Begin Source File

SOURCE=.\SettingFactory\SettingFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingFactory\SettingFactory.h
# End Source File
# End Group
# Begin Group "Localisation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Localisation\LocaleMngr.cpp
# End Source File
# Begin Source File

SOURCE=.\Localisation\LocaleMngr.h
# End Source File
# Begin Source File

SOURCE=.\qd_textdb.cpp
# End Source File
# Begin Source File

SOURCE=.\qd_textdb.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
