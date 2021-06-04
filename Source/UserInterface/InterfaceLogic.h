#ifndef __INTERFACE_LOGIC_H__
#define __INTERFACE_LOGIC_H__

#include "UnitAttribute.h"
#include "Region.h"

class terUnitBase;

//////////////////////////////////////////////////////////////////////////////////////////////////
// CShellLogicDispatcher

//states
enum
{
	STATE_DEFAULT         = 0x00000000,
	STATE_EDIT_REGION     = 0x00000001, //редактирование рабочей зоны
	STATE_TRACKING        = 0x00000006  //селект прямоугольником
};

enum
{
	SHELL_PICK_NONE,
	SHELL_PICK_UNIT_TARGET,
	SHELL_PICK_UNIT_ATTACK,
	SHELL_PICK_UNIT_PATROL,
	SHELL_PICK_UNIT_BUILD
};

enum
{
	editRegionNone,
	editRegion1,
	editRegion2
};

enum
{
	GAME_SHELL_SHOW_REGION_MAIN = 0,
	GAME_SHELL_SHOW_REGION_MAIN_ALPHA,
	GAME_SHELL_SHOW_REGION_ABYSS,
	GAME_SHELL_SHOW_ENERGY,
	GAME_SHELL_SHOW_MAX
};

struct CShellLogicDispatcher
{
	typedef int  (*pfnFQH)();

	typedef list<pfnFQH>                  ListQuantResType;

	ListQuantResType m_lstQRes;

	int            m_nState;
	RECT           m_rcTrack;
	RECT           m_rcEnemy;
	int            m_nPickAction;
	int            m_nPickData;
	int            m_nMouseButtonsState;

	float          m_fMouseCurrentX;
	float          m_fMouseCurrentY;
	float          m_fMousePressX;
	float          m_fMousePressY;

	//region
	int             m_nEditRegion;
	bool            m_bToolzerSizeChangeMode;
	bool            m_bCanFlip;

	bool            m_bTolzerFirstClick;

	cExternalObj*		m_pShowExternal[GAME_SHELL_SHOW_MAX];
	void SetEditRegion(int rg);

	cFont*            m_hFontUnitsLabel;

	//камера и сцена для отображения моделей в окошке
	cCamera*          m_hCamera;
	cScene*           m_hScene;
	cUnkLight*       m_hLight;
	cObjectNodeRoot*       m_hModel;
	MatXf				tvModelPosition;
	const AttributeBase* m_pUnitInfo;


	CShellLogicDispatcher();
	~CShellLogicDispatcher();

	void PreDraw(time_type time);
	void PostDraw();
	void draw();
	void quant(bool game_active);
	void init();
	void initFonts();
	void close();

	void SetUnitView(const AttributeBase* pInfo, bool bPowerOn);

	void SetState(int ns);
	int  GetState(){return m_nState;}
	void ResetState();

	void setArrowCursor();

	void RegionEndEdit();
	void updateSmallCamera();

	RegionMetaDispatcher* regionMetaDispatcher() const;

	//mouse events
	int OnMouseMove(float x, float y);
	int OnLButtonDown(float x, float y);
	int OnLButtonUp(float x, float y);
	int OnLButtonDblClk(float x, float y);
	int OnRButtonDown(float x, float y);
	int OnRButtonUp(float x, float y);
	int OnRButtonDblClk(float x, float y);
	//void OnMouseWheel(float x, float y);

	bool TranslatePickAction();

	//units
	void OnInterfaceMessage(int id, bool fromTrigger = true);
	void SoundOnUnitPick(terUnitBase* p);

	int GetSelectedUnitsCount(int TypeID = -1);
	terUnitBase* GetSelectedUnit(int i = 0);


	void AddResourceQuant(pfnFQH pfn);


	void OnOverUnit(terUnitBase* p);
	void OnOverEnemy(terUnitBase* p);
	void OnOverFriend(terUnitBase* p);
	void OnOverFrame();
	void OnMouseIdle();

	bool unitNeedRepairOrBuild(terUnitBase* p);

	bool ShowTerraform() const;

	cFont* getFont() const { return	m_hFontUnitsLabel; }
	void toggleAlwaysShowLifebars() {
		alwaysShowLifeBars = !alwaysShowLifeBars;
	}

private:
	class terRegionColumnMain* pColumnMain;
	bool alwaysShowLifeBars;
};

extern CShellLogicDispatcher*  _pShellDispatcher;

#define _RELEASE(p) if(p) {(p)->Release(); (p) = 0;}


#endif //__INTERFACE_LOGIC_H__