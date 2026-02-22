// EffectToolDoc.h : interface of the CEffectToolDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTTOOLDOC_H__92834159_15C2_458C_843B_5056264B2CB8__INCLUDED_)
#define AFX_EFFECTTOOLDOC_H__92834159_15C2_458C_843B_5056264B2CB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EffectStorage.h"

#define WM_SLIDER_UPDATE  (WM_USER+1)
#define WM_UPDATE_BAR     (WM_USER+2)
#define WM_ACTIVE_POINT   (WM_USER+3)

#define HINT_UPDATE_TREE   1

#define TYPE_3DBACK 0
#define TYPE_3DMODEL 1
#define MODE_FIND 0
#define MODE_LOAD 1


//// Project File Structure
class CEmitterDeskr : public CObject
{
public:
	CString m_name;
	// dummy values
	CString d1,d2,d3;
	int i1,i2,i3;
	
	CEmitterDeskr(){i1 = i2 = i3 = 0;};
	CEmitterDeskr(LPCTSTR name){m_name = name; i1 = i2 = i3 = 0;};
	virtual void Serialize(CArchive& ar);
};
class CEffectDeskr : public CObject
{
public:
	CString m_name,
			m_Path3DModel;
	CArray<CEmitterDeskr, CEmitterDeskr> m_emitter;
	// dummy values
	CString d2,d3;
	int m_bExpand,i2,i3;

	CEffectDeskr(){m_bExpand = i2 = i3 = 0;};
	CEffectDeskr(LPCTSTR name){m_name = name; m_bExpand = i2 = i3 = 0;};
	virtual void Serialize(CArchive& ar);
	void AddEmitter(LPCTSTR name);
};
class CGroupDescr : public CObject
{
public:
	EffectStorageType  m_effects;

	CString m_name,
			m_Path3DBack;
	bool	m_bShow3DBack;
	CArray<CEffectDeskr,CEffectDeskr> m_effect;
	// dummy values
	CString d2,d3;
	int m_bExpand,i2,i3;
	CGroupDescr(){i2 = i3 = 0; m_bShow3DBack = true; m_bExpand = true;};
	CGroupDescr(LPCTSTR name){ m_name = name; i2 = i3 = 0; m_bShow3DBack = true; m_bExpand = true;};
	void Init(){
		m_effect.RemoveAll();
	};
	void AddEffect(LPCTSTR name, LPCTSTR model_name, bool bExpand);
	void AddEmitter(LPCTSTR name);
	virtual void Serialize(CArchive& ar);
};
////

class CEffectToolDoc : public CDocument
{
protected: // create from serialization only
	CEffectToolDoc();
	DECLARE_DYNCREATE(CEffectToolDoc)

	void SaveEmitter(CEmitterData* pEmitter, LPCTSTR szPath);
	void SaveEffect(LPCTSTR strGroup, CEffectData* pEffect);
	bool LoadEffectFromFile(LPCTSTR szFolder);

	bool CheckGroupName(LPCTSTR lpsz);

	bool EmittersCheckDirty();
	void SaveAll();

	void SaveRegistry();
	void LoadRegistry();

	void LoadWorld();

public:

	CObArray m_group;

//	EffectStorageType  m_effects;
	CEffectData*		m_pActiveEffect;
	CEmitterData*		m_pActiveEmitter;
	CGroupDescr*		m_pActiveGroup;

	Vect3f				m_EffectCenter;
	Vect3f				m_NullCenter;
	bool m_EnableButForAll;
	int  m_nCurrentGenerationPoint;
	int  m_nCurrentParticlePoint;

//	int  m_nCurrentGroup;

	CString m_WorldPath;

	bool LoadProject(CString strProject);
	void SaveProject(void);

	bool  Load3DModel(int mode, int type);

	void  ResetEmitterType(EMITTER_CLASS cls);

	//интервал времени - от начала жизни эмитера до гибели последней его частицы - 
	//  масштабируется в [0..1]
	float get_gen_point_time(int nPoint, EmitterKeyBase* pEmitter = 0);
	void  set_gen_point_time(int nPoint, float tm, CEmitterData* pEmitter = NULL);

	float get_particle_key_time(int nGenPoint, int nParticleKey);
	void  set_particle_key_time(int nGenPoint, int nParticleKey, float tm);

	void  change_particle_life_time(int nGenPoint, float tm);

	void InitEmptyDocument();
	void SetPause();

	MatXf& GetSplineMatrix(MatXf& mat);

	bool CheckEffectName(LPCTSTR lpsz);

	void SaveCurrentEffect();
	void SaveCurrentEmitter();
	bool LoadEffect(LPCTSTR szFolder, LPCTSTR szFolderSprite);
	void LoadEmitter(CEffectData* pEffect, LPCTSTR szPath, LPCTSTR szFolderSprite, int mode = 1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectToolDoc)
	public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

public:
	virtual ~CEffectToolDoc();

protected:
	//{{AFX_MSG(CEffectToolDoc)
	afx_msg void OnToolbarSprite();
	afx_msg void OnUpdateToolbarSprite(CCmdUI* pCmdUI);
	afx_msg void OnTbPause();
	afx_msg void OnTbFirst();
	afx_msg void OnUpdateTbFirst(CCmdUI* pCmdUI);
	afx_msg void OnTbLast();
	afx_msg void OnUpdateTbLast(CCmdUI* pCmdUI);
	afx_msg void OnToolbarSave();
	afx_msg void OnUpdateToolbarSave(CCmdUI* pCmdUI);
	afx_msg void OnToolbarOpen();
	afx_msg void OnTbNext();
	afx_msg void OnUpdateTbNext(CCmdUI* pCmdUI);
	afx_msg void OnToolbarExport();
	afx_msg void OnUpdateToolbarExport(CCmdUI* pCmdUI);
	afx_msg void OnToolbarSaveall();
	//}}AFX_MSG
	void OnSelchangeComboPos();
	void OnSelchangeComboOut();
	void OnUpdateGeneral(CCmdUI* pCmdUI);
	void OnUpdateLabel(CCmdUI* pCmdUI);
	void OnUpdateTime(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
public:
	void MoveEffectCenter(float dx, float dy);
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnNewDocument();
	CGroupDescr* AddGroup(void);
	void DeleteCurrentGroup(void);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	CString m_StorePath;
	void Init(void);
	CString CopySprite(LPCTSTR name, LPCTSTR export_path = NULL);
	afx_msg void OnUpdateToolbarOpen(CCmdUI *pCmdUI);
	CString m_CurPath3DModel;
	afx_msg void OnUpdateSet3dmodel(CCmdUI *pCmdUI);
	afx_msg void OnSet3dmodel();
	afx_msg void OnUpdateShow3dmodel(CCmdUI *pCmdUI);
	afx_msg void OnShow3dmodel();
	afx_msg void OnUpdateSet3dbackground(CCmdUI *pCmdUI);
	afx_msg void OnSet3dbackground();
	afx_msg void OnShow3dbackground();
	afx_msg void OnUpdateShow3dbackground(CCmdUI *pCmdUI);
	void SetWorkingDir(void);
	int Release3DModel(void);
};

extern CEffectToolDoc* _pDoc;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTOOLDOC_H__92834159_15C2_458C_843B_5056264B2CB8__INCLUDED_)
