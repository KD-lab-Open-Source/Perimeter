INCLUDE_DIRECTORIES(
        BEFORE .
        "${PROJECT_SOURCE_DIR}/Source/Util"
        "${PROJECT_SOURCE_DIR}/Source/Game"
        "${PROJECT_SOURCE_DIR}/Source/Scripts"
        "${PROJECT_SOURCE_DIR}/Source/XTool"
        "${PROJECT_SOURCE_DIR}/MSDXSDK_02_06/Include"
)

SET(TriggerEditor_SRCS
        Main/ChooseName.cpp
        StdAfx.cpp
        TEFrame.cpp
        TriggerEditor.cpp
        TriggerEditorApp.cpp
        TriggerExport.cpp
        Tree/TETreeDlg.cpp
        Tree/TETreeLogic.cpp
        Tree/TETreeManager.cpp
        Tree/UITreeNodeFabric.cpp
        Tree/TreeNodes/UITreeNode.cpp
        "Custom Controls/CoolDialogBar.cpp"
        "Custom Controls/ToolTip.cpp"
        "Custom Controls/xTreeListCtrl.cpp"
        Scale/ScaleBar.cpp
        Scale/ScaleMgr.cpp
        Utils/AdjustComboboxWidth.cpp
        Utils/EnsureTriggerNameOriginality.cpp
        Utils/TETreeLoader.cpp
        TEEngine/SelectIfIntersect.cpp
        TEEngine/SelectionManager.cpp
        TEEngine/TEGrid.cpp
        TEEngine/TriggerEditorLogic.cpp
        TEEngine/TriggerEditorView.cpp
        TEEngine/WorkModes/TEBaseWorkMode.cpp
        TEEngine/WorkModes/TEProfilerWorkMode.cpp
        TEEngine/WorkModes/TESingleChainWorkMode.cpp
        TEEngine/WorkModes/TEWorkModeBase.cpp
        TEEngine/WorkModes/WMUtils/ClipBoardDataInserter.cpp
        TEEngine/WorkModes/WMUtils/CopyToClipBoard.cpp
        TEEngine/Drawers/BaseTEDrawer.cpp
        TEEngine/Drawers/TEBaseWorkModeDrawer.cpp
        TEEngine/Drawers/TEBaseWorkModeDrawerBase.cpp
        TEEngine/Drawers/TEConditionColoredEleDrawer.cpp
        TEEngine/Drawers/TEDrawerBase.cpp
        TEEngine/Drawers/TESelfColoredEleDrawer.cpp
        TEEngine/Drawers/TEStatusColoredEleDrawer.cpp
        TEEngine/Actions/AddElement.cpp
        TEEngine/Actions/AddLink.cpp
        TEEngine/Actions/ChangeAutoRestart.cpp
        TEEngine/Actions/ChangeColor.cpp
        TEEngine/Actions/ChangeLinkType.cpp
        TEEngine/Actions/DeleteColumns.cpp
        TEEngine/Actions/DeleteElement.cpp
        TEEngine/Actions/DeleteLink.cpp
        TEEngine/Actions/DeleteRows.cpp
        TEEngine/Actions/InsertColumns.cpp
        TEEngine/Actions/InsertRows.cpp
        TEEngine/Actions/MakeLinkActive.cpp
        TEEngine/Actions/MoveLinkEnd.cpp
        TEEngine/Actions/MoveTEElementPos.cpp
        TEEngine/Actions/RenameElement.cpp
        TEEngine/UndoManager/TEUndoManager.cpp
        TEEngine/TEUtils/BoundingRectCalculator.cpp
        TEEngine/TEUtils/FindLinkByPoint.cpp
        TEEngine/TEUtils/FindLinkInTriggerByChild.cpp
        TEEngine/TEUtils/FindLinkInTriggerByPoint.cpp
        TEEngine/TEUtils/FindTrigger.cpp
        TEEngine/te_helpers.cpp
        TEEngine/Profiler/TrgProfLegend.cpp
        TEEngine/Profiler/TriggerDbgDlg.cpp
        TEEngine/Profiler/TriggerProfList.cpp
        TEEngine/Profiler/TriggerChainProfiler/TriggerChainProfiler.cpp
        TEEngine/TriggerClipBuffer.cpp
)

ADD_DEFINITIONS(-D_TRIGGER_EDITOR)

ADD_LIBRARY(TriggerEditor STATIC ${TriggerEditor_SRCS})

TARGET_LINK_LIBRARIES(TriggerEditor
        ${BOOST_LIBRARY}
)
