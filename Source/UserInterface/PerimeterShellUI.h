
#pragma once
#ifndef _PERIMETERSHELLUI_H
#define _PERIMETERSHELLUI_H

#include "tweaks.h"
#include "GameShellSq.h"
#include "AudioPlayer.h"
#include "Universe.h"
#include "qd_textdb.h"
#include "SourceUIResolution.h"

enum
{
	effectButtonsFadeIn = 0x0001,
	effectButtonsFadeOut= 0x0002,	
	effectCtrlRollIn    = 0x0004,
	effectCtrlRollOut   = 0x0008,
	effectSplashFadeIn  = 0x0010,
	effectSplashFadeOut = 0x0020,	
};

class CShellWindow;
struct LocalizedText;

typedef void(*EVENTPROC)(CShellWindow* pWnd, InterfaceEventCode code, int param);
typedef int(*DYNCALLBACK)(float,float);

//картикки в на закладках папок
struct SQUAD_ICON
{
	cTexture*	texture;
	cTexture*	highlightTexture;
	cTexture*	checkedTexture;
	cTexture*	disabledTexture;
	TabIcon*  attr;

	Vect2f	uv;
	Vect2f	dudv;
	Vect2f	highUV;
	Vect2f	highDUDV;
	Vect2f	checkedUV;
	Vect2f	checkedDUDV;
	Vect2f	disabledUV;
	Vect2f	disabledDUDV;


	SQUAD_ICON(TabIcon* a);
	~SQUAD_ICON();
	void init();
};


//базовый класс для всек контролов
class CShellWindow
{
protected:
	EVENTPROC		m_handler;
	cTexture*		m_hTexture;
	Vect2f			m_vTexPos[2];
	sColor4c        m_cColor;
	sColor4c        m_cBGColor;
	sColor4c        markColor;
	float			m_ftime;
	int             m_effect;

	int flashR;
	int flashG;
	int flashB;
	int flashA;

	int flashBG_R;
	int flashBG_G;
	int flashBG_B;
	int flashBG_A;

	float tickTime;

	std::string hotKeyPopupString;

//	float flashInterval;
	float flashTimer;
	bool checkHotKeyVisibility;

public:
	cFont*       m_hFont;
	const char*        m_sound;
	const sqshControl* m_attr;
	const sqshControlContainer* m_attr_cont;
    SHELL_ANCHOR anchor;
    SHELL_ANCHOR anchor_children;

	float x, y;
	float sx, sy;

	int    ID;
	int    state;

	std::list<CShellWindow*> m_children;
	CShellWindow*       m_pParent;

	CShellWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	virtual ~CShellWindow();

	virtual bool actionPerformed(InterfaceEventCode code, int param);
	void SetTexPos(const Vect2f& v1, const Vect2f& v2);
	void setFlashingInterval(float interval = -1);
	virtual bool updateColor();
	void createHotKeyString();
	virtual void loadFlashingParams(const sqshControl* attr) {
		flashBG_R = attr->flashBG_R;
		flashBG_G = attr->flashBG_G;
		flashBG_B = attr->flashBG_B;
		flashBG_A = attr->flashBG_A;
		flashR = attr->flashR;
		flashG = attr->flashG;
		flashB = attr->flashB;
		flashA = attr->flashA;
		tickTime = attr->tickTime;
	}
	void setTickTime(float newTickTime) {
		tickTime = newTickTime;
	}
	void setFlashBGColor(const sColor4c& newColor) {
		flashBG_R = newColor.r;
		flashBG_G = newColor.g;
		flashBG_B = newColor.b;
		flashBG_A = newColor.a;
	}
	void setFlashColor(const sColor4c& newColor) {
		flashR = newColor.r;
		flashG = newColor.g;
		flashB = newColor.b;
		flashA = newColor.a;
	}

	int IsEffect(){ return m_effect != 0;}

	bool isContainer() const {
		return m_attr_cont;
	}

	bool isEditWindow() const { return (!isContainer() && m_attr && ( m_attr->type == SQSH_EDIT_BOX_TYPE || m_attr->type == SQSH_INGAME_CHAT_EDIT_BOX_TYPE)); }

	bool checkDefaultEnterBtns();
	bool checkDefaultEscBtn();

	virtual void postLoad() {
		checkHotKeyVisibility = false;
	}

	static bool checkDefaultBtn(CShellWindow* btn) {
		if (btn && btn->isEnabled() && btn->isVisible()) {
			btn->m_handler( btn, EVENT_UNPRESSED, 0 );
			return true;
		} else {
			return false;
		}
	}

	virtual void LoadMenuWnd(const sqshControlContainer* attr);
	virtual void Load(const sqshControl* attr);
    virtual void loadAnchor();
	virtual void reload();
	virtual int  HitTest(float _x, float _y);
	virtual void OnMouseMove(float _x, float _y);
	virtual void OnLButtonDown(float _x, float _y);
	virtual void OnLButtonUp(float _x, float _y);
	virtual void OnLButtonDblClk(float _x, float _y);
	virtual void OnLButtonHold();
	virtual void OnRButtonDown(float _x, float _y);
	virtual void OnRButtonUp(float _x, float _y);
	virtual void OnChar(char key){}
	virtual bool OnKeyDown(int key);
	virtual bool OnKeyUp(int key);
	virtual void OnMouseWheel(int delta);

	virtual void Move(float _x, float _y, float _sx, float _sy);
	virtual void scale(Vect2f& scaleV);
	virtual void Show(int bShow);
	virtual void Show(int bShow, int effect);
	virtual void ClearEffect();
	virtual void Enable(int bEnable);
	bool isVisible() {
		return (state & SQSH_VISIBLE);
	}
	bool isEnabled() {
		return (state & SQSH_ENABLED);
	}
	float getFlashingInterval() {
//		return flashInterval;
		return flashTimer;
	}
	bool isFlashing() {
		return (flashTimer >= 0);
	}
	virtual void OnWindow(int enable);
	virtual void OnFormatPopupMessage(char* cbBuffer);
	virtual void OnFormatHotKeyPopupMessage(char* cbBuffer);

	virtual void draw(int bFocus);

	virtual int  EffectSupported(){return 0;}
	virtual void OnEffectStart(int effect){ m_effect = effect;}
	bool OnEffectStop(int effect) {
		m_effect = 0;
/*
		if (effect == effectButtonsFadeOut || effect == effectCtrlRollOut) {
			Show(0);
			return false;
		}
		return true;
*/
		switch (effect) {
			case effectButtonsFadeOut:
			case effectCtrlRollOut:
				Show(0);
				return false;
			default:
				Show(1);
				return true;
		}
	}
	virtual bool canHandleDisabledInput() const {
		return m_attr && m_attr->canHandleDisabledInput;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////
//простая кнопка главного меню
class CShellPushButton : public CShellWindow
{
public:
	int				flag;
	int				target;
	float			txtdy;
	std::string		labelText;
	Vect2f			uv;
	Vect2f			dudv;
	enum
	{
		unpressed = 0,
		pressed	=1<<0,
		active	=1<<1,
	};

	CShellPushButton(int id, CShellWindow* pParent, EVENTPROC p = 0);
	virtual ~CShellPushButton();

	virtual void Load(const sqshControl* attr);
	virtual void OnLButtonDown(float _x, float _y);
	virtual void OnLButtonUp(float _x, float _y);
	virtual void OnRButtonDown(float _x, float _y);
	virtual void OnRButtonUp(float _x, float _y);
	virtual void OnWindow(int enable);

	virtual void draw(int bFocus);

	virtual int  EffectSupported(){
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
	void setText(const std::string& newLabelText) {
		labelText = newLabelText;
	}
};

//////////////////////////
class CReplayPlayerPushButton : public CShellPushButton
{
public:

	CReplayPlayerPushButton(int id, CShellWindow* pParent, EVENTPROC p = 0);
	virtual ~CReplayPlayerPushButton();

	void Load(const sqshControl* attr) override;
	void draw(int bFocus) override;
	void setPlayer(const std::string& newName, const sColor4f& newColor) {
		labelText = newName;
		color = newColor;
	}

protected:
	sColor4f color;
	cTexture* m_hPopupTexture;
};

//////////////////////////////////////////////////////////////////////////////////////////////
//основная кнопка интерфейса. используется как сама, так и как базовый класс для других
class CShellComplexPushButton : public CShellWindow
{
protected:
	cTexture*	m_hTextureHighlight; 
	cTexture*	m_hTexturePressed; 
	cTexture*	m_hTextureDisabled; 
	const sqshImage* m_pTextureHAttr;
	const sqshImage* m_pCheckAttr;
	const sqshImage* m_pDisabledAttr;

	cTexture*	m_hTexture2; 
	cTexture*	m_hTexture2Highlight; 
	cTexture*	m_hTexture2Pressed; 
	cTexture*	m_hTexture2Disabled; 
	const sqshImage* m_pTexture2Attr;
	const sqshImage* m_pTexture2HAttr;
	const sqshImage* m_pCheck2Attr;
	const sqshImage* m_pDisabled2Attr;

	Vect2f			m_vTexHighPos;
	Vect2f			m_vTexCheckPos;
	Vect2f			m_vTexDisabledPos;
	
	Vect2f			m_vTex2Pos;
	Vect2f			m_vTex2HighPos;
	Vect2f			m_vTex2CheckPos;
	Vect2f			m_vTex2DisabledPos;

	cFont*      m_hFontLabel;
	sColor4f    m_ColorLabel;
	float       m_label_x; //TODO seem to be unused
	float		m_label_y;
	enum
	{
		pressed	= 1,
		active	= 2,
	};
	int flag;
	char		m_cb[5];

	float		m_fphase;
	bool        m_bChecked;

	bool        m_bStatusNoEnergy;
	bool        m_bPaused;

public:

	CShellComplexPushButton(int id, CShellWindow* pParent, EVENTPROC p = 0);
	virtual ~CShellComplexPushButton();

	virtual void Load(const sqshControl* attr);
	virtual void OnLButtonDown(float _x, float _y);
	virtual void OnLButtonUp(float _x, float _y);
	virtual void OnWindow(int enable);
	virtual void Enable(int bEnable);
	virtual void draw(int bFocus);
	virtual void OnFormatPopupMessage(char* cbBuffer);
	virtual void scale(Vect2f& scaleV);

	void SetCheck(bool b);
	bool GetCheck(){return m_bChecked;}
	void SetValue(int val, bool bCanBuildNow);
	void SetProgress(float v);

	void SetStatusNoEnergy(bool bSet){
		m_bStatusNoEnergy = bSet;
	}
	bool GetStatusNoEnergy(){
		return m_bStatusNoEnergy;
	}
	float GetPhase(){
		return m_fphase;
	}

	virtual void Pause(bool bOn);
	virtual bool GetPause();
	virtual bool CanPause();

};

//////////////////////////////////////////////////////////////////////////////////////////////
//кнопка производных юнитов
class CShellLegionButton : public CShellComplexPushButton
{
	cFont*      m_hFontLabel;
	char		m_cbTotal[5];
public:

	bool        m_bS, m_bO, m_bT; //хватает ли базовых

	CShellLegionButton(int id, CShellWindow* pParent, EVENTPROC p = 0);
	virtual ~CShellLegionButton();

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);

	void SetTotalUnitCount(int count, bool append);
	void SetAtomEnable(bool bS, bool bO, bool bT){
		m_bS = bS; m_bO = bO; m_bT = bT;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////
//кнопка заказа базовых юнитов и структур
class CShellAtomButton : public CShellComplexPushButton
{
//	cFont*     m_hFont;
	bool       m_bTextLeft;

	float button_atom_x;
	float button_atom_y;

	char       m_cb1[12];
public:
	CShellAtomButton(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CShellAtomButton();

	virtual void draw(int bFocus);
	virtual void Load(const sqshControl* attr);

	void SetVal(int v, bool bTextLeft = false);
};

//////////////////////////////////////////////////////////////////////////////////////////////
//кнопка заказа бригадиров/прорабов
class CTerrainBuildButton : public CShellComplexPushButton
{
	bool  m_bPointingLeft;

	float half_sx;
	float right_x;
	
	Vect2f activeHalfBrig_uv;
	Vect2f activeHalfTerr_uv;

	Vect2f flashingHalfBrig_uv;
	Vect2f flashingHalfTerr_uv;

	Vect2f halfBrig_uv;
	Vect2f halfTerr_uv;

	Vect2f half_dudv;

	Vect2f brig_uv;
	Vect2f terr_uv;

	Vect2f pressedBrig_uv;
	Vect2f pressedTerr_uv;

	Vect2f activeBrig_uv;
	Vect2f activeTerr_uv;

	Vect2f whole_dudv;

	bool unitReady;
	float productionPhase;
	bool isBrig;

public:
	CTerrainBuildButton(int id, CShellWindow* pParent, EVENTPROC p);
	virtual ~CTerrainBuildButton();

	virtual void draw(int bFocus);
	virtual int  HitTest(float _x, float _y);
	virtual void OnFormatPopupMessage(char* cbBuffer);
	virtual void Load(const sqshControl* attr);
	void setParams(bool unitReady, float productionPhase, bool isBrig) {
		CTerrainBuildButton::unitReady = unitReady;
		CTerrainBuildButton::productionPhase = productionPhase;
		CTerrainBuildButton::isBrig = isBrig;
	}
	int partDisable;
};

//////////////////////////////////////////////////////////////////////////////////////////////
//tabsheets сквадов и строений
class CUITabSheet : public CShellWindow
{
	const sqshTabSheet*           m_tabattr;

	std::vector<const sqshTabElement*> tabAttrs;
	std::vector<cTexture*>		tabTextures;
	std::vector<cTexture*>		tabHighlightTextures;
	std::vector<cTexture*>		tabCheckedTextures;
	std::vector<cTexture*>		tabDisabledTextures;
	std::vector<cTexture*>		tabFlashingTextures;
	std::vector<Vect2f>			tabUVs;
	std::vector<Vect2f>			tabDUDVs;
	std::vector<Vect2f>			tabHighUVs;
	std::vector<Vect2f>			tabHighDUDVs;
	std::vector<Vect2f>			tabCheckedUVs;
	std::vector<Vect2f>			tabCheckedDUDVs;
	std::vector<Vect2f>			tabDisabledUVs;
	std::vector<Vect2f>			tabDisabledDUDVs;
	std::vector<Vect2f>			tabFlashingUVs;
	std::vector<Vect2f>			tabFlashingDUDVs;

	std::vector<int>			tabXs;
	std::vector<int>			tabYs;
	std::vector<int>			tabSXs;
	std::vector<int>			tabSYs;

	std::vector<cTexture*>		tabActiveTextures;
	std::vector<Vect2f>			tabActiveUVs;
	std::vector<Vect2f>			tabActiveDUDVs;
	std::vector<int>			tabActiveXs;
	std::vector<int>			tabActiveYs;
	std::vector<int>			tabActiveSXs;
	std::vector<int>			tabActiveSYs;

	int x;
	int y;
	int sx;
	int sy;

	int dx;
	int dy;
	int ddsx;
	int ddsy;

	std::vector<SQUAD_ICON*>     m_squad_icon_list;
	std::vector<std::string>     m_page_numbers;

//	cFont*                  m_hFont;

	float		m_fphase;

	int   m_nTabHover;
	bool  m_bLabelHover;
	uint32_t m_nVisiblePagesBits;
	uint32_t m_nEnabledPagesBits;
	uint32_t m_nSelectedPagesBits;
	std::vector<float> flashTimers;
	int   m_nActivePage;

	int HitTestTabHeader(float _x, float _y);
	virtual void loadFlashingParams(const sqshTabSheet* attr) {
		flashBG_R = attr->flashBG_R;
		flashBG_G = attr->flashBG_G;
		flashBG_B = attr->flashBG_B;
		flashBG_A = attr->flashBG_A;
		flashR = attr->flashR;
		flashG = attr->flashG;
		flashB = attr->flashB;
		flashA = attr->flashA;
		tickTime = attr->tickTime;
	}


public:
	CUITabSheet(int id, CShellWindow* pParent, EVENTPROC p);
	~CUITabSheet();

	void Load(const sqshTabSheet* attr);
	void reload();
	void EnablePage(int nPage, bool bEnable);
	void setFlashingPageInterval(int nPage, float interval = -1);
	float getFlashingPageInterval(int nPage) {
		return flashTimers[nPage];
	}
	bool isPageFlashing(int nPage) {
		return (flashTimers[nPage] >= 0);
	}
	void ShowPage(int nPage, bool bShow);
	void SelectPage(int nPage, bool bSelect);
	void SwitchPage(int nNewPage, bool bForceSelectUnit = true);
	void SetSquadIcons(int n, SQUAD_ICON* pSqIcon);
	void SetPageNumber(int nPage, int nNumber);
	bool actionPerformed(InterfaceEventCode code, int param);
	void createHotKeyString();
	bool updateColor();

	void postLoad();

	int  GetActivePage(){
		return m_nActivePage;
	}
	bool IsPageSelected(int nPage){
		return m_nSelectedPagesBits & (1 << nPage);
	}
	int getActivePageNumber() {
		return m_nActivePage;
	}

	virtual int  HitTest(float _x, float _y);
	virtual void OnLButtonDown(float _x, float _y);
	virtual void OnLButtonDblClk(float _x, float _y);
	virtual void OnRButtonDown(float _x, float _y);
	virtual void draw(int bFocus);
	virtual void scale(Vect2f& scaleV);
	virtual void OnWindow(int enable);

	void setActivePageChildrenVisible(bool visible);
};

//////////////////////////////////////////////////////////////////////////////////////////////
//окно с мультитекстурингом - экраны главного меню
class CMultiTexWindow : public CShellWindow
{
public:
	cTexture *m_hTexture2;
	cTexture *m_hTexture3;

	CMultiTexWindow(int id, CShellWindow* pParent, EVENTPROC p) :
		CShellWindow(id, pParent, p){ m_hTexture2=0; m_hTexture3=0; }
	virtual ~CMultiTexWindow();
	virtual void Load(const sqshControl* attr);
	virtual void LoadMenuWnd(const sqshControlContainer* attr);
	virtual void init();
	virtual void draw(int bFocus);
	virtual int  EffectSupported() { return m_hTexture2 ? (effectCtrlRollIn|effectCtrlRollOut) : 0; }
};

//////////////////////////////////////////////////////////////////////////////////////////////
//окошко с картой мира в главном меню
class CShowMapWindow : public CShellWindow
{
public:
	CShowMapWindow(int id, CShellWindow* pParent, EVENTPROC p);
	~CShowMapWindow();

	cTexture*		m_hTextureBG; 
	Vect2f			m_vTexBGPos;
	Vect2f			m_vTexBGSize;

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);
	void setWorldID(int id);
	virtual int  EffectSupported(){
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
protected:
	int worldID;
    float mx, my;
    float msx, msy;
};

//////////////////////////////////////////////////////////////////////////////////////////////
//используется как рамка для кнопок в главном меню
class CMoveButton : public CShellWindow
{
public:
	static int snd;
	float xstart,ystart;

	CMoveButton(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) { }
	
	virtual int  EffectSupported() { return effectCtrlRollIn|effectCtrlRollOut; }
//	virtual int  EffectSupported() { return effectButtonsFadeIn|effectButtonsFadeOut; }
	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);
};



//////////////////////////////////////////////////////////////////////////////////////////////
//контролы главного меню
class CScaleButton : public CShellWindow
{
public:
	static int	snd1,snd2;
	float size;
	cTexture *m_hTexture2;
	cTexture *m_hTexture3;

	cTexture*		m_hTextureBG; 
	Vect2f			m_vTexBGPos;
	Vect2f			m_vTexBGSize;

	bool soundEnabled;

	CScaleButton(int id, CShellWindow* pParent, EVENTPROC p);
	~CScaleButton();
	virtual int  EffectSupported()	{ return effectButtonsFadeIn|effectButtonsFadeOut; }
	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);
	void setTexture(const char* name);
};

class CPortraitWindow : public CShellWindow
{
public:
	CPortraitWindow(int id, CShellWindow* pParent, EVENTPROC p);
	~CPortraitWindow();

	cTexture*		m_hTextureBG; 
	Vect2f			m_vTexBGPos;
	Vect2f			m_vTexBGSize;

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);
	void setTexture(const char* name);
	virtual int  EffectSupported(){
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
};

class CLogoWindow : public CShellWindow
{
public:
	CLogoWindow(int id, CShellWindow* pParent, EVENTPROC p);
	~CLogoWindow();
	Vect2f			m_vTexPosRace[2][3];

	cTexture*		m_hTextureBG; 
	Vect2f			m_vTexBGPos;
	Vect2f			m_vTexBGSize;

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);
	virtual int  EffectSupported(){
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
	void setRace(int newRace) {
		race = newRace;
	}
	int getRace() const {
		return race;
	}
protected:
	int race;
};

class CPushButton : public CShellPushButton
{
public:
	CPushButton(int id, CShellWindow* pParent, EVENTPROC p) : CShellPushButton(id, pParent, p) { }
	virtual int  EffectSupported()	{return 0;}
};

class CPushScaleButton : public CShellPushButton
{
public:
	CPushScaleButton(int id, CShellWindow* pParent, EVENTPROC p) : CShellPushButton(id, pParent, p) { }
	virtual int  EffectSupported() { return effectButtonsFadeIn|effectButtonsFadeOut; }
	virtual void draw(int bFocus);
};

class CWorldEffect : public CMultiTexWindow
{
public:
	CWorldEffect(int id, CShellWindow* pParent, EVENTPROC p) : CMultiTexWindow(id, pParent, p) { }
	virtual int  EffectSupported() { return effectButtonsFadeIn|effectButtonsFadeOut; }
	virtual void draw(int bFocus);
};

class CDialogWindow : public CShellWindow
{
public:
	CDialogWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) { }
	virtual void draw(int bFocus);
};

//////////////////////////////////////////////////////////////////////////////////////////////
//окно с картой в игре
class CMapWindow : public CShellWindow
{
	cTexture*	   m_pMapTexture;
//	cFont*         m_hFont;
	sColor4c*      m_bitmap;

	char           m_cb1[15];
	char           m_cb2[15];
	char           m_cb3[15];

	cTexture*		activeObjectTx;
	float			activeObjectSx;
	float			activeObjectSy;

	cTexture*		eventTx[EVENT_MINI_MAP_MAX];
	float			eventSx[EVENT_MINI_MAP_MAX];
	float			eventSy[EVENT_MINI_MAP_MAX];

	float			map_xs;
	float			map_ys;

public:

	CMapWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CMapWindow();

	void drawBitmap(sColor4c* bitmap);

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);
};

//////////////////////////////////////////////////////////////////////////////////////////////
//просто текст.
class CTextWindow : public CShellWindow
{

public:
	CTextWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CTextWindow();

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);

	void SetText(const char* text) {
		textData = text;
	}
	void setText(const std::string& text) {
		textData = text;
	}
	const std::string& getText() const {
		return textData;
	}
	virtual int EffectSupported() {
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}

	bool victory;
	int colorIndex;

protected:
	std::string textData;
	Vect2f			uv;
	Vect2f			dudv;

};

class CTextStringWindow : public CTextWindow
{

public:
	CTextStringWindow(int id, CShellWindow* pParent, EVENTPROC p = 0) : CTextWindow(id, pParent, p) {
	};

	void draw(int bFocus) override;
};

//////////////////////////////////////////////////////////
class CCreditsWindow : public CShellWindow
{
	std::string textData;
	Vect2f			uv;
	Vect2f			dudv;
	float	timer;
	float	maxTime;

public:
	CCreditsWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CCreditsWindow();

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);
	virtual void Show(int bShow);

	virtual int EffectSupported() {
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////
//listbox главного меню
struct sItem
{
	float x;
	std::deque<std::string> m_data;
	int id;
};

class CListBoxWindow : public CShellWindow
{
//	cFont*          m_hFont;
	sItem			*m_pItem;
	int				m_nNumberItem;
	int				m_nCurSel;
	int				m_bScroller;
	int				m_nTopItem;
	float			m_fStringHeight;
	float			m_fScrollerThumbPos;
	float			txtdx;
	float			txtdy;
	cTexture*		thumbTexture;
	Vect2f			thumbUV;
	Vect2f			thumbDUDV;
	float			vScrollSX;
	float			vScrollSY;
	float			vScrollThmbSX;
	float			vScrollThmbSY;

	cTexture*		m_hTextureBG;
	Vect2f			m_vTexBGPos;
	Vect2f			m_vTexBGSize;
	Vect2f			uv;
	Vect2f			dudv;

	int currentScrollDirection;

	enum
	{
		thumb_none,
		thumb_up,
		thumb_dn
	};

	int CheckClick(float _x,float  _y);
	int ItemFromPoint(float  _y);
public:
	CListBoxWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CListBoxWindow();

	void updateScroller();

	void Clear();
	void AddString(const char* cb,int nrow=0);
	int  GetItemCount(int nrow=0){
		return m_pItem[nrow].m_data.size();
	}
	int  GetCurSel(){
		return m_nCurSel;
	}

	void SetCurSel(int n);
	void SetCurSelPassive(int n);

	const char* GetCurSelString();

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);

	virtual void Move(float _x, float _y, float _sx, float _sy);
	virtual void OnLButtonDown(float _x, float _y);
	virtual void OnLButtonDblClk(float _x, float _y);
	virtual void OnMouseWheel(int delta);
	
	void NewItem(int nrow=0);
	sItem& GetItem(int nrow)	{ return m_pItem[nrow]; }
	virtual int EffectSupported() {
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
};

////////////////////

class ChatWindow : public CShellWindow {
    
protected:
    bool            scroll_left;
	std::vector<LocalizedText>	m_data;
	int				m_bScroller;
	int				m_nTopItem;
	float			m_fStringHeight;
	float			m_fScrollerThumbPos;
	float			txtdx;
	float			txtdy;
	cTexture*		thumbTexture;
	Vect2f			thumbUV;
	Vect2f			thumbDUDV;
	float			vScrollSX;
	float			vScrollSY;
	float			vScrollThmbSX;
	float			vScrollThmbSY;

	cTexture*		m_hTextureBG;
	Vect2f			m_vTexBGPos;
	Vect2f			m_vTexBGSize;
	Vect2f			uv;
	Vect2f			dudv;

	int currentScrollDirection;

    cFont*       m_hFont1250;
    cFont*       m_hFont1251;

	enum
	{
		thumb_none,
		thumb_up,
		thumb_dn
	};

    virtual int CheckClick(float _x,float  _y);
	/// возвращает длину части строки, которая влезет в окно по ширине
	/// если строка войдёт целиком, то возвращает -1
	int GetStringBreak(const std::string& str, bool ignore_spaces = false) const;
    
    void freeLocaleFonts();
    void setupLocaleFonts(int size);

public:
	ChatWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~ChatWindow();

	void updateScroller();

	void Clear();
	void AddString(const LocalizedText* text);

	int GetRowCount(){
		return m_data.size();
	}
    
    void drawText(float Alpha);

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);

    void OnMouseWheel(int delta);
	virtual void OnLButtonDown(float _x, float _y);
	virtual void OnLButtonDblClk(float _x, float _y);
	
	virtual int EffectSupported() {
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
};

////////////////////

class CStatListBoxWindow : public CShellWindow {

	sItem			*m_pItem;
	int				m_nNumberItem;
	float			m_fStringHeight;
	float			txtdx;
	float			txtdy;

	Vect2f			m_vTexPosRace[2][3];

	std::deque<int> races;
	std::deque<sColor4c> colors;

	cTexture*		m_hTextureBG;
	Vect2f			m_vTexBGPos;
	Vect2f			m_vTexBGSize;

public:
	CStatListBoxWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CStatListBoxWindow();

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);

	void Clear();
	void AddString(const char* cb,int nrow=0);
	void AddRace(int race, const sColor4c& color);
	void NewItem(int nrow = 0);
	sItem& GetItem(int nrow)	{ return m_pItem[nrow]; }

	virtual int EffectSupported() {
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////
//slider для главного меню
class CSliderWindow : public CShellWindow
{
public:
	int				bPress;
	float			pos;
	int				target;

	float			arrowSize;
	float			thumbSize;

	Vect2f			leftArrowUV;
	Vect2f			leftArrowDUDV;
	Vect2f			rightArrowUV;
	Vect2f			rightArrowDUDV;
	Vect2f			thumbUV;
	Vect2f			thumbDUDV;

	cTexture*		m_hTextureBG; 
	Vect2f			m_vTexBGPos;
	Vect2f			m_vTexBGSize;

	CSliderWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CSliderWindow();
	virtual void draw(int bFocus);
	virtual int  HitTest(float _x, float _y);
    virtual void OnLButtonDown(float _x, float _y);
	virtual void Load(const sqshControl* attr);
	virtual void OnWindow(int enable);
	virtual int EffectSupported() {
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////
//edit  для главного меню
class CEditWindow : public CShellWindow
{
//	cFont*   m_hFont;

public:
	CEditWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CEditWindow();
	float			txtdy;
	Vect2f			uv;
	Vect2f			dudv;

	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);
	virtual void OnChar(char key);

	virtual int  EffectSupported(){
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
	void SetText(const char* lpszText);
	const char* GetText(){
		return m_data.c_str();
	}
	const std::string& getText() const {
		return m_data;
	}
    bool isEmptyText() const;
    bool isValidFilename() const;
protected:
	std::string   m_data;

};

class CChatInGameEditWindow : public CEditWindow
{
	cTexture* m_hPopupTexture;

public:
	CChatInGameEditWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CChatInGameEditWindow();

	void Load(const sqshControl* attr) override;

	void draw(int bFocus) override;
	void OnChar(char key) override;

	bool alliesOnlyMode;

    static const char* getModePostfix(bool clanOnly) {
		return qdTextDB::instance().getText(clanOnly ? "Interface.Tips.ToClanPostfix" : "Interface.Tips.ToAllPostfix");
	}

protected:
	const char* getModePrefix() const {
		return qdTextDB::instance().getText(alliesOnlyMode ? "Interface.Tips.ToClanPrefix" : "Interface.Tips.ToAllPrefix");
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////
//combo  для главного меню
class CComboWindow : public CShellWindow
{
public:
//	cFont*      m_hFont;
	int 		pos;
    size_t      size;
	std::vector<std::string> Array;
	float		m_fStringHeight;
	int			target;
	Vect2f			uv;
	Vect2f			dudv;

	CComboWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CComboWindow();
	virtual void draw(int bFocus);
	virtual void OnLButtonUp(float _x, float _y);
	virtual void OnRButtonUp(float _x, float _y);
	virtual void Load(const sqshControl* attr);
	virtual void OnWindow(int enable);
	virtual int EffectSupported() {
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
};

class CColorComboWindow : public CShellWindow
{
public:
	int			pos,size;
	sColor4f*	colors;
	int			target;

	CColorComboWindow(int id, CShellWindow* pParent, EVENTPROC p = 0);
	~CColorComboWindow();
	virtual void draw(int bFocus);
	virtual void OnLButtonUp(float _x, float _y);
	virtual void OnRButtonUp(float _x, float _y);
	virtual void Load(const sqshControl* attr);
	virtual void OnWindow(int enable);
	virtual int EffectSupported() {
		return effectButtonsFadeIn|effectButtonsFadeOut;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////
//splash screen
class CSplashWindow : public CShellWindow
{
public:
	int   m_alpha;

	CSplashWindow(int id, CShellWindow* pParent, EVENTPROC p);

	virtual void LoadMenuWnd(const sqshControlContainer* attr);
	virtual void Show(int bShow);
	virtual void draw(int bFocus);
	virtual bool OnKeyDown(int key) {
		if(m_handler && (state & SQSH_VISIBLE)) {
			m_handler(this, EVENT_PRESSED, key );
			return true;
		}
		return false;
	}
	virtual bool OnKeyUp(int key) {
		if(m_handler && (state & SQSH_VISIBLE)) {
			m_handler(this, EVENT_UNPRESSED, key );
			return true;
		}
		return false;
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////

class CGeneralWindow : public CShellWindow
{
public:
	CGeneralWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p){}

	virtual int  EffectSupported()	{ return effectButtonsFadeIn|effectButtonsFadeOut; }
	virtual void draw(int bFocus);
	virtual int  HitTest(float _x, float _y){
		return 0;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////
//прогресс бары
class CProgressEnergy : public CShellWindow
{
public:
	float showedProduced;
	float showedUsed;
	float xstart;
	float arrowSx;
	float arrowSy;

	cTexture*		m_hTexture_h;
	Vect2f			m_vTexPos_h[2];

	CProgressEnergy(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p), showedProduced(0), showedUsed(0), m_hTexture_h(0) {produced = 0; requested = 0;}
	~CProgressEnergy();

	virtual void draw(int bFocus);
	virtual void OnWindow(int enable);
	virtual void Load(const sqshControl* attr);
	virtual int  HitTest(float _x, float _y) { return 0; };
	void setEnergyData(float newProduced, float newRequested) {
		xassert(produced >= 0);
		xassert(requested >= 0);
		produced = newProduced;
		requested = newRequested;
	}

protected:
	float produced;
	float requested;
};

class CProgressCollected : public CShellWindow
{
public:
	float showedDirection;
	CProgressCollected(int id, CShellWindow* pParent, EVENTPROC p) 
		: CShellWindow(id, pParent, p), showedDirection(0), 
		produced(0), requested(0), capacity(0), accumulated(0) {}

	virtual void draw(int bFocus);
	virtual void OnWindow(int enable);
	virtual void Load(const sqshControl* attr);
	void setEnergyData(float newProduced, float newRequested, float newCapacity, float newAccumulated) {
		produced = newProduced;
		requested = newRequested;
		capacity = newCapacity;
		accumulated = newAccumulated;
	}

protected:
	float produced;
	float requested;
	float capacity;
	float accumulated;
};

class CProgressShield : public CShellWindow
{
public:
	CProgressShield(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p){}

	virtual void draw(int bFocus);
	virtual void OnWindow(int enable);
};
class CProgressTerrain : public CShellWindow
{
public:
	CProgressTerrain(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p){}

	virtual void draw(int bFocus);
	virtual void OnWindow(int enable);
};
class CProgressMutation : public CShellWindow
{
public:
	CProgressMutation(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p){}

	virtual void draw(int bFocus);
	virtual void OnWindow(int enable);
	void setMutationEnergy(float energy) {
		mutationEnergy = energy;
	}
protected:
	float mutationEnergy;
};
class CProgressUnitCharge : public CShellWindow
{
public:
	CProgressUnitCharge(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p){chargeLevel = 0;}

	virtual void draw(int bFocus);

	void setParams(float newChargeLevel, bool isSpiralCharge) {
		chargeLevel = newChargeLevel;
		isSpiral = isSpiralCharge;
	}

protected:
	float chargeLevel;
	bool isSpiral;
};

class CPlayerColorWnd : public CShellWindow {
	public:
		CPlayerColorWnd(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) {
		}
		void draw(int bFocus) override;
};


class CScaleResultButton : public CScaleButton
{
public:
	cTexture *m_hTexture4;
	Vect2f	  m_vTex4Pos[2];

	bool victory;

	CScaleResultButton(int id, CShellWindow* pParent, EVENTPROC p);
	~CScaleResultButton();
	virtual void Load(const sqshControl* attr);
	virtual void draw(int bFocus);
};

////////////////////////////////////////////////////////////////////////////////////////////////
//инвормационное окошко

typedef void (*InfoWndFormatProcType)(char*, void*);

class CInfoWindow : public CShellWindow
{
	bool  m_bCentered;

	char  m_cbText[255];
	int   m_nTimeToDisplay;

	void*                  m_pParam;
	InfoWndFormatProcType  m_pFmtProc;

public:
	CInfoWindow(int id, CShellWindow* pParent, EVENTPROC p);
	~CInfoWindow();

	void SetText(const char* cb, InfoWndFormatProcType proc = 0, void* param = 0);
	void SetTime(int t);
	void Centered(){
		m_bCentered = true;
	}
	virtual void draw(int bFocus);
};

class CHintWindow : public CShellWindow
{

	std::string textData;
	int   m_nTimeToDisplay;

	float cutSceneX;
	float cutSceneY;
	float cutSceneSX;

public:
	CHintWindow(int id, CShellWindow* pParent, EVENTPROC p);
	~CHintWindow();

	void Load(const sqshControl* attr);
	void SetText(const char* cb, bool incremental = false);
	void SetTime(int t);
	virtual void draw(int bFocus);
	void drawHint(bool cutScene);
};

class CChatInfoWindow : public ChatWindow
{
    cTexture* m_hPopupTexture;
	int   m_nTimeToDisplay;

public:
	CChatInfoWindow(int id, CShellWindow* pParent, EVENTPROC p);
	~CChatInfoWindow();

	void setTime(int time) {
		m_nTimeToDisplay = time;
	}
    
	void updateTime(int time) {
		if (m_nTimeToDisplay >= 0 && m_nTimeToDisplay < time) {
			m_nTimeToDisplay = time;
		}
	}
    
    int CheckClick(float _x,float  _y) override;
    
	int HitTest(float _x, float _y) override;

    void Load(const sqshControl* attr) override;
    
	void draw(int bFocus) override;
};

class CNetLatencyInfoWindow : public CShellWindow
{
    std::string fullData;
    std::string briefData;
    bool on_window = false;

public:
    CNetLatencyInfoWindow(int id, CShellWindow* pParent, EVENTPROC p);
    ~CNetLatencyInfoWindow();

    void Load(const sqshControl* attr) override;
    void SetText(const std::string& brief, const std::string& full);
    void OnWindow(int enable) override;
    void draw(int bFocus) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////
//background под интерфейсом
class CBackgroundWindow : public CShellWindow
{

public:
	CBackgroundWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) {
	}
	virtual int HitTest(float _x, float _y);

	virtual void draw(int bFocus);
};

////////////////////////////////////////////////////////////////////////////////////////////////
class ANIFile;

//курсоры
class CShellCursorManager
{
	struct CShellCursor
	{
        cTexture* texture;
        ANIFile* anifile;
		char bHotspotCentered; //This is used for toolzers
        //Size
		float sx, sy;
	};

	float m_ftime;
	float cur_x, cur_y;
	std::deque<CShellCursor> m_cursors;
    CShellCursor* m_pActiveCursor;
    CShellCursor* m_pCursorDefault;
	int           m_nCursorShift;

	cFont*        m_hFontCursorWorkarea;

public:
	enum
	{
		arrow,
		workarea_in,
		workarea_out,
		core_install,
		core_install_no,
		core_cant_install,
		sell,
		sell_no,
		select,
		attack,
		wait,
		arrow_up,
		arrow_down,
		arrow_left,
		arrow_right,
		arrow_up_left,
		arrow_up_right,
		arrow_down_left,
		arrow_down_right,
		map_move,
		tilt,
		rotate,
		goto_frame,
		rov,
		no_attack,
		move,
		arrow_enemy
	};

	char  m_bCursorPermanent;
	char  m_bShowSideArrows;

	CShellCursorManager();
	~CShellCursorManager();

	void Load();
	void Done();
	void SetActiveCursor(int cursor, char bPermanent = 0);
	void OnWMSetCursor();
	void OnMouseMove(float x, float y);

	void draw();
    static void DrawCursor(CShellCursor* cursor, int x, int y, float phase = 0, float scale = 1.0f);

	float GetSize();
	void  SetSize(float sx);

	void HideCursor();
	void ShowCursor();
};

////////////////////////////////////////////////////////////////////////////////////////////////
//

enum
{
	CBCODE_QUANT = 1, //задержка по времени в delay(ms)
	CBCODE_MOUSEMOVE,
	CBCODE_LBDOWN,
	CBCODE_LBUP,
	CBCODE_RBDOWN,
	CBCODE_RBUP,
};

struct DYN_QUEUE_ITEM
{
	DYNCALLBACK cbproc;
	int         code;
	int         time_delay;
	char        bDelete;


	DYN_QUEUE_ITEM(DYNCALLBACK _p, int _c, int delay){
		cbproc = _p; code = _c;
		bDelete = 0;
		time_delay = delay;
	}
};

struct FRAME_TERRAIN_BUILD_DATA
{
	int              nUnitRequested;
	terUnitBase* pUnit;
	float            phase;
};

struct MiniMapEventIcon : public MiniMapEvent {
	MiniMapEventIcon(const MiniMapEvent& ev) {
		code = ev.code;
		pos = ev.pos;
		timeRemain = eventSymbols[code].period;
		animTime = eventSymbols[code].animTime;
		period = timeRemain;
	}
	float timeRemain;
	float period;
	float animTime;
};

class CShellIconManager
{
	struct Task {
		Task(std::string text, ActionTask::Type type) : text(text), type(type) {
		}
		std::string text;
		ActionTask::Type type;
	};

	CShellWindow*	m_pDesktop;
	CShellWindow*	m_pFocus;
	CShellWindow*	m_pLastClicked;
	CShellWindow*	m_pModalWnd;
	CShellWindow*	controls[SQSH_MAX + 1];
	float			m_fTimePressed;

	cFont*          m_hFontPopup;
	cFont*          m_hFontCountDownTime;
	cTexture*		m_hPopupTexture;

	float           m_fPhase;

	int             m_nPromptMessageDelayTime;

	int             m_nControlGroupLoaded;

	int             m_nTotalUnitsBaseCount;

	std::list<MiniMapEventIcon> miniMapEventIcons;

	typedef std::list<Task> TaskList;
	TaskList tasks;

	std::vector<SQUAD_ICON*> m_squad_icon_table;
	void BuildSquadIconTable();
	void ClearSquadIconTable();

	//dynamic handlers
	std::list<DYN_QUEUE_ITEM> m_dyn_queue;
	int ProcessDynQueue(int code, float x=0, float y=0);
	void QuantDynQueue(int dt);

	CShellWindow* HitTest(CShellWindow* pTop, float x, float y);
	CShellWindow* HitTest(float x, float y);
	CShellWindow* FindWnd(CShellWindow* pTop, int id);
	void Destroy(CShellWindow* pTop);
	void DrawControls(CShellWindow* pTop);

	int EffectControls(CShellWindow* pWnd, int effect);

	void OpenPromtMessagesFile();

	bool handleEvent(const iEvent* ev);

	//icons status
	void ShowUpgradeBuildProcessIcons(const terUnitBase* pUnit);
	void ShowActionIcons(const UnitInterfacePrm& intf_prm, const UnitInterfaceActions& actions, terUnitBase* pUnit);
	void UpdateActionStatus(terUnitBase* pUnit);
	void UpdateIcons();
	void UpdateMiniMapEvents();
	void UpdateSelectionIcons();
	void UpdateBrigFrameIcons();
	void UpdateSquadIcons();
	void UpdateStationIcons();

	void UpdateMiscIcons();
	void UpdateGunsIcons();
	void UpdateBuildingsIcons();

	void updateControlsFromExternalStates();

    std::vector<SaveControlData> externalControlStates;
	std::vector<SaveControlData> externalBuildTabStates;
	std::vector<SaveControlData> externalSquadTabStates;

	bool cutSceneModeOn;

	int cutSceneAnimDirection;
	int cutSceneAnimStep;
	int cutSceneAnimTimer;

	SpeechPlayer* speechSound;
	bool resultMusicStarted;

	void onCutSceneStart();
	void onCutSceneStop();

	float game_speed_to_resume;

	struct CutSceneCommand {
		bool active;
		bool on;
		bool animated;
	};

	CutSceneCommand cutSceneCommand;

public:
	std::string hold;
	std::string units;
	std::string totalTime;
    ShellControlID initialMenu;

	LogicData::sProgress& GetProgress(terUnitAttributeID n_struct);
	float			m_fEffectTime;

	CShellWindow*   m_pCtrlHover;
	int             m_nMouseButtonsState;

	/// true если активно поле ввода
	/// при этом отрабатывается автоповтор при удержании кнопок
	/// если false то автоповтор игнорируется
	bool			m_editMode;

	bool            m_bEnergyCanBuild;
	float           m_fProgressEnergy;

	cTexture*		m_hTextureProgressBars;

	std::list<CShellWindow*>  m_effect_list;

	CShellIconManager();
	~CShellIconManager();

	void load(const SaveTaskList& tasksIn);
	void save(SaveTaskList& tasksOut) const;

	void Done();
	void LoadControlsGroup(int nGroup, bool force = false);
	void LoadTabSheets();
	void PostLoadTabSheets();
	void Toggle(bool game_active);
	void setCutSceneMode(bool on, bool animated = true);
	void setCutSceneModeSafe(bool on, bool animated = true);

	std::list<MiniMapEventIcon>& getMiniMapEventIcons() {
		return miniMapEventIcons;
	}

	CShellWindow* GetWnd(int id);
	CShellWindow* CreateWnd(int id, ShellControlType type, CShellWindow* pParent = 0, EVENTPROC proc = 0);
	void reload(CShellWindow* pTop);

	bool isDynQueueEmpty();

	void FormatUnitPopup(const AttributeBase* attr, char* cbBuffer, bool onControl, terUnitBase* unit);
	char* FormatMessageText(const char* cbTag, char* cb, ...);

	void AddDynamicHandler(DYNCALLBACK _p, int code, int delay = 0);
	void DelDynamicHandler(DYNCALLBACK _p, int code);
	bool HasDynamicHandler(DYNCALLBACK _p, int code);

	void SetFocus(int id);
	void SetModalWnd(int id);
	int  IsFocusControl();
	bool isInEditMode() const;

	int OnLButtonDown(float x, float y);
	int OnLButtonUp(float x, float y);
	int OnRButtonDown(float x, float y);
	int OnRButtonUp(float x, float y);

	void changeControlState(const std::vector<SaveControlData>& newControlStates, bool reset_controls);
	void fillControlState(std::vector<SaveControlData>& controlStatesToSave);

	int OnMouseMove(float x, float y);
	int OnLButtonDblClk(float x, float y);
//	int OnRButtonDblClk(float x, float y);
	int OnChar(char key);
	int OnKeyDown(int key);
	int OnKeyUp(int key);
	int OnMouseWheel(int delta);
		 
	void Effect(int effect, CShellWindow* pWnd = 0);
	void Effect(int effect, int id);
	void SwitchMenuScreens(int id_off, int id_on);

	void quant(float dTime);
	void draw();

	void ClearQueue(){
		MTAuto dynQueue_autolock(&dynQueue_lock);
		m_dyn_queue.clear();
	}
	inline int IsEffect()	{ return m_fEffectTime>=0; }
	inline int IsInterface(){ return getDesktop() != 0;}

	void ShowControls(int bShow, const ShellControlID ids[], int numIds);

	void onSizeChanged();

	void addChatString(const LocalizedText* newChatString);

	void showHintChat(const LocalizedText* text, int showTime);
	void showHint(const char* text, int showTime, ActionTask::Type actionType = ActionTask::ASSIGNED);
	void setTask(const char* text, ActionTask::Type actionType = ActionTask::ASSIGNED);

	void lButtonReset();

	void UnEffectControls(CShellWindow* pWnd, int effect);
	void fillTaskWnd();
	CShellWindow* getDesktop() {
		return cutSceneModeOn ? 0 : m_pDesktop;
	}
	bool isCutSceneMode() {
		return cutSceneModeOn;
	}

	void forceDraw();

	float playSpeech(const char* id);
	void playGameOverSound(const char* path);
	void setupAudio();
	void speedChanged(float speed);

	void gameTypeChanged(UserSingleProfile::GameType newType);

	bool menuVisible();
	bool isInGameGroup() const {
		return (m_nControlGroupLoaded == SHELL_LOAD_GROUP_GAME);
	}
	int getVisibleMenuScr();
	void hideAllVisibleMenuScr();

	void setNormalSpeed() {
		if (GetWnd(SQSH_SPEED_100)->isEnabled()) {
			gameShell->setSpeed(1);
		}
	}
	void decrSpeedStep() {
		if (gameShell->getSpeed() > 0) {
			if (gameShell->getSpeed() <= 1) {
				if (GetWnd(SQSH_SPEED_50)->isEnabled()) {
					gameShell->setSpeed(0.5f);
				}
			} else if (gameShell->getSpeed() <= 2) {
				if (GetWnd(SQSH_SPEED_100)->isEnabled()) {
					gameShell->setSpeed(1);
				}
			}
		}
	}
	void incrSpeedStep() {
		if (gameShell->getSpeed() >= 1) {
			if (GetWnd(SQSH_SPEED_150)->isEnabled()) {
				gameShell->setSpeed(2);
			}
		} else if (gameShell->getSpeed() >= 0.5f) {
			if (GetWnd(SQSH_SPEED_100)->isEnabled()) {
				gameShell->setSpeed(1);
			}
		}
	}

	bool getCurrentEnabledOperation();
	void hideChatInfo() {
		CChatInfoWindow* chatInfo = (CChatInfoWindow*) controls[SQSH_CHAT_INFO_ID];
		chatInfo->setTime(CHATINFO_VISIBLE_TIME_AFTER_HIDE_EDIT);
	}

	MTSection dynQueue_lock;
};

extern CShellIconManager   _shellIconManager;
extern CShellCursorManager _shellCursorManager;
extern MissionDescription *gb_SelectWorld;

inline void _WaitCursor() {
    _shellCursorManager.SetActiveCursor(CShellCursorManager::wait);
}

////
//event handlers
void OnButtonWorkArea(CShellWindow* pWnd, InterfaceEventCode code, int param);
int OnLBUpWorkarea(float,float);
int OnLBDownWorkarea(float,float);
int OnRBDownWorkarea(float,float);
int OnMouseMoveRegionEdit(float x, float y);
int OnMouseMoveRegionEdit2(float x, float y);
void OnButtonSell(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnCluster1(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonLegion(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonStructure(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnButtonTogether(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnButtonFieldOn(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonFieldOff(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnButtonDisintegrate(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonSquadMutate(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonOffensiveDefensive(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnMapWindowClicked(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonBrigadiers(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonGotoBase(CShellWindow* pWnd, InterfaceEventCode code, int param);

//start / general
void showSingleMenu(CShellWindow* pWnd);
void onMMSingleButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMMultiplayerButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMQuitButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMOptionsButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLangButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMOpenLinkButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//addons
void loadAddonsList();
void onMMAddonsButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMAddonsList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMAddonsApplyButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMAddonsEnableCombo(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMAddonsGuideButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMAddonsBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//community
void onMMCommunityButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//briefing
void onMMNomadNameBriefing(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMNomadIconBriefing(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMQuitFromBriefingButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMContinueBriefingButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMStartMissionButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMSkipMissionButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMSkipBriefingButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMYearBriefing(CShellWindow* pWnd, InterfaceEventCode code, int param);

//single
void onMMProfileButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMScenarioButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattleButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLoadButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLoadReplayButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//content chooser
void fillContentChooserList();
void onMMContentChooserButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMContentChooserList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMContentChooserSelectButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//profile
void fillProfileList();
void onMMNewProfileButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMDelProfileButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMSelectProfileButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBackFromProfileButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMProfileList(CShellWindow* pWnd, InterfaceEventCode code, int param);

//scenario
void onMMMissionList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMDifficultyCombo(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//endmission
void onMMResumeButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMContinueButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//stats
void onMMRestartButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMSaveReplayButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMQuitFromStatsButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMContinueFromStatsButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMStatsTotalButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMStatsGeneralButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMStatsUnitsButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMStatsBuildingsButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//inmission
void onMMInMissSaveButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMInMissLoadButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMInMissOptionsButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMInMissRestartButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMInMissResumeButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMInMissQuitButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//battle
void onMMMapList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattleFirstSlotButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattleFrmButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattleClrButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattleSlotButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattleClanButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattleHCButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattleGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattlePageNextButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattlePagePrevButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMBattlePageText(CShellWindow* pWnd, InterfaceEventCode code, int param);

//load game
void onMMLoadMapList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLoadGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMDelSaveButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//load replay
void onMMReplayList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMReplayGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMDelReplayButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//load in game
void onMMLoadInGameMapList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLoadInGameGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMDelLoadInGameButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//save game
void onMMSaveGameMapList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMSaveGameGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMDelSaveGameButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//save replay
void onMMSaveReplayList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMSaveReplayGoButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMDelSaveReplayButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

void onMMTaskButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//join game
void onMMMultiplayerJoinNextBtn(CShellWindow* pWnd, InterfaceEventCode code, int param);

//multiplayer list
void onMMMultiplayerListGameList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMMultiplayerListCreateButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMMultiplayerListJoinButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMMultiplayerListDirectButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMMultiplayerListBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//multiplayer host
void fillMultiplayerHostList();
void onMMMultiplayerHostDelButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMMultiplayerHostMapList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMMultiplayerHostNextButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMMultiplayerHostTypeCombo(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMMultiplayerGameSpeedSlider(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMMultiplayerHostBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//lobby
void setupMultiplayerLobby();
void onMMLobbyGameNameButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyNameButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyFrmButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyClrButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbySlotButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyClanButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyHCButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyStartButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyChatInputButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyMapList(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyPageNextButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyPagePrevButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMLobbyPageText(CShellWindow* pWnd, InterfaceEventCode code, int param);

//options
void onMMGameButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMGraphicsButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMSoundButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//submit dialog
void onMMSubmitYesButton(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onMMSubmitNoButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnButtonMMOptions(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnButtonOptionGame(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonOptionGraphics(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonOptionSound(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnSliderSoundVolume(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnSliderMusicVolume(CShellWindow* pWnd, InterfaceEventCode code, int param);

//graphics
void OnComboGraphicsUIAnchor(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGraphicsInputGrab(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGraphicsFog(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGraphicsSettings(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnGraphicsCustomBtn(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnGraphicsApplyBtn(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnSliderGraphicsGamma(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGraphicsColorDepth(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGraphicsResolution(CShellWindow* pWnd, InterfaceEventCode code, int param);

//custom
void onLODCombo(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onParticleSlider(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onOcclusionCombo(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onPointLightCombo(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onSamplesCombo(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onBumpCombo(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onBumpChaosCombo(CShellWindow* pWnd, InterfaceEventCode code, int param);
void onCompressCombo(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGraphicsFurrows(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGraphicsShadows(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGraphicsReflection(CShellWindow* pWnd, InterfaceEventCode code, int param);

//credits 
void onMMCreditsButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

void onMMInGameChatInputButton(CShellWindow* pWnd, InterfaceEventCode code, int param);

//ingame graphics
void OnIngameGraphicsCustomBtn(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnComboGraphicsMode(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnComboSoundMusic(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboSoundEffects(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnComboGameTooltips(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGameRunBackground(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGameStartSplash(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnComboGameCameraMode(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnSliderScrollRate(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnSliderAngleSens(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnSplashScreenKey1(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnSplashScreenKey2(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnSplashScreenKey3(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnSplashScreenKey4(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnSplashScreenKeyLast(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnButtonTerrainBuild(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonSpeed(CShellWindow* pWnd, InterfaceEventCode code, int param);

void exitToInterfaceMessage(CShellWindow* pWnd);

void OnSquadTabEvent(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnFrameTabEvent(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonMenu(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonTarget(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonAttack(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonFrameInstall(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonFrameTeleportate(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonUpgrade(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonBackToFrame(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonAlarm(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonStartCharge(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonStopCharge(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonFieldSingleOn(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonFieldSingleOff(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonStructuteOn(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonStructuteOff(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonPatrol(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonStop(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonBrigadierChange(CShellWindow* pWnd, InterfaceEventCode code, int param);
void OnButtonBrigadierBuild(CShellWindow* pWnd, InterfaceEventCode code, int param);

void OnButtonReplayPlayer(CShellWindow* pWnd, InterfaceEventCode code, int param);

std::string getImageFileName(const sqshImage* image, const char* fileName = 0);

//relative------------------
/*
inline void DrawSpriteRelative(int x, int y, int dx, int dy, float u, float v, float du, float dv,
		cTexture *Texture, sColor4c &ColorMul = sColor4c(255,255,255,255), float phase = 0, eBlendMode mode = ALPHA_NONE)
{
//	terRenderDevice->DrawSprite(relativeX(x), relativeY(y), relativeX(dx), relativeY(dy), u, v, du, dv, Texture, ColorMul, phase, mode);
}
inline int OutTextRelative(int x,int y,const char *string,sColor4f *color,int align=-1) {
//	return terRenderDevice->OutText(relativeX(x), relativeY(y), string, color, align);
	return 1;
}

inline void OutTextRectRelative(int x,int y,const char *string,int align,Vect2f& bmin,Vect2f& bmax) {
//	terRenderDevice->OutTextRect(relativeX(x), relativeY(y), string, align, bmin, bmax);
}
inline void DrawRectangleRelative(int x,int y,int dx,int dy,sColor4c color,bool outline=false) {
//	terRenderDevice->DrawRectangle(relativeX(x), relativeY(y), relativeX(dx), relativeY(dy), color, outline);
}
inline void draw_lineRelative(const Vect2f& a, const Vect2f& b, const sColor4f& c = sColor4f(1, 1, 1, 1))
{
	int x1 = a.x;
	int x2 = b.x;
	int y1 = a.y;
	int y2 = b.y;
//	DrawLineRelative(x1, y1, x2, y2, int(c.r*255),int(c.g*255),int(c.b*255),int(c.a*255));
}
inline void draw_rectRelative(const Vect2i& a, const Vect2i& b, const sColor4c& c)
{
	int x1 = a.x;
	int x2 = b.x;
	int y1 = a.y;
	int y2 = b.y;

//	DrawRectangleRelative(x1, y1, x2-x1, y2-y1, c);
}
inline void draw_rect_emptyRelative(const Vect2f& a, const Vect2f& b, const sColor4f& c = sColor4f(0, 0, 0, 1))
{
//	draw_lineRelative(Vect2f(a.x, a.y), Vect2f(b.x, a.y), c);
//	draw_lineRelative(Vect2f(b.x, a.y), Vect2f(b.x, b.y), c);
//	draw_lineRelative(Vect2f(b.x, b.y), Vect2f(a.x, b.y), c);
//	draw_lineRelative(Vect2f(a.x, b.y), Vect2f(a.x, a.y), c);
}
inline void draw_rectRelative(const Vect2i& a, const Vect2i& b, const sColor4f& c = sColor4f(0, 0, 0, 1))
{
//	int x1 = a.x;
//	int x2 = b.x;
//	int y1 = a.y;
//	int y2 = b.y;
//
//	DrawRectangleRelative(x1, y1, x2-x1, y2-y1, sColor4c(c));
}
inline void drawPointRelative(const Vect2i& a, const sColor4f& c = sColor4f(0, 0, 0, 1), int sz = 1)
{
//	int x1 = a.x;
//	int x2 = x1 + sz;
//	int y1 = a.y + 1;
//	int y2 = y1 + sz;
//
//	DrawRectangleRelative(x1, y1, x2-x1, y2-y1, sColor4c(c));
}
*/
//absolute-----------------
inline void DrawSprite(int x, int y, int dx, int dy, float u, float v, float du, float dv,
		cTexture *Texture, const sColor4c &ColorMul = sColor4c(255,255,255,255), float phase = 0, eBlendMode mode = ALPHA_NONE)
{
	terRenderDevice->DrawSprite(x, y, dx, dy, u, v, du, dv, Texture, ColorMul, phase, mode);
}

inline void DrawSprite2(int x, int y, int dx, int dy, float u, float v, float du, float dv,
		cTexture *Tex1, cTexture *Tex2, const sColor4c &ColorMul = sColor4c(255,255,255,255), float phase = 0)
{
	terRenderDevice->DrawSprite2(x, y, dx, dy, u, v, du, dv, Tex1, Tex2, ColorMul, phase);
}

inline void DrawSprite2(int x, int y, int dx, int dy, float u, float v, float du, float dv, float u1, float v1, float du1, float dv1,
		cTexture *Tex1, cTexture *Tex2, const sColor4c &ColorMul = sColor4c(255,255,255,255), float phase = 0, eColorMode mode = COLOR_MOD)
{
	terRenderDevice->DrawSprite2(x, y, dx, dy, u, v, du, dv, u1, v1, du1, dv1, Tex1, Tex2, ColorMul, phase, mode);
//	terRenderDevice->DrawRectangle(relativeX(x), relativeY(y), relativeX(dx), relativeY(dy), sColor4c(255,255,0,255), true);
}

int OutText(int x,int y,const char *string,sColor4f *color,int align=-1,eBlendMode blend_mode=ALPHA_BLEND);

inline void OutTextRect(int x,int y,const char *string,int align,Vect2f& bmin,Vect2f& bmax) {
	terRenderDevice->OutTextRect(x, y, string, align, bmin, bmax);
}

inline void draw_line(const Vect2f& a, const Vect2f& b, const sColor4f& c = sColor4f(1, 1, 1, 1))
{
	int x1 = a.x;
	int x2 = b.x;
	int y1 = a.y;
	int y2 = b.y;
	terRenderDevice->DrawLine(x1, y1, x2, y2, sColor4c(c));
}

inline void draw_rect(const Vect2f& a, const Vect2f& b, const sColor4f& c = sColor4f(0, 0, 0, 1))
{
	int x1 = a.x;
	int x2 = b.x;
	int y1 = a.y;
	int y2 = b.y;

	terRenderDevice->DrawRectangle(x1, y1, x2-x1, y2-y1, sColor4c(c));
}
inline void draw_rect(const Vect2i& a, const Vect2i& b, const sColor4c& c)
{
	int x1 = a.x;
	int x2 = b.x;
	int y1 = a.y;
	int y2 = b.y;

	terRenderDevice->DrawRectangle(x1, y1, x2-x1, y2-y1, c);
}

inline void draw_rect_empty(const Vect2f& a, const Vect2f& b, const sColor4f& c = sColor4f(0, 0, 0, 1))
{
    int x1 = a.x;
    int x2 = b.x;
    int y1 = a.y;
    int y2 = b.y;
    terRenderDevice->DrawRectangle(x1, y1, x2-x1, y2-y1, c, 1.0f);
}
inline void draw_line(const Vect2i& a, const Vect2i& b, const sColor4f& c = sColor4f(1, 1, 1, 1))
{
	terRenderDevice->DrawLine(a.x, a.y, b.x, b.y, sColor4c(c));
}
inline void draw_rect(const Vect2i& a, const Vect2i& b, const sColor4f& c = sColor4f(0, 0, 0, 1))
{
	int x1 = a.x;
	int x2 = b.x;
	int y1 = a.y;
	int y2 = b.y;

	terRenderDevice->DrawRectangle(x1, y1, x2-x1, y2-y1, sColor4c(c));
}
inline void draw_point(const Vect2i& a, const sColor4f& c = sColor4f(0, 0, 0, 1), int sz = 1)
{
	int x1 = a.x;
	int x2 = x1 + sz;
	int y1 = a.y + 1;
	int y2 = y1 + sz;

	terRenderDevice->DrawRectangle(x1, y1, x2-x1, y2-y1, sColor4c(c));
}

inline void draw_circle(Vect2i& v, int r, sColor4c& c)
{
	float dp = XM_PI/40, phi = dp;
	int x1 = r + v.x;
	int y1 = v.y;
	int x, y;

	while(phi < 2*XM_PI)
	{
		x = r*xm::cos(phi) + v.x;
		y = r*xm::sin(phi) + v.y;

		terRenderDevice->DrawLine(x, y, x1, y1, c);

		phi += dp;
		x1 = x; y1 = y;
	}
	x = r + v.x;
	y = v.y;
	terRenderDevice->DrawLine(x, y, x1, y1, c);
}

inline std::string getTextFromBase(const char *keyStr) {
	std::string key("Interface.ButtonLabels.");
	key += keyStr;
	const char* stringFromBase = qdTextDB::instance().getText(key.c_str());
	return (*stringFromBase) ? stringFromBase : keyStr;
}

#endif //_PERIMETERSHELLUI_H
