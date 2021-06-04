# Microsoft Developer Studio Project File - Name="qr2ps2prodg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=qr2ps2prodg - Win32 Release_SNSystems
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qr2ps2prodg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qr2ps2prodg.mak" CFG="qr2ps2prodg - Win32 Release_SNSystems"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qr2ps2prodg - Win32 Debug_EENet" (based on "Win32 (x86) Console Application")
!MESSAGE "qr2ps2prodg - Win32 Debug_SNSystems" (based on "Win32 (x86) Console Application")
!MESSAGE "qr2ps2prodg - Win32 Release_EENet" (based on "Win32 (x86) Console Application")
!MESSAGE "qr2ps2prodg - Win32 Release_SNSystems" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Gamespy/GOA/qr2/qr2csample/qr2ps2prodg", ZTEDAAAA"
# PROP Scc_LocalPath "."
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qr2ps2prodg - Win32 Debug_EENet"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_EENet"
# PROP BASE Intermediate_Dir "Debug_EENet"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_EENet"
# PROP Intermediate_Dir "Debug_EENet"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Od /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /D "SN_TARGET_PS2" /Fo"PS2_EE_Debug/" /FD /debug /c
# ADD CPP /nologo /w /W0 /Od /I "c:\usr\local\sce\ee\include\libeenet" /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /D "EENET" /D "SN_TARGET_PS2" /D "_DEBUG" /FD /debug /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 libsn.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /pdb:none /debug /machine:IX86 /out:"PS2_EE_Debug\qr2ps2prodg.elf" /D:SN_TARGET_PS2
# ADD LINK32 eenetctl.a ent_smap.a ent_eth.a ent_ppp.a libeenet.a libscf.a libcdvd.a libsn.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /pdb:none /debug /machine:IX86 /out:"Debug_EENet\qr2ps2prodg.elf" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "qr2ps2prodg - Win32 Debug_SNSystems"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug_SNSystems"
# PROP BASE Intermediate_Dir "Debug_SNSystems"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_SNSystems"
# PROP Intermediate_Dir "Debug_SNSystems"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /Od /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /D "SN_TARGET_PS2" /Fo"PS2_EE_Debug/" /FD /debug /c
# ADD CPP /nologo /w /W0 /Od /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /D "_DEBUG" /D "SN_TARGET_PS2" /D "SN_SYSTEMS" /FD /debug /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 libsn.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /pdb:none /debug /machine:IX86 /out:"PS2_EE_Debug\qr2ps2prodg.elf" /D:SN_TARGET_PS2
# ADD LINK32 sneetcp.a libcdvd.a libsn.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /pdb:none /debug /machine:IX86 /out:"Debug_SNSystems\qr2ps2prodg.elf" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "qr2ps2prodg - Win32 Release_EENet"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Release_EENet"
# PROP BASE Intermediate_Dir "Release_EENet"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release_EENet"
# PROP Intermediate_Dir "Release_EENet"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /D "SN_TARGET_PS2" /Fo"PS2_EE_Release/" /FD /c
# ADD CPP /nologo /w /W0 /O2 /I "c:\usr\local\sce\ee\include\libeenet" /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /D "SN_TARGET_PS2" /D "EENET" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 libsn.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /pdb:none /machine:IX86 /out:"PS2_EE_Release\qr2ps2prodg.elf" /D:SN_TARGET_PS2
# ADD LINK32 eenetctl.a ent_smap.a ent_eth.a ent_ppp.a libeenet.a libscf.a libcdvd.a libsn.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /pdb:none /machine:IX86 /out:"Release_EENet\qr2ps2prodg.elf" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "qr2ps2prodg - Win32 Release_SNSystems"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Release_SNSystems"
# PROP BASE Intermediate_Dir "Release_SNSystems"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release_SNSystems"
# PROP Intermediate_Dir "Release_SNSystems"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /w /W0 /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /D "SN_TARGET_PS2" /Fo"PS2_EE_Release/" /FD /c
# ADD CPP /nologo /w /W0 /O2 /I "C:\usr\local\sce\ee\include" /I "C:\usr\local\sce\common\include" /D "SN_TARGET_PS2" /D "SN_SYSTEMS" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 libsn.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /pdb:none /machine:IX86 /out:"PS2_EE_Release\qr2ps2prodg.elf" /D:SN_TARGET_PS2
# ADD LINK32 sneetcp.a libcdvd.a libsn.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a /nologo /pdb:none /machine:IX86 /out:"Release_SNSystems\qr2ps2prodg.elf" /D:SN_TARGET_PS2

!ENDIF 

# Begin Target

# Name "qr2ps2prodg - Win32 Debug_EENet"
# Name "qr2ps2prodg - Win32 Debug_SNSystems"
# Name "qr2ps2prodg - Win32 Release_EENet"
# Name "qr2ps2prodg - Win32 Release_SNSystems"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\ps2common\ps2common.c
# End Source File
# Begin Source File

SOURCE=..\qr2csample.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\ps2common\prodg\PS2_in_VC.h
# End Source File
# End Group
# Begin Group "GSI"

# PROP Default_Filter ""
# Begin Group "QR2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\qr2.c
# End Source File
# Begin Source File

SOURCE=..\..\qr2.h
# End Source File
# Begin Source File

SOURCE=..\..\qr2regkeys.c
# End Source File
# Begin Source File

SOURCE=..\..\qr2regkeys.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\nonport.c
# End Source File
# Begin Source File

SOURCE=..\..\..\nonport.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=c:\usr\local\sce\ee\lib\app.cmd
# End Source File
# Begin Source File

SOURCE=c:\usr\local\sce\ee\lib\crt0.s
# End Source File
# Begin Source File

SOURCE=..\..\..\ps2common\prodg\ps2.lk
# End Source File
# End Target
# End Project
