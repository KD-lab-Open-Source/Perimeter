// EffectToolDoc.cpp : implementation of the CEffectToolDoc class
//

#include "stdafx.h"
#include "EffectTool.h"

#include "EffectToolDoc.h"
#include "DlgLoadSprite.h"
#include "ControlView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CEffectToolDoc* _pDoc = 0;

void RepaintGraph();

/////////////////////////////////////////////////////////////////////////////
// CEffectToolDoc

IMPLEMENT_DYNCREATE(CEffectToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CEffectToolDoc, CDocument)
	//{{AFX_MSG_MAP(CEffectToolDoc)
	ON_COMMAND(ID_TOOLBAR_SPRITE, OnToolbarSprite)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_SPRITE, OnUpdateToolbarSprite)
	ON_COMMAND(ID_TB_PAUSE, OnTbPause)
	ON_COMMAND(ID_TB_FIRST, OnTbFirst)
	ON_UPDATE_COMMAND_UI(ID_TB_FIRST, OnUpdateTbFirst)
	ON_COMMAND(ID_TB_LAST, OnTbLast)
	ON_UPDATE_COMMAND_UI(ID_TB_LAST, OnUpdateTbLast)
	ON_COMMAND(ID_TOOLBAR_SAVE, OnToolbarSave)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_SAVE, OnUpdateToolbarSave)
	ON_COMMAND(ID_TOOLBAR_OPEN, OnToolbarOpen)
	ON_COMMAND(ID_TB_NEXT, OnTbNext)
	ON_UPDATE_COMMAND_UI(ID_TB_NEXT, OnUpdateTbNext)
	ON_COMMAND(ID_TOOLBAR_EXPORT, OnToolbarExport)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_EXPORT, OnUpdateToolbarExport)
	ON_COMMAND(ID_TOOLBAR_SAVEALL, OnToolbarSaveall)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_SAVEALL, OnUpdateToolbarSave)
	//}}AFX_MSG_MAP
/*	ON_UPDATE_COMMAND_UI(IDC_COMBO_POS_TYPE, OnUpdateGeneral)
	ON_UPDATE_COMMAND_UI(IDC_COMBO_OUT_TYPE, OnUpdateGeneral)
	ON_UPDATE_COMMAND_UI(IDC_SHOW, OnUpdateGeneral)
	ON_UPDATE_COMMAND_UI(IDC_STATIC_X, OnUpdateLabel)
	ON_UPDATE_COMMAND_UI(IDC_STATIC_Y, OnUpdateLabel)
	ON_UPDATE_COMMAND_UI(IDC_STATIC_Z, OnUpdateLabel)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_X, OnUpdateLabel)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_Y, OnUpdateLabel)
	ON_UPDATE_COMMAND_UI(IDC_EDIT_Z, OnUpdateLabel)*/
	ON_UPDATE_COMMAND_UI(IDC_EMITTER_LIFE_TIME, OnUpdateTime)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_OPEN, OnUpdateToolbarOpen)
	ON_UPDATE_COMMAND_UI(ID_SET3DMODEL, OnUpdateSet3dmodel)
	ON_COMMAND(ID_SET3DMODEL, OnSet3dmodel)
	ON_UPDATE_COMMAND_UI(ID_SHOW3DMODEL, OnUpdateShow3dmodel)
	ON_COMMAND(ID_SHOW3DMODEL, OnShow3dmodel)
	ON_UPDATE_COMMAND_UI(ID_SET3DBACKGROUND, OnUpdateSet3dbackground)
	ON_COMMAND(ID_SET3DBACKGROUND, OnSet3dbackground)
	ON_COMMAND(ID_SHOW3DBACKGROUND, OnShow3dbackground)
	ON_UPDATE_COMMAND_UI(ID_SHOW3DBACKGROUND, OnUpdateShow3dbackground)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectToolDoc construction/destruction

CEffectToolDoc::CEffectToolDoc()
: m_StorePath(_T(""))
, m_CurPath3DModel(_T(""))
{
	m_pActiveEffect = 0;
	m_pActiveEmitter = 0;
	m_pActiveGroup = 0;

	m_nCurrentGenerationPoint = 0;
	m_nCurrentParticlePoint = 0;

//	m_nCurrentGroup = 0;

	theApp.scene.SetDocument(this);

//	theApp.scene.LoadWorld();
	LoadWorld();

//	if(!LoadRegistry())
//	InitEmptyDocument();

	m_StorePath = "";

	_pDoc = this;

	m_EffectCenter = Vect3f::ZERO;
	m_NullCenter = Vect3f::ZERO;

	LoadRegistry();
}

CEffectToolDoc::~CEffectToolDoc()
{
	Init();

	SaveRegistry();
}

void CEffectToolDoc::Init(void)
{
	for(int i=0; i<m_group.GetSize(); i++)
		delete m_group[i];
	m_group.RemoveAll();

	m_pActiveGroup = NULL;
}

void CEffectToolDoc::InitEmptyDocument()
{
	m_group.Add(new CGroupDescr("Group1"));

	CEffectData* pNewEffect;
	((CGroupDescr*)m_group[0])->m_effects.push_back(pNewEffect = new CEffectData);

	pNewEffect->add_emitter();
}

void CEffectToolDoc::LoadRegistry()
{
	CString temp;

	temp = theApp.GetProfileString("EffectCenter", "x", "");
	float fx = atof(temp);
	temp = theApp.GetProfileString("EffectCenter", "y", "");
	float fy = atof(temp);
	temp = theApp.GetProfileString("EffectCenter", "z", "");
	float fz = atof(temp);

	m_EffectCenter.set(fx, fy, fz);
}
void CEffectToolDoc::SaveRegistry()
{
	CString temp;

	temp.Format("%f", m_EffectCenter.x);
	theApp.WriteProfileString("EffectCenter", "x", temp);
	temp.Format("%f", m_EffectCenter.y);
	theApp.WriteProfileString("EffectCenter", "y", temp);
	temp.Format("%f", m_EffectCenter.z);
	theApp.WriteProfileString("EffectCenter", "z", temp);
}

/*
void CEffectToolDoc::SaveRegistry()
{
	CString s;

	EffectStorageType::iterator i;
	FOR_EACH(m_effects, i)
	{
		s += (*i)->name.c_str();
		s += ";";
	}

	theApp.WriteProfileString("State", "Last", s);
}
bool CEffectToolDoc::LoadRegistry()
{
	CString s = theApp.GetProfileString("State", "Last");
	if(s.IsEmpty())
		return false;

	bool bRet = false;

	char cb[255];
	strncpy(cb, s, 255);

	char* token = strtok(cb, ";");
	while(token)
	{
		if(LoadEffect(theApp.m_szDir + token))
			bRet = true;

		token = strtok(0, ";");
	}
	return bRet;
}
*/
bool CEffectToolDoc::EmittersCheckDirty()
{
	EffectStorageType::iterator i;
	for(int g=0; g<m_group.GetSize(); g++)
	{
		FOR_EACH(((CGroupDescr*)m_group[g])->m_effects, i)
		{
			if((*i)->bDirty)
				return true;

			EmitterListType::iterator j;
			FOR_EACH((*i)->emitters, j)
				if((*j)->bDirty)
					return true;
		}
	}
	return false;
}
void CEffectToolDoc::SaveAll()
{
	for(int g=0; g<m_group.GetSize(); g++)
	{
		EffectStorageType::iterator i;
		FOR_EACH(((CGroupDescr*)m_group[g])->m_effects, i)
			SaveEffect(((CGroupDescr*)m_group[g])->m_name, *i);
	}
}

void CEffectToolDoc::SetPause()
{
	theApp.scene.bPause = true;

	POSITION pos = GetFirstViewPosition();
	while(pos)
	{
		CControlView* pView = dynamic_cast<CControlView*>(GetNextView(pos));
		if(pView)
			pView->SetPause();
	}   
}
MatXf& CEffectToolDoc::GetSplineMatrix(MatXf& mat)
{
	float fTime = m_pActiveEmitter->Effect2EmitterTime(theApp.scene.GetEffect()->GetTime());

	if(m_pActiveEmitter->data)
		m_pActiveEmitter->data->GetPosition(fTime, mat);
//	else
//		m_pActiveEmitter->data_light->GetPosition(fTime, mat);

	return mat;
}

bool CEffectToolDoc::CheckEffectName(LPCTSTR lpsz)
{
	ASSERT(m_pActiveGroup);
	EffectStorageType::iterator i;
	FOR_EACH(m_pActiveGroup->m_effects, i)
		if((*i)->name == lpsz)
			return false;

	return true;
}

bool  CEffectToolDoc::Load3DModel(int mode, int type)
{
	if(!m_pActiveEffect||!m_pActiveGroup)
		return false;

	if(mode==MODE_FIND){

		if(m_pActiveEffect->Path3Dmodel.IsEmpty()||!m_pActiveEffect->Show3Dmodel){
			RELEASE(theApp.scene.m_pModel);
		}
		if(m_pActiveGroup->m_Path3DBack.IsEmpty()||!m_pActiveGroup->m_bShow3DBack){
			RELEASE(theApp.scene.m_pBackModel);
		}

		if(type==TYPE_3DMODEL&&!m_pActiveEffect->Show3Dmodel)
			return false;
		if(type==TYPE_3DBACK&&!m_pActiveGroup->m_bShow3DBack)
			return false;

		CString path;
		if(type==TYPE_3DMODEL){
			path = m_pActiveEffect->Path3Dmodel;
		}else{
			path = m_pActiveGroup->m_Path3DBack;
		}

		CFileFind finder;
		if(!path.IsEmpty()&&
			finder.FindFile(path)){

			theApp.scene.LoadParticleModel(path, type);
			m_CurPath3DModel = m_pActiveEffect->Path3Dmodel;
			return true;
		}
	}
	else{
		CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY, "Models (*.m3d)|*.m3d|All files|*.*||");
		if(dlg.DoModal() == IDOK){
			CString name = dlg.GetPathName();
			if(type==TYPE_3DMODEL){
				if(m_pActiveEmitter)
					return theApp.scene.LoadModelInsideEmitter(name);
				m_pActiveEffect->Path3Dmodel = name;
				m_CurPath3DModel = m_pActiveEffect->Path3Dmodel;
			}else{
				m_pActiveGroup->m_Path3DBack = name;
			}
			theApp.scene.LoadParticleModel(name, type);
			return true;
		}
	}
	return false;
}

void  CEffectToolDoc::ResetEmitterType(EMITTER_CLASS cls)
{
	ASSERT(m_pActiveEmitter);

	if(m_pActiveEmitter->data){
		if(m_pActiveEmitter->data->GetType() != cls)
			m_pActiveEmitter->Reset(cls);
	}else{
		if(m_pActiveEmitter->data_light->GetType() != cls)
			m_pActiveEmitter->Reset(cls);
	}
	m_nCurrentGenerationPoint = 0;
	/*
	if(!m_StorePath.IsEmpty()){
		if(m_pActiveEmitter->data){
			m_pActiveEmitter->data->texture_name = FOLDER_SPRITE;
			m_pActiveEmitter->data->texture_name += DEFAULT_TXT_FIRE;
		}else{
			m_pActiveEmitter->data_light->texture_name = FOLDER_SPRITE;
			m_pActiveEmitter->data_light->texture_name += DEFAULT_TXT_LIGHT;
		}
	}
	*/
}

float CEffectToolDoc::get_gen_point_time(int nPoint, EmitterKeyBase* pEmitter)
{
	if(pEmitter == 0)
		pEmitter = m_pActiveEmitter->data;

	if(!pEmitter)
		return 0;

	float sc = 0;
/*
	if(m_pActiveEmitter->Class()==EMC_LIGHT){
		sc = m_pActiveEmitter->data_light->emitter_life_time;
		return m_pActiveEmitter->GetLight()->emitter_size[nPoint].time*sc;
	}else{
*/
		sc = pEmitter->emitter_life_time;
		return pEmitter->num_particle[nPoint].time*sc;
//	}
}

void  CEffectToolDoc::set_gen_point_time(int nPoint, float tm, CEmitterData* pEmitter)
{
	if (!pEmitter)  pEmitter =  m_pActiveEmitter;
	float sc = pEmitter->EmitterLifeTime();

	sc = tm/sc;
	if(sc > 1.0f)
		sc = 1.0f;

	pEmitter->SetGenerationPointTime(nPoint, sc);
}

float CEffectToolDoc::get_particle_key_time(int nGenPoint, int nParticleKey)
{
	float f = 0;
//	if(nParticleKey>(m_pActiveEmitter->data->p_size.size()-1))
//		nParticleKey = (m_pActiveEmitter->data->p_size.size()-1);

	if(m_pActiveEmitter->Class()==EMC_LIGHT){
		f = m_pActiveEmitter->GetLight()->emitter_life_time*m_pActiveEmitter->GetLight()->emitter_size[nParticleKey].time;
	}else{
		f = m_pActiveEmitter->data->num_particle[nGenPoint].time*m_pActiveEmitter->data->emitter_life_time + 
			m_pActiveEmitter->GenerationLifeTime(nGenPoint)*m_pActiveEmitter->data->p_size[nParticleKey].time;
	}

	return f;
}
void  CEffectToolDoc::set_particle_key_time(int nGenPoint, int nParticleKey, float tm)
{
	if(m_pActiveEmitter->Class()==EMC_LIGHT)
		tm /= m_pActiveEmitter->GetLight()->emitter_life_time;
	else{
		tm -= m_pActiveEmitter->data->num_particle[nGenPoint].time*m_pActiveEmitter->data->emitter_life_time;
		tm /= m_pActiveEmitter->GenerationLifeTime(nGenPoint);
	}

	if(tm > 1.0f)
		tm = 1.0f;
	if(tm < 0)
		tm = 0;

/*	if(nParticleKey > 0) //время должно быть неубывающим
	{
		if(m_pActiveEmitter->velocity[nParticleKey - 1].x > tm)
			tm = m_pActiveEmitter->velocity[nParticleKey - 1].x;
	}
*/
	m_pActiveEmitter->SetParticleKeyTime(nParticleKey, tm);
}

void CEffectToolDoc::change_particle_life_time(int nGenPoint, float tm)
{
	if(m_pActiveEmitter->Class()==EMC_LIGHT){
		if(tm > 100.0f)	tm = 100.0f;
		if(tm < 0.001f)	tm = 0.001f;
		m_pActiveEmitter->GetLight()->emitter_life_time = tm;
	}else{
		tm -= m_pActiveEmitter->data->num_particle[nGenPoint].time*m_pActiveEmitter->data->emitter_life_time;

		if(tm > 100.0f)	tm = 100.0f;
		if(tm < 0.001f)	tm = 0.001f;

		m_pActiveEmitter->ChangeGenerationLifetime(nGenPoint, tm);
	}

}


/////////////////////////////////////////////////////////////////////////////
// CEffectToolDoc commands

BOOL CEffectToolDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	/*
	EffectLibrary eflib;
	if(eflib.Load(lpszPathName))
	{
		EffectLibrary::iterator i;
		FOR_EACH(eflib, i)
			m_effects.push_back(new CEffectData(*i));
	}
	*/
	Init();
	bool res = CDocument::OnOpenDocument(lpszPathName);
	LoadProject(lpszPathName);
	return res;
}

CString CEffectToolDoc::CopySprite(LPCTSTR name, LPCTSTR export_path)
{
	CString from(name);
	CString filename(from.Right(from.GetLength() - from.ReverseFind('\\') -1));
	CString to;
	if(export_path==NULL){
		to = m_StorePath;
		to += "\\";
		to += FOLDER_SPRITE;
	}else{
		to = export_path;
	}
	//
	if(_chdir(to)==-1){
		_mkdir(to);
	}
	//
//	to += "\\";
	to += filename;

//	if(from==to)return to;

	CopyFile(from,to, false);

	SetWorkingDir();
	
	return filename;
}

void CEffectToolDoc::OnToolbarSprite() 
{
	ASSERT(m_pActiveEmitter);

	CEmitterData*  pActEmitter = m_pActiveEmitter;
	CDlgLoadSprite dlg;
	if(pActEmitter->data)
	{
		dlg.SetStrTexture(pActEmitter->data->texture_name.c_str());
	}
	else
		dlg.SetStrTexture(pActEmitter->data_light->texture_name.c_str());

	char cb[MAX_PATH];
	_getcwd(cb, 100);
	std::string s = theApp.GetProfileString("Paths","SpritePath",cb);
	dlg.m_ofn.lpstrInitialDir = (LPSTR)s.c_str();
	if(dlg.DoModal() == IDOK)
	{
		// Copy Sprite to Project/Image dir
		CString name = CopySprite(dlg.GetStrTexture());
		if (name!="")
			name = FOLDER_SPRITE+name;
		if(pActEmitter->data)
			pActEmitter->data->texture_name = name;
		else
			pActEmitter->data_light->texture_name = name;

		pActEmitter->bDirty = true;
		theApp.scene.InitEmitters();
		theApp.WriteProfileString("Paths","SpritePath",dlg.GetStrTexture());
	}
}
void CEffectToolDoc::OnUpdateToolbarSprite(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pActiveEmitter != 0 && !m_StorePath.IsEmpty());
}

void CEffectToolDoc::OnUpdateGeneral(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pActiveEmitter != 0 && m_pActiveEmitter->data != 0);
}

void CEffectToolDoc::OnUpdateLabel(CCmdUI* pCmdUI)
{
	OnUpdateGeneral(pCmdUI);

/*	if(m_pActiveEmitter)
	{
		CString s;
		switch(pCmdUI->m_nID)
		{
		case IDC_EDIT_X:
			s.Format("%.2f", m_pActiveEmitter->particle_position.size.x);
			break;
		}

		pCmdUI->m_pOther->SetWindowText(s);
	}
*/
}

void CEffectToolDoc::OnUpdateTime(CCmdUI* pCmdUI)
{
	if(theApp.scene.GetEffect())
	{
		CString s;
		s.Format("%.2f", theApp.scene.GetEffect()->GetSummaryTime());
		pCmdUI->m_pOther->SetWindowText(s);
	}
}

void CEffectToolDoc::OnTbPause() 
{
	theApp.scene.bPause = !theApp.scene.bPause;

	RepaintGraph();
	//UpdateAllViews(0);
}

void CEffectToolDoc::OnTbFirst() 
{
	theApp.scene.InitEmitters();
}
void CEffectToolDoc::OnTbLast() 
{
	if(m_pActiveEmitter)
		theApp.scene.GetEffect()->MoveToTime(m_pActiveEmitter->ParticleLifeTime() - 0.001f);
	else
		theApp.scene.GetEffect()->MoveToTime(theApp.scene.GetEffect()->GetSummaryTime() - 0.001f);
}
void CEffectToolDoc::OnTbNext() 
{
	theApp.scene.GetEffect()->MoveToTime(theApp.scene.GetEffect()->GetTime() + 0.01f);
}
void CEffectToolDoc::OnUpdateTbFirst(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.scene.bPause);
}
void CEffectToolDoc::OnUpdateTbLast(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.scene.bPause);
}
void CEffectToolDoc::OnUpdateTbNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.scene.bPause);
}

static LPCTSTR lpszFilter = "Effect files|*.effect|All files|*.*||";
void CEffectToolDoc::OnToolbarSave() 
{
	if(m_pActiveEmitter)
		SaveCurrentEmitter();
	else if(m_pActiveEffect)
		SaveCurrentEffect();
}


struct BrowseCBDataType
{
	LPCTSTR lpszPath;

	BrowseCBDataType(LPCTSTR lpsz){
		lpszPath = lpsz; 
	}
};

int __stdcall BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
{
	BrowseCBDataType* pCBData = (BrowseCBDataType*)pData;

	switch(uMsg) 
	{
	case BFFM_INITIALIZED:
		if(_tcslen((LPCTSTR)pData)) 
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pCBData->lpszPath);
		break;
	}

	return 0;
}
bool BrowseFolder(LPCTSTR szTitle, CString& s, CString& path, int mode)
{
	bool           bRet = false;
	BROWSEINFO     bri;
	IMalloc*       pMalloc;
	LPITEMIDLIST   idlist;
	TCHAR          cbBuf[MAX_PATH];

	CString sDef;
	if(mode)
		sDef = theApp.GetProfileString("Browse", "LastFXPath", path);
	else
		sDef = theApp.GetProfileString("Browse", "LastSpritePath", path);

	if(::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BrowseCBDataType cb_data(sDef);

		memset(&bri, 0, sizeof(bri));
		bri.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
		bri.lpszTitle = szTitle;
		bri.ulFlags = BIF_RETURNONLYFSDIRS|BIF_BROWSEINCLUDEFILES;
		bri.lpfn = BrowseCallbackProc;
		bri.lParam = (LPARAM)&cb_data;

		if(idlist = ::SHBrowseForFolder(&bri))
		{
			if(::SHGetPathFromIDList(idlist, cbBuf))
			{
				s = cbBuf;
				bRet = true;

				if(mode)
					theApp.WriteProfileString("Browse", "LastFXPath", cbBuf);
				else
					theApp.WriteProfileString("Browse", "LastSpritePath", cbBuf);
			}
			pMalloc->Free(idlist);
		}

		pMalloc->Release();
	}

	return bRet;
}
void CEffectToolDoc::OnToolbarOpen() 
{
	CString fx, sprite;
	if(BrowseFolder("Выберите эффект или эмиттер", fx, m_StorePath, 1)){
		if(GetFileAttributes(fx) != FILE_ATTRIBUTE_DIRECTORY&&(fx.Find(".effect")>=0))
			LoadEffectFromFile(fx);
		else
			if(BrowseFolder("Укажите библиотеку спрайтов", sprite, m_StorePath, 0))
			{
				if(GetFileAttributes(fx) == FILE_ATTRIBUTE_DIRECTORY) 
					LoadEffect(fx, sprite);
				else
					LoadEmitter(m_pActiveEffect, fx, sprite);
			}
	}
/*	char cb[MAX_PATH];
	_getcwd(cb, 100);

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY, lpszFilter);
	if(dlg.DoModal() == IDOK)
	{
		m_effects.clear();

		OnOpenDocument(dlg.GetPathName());
		UpdateAllViews(0, HINT_UPDATE_TREE);
	}

	_chdir(cb);
*/
}

///// Load World /////

bool BrowseWorldFolder(CString& s)
{
	bool           bRet = false;
	BROWSEINFO     bri;
	IMalloc*       pMalloc;
	LPITEMIDLIST   idlist;
	TCHAR          cbBuf[MAX_PATH];

	CFileFind ff;
	CString sDef;

	sDef = theApp.GetProfileString("Worlds", "LastWorldsPath", "");

	if(ff.FindFile(sDef)){
		s = sDef;
		return true;
	}else{
		sDef = "";
	}

	if(::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BrowseCBDataType cb_data(sDef);

		memset(&bri, 0, sizeof(bri));
		bri.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
		bri.lpszTitle = "Укажите файл 'worlds.prm'";
		bri.ulFlags = BIF_RETURNONLYFSDIRS|BIF_BROWSEINCLUDEFILES;
		bri.lpfn = BrowseCallbackProc;
		bri.lParam = (LPARAM)&cb_data;

		if(idlist = ::SHBrowseForFolder(&bri))
		{
			if(::SHGetPathFromIDList(idlist, cbBuf))
			{
				s = cbBuf;
				bRet = true;

				theApp.WriteProfileString("Worlds", "LastWorldsPath", cbBuf);
			}
			pMalloc->Free(idlist);
		}

		pMalloc->Release();
	}

	return bRet;
}

void CEffectToolDoc::LoadWorld() 
{
	if(BrowseWorldFolder(m_WorldPath)){
		if(GetFileAttributes(m_WorldPath) != FILE_ATTRIBUTE_DIRECTORY&&m_WorldPath.Find("worlds.prm"))
			theApp.scene.LoadWorld(m_WorldPath);
		else
			m_WorldPath = "";
	}
}

//////////////////////

void CEffectToolDoc::OnUpdateToolbarSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pActiveGroup&&m_group.GetSize()
		&&!m_pActiveGroup->m_effects.empty());
}


void CEffectToolDoc::SaveCurrentEffect()
{
	ASSERT(m_pActiveEffect);
	ASSERT(m_pActiveGroup);

	CString s;
	s.Format("Сохранить эффект %s?", m_pActiveEffect->name.c_str());
	if(AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION) == IDYES)
		SaveEffect(m_pActiveGroup->m_name, m_pActiveEffect);
}
void CEffectToolDoc::SaveCurrentEmitter()
{
	ASSERT(m_pActiveEmitter);

	CString s;
	if(m_pActiveEmitter->data)
		s.Format("Сохранить эмиттер %s?", m_pActiveEmitter->data->name.c_str());
	else
		s.Format("Сохранить эмиттер %s?", m_pActiveEmitter->data_light->name.c_str());

	if(AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION) == IDYES)
		SaveEmitter(m_pActiveEmitter, theApp.m_szDirEmitters);
}

void RMDIR(LPCTSTR lpszDir)
{
	CString sFolder(lpszDir);
	sFolder += "*.*";

	WIN32_FIND_DATA ff;
	HANDLE fh = ::FindFirstFile(sFolder, &ff);
	if(fh != INVALID_HANDLE_VALUE)
	{
		do
		{
			if((ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				CString s(lpszDir);
				s += ff.cFileName;

				DeleteFile(s);
			}
		}
		while(::FindNextFile(fh, &ff));
		::FindClose(fh);
	}

	_rmdir(lpszDir);
}
void CEffectToolDoc::SaveEffect(LPCTSTR strGroup, CEffectData* pEffect)
{
//	CString sFolder = theApp.m_szDir + pEffect->name.c_str();
	CString sFolder(m_StorePath);
	sFolder += "\\";
	sFolder += strGroup;
	sFolder += "\\";
	sFolder += pEffect->name.c_str();
	sFolder += "\\";

	RMDIR(sFolder);
	_mkdir(sFolder);

	EmitterListType::iterator i;
	FOR_EACH(pEffect->emitters, i)
		SaveEmitter(*i, sFolder);

	pEffect->bDirty = false;
}
void CEffectToolDoc::SaveEmitter(CEmitterData* pEmitter, LPCTSTR szPath)
{
	CString s(szPath);
	if(pEmitter->data)
		s += pEmitter->data->name.c_str();
	else
		s += pEmitter->data_light->name.c_str();

	CSaver sv(s);
	pEmitter->Save(sv);
	pEmitter->bDirty = false;
}
bool CEffectToolDoc::LoadEffectFromFile(LPCTSTR szFolder)
{
	if(GetFileAttributes(szFolder) == 0xFFFFFFFF)
		return false;
	ASSERT(m_pActiveGroup);
	CEffectData* pEffect = NULL;
	CString strFXPath(szFolder);
	strFXPath = strFXPath.Left(strFXPath.ReverseFind('\\'));

	EffectLibrary eflib;
	if(eflib.Load(szFolder))
	{
		EffectLibrary::iterator i;
		FOR_EACH(eflib, i){
			m_pActiveGroup->m_effects.push_back(pEffect = new CEffectData(*i));
			//// Copy Sprites to TEXTURES
			EmitterListType::iterator i_e;
			FOR_EACH(pEffect->emitters, i_e)
			{
				CString sprite_name;
				if((*i_e)->data)
					sprite_name = ((*i_e)->data->texture_name.c_str());
				else
					sprite_name = ((*i_e)->data_light->texture_name.c_str());

				if(sprite_name.Find('\\')){
					sprite_name = sprite_name.Right(sprite_name.GetLength() - sprite_name.ReverseFind('\\') -1);
				}
				CString from(strFXPath);
				from += "\\";
				from += FOLDER_TEXTURES;
				from += "\\";
				from += sprite_name;

				CopySprite(from);
			}
			////
		}
	}

/*
	m_pActiveGroup->m_effects.push_back(m_pActiveEffect = new CEffectData);
	
	CLoadDirectoryFile rd;
	if(rd.Load(szFolder))
	{
		CLoadData* ID = rd.next();
		m_pActiveEffect->Load(ID);
	}
*/
	UpdateAllViews(0, HINT_UPDATE_TREE);

	return true;
}
bool CEffectToolDoc::LoadEffect(LPCTSTR szFolder, LPCTSTR szFolderSprite)
{
	if(GetFileAttributes(szFolder) == 0xFFFFFFFF)
		return false;
	ASSERT(m_pActiveGroup);

	m_pActiveGroup->m_effects.push_back(m_pActiveEffect = new CEffectData);
	
	CEffectData*	pEffect = m_pActiveEffect;

	CString sFolder(szFolder);

	int n = sFolder.ReverseFind('\\');
	pEffect->name = sFolder.Right(sFolder.GetLength() - n - 1);

	sFolder += "\\*.";

	WIN32_FIND_DATA ff;
	HANDLE fh = ::FindFirstFile(sFolder, &ff);
	if(fh != INVALID_HANDLE_VALUE)
	{
		do
		{
			if((ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				CString s(szFolder);
				s += "\\";
				s += ff.cFileName;

				LoadEmitter(pEffect, s, szFolderSprite);

			}
		}
		while(::FindNextFile(fh, &ff));
		::FindClose(fh);
	}

	UpdateAllViews(0, HINT_UPDATE_TREE);

	return true;
}
void CEffectToolDoc::LoadEmitter(CEffectData* pEffect, LPCTSTR szPath, LPCTSTR szFolderSprite, int mode)
{
	ASSERT(pEffect);

	CLoadDirectoryFile rd;

	if(rd.Load(szPath))
	{
		CEmitterData* p = 0;

		CLoadData* ID = rd.next();
		switch(ID->id)
		{
		case IDS_BUILDKEY_Z:
			p = pEffect->add_emitter(EMC_INTEGRAL_Z);
			break;
		case IDS_BUILDKEY_LIGHT:
			p = pEffect->add_emitter(EMC_LIGHT);
			break;
		case IDS_BUILDKEY_SPL:
			p = pEffect->add_emitter(EMC_SPLINE);
			break;
		case IDS_BUILDKEY_INT:
			p = pEffect->add_emitter(EMC_INTEGRAL);
			break;
		default:
			return;
			break;
		}

		if(p)
			p->Load(ID);
		
		// Copy Sprite
		CString sprite_path(szFolderSprite);
		if(!sprite_path.IsEmpty()){
			if(GetFileAttributes(sprite_path) != FILE_ATTRIBUTE_DIRECTORY){
				sprite_path = sprite_path.Left(sprite_path.ReverseFind('\\'));
			}
			CString sprite_name;
			if(p->data)
				sprite_name = (p->data->texture_name.c_str());
			else
				sprite_name = (p->data_light->texture_name.c_str());

			if(sprite_name.Find('\\')){
				sprite_name = sprite_name.Right(sprite_name.GetLength() - sprite_name.ReverseFind('\\') -1);
			}

			CString from(sprite_path);
			from += "\\";
			from += sprite_name;

			CString to(FOLDER_SPRITE + sprite_name);

			CopySprite(from);

			if(p->data)
				p->data->texture_name = to;
			else
				p->data_light->texture_name = to;
		}
		//

		if(p){
			int n = 1;
			char cb[255];
			
			if(p->data)
				strcpy(cb, p->data->name.c_str());
			else
				strcpy(cb, p->data_light->name.c_str());

			while(!pEffect->CheckName(cb, p))
			{
				if(p->data)
					sprintf(cb, "%s%d", p->data->name.c_str(), n++);
				else
					sprintf(cb, "%s%d", p->data_light->name.c_str(), n++);
			}
			if(p->data)
				p->data->name = cb;
			else
				p->data_light->name = cb;
		}
	}

	if(mode)
		UpdateAllViews(0, HINT_UPDATE_TREE);
}

void CEffectToolDoc::OnToolbarExport() 
{
	ASSERT(m_pActiveGroup);
	char cb[MAX_PATH];
	_getcwd(cb, 100);

	CFileDialog dlg(FALSE, "effect", m_pActiveGroup->m_name, OFN_HIDEREADONLY, lpszFilter);
	std::string s = theApp.GetProfileString("Paths","ExportPath",cb);
	dlg.m_ofn.lpstrInitialDir = (LPSTR)s.c_str();
	if(dlg.DoModal() == IDOK)
	{
		CSaver saver;
		saver.Init(dlg.GetPathName());
		saver.SetData(EXPORT_TO_GAME);
		// sprite path
		CString export_path(dlg.GetPathName());
		export_path = export_path.Left(export_path.ReverseFind('\\'));
		///
		EffectStorageType::iterator i;
		FOR_EACH(m_pActiveGroup->m_effects, i)
		{
			(*i)->prepare_effect_data(NULL);
			(*i)->Save(saver);
			//// Copy Sprites to TEXTURES
			EmitterListType::iterator i_e;
			FOR_EACH((*i)->emitters, i_e)
			{
				CString from(m_StorePath);
				from += "\\";

				if((*i_e)->data)
					from += (*i_e)->data->texture_name.c_str();
				else
					from += (*i_e)->data_light->texture_name.c_str();

				CString sprite_name;
				
				if((*i_e)->data)
					((*i_e)->data->texture_name.c_str());
				else
					((*i_e)->data_light->texture_name.c_str());

				if(sprite_name.Find('\\')){
					sprite_name = sprite_name.Right(sprite_name.GetLength() - sprite_name.ReverseFind('\\') -1);
				}

				CString to(export_path);
				to += "\\";
				to += FOLDER_TEXTURES;

				CopySprite(from, to);
			}
			////
		}
		theApp.WriteProfileString("Paths","ExportPath",dlg.GetPathName());
	}

	_chdir(cb);

	SetWorkingDir();
}
void CEffectToolDoc::OnUpdateToolbarExport(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pActiveGroup&&m_group.GetSize()
		&&!m_pActiveGroup->m_effects.empty());
}

BOOL CEffectToolDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	if(EmittersCheckDirty())
	{
		SetModifiedFlag();
		/*
		switch(AfxMessageBox(IDS_CLOSEFRAMEDIRTY, MB_YESNOCANCEL|MB_ICONQUESTION))
		{
		case IDYES:
//			SaveRegistry();
//			SaveAll();
			break;

		case IDNO:
			return TRUE;

		default:
			return FALSE;
		}
		*/
	}
//	else
//		SaveRegistry();
	
	return CDocument::CanCloseFrame(pFrame);
}

void CEffectToolDoc::OnToolbarSaveall() 
{
	SaveAll();	
}

///////////////////////////
//// Project Serialize ////
///////////////////////////
// Emitter
void CEmitterDeskr::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{	// storing code
		ar << m_name;
		// dummy values
		ar << d1 << d2 << d3;
		ar << i1 << i2 << i3;
	}
	else
	{	// loading code
		ar >> m_name;
		// dummy values
		ar >> d1 >> d2 >> d3;
		ar >> i1 >> i2 >> i3;
	}
}
// Effect
void CEffectDeskr::AddEmitter(LPCTSTR name)
{
	m_emitter.SetSize(m_emitter.GetSize()+1);
	m_emitter[m_emitter.GetSize()-1].m_name = name;
}
void CEffectDeskr::Serialize(CArchive& ar)
{
	int count = 0;
	if (ar.IsStoring())
	{	// storing code
		ar << m_name;
		// dummy values
		ar << m_Path3DModel << d2 << d3;
		ar << m_bExpand << i2 << i3;		
		// array
		count = m_emitter.GetSize();
		ar << count;
		for(int i=0; i<count; i++)
			m_emitter[i].Serialize(ar);
	}
	else
	{	// loading code
		ar >> m_name;
		// dummy values
		ar >> m_Path3DModel >> d2 >> d3;
		ar >> m_bExpand >> i2 >> i3;
		// array
		ar >> count;
		m_emitter.SetSize(count);
		for(int i=0; i<count; i++)
			m_emitter[i].Serialize(ar);
	}
}
// Project
void CGroupDescr::AddEmitter(LPCTSTR name)
{
	m_effect[m_effect.GetSize()-1].AddEmitter(name);
}
void CGroupDescr::AddEffect(LPCTSTR name, LPCTSTR model_name, bool bExpand)
{
	m_effect.SetSize(m_effect.GetSize()+1);
	m_effect[m_effect.GetSize()-1].m_name = name;
	m_effect[m_effect.GetSize()-1].m_Path3DModel = model_name;
	m_effect[m_effect.GetSize()-1].m_bExpand = bExpand;
}
void CGroupDescr::Serialize(CArchive& ar)
{
	int count = 0;
	if (ar.IsStoring())
	{	// storing code
		ar << m_name;
		// dummy values
		ar << m_Path3DBack << d2 << d3;
		ar << m_bExpand << i2 << i3;
		// array
		count = m_effect.GetSize();
		ar << count;
		for(int i=0; i<count; i++)
			m_effect[i].Serialize(ar);
	}
	else
	{	// loading code
		ar >> m_name;
		// dummy values
		ar >> m_Path3DBack >> d2 >> d3;
		ar >> m_bExpand >> i2 >> i3;
		// array
		ar >> count;
		m_effect.SetSize(count);
		for(int i=0; i<count; i++)
			m_effect[i].Serialize(ar);
	}
}
void CEffectToolDoc::Serialize(CArchive& ar)
{
	int count = 0;
	if (ar.IsStoring())
	{	// storing code
		// array
		count = m_group.GetSize();
		ar << count;
		for(int i=0; i<count; i++)
			((CGroupDescr*)m_group[i])->Serialize(ar);
	}
	else
	{	// loading code
		// array
		ar >> count;
		for(int i=0; i<count; i++)
		{
			m_group.Add(new CGroupDescr);
			((CGroupDescr*)m_group[i])->Serialize(ar);
		}
		//
	}
}

BOOL CEffectToolDoc::OnNewDocument()
{
	InitEmptyDocument();
	return CDocument::OnNewDocument();
}

bool CEffectToolDoc::LoadProject(CString strProject)
{
	CString cur_path(strProject.Left(strProject.ReverseFind('.')));
	m_StorePath = cur_path;
	SetWorkingDir();

	for(int i=0; i<m_group.GetSize(); i++)
	{
		CString group_path;
		group_path = cur_path + "\\";
		group_path += ((CGroupDescr*)m_group[i])->m_name;

		for(int fx=0; fx<((CGroupDescr*)m_group[i])->m_effect.GetSize(); fx++)
		{
			((CGroupDescr*)m_group[i])->m_effects.push_back(m_pActiveEffect = new CEffectData);
			m_pActiveEffect->name =			((CGroupDescr*)m_group[i])->m_effect[fx].m_name;
			m_pActiveEffect->Path3Dmodel =	((CGroupDescr*)m_group[i])->m_effect[fx].m_Path3DModel;
			m_pActiveEffect->bExpand =	((CGroupDescr*)m_group[i])->m_effect[fx].m_bExpand;
//			if(!m_pActiveEffect->Path3Dmodel.IsEmpty())
//				Load3DModel(1);

			CString fx_path(group_path);
			fx_path += "\\";
			fx_path += m_pActiveEffect->name.c_str();

			for(int mt=0; mt<((CGroupDescr*)m_group[i])->m_effect[fx].m_emitter.GetSize(); mt++)
			{
				CString mt_path(fx_path);
				mt_path += "\\";
				mt_path += ((CGroupDescr*)m_group[i])->m_effect[fx].m_emitter[mt].m_name;			
				LoadEmitter(m_pActiveEffect, mt_path, "", false);
			}
		}
	}
	return true;
}

bool CEffectToolDoc::CheckGroupName(LPCTSTR lpsz)
{
	for(int i=0; i<m_group.GetSize(); i++)
	{
		if(((CGroupDescr*)m_group[i])->m_name==lpsz)
			return false;
	}
	return true;
}

CGroupDescr* CEffectToolDoc::AddGroup(void)
{
	CString name("Group1");
	int i = 1;
	while(!CheckGroupName(name))
	{
		i++;
		CString proto;
		name.Format("Group%d", i);
	}

	CGroupDescr* pg;
	m_group.Add(pg = new CGroupDescr(name));
/*
	CEffectData* pNewEffect;
	pg->m_effects.push_back(pNewEffect = new CEffectData);
	pNewEffect->add_emitter();
*/
	return pg;
}

void CEffectToolDoc::DeleteCurrentGroup(void)
{
	if(m_group.GetSize()<2)
		return;
	for(int i=0; i<m_group.GetSize(); i++)
	{
		if(m_pActiveGroup==m_group[i])
		{
			Release3DModel();
			delete m_group[i];
			m_group.RemoveAt(i);
			m_pActiveGroup = (CGroupDescr*)m_group[0];
			return;
		}
	}
}

void CEffectToolDoc::SaveProject(void)
{
	for(int i=0; i<m_group.GetSize(); i++)
	{
		// make dir
		CString group_path(m_StorePath);
		group_path += "\\";
		group_path += ((CGroupDescr*)m_group[i])->m_name;
		_mkdir((LPCTSTR)group_path);
		//
		((CGroupDescr*)m_group[i])->Init();
		EffectStorageType::iterator it;
		FOR_EACH(((CGroupDescr*)m_group[i])->m_effects, it)
		{
			// make dir
			CString effect_path(group_path);
			effect_path += "\\";
			effect_path += (*it)->name.c_str();
			_mkdir((LPCTSTR)effect_path);
			//
			((CGroupDescr*)m_group[i])->AddEffect((*it)->name.c_str(), (*it)->Path3Dmodel, (*it)->bExpand);
			
			EmitterListType::iterator i_e;
			FOR_EACH((*it)->emitters, i_e)
			{
/*
				if((*i_e)->data){
					if((*i_e)->data->texture_name == DEFAULT_TXT_FIRE){
						CString sprite(FOLDER_SPRITE);
						sprite += DEFAULT_TXT_FIRE;
						(*i_e)->data->texture_name = sprite;
					}
				}else{
					if((*i_e)->data_light->texture_name == DEFAULT_TXT_LIGHT){
						CString sprite(FOLDER_SPRITE);
						sprite += DEFAULT_TXT_LIGHT;
						(*i_e)->data_light->texture_name = sprite;
					}
				}
*/
				if((*i_e)->data)
					((CGroupDescr*)m_group[i])->AddEmitter((*i_e)->data->name.c_str());
				else
					((CGroupDescr*)m_group[i])->AddEmitter((*i_e)->data_light->name.c_str());
			}
		}
	}
}

BOOL CEffectToolDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CString path(lpszPathName);
	m_StorePath = path.Left(path.ReverseFind('.'));

	SetWorkingDir();
//	CopySprite(theApp.m_szDir + DEFAULT_TXT_FIRE);
//	CopySprite(theApp.m_szDir + DEFAULT_TXT_LIGHT);

	SaveProject();

	SaveAll();

	SetWorkingDir();

	return CDocument::OnSaveDocument(lpszPathName);
}

void CEffectToolDoc::OnUpdateToolbarOpen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_StorePath.IsEmpty());
}

void CEffectToolDoc::OnUpdateSet3dmodel(CCmdUI *pCmdUI)
{
	if (m_pActiveEmitter)
		pCmdUI->Enable(m_pActiveEmitter->data && 
						m_pActiveEmitter->data->particle_position.type == EMP_3DMODEL_INSIDE);
	else 
		pCmdUI->Enable(m_pActiveEffect!=NULL);
}

void CEffectToolDoc::OnSet3dmodel()
{
	Load3DModel(MODE_LOAD, TYPE_3DMODEL);
}

void CEffectToolDoc::OnUpdateShow3dmodel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveEffect!=NULL&&!m_pActiveEffect->Path3Dmodel.IsEmpty());
	pCmdUI->SetCheck(m_pActiveEffect!=NULL&&m_pActiveEffect->Show3Dmodel);
}

void CEffectToolDoc::OnShow3dmodel()
{
	if(m_pActiveEffect){
		if(m_pActiveEffect->Show3Dmodel)
			m_pActiveEffect->Show3Dmodel = false;
		else
			m_pActiveEffect->Show3Dmodel = true;
	}
	UpdateAllViews(0);
}

void CEffectToolDoc::OnUpdateSet3dbackground(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveGroup!=NULL);
}

void CEffectToolDoc::OnSet3dbackground()
{
	Load3DModel(MODE_LOAD, TYPE_3DBACK);
}

void CEffectToolDoc::OnShow3dbackground()
{
	if(m_pActiveGroup){
		if(m_pActiveGroup->m_bShow3DBack)
			m_pActiveGroup->m_bShow3DBack = false;
		else
			m_pActiveGroup->m_bShow3DBack = true;
	}
	UpdateAllViews(0);
}

void CEffectToolDoc::OnUpdateShow3dbackground(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pActiveGroup!=NULL&&!m_pActiveGroup->m_Path3DBack.IsEmpty());
	pCmdUI->SetCheck(m_pActiveGroup!=NULL&&m_pActiveGroup->m_bShow3DBack);
}

void CEffectToolDoc::SetWorkingDir(void)
{
	if(!m_StorePath.IsEmpty()&&_chdir(m_StorePath)==-1){
		_mkdir(m_StorePath);
	}
	if(!m_StorePath.IsEmpty())
		_chdir(m_StorePath);
}

int CEffectToolDoc::Release3DModel(void)
{
	RELEASE(theApp.scene.m_pModel);
	RELEASE(theApp.scene.m_pBackModel);
	return 0;
}

#include "..\Terra\terra.h"

void CEffectToolDoc::MoveEffectCenter(float dx, float dy)
{
	float x, y;
	x = m_EffectCenter.x += dx;
	y = m_EffectCenter.y += dy;

	if(m_EffectCenter.x<0) m_EffectCenter.x = 0;
	if(m_EffectCenter.y<0) m_EffectCenter.y = 0;
	if(m_EffectCenter.x>=vMap.H_SIZE) m_EffectCenter.x = vMap.H_SIZE -1;
	if(m_EffectCenter.y>=vMap.V_SIZE) m_EffectCenter.y = vMap.V_SIZE -1;

//	m_EffectCenter.z = vMap.GetApproxAlt(x, y) + 50;
}

