#ifndef OptTree_h
#define OptTree_h
#pragma once

enum
{
IX_NONE				=0,			
IX_ShowGrid			=1,
IX_ShowWorld		=2,
IX_Scale			=3,
IX_Rate				=4,
IX_ShowFigure		=5,
IX_SpriteBlend 		=6,
IX_PosType			=7,
IX_X 				=8,
IX_Y 				=9,
IX_Z 				=10,
IX_AlphaMin 		=11,
IX_AlphaMax 		=12,
IX_ThetaMin 		=13,
IX_ThetaMax 		=14,
IX_UseLight 		=15,
IX_EmitterType	 	=16,
IX_PointsCount 		=17,
IX_PointLoc			=18,
IX_SpRadius1		=19,
IX_SpRadius2 		=20,
IX_SpHeight1 		=21,
IX_SpHeight2 		=22,
IX_SpAngle	  		=23,
IX_SpCompress		=24,
IX_EmitterTimeLife	=25,
IX_ParticlesCount	=26,
IX_Prolonged 		=27,
IX_PlayCycled		=28,
IX_Velocity			=29,
IX_Mul				=30,
IX_VelType			=31,
IX_SplineEnding		=32,
IX_SplTypeDirection =33,
IX_Size				=34,
IX_Gravitation		=35,
IX_AngleVel			=36,
IX_Plume			=37,
IX_PlInterval		=38,
IX_PlTracesCount	=39,
IX_PlTimeScale		=40,
IX_PlSizeScale		=41,
IX_DeltaSize 		=42,
IX_DeltaLifeTime 	=43,	
IX_DeltaVel  		=44,
IX_AngleChaos		=45,
IX_SurAddZ			=46,
IX_SurAngle			=47,
IX_SurCenter		=48,
IX_SurUseForceField	=49,
IX_SurPlanar		=50,
IX_OtherEmiter		=51,
IX_Filling			=52,
IX_Fix_X			=53,
IX_Fix_Y			=54,
IX_Fix_Z			=55,
IX_Fix_Pos			=56,
IX_PlSmooth			=57,
IX_Realtive			=58,
};					










/*
// COptTree form view
#define PARENT_EMMITER			100
	#define COMBO_EMMITER_TYPE  101  
	#define EDIT_NPOINTS		102
	#define EDIT_TIME			103
#define PARENT_GENER			200
	#define EDIT_PARTICLE_COUNT 201
	#define CHECK_PARTICLE_COUNT EDIT_PARTICLE_COUNT
	#define CHECK_PROLONGED		202
	#define CHECK_PLAY_CYCLED	203
#define PARENT_VELOCITY			300

#define CHECK_VELOCITY_MUL 4
#define COMBO_VEL_TYPE 5
#define COMBO_SPLINE_ENDING 6
#define CHECK_SIZE 7
#define CHECK_GRAV 8
#define CHECK_ANG_VEL 9
#define CHECK_ANG_CHAOS 10
#define CHECK_SUR_CENTER 11
#define CHECK_USE_FORCE_FIELD 12
#define CHECK_SUR_PLANAR 13
#define EDIT_VEL_START CHECK_VELOCITY_MUL
#define EDIT_SIZE CHECK_SIZE
#define EDIT_GRAVITATION CHECK_GRAV
#define EDIT_ANG_VEL CHECK_ANG_VEL
#define EDIT_SIZE_DELTA 18
#define EDIT_TIME_DISP 19
#define EDIT_VEL_DISP 20
#define EDIT_SUR_SIZE 21
#define EDIT_SUR_ANGLE 22
#define EDIT_VELOCITY 23
#define CHECK_VELOCITY EDIT_VELOCITY

#define BUTTON_FORALL 26	
#define CHECK_USEL_LIGHT 27	
#define CHECK_SHOW 28
#define COMBO_POS_TYPE 29
#define COMBO_OUT_TYPE 30
#define EMITTER_LIFE_TIME 31
#define EDIT_X 32
#define EDIT_Y 33
#define EDIT_Z 34
#define FPS4 35
#define CHECK_SHOW_GRID 36
#define CHECK_SHOW_WORLD 37
#define EDIT_SCALE 38
#define EDIT_RATE 39
#define CHECK_VIEW_MODE 40
#define EDIT_ALPHA_MIN 41
#define EDIT_ALPHA_MAX 42
#define EDIT_TETA_MIN 43 
#define EDIT_TETA_MAX 44

#define SPL_TYPE_DIRECTION 45

#define CHECK_PLUME 46
#define EDIT_PLUME_INTERVAL 47
//#define EDIT_BEGIN_TIME_PLUME 48
#define EDIT_COUNT_TRACE 49
#define EDIT_TIME_TRACE 50
#define EDIT_SIZE_TRACE 51
#define COMBO_POINTS_LOCATION 52

#define P_L_RADIUS1 53
#define P_L_RADIUS2 54
#define P_L_ANGLE   55
#define P_L_COMPRESS 56
#define P_L_HEIGHT1 57
#define P_L_HEIGHT2 58
*/

#include "TreeList/TreeListCtrl.h"
#include "TreeList/Null Listeners/NullChangeItemLabelNotifyListener.h"
#include "TreeList/Null Listeners/NullItemDragdropNotifyListener.h"
#include "TreeList/Null Listeners/NullItemChangeNotifyListener.h"
#include "TreeList/Null Listeners/NullItemOperationNotifyListener.h"
#include "TreeList/Null Listeners/NullTreeListNotifyListener.h"
#include "TreeList/Null Listeners/NullGeneralNotifyListener.h"
#include "ControlView.h"

enum StyleTreeItem
{
	STI_TITLE = 0,
	STI_EDIT = 1,
	STI_CHECKBOX = 2,
	STI_CHECKGRAPH = 4,
	STI_GRAPH = STI_CHECKGRAPH | STI_EDIT,
	STI_COMBO = 8,
};
extern const float fl_max;
class CDataItem
{
	struct ComboStr
	{
		CString str;
		bool Visible;
	};
	DWORD index;
	DWORD style;
	bool check;
	float value;
	std::vector<ComboStr> combo_stings;
	//обработка
	bool is_int;
	float min_val;
	float max_val;
public:
	CDataItem(){value = 0;check = false;};
	void Init(DWORD ix, DWORD style, float min =-fl_max, float max =fl_max, bool isint = false, CString combo_text = "");
	DWORD GetIndex(){ return index;}
	DWORD GetStyle();
	float GetValue();
	float GetRealyValue();
	void SetValue(float nv);
	void SetStrValue(CString& nv);
	void SetRealyValue(float nv);
	bool GetCheck();
	void SetCheck(bool nch);
	const CString& GetComboStr(int ix);
	bool GetComboStrVisible(int ix);
	void SetComboStrVisible(int i, bool nb);
	void SetComboStr(CString s);
	int GetComboStrCount();
	CString GetStrValue();
	float Delta();
};

class COptTree : public CFormView
						, protected NullChangeItemLabelNotifyListener
						, protected NullItemDragdropNotifyListener
						, protected NullItemChangeNotifyListener
						, protected NullItemOperationNotifyListener
						, protected NullTreeListNotifyListener
						, protected NullGeneralNotifyListener

{
	DECLARE_DYNCREATE(COptTree)

protected:
	COptTree();           // protected constructor used by dynamic creation
//	virtual ~COptTree();
	CTreeListCtrl treeList;
	CImageList ImageList;
	std::vector<CDataItem> items;
	int curent_editing;
//	int cur_ix;
//	CWnd* EditCtrl;
//	CRect EditRect;
//	int prev_y;
	bool B[2];
	CDataItem* GetDataItem(int ix);
	void COptTree::Write(int ix, float* p_nv);
	void Write(int ix, float nv);
	float floatV(int ix);
	int intV(int ix);
	bool boolV(int ix); 
	void AddItem(DWORD ix, DWORD style, float min_val = -fl_max, float max_val =fl_max, int isint = false);
	void AddItem(DWORD ix, DWORD style, CString cs);
public:
	CToolBar    m_wndToolBar;
	bool ined;
	enum { IDD = IDD_OPTTREE };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
public:
/*	bool m_ViewMode;
	float X,Y,Z;
	int OutType;
	int PosType;
	float Scale,Rate;
	bool m_chPlume;
	float m_fPlumeInterval;
	float m_TraceCount;
	float m_fPlumeTimeScaling;
	float m_fPlumeSizeScaling;

	int m_pt_location;
	float m_pl_radius1;
	float m_pl_radius2;
	float m_pl_angle; 
	float m_pl_compres;
	float m_pl_height1;
	float m_pl_height2;
*/
protected:
	float SetParamSpiral(CKeyPos& kp);
	void Spiral(bool set_par = false);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	virtual DWORD onRequestCtrl(CTreeListCtrl& source,CHANGE_LABEL_NOTIFY_INFO* info);
	virtual bool onBeginControl(CTreeListCtrl& source,CHANGE_LABEL_NOTIFY_INFO* info);
	virtual void onEndControl(CTreeListCtrl& source,CHANGE_LABEL_NOTIFY_INFO* info);
	virtual bool onBeginLabelEdit(CTreeListCtrl& source,CHANGE_LABEL_NOTIFY_INFO* info);
	virtual bool onEndLabelEdit(CTreeListCtrl& source,CHANGE_LABEL_NOTIFY_INFO* info);
	virtual void onClick(CTreeListCtrl& tree, GENERAL_NOTIFY_INFO* info);
	void onItemCheckChanged(CTreeListCtrl& tree, ITEM_CHANGE_INFO* info);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	CTreeListItem* InsertItem(LPSTR cap,DWORD dat,CTreeListItem* item);
	void InitTreeList(void);
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	bool SetItemText(DWORD id,CString s);
	bool SetItemCheck(DWORD id,bool v);
	void SetFps(float fps);
	void UpDateToolBar();
	void PutButton(int ix);
	void ShowOptEmiter();
	void UpdateData();
	void SetControlsData(bool show = true, bool update = false);
	void SaveControlsData(DWORD ix  =IX_NONE,bool show = true);
	void CheckRangeEmitter();
	void ChangeEmitterName(CString old_name, CString new_name);
protected:
	CEffectToolDoc* GetDocument(){return (CEffectToolDoc*)CFormView::GetDocument();}
	CTreeListItem * FindItem(DWORD id,CTreeListItem* item = NULL);
public:
	bool ActEmitIsInt();
	bool NeedGraphSize();
	bool NeedGraphAngleVel();
	bool NeedGraphGravitation();
	bool NeedGraphVelMul();
	bool NeedGraphVelocity();
	bool NeedGraphParticlesCount();
	bool NeedGraphDeltaVel();
	bool NeedGraphDeltaSize();
	bool NeedGraphDeltaTimeLife();
	bool EnableEditPos(){return intV(IX_PointLoc)==0;}
	afx_msg void OnTimer(UINT nIDEvent);
};



#endif //define OptTree_h