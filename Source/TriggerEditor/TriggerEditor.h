// TriggerEditor.h: interface for the TriggerEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIGGEREDITOR_H__9948975E_15E2_4272_B228_43F0C2F0C04D__INCLUDED_)
#define AFX_TRIGGEREDITOR_H__9948975E_15E2_4272_B228_43F0C2F0C04D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _FINAL_VERSION_

#pragma warning(disable: 4251)
#ifdef _TRIGGER_EDITOR
#define DLL_INTERFACE __declspec(dllexport)
#else
#define DLL_INTERFACE __declspec(dllimport)
#pragma comment(lib,"TriggerEditor.lib")
#endif

class TEFrame;
class TETreeManager;
class TETreeLogic;
class TriggerEditorLogic;
class TriggerChainProfiler;
class TriggerProfList;

interface ITriggerEditorView;

class DLL_INTERFACE TriggerEditor  
{
public:
	TriggerEditor(TriggerInterface& triggerInterface);
	virtual ~TriggerEditor();

	bool run(TriggerChain& triggerChain, HWND hWnd);

	bool isDataSaved() const;
	void setDataChanged(bool value = true);
	bool save();

protected:
	bool createFrame(TEFrame* frame, HWND wndParent);
	void closeFrame(TEFrame* frame);
	bool initTETree(TETreeManager* mgr, TETreeLogic* logic);
	bool initTriggerEditorLogic(TriggerEditorLogic& logic, 
								TEFrame& frame,
								TriggerChainProfiler& triggerChainProfiler);
	bool initTriggerProfList(TriggerProfList* ptrTriggerProfList,
							TriggerChainProfiler& triggerChainProfiler, 
							ITriggerEditorView* ptrTriggerEditorView);
	void setChangesWasOnceSaved(bool value = true);
	bool getChangesWasOnceSaved() const;

private:
	bool dataChanged_;
	//! Изменения были хотя бы единожды сохранены
	bool changesWasOnceSaved_;
};

#endif

#endif // !defined(AFX_TRIGGEREDITOR_H__9948975E_15E2_4272_B228_43F0C2F0C04D__INCLUDED_)
