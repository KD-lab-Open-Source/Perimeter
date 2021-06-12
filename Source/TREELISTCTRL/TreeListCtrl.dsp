# Microsoft Developer Studio Project File - Name="TreeListCtrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TreeListCtrl - Win32 Debug Shared MT
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TreeListCtrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TreeListCtrl.mak" CFG="TreeListCtrl - Win32 Debug Shared MT"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TreeListCtrl - Win32 Release MT" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 Debug Static MT" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 Release Static MT" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 VC6 Debug Single Thread" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 VC6 Debug Static SingleThread" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 VC6 Release SingleThread" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 VC6 Release Static SingleThread" (based on "Win32 (x86) Static Library")
!MESSAGE "TreeListCtrl - Win32 Debug Shared MT" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TREELISTCTRL", VPPAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TreeListCtrl - Win32 Release MT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TreeListCtrl___Win32_Release MT"
# PROP BASE Intermediate_Dir "TreeListCtrl___Win32_Release MT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "VC6_Release_MT"
# PROP Intermediate_Dir "VC6_Release_MT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Output\xTreeListCtrlVC6_SH_MT.lib"

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 Debug Static MT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug Static MT"
# PROP BASE Intermediate_Dir "Debug Static MT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "VC6_DebugStatic"
# PROP Intermediate_Dir "VC6_DebugStatic"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC6_Debug\TreeListCtrlVC6_SH_MT_D.lib"
# ADD LIB32 /nologo /out:"Output\xTreeListCtrlVC6_ST_MT_D.lib"

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 Release Static MT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release Static MT"
# PROP BASE Intermediate_Dir "Release Static MT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "VC6_Release Static MT"
# PROP Intermediate_Dir "VC6_Release Static MT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC6_Release\TreeListCtrlVC6_SH_MT.lib"
# ADD LIB32 /nologo /out:"Output\xTreeListCtrlVC6_ST_MT.lib"

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Debug Single Thread"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "VC6 Debug Single Thread"
# PROP BASE Intermediate_Dir "VC6 Debug Single Thread"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "VC6 Debug Single Thread"
# PROP Intermediate_Dir "VC6 Debug Single Thread"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC6_Debug\TreeListCtrlVC6_SH_MT_D.lib"
# ADD LIB32 /nologo /out:"Output\xTreeListCtrlVC6_SH_D.lib"

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Debug Static SingleThread"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "VC6 Debug Static SingleThread"
# PROP BASE Intermediate_Dir "VC6 Debug Static SingleThread"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "VC6 Debug Static SingleThread"
# PROP Intermediate_Dir "VC6 Debug Static SingleThread"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC6_Debug\TreeListCtrlVC6_ST_MT_D.lib"
# ADD LIB32 /nologo /out:"Output\xTreeListCtrlVC6_ST_D.lib"

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Release SingleThread"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "VC6 Release SingleThread"
# PROP BASE Intermediate_Dir "VC6 Release SingleThread"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "VC6 Release SingleThread"
# PROP Intermediate_Dir "VC6 Release SingleThread"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC6_Release\TreeListCtrlVC6_SH_MT.lib"
# ADD LIB32 /nologo /out:"Output\xTreeListCtrlVC6_SH.lib"

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 VC6 Release Static SingleThread"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "VC6 Release Static SingleThread"
# PROP BASE Intermediate_Dir "VC6 Release Static SingleThread"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "VC6 Release Static SingleThread"
# PROP Intermediate_Dir "VC6 Release Static SingleThread"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"VC6_Release\TreeListCtrlVC6_ST_MT.lib"
# ADD LIB32 /nologo /out:"Output\xTreeListCtrlVC6_ST.lib"

!ELSEIF  "$(CFG)" == "TreeListCtrl - Win32 Debug Shared MT"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TreeListCtrl___Win32_Debug_Shared_MT"
# PROP BASE Intermediate_Dir "TreeListCtrl___Win32_Debug_Shared_MT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Shared_MT"
# PROP Intermediate_Dir "Debug_Shared_MT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"Output\xTreeListCtrlVC6_ST_MT_D.lib"
# ADD LIB32 /nologo /out:"Output\xTreeListCtrlVC6_SH_MT_D.lib"

!ENDIF 

# Begin Target

# Name "TreeListCtrl - Win32 Release MT"
# Name "TreeListCtrl - Win32 Debug Static MT"
# Name "TreeListCtrl - Win32 Release Static MT"
# Name "TreeListCtrl - Win32 VC6 Debug Single Thread"
# Name "TreeListCtrl - Win32 VC6 Debug Static SingleThread"
# Name "TreeListCtrl - Win32 VC6 Release SingleThread"
# Name "TreeListCtrl - Win32 VC6 Release Static SingleThread"
# Name "TreeListCtrl - Win32 Debug Shared MT"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\NullCustomDrawNotifyListener.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TLCDragWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TLCDropWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TLHDragWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TLHDropWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeListColumnInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeListComboCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeListCtrl.rc
# End Source File
# Begin Source File

SOURCE=.\TreeListDC.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeListEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeListHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeListItem.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeListStaticCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeListTipCtrl.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ICustomDrawNotifyListener.h
# End Source File
# Begin Source File

SOURCE=.\NullCustomDrawNotifyListener.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TLCDragWnd.h
# End Source File
# Begin Source File

SOURCE=.\TLCDropWnd.h
# End Source File
# Begin Source File

SOURCE=.\TLHDragWnd.h
# End Source File
# Begin Source File

SOURCE=.\TLHDropWnd.h
# End Source File
# Begin Source File

SOURCE=.\TreeListColumnInfo.h
# End Source File
# Begin Source File

SOURCE=.\TreeListComboCtrl.h
# End Source File
# Begin Source File

SOURCE=.\TreeListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\TreeListDC.h
# End Source File
# Begin Source File

SOURCE=.\TreeListEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\TreeListHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\TreeListItem.h
# End Source File
# Begin Source File

SOURCE=.\TreeListStaticCtrl.h
# End Source File
# Begin Source File

SOURCE=.\TreeListTipCtrl.h
# End Source File
# Begin Source File

SOURCE=.\xTreeCtrlLib.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\cur_button.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur_checkbox.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur_cross.cur
# End Source File
# Begin Source File

SOURCE=.\res\CUR_DROP.CUR
# End Source File
# Begin Source File

SOURCE=.\res\CUR_HORZ.CUR
# End Source File
# Begin Source File

SOURCE=.\res\cur_lockbox.cur
# End Source File
# Begin Source File

SOURCE=.\res\CUR_SIZE.CUR
# End Source File
# Begin Source File

SOURCE=.\res\cur_split.cur
# End Source File
# Begin Source File

SOURCE=.\res\CUR_STOP.CUR
# End Source File
# Begin Source File

SOURCE=.\res\CUR_VERT.CUR
# End Source File
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
