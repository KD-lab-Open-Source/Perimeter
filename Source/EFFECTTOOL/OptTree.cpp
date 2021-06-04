// OptTree.cpp : implementation file
//
#include "stdafx.h"
#include "EffectTool.h"
#include "OptTree.h"
#include ".\opttree.h"
#include "MainFrm.h"
#include <ios>


// COptTree
const float fl_max = float(1e35);
extern CControlView* ctrv;
extern CMainFrame* mf;
IMPLEMENT_DYNCREATE(COptTree, CFormView)

CString ToStr(float p)
{
	CString s;
	s.Format("%f",p);
	return s;
}
CString ToStr(int p)
{
	CString s;
	s.Format("%d",p);
	return s;
}

inline bool SpiralDef()
{
#ifdef EFFECTTOOL
	return true;
#else 
	return false;
#endif
}

enum E_LOCATION
{
	MANUALLY	= 0,
	SPIRAL		= 1,
};
Vect3f GetKeyPosCenter(CKeyPos& kp)
{
	int size = kp.size();
	if (!size) return Vect3f::ZERO;
	Vect3f cnt(0,0,0);
	CKeyPos::iterator it;
	FOR_EACH(kp,it)
		cnt+= it->pos;
	return cnt/=size;
}
float COptTree::SetParamSpiral(CKeyPos& kp)
{
	int size = kp.size();
	if (size<2) return 0 ;
	Vect3f cnt = GetKeyPosCenter(kp);
	float ang = acos(Vect3f(kp[0].pos.x,kp[0].pos.y,0).Normalize().dot(Vect3f(kp[1].pos.x,kp[1].pos.y,0).Normalize()));
	Write(IX_SpAngle,R2G(ang));
	float alpha = acos(Vect3f(1,0,0).dot(Vect3f(kp[0].pos.x,kp[0].pos.y,0).Normalize()));
	Write(IX_SpHeight1,kp[0].pos.z);
	Write(IX_SpHeight2,kp[size-1].pos.z);
	float r = sqrt(pow(kp[0].pos.x-cnt.x,2)+pow(kp[0].pos.y-cnt.y,2));
	Write(IX_SpRadius1,r);
	r = sqrt(pow(kp[size-1].pos.x-cnt.x,2)+pow(kp[size-1].pos.y-cnt.y,2));
	Write(IX_SpRadius2,r);
	Write(IX_SpCompress,1);
	return alpha;
}
void COptTree::Spiral(bool set_par /*= false*/)
{
#ifdef EFFECTTOOL
	if (intV(IX_PointLoc)!=1 || !GetDocument()->m_pActiveEmitter)
		return;
	CKeyPos* kt = NULL;
	SpiralData::Dat* dat = NULL;
	if (theApp.scene.m_ToolMode == CD3DScene::TOOL_SPLINE) 
	{
		kt = &((EmitterKeySpl*)GetDocument()->m_pActiveEmitter->data)->p_position;
		dat = &GetDocument()->m_pActiveEmitter->data->sdat.GetData(1);
	}
	else if (theApp.scene.m_ToolMode == CD3DScene::TOOL_POSITION)
	{
		kt = &GetDocument()->m_pActiveEmitter->emitter_pos();
		if (GetDocument()->m_pActiveEmitter->Class()==EMC_LIGHT)
			dat = &GetDocument()->m_pActiveEmitter->data_light->sdat.GetData(0);
		else dat = &GetDocument()->m_pActiveEmitter->data->sdat.GetData(0);
	}
	else return;
	ASSERT(dat);
	CKeyPos& kp = *kt;
	int size = kp.size();
	if (size==0) return;

	float alpha = 0;
	float r = dat->r1;//floatV(IX_SpRadius1);
	float h = dat->h1;//floatV(IX_SpHeight1);
	float dalpha = dat->dalpha;//G2R(floatV(IX_SpAngle));
	float dr = (dat->r2 - dat->r1)/size;//(floatV(IX_SpRadius2)-floatV(IX_SpRadius1))/size;
//	float dh = (floatV(IX_SpHeight2)-floatV(IX_SpHeight1))/size;
	float k = 1/dat->compress;//1/floatV(IX_SpCompress);
	float dh_max = dat->h2 - dat->h1;//floatV(IX_SpHeight2)-floatV(IX_SpHeight1);
	float k_norm = pow(k,size-1)-1;
	for(int i=0;i<size;++i)
	{
		kp[i].pos.x = r*cos(alpha);
		kp[i].pos.y = -r*sin(alpha);
		float tt;
		if (k!=1) tt = (pow(k,i)-1)/k_norm;
		else tt = float(i)/(size-1);
		kp[i].pos.z = h + tt*dh_max;
		alpha+=dalpha;
		r+=dr;
	}
	for(int i=0;i<size;++i)
	{
//		kp[i].pos+=first;
		kp[i].pos=dat->mat*kp[i].pos;
	}
#endif
}

void CDataItem::SetComboStr(CString combo_text)
{
	combo_stings.clear();
	int end;
	int begin=0;
	while((end = combo_text.Find(',' , begin))!=-1)
	{
		combo_stings.push_back();
		combo_stings.back().Visible = true;
		combo_stings.back().str = combo_text.Mid(begin,end-begin);
		begin = end+1;
	}
	if (round(value)>combo_stings.size())
		value = 0;
}

void CDataItem::Init(DWORD inx, DWORD style_, float min, float max,bool isint, CString combo_text)
{
	style = style_;
	index = inx;
	is_int = isint;
	min_val = min;
	max_val = max;
	if (style & STI_COMBO)
		SetComboStr(combo_text);
}

inline DWORD CDataItem::GetStyle()
{
	return style;
}
float CDataItem::GetValue()
{
	return value;
}
inline float CDataItem::GetRealyValue()
{
	return value;
}
void CDataItem::SetValue(float nv)
{
	if (nv<min_val) 	nv = min_val;
	else if (nv>max_val)nv = max_val;
	value = nv;
}
inline void CDataItem::SetRealyValue(float nv)
{
	value = nv;
}
inline bool CDataItem::GetCheck()
{
	return check;
}
inline void CDataItem::SetCheck(bool nch)
{
	check = nch;
}

float CDataItem::Delta()
{
	if (fabs(value/100)<0.1f)
		return 0.1f;
	return fabs(value/100);
}

CString CDataItem::GetStrValue()
{
	if (GetStyle()&STI_EDIT)
		if (is_int) return ::ToStr(round(GetValue()));
		else return ::ToStr(GetValue());
	if (GetStyle()&STI_COMBO)
		return GetComboStr(round(GetValue()));
	return "";
}
void CDataItem::SetStrValue(CString& nv)
{
	if (GetStyle()&STI_EDIT)
		if (is_int) SetValue(round(atof(nv)));
		else SetValue(atof(nv));
	if (GetStyle()&STI_COMBO)
	{
		int i = combo_stings.size()-1;
		for(;i>=0;--i)
			if (combo_stings[i].str==nv)
				break;
		if ((UINT)i>=combo_stings.size())
			return;
		value = i;
	}
}

inline int CDataItem::GetComboStrCount()
{
	return combo_stings.size();
}
bool CDataItem::GetComboStrVisible(int ix)
{
	if ((UINT)ix>=combo_stings.size()){ASSERT(false);return 0;}
	return combo_stings[ix].Visible;
}
void CDataItem::SetComboStrVisible(int i, bool nb)
{
	if ((UINT)i>=combo_stings.size()){ASSERT(false);return;}
	combo_stings[i].Visible = nb;
}
const CString& CDataItem::GetComboStr(int ix)
{
	static const CString sss;
	if ((UINT)ix>=combo_stings.size()){ASSERT(false);return sss;}
	return combo_stings[ix].str;
}




COptTree::COptTree()
	: CFormView(COptTree::IDD),ined(0)
{
	B[0] = true;
	B[1] = false;
	theApp.scene.bShowWorld = 1;
	theApp.scene.bShowGrid = 1;
	curent_editing = -1;
}

void COptTree::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM1, treeList);
}

BEGIN_MESSAGE_MAP(COptTree, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_BUT_ONE, UpDateToolBar)
	ON_COMMAND(ID_BUT_TWO, UpDateToolBar)
ON_WM_TIMER()
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// COptTree diagnostics
void COptTree::UpDateToolBar()
{
	if (ined)
	{
		PutButton(B[0]==1);
		ShowOptEmiter();
	}
}

#ifdef _DEBUG
void COptTree::AssertValid() const
{
	CFormView::AssertValid();
}

void COptTree::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// COptTree message handlers

int COptTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	BOOL create=m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS|CBRS_FLYBY);
	BOOL load=m_wndToolBar.LoadToolBar(IDR_TOOLBAR_OPT);
	m_wndToolBar.SetButtonStyle(0, TBBS_CHECKBOX);
	m_wndToolBar.SetButtonStyle(1, TBBS_CHECKBOX);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_BUT_ONE, true);
	return 0;
}
void COptTree::PutButton(int ix)
{
   	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_BUT_ONE, B[0]=ix==0);
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_BUT_TWO, B[1]=ix==1);
}
/*
const nEMMITER_TYPE = 4;
int nVEL_TYPE = 8;
const nSPLINE_ENDING=3;
const nPOS_TYPE = 7;
const nOUT_TYPE = 3;
const nTYPE_DIRECTION = 4;
const nPOINTS_LOCATION = 2;
char* EMMITER_TYPE[nEMMITER_TYPE] = {"Базовый","Поверхность","Сплайн","Свет"};
char* VEL_TYPE[9] = {"Box","Cylinder","Sphere","Line","Normal in","Normal out",
					"Normal in out","Vertical","Normal 3D Model"};
char* SPLINE_ENDING[nSPLINE_ENDING] = {"Closed", "Free", "Cycled"};
char* YN[] = {"Нет","Да"};
char* POS_TYPE[nPOS_TYPE] = {"Box","Cylinder","Sphere","Line","Ring","3DModel","3DModelInside"};
char* OUT_TYPE[nOUT_TYPE] = {"Умножение","Сложение","Вычитание"};
char* TYPE_DIRECTION[nTYPE_DIRECTION] = {"Перемещение","Случайное","Фигура 1","Фигура 2"};
char* POINTS_LOCATION[nPOINTS_LOCATION] = {"Вручную","Спираль"};
*/
CTreeListItem* COptTree::FindItem(DWORD id,CTreeListItem* item)
{
	if (!item) item = treeList.GetRootItem();
	if (!item) return NULL;
	do
	{
		if (treeList.GetItemData(item)==id) 
			return item;
		CTreeListItem* chit = treeList.GetNextItem(item,TLGN_CHILD);
		if (chit&&(chit=FindItem(id,chit)))
			return chit;

	}while((item = treeList.GetNextItem(item,TLGN_NEXT)));
	return NULL;
}
CTreeListItem* COptTree::InsertItem(LPSTR cap,DWORD dat,CTreeListItem* item = TLI_ROOT)
{
	CTreeListItem* citem = treeList.InsertItem(cap, item);
	treeList.ShowItemCheckBox(citem,GetDataItem(dat)->GetStyle() & STI_CHECKBOX);
	treeList.SetItemData(citem,dat);
	return citem;
}
bool COptTree::SetItemText(DWORD id,CString s)
{
	CTreeListItem* item = FindItem(id);
	if (!item) return false;
	return treeList.SetItemText(item,1,s);
}
bool COptTree::SetItemСheck(DWORD id,bool v)
{
	CTreeListItem* item = FindItem(id);
	if (!item) return false;
	if (GetDataItem(id)->GetStyle()&STI_CHECKBOX)
		return treeList.SetItemCheck(item,v,0);
	if (GetDataItem(id)->GetStyle()&STI_CHECKGRAPH)
		treeList.SetItemImage(item,v);
	return true;
}
void COptTree::UpdateData()
{
	for(int i = items.size()-1; i>=0; --i)
	{
		CDataItem* it = GetDataItem(i);
		if(it->GetStyle()& (STI_EDIT | STI_COMBO))
			SetItemText(it->GetIndex(),	it->GetStrValue());
		if((it->GetStyle()& STI_CHECKBOX) || (it->GetStyle()& STI_CHECKGRAPH))
			SetItemСheck(it->GetIndex(),	  it->GetCheck());
	}
	Spiral();
}

void COptTree::AddItem(DWORD ix, DWORD style, float min_val, float max_val, int isint)
{
	items.push_back();
	items.back().Init(ix, style, min_val, max_val, isint);
}

void COptTree::AddItem(DWORD ix, DWORD style, CString cs)
{
	ASSERT(style & STI_COMBO);
	items.push_back();
	items.back().Init(ix, style, -fl_max, fl_max, true, cs);
}

void COptTree::InitTreeList()
{
	treeList.SetStyle
		( 
		  TLC_TREELIST								// TreeList or List
//		| TLC_SHOWSELACTIVE							// show active column of selected item
		| TLC_SHOWSELFULLROWS						// show selected item in fullrow mode
		| TLC_HGRID									// show horizonal grid lines
		| TLC_VGRID									// show vertical grid lines
//		| TLC_TREELINE								// show tree line
		| TLC_ROOTLINE								// show root line
//		| TLC_BUTTON								// show expand/collapse button [+]
		| TLC_CHECKBOX								// show check box
		| TLC_NOAUTOCHECK
		| TLC_HOTTRACK								// show hover text 
		| TLC_HEADDRAGDROP							// head drag drop
		| TLC_IMAGE									// show image
		);

	treeList.InsertColumn("0", TLF_DEFAULT_LEFT, 100);
	treeList.InsertColumn("1", TLF_DEFAULT_LEFT, 100);

	treeList.SetColumnModify(0, TLM_STATIC);
	treeList.SetColumnModify(1, TLM_REQUEST);
	treeList.setChangeItemLabelNotifyListener(this);
	treeList.setItemChangeNotifyListener(this);
	treeList.setGeneralNotifyListener(this);
	const graph_max = 200;
	const graph_min = -200;
	ImageList.Create(IDB_GRAPH_CHECKBOX, 16, 0, RGB(255, 255, 0));
	treeList.SetImageList(&ImageList);

		AddItem(IX_NONE,			STI_TITLE);
		AddItem(IX_ShowGrid,		STI_CHECKBOX);
		AddItem(IX_ShowWorld,		STI_CHECKBOX);
		AddItem(IX_Scale,			STI_EDIT, 1, 1000, true);
		AddItem(IX_Rate,			STI_EDIT, 1, 100, true);
		AddItem(IX_ShowFigure,		STI_CHECKBOX);
		AddItem(IX_SpriteBlend, 	STI_COMBO,"Умножение,Сложение,Вычитание,");
		AddItem(IX_PosType,			STI_COMBO, "Box,Cylinder,Sphere,Line,Ring,3DModel,3DModelInside,По эмитеру,");
		AddItem(IX_X, 				STI_EDIT,0,+1e10,true);
		AddItem(IX_Y, 				STI_EDIT,0,+1e10,true);
		AddItem(IX_Z, 				STI_EDIT,0,+1e10,true);
		AddItem(IX_AlphaMin, 		STI_EDIT, -180, 180, true);
		AddItem(IX_AlphaMax, 		STI_EDIT, -180, 180, true);
		AddItem(IX_ThetaMin, 		STI_EDIT, -90, 90, true);
		AddItem(IX_ThetaMax, 		STI_EDIT, -90, 90, true);
		AddItem(IX_UseLight, 		STI_CHECKBOX);
		AddItem(IX_EmitterType, 	STI_COMBO,"Базовый,Поверхность,Сплайн,Свет,");
		AddItem(IX_PointsCount, 	STI_EDIT, 1, 100, true);
		AddItem(IX_PointLoc,		STI_COMBO,"Вручную,Спираль,");
		AddItem(IX_SpRadius1, 		STI_EDIT,0,1000,true);
		AddItem(IX_SpRadius2, 		STI_EDIT,0,1000,true);
		AddItem(IX_SpHeight1, 		STI_EDIT,0,10000,true);
		AddItem(IX_SpHeight2, 		STI_EDIT,0,10000,true);
		AddItem(IX_SpAngle,	  		STI_EDIT,-180,180,true);
		AddItem(IX_SpCompress,		STI_EDIT, 0,200);
		AddItem(IX_EmitterTimeLife,	STI_EDIT,0.1f,60);
		AddItem(IX_ParticlesCount,	STI_GRAPH, 0, 10000, true);
		AddItem(IX_Prolonged, 		STI_CHECKBOX);
		AddItem(IX_PlayCycled,		STI_CHECKBOX);
		AddItem(IX_Velocity,		STI_GRAPH,graph_min, graph_max);
		AddItem(IX_Mul,				STI_GRAPH,graph_min, graph_max);
		AddItem(IX_VelType,			STI_COMBO,"Box,Cylinder,Sphere,Line,Normal in,Normal out,Normal in out,Vertical,Normal 3D Model,Invariably,");
		AddItem(IX_SplineEnding,	STI_COMBO,"Closed,Free,Cycled,");
		AddItem(IX_SplTypeDirection,STI_COMBO,"Перемещение,Случайное,Фигура 1,Фигура 2,");
		AddItem(IX_Size,			STI_GRAPH,0, graph_max);
		AddItem(IX_Gravitation,		STI_GRAPH,graph_min, graph_max);
		AddItem(IX_AngleVel,		STI_GRAPH,graph_min, graph_max);
		AddItem(IX_Plume,			STI_CHECKBOX);
		AddItem(IX_PlInterval,		STI_EDIT, 1 , 1000);
		AddItem(IX_PlTracesCount,	STI_EDIT,1, 1000,true);
		AddItem(IX_PlTimeScale,		STI_EDIT);
		AddItem(IX_PlSizeScale,		STI_EDIT,0);
		AddItem(IX_DeltaSize, 		STI_GRAPH,0, graph_max);
		AddItem(IX_DeltaLifeTime, 	STI_GRAPH,0, graph_max);
		AddItem(IX_DeltaVel,  		STI_GRAPH,graph_min, graph_max);
		AddItem(IX_AngleChaos,		STI_CHECKBOX);
		AddItem(IX_SurAddZ,			STI_EDIT,0);
		AddItem(IX_SurAngle,		STI_EDIT, -180, 180, true);
		AddItem(IX_SurCenter,		STI_CHECKBOX);
		AddItem(IX_SurUseForceField,STI_CHECKBOX);
		AddItem(IX_SurPlanar,		STI_CHECKBOX);
		AddItem(IX_OtherEmiter,		STI_COMBO," ,");
		AddItem(IX_Filling,			STI_CHECKBOX);
		AddItem(IX_Fix_X,			STI_EDIT,1,1000,true);
		AddItem(IX_Fix_Y,			STI_EDIT,1,1000,true);
		AddItem(IX_Fix_Z,			STI_EDIT,1,1000,true);
		AddItem(IX_Fix_Pos,			STI_CHECKBOX);
		AddItem(IX_PlSmooth,		STI_CHECKBOX);
		AddItem(IX_Realtive,		STI_CHECKBOX);
		for(int i=0;i<items.size();++i)
		{
			ASSERT(i==items[i].GetIndex());
		}
		GetDataItem(IX_Velocity)->SetCheck(true);
		Write(IX_Scale, 100);
		SetTimer(WM_TIMER,100,NULL);
}

DWORD COptTree::onRequestCtrl(CTreeListCtrl& source,CHANGE_LABEL_NOTIFY_INFO* info)
{
	DWORD dat = source.GetItemData(info->pItem);
	if (dat<items.size())
	{
		DWORD st  = items[dat].GetStyle();
		if (st & STI_EDIT)
			return TLM_EDIT;
		if (st & STI_COMBO)
			return TLM_COMBO;
	}
	return TLM_STATIC;		
}

bool COptTree::onBeginControl(CTreeListCtrl& source, CHANGE_LABEL_NOTIFY_INFO* info)
{
	DWORD dat = source.GetItemData(info->pItem);
	if (dat<items.size())
	{
		curent_editing = dat;
		CDataItem& item = *GetDataItem(dat);
		switch (onRequestCtrl(source, info))
		{
		case TLM_COMBO:
			{
				CComboBox *cb = static_cast<CComboBox*>(info->pEditControl);
				cb->ResetContent();
				for(int i=0; i<item.GetComboStrCount(); ++i)
					if (item.GetComboStrVisible(i))
						cb->AddString(item.GetComboStr(i));
				cb->SelectString(-1,item.GetStrValue());
			}
			break;
		case TLM_EDIT:
			info->pEditControl->SetWindowText(item.GetStrValue());
			break;
		}
	}
	return false;
}
void COptTree::onEndControl(CTreeListCtrl& source,CHANGE_LABEL_NOTIFY_INFO* info)
{
	curent_editing = -1;
}
bool COptTree::onBeginLabelEdit(CTreeListCtrl& source, CHANGE_LABEL_NOTIFY_INFO* info)
{
	return onRequestCtrl(source,info)!=TLM_STATIC&&info->iCol==1;
}

	//! Уведомляет о том, что редактирование закончено
bool COptTree::onEndLabelEdit(CTreeListCtrl& source, CHANGE_LABEL_NOTIFY_INFO* info)
{
	if (info->text=="") return false;
	DWORD dat = source.GetItemData(info->pItem);
	if (dat<items.size())
	{
		CDataItem& item = items[source.GetItemData(info->pItem)];
		item.SetStrValue(info->text);
		int pos = treeList.GetScrollPos(1);
		SaveControlsData(dat);
		treeList.SetScrollPos(1,pos, false);
		UpdateData();
	}
	return false;
}
void COptTree::onClick(CTreeListCtrl& source, GENERAL_NOTIFY_INFO* info)
{
	static int myClick = 0;
	if (myClick)
	{
		myClick--;
		return;
	}
	if (info->item==NULL)
		return;
	if(info->iSubItem==1)
	{
		CPoint pt;
		GetCursorPos(&pt);
		if (GetDataItem(source.GetItemData(info->item))->GetStyle() & STI_COMBO) 
		{
			CRect t;
			::GetWindowRect(GetDlgItem(IDC_CUSTOM1)->GetSafeHwnd(),&t);
			pt.x-=t.left;
			pt.y-=t.top;
			DWORD pp = ((DWORD)pt.y<<16)+(WORD)pt.x;
			myClick = true;
			myClick = 2;
			::PostMessage(GetDlgItem(IDC_CUSTOM1)->GetSafeHwnd(),WM_LBUTTONDOWN,0,(LPARAM)pp);
			::PostMessage(GetDlgItem(IDC_CUSTOM1)->GetSafeHwnd(),WM_LBUTTONUP,0,(LPARAM)pp);
			::PostMessage(GetDlgItem(IDC_CUSTOM1)->GetSafeHwnd(),WM_LBUTTONDOWN,0,(LPARAM)pp);
			::PostMessage(GetDlgItem(IDC_CUSTOM1)->GetSafeHwnd(),WM_LBUTTONUP,0,(LPARAM)pp);
		}
	}
	else
	{	
		DWORD dat = source.GetItemData(info->item);
		if (dat>=items.size())
			return;
		CDataItem& item = items[source.GetItemData(info->item)];
		if (item.GetStyle() & STI_CHECKGRAPH)
		{
			item.SetCheck(!item.GetCheck());
			treeList.SetItemImage(info->item,item.GetCheck());
			ctrv->ShowKeys();
		}
	}
}
void COptTree::onItemCheckChanged(CTreeListCtrl& source, ITEM_CHANGE_INFO* info)
{
	DWORD dat = source.GetItemData(info->item);
	if (dat>=items.size())
		return;
	CDataItem* item = GetDataItem(dat);
	bool ch = treeList.GetItemCheck(info->item); 
	if (item&&item->GetCheck()!=ch)
	{

		item->SetCheck(ch);
		SaveControlsData(dat);
	}
}
CDataItem* COptTree::GetDataItem(int ix)
{
	ASSERT((UINT)ix<items.size());
	return &items[ix];
}
inline float COptTree::floatV(int ix)
{
	CDataItem* it = GetDataItem(ix);
	if (!it){ASSERT(false); return 0;}
	return it->GetRealyValue();
}
int COptTree::intV(int ix)
{
	CDataItem* it = GetDataItem(ix);
	if (!it){ASSERT(false); return 0;}
	return round(it->GetRealyValue());
}
bool COptTree::boolV(int ix)
{
	CDataItem* it = GetDataItem(ix);
	if (!it){ASSERT(false); return 0;}
	return it->GetCheck();
}
void COptTree::Write(int ix, float* p_nv)
{
	ASSERT(p_nv);
	CDataItem* it = GetDataItem(ix);
	if (!it){ASSERT(false); return;}
	if (it->GetStyle() & (STI_EDIT|STI_COMBO))
	{
		it->SetValue(*p_nv);//Realy
		if (it->GetValue()!=*p_nv)
			*p_nv = it->GetValue();
	}
	else it->SetCheck(*p_nv);
}
void COptTree::Write(int ix, float nv)
{
	CDataItem* it = GetDataItem(ix);
	if (!it){ASSERT(false); return;}
	if (it->GetStyle() & (STI_EDIT|STI_COMBO))
		it->SetValue(nv);//Realy
	else it->SetCheck(nv);
}
void COptTree::ChangeEmitterName(CString old_name, CString new_name)
{
	CEffectData* ek = GetDocument()->m_pActiveEffect;
	if (!ek)
		return;
	vector<CEmitterData*>::iterator it;
	FOR_EACH(ek->emitters,it)
		if ((*it)->data)
			if (old_name ==(*it)->data->other.c_str())
				(*it)->data->other = new_name;
	UpdateData();
}
void COptTree::CheckRangeEmitter()
{
	CEffectToolDoc* pDoc = GetDocument();
	if (!pDoc) 
		return;
	CEmitterData* pActiveEmitter = pDoc->m_pActiveEmitter;
	if (pActiveEmitter)
	{
		int n;
		if (pActiveEmitter->data)
			n = pActiveEmitter->data->emitter_position.size();
		else 
			n = pActiveEmitter->data_light->emitter_position.size();
		int t = GetDocument()->m_nCurrentGenerationPoint;
		for(int i = n-1; i>=0; --i)
		{
			GetDocument()->m_nCurrentGenerationPoint = i;
			SetControlsData(false, i==t);
		}
		GetDocument()->m_nCurrentGenerationPoint = t;

		if (pActiveEmitter->data)
			n = pActiveEmitter->data->p_size.size();
		else 
			n = pActiveEmitter->data_light->emitter_size.size();
		t = GetDocument()->m_nCurrentParticlePoint;
		for(int i = n-1; i>=0; --i)
		{
			GetDocument()->m_nCurrentParticlePoint = i;
			SetControlsData(false, i==t);
		}
		GetDocument()->m_nCurrentParticlePoint = t;
	}
}

void COptTree::SetControlsData(bool show /*= true*/, bool update /*= false*/)
{
	static bool stop = false;
	if (stop) return;
	stop = true;
	Write(IX_Rate,			100*theApp.scene.m_particle_rate);
	Write(IX_ShowFigure,	theApp.scene.bShowEmitterBox);
	Write(IX_ShowWorld,		theApp.scene.bShowWorld);
	Write(IX_ShowGrid,		theApp.scene.bShowGrid);

	CEffectToolDoc* pDoc = GetDocument();
	if (!pDoc){stop = false;return;}
	CEmitterData* pActiveEmitter = pDoc->m_pActiveEmitter;
	if (!pActiveEmitter){stop = false;return;}
	if(pActiveEmitter->Class()!=EMC_LIGHT)
	{
		Write(IX_SpriteBlend, pActiveEmitter->data->sprite_blend);
		Write(IX_PosType, pActiveEmitter->data->particle_position.type);

		Write(IX_X, pActiveEmitter->data->particle_position.size.x*100);
		Write(IX_Y, pActiveEmitter->data->particle_position.size.y*100);
		Write(IX_Z, pActiveEmitter->data->particle_position.size.z*100);
		Write(IX_Fix_Pos, pActiveEmitter->data->particle_position.fix_pos);
		Write(IX_Fix_X, pActiveEmitter->data->particle_position.num.x);
		Write(IX_Fix_Y, pActiveEmitter->data->particle_position.num.y);
		Write(IX_Fix_Z, pActiveEmitter->data->particle_position.num.z);

		Write(IX_Realtive,pActiveEmitter->data->relative);
		Write(IX_Filling, pActiveEmitter->data->chFill);

		Write(IX_PointsCount, pActiveEmitter->data->num_particle.size());
		Write(IX_EmitterTimeLife, &pActiveEmitter->data->emitter_life_time);
		Write(IX_ParticlesCount, &pActiveEmitter->data->num_particle[GetDocument()->m_nCurrentGenerationPoint].f);
		Write(IX_DeltaSize, &pActiveEmitter->data->begin_size_delta[GetDocument()->m_nCurrentGenerationPoint].f);
		Write(IX_DeltaLifeTime, &pActiveEmitter->data->life_time_delta[GetDocument()->m_nCurrentGenerationPoint].f);
		Write(IX_Size, &pActiveEmitter->data->p_size[GetDocument()->m_nCurrentParticlePoint].f);
		Write(IX_AngleVel, &pActiveEmitter->data->p_angle_velocity[GetDocument()->m_nCurrentParticlePoint].f);

		Write(IX_AngleChaos, pActiveEmitter->data->rotation_direction == ETRD_RANDOM);

		Write(IX_AlphaMin, R2G(pActiveEmitter->data->particle_position.alpha_min));
		Write(IX_AlphaMax, R2G(pActiveEmitter->data->particle_position.alpha_max));
		Write(IX_ThetaMin, R2G(pActiveEmitter->data->particle_position.teta_min));
		Write(IX_ThetaMax, R2G(pActiveEmitter->data->particle_position.teta_max));

		Write(IX_Plume, pActiveEmitter->data->chPlume);
		Write(IX_PlTracesCount, pActiveEmitter->data->TraceCount);
		Write(IX_PlInterval, pActiveEmitter->data->PlumeInterval*100);
		Write(IX_PlSmooth, pActiveEmitter->data->smooth);
		if (SpiralDef())
		{
			int ix = -1;
			if (intV(IX_PointsCount)>1&&theApp.scene.m_ToolMode == CD3DScene::TOOL_POSITION)
				ix = 0;
			else if (pActiveEmitter->Class()==EMC_SPLINE && 
				theApp.scene.m_ToolMode == CD3DScene::TOOL_SPLINE)
				ix = 1;
			if (ix!=-1)
			{
				SpiralData::Dat &sdat = pActiveEmitter->data->sdat.GetData(ix);
				Write(IX_SpAngle,R2G(sdat.dalpha));
				Write(IX_SpRadius1,sdat.r1);
				Write(IX_SpRadius2,sdat.r2);
				Write(IX_SpHeight1,sdat.h1);
				Write(IX_SpHeight2,sdat.h2);
				Write(IX_SpCompress,sdat.compress*100);
			}
		}
		Write(IX_PlayCycled, pActiveEmitter->data->cycled);
		Write(IX_Prolonged, pActiveEmitter->data->generate_prolonged);
//		if (pActiveEmitter->data->particle_position.type==EMP_OTHER_EMITTER)
		{
			CEffectData* ek = pDoc->m_pActiveEffect;
			CString s = "не связан,";
			CString cur = pActiveEmitter->data->other.c_str();
			vector<CEmitterData*>::iterator it;
			int ix=0,i=0;
			FOR_EACH(ek->emitters,it)
			{
				if (*it!=pActiveEmitter && (*it)->data)
				{
					if (cur==(*it)->data->name.c_str())
						ix = i+1;
					s+=((*it)->data->name + ",").c_str();
					++i;
				}
			}
			GetDataItem(IX_OtherEmiter)->SetComboStr(s);
			Write(IX_OtherEmiter,ix);
		}
	}
	else 
	{
		Write(IX_EmitterType,TYPE_LIGHT);
		Write(IX_PointsCount, pActiveEmitter->data_light->emitter_position.size());
		Write(IX_EmitterTimeLife, &pActiveEmitter->data_light->emitter_life_time);
		Write(IX_PlayCycled, pActiveEmitter->data_light->cycled);
		Write(IX_Size, &pActiveEmitter->data_light->emitter_size[GetDocument()->m_nCurrentParticlePoint].f);

		if (SpiralDef()&&(intV(IX_PointsCount)>1))
		{
			if (theApp.scene.m_ToolMode == CD3DScene::TOOL_POSITION)
			{
				SpiralData::Dat &sdat = pActiveEmitter->data->sdat.GetData(0);
				Write(IX_SpAngle,R2G(sdat.dalpha));
				Write(IX_SpRadius1,sdat.r1);
				Write(IX_SpRadius2,sdat.r2);
				Write(IX_SpHeight1,sdat.h1);
				Write(IX_SpHeight2,sdat.h2);
				Write(IX_SpCompress,sdat.compress*100);
			}
		}
//		Write(IX_Prolonged, pActiveEmitter->data_light->generate_prolonged);
	}
	if (pActiveEmitter->GetSpl())
	{
		Write(IX_EmitterType,TYPE_SPLINE);
		Write(IX_SplineEnding, pActiveEmitter->GetSpl()->p_position.cbegin);
		Write(IX_SplineEnding, pActiveEmitter->GetSpl()->p_position.cend);
		Write(IX_SplTypeDirection, pActiveEmitter->GetSpl()->direction);
	}
	if (pActiveEmitter->GetInt())
	{
		Write(IX_EmitterType,(float)TYPE_INTEGRAL);
		Write(IX_DeltaVel, &pActiveEmitter->GetInt()->velocity_delta[GetDocument()->m_nCurrentGenerationPoint].f);
		Write(IX_Gravitation, &pActiveEmitter->GetInt()->p_gravity[GetDocument()->m_nCurrentParticlePoint].f);
		Write(IX_Velocity, &pActiveEmitter->GetInt()->p_velocity[GetDocument()->m_nCurrentParticlePoint].f);
		Write(IX_VelType, pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint].velocity);
		GetDataItem(IX_VelType)->SetComboStrVisible(EMV_NORMAL_3D_MODEL, intV(IX_PosType) == EMP_3DMODEL_INSIDE || intV(IX_PosType) == EMP_3DMODEL);
		Write(IX_Mul, &pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint].mul);
		Write(IX_UseLight, pActiveEmitter->GetInt()->use_light);
	}
	if (pActiveEmitter->GetZ())
	{
		Write(IX_EmitterType,TYPE_SURFACE);
		Write(IX_SurAddZ, pActiveEmitter->GetZ()->add_z);
		Write(IX_SurCenter, pActiveEmitter->GetZ()->angle_by_center);
		Write(IX_SurAngle, R2G(pActiveEmitter->GetZ()->base_angle));
		Write(IX_SurPlanar, pActiveEmitter->GetZ()->planar);
		Write(IX_SurUseForceField, pActiveEmitter->GetZ()->use_force_field);
	}
	if (show) ShowOptEmiter();
	if(update) UpdateData();
	stop = false;
} 

void COptTree::SaveControlsData(DWORD IX, bool show /*= true*/)
{
	static bool stop = false;
	if (stop) return;
	stop = true;
	theApp.scene.m_particle_rate = floatV(IX_Rate)/100;
	theApp.scene.bShowEmitterBox = boolV(IX_ShowFigure);
	theApp.scene.bShowWorld = boolV(IX_ShowWorld);
	theApp.scene.bShowGrid = boolV(IX_ShowGrid);

	CEffectToolDoc* pDoc = GetDocument();
	if (!pDoc){stop = false; return;}
	if(IX == IX_Scale && pDoc->m_pActiveEffect)
	{
		float scale = intV(IX_Scale)/100.0f;
		Write(IX_Scale,100);
		pDoc->m_pActiveEffect->RelativeScale(scale);
	}
	CEmitterData* pActiveEmitter = pDoc->m_pActiveEmitter;
	if (!pActiveEmitter){stop = false; return;}

	switch(IX)
	{
	case IX_EmitterType:
		mf->SplineEndEdit();
		pDoc->m_nCurrentGenerationPoint = 0;
		pDoc->m_nCurrentParticlePoint = 0;
		if (intV(IX_EmitterType)==TYPE_LIGHT)
		{ 
			CEffectData* ek = pDoc->m_pActiveEffect;
			ASSERT(ek);
			vector<CEmitterData*>::iterator it;
			FOR_EACH(ek->emitters,it)
				if ((*it)->data)
					if ((*it)->data->other == pActiveEmitter->data->name)
						(*it)->data->other = "";
		}
		ctrv->m_nEmmiterType = intV(IX_EmitterType);
		ctrv->OnSelchangeComboEmmiterType();
//		SetControlsData(false,false);
		break;
	case IX_PointsCount:
		pActiveEmitter->SetGenerationPointCount(intV(IX_PointsCount));
		pDoc->m_nCurrentGenerationPoint = 0;
		break;
	case IX_EmitterTimeLife:
		pActiveEmitter->ChangeLifetime(floatV(IX_EmitterTimeLife));
		break;
	case IX_PointLoc:
		Spiral(true);
		break;
	}

	if(pActiveEmitter->Class()!=EMC_LIGHT)
	{
		pActiveEmitter->data->sprite_blend = (EMITTER_BLEND)intV(IX_SpriteBlend);
		pActiveEmitter->data->particle_position.type = (EMITTER_TYPE_POSITION)intV(IX_PosType);
		switch(intV(IX_PosType))
		{
		case EMP_3DMODEL_INSIDE:
		case EMP_3DMODEL:
			GetDataItem(IX_VelType)->SetComboStrVisible(8, true);
			mf->m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TOOLS_DIR, FALSE);
			if(theApp.scene.m_ToolMode == CD3DScene::TOOL_DIRECTION)
				theApp.scene.m_ToolMode = CD3DScene::TOOL_NONE;
			pDoc->Load3DModel(MODE_FIND, TYPE_3DMODEL);
			pDoc->Load3DModel(MODE_FIND, TYPE_3DBACK);
			mf->UpdateControlBar();
			break;
		default:
			break;
		}
		pActiveEmitter->data->particle_position.size.x = floatV(IX_X)/100;
		pActiveEmitter->data->particle_position.size.y = floatV(IX_Y)/100;
		pActiveEmitter->data->particle_position.size.z = floatV(IX_Z)/100;

		pActiveEmitter->data->particle_position.fix_pos = boolV(IX_Fix_Pos);
		pActiveEmitter->data->particle_position.num.x = floatV(IX_Fix_X);
		pActiveEmitter->data->particle_position.num.y = floatV(IX_Fix_Y);
		if (boolV(IX_Filling) || pActiveEmitter->data->particle_position.type==EMP_BOX)
			pActiveEmitter->data->particle_position.num.z = floatV(IX_Fix_Z);
		else pActiveEmitter->data->particle_position.num.z = 1;

		pActiveEmitter->data->relative = boolV(IX_Realtive);
		pActiveEmitter->data->chFill = boolV(IX_Filling);

		pActiveEmitter->data->num_particle[GetDocument()->m_nCurrentGenerationPoint].f = floatV(IX_ParticlesCount);
		pActiveEmitter->data->begin_size_delta[GetDocument()->m_nCurrentGenerationPoint].f = floatV(IX_DeltaSize);
		pActiveEmitter->data->life_time_delta[GetDocument()->m_nCurrentGenerationPoint].f = floatV(IX_DeltaLifeTime);
		pActiveEmitter->data->p_size[GetDocument()->m_nCurrentParticlePoint].f = floatV(IX_Size);
		pActiveEmitter->data->p_angle_velocity[GetDocument()->m_nCurrentParticlePoint].f = floatV(IX_AngleVel);

		pActiveEmitter->data->rotation_direction = boolV(IX_AngleChaos) ? ETRD_RANDOM : ETRD_CW;

		pActiveEmitter->data->particle_position.alpha_min = G2R(floatV(IX_AlphaMin));
		pActiveEmitter->data->particle_position.alpha_max = G2R(floatV(IX_AlphaMax));
		pActiveEmitter->data->particle_position.teta_min = G2R(floatV(IX_ThetaMin));
		pActiveEmitter->data->particle_position.teta_max = G2R(floatV(IX_ThetaMax));

		pActiveEmitter->data->chPlume = boolV(IX_Plume);
		pActiveEmitter->data->TraceCount = floatV(IX_PlTracesCount);
		pActiveEmitter->data->PlumeInterval = floatV(IX_PlInterval)/100;
		pActiveEmitter->data->smooth = boolV(IX_PlSmooth);
		if (SpiralDef())
		{
			int ix = -1;
			if (intV(IX_PointsCount)>1&&theApp.scene.m_ToolMode == CD3DScene::TOOL_POSITION)
				ix = 0;
			else if (pActiveEmitter->Class()==EMC_SPLINE 
				&& theApp.scene.m_ToolMode == CD3DScene::TOOL_SPLINE)
				ix = 1;
			if (ix!=-1)
			{
				SpiralData::Dat &sdat = pActiveEmitter->data->sdat.GetData(ix);
				sdat.dalpha = G2R(floatV(IX_SpAngle));
				sdat.r1 = floatV(IX_SpRadius1);
				sdat.r2 = floatV(IX_SpRadius2);
				sdat.h1 = floatV(IX_SpHeight1);
				sdat.h2 = floatV(IX_SpHeight2);
				sdat.compress = floatV(IX_SpCompress)/100;
			}
		}
		pActiveEmitter->data->cycled = boolV(IX_PlayCycled);
		pActiveEmitter->data->generate_prolonged = boolV(IX_Prolonged);
		if (pActiveEmitter->data->particle_position.type==EMP_OTHER_EMITTER )
			pActiveEmitter->data->other = GetDataItem(IX_OtherEmiter)->GetStrValue();
		else
			pActiveEmitter->data->other = ""; 
	}
	else 
	{
		pActiveEmitter->data_light->cycled = boolV(IX_PlayCycled);
		pActiveEmitter->data_light->emitter_size[GetDocument()->m_nCurrentParticlePoint].f = floatV(IX_Size);
		if (SpiralDef()&&(intV(IX_PointsCount)>1&&theApp.scene.m_ToolMode == CD3DScene::TOOL_POSITION))
		{
			SpiralData::Dat &sdat = pActiveEmitter->data->sdat.GetData(0);
			sdat.dalpha = G2R(floatV(IX_SpAngle));
			sdat.r1 = floatV(IX_SpRadius1);
			sdat.r2 = floatV(IX_SpRadius2);
			sdat.h1 = floatV(IX_SpHeight1);
			sdat.h2 = floatV(IX_SpHeight2);
			sdat.compress = floatV(IX_SpCompress)/100;
		}
	}
	if (pActiveEmitter->GetSpl())
	{
		pActiveEmitter->GetSpl()->p_position.cbegin = (CKeyPosHermit::CLOSING)intV(IX_SplineEnding);
		pActiveEmitter->GetSpl()->p_position.cend   = (CKeyPosHermit::CLOSING)intV(IX_SplineEnding);
		pActiveEmitter->GetSpl()->direction = (EMITTER_TYPE_DIRECTION_SPL)intV(IX_SplTypeDirection);
	}
	if (pActiveEmitter->GetInt())
	{
		pActiveEmitter->GetInt()->velocity_delta[GetDocument()->m_nCurrentGenerationPoint].f = floatV(IX_DeltaVel);
		pActiveEmitter->GetInt()->p_gravity[GetDocument()->m_nCurrentParticlePoint].f = floatV(IX_Gravitation);
		pActiveEmitter->GetInt()->p_velocity[GetDocument()->m_nCurrentParticlePoint].f = floatV(IX_Velocity);
		pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint].velocity = (EMITTER_TYPE_VELOCITY)intV(IX_VelType);
		pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint].mul = floatV(IX_Mul);
		pActiveEmitter->GetInt()->use_light = boolV(IX_UseLight);
		if (IX == IX_PosType && intV(IX_PosType) != EMP_3DMODEL_INSIDE && intV(IX_PosType) != EMP_3DMODEL)
		{
			std::vector<EffectBeginSpeed>::iterator it;
			FOR_EACH(pActiveEmitter->GetInt()->begin_speed, it)
				if (it->velocity == EMV_NORMAL_3D_MODEL)
					it->velocity = EMV_BOX;
			if(intV(IX_VelType) == EMV_NORMAL_3D_MODEL)
				Write(IX_VelType, EMV_BOX);
		}
	}
	if (pActiveEmitter->GetZ())
	{
		pActiveEmitter->GetZ()->add_z = floatV(IX_SurAddZ);
		pActiveEmitter->GetZ()->angle_by_center = boolV(IX_SurCenter);
		pActiveEmitter->GetZ()->base_angle = G2R(floatV(IX_SurAngle));
		pActiveEmitter->GetZ()->planar = boolV(IX_SurPlanar);
		pActiveEmitter->GetZ()->use_force_field = boolV(IX_SurUseForceField);
	}

	ctrv->ShowKeys();
	ctrv->m_graph.Update(false);
	ctrv->InitSlider();

	pActiveEmitter->bDirty = true;
	theApp.scene.InitEmitters();
	if (show) ShowOptEmiter();
	stop = false;
}

void COptTree::OnInitialUpdate()
{
	__super::OnInitialUpdate();
	static bool first=1;
	if(first)
		InitTreeList();
	first = 0;
	ined=1;
}
void COptTree::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if (ined)
	{
		RECT r;
		GetClientRect(&r);
		r.top+=18;
		treeList.MoveWindow(&r);
		int w = (r.right-r.left)-18;
		treeList.SetColumnWidth(0,w/2);
		treeList.SetColumnWidth(1,w/2);
		m_wndToolBar.MoveWindow(0,0,w+18,18);
		GetDlgItem(IDC_STATIC_FPS)->MoveWindow(70,3,50,15);
		GetDlgItem(IDC_TRIANGLE_COUNT)->MoveWindow(110,3,100,15);
		GetDlgItem(IDC_TRIANGLE_SQUARE)->MoveWindow(170,3,100,15);
	}
}


void COptTree::ShowOptEmiter()
{
	if (!ined) return;
	static bool show = false;
	if (show) return;
	show = true;
	int sel=-1;
	if (treeList.GetSelectedItem())
		sel = treeList.GetItemData(treeList.GetSelectedItem());
	treeList.DeleteAllItems();
	CTreeListItem* item;
	SetControlsData();
	GetDlgItem(IDC_TRIANGLE_COUNT)->ShowWindow(GetDocument()->m_pActiveEffect!=NULL);
	GetDlgItem(IDC_TRIANGLE_SQUARE)->ShowWindow(GetDocument()->m_pActiveEffect!=NULL);
	CEmitterData* pActiveEmitter = GetDocument()->m_pActiveEmitter;
	
	if (B[0])
	{
		item = InsertItem("Cеткa",IX_ShowGrid);
		item = InsertItem("Мир",IX_ShowWorld);
		item = InsertItem("Масштаб",IX_Scale);
		item = InsertItem("P",IX_Rate);
	}
	GetDocument()->m_EnableButForAll = (GetDocument()->m_nCurrentParticlePoint == 0) && 
								   GetDocument()->m_pActiveEmitter;
	if(pActiveEmitter)
	{
		bool b = false;
		bool bl = false;
		bool bz = false;
		bool bFirstPoint = GetDocument()->m_nCurrentParticlePoint == 0;

		bool bX,bY,bZ;
		bX = bY = bZ = false;
		switch(intV(IX_PosType))
		{
		case EMP_BOX:
			bX = true;
			bY = true;
			bZ = true;
			break;
		case EMP_CYLINDER:
			bX = true;
			bY = true;
			break;
		case EMP_LINE:
		case EMP_RING:
		case EMP_SPHERE:
			bX = true;
			break;
		case EMP_3DMODEL:
		case EMP_3DMODEL_INSIDE:
		default:
			break;
		}
		switch(pActiveEmitter->Class())
		{
		case EMC_SPLINE:
			b = false;
			break;
		case EMC_INTEGRAL:
			b = true;
			break;
		case EMC_INTEGRAL_Z:
			b = true;
			bz = true;
			break;
		case EMC_LIGHT:
			b = false;
			bl = true;
			break;
		}
		GetDocument()->m_EnableButForAll = !bl;
		bool b1 = false; 
		if(pActiveEmitter->Class()!=EMC_LIGHT)
		{
			b1 = pActiveEmitter->data->num_particle.size()>1;
			if (B[0])
			{
				InsertItem("Показать",IX_ShowFigure);
				InsertItem("Blending",IX_SpriteBlend);
				InsertItem("Распределение",IX_PosType);
				if (intV(IX_PosType)==EMP_OTHER_EMITTER)
					InsertItem("Связь",IX_OtherEmiter);
				else
				{
					if (bX) InsertItem("X",IX_X);
					if (bY) InsertItem("Y",IX_Y);
					if (bZ) InsertItem("Z",IX_Z);
					if (intV(IX_PosType)==EMP_RING)
					{
						InsertItem("Alpha min",IX_AlphaMin);
						InsertItem("Alpha max",IX_AlphaMax);
						InsertItem("Teta min", IX_ThetaMin);
						InsertItem("Teta max", IX_ThetaMax);
					}
					InsertItem("Заполнение", IX_Filling);
					item = InsertItem("Фиксировать", IX_Fix_Pos);
					if (boolV(IX_Fix_Pos))
					{
						InsertItem("dx",IX_Fix_X);
						InsertItem("dy",IX_Fix_Y);
						if (boolV(IX_Filling)||bZ)InsertItem("dz",IX_Fix_Z);
					}
				}
			}
		}
		if (B[1])
		{
			item = InsertItem("Освещение",IX_UseLight);
			item = InsertItem("Эмитер",IX_NONE);
			InsertItem("Тип",IX_EmitterType,item); 
			InsertItem("Кол-во точек",IX_PointsCount,item);
			if ((intV(IX_PointsCount)>1 && theApp.scene.m_ToolMode == CD3DScene::TOOL_POSITION)
					||((!b&&!bl) && theApp.scene.m_ToolMode == CD3DScene::TOOL_SPLINE))
			{
				CTreeListItem* it = InsertItem("Расположение",IX_PointLoc,item);
				if (intV(IX_PointLoc) == SPIRAL)
				{
					InsertItem("Радиус 1",IX_SpRadius1,it);
					InsertItem("Радиус 2",IX_SpRadius2,it);
					InsertItem("Высота 1",IX_SpHeight1,it);
					InsertItem("Высота 2",IX_SpHeight2,it);
					InsertItem("Угол",	  IX_SpAngle,it);
					InsertItem("Cжатие",  IX_SpCompress,it);
					treeList.Expand(it,TLIS_EXPANDED);
				}
			}
			InsertItem("Т жизни",IX_EmitterTimeLife,item);
			if (bl) 
			{
				CTreeListItem* it = InsertItem("Размер",IX_Size,item);
				if (it)
				{
					treeList.SetItemImage(it, 1);
					GetDataItem(IX_Size)->SetCheck(true);
				}
			}
			treeList.Expand(item,TLIS_EXPANDED);
			item = InsertItem("Генерация",IX_NONE);
			if (!bl) InsertItem("Кол-во частиц",IX_ParticlesCount,item);
 			if (!bl) InsertItem("Непрерывно",IX_Prolonged,item);
			InsertItem("Зациклить",IX_PlayCycled,item);
			treeList.Expand(item,TLIS_EXPANDED);

			if (b||!bl||(!b&&!bl))
			{
				if (!b&&!bl) item = InsertItem("Скорость",IX_NONE);
				else item = InsertItem("Скорость",IX_Velocity);
				if (b)
				{
					InsertItem("Множитель",IX_Mul,item); 
					InsertItem("Распределн.",IX_VelType,item);
				}
				if (!b&&!bl) 
				{
					InsertItem("Сплайн",IX_SplineEnding,item);
					InsertItem("Движение", IX_SplTypeDirection, item);
				}
				treeList.Expand(item,TLIS_EXPANDED);
			}

			if (!bl||b)
			{
				item = InsertItem("Частица",IX_NONE);
				if (!bl) InsertItem("Размер",IX_Size,item);		
				if (b)	 InsertItem("Гравитация",IX_Gravitation,item);	
				if (!bl) InsertItem("Вращение",IX_AngleVel,item);	
				if (!bl) InsertItem("Относительно",IX_Realtive,item);	
				CTreeListItem* it = InsertItem("Шлейф",IX_Plume,item);
				if (boolV(IX_Plume))
				{
					InsertItem("Интервал",IX_PlInterval,item);
					InsertItem("Гибкость",IX_PlTracesCount,item);
					if (bz)InsertItem("Cглаживать",IX_PlSmooth, item);
//					InsertItem("Растяжение", IX_PlTimeScale,item);
//					InsertItem("Размер",IX_PlSizeScale,item);
				}

				treeList.Expand(item,TLIS_EXPANDED);
			}
			
			if (!bl||(b&&bFirstPoint)||(!bl&&bFirstPoint))	
			{
				item = InsertItem("Разброс",IX_NONE);
				if (!bl) InsertItem("Размер",IX_DeltaSize,item);
				if (!bl) InsertItem("Время",IX_DeltaLifeTime,item);
				if (b)   InsertItem("Скорость",IX_DeltaVel,item);
				if (!bl) InsertItem("Ротор",IX_AngleChaos,item);
				treeList.Expand(item,TLIS_EXPANDED);
			}

			if(bz)
			{
				item = InsertItem("Поверхность",IX_NONE);
				InsertItem("Высота",IX_SurAddZ,item);
				InsertItem("Угол",IX_SurAngle,item);
				InsertItem("Центр",IX_SurCenter,item);
				InsertItem("Поле",IX_SurUseForceField,item);
				InsertItem("Плоский",IX_SurPlanar,item);
				treeList.Expand(item,TLIS_EXPANDED);
			}
		}
	}
	UpdateData();
	if (sel!=-1)
	{
		item = FindItem(sel);
		if (item)
		{
			treeList.SelectItem(item);
			treeList.SetFocusedItem(item);
		}
	}
	show = false;
}
int RBNum(UINT num,USHORT n=2)
{
	UINT t = num;
	int i=1;
	while(t/=10) i++;
	if (i<n) return num;
	return (num/round(pow(10,i-n)))*pow(10,i-n);
}
			  
void COptTree::SetFps(float fps)
{
	GetDlgItem(IDC_STATIC_FPS)->SetWindowText(CString("fps ")+ToStr(round(fps)));
	GetDlgItem(IDC_TRIANGLE_COUNT)->SetWindowText(CString("N = ")+
		ToStr(RBNum(theApp.scene.GetTriangleCount()/2)));
	GetDlgItem(IDC_TRIANGLE_SQUARE)->SetWindowText(CString("S = ")+
		ToStr(RBNum(round(theApp.scene.GetSquareTriangle()/10))));
}
bool COptTree::ActEmitIsInt()
{
	if (GetDocument()&&GetDocument()->m_pActiveEmitter)
		switch(GetDocument()->m_pActiveEmitter->Class())
		{
		case EMC_INTEGRAL:
		case EMC_INTEGRAL_Z:
			return true;
		}
	return false;
}
bool COptTree::NeedGraphSize()			{ return boolV(IX_Size);}
bool COptTree::NeedGraphGravitation()	{ return ActEmitIsInt() && boolV(IX_Gravitation);}
bool COptTree::NeedGraphVelocity()		{ return ActEmitIsInt() && boolV(IX_Velocity);}
bool COptTree::NeedGraphVelMul()		{ return ActEmitIsInt() && boolV(IX_Mul);}
bool COptTree::NeedGraphAngleVel()		{ return boolV(IX_AngleVel);}
bool COptTree::NeedGraphParticlesCount(){ return boolV(IX_ParticlesCount);}
bool COptTree::NeedGraphDeltaVel()		{ return ActEmitIsInt() && boolV(IX_DeltaVel);}
bool COptTree::NeedGraphDeltaSize()		{ return boolV(IX_DeltaSize);}
bool COptTree::NeedGraphDeltaTimeLife()	{ return boolV(IX_DeltaLifeTime);}

void COptTree::OnTimer(UINT nIDEvent)
{
	static int cur_ix=-1;
	static int prev_y=-1;
	static bool changed = false;
	if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		CRect rc;
		CPoint pt;
		GetDlgItem(IDC_CUSTOM1)->GetWindowRect(rc);
		GetCursorPos(&pt);
		if (!rc.PtInRect(pt))
		{
			if (curent_editing!=-1)
				treeList.EndLabelEdit(true);
		}
		if (curent_editing==-1)
		{
			if (prev_y==-1)
			{
				CRect rci;
	//			GetDlgItem(IDC_CUSTOM1)->GetWindowRect(rc);
				CTreeListItem* itsel= treeList.GetSelectedItem();
				if(itsel)
				{
					itsel->GetState();
					treeList.GetItemRect(itsel,1,&rci,false);
					if (pt.x>rc.left+rci.left && pt.x<rc.left+rci.right)
					{

						cur_ix = treeList.GetItemData(itsel);
						if (cur_ix!=IX_Scale&&GetDataItem(cur_ix)->GetStyle()&STI_EDIT)
							prev_y = pt.y;
						else cur_ix = -1;
		/*				pt.y-=rc.top;
						treeList.GetViewRect(&rc);
						treeList.GetV
						pt.y+=rc.top;
						CTreeListItem* it = treeList.GetFirstVisibleItem();
						while(it)
						{
							treeList.GetItemRect(it,1,&rci,false);
							if (pt.y>rci.top && pt.y<rci.bottom)
							{

								cur_ix = treeList.GetItemData(it);
								if (GetDataItem(cur_ix)->GetStyle()&STI_EDIT)
									prev_y = pt.y;
								break;
							}
							it = treeList.GetNextVisibleItem(it);
						}*/
					}
				}
			}
			else if (prev_y!=pt.y)
			{
				changed = true;
				float curv = floatV(cur_ix);
				float nv = floatV(cur_ix)+(prev_y-pt.y)*GetDataItem(cur_ix)->Delta();
				Write(cur_ix,nv);
				SaveControlsData(cur_ix, false);
				UpdateData();
				prev_y = pt.y;
			}
		}
	}else if (prev_y != -1)
	{
		if(cur_ix!=-1&&changed)
		{
			int pos = treeList.GetScrollPos(1);
			SaveControlsData(cur_ix);
			treeList.SetScrollPos(1,pos, false);
		}
		prev_y = -1;
		cur_ix = -1;
		changed = false;
	}

//	__super::OnTimer(nIDEvent);
}
