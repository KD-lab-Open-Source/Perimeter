// TriggerEditor.cpp: implementation of the TriggerEditor class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "resource.h"
#include "TriggerExport.h"
#include "TriggerEditor.h"
#include "TriggerEditorApp.h"

#include "Tree/TETreeManager.h"
#include "Tree/TETreeLogic.h"

#include "TEEngine/TriggerEditorLogic.h"
#include "TEEngine/TriggerEditorView.h"

#include "TriggerExport.h"
#include "TEEngine/Profiler/TriggerChainProfiler/TriggerChainProfiler.h"
#include "TEEngine/Profiler/TriggerProfList.h"

#include "TEFrame.h"

static TriggerInterface* triggerInterfacePtr;

TriggerInterface& triggerInterface() 
{
	return *triggerInterfacePtr;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
std::pair<const CString, bool> AskUserForName(LPCTSTR lpszDef);

TriggerEditor::TriggerEditor(TriggerInterface& triggerInterface)
: dataChanged_(false)
, changesWasOnceSaved_(false)
{
	triggerInterfacePtr = &triggerInterface;
}

TriggerEditor::~TriggerEditor()
{

}

bool TriggerEditor::isDataSaved() const{
	return !dataChanged_;
}

void TriggerEditor::setDataChanged(bool value){
	dataChanged_ = value;
}

void TriggerEditor::setChangesWasOnceSaved(bool value /*= true*/){
	changesWasOnceSaved_ = value;
}
bool TriggerEditor::getChangesWasOnceSaved() const{
	return changesWasOnceSaved_;
}

bool TriggerEditor::createFrame(TEFrame* frame, HWND wndParent)
{
	if (!frame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW, NULL))
		return false;

	frame->setParent(wndParent);
	return true;
}

void TriggerEditor::closeFrame(TEFrame* frame)
{
	if (GetActiveWindow() != frame->getParent())
		SetActiveWindow(frame->getParent());
}

bool TriggerEditor::run(TriggerChain& triggerChain, HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
	CTriggerEditorApp theApp;
	theApp.m_hInstance = pModuleState->m_hCurrentInstanceHandle;
	theApp.SetCurrentHandles();
	theApp.InitInstance();

	setDataChanged(false);
	setChangesWasOnceSaved(false);

	TETreeManager teTreeManager;
	TETreeLogic teTreeLogic;
	TriggerEditorLogic logic(this);

	TriggerChainProfiler triggerChainProfiler;
	triggerChainProfiler.setTriggerChain(&triggerChain);

	TEFrame frame(this);
	frame.setTETreeManager(&teTreeManager);

	if (!createFrame(&frame, hWnd))
		return NULL;
	if (!initTETree(&teTreeManager, &teTreeLogic))
		return NULL;
	if (!initTriggerEditorLogic(logic, frame, triggerChainProfiler))
		return NULL;

	frame.ShowWindow(SW_MAXIMIZE);
	frame.updateViewSize();
	logic.setTriggerChain(&triggerChain);

	theApp.m_pMainWnd = &frame;
	theApp.Run();
	closeFrame(&frame);

	SetForegroundWindow(hWnd);

	triggerChainProfiler.setCurrentEntry(triggerChainProfiler.getEntriesCount() - 1);

	return getChangesWasOnceSaved();
}

bool TriggerEditor::initTETree(TETreeManager* mgr, TETreeLogic* logic)
{
	mgr->setTETreeNotifyListener(logic);
	logic->setTreeManager(mgr);

	if (!logic->init())
		return  false;
	if (!logic->loadTree())
		return false;
	return true;
}

bool TriggerEditor::initTriggerEditorLogic(TriggerEditorLogic& logic, 
										   TEFrame& frame,
										   TriggerChainProfiler& triggerChainProfiler)
{
	TriggerEditorView* ptrTriggerEditorView = frame.addTriggerEditorView();
	if (!ptrTriggerEditorView)
		return false;
	ptrTriggerEditorView->ModifyStyle(0, WS_HSCROLL|WS_VSCROLL);
	logic.setTriggerEditorView(ptrTriggerEditorView);
	ptrTriggerEditorView->setTriggerEditorLogic(&logic);


	TriggerProfList* ptrTriggerProfList = frame.getTriggerProfList();

	if (!initTriggerProfList(ptrTriggerProfList, 
						triggerChainProfiler, 
						ptrTriggerEditorView))
		return false;
	logic.setTriggerProfList(ptrTriggerProfList);
	ptrTriggerEditorView->OnInitialUpdate();
	return true;
}

bool TriggerEditor::initTriggerProfList(TriggerProfList* ptrTriggerProfList,
										TriggerChainProfiler& triggerChainProfiler, 
										ITriggerEditorView* ptrTriggerEditorView)
{
	ptrTriggerProfList->setTriggerEditorView(ptrTriggerEditorView);
	ptrTriggerProfList->setTriggerChainProfiler(&triggerChainProfiler);
	return true;
}

bool TriggerEditor::save()
{
	if (!isDataSaved())
		setChangesWasOnceSaved();
	setDataChanged(false);

	return true;
}