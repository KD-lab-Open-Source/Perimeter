# Microsoft Developer Studio Project File - Name="TriggerEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TriggerEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TriggerEditor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TriggerEditor.mak" CFG="TriggerEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TriggerEditor - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TriggerEditor - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TriggerEditor - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../"
# PROP Intermediate_Dir "\Garbage/Perimeter/Release/TriggerEditor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /Zi /O2 /I "..\Util" /I "..\Game" /I "..\Scripts" /I "./" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /D "_TRIGGER_EDITOR" /D "_STLP_USE_STATIC_LIB" /FR /Yu"stdafx.h" /FD /Zm150 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ScriptEditor.lib /nologo /subsystem:windows /dll /debug /machine:I386

!ELSEIF  "$(CFG)" == "TriggerEditor - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../"
# PROP Intermediate_Dir "\Garbage/Perimeter/Debug/TriggerEditor"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "./" /I "..\Util" /I "..\Game" /I "..\Scripts" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "_USRDLL" /D "_TRIGGER_EDITOR" /D "_STLP_USE_STATIC_LIB" /Yu"stdafx.h" /FD /GZ /Zm150 /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"\Garbage/Perimeter/Debug/TriggerEditor/TriggerEditor.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ScriptEditor.lib /nologo /subsystem:windows /dll /pdb:"TriggerEditor.pdb" /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "TriggerEditor - Win32 Release"
# Name "TriggerEditor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Main\ChooseName.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TEFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\TriggerEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\TriggerEditor.def
# End Source File
# Begin Source File

SOURCE=.\TriggerEditor.rc
# End Source File
# Begin Source File

SOURCE=.\TriggerEditorApp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Main\ChooseName.h
# End Source File
# Begin Source File

SOURCE=.\gdi_resource.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\ITriggerEditorView.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TEFrame.h
# End Source File
# Begin Source File

SOURCE=.\TriggerEditor.h
# End Source File
# Begin Source File

SOURCE=.\TriggerEditorApp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\draghand.cur
# End Source File
# Begin Source File

SOURCE=.\res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\te_main_.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\TriggerEditor.ico
# End Source File
# Begin Source File

SOURCE=.\res\TriggerEditor.rc2
# End Source File
# End Group
# Begin Group "Game"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Game\bibbs.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\GameInclude.h
# End Source File
# Begin Source File

SOURCE=.\Game\SaveAdapter.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\SaveAdapter.h
# End Source File
# Begin Source File

SOURCE=..\Units\TriggersExport.cpp
# End Source File
# End Group
# Begin Group "Tree"

# PROP Default_Filter ""
# Begin Group "TreeNodes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Tree\TreeNodes\ITreeNode.h
# End Source File
# Begin Source File

SOURCE=.\Tree\TreeNodes\UITreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Tree\TreeNodes\UITreeNode.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Tree\TETreeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Tree\TETreeDlg.h
# End Source File
# Begin Source File

SOURCE=.\Tree\TETreeLogic.cpp
# End Source File
# Begin Source File

SOURCE=.\Tree\TETreeLogic.h
# End Source File
# Begin Source File

SOURCE=.\Tree\TETreeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Tree\TETreeManager.h
# End Source File
# Begin Source File

SOURCE=.\Tree\TETreeNotifyListener.h
# End Source File
# Begin Source File

SOURCE=.\Tree\UITreeNodeFabric.cpp
# End Source File
# Begin Source File

SOURCE=.\Tree\UITreeNodeFabric.h
# End Source File
# End Group
# Begin Group "Custom Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Custom Controls\CoolDialogBar.cpp"
# End Source File
# Begin Source File

SOURCE=".\Custom Controls\CoolDialogBar.h"
# End Source File
# Begin Source File

SOURCE=".\Custom Controls\ToolTip.cpp"
# End Source File
# Begin Source File

SOURCE=".\Custom Controls\ToolTip.h"
# End Source File
# Begin Source File

SOURCE=".\Custom Controls\xTreeListCtrl.cpp"
# End Source File
# Begin Source File

SOURCE=".\Custom Controls\xTreeListCtrl.h"
# End Source File
# End Group
# Begin Group "ExternalGame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Util\Dictionary.cpp
# End Source File
# Begin Source File

SOURCE=..\Util\Dictionary.h
# End Source File
# Begin Source File

SOURCE=..\Util\TreeInterface.h
# End Source File
# Begin Source File

SOURCE=..\Util\XPrmPro.h
# End Source File
# Begin Source File

SOURCE=..\Util\XPrmProInternal.cpp
# End Source File
# Begin Source File

SOURCE=..\Util\XPrmProInternal.h
# End Source File
# End Group
# Begin Group "Scale"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Scale\ScaleBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Scale\ScaleBar.h
# End Source File
# Begin Source File

SOURCE=.\Scale\ScaleInterfaces.h
# End Source File
# Begin Source File

SOURCE=.\Scale\ScaleMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Scale\ScaleMgr.h
# End Source File
# End Group
# Begin Group "Utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Utils\AdjustComboboxWidth.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\AdjustComboboxWidth.h
# End Source File
# Begin Source File

SOURCE=.\Utils\EnsureTriggerNameOriginality.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\EnsureTriggerNameOriginality.h
# End Source File
# Begin Source File

SOURCE=.\Utils\TETreeLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils\TETreeLoader.h
# End Source File
# End Group
# Begin Group "TEEngine"

# PROP Default_Filter ""
# Begin Group "Рабочие режимы"

# PROP Default_Filter ""
# Begin Group "WMUtils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TEEngine\WorkModes\WMUtils\ClipBoardDataInserter.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\WMUtils\ClipBoardDataInserter.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\WMUtils\CopyToClipBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\WMUtils\CopyToClipboard.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\TEEngine\WorkModes\ITEWorkMode.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\TEBaseWorkMode.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\TEBaseWorkMode.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\TEProfilerWorkMode.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\TEProfilerWorkMode.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\TESingleChainWorkMode.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\TESingleChainWorkMode.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\TEWorkModeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\WorkModes\TEWorkModeBase.h
# End Source File
# End Group
# Begin Group "Прорисовка"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TEEngine\Drawers\BaseTEDrawer.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\BaseTEDrawer.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\IDrawData.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\ITEDrawer.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TEBaseWorkModeDrawer.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TEBaseWorkModeDrawer.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TEBaseWorkModeDrawerBase.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TEBaseWorkModeDrawerBase.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TEConditionColoredEleDrawer.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TEConditionColoredEleDrawer.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TEDrawerBase.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TEDrawerBase.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TESelfColoredEleDrawer.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TESelfColoredEleDrawer.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TEStatusColoredEleDrawer.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Drawers\TEStatusColoredEleDrawer.h
# End Source File
# End Group
# Begin Group "Traits"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TEEngine\Traits\ConditionTraits.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Traits\ConditionTraits.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Traits\LinkTraits.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Traits\LinkTraits.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Traits\StrategyTraits.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Traits\StrategyTraits.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Traits\TriggerTraits.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Traits\TriggerTraits.h
# End Source File
# End Group
# Begin Group "Actions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TEEngine\Actions\AddElement.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\AddElement.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\AddLink.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\AddLink.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\ChangeAutoRestart.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\ChangeAutoRestart.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\ChangeColor.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\ChangeColor.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\ChangeLinkType.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\ChangeLinkType.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\DeleteColumns.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\DeleteColumns.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\DeleteElement.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\DeleteElement.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\DeleteLink.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\DeleteLink.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\DeleteRows.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\DeleteRows.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\InsertColumns.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\InsertColumns.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\InsertRows.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\InsertRows.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\MakeLinkActive.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\MakeLinkActive.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\MoveLinkEnd.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\MoveLinkEnd.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\MoveTEElementPos.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\MoveTEElementPos.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\RenameElement.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Actions\RenameElement.h
# End Source File
# End Group
# Begin Group "UndoManager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TEEngine\UndoManager\TEUndoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\UndoManager\TEUndoManager.h
# End Source File
# End Group
# Begin Group "TEUtils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TEEngine\TEUtils\BoundingRectCalculator.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\BoundingRectCalculator.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\del_ins_col_row_helpers.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\FindLinkByPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\FindLinkByPoint.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\FindLinkInTriggerByChild.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\FindLinkInTriggerByChild.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\FindLinkInTriggerByPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\FindLinkInTriggerByPoint.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\FindTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\FindTrigger.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\ParentnessTest.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEUtils\ParentnessTest.h
# End Source File
# End Group
# Begin Group "Helpers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TEEngine\geometry_helpers.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\te_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\te_helpers.h
# End Source File
# End Group
# Begin Group "Отладка"

# PROP Default_Filter ""
# Begin Group "TriggerChainProfiler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TEEngine\Profiler\TriggerChainProfiler\ITriggerChainProfiler.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Profiler\TriggerChainProfiler\TriggerChainProfiler.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Profiler\TriggerChainProfiler\TriggerChainProfiler.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\TEEngine\Profiler\ITriggerProfList.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Profiler\TrgProfLegend.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Profiler\TrgProfLegend.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Profiler\TriggerDbgDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Profiler\TriggerDbgDlg.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Profiler\TriggerProfList.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\Profiler\TriggerProfList.h
# End Source File
# End Group
# Begin Group "ClipBuffer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TEEngine\TriggerClipBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TriggerClipBuffer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\TEEngine\SelectIfIntersect.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\SelectIfIntersect.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\SelectionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\SelectionManager.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\te_consts.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TEGrid.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TriggerEditorLogic.cpp
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TriggerEditorLogic.h
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TriggerEditorView.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\TEEngine\TriggerEditorView.h
# End Source File
# End Group
# End Target
# End Project
