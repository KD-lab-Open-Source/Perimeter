; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CGeoToolApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "GeoTool.h"
LastPage=0

ClassCount=12
Class1=CGeoToolApp
Class3=CMainFrame
Class4=CAboutDlg

ResourceCount=11
Resource1=IDR_MAINFRAME
Class2=CChildView
Resource2=IDD_ABOUTBOX
Resource3=IDR_ACTION_MENU
Resource4=IDD_LOAD_MAP
Class5=CBrushDialog
Resource5=IDD_SLOT_PROPERTIES
Class6=CSlotPropertiesDialog
Resource6=IDD_INSTRUMENTS
Class7=CPreviewDialog
Resource7=IDR_MODELMENU
Class8=CSlots
Class9=CSlotButton
Class10=CInstrumentsDialog
Resource8=IDD_SLOTS
Class11=CLoadMapDialog
Resource9=IDD_INDEXER_DIALOG
Resource10=IDD_ABOUTBOX (English (U.S.))
Class12=CIndexerDialog
Resource11=IDR_MAINFRAME (English (U.S.))

[CLS:CGeoToolApp]
Type=0
HeaderFile=GeoTool.h
ImplementationFile=GeoTool.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=ID_BUTTON_ZSHIFT_PLUS

[CLS:CChildView]
Type=0
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp
Filter=N
BaseClass=CWnd 
VirtualFilter=WC
LastObject=CChildView

[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=ID_BUTTONZERO




[CLS:CAboutDlg]
Type=0
HeaderFile=GeoTool.cpp
ImplementationFile=GeoTool.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_LOAD_MAP
Command2=ID_APP_EXIT
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_VIEW_TOOLBAR
Command8=ID_VIEW_STATUS_BAR
Command9=ID_APP_ABOUT
CommandCount=9

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_NEXT_PANE
Command6=ID_PREV_PANE
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_CUT
Command10=ID_EDIT_UNDO
CommandCount=10

[TB:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
Command4=ID_FILE_PRINT
Command5=ID_APP_ABOUT
CommandCount=5

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_BUTTONZERO
Command2=ID_REMOVE_DAM
Command3=ID_PLANE_DAM
Command4=ID_REMOVE_DAM_LOCAL
Command5=ID_PLANE_DAM_LOCAL
Command6=ID_BUTTON_TOGGLE_INDEXED
Command7=ID_BTN_ZOOM
Command8=ID_BUTTON_ZSHIFT_PLUS
Command9=ID_BUTTON_ZSHIFT_MINUS
Command10=ID_APP_ABOUT
CommandCount=10

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_LOAD_MAP
Command2=ID_SAVE_TEXTURE
Command3=ID_LOAD_TEXTURE
Command4=ID_PUBLISH
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_SHOW_INSTRUMENTS
Command9=ID_SHOW_SLOT_PROPERTIES
Command10=ID_SHOW_SLOTS
Command11=ID_APP_ABOUT
CommandCount=11

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=ID_LOAD_MAP
Command6=ID_REFRESH
Command7=ID_NEXT_PANE
Command8=ID_PREV_PANE
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_EDIT_CUT
Command12=ID_EDIT_UNDO
CommandCount=12

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[CLS:CBrushDialog]
Type=0
HeaderFile=BrushDialog.h
ImplementationFile=BrushDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CBrushDialog
VirtualFilter=dWC

[DLG:IDD_SLOT_PROPERTIES]
Type=1
Class=CSlotPropertiesDialog
ControlCount=75
Control1=IDC_EDIT_STEP,edit,1082196096
Control2=IDC_TREE,SysTreeView32,1350631463
Control3=IDC_STATIC_TEXTURE,button,1073741831
Control4=IDC_STATIC_LOCATION,button,1342177287
Control5=IDC_STATIC_FG,static,1073872896
Control6=IDC_STATIC_BG,static,1073872896
Control7=IDC_BUTTON_FG,button,1073807488
Control8=IDC_BUTTON_BG,button,1073807488
Control9=IDC_EDIT_SHIFT_X,edit,1082196096
Control10=IDC_EDIT_SHIFT_Y,edit,1082196096
Control11=IDC_EDIT_SHIFT_Z,edit,1082196096
Control12=IDC_EDIT_SCALE_X,edit,1082196096
Control13=IDC_EDIT_SCALE_Y,edit,1082196096
Control14=IDC_EDIT_SCALE_Z,edit,1082196096
Control15=IDC_STATIC_SHIFT_X,static,1073872896
Control16=IDC_STATIC_SHIFT_Y,static,1073872896
Control17=IDC_STATIC_SHIFT_Z,static,1073872896
Control18=IDC_STATIC_FROM,static,1073872896
Control19=IDC_STATIC_TO,static,1073872896
Control20=IDC_EDIT_FROM_X,edit,1082196096
Control21=IDC_EDIT_FROM_Y,edit,1082196096
Control22=IDC_EDIT_FROM_Z,edit,1082196096
Control23=IDC_EDIT_TO_X,edit,1082196096
Control24=IDC_EDIT_TO_Y,edit,1082196096
Control25=IDC_EDIT_TO_Z,edit,1082196096
Control26=IDC_STATIC_FROM_X,static,1073872896
Control27=IDC_STATIC_FROM_Y,static,1073872896
Control28=IDC_STATIC_FROM_Z,static,1073872896
Control29=IDC_CHECK_BACK,button,1073807395
Control30=IDC_STATIC_STEP,static,1073872896
Control31=IDC_STATIC_GRADATION,static,1073872896
Control32=IDC_EDIT_GRADATION,edit,1082196096
Control33=IDC_STATIC_RING_COUNT,static,1073872896
Control34=IDC_RADIO_PLANE_X,button,1073741833
Control35=IDC_RADIO_PLANE_Y,button,1073741833
Control36=IDC_RADIO_PLANE_Z,button,1073741833
Control37=IDC_RADIO_PLANE_X2,button,1073741833
Control38=IDC_RADIO_PLANE_Y2,button,1073741833
Control39=IDC_RADIO_PLANE_Z2,button,1073741833
Control40=IDC_STATIC_PERSISTENCE,static,1073872896
Control41=IDC_STATIC_OCTAVE_COUNT,static,1073872896
Control42=IDC_EDIT_PERSISTENCE,edit,1082196096
Control43=IDC_EDIT_OCTAVE_COUNT,edit,1082196096
Control44=IDC_STATIC_BRICK,static,1073872896
Control45=IDC_STATIC_GAP,static,1073872896
Control46=IDC_EDIT_BRICK_X,edit,1082196096
Control47=IDC_EDIT_BRICK_Y,edit,1082196096
Control48=IDC_EDIT_BRICK_Z,edit,1082196096
Control49=IDC_EDIT_GAP_X,edit,1082196096
Control50=IDC_EDIT_GAP_Y,edit,1082196096
Control51=IDC_EDIT_GAP_Z,edit,1082196096
Control52=IDC_STATIC_BRICK_X,static,1073872896
Control53=IDC_STATIC_BRICK_Y,static,1073872896
Control54=IDC_STATIC_BRICK_Z,static,1073872896
Control55=IDC_STATIC_CENTER,static,1073872896
Control56=IDC_CHECK_IS_BRICK,button,1073807395
Control57=IDC_STATIC_NEAR_BOUND,static,1073872896
Control58=IDC_EDIT_NEAR_BOUND,edit,1082196096
Control59=IDC_STATIC_FAR_BOUND,static,1073872896
Control60=IDC_EDIT_FAR_BOUND,edit,1082196096
Control61=IDC_STATIC_RADIUS,static,1073872896
Control62=IDC_RADIO_PLANE_LOCATION_X,button,1073741833
Control63=IDC_RADIO_PLANE_LOCATION_Y,button,1073741833
Control64=IDC_RADIO_PLANE_LOCATION_Z,button,1073741833
Control65=IDC_STATIC_FADE,static,1073872896
Control66=IDC_EDIT_FADE,edit,1082196096
Control67=IDC_STATIC_FADING,static,1073872896
Control68=IDC_EDIT_FADING,edit,1082196096
Control69=IDC_PREVIEW_IMAGE,static,1073741838
Control70=IDC_STATIC_SHIFT_IMAGE,static,1073741838
Control71=IDC_STATIC_SCALE_IMAGE,static,1073741838
Control72=IDC_STATIC_PLANE_IMAGE,static,1073741838
Control73=IDC_STATIC_PLANE_IMAGE2,static,1073741838
Control74=IDC_EDIT_RADIUS,edit,1082196096
Control75=IDC_STATIC_PLANE_IMAGE3,static,1073741838

[CLS:CSlotPropertiesDialog]
Type=0
HeaderFile=SlotPropertiesDialog.h
ImplementationFile=SlotPropertiesDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_BUTTON_BG
VirtualFilter=dWC

[CLS:CPreviewDialog]
Type=0
HeaderFile=PreviewDialog.h
ImplementationFile=PreviewDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CPreviewDialog
VirtualFilter=dWC

[DLG:IDD_SLOTS]
Type=1
Class=CSlots
ControlCount=0

[CLS:CSlots]
Type=0
HeaderFile=Slots.h
ImplementationFile=Slots.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_EDIT_COPY
VirtualFilter=dWC

[CLS:CSlotButton]
Type=0
HeaderFile=slotbutton.h
ImplementationFile=slotbutton.cpp
BaseClass=CButton
Filter=W
VirtualFilter=BWC
LastObject=CSlotButton

[MNU:IDR_ACTION_MENU]
Type=1
Class=?
Command1=ID_CLEAR_SLOT
Command2=ID_PAINTER_SLOT
Command3=ID_MIXER_SLOT
CommandCount=3

[DLG:IDD_INSTRUMENTS]
Type=1
Class=CInstrumentsDialog
ControlCount=3
Control1=IDC_ARROW_BUTTON,button,1342181513
Control2=IDC_PIT_BRUSH,button,1342181513
Control3=IDC_PIT_BRUSH_BIG,button,1073746057

[CLS:CInstrumentsDialog]
Type=0
HeaderFile=InstrumentsDialog.h
ImplementationFile=InstrumentsDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_PIT_BRUSH
VirtualFilter=dWC

[DLG:IDD_LOAD_MAP]
Type=1
Class=CLoadMapDialog
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST2,listbox,1353777409

[CLS:CLoadMapDialog]
Type=0
HeaderFile=LoadMapDialog.h
ImplementationFile=LoadMapDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CLoadMapDialog

[MNU:IDR_MODELMENU]
Type=1
Class=?
Command1=ID_DELETE_MODEL_TX
Command2=ID_ADD_MODEL_TX
CommandCount=2

[DLG:IDD_INDEXER_DIALOG]
Type=1
Class=CIndexerDialog
ControlCount=19
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT_FROM_X,edit,1350631552
Control4=IDC_EDIT_FROM_Y,edit,1350631552
Control5=IDC_EDIT_FROM_Z,edit,1350631552
Control6=IDC_EDIT_TO_X,edit,1350631552
Control7=IDC_EDIT_TO_Y,edit,1350631552
Control8=IDC_EDIT_TO_Z,edit,1350631552
Control9=IDC_EDIT_STEP_X,edit,1350631552
Control10=IDC_EDIT_STEP_Y,edit,1350631552
Control11=IDC_EDIT_STEP_Z,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_EDIT_TRESHOLD,edit,1350631552
Control19=IDC_STATIC,static,1342308352

[CLS:CIndexerDialog]
Type=0
HeaderFile=IndexerDialog.h
ImplementationFile=IndexerDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CIndexerDialog
VirtualFilter=dWC

