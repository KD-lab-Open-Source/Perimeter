
#include "StdAfx.h"
#include "InterfaceLogic.h"
#include "Runtime.h"
#include "CameraManager.h"
#include "Config.h"
#include "Terra.h"
#include "UnitAttribute.h"
#include "Universe.h"
#include "GenericControls.h"

#include "UniverseInterface.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"

#include "..\sound\PerimeterSound.h"
#include "..\Game\MusicManager.h"

#define _RELEASE(p) if(p) {(p)->Release(); (p) = 0;}

#define SLIDER_ARROW_SIZE			0
#define SLIDER_POS_SIZE				68

void GetCameraViewPolygon(Vect2f* pv, int zero_level);
bool LineClip2d(Vect2i& v1, Vect2i& v2,  const Vect2i& _w1, const Vect2i& _w2);
void CopyClusterBitmap(sColor4c* bitmap, int sx, int sy);

STARFORCE_API_NEW terUnitAttributeID Button2LegionID(int id);
STARFORCE_API_NEW terUnitAttributeID Button2StructureID(int nBtnID);

terUnitSquad* GetSquadByNumber(int n);

//цвета для кнопок
sColor4c clrNormal(255, 255, 255, 255);
sColor4c clrMark(255, 0, 0, 255);
sColor4c clrDisabled(128, 128, 128, 255);

//фонты для кнопок главного меню
cFont* hFontMainmenu1 = 0;
cFont* hFontMainmenu2 = 0;
cFont* hFontMainmenu3 = 0;
cFont* hFontMainmenu4 = 0;

CShellComplexPushButton* workAreaBtnForInfo;

MpegSound gb_Music;
string strMusic;

void PlayMusic(const char *str = 0)
{
	if (str) {
		if (gb_Music.IsPlay() && strMusic == str) {
			return;
		}
		strMusic = str;
	}
	gb_Music.Stop();
	if( !terMusicEnable || strMusic.empty() ) return;
	int ret = gb_Music.OpenToPlay(strMusic.c_str(),1);
	gb_Music.SetVolume( round(255*terMusicVolume) );
}

void MusicEnable(int enable)
{
	terMusicEnable = enable;
	if(enable) {
		if (gameShell->GameActive) {
			MusicOpenWorld();
		} else {
			PlayMusic( strMusic.c_str() ); 
		}
	} else {
		if (gameShell->GameActive) {
			MusicCloseWorld();
		} else {
			gb_Music.Stop();
		}
	}
}

void SetVolumeMusic(float f)
{
	gb_Music.SetVolume( round(255*terMusicVolume) );
}

inline void draw_progress(cTexture* texture, int x, int y, int sx, int sy, sColor4c& clr, float fPercent)
{
	DrawSprite(x, y, sx*fPercent, sy, 0, 0, 1, 1, texture, clr);
}


int getValidatedTextLength(const string& text, float validWidth) {
	if (validWidth >= terRenderDevice->GetFontLength(text.c_str())) {
		return text.length();
	}

	sColor4c diffuse(0,0,0,0);

	float dotWidth = terRenderDevice->GetFontLength("...");

	const char* textStart = text.c_str();

	float width = 0;
	for (const char* str = textStart; *str; str++) {
		terRenderDevice->ChangeTextColor(str, diffuse);
		width += terRenderDevice->GetCharLength(*str);
		if (width > (validWidth - dotWidth)) {
			return (str - textStart);
		}
	}

	return text.length(); 
}

string getValidatedText(const string& text, float validWidth) {
	int count = getValidatedTextLength(text, validWidth);
	if (count == text.length()) {
		return text;
	} else {
		return (text.substr(0, count) + "...");
	}
}

///////////////////////////////////////////////////////////////////

CShellWindow::CShellWindow(int id, CShellWindow* pParent, EVENTPROC p)
{
	m_effect = 0;
	m_hTexture = 0;
	m_ftime = 0;
	m_attr = 0;
	m_attr_cont = 0;

	ID = id;
	m_handler = p;
	m_pParent = pParent;
	m_sound = 0;
	m_hFont = 0;
	  
	m_cColor = clrNormal;
	m_cBGColor = clrNormal;

	flashR = 255;
	flashG = 0;
	flashB = 0;
	flashA = 255;
	flashBG_R = 255;
	flashBG_G = 0;
	flashBG_B = 0;
	flashBG_A = 64;
	tickTime = 0;

	state = 0;

//	flashInterval = -1;
	flashTimer = -1;
	checkHotKeyVisibility = true;

	if(pParent)
		pParent->m_children.push_back(this);
}
CShellWindow::~CShellWindow()
{
	_RELEASE(m_hTexture);
	_RELEASE(m_hFont);
}

void CShellWindow::OnMouseWheel(int delta)
{
	if( !(state & SQSH_VISIBLE) ) return;
	if( m_handler )
		m_handler(this, EVENT_WHEEL, delta );
}

bool CShellWindow::checkDefaultEnterBtns() {
	bool res = false;
	if (m_attr_cont) {
		CShellWindow* btn = _shellIconManager.GetWnd(m_attr_cont->defaultEnterBtnID);
		res = checkDefaultBtn(btn);
		if (!res) {
			btn = _shellIconManager.GetWnd(m_attr_cont->defaultEnterBtnIDAux);
			res = checkDefaultBtn(btn);
		}
	}
	return res;
}
bool CShellWindow::checkDefaultEscBtn() {
	bool res = false;
	if (m_attr_cont) {
		CShellWindow* btn = _shellIconManager.GetWnd(m_attr_cont->defaultEscBtnID);
		res = checkDefaultBtn(btn);
	}
	return res;
}

bool CShellWindow::OnKeyDown(int key)
{
	if(_shellIconManager.IsEffect())
		return false;

	bool res = false;

	CShellWindow* container = isContainer() ? this : m_pParent;

	if (key == VK_ESCAPE) {
		res = container->checkDefaultEscBtn();
	} else if (key == VK_RETURN) {
		res = container->checkDefaultEnterBtns();
	}
	return res;

//	if(!res && m_handler && (state & SQSH_VISIBLE))
//		m_handler(this, EVENT_PRESSED, key );
}
bool CShellWindow::OnKeyUp(int key)
{
//	if(_shellIconManager.IsEffect()) 
//		return false;
	return false;
//	if(m_handler && (state & SQSH_VISIBLE) )
//		m_handler(this, EVENT_UNPRESSED, key );
}
void CShellWindow::OnMouseMove(float _x,float _y)
{
	if(	!m_handler ) return;
	float xy[2] = { _x, _y };
	m_handler(this, EVENT_SLIDERUPDATE, (int)&xy[0] );
}
void CShellWindow::LoadMenuWnd(const sqshControlContainer* attr)
{
	_RELEASE(m_hTexture);

	m_attr_cont = attr;
	if(strlen(attr->image.texture))
	{
		m_hTexture = terVisGeneric->CreateTexture( getImageFileName(&(attr->image)).c_str() );
//		SetTexPos(Vect2f(0,0), Vect2f(1, 1));
		m_vTexPos[0] = relativeUV(attr->image._ix, attr->image._iy, m_hTexture);
		m_vTexPos[1] = Vect2f(
			attr->sx * (float)terRenderDevice->GetSizeX() / (float)m_hTexture->GetWidth(),
			attr->sy * (float)terRenderDevice->GetSizeY() / (float)m_hTexture->GetHeight() );
	}
	x = absoluteX(attr->x);
	y = absoluteY(attr->y);
	sx = absoluteX(attr->sx);
	sy = absoluteY(attr->sy);

	_RELEASE(m_hFont);
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont, defaultFontSize);
}

void CShellWindow::createHotKeyString() {
	hotKeyPopupString = "\n";
	static char hotKeyBuffer[200];
	for (int i = 0; i < m_attr->actions.size(); i++) {
		if (strlen(m_attr->actions[i].name) && strlen(m_attr->actions[i].hotKeyPopup)) {
			_shellIconManager.FormatMessageText(
				m_attr->actions[i].hotKeyPopup,
				hotKeyBuffer,
				gameShell->hotKeyManager->getKeyNameForControlAction(m_attr->actions[i]).c_str()
			);
			hotKeyPopupString += hotKeyBuffer;
		}
	}
}

void CShellWindow::Load(const sqshControl* attr)
{
	_RELEASE(m_hTexture);

	loadFlashingParams(attr);
	m_attr = attr;
	createHotKeyString();
	x = absoluteX(m_attr->x);
	y = absoluteY(m_attr->y);
	sx = absoluteX(m_attr->sx);
	sy = absoluteY(m_attr->sy);
	if(strlen(attr->image.texture))
	{
		m_hTexture = terVisGeneric->CreateTexture( getImageFileName(&(attr->image)).c_str() );
		m_vTexPos[0] = relativeUV(attr->image._ix, attr->image._iy, m_hTexture);
		if (attr->sx > 2.0f || attr->sy > 2.0f) {
			m_vTexPos[1] = Vect2f(
				attr->sx / 1024.0f * (float)terRenderDevice->GetSizeX() / (float)m_hTexture->GetWidth(),
				attr->sy / 768.0f * (float)terRenderDevice->GetSizeY() / (float)m_hTexture->GetHeight() );
		} else {
			m_vTexPos[1] = Vect2f(
				attr->sx * (float)terRenderDevice->GetSizeX() / (float)m_hTexture->GetWidth(),
				attr->sy * (float)terRenderDevice->GetSizeY() / (float)m_hTexture->GetHeight() );
		}
	}

	markColor.set(flashR, flashG, flashB, flashA);
	if(attr->sound && strlen(attr->sound))
		m_sound = attr->sound;
}

void CShellWindow::reload() {
	if (m_attr) {
		Load(m_attr);
	} else if (m_attr_cont) {
		LoadMenuWnd(m_attr_cont);
	}
}

void CShellWindow::scale(Vect2f& scaleV)
{
	x *= scaleV.x;
	y *= scaleV.y;
	sx *= scaleV.x;
	sy *= scaleV.y;
}

void CShellWindow::SetTexPos(const Vect2f& v1, const Vect2f& v2)
{
	m_vTexPos[0] = v1;
	m_vTexPos[1] = v2;
}
void CShellWindow::setFlashingInterval(float interval) {
//	flashInterval = interval;
	flashTimer = interval;
//	if (isFlashing()) {
//		markColor.set(flashR, flashG, flashB, flashA);
//	}
}
bool CShellWindow::updateColor() {
	if (isFlashing()) {
		m_ftime += frame_time.delta();
		if (flashTimer) {
			flashTimer -= frame_time.delta();
			if (flashTimer == 0) {
				flashTimer = -1;
			}
		}
		if(tickTime != 0 && fmod(m_ftime, tickTime) / tickTime < 0.5f) {
			m_cColor = clrNormal;
//			m_cBGColor.set(0, 0, 0, 0);
			return false;
		} else {
			m_cColor = markColor;
//			m_cBGColor.set(flashBG_R, flashBG_G, flashBG_B, flashBG_A);
			return true;
		}
	} else {
		m_cColor = clrNormal;
//		m_cBGColor.set(0, 0, 0, 0);
		return false;
	}
}

int CShellWindow::HitTest(float xT, float yT)
{
	if((state & SQSH_VISIBLE) == 0 || (m_attr && m_attr->hitTestMode == HITTEST_NONE))
		return 0;
	float xF = xT * (float)terRenderDevice->GetSizeX();
	float yF = yT * (float)terRenderDevice->GetSizeY();

	return xF>=x && xF<=(x+sx) && yF>=y && yF<=(y+sy);
}
void CShellWindow::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
		return;

	m_ftime += frame_time.delta();

	if( (state & SQSH_VISIBLE) && m_hTexture ) // draw button
		DrawSprite(x, y, sx, sy, 
			m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, m_hTexture, clrNormal);

	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

bool CShellWindow::actionPerformed(InterfaceEventCode code, int param) {
	if (
			m_handler
		&&	(state & SQSH_ENABLED)
		&&	((state & SQSH_VISIBLE) || !checkHotKeyVisibility)	) {

		if (universe()) {
			universe()->checkEvent(EventClickOnButton( (ShellControlID)ID) );
		}

		m_handler(this, code, param);
		return true;
	}
	return false;
}

void CShellWindow::OnLButtonDown(float _x, float _y)
{
	if(m_handler)
	{
		if((canHandleDisabledInput() || state & SQSH_ENABLED) && m_effect == 0)
		{
			SND2DPlaySound( m_sound, x);
			m_handler(this, EVENT_PRESSED, 0);
		}
		else
			m_handler(this, EVENT_PRESSED_DISABLED, 0);
	}
}
void CShellWindow::OnLButtonUp(float _x, float _y)
{
	if(state & SQSH_ENABLED && m_effect == 0)
	{
		if(m_handler)
			m_handler(this, EVENT_UNPRESSED, VK_LBUTTON );

	}
}
void CShellWindow::OnLButtonDblClk(float _x, float _y)
{
	if(m_handler /*&& (state & SQSH_ENABLED)*/ && m_effect == 0)
		m_handler(this, EVENT_DOUBLECLICK, VK_LBUTTON );
}
void CShellWindow::OnLButtonHold()
{
	if(m_handler && (state & SQSH_ENABLED) && m_effect == 0)
		m_handler(this, EVENT_HOLD, 0);
}
void CShellWindow::OnRButtonDown(float _x, float _y)
{
	//SND2DPlaySound( m_sound, x );
	if(m_handler /*&& (state & SQSH_ENABLED)*/ && m_effect == 0) //нужен правый клик в задизабленые кнопки!
		m_handler(this, EVENT_RPRESSED, 0);
}
void CShellWindow::OnRButtonUp(float _x, float _y)
{
	if(m_handler /*&& (state & SQSH_ENABLED)*/ && m_effect == 0)// нужен правый клик в задизабленые кнопки!
		m_handler(this, EVENT_RUNPRESSED, 0);
}

void CShellWindow::Move(float _x, float _y, float _sx, float _sy)
{
//	if(_x >= 0) x = _x; y = _y;
//	sx = _sx; sy = _sy;
}

void CShellWindow::Show(int bShow)
{
	list<CShellWindow*>::iterator i;
	FOR_EACH(m_children, i)
		(*i)->Show(bShow);

	if(bShow) 
	{
		if(ID >= 0)
			state |= SQSH_VISIBLE;
	}
	else 
		state &= ~SQSH_VISIBLE;

	if(m_handler)
		m_handler(this, bShow ? EVENT_SHOWWND : EVENT_HIDEWND, 0);
}
void CShellWindow::Show(int bShow, int effect)
{
	list<CShellWindow*>::iterator i;
	FOR_EACH(m_children, i)
		(*i)->Show(bShow, effect);

	if (EffectSupported() & effect) {
		if(bShow) 
		{
			if(ID >= 0)
				state |= SQSH_VISIBLE;
		}
		else 
			state &= ~SQSH_VISIBLE;

		if(m_handler)
			m_handler(this, bShow ? EVENT_SHOWWND : EVENT_HIDEWND, 0);
	}
}
void CShellWindow::ClearEffect()
{
	list<CShellWindow*>::iterator i;
	FOR_EACH(m_children, i)
		(*i)->ClearEffect();
	m_effect = 0;
}
void CShellWindow::Enable(int bEnable)
{
	list<CShellWindow*>::iterator i;
	FOR_EACH(m_children, i)
		(*i)->Enable(bEnable);

	if(bEnable) 
		state |= SQSH_ENABLED;
	else 
		state &= ~SQSH_ENABLED;
}
void CShellWindow::OnWindow(int enable)
{
	if(m_handler)
		m_handler(this, EVENT_ON_WINDOW, enable);
}
void CShellWindow::OnFormatPopupMessage(char* cbBuffer)
{
	if(m_attr->popup[0] == '<')
		_shellIconManager.FormatMessageText(m_attr->popup, cbBuffer);
	else
		strcpy(cbBuffer, m_attr->popup);
}

void CShellWindow::OnFormatHotKeyPopupMessage(char* cbBuffer) {
	strcat(cbBuffer, hotKeyPopupString.c_str());
}

///////////////////////////
CComboWindow::CComboWindow(int id, CShellWindow* pParent, EVENTPROC p):CShellWindow(id, pParent, p)
{
	target=-1;
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont, comboBoxFontSize);
	m_fStringHeight = comboBoxRowHeight;

	pos = size = 0;
	if( m_handler )
		m_handler(this, EVENT_CREATEWND, 0);
}
CComboWindow::~CComboWindow()
{
	_RELEASE(m_hFont);
}
void CComboWindow::draw(int bFocus)
{
	if( !(state & SQSH_VISIBLE) ) return;
	m_ftime += frame_time.delta();

	float Alpha;

	if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
	{
		Alpha=0;
		float phase;
		if( m_effect==effectButtonsFadeIn )
			phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
		else
			phase = _shellIconManager.m_fEffectTime;
		if( phase<0 ) {
			Alpha=1.0f;
			if (!OnEffectStop(m_effect)) {
				return;
			}
		}
		else if( phase<=_fEffectButtonTime1 );
		else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
		else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
			Alpha=phase/_fEffectButtonTime3;
		else {
			Alpha=1.0f;
			if (!OnEffectStop(m_effect)) {
				return;
			}
		}
	}
	else 
		{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

	if (debug_show_intf_borders) {
		draw_rect_empty( Vect2i(x, y), Vect2i(x + sx,y + sy), sColor4f(1, 1, 0, 1) );
	}

	if( pos<0 || pos>=size ) return;

	sColor4f clr(1, 1, 1, Alpha);
	if ( !(state & SQSH_ENABLED) ) {
		clr *= pushButtonDisabled;
		clr.a = Alpha * pushButtonDisabledAlpha;
	}

	terRenderDevice->SetFont(m_hFont);
	int xx = (m_attr->txt_align == SHELL_ALIGN_CENTER) ? 0.5f*sx : 0;
	string toScr = getValidatedText(Array[pos], sx);
	if (m_hTexture) {
		terRenderDevice->OutText(
			x+xx, y+0.5f*sy-m_hFont->GetHeight()/2,
			(char*)toScr.c_str(), clr, m_attr->txt_align, ALPHA_ADDBLENDALPHA, m_hTexture, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
	} else {
		OutText( x+xx, y+0.5f*sy-m_hFont->GetHeight()/2, 
			(char*)toScr.c_str(), &clr, m_attr->txt_align );
	}
	terRenderDevice->SetFont(0);

	if( m_handler )
		m_handler(this, EVENT_DRAWWND, 0);
}
void CComboWindow::OnLButtonUp(float _x, float _y)
{
	if (state & SQSH_ENABLED) {
		if (!m_attr->passive) {
			pos++;
			if(pos>=size) pos=0;
		}
		if(m_handler)
			m_handler(this, EVENT_UNPRESSED, 0);
	}
}
void CComboWindow::OnRButtonUp(float _x, float _y)
{
	if (state & SQSH_ENABLED) {
		if (!m_attr->passive) {
			pos--;
			if(pos<0) pos=size-1;
		}
		if(m_handler)
			m_handler(this, EVENT_RUNPRESSED, 0);
	}
}
void CComboWindow::Load(const sqshControl* attr)
{
	CShellWindow::Load( attr );
	m_fStringHeight = absoluteY(comboBoxRowHeight);
	target = attr->target;
	if (m_hTexture) {
		float tx = absoluteX(m_attr->image.dx);
		float ty = absoluteY(m_attr->image.dy);
		float tsx = absoluteX(m_attr->image.dsx);
		float tsy = absoluteY(m_attr->image.dsy);

		uv.x = (x - tx) / tsx;
		uv.y = (y - ty) / tsy;
//		dudv.x =  (float)m_hTexture->GetWidth() / tsx;
//		dudv.y = (float)m_hTexture->GetHeight() / tsy;
		dudv.x = 1.0f / tsx;
		dudv.y = 1.0f / tsy;
	}
}
void CComboWindow::OnWindow(int enable)
{
	if( state & SQSH_ENABLED )
		if( target>0 )
		{
			CShellWindow *wnd=_shellIconManager.GetWnd(target);
			wnd->OnWindow(enable);
		}
}

CColorComboWindow::CColorComboWindow(int id, CShellWindow* pParent, EVENTPROC p):CShellWindow(id, pParent, p)
{
	target=-1;
	colors = new sColor4f[playerColorsSize];
	for (int i = 0; i < playerColorsSize; i++) {
		colors[i] = sColor4f(playerColors[i].unitColor[0], playerColors[i].unitColor[1], playerColors[i].unitColor[2], playerColors[i].unitColor[3]);
	}

	pos = 0;
	size = playerColorsSize;
	if( m_handler )
		m_handler(this, EVENT_CREATEWND, 0);
}
CColorComboWindow::~CColorComboWindow()
{
	delete [] colors;
	_RELEASE(m_hFont);
}
void CColorComboWindow::draw(int bFocus)
{
	if( !(state & SQSH_VISIBLE) ) return;
	m_ftime += frame_time.delta();

	float Alpha;

	if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
	{
		Alpha=0;
		float phase;
		if( m_effect==effectButtonsFadeIn )
			phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
		else
			phase = _shellIconManager.m_fEffectTime;
		if( phase<0 )
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		else if( phase<=_fEffectButtonTime1 );
		else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
		else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
			Alpha=phase/_fEffectButtonTime3;
		else
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
	}
	else 
		{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

	if( pos<0 || pos>=size ) return;
	sColor4f clr = colors[pos];
	clr.a *= Alpha * colorComboAlpha;
	draw_rect(Vect2f(x, y), Vect2f(x + sx, y + sy), clr);

	if( m_handler )
		m_handler(this, EVENT_DRAWWND, 0);
}
void CColorComboWindow::OnLButtonUp(float _x, float _y)
{
	if (!m_attr->passive) {
		pos++;
		if(pos>=size) pos=0;
	}
	if(m_handler && (state & SQSH_ENABLED))
		m_handler(this, EVENT_UNPRESSED, 0);
}
void CColorComboWindow::OnRButtonUp(float _x, float _y)
{
	if (!m_attr->passive) {
		pos--;
		if(pos<0) pos=size-1;
	}
	if(m_handler && (state & SQSH_ENABLED))
		m_handler(this, EVENT_RUNPRESSED, 0);
}
void CColorComboWindow::Load(const sqshControl* attr)
{
	CShellWindow::Load( attr );
	target = attr->target;
}
void CColorComboWindow::OnWindow(int enable)
{
	if( state & SQSH_ENABLED )
		if( target>0 )
		{
			CShellWindow *wnd=_shellIconManager.GetWnd(target);
			wnd->OnWindow(enable);
		}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//

CShellPushButton::CShellPushButton(int id, CShellWindow* pParent, EVENTPROC p):CShellWindow(id, pParent, p)
{
	target=-1;
	flag=unpressed;
}
CShellPushButton::~CShellPushButton()
{
}

void CShellPushButton::Load(const sqshControl* attr)
{
	CShellWindow::Load(attr);
	target = attr->target;
//	labelText = getTextFromBase(m_attr->text);
	string key("Interface.Menu.ButtonLabels.");
	key += m_attr->text;
	labelText = qdTextDB::instance().getText(key.c_str());

	txtdy = absoluteY(attr->txt_dy);

	if (m_hTexture) {
		float tx = absoluteX(m_attr->image.dx);
		float ty = absoluteY(m_attr->image.dy);
		float tsx = absoluteX(m_attr->image.dsx);
		float tsy = absoluteY(m_attr->image.dsy);

		uv.x = (x - tx) / tsx;
		uv.y = (y - ty + txtdy) / tsy;
//		dudv.x =  (float)m_hTexture->GetWidth() / tsx;
//		dudv.y = (float)m_hTexture->GetHeight() / tsy;
		dudv.x = 1.0f / tsx;
		dudv.y = 1.0f / tsy;
	}
}

void CShellPushButton::draw(int bFocus)
{
	if( !(state & SQSH_VISIBLE) ) return;
	m_ftime += frame_time.delta();

	float Alpha;

	if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
	{
		Alpha=0;
		float phase;
		if( m_effect==effectButtonsFadeIn )
			phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
		else
			phase = _shellIconManager.m_fEffectTime;
		if( phase<0 )
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		else if( phase<=_fEffectButtonTime1 );
		else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
		else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
			Alpha=phase/_fEffectButtonTime3;
		else
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
	}
	else 
		{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

	if (debug_show_intf_borders) {
		draw_rect_empty( Vect2i(x, y), Vect2i(x + sx,y + sy), sColor4f(1, 1, 0, 1) );
	}
	
	if(*m_attr->text)
	{
		cFont* font;
		switch(m_attr->font_group)
		{
		case 1:
			font = hFontMainmenu1;
			break;
		case 2:
			font = hFontMainmenu2;
			break;
		case 3:
			font = hFontMainmenu3;
			break;
		case 4:
			font = hFontMainmenu4;
			break;
		default:
			font = hFontMainmenu1;
		}

		terRenderDevice->SetFont(font);
		float centerY = y + sy / 2 - font->GetHeight() / 2;

		string toScr = getValidatedText(labelText, sx);

		Alpha *= pushButtonAlpha;
//		sColor4f clr(m_attr->flashR, m_attr->flashG, m_attr->flashB, 0);
		sColor4f clr(1, 1, 1, 0);
//		sColor4f clr(0.7f, 0.7f, 0.7f, Alpha);
		if( state & SQSH_ENABLED )
		{

			if (flag == pressed || flag == active) {
				clr = sColor4f(1, 1, 1);
				clr *= pushButtonPressedEnabled;
				clr.a = Alpha;
			} else {
				clr *= pushButtonNormalEnabled;
				clr.a = Alpha;
			}

			if (m_hTexture) {
				if (m_attr->txt_align == SHELL_ALIGN_CENTER) {
					terRenderDevice->OutText(x + sx / 2.0f, centerY, toScr.c_str(), clr, m_attr->txt_align, ALPHA_ADDBLENDALPHA, m_hTexture, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
				} else if (m_attr->txt_align == SHELL_ALIGN_RIGHT) {
					float txtWidth = terRenderDevice->GetFontLength(toScr.c_str());
					terRenderDevice->OutText(x + sx - txtWidth - 1, centerY, toScr.c_str(), clr, SHELL_ALIGN_LEFT, ALPHA_ADDBLENDALPHA, m_hTexture, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
				} else {
					terRenderDevice->OutText(x, centerY, toScr.c_str(), clr, m_attr->txt_align, ALPHA_ADDBLENDALPHA, m_hTexture, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
				}
			} else {
				if (m_attr->txt_align == SHELL_ALIGN_CENTER) {
					OutText(x + sx / 2.0f, centerY, toScr.c_str(), &clr, m_attr->txt_align);
				} else if (m_attr->txt_align == SHELL_ALIGN_RIGHT) {
					float txtWidth = terRenderDevice->GetFontLength(toScr.c_str());
					OutText(x + sx - txtWidth - 1, centerY, toScr.c_str(), &clr, SHELL_ALIGN_LEFT);
				} else {
					OutText(x, centerY, toScr.c_str(), &clr, m_attr->txt_align);
				}
			}
		}
		else {
			clr *= pushButtonDisabled;
			clr.a = Alpha * pushButtonDisabledAlpha;
			if (m_hTexture) {
				if (m_attr->txt_align == SHELL_ALIGN_CENTER) {
					terRenderDevice->OutText(x + sx / 2.0f, centerY, toScr.c_str(), clr, m_attr->txt_align, ALPHA_ADDBLENDALPHA, m_hTexture, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
				} else if (m_attr->txt_align == SHELL_ALIGN_RIGHT) {
					float txtWidth = terRenderDevice->GetFontLength(toScr.c_str());
					terRenderDevice->OutText(x + sx - txtWidth - 1, centerY, toScr.c_str(), clr, SHELL_ALIGN_LEFT, ALPHA_ADDBLENDALPHA, m_hTexture, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
				} else {
					terRenderDevice->OutText(x, centerY, toScr.c_str(), clr, m_attr->txt_align, ALPHA_ADDBLENDALPHA, m_hTexture, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
				}
			} else {
				if (m_attr->txt_align == SHELL_ALIGN_CENTER) {
					OutText(x + sx / 2.0f, centerY, toScr.c_str(), &clr, m_attr->txt_align);
				} else if (m_attr->txt_align == SHELL_ALIGN_RIGHT) {
					float txtWidth = terRenderDevice->GetFontLength(toScr.c_str());
					OutText(x + sx - txtWidth - 1, centerY, toScr.c_str(), &clr, SHELL_ALIGN_LEFT);
				} else {
					OutText(x, centerY, toScr.c_str(), &clr, m_attr->txt_align);
				}
			}
		}

		terRenderDevice->SetFont(0);
	}
	if( m_handler )
		m_handler(this, EVENT_DRAWWND, 0);
}
void CShellPushButton::OnLButtonDown(float _x, float _y)
{
	if( state & SQSH_ENABLED ) 
		flag=pressed;
	CShellWindow::OnLButtonDown(_x, _y);
	if( target>0 )
	{
		CShellWindow *wnd=_shellIconManager.GetWnd(target);
		wnd->OnLButtonDown(_x,_y);
	}
}
void CShellPushButton::OnLButtonUp(float _x, float _y)
{
	if( state & SQSH_ENABLED )	flag=unpressed;
	if( target>0 )
	{
		CShellWindow *wnd=_shellIconManager.GetWnd(target);
		if(wnd)
			wnd->OnLButtonUp(_x,_y);
	}
	CShellWindow::OnLButtonUp(_x, _y);
}
void CShellPushButton::OnRButtonDown(float _x, float _y)
{
	if( state & SQSH_ENABLED ) 
		flag=pressed;
	CShellWindow::OnRButtonDown(_x, _y);
	if( target>0 )
	{
		CShellWindow *wnd=_shellIconManager.GetWnd(target);
		if(wnd)
			wnd->OnLButtonDown(_x,_y);
	}
}
void CShellPushButton::OnRButtonUp(float _x, float _y)
{
	if( state & SQSH_ENABLED )	flag=unpressed;
	CShellWindow::OnRButtonUp(_x, _y);
	if( target>0 )
	{
		CShellWindow *wnd=_shellIconManager.GetWnd(target);
		if(wnd)
			wnd->OnLButtonUp(_x,_y);
	}
}
void CShellPushButton::OnWindow(int enable)
{
	if( state & SQSH_ENABLED )
		if ( enable ) {
			flag |= active;
			SND2DPlaySound( "menu_toggle");
		} else {
			flag &= ~active;
			flag &= ~pressed;
		}
}

//general wnd
void CGeneralWindow::draw(int bFocus)
{
	if( !(state & SQSH_VISIBLE) ) return;
	m_ftime += frame_time.delta();

	float Alpha;

	if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
	{
		Alpha=0;
		float phase;
		if( m_effect==effectButtonsFadeIn )
			phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
		else
			phase = _shellIconManager.m_fEffectTime;
		if( phase<0 )
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		else if( phase<=_fEffectButtonTime1 );
		else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
		else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
			Alpha=phase/_fEffectButtonTime3;
		else
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
	}
	else 
		{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

	if (m_attr->txt_dx) {
		Alpha *= m_attr->txt_dx; 
	}
	DrawSprite( x, y, sx, sy, 
		m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, 
		m_hTexture, sColor4c(255,255,255,Alpha*255), fmod(m_ftime,1000.f)/1000.f, ALPHA_ADDBLENDALPHA);

	if( m_handler )
		m_handler(this, EVENT_DRAWWND, 0);
}

extern int _id_on;
extern int _id_off;

void CWorldEffect::draw(int bFocus)
{
	m_ftime += frame_time.delta();
	if((state & SQSH_VISIBLE) == 0)
		return;

//	if( ( _id_on==SQSH_MM_BRIEFING_SCR && m_effect==effectButtonsFadeOut && _id_off==SQSH_MM_SCREEN2 ) ||
//		( _id_off==SQSH_MM_BRIEFING_SCR && m_effect==effectButtonsFadeIn && _id_on==SQSH_MM_SCREEN2 ) )
	if( ( _id_on==SQSH_MM_BRIEFING_SCR && m_effect==effectButtonsFadeOut ) ||
		( _id_off==SQSH_MM_BRIEFING_SCR && m_effect==effectButtonsFadeIn ) )
	{
		int Alpha=255,a=100,iColor=100*rand()/RAND_MAX;
		float phase,dt;
		if( _id_off==SQSH_MM_BRIEFING_SCR )
			dt = _shellIconManager.m_fEffectTime;
		else
			dt = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime,phase;
		if( dt<0 )
			{ phase=1; if (!OnEffectStop(m_effect)) return; }
		else if( dt<=_fEffectButtonTime3 )
			phase=1;
		else if( (dt-=_fEffectButtonTime3)<=_fEffectButtonTime2 )
			phase=1-dt/_fEffectButtonTime2, Alpha = round(255*phase), a=100+round(96*(1-phase));
		else if( (dt-=_fEffectButtonTime2)<=_fEffectButtonTime1 )
			phase=1-dt/_fEffectButtonTime1, Alpha = round(255*phase), a=100+round(155*(1-phase));
		else
			{ phase=1; if (!OnEffectStop(m_effect)) return; }
		
		DrawSprite2(x, y, sx, phase*sy, 0, 0, sx/128, sy/128, 
			m_hTexture3, m_hTexture2,  sColor4c(255,255,255,100), fmodf(m_ftime,250)/250 );

		DrawSprite2(x, y+phase*sy, sx, (1-phase)*sy, 0, 0, sx/128, sy/128, 
			m_hTexture3, m_hTexture2,  sColor4c(150-iColor,150-iColor,150-iColor,a), fmodf(m_ftime,250)/250 );
		DrawSprite(x, y+phase*sy, sx, 0.1f*sy, 0, 0, sx/128, 0.1f*sy/128, 
			m_hTexture3, sColor4c(196,196,196,Alpha), fmodf(m_ftime,300)/300 );
	}
//	else if( ( _id_on==SQSH_MM_BRIEFING_SCR && m_effect==effectButtonsFadeIn && _id_off==SQSH_MM_SCREEN2 ) ||
//		( _id_off==SQSH_MM_BRIEFING_SCR && m_effect==effectButtonsFadeOut && _id_on==SQSH_MM_SCREEN2 ) )
	else if( ( _id_on==SQSH_MM_BRIEFING_SCR && m_effect==effectButtonsFadeIn ) ||
		( _id_off==SQSH_MM_BRIEFING_SCR && m_effect==effectButtonsFadeOut ) )
	{
		int Alpha=255,a=0;
		float phase,dt;
		if( _id_on==SQSH_MM_BRIEFING_SCR )
			dt = _shellIconManager.m_fEffectTime;
		else
			dt = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime,phase;
		if( dt<0 )
			{ phase=1; if (!OnEffectStop(m_effect)) return; }
		else if( dt<=_fEffectButtonTime3 )
			phase=1;
		else if( (dt-=_fEffectButtonTime3)<=_fEffectButtonTime2 )
			phase=1-dt/_fEffectButtonTime2, Alpha = round(255*phase), a=0+round(128*(1-phase));
		else if( (dt-=_fEffectButtonTime2)<=_fEffectButtonTime1 )
			phase=1-dt/_fEffectButtonTime1, Alpha = round(255*phase), a=128+round(127*(1-phase));
		else
			{ phase=1; if (!OnEffectStop(m_effect)) return; }
		
		DrawSprite2(x, y, sx, sy, 0, 0, sx/128, sy/128, 
			m_hTexture3, m_hTexture2,  sColor4c(150,150,150,a), fmodf(m_ftime,250)/250 );
		DrawSprite(x, y+phase*sy, sx, 0.1f*sy, 0, 0, sx/128, 0.1f*sy/128, 
			m_hTexture3, sColor4c(196,196,196,Alpha), fmodf(m_ftime,300)/300 );
	}
//	else if( ID==SQSH_MM_SCREEN2 )
//		DrawSprite2(x, y, sx, sy, 0, 0, sx/128, sy/128, 
//			m_hTexture3, m_hTexture2, sColor4c(255,255,255,100), fmodf(m_ftime,250)/250 );

	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void CDialogWindow::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
		return;

	CShellWindow::draw(bFocus);

	if(*m_attr->text)
	{
		switch(m_attr->font_group)
		{
		case 1:
			terRenderDevice->SetFont(hFontMainmenu1);
			break;
		case 2:
			terRenderDevice->SetFont(hFontMainmenu2);
			break;
		case 3:
			terRenderDevice->SetFont(hFontMainmenu3);
			break;
		case 4:
			terRenderDevice->SetFont(hFontMainmenu4);
			break;

		default:
			terRenderDevice->SetFont(hFontMainmenu1);
		}

		if( state & SQSH_ENABLED )
			OutText(x+m_attr->txt_dx, y+m_attr->txt_dy, m_attr->text, &sColor4f(0.7f, 0.7f, 0.7f, 1.f), m_attr->txt_align);
		else
			OutText(x+m_attr->txt_dx, y+m_attr->txt_dy, m_attr->text, &sColor4f(0.3f, 0.3f, 0.3f, 1.f), m_attr->txt_align);

		terRenderDevice->SetFont(0);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

CShellComplexPushButton::CShellComplexPushButton(int id, CShellWindow* pParent, EVENTPROC p): CShellWindow(id, pParent, p)
{
	m_hTextureHighlight = 0;
	m_hTexturePressed = 0; 
	m_hTextureDisabled = 0; 

	m_pTextureHAttr = 0;
	m_pCheckAttr = 0;
	m_pDisabledAttr = 0;


	m_hTexture2 = 0; 
	m_hTexture2Highlight = 0; 
	m_hTexture2Pressed = 0; 
	m_hTexture2Disabled = 0; 

	m_pTexture2Attr = 0;
	m_pTexture2HAttr = 0;
	m_pCheck2Attr = 0;
	m_pDisabled2Attr = 0;

	m_hFontLabel = 0;
	memset(m_cb, 0, 5);
	m_label_x = m_label_y = 0;
	m_fphase = 0;
	m_ColorLabel.set(0,1,0,1);

	m_bChecked = false;
	m_bStatusNoEnergy = false;

	m_bPaused = false;

	flag = 0;
}
CShellComplexPushButton::~CShellComplexPushButton()
{
	_RELEASE(m_hTextureHighlight);
	_RELEASE(m_hTexturePressed);
	_RELEASE(m_hTextureDisabled);

	_RELEASE(m_hTexture2);
	_RELEASE(m_hTexture2Highlight);
	_RELEASE(m_hTexture2Pressed);
	_RELEASE(m_hTexture2Disabled);

	_RELEASE(m_hFontLabel);
}

void CShellComplexPushButton::Load(const sqshControl* attr)
{
	CShellWindow::Load(attr);

	_RELEASE(m_hTextureHighlight);
	_RELEASE(m_hTextureDisabled);
	_RELEASE(m_hTexturePressed);
	_RELEASE(m_hTexture2);
	_RELEASE(m_hTexture2Highlight);
	_RELEASE(m_hTexture2Disabled);
	_RELEASE(m_hTexture2Pressed);

	if (strlen(attr->image_h.texture)) {
		m_hTextureHighlight = terVisGeneric->CreateTexture(getImageFileName(&(attr->image_h)).c_str());
		m_pTextureHAttr = &attr->image_h;
		m_vTexHighPos = relativeUV(attr->image_h._ix, attr->image_h._iy, m_hTextureHighlight);
	} else {
		m_hTextureHighlight = terVisGeneric->CreateTexture(getImageFileName(&(attr->image)).c_str());
		m_pTextureHAttr = &attr->image;
		m_vTexHighPos = relativeUV(attr->image._ix, attr->image._iy, m_hTextureHighlight);
	}

	if (strlen(attr->image_disabled.texture)) {
		m_hTextureDisabled = terVisGeneric->CreateTexture(getImageFileName(&(attr->image_disabled)).c_str());
		m_pDisabledAttr = &attr->image_disabled;
		m_vTexDisabledPos = relativeUV(attr->image_disabled._ix, attr->image_disabled._iy, m_hTextureDisabled);
	} else {
		m_hTextureDisabled = terVisGeneric->CreateTexture(getImageFileName(&(attr->image)).c_str());
		m_pDisabledAttr = &attr->image;
		m_vTexDisabledPos = relativeUV(attr->image._ix, attr->image._iy, m_hTextureDisabled);
	}

	if (strlen(attr->image_check.texture)) {
		m_hTexturePressed = terVisGeneric->CreateTexture(getImageFileName(&(attr->image_check)).c_str());
		m_pCheckAttr = &attr->image_check;
		m_vTexCheckPos = relativeUV(attr->image_check._ix, attr->image_check._iy, m_hTexturePressed);
	} else {
		m_hTexturePressed = terVisGeneric->CreateTexture(getImageFileName(&(attr->image)).c_str());
		m_pCheckAttr = &attr->image;
		m_vTexCheckPos = relativeUV(attr->image._ix, attr->image._iy, m_hTexturePressed);
	}

	if (state & SQSH_CHECK2 || m_attr->flashWithImage) {
		m_hTexture2 = terVisGeneric->CreateTexture(getImageFileName(&(attr->image2)).c_str());
		m_pTexture2Attr = &attr->image2;
		m_vTex2Pos = relativeUV(attr->image2._ix, attr->image2._iy, m_hTexture2);
	}

	if (state & SQSH_CHECK2) {
		m_hTexture2Highlight = terVisGeneric->CreateTexture(getImageFileName(&(attr->image2_h)).c_str());
		m_pTexture2HAttr = &attr->image2_h;
		m_vTex2HighPos = relativeUV(attr->image2_h._ix, attr->image2_h._iy, m_hTexture2Highlight);

		if (strlen(attr->image2_disabled.texture)) {
			m_hTexture2Disabled = terVisGeneric->CreateTexture(getImageFileName(&(attr->image2_disabled)).c_str());
			m_pDisabled2Attr = &attr->image2_disabled;
			m_vTex2DisabledPos = relativeUV(attr->image2_disabled._ix, attr->image2_disabled._iy, m_hTexture2Disabled);
		} else {
			m_hTexture2Disabled = terVisGeneric->CreateTexture(getImageFileName(&(attr->image2)).c_str());
			m_pDisabled2Attr = &attr->image2;
			m_vTex2DisabledPos = relativeUV(attr->image2._ix, attr->image2._iy, m_hTexture2Disabled);
		}

		m_hTexture2Pressed = terVisGeneric->CreateTexture(getImageFileName(&(attr->image2_check)).c_str());
		m_pCheck2Attr = &attr->image2_check;
		m_vTex2CheckPos = relativeUV(attr->image2_check._ix, attr->image2_check._iy, m_hTexture2Pressed);
	}
	m_label_x = absoluteX(_buttonLabelX);
	m_label_y = absoluteY(_buttonLabelY);

	_RELEASE(m_hFontLabel);
	m_hFontLabel = terVisGeneric->CreateFont(sqshShellMainFont2, inGameLegionDigit);

	_RELEASE(m_hFont);
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont, inGameButtonFontSize);
}


void CShellComplexPushButton::scale(Vect2f& scaleV)
{
	CShellWindow::scale(scaleV);
	m_label_x *= scaleV.x;
	m_label_y *= scaleV.y;
}

void CShellComplexPushButton::OnLButtonDown(float _x, float _y)
{
	bool oldCheckedState = m_bChecked;
	CShellWindow::OnLButtonDown(_x, _y);
	if(state & SQSH_ENABLED ) 
	{
		flag |= pressed;
		if(!m_bChecked)
			m_ftime = 0;

		if (oldCheckedState == m_bChecked && (state & SQSH_SWITCH || state & SQSH_CHECK2)) {
			m_bChecked = !m_bChecked;
		}
	}
}
void CShellComplexPushButton::OnLButtonUp(float _x, float _y)
{
	if(!(state & SQSH_ENABLED))
		return;

	if (state & SQSH_SWITCH) {
		if (!m_bChecked) {
			flag &= ~pressed;
		}
	} else
		flag &= ~pressed;

	CShellWindow::OnLButtonUp(_x, _y);
}


void FormatTerraText1(char* cb, void* param)
{
	terUnitBase* pFrame = universe()->activePlayer()->frame();
	if (pFrame) {
		const terFrameStatisticsType& t = universe()->activePlayer()->GetFrameStats();
		_shellIconManager.FormatMessageText("<Terra_bar1>", cb, t.ZeroSquareRequest, t.ZeroSquareComplete);
		workAreaBtnForInfo->OnFormatHotKeyPopupMessage(cb);
	} else {
		_shellIconManager.FormatMessageText("<Terra_bar1>", cb, 0, 0);
		workAreaBtnForInfo->OnFormatHotKeyPopupMessage(cb);
	}
}
void FormatTerraText2(char* cb, void* param)
{
	terUnitBase* pFrame = universe()->activePlayer()->frame();
	if (pFrame) {
		const terFrameStatisticsType& t = universe()->activePlayer()->GetFrameStats();
		_shellIconManager.FormatMessageText("<Terra_bar2>", cb, t.AbyssSquareRequest, t.AbyssSquareComplete);
		workAreaBtnForInfo->OnFormatHotKeyPopupMessage(cb);
	} else {
		_shellIconManager.FormatMessageText("<Terra_bar2>", cb, 0, 0);
		workAreaBtnForInfo->OnFormatHotKeyPopupMessage(cb);
	}
}

void CShellComplexPushButton::Enable(int bEnable)
{
	CShellWindow::Enable(bEnable);

	if(!bEnable) {
		flag &= ~active;
		flag &= ~pressed;
	}
}

void CShellComplexPushButton::OnWindow(int enable)
{
	CShellWindow::OnWindow(enable);

	if(state & SQSH_ENABLED)
	{
		if( enable ) 
		{
			if(((flag & pressed) == 0  && !m_bStatusNoEnergy) || (state & SQSH_CHECK2))
			{
				flag |= active;
				SND2DPlaySound( "menu_toggle");
			}
		}
		else {
			flag &= ~active;
			if (!(state & SQSH_SWITCH) && !(state & SQSH_CHECK2)) {
				flag &= ~pressed;
			}
		}
	}
	else
	{
		if(!enable) {
			flag &= ~active;
			if (!(state & SQSH_SWITCH) && !(state & SQSH_CHECK2)) {
				flag &= ~pressed;
			}
		}
	}

	if(!terShowTips)
		return;

	if(ID == SQSH_WORKAREA3_ID || ID == SQSH_WORKAREA2_ID || ID == SQSH_WORKAREA4_ID)
	{
		CInfoWindow* pWnd;
		if(pWnd = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID))
		{
			pWnd->Show(enable);

			if(enable) {
				workAreaBtnForInfo = this;
				pWnd->SetText("<Terra_bar>", ID != SQSH_WORKAREA2_ID ? FormatTerraText1 : FormatTerraText2);
			}
		}
	}
}

void CShellComplexPushButton::draw(int bFocus)
{
	if(!(state & SQSH_VISIBLE)) 
		return;

	bool marked = updateColor();

//	if( flashBG_A != 0 && (state & SQSH_MARKED) )	{
//		draw_rect(Vect2i(x, y), Vect2i(x + sx, y + sy), m_cBGColor);
//	}

	bool bDiagramm = m_fphase > 0;

	if (isFlashing() && m_attr->flashWithImage && marked) {
		DrawSprite(x, y, sx, sy,
			m_vTex2Pos.x, m_vTex2Pos.y, m_vTexPos[1].x, m_vTexPos[1].y,
			m_hTexture2, clrNormal);
	} else if ((state & SQSH_ENABLED) || (flag & pressed)) {

		if (!(state & SQSH_CHECK2) || !m_bChecked) {
			if (flag & pressed) {
				DrawSprite(x + m_pCheckAttr->dx, y + m_pCheckAttr->dy, 
					sx +  m_pCheckAttr->dsx, sy + m_pCheckAttr->dsy,
					m_vTexCheckPos.x, m_vTexCheckPos.y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_hTexturePressed, m_cColor);
			} else if (flag & active) {
				DrawSprite(x + m_pTextureHAttr->dx, y + m_pTextureHAttr->dy, 
					sx +  m_pTextureHAttr->dsx, sy + m_pTextureHAttr->dsy,
					m_vTexHighPos.x, m_vTexHighPos.y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_hTextureHighlight, m_cColor);
			} else {
				DrawSprite(x, y, sx, sy,
					m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_hTexture, m_cColor);
			}
		} else {
			if (flag & pressed) {
				DrawSprite(x + m_pCheck2Attr->dx, y + m_pCheck2Attr->dy, 
					sx +  m_pCheck2Attr->dsx, sy + m_pCheck2Attr->dsy,
					m_vTex2CheckPos.x, m_vTex2CheckPos.y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_hTexture2Pressed, m_cColor);
			} else if (flag & active) {
				DrawSprite(x + m_pTexture2HAttr->dx, y + m_pTexture2HAttr->dy, 
					sx +  m_pTexture2HAttr->dsx, sy + m_pTexture2HAttr->dsy,
					m_vTex2HighPos.x, m_vTex2HighPos.y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_hTexture2Highlight, m_cColor);
			} else {
				DrawSprite(x, y, sx, sy,
					m_vTex2Pos.x, m_vTex2Pos.y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_hTexture2, m_cColor);
			}
		}

	} else { //disabled
		if (!(state & SQSH_CHECK2) || !m_bChecked) {
			if (*m_attr->image_disabled.texture) {
				DrawSprite(x, y, sx, sy,
					m_vTexDisabledPos.x, m_vTexDisabledPos.y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_hTextureDisabled, clrNormal);
			} else {
				DrawSprite(x, y, sx, sy,
					m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_hTexture, clrDisabled);
			}
		} else {
			if (*m_attr->image2_disabled.texture) {
				DrawSprite(x, y, sx, sy,
					m_vTex2DisabledPos.x, m_vTex2DisabledPos.y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_hTexture2Disabled, clrNormal);
			} else {
				DrawSprite(x, y, sx, sy,
					m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_hTexture2, clrDisabled);
			}
		}
	}

//	if(bDiagramm)
//		DrawDiagram(x + m_attr->diagram_delta_x, y + m_attr->diagram_delta_y, 
//		sx + m_attr->diagram_delta_sx, sy + m_attr->diagram_delta_sy, m_fphase);

	if (m_bPaused) {
		terRenderDevice->SetFont(m_hFont);
		OutText(x + sx/2, y+sy/2 - m_hFont->GetHeight()/2, _shellIconManager.hold.c_str(), &sColor4f(1,1,0,1), 0);
		terRenderDevice->SetFont(0);
	} else if (bDiagramm) {
		terRenderDevice->SetFont(m_hFontLabel);
		int ph = round(m_fphase * 100);
		char buff[11];
		sprintf(buff, "%d%%", ph);
		OutText(x + sx / 2, y + sy / 2 - m_hFontLabel->GetHeight() / 2, buff, &sColor4f(1, 1, 0, 1), 0);
		terRenderDevice->SetFont(0);
	}

	if(bDiagramm)
		draw_rect(Vect2i(x, y), Vect2i(x + int(m_fphase*sx) , y+sy), sColor4c(m_attr->diaR, m_attr->diaG, m_attr->diaB, m_attr->diaA));

	if (*m_cb) {
		terRenderDevice->SetFont(m_hFontLabel);

		float txtWidth = terRenderDevice->GetFontLength(m_cb);
		terRenderDevice->OutText(x + sx - txtWidth - 1, y+m_label_y, m_cb, m_ColorLabel, SHELL_ALIGN_LEFT);

		terRenderDevice->SetFont(0);
	}
}
extern void PopupFormatAttack(const AttributeBase* attr, char* cbBuffer, bool gun);

//pSquad=0 -> base unit
STARFORCE_API_NEW void FormatLegionPopup(const sqshControl* pAttr, char* cbBuffer, int nBtnID, SquadPageData* pSquad)
{
	terUnitAttributeID nAttrID = Button2LegionID(nBtnID);

	terPlayer* player = universe()->activePlayer();
	const AttributeBase* attrUnit = universe()->activePlayer()->unitAttribute(nAttrID);

	//необходимые строения
	string sRequired;
	const EnableData& mutationElement = player->GetMutationElement(nAttrID);
	const AttributeLegionary& attrL = *safe_cast<const AttributeLegionary*>(player->unitAttribute(nAttrID));
	int num = attrL.EnableStructure.size();
	for(int j = 0; j < num; j++){
		const AttributeBase* attr = player->unitAttribute(attrL.EnableStructure[j]);
		EnableData& evolution = player->GetEvolutionBuildingData(attr->ID);

		if(evolution.Worked)
			sRequired += "&00FF00";
		else
			sRequired += "&FF0000";
		sRequired += attr->interfaceName();

		if(j != num - 1)
			sRequired += "\n  ";
	}

	xassert(!sRequired.empty());

	static char cbTemp[256];
	PopupFormatAttack(attrUnit, cbTemp, false);
	if(pSquad){
		//наличие базовых юнитов
		DamageMolecula damage_molecula(player->unitAttribute(nAttrID)->damageMolecula);
		bool bSold = pSquad->soldierButton.val >= damage_molecula[0];
		bool bOff  = pSquad->officerButton.val >= damage_molecula[1];
		bool bTech = pSquad->technicButton.val >= damage_molecula[2];

		int count_possible=0;
		int idx_id=nAttrID- UNIT_ATTRIBUTE_SOLDIER - MUTATION_ATOM_MAX;
		if(idx_id>=0 && idx_id<sizeof(pSquad->mutationButtons)/sizeof(pSquad->mutationButtons[0]))
		{
			count_possible=pSquad->mutationButtons[idx_id].count;
		}else
		{
			xassert(0);
			//pSquad->countPossibleUnits(nAttrID)
		}

		_shellIconManager.FormatMessageText(pAttr->popup, cbBuffer, 
			attrUnit->interfaceName(), sRequired.c_str(),
			bSold ? "&00FF00" : "&FF0000", damage_molecula[0],
			bOff  ? "&00FF00" : "&FF0000", damage_molecula[1],
			bTech ? "&00FF00" : "&FF0000", damage_molecula[2], count_possible, attrUnit->interfaceName(),
			cbTemp);
	}
	else{
		_shellIconManager.FormatMessageText(pAttr->popup, cbBuffer, 
			attrUnit->interfaceName(), sRequired.c_str(), attrUnit->buildEnergy(), cbTemp);
	}
}
void CShellComplexPushButton::OnFormatPopupMessage(char* cbBuffer)
{
	if (*(m_attr->popupChecked)) {
		if (m_bChecked) {
			_shellIconManager.FormatMessageText(m_attr->popupChecked, cbBuffer);
		} else {
			_shellIconManager.FormatMessageText(m_attr->popup, cbBuffer);
		}
		return;
	}
	terUnitAttributeID sid = Button2StructureID(ID);
	if(sid == UNIT_ATTRIBUTE_NONE)
	{
		sid = Button2LegionID(ID);
		if(sid == UNIT_ATTRIBUTE_NONE) {
			CShellWindow::OnFormatPopupMessage(cbBuffer);
			return;
		}
	}

	const AttributeBase* attr = universe()->activePlayer()->unitAttribute(sid);

	#ifdef _DEMO_
		if (isForbidden(sid)) {
			_shellIconManager.FormatMessageText("<unit_derived_inaccess>", cbBuffer, attr->interfaceName());
			return;
		}
	#endif

	switch(ID)
	{
	case SQSH_YADRO_ID:
	case SQSH_YADRO_EX_ID:
	case SQSH_RELAY_ID:
	case SQSH_COMMANDER_ID:
	case SQSH_SOLDIER_PLANT_ID:
	case SQSH_OFFICER_PLANT_ID:
	case SQSH_TECHNIC_PLANT_ID:
	case SQSH_STATION1_ID:
	case SQSH_STATION2_ID:
	case SQSH_STATION3_ID:
	case SQSH_STATION4_ID:
	case SQSH_STATION5_ID:
	case SQSH_STATION_ELECTRO_LAB_ID:
	case SQSH_STATION_EXODUS_LAB_ID:
	case SQSH_STATION_EMPIRE_LAB_ID:
	case SQSH_STATION_HARKBACK_LAB_ID:
	case SQSH_GUN_LASER_ID:
	case SQSH_GUN_ELECTRO_ID:
	case SQSH_GUN_ROCKET_ID:
	case SQSH_GUN_HOWITZER_ID:
	case SQSH_GUN_FILTH_ID:
	case SQSH_GUN_GIMLET_ID:
	case SQSH_GUN_SUBCHASER_ID:
	case SQSH_GUN_BALLISTIC_ID:
	case SQSH_STATIC_BOMB_ID:
	case SQSH_CORRIDOR_ALPHA_ID:
	case SQSH_CORRIDOR_OMEGA_ID:
		_shellIconManager.FormatUnitPopup(attr, cbBuffer, true, 0);
		break;

	case SQSH_SOLDIER_ID:
	case SQSH_OFFICER_ID:
	case SQSH_TECHNIC_ID:
		FormatLegionPopup(m_attr, cbBuffer, ID, 0);
		break;
	case SQSH_SQUAD_UNIT1:
	case SQSH_SQUAD_UNIT2:
	case SQSH_SQUAD_UNIT3:
	case SQSH_SQUAD_UNIT4:
	case SQSH_SQUAD_UNIT5:
	case SQSH_SQUAD_UNIT6:
	case SQSH_SQUAD_UNIT7:
	case SQSH_SQUAD_UNIT8:
	case SQSH_SQUAD_UNIT9:
	case SQSH_SQUAD_UNIT10:
	case SQSH_SQUAD_UNIT11:
	case SQSH_SQUAD_UNIT12:
	case SQSH_SQUAD_UNIT13:
	case SQSH_SQUAD_UNIT14:
	case SQSH_SQUAD_UNIT15:
	case SQSH_SQUAD_UNIT16:
	case SQSH_SQUAD_UNIT17:
	case SQSH_SQUAD_UNIT18:
	case SQSH_SQUAD_UNIT19:
	case SQSH_SQUAD_UNIT20:
	case SQSH_SQUAD_UNIT21:
	case SQSH_SQUAD_UNIT22:
	case SQSH_SQUAD_UNIT23:
	case SQSH_SQUAD_UNIT24:
	case SQSH_SQUAD_UNIT25:
		{
			CUITabSheet* pSquadSheet = (CUITabSheet*)_shellIconManager.GetWnd(SQSH_TAB_SQUAD_ID);
			//terUnitSquad* pSquad = GetSquadByNumber(pSquadSheet->GetActivePage());
			int nActivePage=pSquadSheet->GetActivePage();
			SquadPageData* pSquad=NULL;
			if(nActivePage>=0 && nActivePage<5)
			{
				pSquad = &(gameShell->getLogicUpdater().getLogicData()->squads[nActivePage]);
			}

			if(pSquad)
				FormatLegionPopup(m_attr, cbBuffer, ID, pSquad);
			else
				*cbBuffer = 0;
		}
		break;


	default:
		if (attr) {
			//EnableData& data = universe()->activePlayer()->GetEvolutionBuildingData(sid);
			//if(data.hasDowngrade()) {
			//	AttributeBase* downAttr = universe()->activePlayer()->unitAttribute(data.downgrade());
			//	_shellIconManager.FormatMessageText(m_attr->popup, cbBuffer, "", attr->buildEnergy() - downAttr->buildEnergy());
			//} else {
				//Omega Hint
				float buildEnergy = attr->buildEnergy();
				if(attr->isUpgrade)
                    buildEnergy -= universe()->activePlayer()->unitAttribute(safe_cast<const AttributeBuilding*>(attr)->Downgrades.front())->buildEnergy();
				_shellIconManager.FormatMessageText(m_attr->popup, cbBuffer, "", buildEnergy);
			//}
		} else {
			CShellWindow::OnFormatPopupMessage(cbBuffer);
		}
	}
}


void CShellComplexPushButton::SetCheck(bool b)
{
	m_bChecked = b;
	if (!(state & SQSH_CHECK2)) {
		if(b)
			flag |= pressed;
		else
			flag &= ~pressed;
	}
}

void CShellComplexPushButton::SetValue(int val, bool bCanBuildNow)
{
	if(val)
	{
		_itoa(val, m_cb, 10);

		if(bCanBuildNow)
			m_ColorLabel.set(0,1,0,1);
		else
			m_ColorLabel.set(1,0,0,1);
	}
	else
		*m_cb = '\0';
}
void CShellComplexPushButton::SetProgress(float v)
{
	m_fphase = v;
}

void CShellComplexPushButton::Pause(bool bOn)
{
	m_bPaused = bOn;
}
bool CShellComplexPushButton::GetPause()
{
	return m_bPaused;
}
bool CShellComplexPushButton::CanPause()
{
	return *m_cb != 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
const AttributeBase* _GetUnitAttribute(int nBtnID)
{
	terUnitAttributeID unitID = Button2LegionID(nBtnID);
	if (unitID != UNIT_ATTRIBUTE_NONE) {
		return universe()->activePlayer()->unitAttribute(unitID);
	} else {
		return 0;
	}
}
CShellLegionButton::CShellLegionButton(int id, CShellWindow* pParent, EVENTPROC p) : CShellComplexPushButton(id, pParent, p)
{
	m_hFontLabel = 0;

	*m_cbTotal = 0;

	m_bS = m_bO = m_bT = true;
}
CShellLegionButton::~CShellLegionButton()
{
	_RELEASE(m_hFontLabel);
}

void CShellLegionButton::Load(const sqshControl* attr)
{
	CShellComplexPushButton::Load(attr);

	_RELEASE(m_hFontLabel);
	m_hFontLabel = terVisGeneric->CreateFont(sqshShellMainFont2, inGameLegionDigit);
}

static sColor4f sColorGreen(0, 1, 1, 1);
static sColor4f sColorRed(1, 0, 0, 1);
void CShellLegionButton::draw(int bFocus)
{
	CShellComplexPushButton::draw(bFocus);

	if((state & SQSH_VISIBLE) &&  (state & SQSH_ENABLED) && !m_effect)
	{
		terRenderDevice->SetFont(m_hFontLabel);
//		OutText(x+_button_atom1_x, y+_button_atom1_y, m_cb1, m_bS ? &sColorGreen : &sColorRed);
//		OutText(x+_button_atom2_x, y+_button_atom2_y, m_cb2, m_bO ? &sColorGreen : &sColorRed);
//		OutText(x+_button_atom3_x, y+_button_atom3_y, m_cb3, m_bT ? &sColorGreen : &sColorRed);

		terRenderDevice->DrawRectangle(x + 1, y, terRenderDevice->GetFontLength(m_cbTotal), sy * _button_atom5_y + 1, sColor4c(0, 0, 0, 196));
		terRenderDevice->FlushPrimitive2D();

		OutText(x + 1, y, m_cbTotal, &sColorGreen);

		terRenderDevice->SetFont(0);
	}
}
void CShellLegionButton::SetTotalUnitCount(int count, bool append)
{	
	*m_cbTotal = 0;
	if (append && count >= 0) {
		sprintf(m_cbTotal, "+%d", count);
	} else {
		sprintf(m_cbTotal, "%d", count);
	}
//	itoa(count, tot, 10);
//	*m_cbTotal = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//

CShellAtomButton::CShellAtomButton(int id, CShellWindow* pParent, EVENTPROC p) : CShellComplexPushButton(id, pParent, p)
{
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont, defaultFontSize);
	*m_cb1 = 0;
	m_bTextLeft = false;
	button_atom_x = 0;
	button_atom_y = 0;
}
CShellAtomButton::~CShellAtomButton()
{
	_RELEASE(m_hFont);
}

void CShellAtomButton::Load(const sqshControl* attr)
{
	CShellComplexPushButton::Load(attr);

	button_atom_x = absoluteX(_button_atom4_x);
	button_atom_y = absoluteY(_button_atom4_y);
}

void CShellAtomButton::draw(int bFocus)
{
	CShellComplexPushButton::draw(bFocus);

	if(state & SQSH_VISIBLE)
	{
		if(*m_cb1)
		{
			terRenderDevice->SetFont(m_hFontLabel);
			if (m_bTextLeft) {
				terRenderDevice->DrawRectangle(x + 1, y, terRenderDevice->GetFontLength(m_cb1), sy * _button_atom5_y + 1, sColor4c(0, 0, 0, 196));
				terRenderDevice->FlushPrimitive2D();
				OutText(x + 1, y, m_cb1, &sColor4f(0,1,1,1));
			} else {
				OutText(x+button_atom_x, y+button_atom_y, m_cb1, &sColor4f(1,1,0,1));
			}
			terRenderDevice->SetFont(0);
		}
	}
}
void CShellAtomButton::SetVal(int v, bool bTextLeft)
{
	if(v == 0)
		*m_cb1 = 0;
	else
		_itoa(v, m_cb1, 10);
	m_bTextLeft = bTextLeft;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
CTerrainBuildButton::CTerrainBuildButton(int id, CShellWindow* pParent, EVENTPROC p) : CShellComplexPushButton(id, pParent, p)
{
	m_bPointingLeft = false;
	partDisable = 0;
}
CTerrainBuildButton::~CTerrainBuildButton()
{
}

void CTerrainBuildButton::Load(const sqshControl* attr)
{
	CShellWindow::Load(attr);
	half_sx = absoluteX(half_sx_);
	right_x = absoluteX(right_x_);

	activeHalfBrig_uv = relativeUV(activeHalfBrig_u, activeHalfBrig_v, m_hTexture);
	activeHalfTerr_uv = relativeUV(activeHalfTerr_u, activeHalfTerr_v, m_hTexture);

	flashingHalfBrig_uv = relativeUV(flashingHalfBrig_u, flashingHalfBrig_v, m_hTexture);
	flashingHalfTerr_uv = relativeUV(flashingHalfTerr_u, flashingHalfTerr_v, m_hTexture);

	halfBrig_uv = relativeUV(halfBrig_u, halfBrig_v, m_hTexture);
	halfTerr_uv = relativeUV(halfTerr_u, halfTerr_v, m_hTexture);

	half_dudv = relativeUV(half_du, half_dv, m_hTexture);

	brig_uv = relativeUV(brig_u, brig_v, m_hTexture);
	terr_uv = relativeUV(terr_u, terr_v, m_hTexture);

	pressedBrig_uv = relativeUV(pressedBrig_u, pressedBrig_v, m_hTexture);
	pressedTerr_uv = relativeUV(pressedTerr_u, pressedTerr_v, m_hTexture);

	activeBrig_uv = relativeUV(activeBrig_u, activeBrig_v, m_hTexture);
	activeTerr_uv = relativeUV(activeTerr_u, activeTerr_v, m_hTexture);

	whole_dudv = relativeUV(whole_du, whole_dv, m_hTexture);
}

void CTerrainBuildButton::draw(int bFocus)
{
//	if((state & SQSH_VISIBLE) == 0)
//		return;

//	m_ftime += frame_time.delta();

	bool marked = updateColor();
	if(state & SQSH_VISIBLE)
	{
		if (!unitReady) {
//			int w1 = halfBrig_sx;
//			int w2 = gap;

			if (isFlashing() && marked) {
				if (partDisable != 1) {
					//normal half brigadier
					DrawSprite(x, y, half_sx, sy,
						flashingHalfBrig_uv.x, flashingHalfBrig_uv.y, half_dudv.x, half_dudv.y,
						m_hTexture, clrNormal);
				}
				if (partDisable != 2) {
					//normal half terrain master
					DrawSprite(right_x, y, half_sx, sy,
						flashingHalfTerr_uv.x, flashingHalfTerr_uv.y, half_dudv.x, half_dudv.y,
						m_hTexture, clrNormal);
				}
			} else if (flag & active && (productionPhase == 0 || productionPhase == 1.0f)) {
				if (m_bPointingLeft) {
					if (partDisable != 1) {
						//active half brigadier
						DrawSprite(x, y, half_sx, sy,
							activeHalfBrig_uv.x, activeHalfBrig_uv.y, half_dudv.x, half_dudv.y,
							m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
					}
					if (partDisable != 2) {
						//normal half terrain master
						DrawSprite(right_x, y, half_sx, sy,
							halfTerr_uv.x, halfTerr_uv.y, half_dudv.x, half_dudv.y,
							m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
					}
				} else {
					if (partDisable != 1) {
						//normal half brigadier
						DrawSprite(x, y, half_sx, sy,
							halfBrig_uv.x, halfBrig_uv.y, half_dudv.x, half_dudv.y,
							m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
					}
					if (partDisable != 2) {
						//active half terrain master
						DrawSprite(right_x, y, half_sx, sy,
							activeHalfTerr_uv.x, activeHalfTerr_uv.y, half_dudv.x, half_dudv.y,
							m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
					}
				}
			} else {
				if (partDisable != 1) {
					//normal half brigadier
					DrawSprite(x, y, half_sx, sy,
						halfBrig_uv.x, halfBrig_uv.y, half_dudv.x, half_dudv.y,
						m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
				}
				if (partDisable != 2) {
					//normal half terrain master
					DrawSprite(right_x, y, half_sx, sy,
						halfTerr_uv.x, halfTerr_uv.y, half_dudv.x, half_dudv.y,
						m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
				}
			}

			//диаграмма производства вещ-ва

			if (partDisable != 1) {
				draw_rect(Vect2i(x, y), Vect2i(x + int(productionPhase * half_sx), y + sy), sColor4c(255, 255, 0, 64));
			}
			if (partDisable != 2) {
				draw_rect(Vect2i(right_x, y), Vect2i(right_x + int(productionPhase * half_sx), y + sy), sColor4c(255, 255, 0, 64));
			}
		} else {
//			if ( flashBG_A != 0 && (state & SQSH_MARKED) ) {
//				draw_rect(Vect2i(x, y), Vect2i(x + sx, y + sy), sColor4c(flashBG_R, flashBG_G, flashBG_B, flashBG_A));
//			}

			if (isBrig) {
				if (flag & pressed) {
					//pressed terrain master
					DrawSprite(x, y, sx, sy, pressedTerr_uv.x, pressedTerr_uv.y, whole_dudv.x, whole_dudv.y, 
						m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
				} else if (flag & active && ((productionPhase == 0 || productionPhase == 1.0f))) {
					//active terrain master
					DrawSprite(x, y, sx, sy, activeTerr_uv.x, activeTerr_uv.y, whole_dudv.x, whole_dudv.y, 
						m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
				} else {
					DrawSprite(x, y, sx, sy, terr_uv.x, terr_uv.y, whole_dudv.x, whole_dudv.y, 
						m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
				}
			} else {
				if (flag & pressed) {
					//pressed brigadier
					DrawSprite(x, y, sx, sy, pressedBrig_uv.x, pressedBrig_uv.y, whole_dudv.x, whole_dudv.y, 
						m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
				} else if (flag & active && ((productionPhase == 0 || productionPhase == 1.0f))) {
					//active brigadier
					DrawSprite(x, y, sx, sy, activeBrig_uv.x, activeBrig_uv.y, whole_dudv.x, whole_dudv.y, 
						m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
				} else {
					DrawSprite(x, y, sx, sy, brig_uv.x, brig_uv.y, whole_dudv.x, whole_dudv.y, 
						m_hTexture, state & SQSH_ENABLED ? m_cColor : clrDisabled);
				}
			}
		}
	}
}
int CTerrainBuildButton::HitTest(float _x, float _y)
{
	m_bPointingLeft = _x * (float)terRenderDevice->GetSizeX() < x + sx/2;

	return CShellComplexPushButton::HitTest(_x, _y);
}

void CTerrainBuildButton::OnFormatPopupMessage(char* cbBuffer)
{
	if(productionPhase == 0)
		_shellIconManager.FormatMessageText("<Frame_slot_empty>", cbBuffer);
	else if(unitReady)
	{
		if(isBrig)
			_shellIconManager.FormatMessageText("<Prorab>", cbBuffer);
		else
			_shellIconManager.FormatMessageText("<Brigadier>", cbBuffer);
	} else if (productionPhase >= 1) {
		m_bPointingLeft = _pShellDispatcher->m_fMouseCurrentX * (float)terRenderDevice->GetSizeX() < x + sx/2;
		if (m_bPointingLeft && partDisable != 1) {
			_shellIconManager.FormatMessageText("<Brigadier_slot>", cbBuffer);
		} else if (!m_bPointingLeft && partDisable != 2) {
			_shellIconManager.FormatMessageText("<Prorab_slot>", cbBuffer);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
//

CUITabSheet::CUITabSheet(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p)
{
	m_nActivePage = 0;
//	m_nEnabledPagesBits = 0xFFFFFFFF;
	m_nVisiblePagesBits = 0;
	m_nEnabledPagesBits = 0x0000001F;
	m_nSelectedPagesBits = 0;
	m_bLabelHover = false;
	m_fphase = 0;

	m_nTabHover = -1;

	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont2, inGameLegionDigit);

	flashTimers.resize(5, -1);
	m_squad_icon_list.resize(5);
	m_page_numbers.resize(5);
	tabXs.resize(5);
	tabYs.resize(5);
	tabSXs.resize(5);
	tabSYs.resize(5);
	tabActiveXs.resize(5);
	tabActiveYs.resize(5);
	tabActiveSXs.resize(5);
	tabActiveSYs.resize(5);

	tabTextures.resize(5, 0);
	tabHighlightTextures.resize(5, 0);
	tabCheckedTextures.resize(5, 0);
	tabDisabledTextures.resize(5, 0);
	tabFlashingTextures.resize(5, 0);
	tabActiveTextures.resize(5, 0);

	tabActiveUVs.resize(5);
	tabActiveDUDVs.resize(5);
	tabUVs.resize(5);
	tabDUDVs.resize(5);
	tabHighUVs.resize(5);
	tabHighDUDVs.resize(5);
	tabCheckedUVs.resize(5);
	tabCheckedDUDVs.resize(5);
	tabDisabledUVs.resize(5);
	tabDisabledDUDVs.resize(5);
	tabFlashingUVs.resize(5);
	tabFlashingDUDVs.resize(5);

}

void CUITabSheet::postLoad() {
	for (int i = 0; i < m_attr->pages.size(); i++) {
		const sqshTabElement& page = m_attr->pages[i];
		for(int j = 0; j < page.ctrls.size(); j++) {
			if (page.ctrls[j] < 0) {
				break;
			}
			CShellWindow* wnd = _shellIconManager.GetWnd(page.ctrls[j]);
			if (wnd) {
				wnd->postLoad();
			}
		}
	}

}

CUITabSheet::~CUITabSheet()
{
	vector<cTexture*>::iterator it;

	FOR_EACH(tabTextures, it)
		_RELEASE(*it);
	FOR_EACH(tabHighlightTextures, it)
		_RELEASE(*it);
	FOR_EACH(tabCheckedTextures, it)
		_RELEASE(*it);
	FOR_EACH(tabDisabledTextures, it)
		_RELEASE(*it);
	FOR_EACH(tabFlashingTextures, it)
		_RELEASE(*it);
	FOR_EACH(tabActiveTextures, it)
		_RELEASE(*it);

	_RELEASE(m_hFont);
}

int CUITabSheet::HitTestTabHeader(float fx, float fy)
{
	float xF = fx * (float)terRenderDevice->GetSizeX();
	float yF = fy * (float)terRenderDevice->GetSizeY();
	for(int i = 0, s = tabAttrs.size(); i < s; i++)	{
		if (xF >= tabXs[i] && xF <= (tabXs[i] + tabSXs[i]) && yF >= tabYs[i] && yF <= (tabYs[i] + tabSYs[i])) {
			return i;
		}
	}

	return -1;
}
void CUITabSheet::SwitchPage(int nNewPage, bool bForceSelectUnit)
{
	int i;
	CShellWindow* pWnd;

	if(m_nActivePage == nNewPage)
		return;

	if((m_nEnabledPagesBits & (1 << nNewPage)) == 0)
		return;

	//спрятать контролы деактивируемой страницы
	const sqshTabElement* pOldPageAttr = tabAttrs[m_nActivePage];
	for(i=0; i<pOldPageAttr->ctrls.size(); i++)
	{
		if(pOldPageAttr->ctrls[i] < 0)
			break;

		if(pWnd = _shellIconManager.GetWnd(pOldPageAttr->ctrls[i]))
			pWnd->Show(false);
	}


	//показать контролы активируемой страницы
	const sqshTabElement* pNewPageAttr = tabAttrs[nNewPage];
	for(i=0; i<pNewPageAttr->ctrls.size(); i++)
	{
		if(pNewPageAttr->ctrls[i] < 0)
			break;

		if(pWnd = _shellIconManager.GetWnd(pNewPageAttr->ctrls[i]))
			pWnd->Show(true);
	}

	m_nActivePage = nNewPage;

	if(m_handler && bForceSelectUnit)
		m_handler(this, EVENT_TAB_CHANGED, m_nActivePage);
}

void CUITabSheet::setActivePageChildrenVisible(bool visible) {
	const sqshTabElement* aPageAttr = tabAttrs[m_nActivePage];
	CShellWindow* pWnd;
	for (int i = 0; i < aPageAttr->ctrls.size(); i++) {
		if (aPageAttr->ctrls[i] < 0) {
			break;
		}

		if ( pWnd = _shellIconManager.GetWnd(aPageAttr->ctrls[i]) ) {
			pWnd->Show(visible);
		}
	}
}

void CUITabSheet::SetSquadIcons(int n, SQUAD_ICON* pSqIcon)
{
	xassert(m_squad_icon_list.size() >= n+1);

	m_squad_icon_list[n] = pSqIcon;
}
void CUITabSheet::SetPageNumber(int nPage, int nNumber)
{
	if(nNumber > 0)
	{
		char _cb[10];
		m_page_numbers[nPage] = _itoa(nNumber, _cb, 10);
	}
	else
		m_page_numbers[nPage].erase();
}

void CUITabSheet::createHotKeyString() {
	hotKeyPopupString = "";
	char cbBuffer[200];
	for (int i = 0; i < m_attr->actions.size(); i++) {
		if (strlen(m_attr->actions[i].name) && strlen(m_attr->actions[i].hotKeyPopup)) {
			_shellIconManager.FormatMessageText(
				m_attr->actions[i].hotKeyPopup,
				cbBuffer,
				gameShell->hotKeyManager->getKeyNameForControlAction(m_attr->actions[i]).c_str()
			);
			hotKeyPopupString += cbBuffer;
		}
	}
}

void CUITabSheet::reload() {
	if (m_attr) {
		Load(m_attr);
	}
}

void CUITabSheet::Load(const sqshTabSheet* attr)
{
	vector<cTexture*>::iterator it;

	FOR_EACH(tabTextures, it)
		_RELEASE(*it);
	FOR_EACH(tabHighlightTextures, it)
		_RELEASE(*it);
	FOR_EACH(tabCheckedTextures, it)
		_RELEASE(*it);
	FOR_EACH(tabDisabledTextures, it)
		_RELEASE(*it);
	FOR_EACH(tabFlashingTextures, it)
		_RELEASE(*it);
	FOR_EACH(tabActiveTextures, it)
		_RELEASE(*it);

	_RELEASE(m_hFont);
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont2, inGameLegionDigit);

	
	m_attr = attr;
	createHotKeyString();
	loadFlashingParams(attr);
	markColor.set(flashR, flashG, flashB, flashA);

	for (int i = 0; i < attr->pages.size(); i++) {
		const sqshTabElement& page = attr->pages[i];
		tabAttrs.push_back(&page);
		tabTextures[i] = terVisGeneric->CreateTexture(getImageFileName(&(page.icon.image)).c_str());
		tabHighlightTextures[i] = terVisGeneric->CreateTexture(getImageFileName(&(page.icon.image_h)).c_str());
		tabCheckedTextures[i] = terVisGeneric->CreateTexture(getImageFileName(&(page.icon.image_check)).c_str());
		if (strlen(page.icon.image_disabled.texture)) {
			tabDisabledTextures[i] = terVisGeneric->CreateTexture(getImageFileName(&(page.icon.image_disabled)).c_str());
		} else {
			tabDisabledTextures[i] = 0;
		}
		if (strlen(page.icon.image_flashing.texture)) {
			tabFlashingTextures[i] = terVisGeneric->CreateTexture(getImageFileName(&(page.icon.image_flashing)).c_str());
		} else {
			tabFlashingTextures[i] = 0;
		}
		tabXs[i] = absoluteX(page.x);
		tabYs[i] = absoluteY(page.y);
		tabSXs[i] = absoluteX(page.sx);
		tabSYs[i] = absoluteY(page.sy);
		tabActiveTextures[i] = terVisGeneric->CreateTexture(getImageFileName(&(page.activeTabImage)).c_str());
		tabActiveXs[i] = absoluteX(page.activeX);
		tabActiveYs[i] = absoluteY(page.activeY);
		tabActiveSXs[i] = absoluteX(page.activeSX);
		tabActiveSYs[i] = absoluteY(page.activeSY);

		tabActiveUVs[i] = relativeUV(page.activeTabImage._ix, page.activeTabImage._iy, tabActiveTextures[i]);
		tabActiveDUDVs[i] = relativeUV(page.activeTabImage.ix, page.activeTabImage.iy, tabActiveTextures[i]);

		tabUVs[i] = relativeUV(page.icon.image._ix, page.icon.image._iy, tabTextures[i]);
		tabDUDVs[i] = relativeUV(page.icon.image.ix, page.icon.image.iy, tabTextures[i]);

		tabHighUVs[i] = relativeUV(page.icon.image_h._ix, page.icon.image_h._iy,tabHighlightTextures[i]);
		tabHighDUDVs[i] = relativeUV(page.icon.image_h.ix, page.icon.image_h.iy,tabHighlightTextures[i]);

		tabCheckedUVs[i] = relativeUV(page.icon.image_check._ix, page.icon.image_check._iy, tabCheckedTextures[i]);
		tabCheckedDUDVs[i] = relativeUV(page.icon.image_check.ix, page.icon.image_check.iy, tabCheckedTextures[i]);

		if (tabDisabledTextures[i]) {
			tabDisabledUVs[i] = relativeUV(page.icon.image_disabled._ix, page.icon.image_disabled._iy, tabDisabledTextures[i]);
			tabDisabledDUDVs[i] = relativeUV(page.icon.image_disabled.ix, page.icon.image_disabled.iy, tabDisabledTextures[i]);
		} else {
			tabDisabledUVs[i] = Vect2f(0, 0);
			tabDisabledDUDVs[i] = Vect2f(0, 0);
		}
		if (tabFlashingTextures[i]) {
			tabFlashingUVs[i] = relativeUV(page.icon.image_flashing._ix, page.icon.image_flashing._iy, tabFlashingTextures[i]);
			tabFlashingDUDVs[i] = relativeUV(page.icon.image_flashing.ix, page.icon.image_flashing.iy, tabFlashingTextures[i]);
		} else {
			tabFlashingUVs[i] = Vect2f(0, 0);
			tabFlashingDUDVs[i] = Vect2f(0, 0);
		}
	
	}

	x = absoluteX(attr->x);
	y = absoluteY(attr->y);
	sx = absoluteX(attr->sx);
	sy = absoluteY(attr->sy);

	dx = absoluteX(attr->dx);
	dy = absoluteY(attr->dy);
	ddsx = absoluteX(attr->dsx);
	ddsy = absoluteY(attr->dsy);
}

void CUITabSheet::setFlashingPageInterval(int nPage, float interval) {
	flashTimers[nPage] = interval;
//	if (interval >= 0) {
//		markColor.set(flashR, flashG, flashB, flashA);
//	}
}

void CUITabSheet::ShowPage(int nPage, bool bShow) {
	if(bShow)
		m_nVisiblePagesBits |= (1 << nPage);
	else
		m_nVisiblePagesBits &= ~(1 << nPage);
}

void CUITabSheet::EnablePage(int nPage, bool bEnable)
{
	if(bEnable)
		m_nEnabledPagesBits |= (1 << nPage);
	else
	{
		m_nEnabledPagesBits &= ~(1 << nPage);

		if((m_nEnabledPagesBits & (1 << m_nActivePage)) == 0)
			if(m_nActivePage > 0)
				SwitchPage(m_nActivePage - 1);
	}
}
void CUITabSheet::SelectPage(int nPage, bool bSelect)
{
	if(bSelect)
		m_nSelectedPagesBits |= (1 << nPage);
	else
		m_nSelectedPagesBits &= ~(1 << nPage);
}

int CUITabSheet::HitTest(float fx, float fy) {
	float xF = fx * (float)terRenderDevice->GetSizeX();
	float yF = fy * (float)terRenderDevice->GetSizeY();

	bool bHit = (xF>=x && xF<=(x+sx) && yF>=y && yF<=(y+sy));

	if (bHit) {
		m_nTabHover = HitTestTabHeader(fx, fy);
		m_bLabelHover = (m_nTabHover == m_nActivePage);
	}

	return bHit;
}
bool CUITabSheet::actionPerformed(InterfaceEventCode code, int param) {
	if (code == EVENT_DOUBLECLICK) {
		if(param >= 0 && m_handler && m_nEnabledPagesBits & (1 << param)) {
			m_handler(this, EVENT_DOUBLECLICK, m_nActivePage);
			return true;
		}
	} else {
		if(param >= 0 && m_nActivePage != param) {
			SwitchPage(param);
				return true;
		}
		else if (m_nEnabledPagesBits & (1 << param))
		{
			if(m_nActivePage == param && m_handler) {
				m_handler(this, EVENT_TAB_CHANGED, m_nActivePage);
				return true;
			}
		}
	}
	return false;
}
void CUITabSheet::OnLButtonDown(float _x, float _y)
{
	int nHitTab = HitTestTabHeader(_x, _y);
	if(nHitTab >= 0 && m_nActivePage != nHitTab)
		SwitchPage(nHitTab);
	else if (nHitTab >=0 && m_nEnabledPagesBits & (1 << nHitTab))
	{
		if(m_nActivePage == nHitTab && m_handler)
			m_handler(this, EVENT_TAB_CHANGED, m_nActivePage);
	}
}
void CUITabSheet::OnLButtonDblClk(float _x, float _y)
{
	int nHitTab = HitTestTabHeader(_x, _y);
	if(nHitTab >= 0 && m_handler && m_nEnabledPagesBits & (1 << nHitTab))
		m_handler(this, EVENT_DOUBLECLICK, m_nActivePage);
}

void CUITabSheet::OnRButtonDown(float _x, float _y)
{
	int nHitTab = HitTestTabHeader(_x, _y);
	if (nHitTab >=0 && m_nEnabledPagesBits & (1 << nHitTab)) {
		if (m_handler) {
			m_handler(this, EVENT_RPRESSED, nHitTab);
		}
	}
}

void scaleTabElement(sqshTabElement* pa, Vect2f& scaleV) {
}

void CUITabSheet::scale(Vect2f& scaleV) {
}

bool CUITabSheet::updateColor() {
	m_ftime += frame_time.delta();
	for (int i = 0, s = m_attr->pages.size(); i < s; i++) {
		if (isPageFlashing(i)) {
			if (flashTimers[i]) {
				flashTimers[i] -= frame_time.delta();
				if (flashTimers[i] == 0) {
					flashTimers[i] = -1;
				}
			}
		}
	}
	if (tickTime != 0 && fmod(m_ftime, tickTime) / tickTime < 0.5f) {
//		markColor = clrNormal;
		return false;
	} else {
//		markColor = markColor;
		return true;
	}
}

void CUITabSheet::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
		return;

	//flashing
//	m_ftime += frame_time.delta();
/*	for (int i = 0, s = tabAttrs.size(); i < s; i++) {
		sqshTabElement* pTabAttr = tabAttrs[i];
		if( pTabAttr->flashBG_A != 0 && (m_nMarkedPagesBits & (1 << i)) )	{
			if(pTabAttr->tickTime == 0 || fmod(m_ftime, pTabAttr->tickTime) / pTabAttr->tickTime >= 0.5f) {
				m_cBGColor.set(pTabAttr->flashBG_R, pTabAttr->flashBG_G, pTabAttr->flashBG_B, pTabAttr->flashBG_A);
				draw_rect(
					Vect2i(tabXs[i], tabYs[i]),
					Vect2i(tabXs[i] + tabSXs[i], tabYs[i] + tabSYs[i]),
					m_cBGColor);
			}
		}

	}
*/
	bool markedPhase = updateColor();
	for (int i = 0, s = m_attr->pages.size(); i < s; i++) {
		bool marked = isPageFlashing(i);
		bool bEnabled = m_nEnabledPagesBits & (1 << i);
		const sqshTabElement* pa = tabAttrs[i];

		if ( marked && markedPhase ) {
			m_cColor = markColor;
		} else {
			m_cColor = clrNormal;
		}

		SQUAD_ICON* ps = m_squad_icon_list[i];
		if (ps) {
			//squad
			if ( IsPageSelected(i) ) {
				//checked
				DrawSprite(tabXs[i] + dx, tabYs[i] + dy, 
						tabSXs[i] + ddsx, tabSYs[i] + ddsy,
						ps->checkedUV.x, ps->checkedUV.y, 
						ps->checkedDUDV.x, ps->checkedDUDV.y,
						ps->checkedTexture, m_cColor);
			} else if (!bEnabled) {
				//disabled
				if (ps->disabledTexture) {
					DrawSprite(tabXs[i] + dx, tabYs[i] + dy, 
							tabSXs[i] + ddsx, tabSYs[i] + ddsy,
							ps->disabledUV.x, ps->disabledUV.y, 
							ps->disabledDUDV.x, ps->disabledDUDV.y,
							ps->disabledTexture, m_cColor);
				}
			} else if (m_nTabHover >= 0 && i == m_nTabHover) {
				//highlight
				DrawSprite(tabXs[i] + dx, tabYs[i] + dy, 
						tabSXs[i] + ddsx, tabSYs[i] + ddsy,
						ps->highUV.x, ps->highUV.y, 
						ps->highDUDV.x, ps->highDUDV.y,
						ps->highlightTexture, m_cColor);
			} else {
				//normal
				DrawSprite(tabXs[i] + dx, tabYs[i] + dy, 
						tabSXs[i] + ddsx, tabSYs[i] + ddsy,
						ps->uv.x, ps->uv.y, 
						ps->dudv.x, ps->dudv.y,
						ps->texture, m_cColor);
			}
		} else {
			if (marked && markedPhase && tabFlashingTextures[i]) {
				DrawSprite(tabXs[i], tabYs[i], 
					tabSXs[i], tabSYs[i],
					tabFlashingUVs[i].x, tabFlashingUVs[i].y, 
					tabFlashingDUDVs[i].x, tabFlashingDUDVs[i].y,
					tabFlashingTextures[i], clrNormal);
				continue;
			}
			//squad empty
			if ( IsPageSelected(i) || (m_attr->pages.size() == 3 && i == m_nActivePage)) {
				//checked
				DrawSprite(tabXs[i], tabYs[i], 
					tabSXs[i], tabSYs[i],
					tabCheckedUVs[i].x, tabCheckedUVs[i].y, 
					tabCheckedDUDVs[i].x, tabCheckedDUDVs[i].y,
					tabCheckedTextures[i], m_cColor);
			} else if (!bEnabled) {
				//disabled
				bool bVisible = m_nVisiblePagesBits & (1 << i);
				if (bVisible && tabDisabledTextures[i]) {
					DrawSprite(tabXs[i], tabYs[i], 
						tabSXs[i], tabSYs[i],
						tabDisabledUVs[i].x, tabDisabledUVs[i].y, 
						tabDisabledDUDVs[i].x, tabDisabledDUDVs[i].y,
						tabDisabledTextures[i], m_cColor);
				}
			} else if (m_nTabHover >= 0 && i == m_nTabHover) {
				//highlight
				DrawSprite(tabXs[i], tabYs[i], 
					tabSXs[i], tabSYs[i],
					tabHighUVs[i].x, tabHighUVs[i].y, 
					tabHighDUDVs[i].x, tabHighDUDVs[i].y,
					tabHighlightTextures[i], m_cColor);
			} else {
				//normal
				DrawSprite(tabXs[i], tabYs[i], 
					tabSXs[i], tabSYs[i],
					tabUVs[i].x, tabUVs[i].y, 
					tabDUDVs[i].x, tabDUDVs[i].y,
					tabTextures[i], m_cColor);
			}
		}
		
		//цифры на страницах
		if(!m_page_numbers[i].empty())
		{
			terRenderDevice->SetFont(m_hFont);

			char* str = (char*)m_page_numbers[i].c_str();
			terRenderDevice->DrawRectangle(tabXs[i] + dx + 1, tabYs[i] + dy, terRenderDevice->GetFontLength(str), tabSYs[i] * _button_atom6_y + 1, sColor4c(0, 0, 0, 196));
			terRenderDevice->FlushPrimitive2D();
			OutText(tabXs[i] + dx + 2, tabYs[i] + dy, str, &sColor4f(0, 1, 1, 1));

			terRenderDevice->SetFont(0);
		}
	}
	if (m_nEnabledPagesBits) {
		//show active mark
		//m_nActivePage 
		if ( markedPhase && isPageFlashing(m_nActivePage) )	{
			m_cColor = markColor;
		} else {
			m_cColor = clrNormal;
		}
		const sqshTabElement* pa = tabAttrs[m_nActivePage];
		DrawSprite(tabActiveXs[m_nActivePage], tabActiveYs[m_nActivePage], 
			tabActiveSXs[m_nActivePage], tabActiveSYs[m_nActivePage],
			tabActiveUVs[m_nActivePage].x, tabActiveUVs[m_nActivePage].y, 
			tabActiveDUDVs[m_nActivePage].x, tabActiveDUDVs[m_nActivePage].y,
			tabActiveTextures[m_nActivePage], m_cColor);
	}
}

void CUITabSheet::OnWindow(int enable)
{
	if(!enable){
		m_bLabelHover = false;
		m_nTabHover = -1;
	}

	if(terShowTips && ID == SQSH_TAB_BUILD_ID){
		terPlayer* player = universe()->activePlayer();
		terUnitBase* frame = player->frame();
		if(frame)
		{
			CInfoWindow* pWnd;
			if(pWnd = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID)){
				pWnd->Show(enable);

				if(enable){
					static char cbText[200];

					const terFrameStatisticsType& t = universe()->activePlayer()->GetFrameStats();

					int num_core=gameShell->getLogicUpdater().getLogicData()->num_core;
					pWnd->SetText(_shellIconManager.FormatMessageText("<Shield_bar>", cbText, 
						num_core, 
						t.EnergyArea, t.ProtectedArea)); 
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
int _map_x1, _map_y1;
float _map_xs, _map_ys;
float _map_xs_bound, _map_ys_bound;
bool _map_draw = 0;
sColor4c* _bitmap = 0;

Vect3f _arrowSquad[3] = {Vect3f(-100,0,0), Vect3f(100,0,0), Vect3f(0,100,0)};

void DrawPoint(const Vect2f& v, const sColor4f& cl)
{
	Vect2i _v(_map_x1 + _map_xs*v.x, _map_y1 + _map_ys*v.y);
	draw_point(_v, cl, 2);
}
void DrawPolygon(Vect2f* pv, int count, const sColor4f& cl)
{
	Vect2i _w1(_map_x1, _map_y1), _w2(_map_x1 + _map_xs_bound, _map_y1 + _map_ys_bound);
	Vect2i p1, p2;
	int i;
	for(i=0; i<count-1; i++)
	{
		p1.set(_map_x1 + _map_xs*pv[i].x,   _map_y1 + _map_ys*pv[i].y);
		p2.set(_map_x1 + _map_xs*pv[i+1].x, _map_y1 + _map_ys*pv[i+1].y);

		if(LineClip2d(p1, p2, _w1, _w2))
			draw_line(p1, p2, cl);
	}

	p1.set(_map_x1 + _map_xs*pv[0].x, _map_y1 + _map_ys*pv[0].y);
	p2.set(_map_x1 + _map_xs*pv[i].x, _map_y1 + _map_ys*pv[i].y);
	if(LineClip2d(p1, p2, _w1, _w2))
		draw_line(p1, p2, cl);
}

void DrawSquad(const Vect2f& p, float a, const sColor4f& cl)
{
	Vect3f pp;
	Vect2f _pts[3];
	Mat3f  _mat(a + M_PI, Z_AXIS);
	for(int i=0; i<3; i++)
		_pts[i] = p + Vect2f(_mat.xform(_arrowSquad[i], pp));

	DrawPolygon(_pts, 3, cl);
}
void DrawCircle(const Vect2f& v, const sColor4f& cl)
{
	Vect2i _v(_map_x1 + _map_xs*v.x, _map_y1 + _map_ys*v.y);
	draw_point(_v, cl, 2);
//	terRenderDevice->DrawRectangle(x1, y1, x2-x1, y2-y1, sColor4c(c));
}

CMapWindow::CMapWindow(int id, CShellWindow* pParent, EVENTPROC p) : 
	CShellWindow(id, pParent, p)
{
	m_pMapTexture = 0;
	m_hFont = 0;

	m_bitmap = new sColor4c[128*128];
	_bitmap = m_bitmap;

	activeObjectTx = 0;
	for (int i = 0; i < EVENT_MINI_MAP_MAX; i++) {
		eventTx[i] = 0;
	}
}
CMapWindow::~CMapWindow()
{
	_RELEASE(m_pMapTexture);
	_RELEASE(activeObjectTx);
	_RELEASE(m_hFont);

	for (int i = 0; i < EVENT_MINI_MAP_MAX; i++) {
		_RELEASE(eventTx[i]);
	}

	delete m_bitmap;
	_bitmap = 0;
}

void CMapWindow::Load(const sqshControl* attr)
{
	CShellWindow::Load(attr);

	_RELEASE(m_pMapTexture);
	_RELEASE(activeObjectTx);

	m_pMapTexture = terVisGeneric->CreateTexture( GetTargetName("map.tga") );

	if (m_pMapTexture->GetWidth() > m_pMapTexture->GetHeight()) {
		y += sy / 2;
		sy *= float(m_pMapTexture->GetHeight()) / float(m_pMapTexture->GetWidth());
		y -= sy / 2;
	} else if (m_pMapTexture->GetWidth() < m_pMapTexture->GetHeight()) {
		x += sx / 2;
		sx *= float(m_pMapTexture->GetWidth()) / float(m_pMapTexture->GetHeight());
		x -= sx / 2;
	}

	activeObjectTx = terVisGeneric->CreateTexture( activeObjectSymbol.image );
	activeObjectSx = absoluteX( activeObjectSymbol.sx );
	activeObjectSy = absoluteY( activeObjectSymbol.sy );

	for (int i = 0; i < EVENT_MINI_MAP_MAX; i++) {
		_RELEASE(eventTx[i]);
		eventTx[i] = terVisGeneric->CreateTexture( eventSymbols[i].image );
		eventSx[i] = absoluteX( eventSymbols[i].sx );
		eventSy[i] = absoluteY( eventSymbols[i].sy );
	}

	map_xs = sx / vMap.H_SIZE;
	map_ys = sy / vMap.V_SIZE; 

	delete m_bitmap;
	m_bitmap = new sColor4c[int(sx)*int(sy)];
	_bitmap = m_bitmap;
	_map_x1 = x;
	_map_y1 = y;
	_map_xs = sx / vMap.H_SIZE;
	_map_ys = sy / vMap.V_SIZE; 
	_map_xs_bound = sx;
	_map_ys_bound = sy; 

	_RELEASE(m_hFont);
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont1, defaultFontSize);
}

void CMapWindow::draw(int bFocus)
{
	CShellWindow::draw(bFocus);


	_map_draw = false;

	if(state & SQSH_VISIBLE)
	{
		if(m_pMapTexture)
		{
			DrawSprite(x, y, sx, sy, 0, 0, 1, 1, m_pMapTexture);

			_map_draw = true;

			LogicData* logicData = gameShell->getLogicUpdater().getLogicData();
			if (logicData->getMiniMap() &&  (logicData->getWidth() == sx) && (logicData->getHeight() == sy)) {
				drawBitmap(logicData->getMiniMap());
			} else {
				logicData->createMiniMap(sx, sy);
			}

			for (int i = 0; i < logicData->miniMapSquadCount; i++) {
				DrawSquad(logicData->miniMapSquads[i].pos, logicData->miniMapSquads[i].angle, logicData->miniMapSquads[i].color);
			}

			m_ftime += frame_time.delta();


			if(!logicData->alphaPath.empty())
			{
				list<Vect2f>::iterator ai;
				Vect2f posPrev = Vect2f(x + map_xs * logicData->alphaPath.back().x, y + map_ys * logicData->alphaPath.back().y);
				FOR_EACH(logicData->alphaPath, ai) {
					Vect2f pos = Vect2f(x + map_xs * (*ai).x, y + map_ys * (*ai).y);
					draw_line(posPrev, pos, minimapAlphaPathColor);
					posPrev = pos;
				}
			}

			terRenderDevice->FlushPrimitive2D();

			int x1, y1, x2, y2; 
			terRenderDevice->GetClipRect(&x1, &y1, &x2, &y2);
			terRenderDevice->SetClipRect(x, y, x + sx, y + sy);
			list<Vect2f>::iterator pi;
			FOR_EACH(logicData->miniMapLabels, pi) {
				terRenderDevice->DrawSprite(x + map_xs * (*pi).x - activeObjectSx / 2, y + map_ys * (*pi).y - activeObjectSy / 2, activeObjectSx, activeObjectSy, 0, 0, 1, 1, activeObjectTx, sColor4c(255,255,255,255), fmodf(m_ftime,activeObjectSymbol.period)/activeObjectSymbol.period);
			}
			list<MiniMapEventIcon>::iterator ii;
			FOR_EACH(_shellIconManager.getMiniMapEventIcons(), ii) {
				float timeElapsed = (*ii).period - (*ii).timeRemain;
				if (timeElapsed <= (*ii).animTime) {
					terRenderDevice->DrawSprite(x + map_xs * (*ii).pos.x - eventSx[(*ii).code] / 2, y + map_ys * (*ii).pos.y - eventSy[(*ii).code] / 2, eventSx[(*ii).code], eventSy[(*ii).code], 0, 0, 1, 1, eventTx[(*ii).code], sColor4c(255,255,255,255), timeElapsed / (*ii).animTime);
				}
			}
			
			terRenderDevice->SetClipRect(x1, y1, x2, y2);

/*  
			memset(m_bitmap, 0, sizeof(int) * sx * sy);

			universe()->MapUnitInfo(); 

			//отрисовка кластеров
			drawBitmap(m_bitmap);
*/
			if(terCamera->restricted())
			{
				Vect2f vCamera[4];
				GetCameraViewPolygon(vCamera, vMap.hZeroPlast);

				DrawPolygon(vCamera, 4, sColor4f(1,1,1,1));
			}

			terRenderDevice->FlushPrimitive2D();
		}
	}
}

int OutText(int x,int y,const char *string,sColor4f *color,int align,eBlendMode blend_mode)
{
	terRenderDevice->OutText(x, y, string, *color, align,blend_mode);
	return 0;
}

void CMapWindow::drawBitmap(sColor4c* bitmap) {
	int w = sx;
	int h = sy;
	for (int yMap = 0; yMap < h; yMap++, bitmap += w) {
		sColor4c* line = bitmap;
		int x1 = 0;
		sColor4c clr1;
		clr1.RGBA() = 0;

		for (int xMap = 0; xMap < w; xMap++, line++) {
			sColor4c nclr = *line;
			if (nclr.RGBA() != clr1.RGBA()) {
				if (clr1.RGBA()) {
					terRenderDevice->DrawLine(x + x1, y + yMap, x + xMap, y + yMap, clr1);
				}
				clr1 = nclr;
				x1 = xMap;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
CTextWindow::CTextWindow(int id, CShellWindow* pParent, EVENTPROC p):
	CShellWindow(id, pParent, p)
{
	m_hFont = 0;
	victory = false;
	colorIndex = 0;
}
CTextWindow::~CTextWindow()
{
	_RELEASE(m_hFont);
}

void CTextWindow::Load(const sqshControl* attr)
{
	CShellWindow::Load(attr);

	if (m_hTexture) {
		float tx = absoluteX(m_attr->image.dx);
		float ty = absoluteY(m_attr->image.dy);
		float tsx = absoluteX(m_attr->image.dsx);
		float tsy = absoluteY(m_attr->image.dsy);

		uv.x = (x - tx) / tsx;
		uv.y = (y - ty) / tsy;
//		dudv.x =  (float)m_hTexture->GetWidth() / tsx;
//		dudv.y = (float)m_hTexture->GetHeight() / tsy;
		dudv.x = 1.0f / tsx;
		dudv.y = 1.0f / tsy;
	}

	_RELEASE(m_hFont);
	switch(m_attr->font_group)
	{
		case 4:
			m_hFont = terVisGeneric->CreateFont(shell_main_menu_font, shell_main_menu_font_size1_5);
			break;
		case 2:
			m_hFont = terVisGeneric->CreateFont(shell_main_menu_font, shell_main_menu_font_size1);
			break;
		case 3:
			m_hFont = terVisGeneric->CreateFont(shell_main_menu_font, shell_main_menu_font_size3);
			break;
		default:
			m_hFont = terVisGeneric->CreateFont(sqshShellMainFont1, defaultFontSize);
	}
}

void formatPlainStr(char* strToScr, float sx) {
	char* strTestBegin = strToScr;
	char* strTestEnd1 = strToScr;
	char* strTestEnd2;
	while ( (strTestEnd2 = strchr(strTestEnd1, ' ')) != NULL ) {
		*strTestEnd2 = 0;
		if (terRenderDevice->GetFontLength(strTestBegin) > sx) {
			if (strTestEnd1 != strTestBegin) {
				*strTestEnd2 = ' ';
				*(strTestEnd1 - 1) = '\n';
				strTestBegin = strTestEnd1;
			} else {
				*strTestEnd2 = '\n';
				strTestEnd1 = strTestEnd2 + 1;
				strTestBegin = strTestEnd1;
			}
		} else {
			*strTestEnd2 = ' ';
			strTestEnd1 = strTestEnd2 + 1;
		}
	}
	if (strTestEnd1 != strTestBegin && terRenderDevice->GetFontLength(strTestBegin) > sx) {
		*(strTestEnd1 - 1) = '\n';
	}
}

void CTextWindow::draw(int bFocus)
{
/*
	CShellWindow::draw(bFocus);

	if( !(state & SQSH_VISIBLE) || _shellIconManager.IsEffect() )
		return;
	if(!m_hTexture)
		draw_rect(Vect2i(x,y), Vect2i(x+sx,y+sy), sColor4f(0, 0, 0, 0.3f));

	if(!textData.empty())
	{
		terRenderDevice->SetFont(m_hFont);
		if( m_attr->txt_align<0 )
			OutText(x+tutTextWindowDx, y+tutTextWindowDx, textData.c_str(), &sColor4f(1, 1, 1, 1), m_attr->txt_align);
		else
			OutText(x+sx/2 + tutTextWindowDx, y + tutTextWindowDy, textData.c_str(), &sColor4f(1, 1, 1, 1), 0 );
		terRenderDevice->SetFont(0);
	}
*/
	if( !(state & SQSH_VISIBLE) ) return;
	m_ftime += frame_time.delta();

	float Alpha;

	if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
	{
		Alpha=0;
		float phase;
		if( m_effect==effectButtonsFadeIn )
			phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
		else
			phase = _shellIconManager.m_fEffectTime;
		if( phase<0 )
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		else if( phase<=_fEffectButtonTime1 );
		else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
		else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
			Alpha=phase/_fEffectButtonTime3;
		else
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
	}
	else 
		{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

	if (debug_show_intf_borders) {
		draw_rect_empty( Vect2i(x, y), Vect2i(x + sx,y + sy), sColor4f(1, 1, 0, 1) );
	}

	if(!textData.empty())
	{
		terRenderDevice->SetFont(m_hFont);

		char* strToScr = new char[textData.length() + 1];
		strcpy(strToScr, textData.c_str());

		char* strTestEnterBegin = strToScr;
		char* strTestEnter;
		while ( (strTestEnter = strchr(strTestEnterBegin, '\n')) != NULL ) {
			*strTestEnter = 0;
			formatPlainStr(strTestEnterBegin, sx);
			*strTestEnter = '\n';
			strTestEnterBegin = strTestEnter + 1;
		}
		formatPlainStr(strTestEnterBegin, sx);


		float txtX = x;
		float txtY = y;
		int txtAlign = m_attr->txt_align;

		switch (m_attr->txt_align) {
			case SHELL_ALIGN_CENTER:
				txtX += sx/2;
				break;
			case SHELL_ALIGN_RIGHT:
				txtX -= terRenderDevice->GetFontLength(strToScr);
				txtAlign = SHELL_ALIGN_LEFT;
				break;
		}
/*
		if( m_attr->txt_align >= 0 ) {
			txtX += sx/2;
			txtAlign = 0;
		}
*/
		if( m_attr->txt_vert_align == 0 ) {
			Vect2f v1, v2;
			OutTextRect(0, 0 , strToScr, -1, v1, v2);
			txtY += (sy - (v2.y - v1.y)) / 2.0f;
		}

		sColor4f color;
		if (m_attr->font_group == 4) {
			const PlayerColor& pc = playerColors[colorIndex];
			color = sColor4f(pc.unitColor);
			color.a = Alpha;
		} else {
			color = sColor4f(1, 1, 1, Alpha);
		}
		if (m_hTexture) {
			color.a = Alpha * (m_attr->txt_dx ? m_attr->txt_dx : scaleButtonAlpha);
			terRenderDevice->OutText(
				txtX,
				txtY,
				strToScr,
				color,
				txtAlign,
				ALPHA_ADDBLENDALPHA,
				m_hTexture,
				COLOR_MOD,
				uv,
				dudv,
				fmodf(m_ftime,1000)/1000,
				pushButtonTextureWeight);
		} else {
			sColor4f color;
			if (m_attr->font_group == 2) {
				color = victory ? sColor4f(1, 1, 0, Alpha) : sColor4f(1, 0, 0, Alpha);
			} else {
				color = sColor4f(1, 1, 1, Alpha);
			}
			OutText(txtX, txtY, strToScr, &color, txtAlign );
		}
		delete[] strToScr;
/*
		float txtX = x;
		float txtY = y;
		int txtAlign = m_attr->txt_align;
		if( m_attr->txt_align >= 0 ) {
			txtX += sx/2;
			txtAlign = 0;
		}
		if( m_attr->txt_vert_align == 0 ) {
			Vect2f v1, v2;
			OutTextRect(0, 0 , textData.c_str(), -1, v1, v2);
			txtY += (sy - (v2.y - v1.y)) / 2.0f;
		}

		if (m_hTexture) {
			float alpha = Alpha * (m_attr->txt_dx ? m_attr->txt_dx : scaleButtonAlpha);
			terRenderDevice->OutText(
				txtX,
				txtY,
				textData.c_str(),
				sColor4f(1, 1, 1, alpha),
				txtAlign,
				ALPHA_ADDBLENDALPHA,
				m_hTexture,
				COLOR_MOD,
				uv,
				dudv,
				fmodf(m_ftime,1000)/1000,
				pushButtonTextureWeight);
		} else {
			OutText(txtX, txtY, textData.c_str(), &sColor4f(1, 1, 1, Alpha), txtAlign );
		}
*/
		terRenderDevice->SetFont(0);
	}
	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

//---------------------------


//---------------------------

void CTextStringWindow::draw(int bFocus)
{
	if( !(state & SQSH_VISIBLE) ) return;
	m_ftime += frame_time.delta();

	float Alpha;

	if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
	{
		Alpha=0;
		float phase;
		if( m_effect==effectButtonsFadeIn )
			phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
		else
			phase = _shellIconManager.m_fEffectTime;
		if( phase<0 )
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		else if( phase<=_fEffectButtonTime1 );
		else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
		else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
			Alpha=phase/_fEffectButtonTime3;
		else
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
	}
	else 
		{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

	if (debug_show_intf_borders) {
		draw_rect_empty( Vect2i(x, y), Vect2i(x + sx,y + sy), sColor4f(1, 1, 0, 1) );
	}

	if(!textData.empty())
	{
		terRenderDevice->SetFont(m_hFont);

		string toScr = getValidatedText(textData, sx);

		float txtX = x;
		float txtY = y;
		int txtAlign = m_attr->txt_align;

		switch (m_attr->txt_align) {
			case SHELL_ALIGN_CENTER:
				txtX += sx/2;
				break;
			case SHELL_ALIGN_RIGHT:
				txtX -= terRenderDevice->GetFontLength(toScr.c_str());
				txtAlign = SHELL_ALIGN_LEFT;
				break;
		}

		if( m_attr->txt_vert_align == 0 ) {
			Vect2f v1, v2;
			OutTextRect(0, 0, toScr.c_str(), -1, v1, v2);
			txtY += (sy - (v2.y - v1.y)) / 2.0f;
		}

		sColor4f color;
		if (m_attr->font_group == 4) {
			const PlayerColor& pc = playerColors[colorIndex];
			color = sColor4f(pc.unitColor);
			color.a = Alpha;
		} else {
			color = sColor4f(1, 1, 1, Alpha);
		}
		if (m_hTexture) {
			color.a = Alpha * (m_attr->txt_dx ? m_attr->txt_dx : scaleButtonAlpha);
			terRenderDevice->OutText(
				txtX,
				txtY,
				toScr.c_str(),
				color,
				txtAlign,
				ALPHA_ADDBLENDALPHA,
				m_hTexture,
				COLOR_MOD,
				uv,
				dudv,
				fmodf(m_ftime,1000)/1000,
				pushButtonTextureWeight);
		} else {
			sColor4f color;
			if (m_attr->font_group == 2) {
				color = victory ? sColor4f(1, 1, 0, Alpha) : sColor4f(1, 0, 0, Alpha);
			} else {
				color = sColor4f(1, 1, 1, Alpha);
			}
			OutText(txtX, txtY, toScr.c_str(), &color, txtAlign );
		}
		terRenderDevice->SetFont(0);
	}
	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}
////////////////////////////////////////////////////////////////////////////////////////////////

CListBoxWindow::CListBoxWindow(int id, CShellWindow* pParent, EVENTPROC p):CShellWindow(id, pParent, p)
{
	m_pItem = 0;
	m_nNumberItem = 0; 
	m_hFont = 0;
	m_nCurSel = -1;
	m_bScroller = 0;
	m_nTopItem = 0;
	m_fStringHeight = listBoxRowHeight;
	currentScrollDirection = thumb_none;
	thumbTexture = 0;
	m_hTextureBG = 0;
	NewItem(1);
	if( m_handler )
		m_handler(this, EVENT_CREATEWND, 0);
}
CListBoxWindow::~CListBoxWindow()
{
	_RELEASE(m_hFont);
	if(m_pItem) delete [] m_pItem; m_pItem=0;
	_RELEASE(thumbTexture);
	RELEASE(m_hTextureBG);
}

void CListBoxWindow::NewItem(int nrow)
{
	if(m_pItem) delete [] m_pItem;
	m_pItem=new sItem[m_nNumberItem=nrow];
	for( int i=0; i<m_nNumberItem; i++ )
		m_pItem[i].x = 0;
}

int CListBoxWindow::CheckClick(float fx,float  fy)
{
	if(!m_bScroller)
		return thumb_none;

	float _x = fx * terRenderDevice->GetSizeX();
	float _y = fy * terRenderDevice->GetSizeY();

	if(_x > x+sx-vScrollSX && _x < x+sx)
	{
		if(_y > y && _y <y+vScrollSY)
			return thumb_up;
		if(_y < y+sy && _y > y+sy-vScrollSY)
			return thumb_dn;
	}

	return thumb_none;
}

int CListBoxWindow::ItemFromPoint(float  _y)
{
	if(m_pItem[0].m_data.empty())
		return -1;

	int i = int((_y - y)/m_fStringHeight) + m_nTopItem;

	if(i >= m_pItem[0].m_data.size())
		i = m_pItem[0].m_data.size()-1;

	//TEMP
//	if (i > lastAccessibleMissionNumber) {
//		i = lastAccessibleMissionNumber;
//	}
	return i;
}

void CListBoxWindow::Clear()
{
	for(int i=0;i<m_nNumberItem;i++)
		m_pItem[i].m_data.clear();
	m_nCurSel = -1;
	updateScroller();
}
void CListBoxWindow::AddString(const char* cb,int nrow)
{
	m_pItem[nrow].m_data.push_back(cb);
	updateScroller();
}
void CListBoxWindow::updateScroller() {
	int size = m_pItem[0].m_data.size();
	if (m_nTopItem < 0) {
		m_nTopItem = 0;
	} else if (m_nTopItem > size - int(sy/m_fStringHeight)) {
		m_nTopItem = max(size - int(sy/m_fStringHeight), 0);
	}
	m_bScroller = int(sy/m_fStringHeight) < m_pItem[0].m_data.size();
	if(m_bScroller)
		m_fScrollerThumbPos = y + vScrollSY + 1 + float(m_nTopItem)/(m_pItem[0].m_data.size() - int(sy/m_fStringHeight))*(sy - 2*vScrollSY - vScrollThmbSY);
}

const char* CListBoxWindow::GetCurSelString()
{
	if(m_nCurSel >= 0)
		return m_pItem[0].m_data[m_nCurSel].c_str();

	return 0;
}

void CListBoxWindow::Load(const sqshControl* attr)
{
	CShellWindow::Load(attr);

	_RELEASE(thumbTexture);
	_RELEASE(m_hTextureBG);

	m_fStringHeight = absoluteY(listBoxRowHeight);

	_RELEASE(m_hFont);
	if (m_attr->font_group == 1) {
		m_hFont = terVisGeneric->CreateFont(sqshShellMainFont1, statsHeadTableFontSize);
	} else {
		//?
		m_hFont = terVisGeneric->CreateFont(sqshShellMainFont1, statsHeadTableFontSize);
	}
//	m_vTexPos[0] = Vect2f(attr->image._ix, attr->image._iy);
//	m_vTexPos[1] = Vect2f(attr->image.ix, attr->image.iy);
	if (m_hTexture) {
		m_vTexPos[1] = relativeUV(attr->image.ix, attr->image.iy, m_hTexture);
	}
	txtdx = absoluteX(attr->txt_dx);
	txtdy = absoluteY(attr->txt_dy);
	vScrollSX = absoluteX(attr->xstart);
	vScrollSY = absoluteY(attr->ystart);
	vScrollThmbSX = relativeX(attr->image_h.ix);
	vScrollThmbSY = relativeY(attr->image_h.iy);
	if ( strlen(attr->image_h.texture) ) {
		thumbTexture = terVisGeneric->CreateTexture( getImageFileName(&(attr->image_h)).c_str() );
		thumbUV = relativeUV(attr->image_h._ix, attr->image_h._iy, thumbTexture);
		thumbDUDV = relativeUV(attr->image_h.ix, attr->image_h.iy, thumbTexture);
	}
	if (strlen(m_attr->image_check.texture)) {
		m_hTextureBG = terVisGeneric->CreateTexture(getImageFileName(&(m_attr->image_check)).c_str());
		m_vTexBGPos = relativeUV(m_attr->image_check._ix, m_attr->image_check._iy, m_hTextureBG);
		m_vTexBGSize = relativeUV(m_attr->image_check.ix, m_attr->image_check.iy, m_hTextureBG);

		float tx = absoluteX(m_attr->image_check.dx);
		float ty = absoluteY(m_attr->image_check.dy);
		float tsx = absoluteX(m_attr->image_check.dsx);
		float tsy = absoluteY(m_attr->image_check.dsy);

		uv.x = (x - tx + txtdx) / tsx;
		uv.y = (y - ty + txtdy) / tsy;
//		dudv.x =  (float)m_hTexture->GetWidth() / tsx;
//		dudv.y = (float)m_hTexture->GetHeight() / tsy;
		dudv.x = 1.0f / tsx;
		dudv.y = 1.0f / tsy;
	}
}

void CListBoxWindow::SetCurSel(int n){
	m_nCurSel = n;

	m_nTopItem = m_nCurSel;
	updateScroller();
/*
	int size = m_pItem[0].m_data.size();
	if (m_nTopItem < 0) {
		m_nTopItem = 0;
	} else if (m_nTopItem >= size - int(sy/m_fStringHeight)) {
		m_nTopItem = max(size - int(sy/m_fStringHeight) - 1, 0);
	}
	m_bScroller = int(sy/m_fStringHeight) < m_pItem[0].m_data.size();
	if(m_bScroller)
		m_fScrollerThumbPos = y + vScrollSY + 1 + float(m_nTopItem)/(m_pItem[0].m_data.size() - int(sy/m_fStringHeight))*(sy - 2*vScrollSY - vScrollThmbSY);
*/
}

void CListBoxWindow::SetCurSelPassive(int n){
	m_nCurSel = n;

//	m_nTopItem = m_nCurSel;
//	updateScroller();
}

void CListBoxWindow::draw(int bFocus)
{
	if( !(state & SQSH_VISIBLE) ) return;
	m_ftime += frame_time.delta();

	float Alpha;

	if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut))
	{
		Alpha=0;
		float phase;
		if( m_effect==effectButtonsFadeIn )
			phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
		else
			phase = _shellIconManager.m_fEffectTime;
		if( phase<0 )
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		else if( phase<=_fEffectButtonTime1 );
		else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
		else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
			Alpha=phase/_fEffectButtonTime3;
		else
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
	}
	else 
		{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }


	if (currentScrollDirection != thumb_none) {
		if (isPressed(VK_LBUTTON)) {
			if (m_ftime >= scrollSpeed) {
				m_ftime = 0;
				if (currentScrollDirection == thumb_up) {
					if ( m_nTopItem > 0 ) {
						m_nTopItem--;
					}
				} else {
					if(m_nTopItem + int(sy/m_fStringHeight) < m_pItem[0].m_data.size()) {
						m_nTopItem++;
					}
				}
//				if (m_bScroller) {
//					m_fScrollerThumbPos = y + vScrollSY + 1 + float(m_nTopItem)/(m_pItem[0].m_data.size() - int(sy/m_fStringHeight))*(sy - 2*vScrollSY - vScrollThmbSY);
//				}
			}
		} else {
			currentScrollDirection = thumb_none;
		}
	}
	if (m_bScroller) {
		m_fScrollerThumbPos = y + vScrollSY + 1 + float(m_nTopItem)/(m_pItem[0].m_data.size() - int(sy/m_fStringHeight))*(sy - 2*vScrollSY - vScrollThmbSY);
	}
	//контур
//	draw_rect_empty(Vect2f(x, y), Vect2f(x+sx, y+sy), sColor4f(1, 1, 1, Alpha));

	float _sx_client = sx - vScrollSX;
	if(m_bScroller)
	{
		//нужен скроллер

//		draw_rect_empty(Vect2f(x + sx - vScrollSX, y), Vect2f(x+sx, y+sy), sColor4f(0.5f, 0.5f, 0.5f, Alpha));
//		draw_line(Vect2f(x + sx - vScrollSX, y + vScrollSY), Vect2f(x + sx, y + vScrollSY), sColor4f(0.5f, 0.5f, 0.5f, Alpha));
//		draw_line(Vect2f(x + sx - vScrollSX, y+sy - vScrollSY), Vect2f(x + sx, y+sy - vScrollSY), sColor4f(0.5f, 0.5f, 0.5f, Alpha));

		if (m_hTextureBG) {
			terRenderDevice->DrawSprite2(
				x + sx - vScrollSX, m_fScrollerThumbPos, vScrollThmbSX, vScrollThmbSY, thumbUV.x, thumbUV.y, thumbDUDV.x, thumbDUDV.y,
				m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
				thumbTexture, m_hTextureBG,
				mapTextureWeight, Alpha * scaleButtonAlpha, fmodf(m_ftime,1000)/1000,
				COLOR_MOD, ALPHA_ADDBLENDALPHA);
		} else {
			DrawSprite(x + sx - vScrollSX, m_fScrollerThumbPos, vScrollThmbSX, vScrollThmbSY, 
				thumbUV.x, thumbUV.y, thumbDUDV.x, thumbDUDV.y, 
				thumbTexture, sColor4c(255,255,255,Alpha*255) );
		}
//		draw_rect(Vect2f(x + sx - vScrollSX, m_fScrollerThumbPos), Vect2f(x+sx, m_fScrollerThumbPos+vScrollSY), sColor4f(0.5f, 0.5f, 0.5f, Alpha));
	}

	if (debug_show_intf_borders) {
		draw_rect_empty( Vect2i(x, y), Vect2i(x + sx,y + sy), sColor4f(1, 1, 0, 1) );
	}

	//подсветка выделенного элемента
	if ((m_nCurSel - m_nTopItem >= 0)) {
		if (m_attr->alnum) {
			float _y = y + (m_nCurSel - m_nTopItem)*m_fStringHeight;
			if(_y+m_fStringHeight <= y+sy) {
				if (debug_show_intf_borders) {
					draw_rect_empty( Vect2i(x, _y), Vect2i(x + _sx_client, _y + m_fStringHeight), sColor4f(1, 1, 0, 1) );
				}
				DrawSprite(x, _y, _sx_client, m_fStringHeight, 
					m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, 
					m_hTexture, sColor4c(255,255,255,Alpha*255) );
			}
		} else {
/*
			float _y = y + (m_nCurSel - m_nTopItem)*m_fStringHeight;
			if(_y+m_fStringHeight < y+sy)
			DrawSprite(x, _y, _sx_client, sy, 
				m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, 
				m_hTexture, sColor4c(255,255,255,Alpha*255) );
*/
		}
	}

	terRenderDevice->SetFont(m_hFont);

	for(int nItem=0; nItem<m_nNumberItem; nItem++ )
	{
		int i, sz = m_pItem[nItem].m_data.size();
		float y_str = y;

		int width = (nItem == m_nNumberItem - 1) ? sx : sx * m_pItem[nItem + 1].x;
		width -= sx * m_pItem[nItem].x;
		if (m_attr->alnum == 0 && nItem % 2) {
			DrawSprite(x+sx * m_pItem[nItem].x + txtdx, y, width, sy, 
				m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, 
				m_hTexture, sColor4c(255,255,255,Alpha*128) );
		}

		for(i = m_nTopItem; i < sz; i++)
		{
			if(y_str+m_fStringHeight > y+sy)
				break;

			string toStr = getValidatedText(m_pItem[nItem].m_data[i], _sx_client - txtdx);
//			string toStr = getValidatedText(m_pItem[nItem].m_data[i], _sx_client - txtdx - m_fStringHeight/2);

			float yS = y_str + m_fStringHeight / 2 - m_hFont->GetHeight() / 2;

			if (m_hTextureBG) {
				terRenderDevice->OutText(
					x+sx*m_pItem[nItem].x + txtdx,
//					x+m_fStringHeight/2+sx*m_pItem[nItem].x + txtdx,
					yS, (char*)toStr.c_str(), sColor4f(1, 1, 1, Alpha), SHELL_ALIGN_LEFT, ALPHA_ADDBLENDALPHA, m_hTextureBG, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
			} else if (m_attr->alnum) {
				OutText(
					x+sx*m_pItem[nItem].x + txtdx, 
//					x+m_fStringHeight/2+sx*m_pItem[nItem].x + txtdx, 
					yS, (char*)toStr.c_str(), &sColor4f(1, 1, 1, Alpha));
			} else {
				OutText(
					x + sx * m_pItem[nItem].x + txtdx + width / 2, 
//					x + m_fStringHeight/2 + sx * m_pItem[nItem].x + txtdx + width / 2, 
					yS, (char*)toStr.c_str(), &sColor4f(1, 1, 1, Alpha), SHELL_ALIGN_CENTER);
			}
			y_str += m_fStringHeight;
		}
	}
	terRenderDevice->SetFont(0);
	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

void CListBoxWindow::Move(float _x, float _y, float _sx, float _sy)
{
//	CShellWindow::Move(_x, _y, _sx, _sy);

//	m_bScroller = int(_sy/m_fStringHeight) < m_pItem[0].m_data.size();
//	if(m_bScroller)
//		m_fScrollerThumbPos = y + vScrollSY + 1 + float(m_nTopItem)/(m_pItem[0].m_data.size() - int(_sy/m_fStringHeight))*(sy - 2*vScrollSY - vScrollThmbSY - 1);
}

void CListBoxWindow::OnLButtonDown(float _x, float _y)
{
	currentScrollDirection = CheckClick(_x, _y);
	if (currentScrollDirection == thumb_none) {
		if (m_attr->passive != 1) {
			m_nCurSel = ItemFromPoint( terScreenSizeY*_y );
			if(m_handler && (state & SQSH_ENABLED))
				m_handler(this, EVENT_PRESSED, 0);
		} else {
			if(m_handler && (state & SQSH_ENABLED))
				m_handler(this, EVENT_PRESSED, ItemFromPoint( terScreenSizeY*_y ));
		}
	}
}

void CListBoxWindow::OnLButtonDblClk(float _x, float _y)
{
	if(m_handler && (_x * terRenderDevice->GetSizeX() <= x+sx-vScrollSX) && (state & SQSH_ENABLED) && m_effect == 0) {
		m_handler(this, EVENT_DOUBLECLICK, VK_LBUTTON );
	}
}

void CListBoxWindow::OnMouseWheel(int delta)
{
	if( !(state & SQSH_VISIBLE) ) return;

	m_nTopItem += delta;

	if(m_nTopItem < 0)
		m_nTopItem = 0;
	else if(m_nTopItem + int(sy/m_fStringHeight) > m_pItem[0].m_data.size())
		m_nTopItem = max(m_pItem[0].m_data.size() - int(sy/m_fStringHeight), 0);

	CShellWindow::OnMouseWheel(delta);
}

/////////////////////////////////////
CStatListBoxWindow::CStatListBoxWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) {
	m_hFont = 0;
	m_pItem = 0;
	m_fStringHeight = statsTableRowHeight;
	m_hTextureBG = 0;
	NewItem(1);
	if( m_handler )
		m_handler(this, EVENT_CREATEWND, 0);
}
CStatListBoxWindow::~CStatListBoxWindow() {
	if(m_pItem) delete [] m_pItem; m_pItem=0;
	_RELEASE(m_hFont);
	_RELEASE(m_hTextureBG);
}

void CStatListBoxWindow::NewItem(int nrow)
{
	if(m_pItem) delete [] m_pItem;
	m_pItem = new sItem[m_nNumberItem = nrow];

	for ( int i = 0; i < m_nNumberItem; i++ ) {
		m_pItem[i].x = 0;
	}
}

void CStatListBoxWindow::Clear()
{
	for (int i = 0;i < m_nNumberItem;i++) {
		m_pItem[i].m_data.clear();
		races.clear();
		colors.clear();
	}
}
void CStatListBoxWindow::AddString(const char* cb,int nrow)
{
	m_pItem[nrow].m_data.push_back(cb);
}

void CStatListBoxWindow::AddRace(int race, const sColor4c& color)
{
	m_pItem[0].m_data.push_back("");
	races.push_back(race);
	colors.push_back(color);
}

void CStatListBoxWindow::Load(const sqshControl* attr) {
	CShellWindow::Load(attr);

	_RELEASE(m_hTextureBG);

	m_fStringHeight = absoluteY(statsTableRowHeight);

	if (m_hTexture) {
		for (int i = 0; i < 3; i++) {
			m_vTexPosRace[0][i] = relativeUV(imageRacePos[i].x, imageRacePos[i].y, m_hTexture);
			m_vTexPosRace[1][i] = relativeUV(imageRaceSize[i].x, imageRaceSize[i].y, m_hTexture);
		}
	}

	m_hTextureBG = terVisGeneric->CreateTexture(getImageFileName(&(m_attr->image_h)).c_str());
	m_vTexBGPos = relativeUV(m_attr->image_h._ix, m_attr->image_h._iy, m_hTextureBG);
	m_vTexBGSize = relativeUV(m_attr->image_h.ix, m_attr->image_h.iy, m_hTextureBG);

	_RELEASE(m_hFont);
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont1, statsTableFontSize);
	txtdx = absoluteX(attr->txt_dx);
	txtdy = absoluteY(attr->txt_dy);
}

void CStatListBoxWindow::draw(int bFocus) {
	if( !(state & SQSH_VISIBLE) ) return;
	m_ftime += frame_time.delta();

	float Alpha;

	if ( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) {
		Alpha=0;
		float phase;
		if( m_effect==effectButtonsFadeIn )
			phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
		else
			phase = _shellIconManager.m_fEffectTime;
		if( phase<0 )
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		else if( phase<=_fEffectButtonTime1 );
		else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
		else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
			Alpha=phase/_fEffectButtonTime3;
		else
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
	}
	else 
		{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }


	//контур
	if (debug_show_intf_borders) {
		draw_rect_empty( Vect2i(x, y), Vect2i(x + sx,y + sy), sColor4f(1, 1, 0, 1) );
	}

	terRenderDevice->SetFont(m_hFont);

	for (int nItem = 0; nItem < m_nNumberItem; nItem++ ) {
		int sz = m_pItem[nItem].m_data.size();
		int width = (nItem == m_nNumberItem - 1) ? sx : sx * m_pItem[nItem + 1].x;
		width -= sx * m_pItem[nItem].x;
		if (nItem % 2) {
			DrawSprite(x+sx * m_pItem[nItem].x + txtdx, y, width, m_fStringHeight * sz, 
				m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y, 
				m_hTextureBG, sColor4c(255,255,255,Alpha*128) );
		}
		if (debug_show_intf_borders) {
			draw_rect_empty( Vect2i(x+sx * m_pItem[nItem].x + txtdx, y), Vect2i(x+sx * m_pItem[nItem].x + txtdx + width, y + m_fStringHeight * sz), sColor4f(1, 1, 0, 1) );
		}
		float y_str = y;
		for (int i = 0; i < sz; i++) {
			if (nItem == 0) {
				colors[i].a = 255 * Alpha;
				terRenderDevice->DrawSprite(
					x + sx * m_pItem[nItem].x + txtdx, y_str + txtdy / 2, sy, sy, m_vTexPosRace[0][races[i]].x, m_vTexPosRace[0][races[i]].y, m_vTexPosRace[1][races[i]].x, m_vTexPosRace[1][races[i]].y,
					m_hTexture, colors[i], fmodf(m_ftime,1000)/1000, ALPHA_ADDBLENDALPHA);
//				terRenderDevice->DrawSprite(
//					x + sx * m_pItem[nItem].x + txtdx, y_str + txtdy, sy, sy, m_vTexPosRace[0][races[i]].x, m_vTexPosRace[0][races[i]].y, m_vTexPosRace[1][races[i]].x, m_vTexPosRace[1][races[i]].y,
//					m_hTexture, colors[i], fmodf(m_ftime,1000)/1000);
			} else {
				string toStr = getValidatedText(m_pItem[nItem].m_data[i], width);

				OutText(
					x + sx * m_pItem[nItem].x + txtdx + width / 2, 
					y_str + txtdy,
					toStr.c_str(),
					&sColor4f(1, 1, 1, Alpha),
					SHELL_ALIGN_CENTER
				);
				if (debug_show_intf_borders) {
					draw_rect_empty( Vect2i(x+sx * m_pItem[nItem].x + txtdx, y_str), Vect2i(x+sx * m_pItem[nItem].x + txtdx + width, y_str + m_fStringHeight), sColor4f(1, 1, 0, 1) );
				}
			}
			y_str += m_fStringHeight;
		}
	}
	terRenderDevice->SetFont(0);
	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}
//////////////////////////////////////////

ChatWindow::ChatWindow(int id, CShellWindow* pParent, EVENTPROC p):CShellWindow(id, pParent, p)
{
	m_hFont = 0;
	m_bScroller = 0;
	m_nTopItem = 0;
	m_fStringHeight = listBoxRowHeight;
	currentScrollDirection = thumb_none;
	thumbTexture = 0;
	m_hTextureBG = 0;
	if( m_handler )
		m_handler(this, EVENT_CREATEWND, 0);
}
ChatWindow::~ChatWindow()
{
	_RELEASE(m_hFont);
	_RELEASE(thumbTexture);
	RELEASE(m_hTextureBG);
}

int ChatWindow::CheckClick(float fx,float  fy)
{
	if(!m_bScroller)
		return thumb_none;

	float _x = fx * terRenderDevice->GetSizeX();
	float _y = fy * terRenderDevice->GetSizeY();

	if(_x > x+sx-vScrollSX && _x < x+sx)
	{
		if(_y > y && _y <y+vScrollSY)
			return thumb_up;
		if(_y < y+sy && _y > y+sy-vScrollSY)
			return thumb_dn;
	}

	return thumb_none;
}

void ChatWindow::Clear()
{
	m_data.clear();
	updateScroller();
}

void ChatWindow::AddString(const char* cb)
{
	int break_len = GetStringBreak(cb);

	if(break_len != -1){
		string text = cb;
		string text1 = text.substr(break_len, text.size() - break_len);

		int break_len1 = GetStringBreak(text1.c_str());
		if(break_len1 != -1){
			break_len1 = GetStringBreak(cb, true);
			m_data.push_back(text.substr(0, break_len1));
			m_data.push_back(text.substr(break_len1, text.size() - break_len1));
		}
		else {
			m_data.push_back(text.substr(0, break_len));
			m_data.push_back(text1);
		}
	}
	else 
		m_data.push_back(cb);

	updateScroller();
}

int ChatWindow::GetStringBreak(const char* str, bool ignore_spaces) const
{
	terRenderDevice->SetFont(m_hFont);

	float x = sx - vScrollSX - txtdx;

	if(terRenderDevice->GetFontLength(str) > x){
		sColor4c diffuse(0,0,0,0);

		string text = str;
		const char* textStart = text.c_str();

		float width = 0;
		int last_space = -1;
		for(const char* p = textStart; *p; p++){
			terRenderDevice->ChangeTextColor(p, diffuse);
			width += terRenderDevice->GetCharLength(*p);

			if(width > x){
				return (last_space == -1) ? (p - textStart) : last_space;
			}
			else {
				if(!ignore_spaces && *p == ' ')
					last_space = p - textStart;
			}
		}
	}

	terRenderDevice->SetFont(0);

	return -1;
}

void ChatWindow::updateScroller() {
	int size = m_data.size();
//	if (m_nTopItem < 0) {
//		m_nTopItem = 0;
//	} else if (m_nTopItem > size - int(sy/m_fStringHeight)) {
//		m_nTopItem = max(size - int(sy/m_fStringHeight), 0);
//	}
	m_bScroller = int(sy/m_fStringHeight) < m_data.size();
	if (m_bScroller) {
		m_nTopItem = m_data.size() - (sy/m_fStringHeight) + 1;
		m_fScrollerThumbPos = y + vScrollSY + 1 + float(m_nTopItem)/(m_data.size() - int(sy/m_fStringHeight))*(sy - 2*vScrollSY - vScrollThmbSY);
	} else {
		m_nTopItem = 0;
	}
}

void ChatWindow::Load(const sqshControl* attr)
{
	CShellWindow::Load(attr);

	_RELEASE(thumbTexture);
	_RELEASE(m_hTextureBG);

	m_fStringHeight = absoluteY(18);

	_RELEASE(m_hFont);

	//!!!
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont1, 16);

	if (m_hTexture) {
		m_vTexPos[1] = relativeUV(attr->image.ix, attr->image.iy, m_hTexture);
	}
	txtdx = absoluteX(attr->txt_dx);
	txtdy = absoluteY(attr->txt_dy);
	vScrollSX = absoluteX(attr->xstart);
	vScrollSY = absoluteY(attr->ystart);
	vScrollThmbSX = relativeX(attr->image_h.ix);
	vScrollThmbSY = relativeY(attr->image_h.iy);
	if ( strlen(attr->image_h.texture) ) {
		thumbTexture = terVisGeneric->CreateTexture( getImageFileName(&(attr->image_h)).c_str() );
		thumbUV = relativeUV(attr->image_h._ix, attr->image_h._iy, thumbTexture);
		thumbDUDV = relativeUV(attr->image_h.ix, attr->image_h.iy, thumbTexture);
	}
	if (strlen(m_attr->image_check.texture)) {
		m_hTextureBG = terVisGeneric->CreateTexture(getImageFileName(&(m_attr->image_check)).c_str());
		m_vTexBGPos = relativeUV(m_attr->image_check._ix, m_attr->image_check._iy, m_hTextureBG);
		m_vTexBGSize = relativeUV(m_attr->image_check.ix, m_attr->image_check.iy, m_hTextureBG);

		float tx = absoluteX(m_attr->image_check.dx);
		float ty = absoluteY(m_attr->image_check.dy);
		float tsx = absoluteX(m_attr->image_check.dsx);
		float tsy = absoluteY(m_attr->image_check.dsy);

		uv.x = (x - tx + txtdx) / tsx;
		uv.y = (y - ty + txtdy) / tsy;
//		dudv.x =  (float)m_hTexture->GetWidth() / tsx;
//		dudv.y = (float)m_hTexture->GetHeight() / tsy;
		dudv.x = 1.0f / tsx;
		dudv.y = 1.0f / tsy;
	}
}

void ChatWindow::draw(int bFocus)
{
	if( !(state & SQSH_VISIBLE) ) return;
	m_ftime += frame_time.delta();

	float Alpha;

	if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut))
	{
		Alpha=0;
		float phase;
		if( m_effect==effectButtonsFadeIn )
			phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
		else
			phase = _shellIconManager.m_fEffectTime;
		if( phase<0 )
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		else if( phase<=_fEffectButtonTime1 );
		else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
		else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
			Alpha=phase/_fEffectButtonTime3;
		else
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
	}
	else 
		{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }


	if (currentScrollDirection != thumb_none) {
		if (isPressed(VK_LBUTTON)) {
			if (m_ftime >= scrollSpeed) {
				m_ftime = 0;
				if (currentScrollDirection == thumb_up) {
					if ( m_nTopItem > 0 ) {
						m_nTopItem--;
					}
				} else {
					if(m_nTopItem + int(sy/m_fStringHeight) < m_data.size()) {
						m_nTopItem++;
					}
				}
				if (m_bScroller) {
					m_fScrollerThumbPos = y + vScrollSY + 1 + float(m_nTopItem)/(m_data.size() - int(sy/m_fStringHeight))*(sy - 2*vScrollSY - vScrollThmbSY);
				}
			}
		} else {
			currentScrollDirection = thumb_none;
		}
	}

	float _sx_client = sx - vScrollSX;
	if(m_bScroller)
	{
		//нужен скроллер

		if (m_hTextureBG) {
			terRenderDevice->DrawSprite2(
				x + sx - vScrollSX, m_fScrollerThumbPos, vScrollThmbSX, vScrollThmbSY, thumbUV.x, thumbUV.y, thumbDUDV.x, thumbDUDV.y,
				m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
				thumbTexture, m_hTextureBG,
				mapTextureWeight, Alpha * scaleButtonAlpha, fmodf(m_ftime,1000)/1000,
				COLOR_MOD, ALPHA_ADDBLENDALPHA);
		} else {
			DrawSprite(x + sx - vScrollSX, m_fScrollerThumbPos, vScrollThmbSX, vScrollThmbSY, 
				thumbUV.x, thumbUV.y, thumbDUDV.x, thumbDUDV.y, 
				thumbTexture, sColor4c(255,255,255,Alpha*255) );
		}
//		draw_rect(Vect2f(x + sx - vScrollSX, m_fScrollerThumbPos), Vect2f(x+sx, m_fScrollerThumbPos+vScrollSY), sColor4f(0.5f, 0.5f, 0.5f, Alpha));
	}

	if (debug_show_intf_borders) {
		draw_rect_empty( Vect2i(x, y), Vect2i(x + sx,y + sy), sColor4f(1, 1, 0, 1) );
	}

	terRenderDevice->SetFont(m_hFont);

	int i, sz = m_data.size();
	float y_str = y;

	if (!m_bScroller) {
		y_str += sy - m_data.size() * m_fStringHeight;
	}

	for(i = m_nTopItem; i < sz; i++)
	{
		if(y_str+m_fStringHeight > y+sy)
			break;

		string toStr = getValidatedText(m_data[i], _sx_client - txtdx);
//		string toStr = getValidatedText(m_data[i], _sx_client - txtdx - m_fStringHeight/2);

		float yS = y_str + m_fStringHeight / 2 - m_hFont->GetHeight() / 2;

		if (m_hTextureBG) {
			terRenderDevice->OutText(
				x + txtdx,
				yS, (char*)toStr.c_str(), sColor4f(1, 1, 1, Alpha), SHELL_ALIGN_LEFT, ALPHA_ADDBLENDALPHA, m_hTextureBG, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
		} else if (m_attr->alnum) {
			OutText(
				x + txtdx, 
				yS, (char*)toStr.c_str(), &sColor4f(1, 1, 1, Alpha));
		}
		y_str += m_fStringHeight;
	}

	terRenderDevice->SetFont(0);
	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

void ChatWindow::OnLButtonDown(float _x, float _y)
{
	currentScrollDirection = CheckClick(_x, _y);
	if (currentScrollDirection == thumb_none) {
		if(m_handler && (state & SQSH_ENABLED))
			m_handler(this, EVENT_PRESSED, 0);
	}
}

void ChatWindow::OnLButtonDblClk(float _x, float _y)
{
	if(m_handler && (_x * terRenderDevice->GetSizeX() <= x+sx-vScrollSX) && (state & SQSH_ENABLED) && m_effect == 0) {
		m_handler(this, EVENT_DOUBLECLICK, VK_LBUTTON );
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
CSliderWindow::CSliderWindow(int id, CShellWindow* pParent, EVENTPROC p):CShellWindow(id, pParent, p)
{
	sounded = false;
	target=-1;
	bPress=0; pos=0;
	m_hTextureBG = 0;
	if( m_handler )
		m_handler(this, EVENT_CREATEWND, 0);
}
CSliderWindow::~CSliderWindow()
{
	_RELEASE( m_hTextureBG );
	_RELEASE( m_hTexture );
}
void CSliderWindow::draw(int bFocus)
{
	if( !(state & SQSH_VISIBLE) )
		return;

	m_ftime += frame_time.delta();

	float Alpha;

	if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
	{
		Alpha=0;
		float phase;
		if( m_effect==effectButtonsFadeIn )
			phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
		else
			phase = _shellIconManager.m_fEffectTime;
		if( phase<0 )
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		else if( phase<=_fEffectButtonTime1 );
		else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
		else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
			Alpha=phase/_fEffectButtonTime3;
		else
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
	}
	else 
		{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

	if (bPress) {
		if (isPressed(VK_LBUTTON)) {
			float mx = (gameShell->mousePosition().x + 0.5f)*terRenderDevice->GetSizeX();
			if (mx <= (x + thumbSize / 2)) {
				pos = 0;
			} else if (mx >= (x + sx - thumbSize / 2)) {
				pos = 1;
			} else {
				pos = (mx - x - thumbSize / 2) / (sx - thumbSize);
			}
			if(m_handler)
				m_handler(this, EVENT_SLIDERUPDATE, 0);
		} else {
			bPress = 0;
		}
	}

	int xc = (int)x + thumbSize / 2 + (sx - thumbSize) * pos;

	if( m_hTexture ) {
		if (m_hTextureBG) {
			float alpha = Alpha * (m_attr->txt_dx ? m_attr->txt_dx : scaleButtonAlpha);
			// draw background
			terRenderDevice->DrawSprite2(
				x, y, sx, sy, m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y,
				m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
				m_hTexture, m_hTextureBG,
				mapTextureWeight, alpha, fmodf(m_ftime,1000)/1000,
				COLOR_MOD, ALPHA_ADDBLENDALPHA);
			// draw pos
			terRenderDevice->DrawSprite2(
				xc-thumbSize/2, y, thumbSize, sy, thumbUV.x, thumbUV.y, thumbDUDV.x, thumbDUDV.y,
				m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
				m_hTexture, m_hTextureBG,
				mapTextureWeight, alpha, fmodf(m_ftime,1000)/1000,
				COLOR_MOD, ALPHA_ADDBLENDALPHA);
			if (arrowSize) {
				// draw left arrow
				terRenderDevice->DrawSprite2(
					x - arrowSize, y, arrowSize, sy, leftArrowUV.x, leftArrowUV.y, leftArrowDUDV.x, leftArrowDUDV.y,
					m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
					m_hTexture, m_hTextureBG,
					mapTextureWeight, alpha, fmodf(m_ftime,1000)/1000,
					COLOR_MOD, ALPHA_ADDBLENDALPHA);
				// draw right arrow
				terRenderDevice->DrawSprite2(
					x + sx, y, arrowSize, sy, rightArrowUV.x, rightArrowUV.y, rightArrowDUDV.x, rightArrowDUDV.y,
					m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
					m_hTexture, m_hTextureBG,
					mapTextureWeight, alpha, fmodf(m_ftime,1000)/1000,
					COLOR_MOD, ALPHA_ADDBLENDALPHA);
			}
		} else {
			// draw background
			DrawSprite(x, y, sx, sy, m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, m_hTexture, sColor4c(255, 255, 255, Alpha*255));
			// draw pos
			DrawSprite( xc-thumbSize/2, y, thumbSize, sy, thumbUV.x, thumbUV.y, thumbDUDV.x, thumbDUDV.y, m_hTexture, sColor4c(255, 255, 255, Alpha*255));
			if (arrowSize) {
				// draw left arrow
				DrawSprite(x - arrowSize, y, arrowSize, sy, leftArrowUV.x, leftArrowUV.y, leftArrowDUDV.x, leftArrowDUDV.y, m_hTexture, sColor4c(255, 255, 255, Alpha*255));
				// draw right arrow
				DrawSprite(x + sx, y, arrowSize, sy, rightArrowUV.x, rightArrowUV.y, rightArrowDUDV.x, rightArrowDUDV.y, m_hTexture, sColor4c(255, 255, 255, Alpha*255));
			}
		}
	}


	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}
void CSliderWindow::OnLButtonDown(float _x, float _y)
{
	if( bPress ) return;
	float mx = _x*terRenderDevice->GetSizeX();
	if (mx <= (x + thumbSize / 2)) {
		pos-=0.05f; 
		if( pos < 0 ) pos=0;
	} else if (mx >= (x + sx - thumbSize / 2)) {
		pos+=0.05f; 
		if( pos > 1 ) pos=1;
	} else {
		bPress = 1;
		pos = (mx - x - thumbSize / 2) / (sx - thumbSize);
	}
	if (sounded) {
		SND2DPlaySound("mainmenu_clock");
	}
	if(m_handler)
		m_handler(this, EVENT_SLIDERUPDATE, 0);
}
int CSliderWindow::HitTest(float xT, float yT)
{
	if((state & SQSH_VISIBLE) == 0)
		return 0;
	float xF = xT * (float)terRenderDevice->GetSizeX();
	float yF = yT * (float)terRenderDevice->GetSizeY();

	return xF>=(x-arrowSize) && xF<=(x+sx+arrowSize) && yF>=y && yF<=(y+sy);
}
void CSliderWindow::Load(const sqshControl* attr)
{
	CShellWindow::Load( attr );
	_RELEASE(m_hTextureBG);

	target = attr->target;
	arrowSize = absoluteX(m_attr->xstart);
	thumbSize = absoluteX(m_attr->ystart);

	if (m_attr->xstart) {
		leftArrowUV = relativeUV(m_attr->image_h._ix, m_attr->image_h._iy, m_hTexture);
		leftArrowDUDV = relativeUV(m_attr->image_h.ix, m_attr->image_h.iy, m_hTexture);
		rightArrowUV = relativeUV(m_attr->image_check._ix, m_attr->image_check._iy, m_hTexture);
		rightArrowDUDV = relativeUV(m_attr->image_check.ix, m_attr->image_check.iy, m_hTexture);
	}
	thumbUV = relativeUV(m_attr->image_disabled._ix, m_attr->image_disabled._iy, m_hTexture);
	thumbDUDV = relativeUV(m_attr->image_disabled.ix, m_attr->image_disabled.iy, m_hTexture);
	if (strlen(m_attr->image2.texture)) {
		m_hTextureBG = terVisGeneric->CreateTexture(getImageFileName(&(m_attr->image2)).c_str());
		m_vTexBGPos = relativeUV(m_attr->image2._ix, m_attr->image2._iy, m_hTextureBG);
		m_vTexBGSize = relativeUV(m_attr->image2.ix, m_attr->image2.iy, m_hTextureBG);
	}
}
void CSliderWindow::OnWindow(int enable)
{
	if( state & SQSH_ENABLED )
		if( target>0 )
		{
			CShellWindow *wnd=_shellIconManager.GetWnd(target);
			wnd->OnWindow(enable);
		}
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
CMultiTexWindow::~CMultiTexWindow() {
	if( m_hTexture2 ) m_hTexture2->Release(); m_hTexture2=0;
	if( m_hTexture3 ) m_hTexture3->Release(); m_hTexture3=0;
}

void CMultiTexWindow::LoadMenuWnd(const sqshControlContainer* attr)
{
	m_attr_cont = attr;
	init();
}
void CMultiTexWindow::Load(const sqshControl* attr)
{
	loadFlashingParams(attr);
	m_attr = attr;
	createHotKeyString();

	init();

	if(attr->sound && strlen(attr->sound))
		m_sound = attr->sound;
}
void CMultiTexWindow::init() {
	_RELEASE(m_hTexture2);
	_RELEASE(m_hTexture3);
	x = absoluteX(m_attr_cont->x);
	y = absoluteY(m_attr_cont->y);
	sx = absoluteX(m_attr_cont->sx);
	sy = absoluteY(m_attr_cont->sy);
	if(strlen(m_attr_cont->image.texture))
	{
		m_hTexture3 = terVisGeneric->CreateTexture( m_attr_cont->image.texture );
		SetTexPos(Vect2f(0,0), Vect2f(1, 1));
	}

	if(strlen(m_attr_cont->image2.texture))
		m_hTexture2 = terVisGeneric->CreateTexture( m_attr_cont->image2.texture );
	_RELEASE(m_hFont);
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont, defaultFontSize);
}

void CMultiTexWindow::draw(int bFocus)
{
	m_ftime += frame_time.delta();
	if((state & SQSH_VISIBLE) == 0)
		return;

	if( m_hTexture3 && m_hTexture2 ) // draw button
	{
		if (m_attr_cont->image2.hasBelligerentVersion) {
			DrawSprite(x, y, sx, sy, 0, 0, sx/128, sy/128, 
				m_hTexture3, sColor4c(255,255,255,100), fmodf(m_ftime,250)/250 );
			DrawSprite(x, y, sx, sy, 0, 0, 1, 1, 
				m_hTexture2, sColor4c(255,255,255,255), fmodf(m_ftime,250)/250 );
		} else {
			DrawSprite2(x, y, sx, sy, 0, 0, sx/128, sy/128, 
				m_hTexture3, m_hTexture2, sColor4c(255,255,255,100), fmodf(m_ftime,250)/250 );
		}
	}
	else if( m_hTexture3 )
	{
		if (m_attr_cont->image2.hasBelligerentVersion) {
			DrawSprite(x, y, sx, sy, 0, 0, 1, 1, 
				m_hTexture3, sColor4c(255,255,255,50), fmodf(m_ftime,250)/250 );
		} else {
			DrawSprite(x, y, sx, sy, 0, 0, sx/128, sy/128, 
				m_hTexture3, sColor4c(255,255,255,50), fmodf(m_ftime,250)/250 );
		}
	}

	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}
////////////////////////////////////////////////////////////////////////////////////////////////
//
CShowMapWindow::CShowMapWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p), worldID(-1) {
	m_hTextureBG = 0;
	worldID = -1;
}
CShowMapWindow::~CShowMapWindow() {
	RELEASE(m_hTextureBG);
}
void CShowMapWindow::Load(const sqshControl* attr) {
	CShellWindow::Load(attr);
	_RELEASE(m_hTextureBG);
	if (strlen(m_attr->image_h.texture)) {
		m_hTextureBG = terVisGeneric->CreateTexture(getImageFileName(&(m_attr->image_h)).c_str());
		m_vTexBGPos = relativeUV(m_attr->image_h._ix, m_attr->image_h._iy, m_hTextureBG);
		m_vTexBGSize = relativeUV(m_attr->image_h.ix, m_attr->image_h.iy, m_hTextureBG);
	}
}
void CShowMapWindow::setWorldID(int id) {
	worldID = id;
	RELEASE(m_hTexture);
	if (worldID >= 0) {
		string mapPath;
		if ( isWorldIDValid(worldID) ) {
			mapPath = GetTargetName( worldID, "map.tga" );
		} else {
			mapPath = "RESOURCE\\Icons\\MainMenu\\UnknownWorld.tga";
		}
		m_hTexture = terVisGeneric->CreateTexture( mapPath.c_str() );
		x = absoluteX(m_attr->x);
		y = absoluteY(m_attr->y);
		sx = absoluteX(m_attr->sx);
		sy = absoluteY(m_attr->sy);
		if (m_hTexture->GetWidth() > m_hTexture->GetHeight()) {
			y += sy / 2;
			sy *= float(m_hTexture->GetHeight()) / float(m_hTexture->GetWidth());
			y -= sy / 2;
		} else if (m_hTexture->GetWidth() < m_hTexture->GetHeight()) {
			x += sx / 2;
			sx *= float(m_hTexture->GetWidth()) / float(m_hTexture->GetHeight());
			x -= sx / 2;
		}
	}
}

void CShowMapWindow::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
	{
		if( m_hTexture ) m_hTexture->Release(); m_hTexture=0;
		return;
	}
//	draw_rect_empty(Vect2i(x,y), Vect2i(x+sx,y+sy), sColor4f(1, 1, 1, 1));
	if (worldID >= 0) {
		if( !m_hTexture ) {
			const char* mapPath = safeGetTargetName( worldID, "map.tga" );
			if (mapPath) {
				m_hTexture = terVisGeneric->CreateTexture( mapPath );
			} else {
				return;
			}
		}

		m_ftime += frame_time.delta();

		if ( (state & SQSH_VISIBLE) && m_hTexture ) {
			float Alpha;

			if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
			{
				Alpha=0;
				float phase;
				if( m_effect==effectButtonsFadeIn )
					phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
				else
					phase = _shellIconManager.m_fEffectTime;
				if( phase<0 )
					{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
				else if( phase<=_fEffectButtonTime1 );
				else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
				else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
					Alpha=phase/_fEffectButtonTime3;
				else
					{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
			}
			else 
				{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

			if (m_hTextureBG) {
				terRenderDevice->DrawSprite2(
					x, y, sx, sy, 0, 0, 1, 1,
					m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
					m_hTexture, m_hTextureBG,
					mapTextureWeight, Alpha, fmodf(m_ftime,1000)/1000,
					COLOR_MOD, ALPHA_ADDBLENDALPHA);
			} else {
				DrawSprite( x, y, sx, sy, 0, 0, 1, 1, m_hTexture, sColor4c(255, 255, 255, 255*Alpha) );
			}
		}
	}

	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

int CMoveButton::snd=0;
void CMoveButton::Load(const sqshControl* attr)
{
	snd=0;
	CShellWindow::Load(attr);

	xstart=round(attr->xstart * terRenderDevice->GetSizeX() / SQSH_COORD_WIDTH_SCALE);
	ystart=round(attr->ystart * terRenderDevice->GetSizeY() / SQSH_COORD_HEIGHT_SCALE);
}
void CMoveButton::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
		return;

	m_ftime += frame_time.delta();

	if( (state & SQSH_VISIBLE) && m_hTexture ) // draw button
	{
		float phase = 0;
/*
		if( m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut )
		{
			if( m_effect==effectButtonsFadeIn )
				phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
			else
				phase = _shellIconManager.m_fEffectTime;
			if( phase<0 )
				{ phase=1; if (!OnEffectStop(m_effect)) return; }
			else if( phase<(_fEffectButtonTime1+_fEffectButtonTime2) )
			{
				phase=1-phase/(_fEffectButtonTime1+_fEffectButtonTime2);
				if( !snd && phase<0.5f ) SND2DPlaySound( "menu_rectangle"), snd=1;
			}
			else if( phase<_fEffectButtonTotalTime )
				phase=0;
			else 
				{ phase=0; if (!OnEffectStop(m_effect)) return; }
		}
		if( m_effect==effectCtrlRollIn || m_effect==effectCtrlRollOut )
		{
			if( m_effect==effectCtrlRollIn )
				phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
			else
				phase = _shellIconManager.m_fEffectTime;
			if( phase<0 )
				{ phase=1; if (!OnEffectStop(m_effect)) return; }
			else if( phase<(_fEffectButtonTime1+_fEffectButtonTime2) )
			{
				phase=1-phase/(_fEffectButtonTime1+_fEffectButtonTime2);
				if( !snd && phase<0.5f ) SND2DPlaySound( "menu_rectangle"), snd=1;
			}
			else if( phase<_fEffectButtonTotalTime )
				phase=0;
			else 
				{ phase=0; if (!OnEffectStop(m_effect)) return; }
		}
*/		if( m_effect==effectCtrlRollIn || m_effect==effectCtrlRollOut )
		{
			if( m_effect==effectCtrlRollIn )
				phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
			else
				phase = _shellIconManager.m_fEffectTime;

			if( phase<0)
				{ phase=1; m_effect=0; }
			else if( phase<(_fEffectButtonTime1+_fEffectButtonTime2) )
			{
				phase=1-phase/(_fEffectButtonTime1+_fEffectButtonTime2);
				if( !snd && phase<0.5f ) SND2DPlaySound( "menu_rectangle"), snd=1;
			}
			else if( phase<=_fEffectButtonTotalTime )
				phase=0;
			else 
				{ phase=0; m_effect=0; }
		}
		else
			snd=0;
		int _x = round( x+phase*(xstart-x) );
		int _y = round( y+phase*(ystart-y) );
		DrawSprite(_x, _y, sx, sy, 
			m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, 
			m_hTexture, sColor4c(255,255,255,255), fmod(m_ftime,1000.f)/1000.f );
	}

	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}
/////////////////////////////////
CPortraitWindow::CPortraitWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) {
	m_hTextureBG = 0;
}
CPortraitWindow::~CPortraitWindow() {
	RELEASE(m_hTextureBG);
}
void CPortraitWindow::Load(const sqshControl* attr) {
	CShellWindow::Load(attr);
	_RELEASE(m_hTextureBG);
	if (m_hTexture) {
		if (m_attr->image.hasResolutionVersion) {
			m_vTexPos[0] = relativeUV(m_attr->image._ix, m_attr->image._iy, m_hTexture);
			m_vTexPos[1] = relativeUV(m_attr->image.ix, m_attr->image.iy, m_hTexture);
		} else {
			m_vTexPos[0] = Vect2f(m_attr->image._ix / m_hTexture->GetWidth(), m_attr->image._iy / m_hTexture->GetHeight());
			m_vTexPos[1] = Vect2f(m_attr->image.ix / m_hTexture->GetWidth(), m_attr->image.iy / m_hTexture->GetHeight());
		}
	}
	if (strlen(m_attr->image_h.texture)) {
		m_hTextureBG = terVisGeneric->CreateTexture(getImageFileName(&(m_attr->image_h)).c_str());
		m_vTexBGPos = relativeUV(m_attr->image_h._ix, m_attr->image_h._iy, m_hTextureBG);
		m_vTexBGSize = relativeUV(m_attr->image_h.ix, m_attr->image_h.iy, m_hTextureBG);
	}
}
void CPortraitWindow::setTexture(const char* name) {
	RELEASE(m_hTexture);
	if (strlen(name)) {
		m_hTexture = terVisGeneric->CreateTexture( getImageFileName(&(m_attr->image), name).c_str() );
		m_vTexPos[0] = relativeUV(m_attr->image._ix, m_attr->image._iy, m_hTexture);
		m_vTexPos[1] = relativeUV(m_attr->image.ix, m_attr->image.iy, m_hTexture);
	}
}

void CPortraitWindow::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
	{
		return;
	}
//	draw_rect_empty(Vect2i(x,y), Vect2i(x+sx,y+sy), sColor4f(1, 1, 1, 1));
	m_ftime += frame_time.delta();

	if ( (state & SQSH_VISIBLE) && m_hTexture ) {
		float Alpha;

		if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
		{
			Alpha=0;
			float phase;
			if( m_effect==effectButtonsFadeIn )
				phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
			else
				phase = _shellIconManager.m_fEffectTime;
			if( phase<0 )
				{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
			else if( phase<=_fEffectButtonTime1 );
			else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
			else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
				Alpha=phase/_fEffectButtonTime3;
			else
				{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		}
		else 
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

		if (m_hTextureBG) {
			terRenderDevice->DrawSprite2(
				x, y, sx, sy, m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y,
				m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
				m_hTexture, m_hTextureBG,
				mapTextureWeight, Alpha, fmodf(m_ftime,1000)/1000,
				COLOR_MOD, ALPHA_ADDBLENDALPHA);
		} else {
			terRenderDevice->DrawSprite(
				x, y, sx, sy, m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y,
				m_hTexture, sColor4c(255, 255, 255, 255*Alpha) );
		}
	}

	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

/////////////////////////////////
CLogoWindow::CLogoWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) {
	m_hTextureBG = 0;
	race = 0;
}
CLogoWindow::~CLogoWindow() {
	RELEASE(m_hTextureBG);
}
void CLogoWindow::Load(const sqshControl* attr) {
	CShellWindow::Load(attr);
	_RELEASE(m_hTextureBG);
	if (m_hTexture) {
		for (int i = 0; i < 3; i++) {
			m_vTexPosRace[0][i] = relativeUV(imageRacePos[i].x, imageRacePos[i].y, m_hTexture);
			m_vTexPosRace[1][i] = relativeUV(imageRaceSize[i].x, imageRaceSize[i].y, m_hTexture);
		}
	}
	if (strlen(m_attr->image_h.texture)) {
		m_hTextureBG = terVisGeneric->CreateTexture(getImageFileName(&(m_attr->image_h)).c_str());
		m_vTexBGPos = relativeUV(m_attr->image_h._ix, m_attr->image_h._iy, m_hTextureBG);
		m_vTexBGSize = relativeUV(m_attr->image_h.ix, m_attr->image_h.iy, m_hTextureBG);
	}
}
void CLogoWindow::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
	{
		return;
	}
//	draw_rect_empty(Vect2i(x,y), Vect2i(x+sx,y+sy), sColor4f(1, 1, 1, 1));
	m_ftime += frame_time.delta();

	if ( (state & SQSH_VISIBLE) && m_hTexture && race >= 0) {
		float Alpha;

		if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
		{
			Alpha=0;
			float phase;
			if( m_effect==effectButtonsFadeIn )
				phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
			else
				phase = _shellIconManager.m_fEffectTime;
			if( phase<0 )
				{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
			else if( phase<=_fEffectButtonTime1 );
			else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
			else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
				Alpha=phase/_fEffectButtonTime3;
			else
				{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		}
		else 
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

		if (m_hTextureBG) {
			terRenderDevice->DrawSprite2(
				x, y, sx, sy, m_vTexPosRace[0][race].x, m_vTexPosRace[0][race].y, m_vTexPosRace[1][race].x, m_vTexPosRace[1][race].y,
				m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
				m_hTexture, m_hTextureBG,
				mapTextureWeight, Alpha, fmodf(m_ftime,1000)/1000,
				COLOR_MOD, ALPHA_ADDBLENDALPHA);
		} else {
			DrawSprite( x, y, sx, sy, 0, 0, 1, 1, m_hTexture, sColor4c(255, 255, 255, 255*Alpha) );
		}
	}

	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
int	CScaleButton::snd1=0,CScaleButton::snd2=0;
CScaleButton::CScaleButton(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) 
{ 
	soundEnabled = false;
	size=0;
	m_hTexture2 = terVisGeneric->CreateTexture( "RESOURCE\\ICONS\\MAINMENU\\lightmap.tga" );
	m_hTexture3 = terVisGeneric->CreateTexture( "RESOURCE\\ICONS\\MAINMENU\\tv.avi" ); 
	m_hTextureBG = 0;

}
CScaleButton::~CScaleButton()
{
	if( m_hTexture2 ) m_hTexture2->Release(); m_hTexture2=0;
	if( m_hTexture3 ) m_hTexture3->Release(); m_hTexture3=0;
	RELEASE(m_hTextureBG);
}
void CScaleButton::Load(const sqshControl* attr)
{
	CShellWindow::Load( attr );
	size = absoluteY(attr->ystart);
	_RELEASE(m_hTextureBG);
//	m_vTexPos[1] = Vect2f(attr->image.ix, attr->image.iy);
	if (m_hTexture) {
		m_vTexPos[1] = relativeUV(attr->image.ix, attr->image.iy, m_hTexture);
	}
	if (strlen(m_attr->image_h.texture)) {
		m_hTextureBG = terVisGeneric->CreateTexture(getImageFileName(&(m_attr->image_h)).c_str());
		m_vTexBGPos = relativeUV(m_attr->image_h._ix, m_attr->image_h._iy, m_hTextureBG);
		m_vTexBGSize = relativeUV(m_attr->image_h.ix, m_attr->image_h.iy, m_hTextureBG);
	}
	soundEnabled = attr->alnum;
}

void CScaleButton::setTexture(const char* name) {
	RELEASE(m_hTexture);
	if (name) {
		m_hTexture = terVisGeneric->CreateTexture( getImageFileName(&(m_attr->image), name).c_str() );
//		m_vTexPos[0] = relativeUV(m_attr->image._ix, m_attr->image._iy, m_hTexture);
//		m_vTexPos[1] = relativeUV(m_attr->image.ix, m_attr->image.iy, m_hTexture);
	}
}

void CScaleButton::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
		return;

	m_ftime += frame_time.delta();

	if( (state & SQSH_VISIBLE) ) // draw button
	{
		int iColor=255,Color3=0;
		float phase = 0;
		float _sx=sx, _sy=sy;
		float _size=size;
		if( m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut )
		{
			if( m_effect==effectButtonsFadeIn )
				phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
			else
				phase = _shellIconManager.m_fEffectTime;

			if ( m_hTexture ) {
				iColor=round(255*phase/(_fEffectButtonTime1+_fEffectButtonTime2));
				if( iColor>255 ) iColor=255; else if( iColor<0 ) iColor=0;
				if( phase<0 )
					{ phase=1; if (!OnEffectStop(m_effect)) return; }
				else if( phase<=_fEffectButtonTime1 )
				{
					if( !snd1 && soundEnabled ) SND2DPlaySound( "menu_button_switth_1"), snd1=1;
					phase/=_fEffectButtonTime1, _sy=sy*0.1f, _sx=sx*phase, _size=0;
				}
				else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 )
				{
					if( !snd2 && soundEnabled ) SND2DPlaySound( "menu_button_switth_2"), snd2=1;
					phase/=_fEffectButtonTime2, _sy=sy*phase, _size=size*phase, Color3=round(128*phase);
				}
				else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
					Color3=128*(1-phase/_fEffectButtonTime3),phase=1;
				else
					{ phase=1; if (!OnEffectStop(m_effect)) return; }

			} else {
				iColor=round(255*phase/(_fEffectButtonTime1+_fEffectButtonTime2)) - 90;
				if( iColor>255 ) iColor=255; else if( iColor<0 ) iColor=0;
				if( phase<0 )
					{ phase=1; if (!OnEffectStop(m_effect)) return; }
				else if( phase<=_fEffectButtonTime1 )
				{
					if( !snd1 && soundEnabled ) SND2DPlaySound( "menu_button_switth_1"), snd1=1;
					phase/=_fEffectButtonTime1, _sy=sy*0.1f, _sx=sx*phase, _size=0;
				}
				else if( (phase-=_fEffectButtonTime1)<=(_fEffectButtonTime2 + _fEffectButtonTime3) )
				{
					if( !snd2 && soundEnabled) SND2DPlaySound( "menu_button_switth_2"), snd2=1;
					phase/=(_fEffectButtonTime2 + _fEffectButtonTime3), _sy=sy*phase, _size=size*phase, Color3=round(128*phase);
				}
				else
					{ phase=1; if (!OnEffectStop(m_effect)) return; }
			}


		}
		else
			snd1=snd2=0;
		int _x = x+sx*0.5f-_sx*0.5f;
		int _y = y+sy*0.5f-_sy*0.5f;
		if ( m_hTexture ) {
			if (m_hTextureBG) {
				float alpha = float(iColor) / 256.0f * (m_attr->txt_dx ? m_attr->txt_dx : scaleButtonAlpha); 
				terRenderDevice->DrawSprite2(
					_x, _y, _sx, _sy, m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y,
					m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
					m_hTexture, m_hTextureBG,
					mapTextureWeight, alpha, fmodf(m_ftime,1000)/1000,
					COLOR_MOD, ALPHA_ADDBLENDALPHA);
			} else {
				DrawSprite( _x, _y, _sx, _sy, 
					m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, 
					m_hTexture, sColor4c(255,255,255,iColor), fmod(m_ftime,1000.f)/1000.f);
			}
		}
		if( m_hTexture && m_hTexture3 && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut) )
		{
			DrawSprite( _x, _y, _sx, _size, 
				0, 0, 1, 1, 
				m_hTexture3, sColor4c(255,255,255,Color3), fmod(m_ftime,1000.f)/1000.f, ALPHA_ADDBLENDALPHA );
		}
		if( m_hTexture2 && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut) )
		{
			if (!m_hTexture) {
				if (m_hTexture3) {
					float alpha = float(255-iColor) / 256.0f * (m_attr->txt_dx ? m_attr->txt_dx : scaleButtonAlpha); 
					terRenderDevice->DrawSprite2(
						_x, _y, _sx, _sy, 0, 0, 1, 1,
						0, 0, 1, 1,
						m_hTexture2, m_hTexture3,
						scaleButtonWeight, alpha, fmodf(m_ftime,1000)/1000,
						COLOR_MOD, ALPHA_ADDBLENDALPHA);
				} else {
					terRenderDevice->DrawSprite2(_x, _y, _sx, _sy,
						0, 0, 1, 1, 0, 0, 1, 1, m_hTexture2, m_hTexture3, sColor4c(255,255,255,255-iColor), fmod(m_ftime,1000.f)/1000.f, COLOR_MOD, ALPHA_ADDBLENDALPHA);
				}
			} else {
					terRenderDevice->DrawSprite(_x, _y, _sx, _sy, 
						0, 0, 1, 1, m_hTexture2, sColor4c(255,255,255,255-iColor), fmod(m_ftime,1000.f)/1000.f, ALPHA_ADDBLENDALPHA );
			}
		}
	}

	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

void CPushScaleButton::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
		return;
	if( !_shellIconManager.IsEffect() ) 
	{ // draw button
		CShellPushButton::draw(bFocus);
		return;
	}

	m_ftime += frame_time.delta();

	if( (state & SQSH_VISIBLE) && m_hTexture ) // draw button
	{
		float phase = 0;
		float _sx=sx, _sy=sy;
		if( m_effect==effectButtonsFadeIn )
		{
			phase = (_shellIconManager.m_fEffectTime-_fEffectButtonTime2)/_fEffectButtonTime1;
			if( phase<0 ) 
			{
				phase=-phase; 
				if( phase>1 ) { phase=1; if (!OnEffectStop(m_effect)) return; }
				_sy=sy*phase;
			}
			else
			{
				_sx=sx*(1-phase);
				_sy=sy*0.1f;
			}
		}
		else if( m_effect==effectButtonsFadeOut )
		{
			phase = (_shellIconManager.m_fEffectTime-_fEffectButtonTime2)/_fEffectButtonTime1;
			if( phase<0 ) 
			{
				phase=-phase; 
				if( phase>1 ) phase=1;
				phase = 1-phase;
				_sx=sx*phase;
				_sy=sy*0.1f;
			}
			else
			{
				_sy=sy*phase;
			}
		}
		int _x = x+sx*0.5f-_sx*0.5f;
		int _y = y;
		DrawSprite(_x, _y, _sx, _sy, 
			m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, 
			m_hTexture, sColor4c(255,255,255,255), 0 );
	}

	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
CEditWindow::CEditWindow(int id, CShellWindow* pParent, EVENTPROC p):CShellWindow(id, pParent, p)
{
	m_hFont = 0;
}
CEditWindow::~CEditWindow()
{
	_RELEASE(m_hFont);
}

void CEditWindow::Load(const sqshControl* attr)
{
	CShellWindow::Load(attr);
	if (m_attr->font_group == 4) {
		txtdy = 0;
	} else {
		txtdy = absoluteY(m_attr->txt_dy);
	}
	if (m_hTexture) {
		float tx = absoluteX(m_attr->image.dx);
		float ty = absoluteY(m_attr->image.dy);
		float tsx = absoluteX(m_attr->image.dsx);
		float tsy = absoluteY(m_attr->image.dsy);

		uv.x = (x - tx) / tsx;
		uv.y = (y - ty + txtdy) / tsy;
//		dudv.x =  (float)m_hTexture->GetWidth() / tsx;
//		dudv.y = (float)m_hTexture->GetHeight() / tsy;
		dudv.x = 1.0f / tsx;
		dudv.y = 1.0f / tsy;
	}
	_RELEASE(m_hFont);
	if (m_attr->font_group == 4) {
		m_hFont = terVisGeneric->CreateFont(sqshShellMainFont2, 16);
	} else {
		m_hFont = terVisGeneric->CreateFont(sqshShellMainFont2, editBoxFontSize);
	}
}
void CEditWindow::draw(int bFocus)
{
	if(state & SQSH_VISIBLE)
	{
		//контур
//		draw_rect_empty(Vect2f(x, y), Vect2f(x+sx, y+sy), sColor4f(0.5f, 0.5f, 0.5f, 1));
		m_ftime += frame_time.delta();

		float Alpha;

		if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
		{
			Alpha=0;
			float phase;
			if( m_effect==effectButtonsFadeIn )
				phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
			else
				phase = _shellIconManager.m_fEffectTime;
			if( phase<0 )
				{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
			else if( phase<=_fEffectButtonTime1 );
			else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
			else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
				Alpha=phase/_fEffectButtonTime3;
			else
				{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
		}
		else 
			{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }

		float x_caret = x+0.001;
		
		Alpha *= pushButtonAlpha;

		sColor4f clr(1, 1, 1, Alpha);
		if (!isEnabled()) {
			clr *= pushButtonDisabled;
			clr.a = Alpha * pushButtonDisabledAlpha;
		}

		if (debug_show_intf_borders) {
			draw_rect_empty( Vect2i(x, y), Vect2i(x + sx,y + sy), sColor4f(1, 1, 0, 1) );
		}

		if(!m_data.empty())
		{
			terRenderDevice->SetFont(m_hFont);
			m_data = getValidatedText(m_data, sx);
			char *c = (char*)m_data.c_str();
//			terRenderDevice->SetFont(hFontMainmenu2);
			if (m_hTexture) {
				terRenderDevice->OutText(x, y + txtdy, c, clr, m_attr->txt_align, ALPHA_ADDBLENDALPHA, m_hTexture, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
			} else {
				OutText(x, y + txtdy, c, &clr);
			}
			x_caret += terRenderDevice->GetFontLength(c);
			terRenderDevice->SetFont(0);
		}

		if(isEnabled() && bFocus && fmodf(m_ftime,1000) > 500){
			terRenderDevice->SetFont(m_hFont);
			OutText(x_caret, y + txtdy, "|", &clr);
			terRenderDevice->SetFont(0);
		}
//			draw_line(Vect2f(x_caret, y), Vect2f(x_caret, y+sy), sColor4f(1, 1, 1, Alpha));
	}
}
void CEditWindow::OnChar(char key)
{
/*
	if(key == VK_BACK)
		m_data = m_data.substr(0, m_data.length()-1);

	if((key >= '0') && (key <= 'z')) {
		m_data += key;
	} else if (key == ' ') {
		m_data += ' ';
	}
*/
	if (isEnabled()) {
		terRenderDevice->SetFont(m_hFont);
		float width = terRenderDevice->GetFontLength((char*)m_data.c_str());
		if (key == VK_BACK) {
			m_data = m_data.substr(0, m_data.length()-1);
		} else if (key == VK_RETURN && m_handler) {
			m_handler(this, EVENT_DOUBLECLICK, 0);
		} else if ( width + terRenderDevice->GetCharLength(key) < sx ) {
			if (m_attr->alnum) {
				if ( IsCharAlphaNumeric(key) ) {
					m_data += key;
				}
			} else {
				BYTE c = (unsigned char) key;
				if (c >= 32) {
					m_data += key;
				}
			}
		}
		terRenderDevice->SetFont(0);
	}
}


CChatInGameEditWindow::CChatInGameEditWindow(int id, CShellWindow* pParent, EVENTPROC p) : CEditWindow(id, pParent, p) {
	alliesOnlyMode = false;
	m_hPopupTexture = 0;
}
CChatInGameEditWindow::~CChatInGameEditWindow() {
	_RELEASE(m_hPopupTexture);
}

void CChatInGameEditWindow::Load(const sqshControl* attr) {
	CEditWindow::Load(attr);
	_RELEASE(m_hPopupTexture);
	m_hPopupTexture = terVisGeneric->CreateTexture(sPopupTexture);
}

void CChatInGameEditWindow::draw(int bFocus) {
	if(state & SQSH_VISIBLE) {
		if (!bFocus) {
			Show(0);
			_shellIconManager.hideChatInfo();
		} else {
				//контур
		//		draw_rect_empty(Vect2f(x, y), Vect2f(x+sx, y+sy), sColor4f(0.5f, 0.5f, 0.5f, 1));
				m_ftime += frame_time.delta();

				float Alpha;

				if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
				{
					Alpha=0;
					float phase;
					if( m_effect==effectButtonsFadeIn )
						phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
					else
						phase = _shellIconManager.m_fEffectTime;
					if( phase<0 )
						{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
					else if( phase<=_fEffectButtonTime1 );
					else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
					else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
						Alpha=phase/_fEffectButtonTime3;
					else
						{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }
				}
				else 
					{ Alpha=1.0f; if (!OnEffectStop(m_effect)) return; }


				terRenderDevice->DrawSprite(x, y, sx, sy, 0, 0, 1, 1, m_hPopupTexture, sColor4c(255, 255, 255, Alpha*255));

				float x_caret = x+0.001;
				
				Alpha *= pushButtonAlpha;

				sColor4f clr(1, 1, 1, Alpha);
				if (!isEnabled()) {
					clr *= pushButtonDisabled;
					clr.a = Alpha * pushButtonDisabledAlpha;
				}

				if (debug_show_intf_borders) {
					draw_rect_empty( Vect2i(x, y), Vect2i(x + sx,y + sy), sColor4f(1, 1, 0, 1) );
				}

		//			if(!m_data.empty())
		//			{
					terRenderDevice->SetFont(m_hFont);
					m_data = getValidatedText(m_data, sx);
					string toScr = getModePrefix() + m_data;
					char *c = (char*)toScr.c_str();
		//			terRenderDevice->SetFont(hFontMainmenu2);
					if (m_hTexture) {
						terRenderDevice->OutText(x, y + txtdy, c, clr, m_attr->txt_align, ALPHA_ADDBLENDALPHA, m_hTexture, COLOR_MOD, uv, dudv, fmodf(m_ftime,1000)/1000, pushButtonTextureWeight);
					} else {
						OutText(x, y + txtdy, c, &clr);
					}
					x_caret += terRenderDevice->GetFontLength(c);
					terRenderDevice->SetFont(0);
		//			}

				if(isEnabled() && bFocus && fmodf(m_ftime,1000) > 500)
					draw_line(Vect2f(x_caret, y), Vect2f(x_caret, y+sy), sColor4f(1, 1, 1, Alpha));
		}
	}
}

void CChatInGameEditWindow::OnChar(char key)
{
	if (isEnabled()) {
		terRenderDevice->SetFont(m_hFont);
		string toScr = getModePrefix() + m_data;
		float width = terRenderDevice->GetFontLength((char*)toScr.c_str());
		if (key == VK_BACK) {
			m_data = m_data.substr(0, m_data.length()-1);
		} else if (key == VK_RETURN && m_handler) {
			m_handler(this, EVENT_DOUBLECLICK, 0);
		} else if ( width + terRenderDevice->GetCharLength(key) < sx ) {
			if (m_attr->alnum) {
				if ( IsCharAlphaNumeric(key) ) {
					m_data += key;
				}
			} else {
				BYTE c = (unsigned char) key;
				if (c >= 32) {
					m_data += key;
				}
			}
		}
		terRenderDevice->SetFont(0);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// 

CSplashWindow::CSplashWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p)
{
	m_alpha = 0;

	if( m_handler )
		m_handler(this, EVENT_CREATEWND, 0);
}
void CSplashWindow::LoadMenuWnd(const sqshControlContainer* attr) {
	_RELEASE(m_hTexture);

	m_attr_cont = attr;
	x = absoluteX(m_attr_cont->x);
	y = absoluteY(m_attr_cont->y);
	sx = absoluteX(m_attr_cont->sx);
	sy = absoluteY(m_attr_cont->sy);
	if(strlen(attr->image.texture))
	{
		m_hTexture = terVisGeneric->CreateTexture( getImageFileName(&(attr->image)).c_str() );
		m_vTexPos[0] = relativeUV(attr->image._ix, attr->image._iy, m_hTexture);
		if (attr->sx > 2.0f || attr->sy > 2.0f) {
			m_vTexPos[1] = Vect2f(
				attr->sx / 1024.0f * (float)terRenderDevice->GetSizeX() / (float)m_hTexture->GetWidth(),
				attr->sy / 768.0f * (float)terRenderDevice->GetSizeY() / (float)m_hTexture->GetHeight() );
		} else {
			m_vTexPos[1] = Vect2f(
				attr->image.ix / (float)m_hTexture->GetWidth(),
				attr->image.iy / (float)m_hTexture->GetHeight() );
		}
	}
}
void CSplashWindow::Show(int bShow)
{
	if(bShow)
	{
		m_alpha = 0;
		OnEffectStart(effectSplashFadeIn);

		state |= SQSH_VISIBLE;

		if(m_handler)
			m_handler(this, EVENT_SHOWWND, 0);

	}
	else
	{
		//m_alpha = 255;
		OnEffectStart(effectSplashFadeOut);
	}
}
void CSplashWindow::draw(int bFocus)
{
	if(!m_effect)
	{
		CShellWindow::draw(bFocus);
	}
	else
	{
		if(state & SQSH_VISIBLE)
		{
			if(m_effect == effectSplashFadeIn)
			{
				m_alpha += alpha_delta;
				if(m_alpha > 255)
				{
					m_alpha = 255;
					m_effect = 0;
				}
			}
			else
			{
				m_alpha -= alpha_delta;
				if(m_alpha < 0)
				{
					m_alpha = 0;
					m_effect = 0;

					state &= ~SQSH_VISIBLE;
					if(m_handler)
						m_handler(this, EVENT_HIDEWND, 0);
				}
			}

			DrawSprite(x, y, sx, sy, 
				m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, m_hTexture, sColor4c(255, 255, 255, m_alpha));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
void CPlayerColorWnd::draw(int bFocus) {
	if (state & SQSH_VISIBLE) {
		sColor4c color = sColor4c(universe()->activePlayer()->unitColor());
		DrawSprite(x, y, sx, sy, m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, m_hTexture, color);
	}
}

CProgressEnergy::~CProgressEnergy() {
	_RELEASE(m_hTexture_h);
}

void CProgressEnergy::Load(const sqshControl* attr) {
	CShellWindow::Load(attr);

	_RELEASE(m_hTexture_h);

	xstart = absoluteX(m_attr->xstart);
	arrowSx = absoluteX(m_attr->txt_dx);
	arrowSy = absoluteY(m_attr->txt_dy);

	m_vTexPos[0] = relativeUV(attr->image._ix, attr->image._iy, m_hTexture);
	m_vTexPos[1] = relativeUV(attr->image.ix, attr->image.iy, m_hTexture);

	m_hTexture_h = terVisGeneric->CreateTexture( getImageFileName(&(attr->image_h)).c_str() );
	m_vTexPos_h[0] = relativeUV(attr->image_h._ix, attr->image_h._iy, m_hTexture_h);
	m_vTexPos_h[1] = relativeUV(attr->image_h.ix, attr->image_h.iy, m_hTexture_h);

}
void CProgressEnergy::draw(int bFocus)
{
	if(state & SQSH_VISIBLE)
	{
		float dt = frame_time.delta();
		dt = dt > collectedProgressRelaxationTime ? 0.0f : (1.0f - dt / collectedProgressRelaxationTime);

		showedProduced = produced * (1.0f - dt) + showedProduced * dt;
		showedUsed = requested * (1.0f - dt) + showedUsed * dt;

		int state = 0;
		float rem = 0;
		if (showedProduced > showedUsed) {
			float test = 3.0f * (showedProduced - showedUsed) / showedProduced;
			state = round(test - 0.5f);
			rem = test - state;
		} else if (showedProduced * 2.0f < showedUsed || showedProduced == 0) {
			state = -3;
		} else if (showedProduced < showedUsed) {
			float test = 3.0f * (showedProduced - showedUsed) / showedProduced;
			state = round(test + 0.5f);
			rem = state - test;
		}

		if (state > 0) {
			int i;
			for (i = 0; i < state; i++) {
				DrawSprite(sx + xstart * i, sy, arrowSx, arrowSy, 
					m_vTexPos_h[0].x, m_vTexPos_h[0].y, m_vTexPos_h[1].x, m_vTexPos_h[1].y, m_hTexture_h);
			}
			if (state < 3) {
				DrawSprite(sx + xstart * i, sy, arrowSx, arrowSy, 
					m_vTexPos_h[0].x, m_vTexPos_h[0].y, m_vTexPos_h[1].x, m_vTexPos_h[1].y, m_hTexture_h, sColor4c(255,255,255,round(255.0f * rem)));
			}
		} else if (state < 0) {
			int i;
			for (i = 0; i < -state; i++) {
				DrawSprite(x + xstart * (2 - i), y, arrowSx, arrowSy, 
					m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, m_hTexture);
			}
			if (state > -3) {
				DrawSprite(x + xstart * (2 - i), y, arrowSx, arrowSy, 
					m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, m_hTexture, sColor4c(255,255,255,round(255.0f * rem)));
			}
		}
	}
}

void FormatEnergyText(char* cb, void* param)
{
	terUnitBase* pFrame = universe()->activePlayer()->frame();
	if(pFrame)
	{
		terEnergyDataType& t = universe()->activePlayer()->energyData();
		_shellIconManager.FormatMessageText("<Energy_bar>", cb, t.accumulated(), t.capacity(), t.produced()*10, t.used()*10, (t.produced() - t.used())*10, t.efficiency()); 
	} else {
		_shellIconManager.FormatMessageText("<Energy_bar>", cb, 0, 0, 0, 0, 0, 0); 
	}
}
void CProgressEnergy::OnWindow(int enable)
{
	if(!terShowTips)
		return;

	CInfoWindow* pWnd;
	if(pWnd = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID))
	{
		pWnd->Show(enable);

		if(enable) {
			terPlayer* player = universe()->activePlayer();
			terUnitBase* frame = player->frame();
			if(frame) {
				pWnd->SetText("<Energy_bar>", FormatEnergyText);
			} else {
				pWnd->SetText("");
			}
		}
	}
}

void outTextOutlined(int visX, int visY, const char* buffer, sColor4f& c0, sColor4f& c1)
{
	OutText(visX - 1, visY, buffer, &c0, SHELL_ALIGN_CENTER);
	OutText(visX, visY + 1, buffer, &c0, SHELL_ALIGN_CENTER);
	OutText(visX, visY - 1, buffer, &c0, SHELL_ALIGN_CENTER);
	OutText(visX + 1, visY, buffer, &c0, SHELL_ALIGN_CENTER);
	OutText(visX, visY, buffer, &c1, SHELL_ALIGN_CENTER);
}

void CProgressCollected::draw(int bFocus)
{
	if(state & SQSH_VISIBLE)
	{
		float accToCapacity = (capacity > FLT_EPS) ? accumulated / capacity : 0.0f;

		float direction = 0;
		if (produced > 0) {
			direction = 2.0f - requested / produced;
		}
		if (direction < 0) {
			direction = 0;
		}
		direction /= 2.0f;

		float dt = frame_time.delta();
		dt = dt > collectedProgressRelaxationTime ? 0.0f : (1.0f - dt / collectedProgressRelaxationTime);
		showedDirection = direction * (1.0f - dt) + showedDirection * dt;

//		m_cColor.set((1.0f - showedDirection) * 255.0f, showedDirection * 255.0f, 0, 196);

		m_cColor.b = 0;
		if (showedDirection > 0.5f) {
			m_cColor.r = 510.0f * (1.0f - showedDirection);
			m_cColor.g = 255;
		} else {
			m_cColor.r = 255;
			m_cColor.g = 510.0f * showedDirection;
		}

		if (isFlashing()) {
			m_ftime += frame_time.delta();
			if (tickTime == 0 || fmod(m_ftime, tickTime) / tickTime < 0.5f) {
				m_cColor = markColor;
			}
		}

		draw_progress(_shellIconManager.m_hTextureProgressBars, 
			x, y, sx, sy, m_cColor, accToCapacity);
//		draw_progress(_shellIconManager.m_hTextureProgressBars, 
//			x, y, sx, sy, sColor4c((1.0f - accToCapacity) * 255.0f, accToCapacity * 255.0f, 0, 196), accToCapacity);

		terRenderDevice->SetFont(m_hFont);

		XBuffer buffer;
		buffer.SetDigits(1);
		if(accumulated < 1.0f)
			buffer <= accumulated;
		else
			buffer <= round(accumulated);
		buffer < " / " <= round(capacity);

/*
		float accL = terRenderDevice->GetFontLength((char*)acc.c_str());
		float delL = terRenderDevice->GetFontLength(" / ");
		acc += " / " + cap;
		float textL = ( sx + terRenderDevice->GetFontLength((char*)acc.c_str()) - delL ) / 2 - accL;
*/
		float visX = x + sx / 2.0f;
		float visY = y + (sy - m_hFont->GetHeight()) / 2.0f;

		sColor4f c0(0, 0, 0, 1);
		sColor4f c1(1, 1, 1, 1);

		outTextOutlined(visX, visY, buffer, c0, c1);

		terRenderDevice->SetFont(0);
	}
}
void CProgressCollected::Load(const sqshControl* attr) {
	CShellWindow::Load(attr);
	_RELEASE(m_hFont);
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont1, collectedEnergyBarFontSize);
}
void CProgressCollected::OnWindow(int enable)
{
	if(!terShowTips)
		return;

	CInfoWindow* pWnd;
	if(pWnd = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID))
	{
		pWnd->Show(enable);

		if(enable) {
			terPlayer* player = universe()->activePlayer();
			terUnitBase* frame = player->frame();
			if(frame) {
				pWnd->SetText("<Energy_bar>", FormatEnergyText);
			} else {
				pWnd->SetText("");
			}
		}
	}
}

void CProgressShield::draw(int bFocus)
{
	if(state & SQSH_VISIBLE)
	{
		Vect2i a(x, y);
		Vect2i b(x+sx, y+sy);

		terPlayer* player = universe()->activePlayer();
		int nCores = player->countUnits(UNIT_ATTRIBUTE_CORE);
		int nGen = player->countUnits(UNIT_ATTRIBUTE_CORE);
		float f = 0;
		if(nCores > 0 )
			f = float(nGen)/(nCores + nGen);
		else if(nGen > 0)
			f = 1;
		else
			f = 0;

		b.x = a.x + int(f*(b.x - a.x));
		draw_rect(a, b, sColor4c(0, 255, 0, 128));
	}
}
void CProgressShield::OnWindow(int enable)
{
	if(!terShowTips)
		return;

	CInfoWindow* pWnd;
	if(pWnd = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID)){
		pWnd->Show(enable);

		if(enable){
			static char cbText[200];
			pWnd->SetText(_shellIconManager.FormatMessageText("<Shield_bar>", cbText, 0, 
				universe()->activePlayer()->countUnits(UNIT_ATTRIBUTE_CORE), 0, 0)); 
		}
	}
}

void CProgressTerrain::draw(int bFocus)
{
}
void CProgressTerrain::OnWindow(int enable)
{
	if(!terShowTips)
		return;

	CInfoWindow* pWnd;
	if(pWnd = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID))
	{
		pWnd->Show(enable);

		if(enable)
		{
			static char cbText[200];
			pWnd->SetText(_shellIconManager.FormatMessageText("<Terra_bar>", cbText, 0, 0, 0, 0)); 
		}
	}
}

void CProgressMutation::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
		return;

	if (mutationEnergy > 1.f - FLT_EPS) {
		m_cColor.set(0, 255, 0, 255);
	} else {
		m_cColor.set(255, 0, 0, 255);
	}
	if (isFlashing()) {
		m_ftime += frame_time.delta();
		if (tickTime == 0 || fmod(m_ftime, tickTime) / tickTime < 0.5f) {
			m_cColor = markColor;
		}
	}

	draw_progress(_shellIconManager.m_hTextureProgressBars, 
		x, y, sx, sy, m_cColor, mutationEnergy);

	//draw_rect(Vect2i(x, y), Vect2i(x + fEn*sx, y+sy), sColor4c(r, g, 0, 128));
}

void FormatProgressText(char* cb, void* param)
{
	terUnitSquad* pSquad = safe_cast<terUnitSquad*>((terUnitBase*)param);
	int elements = pSquad->squadMutationMolecula().elementCount(DAMAGE_FILTER_BASE);
	_shellIconManager.FormatMessageText("<Mutation_bar>", cb, elements, pSquad->mutationEnergy()*100); 
}
void CProgressMutation::OnWindow(int enable)
{
	if(!terShowTips)
		return;

	CInfoWindow* pWnd;
	if(pWnd = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID))
	{
		terUnitSquad* pSquad = GetSquadByNumber(ID - SQSH_BAR_SQUAD1_ID);
		if(pSquad)
		{
			pWnd->Show(enable);
	
			if(enable)
				pWnd->SetText("<Mutation_bar>", FormatProgressText, pSquad);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CProgressUnitCharge::draw(int bFocus)
{
	if(state & SQSH_VISIBLE)
	{
		int height = int(sy * chargeLevel);
		sColor4c clr;
		if (isSpiral) {
			clr = sColor4c(spiralR, spiralG, spiralB, spiralA);
		} else {
			clr = (chargeLevel == 1) ? sColor4c(0, 255, 0, 90) : sColor4c(255, 255, 0, 90);
		}
		terRenderDevice->DrawRectangle(x, y + sy - height, sx, height, clr);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

CInfoWindow::CInfoWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p)
{
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont1, infoWndFontSize);
	*m_cbText = 0;
	m_pFmtProc = 0;
	m_bCentered = false;
}

CInfoWindow::~CInfoWindow()
{
	_RELEASE(m_hFont);
}

void CInfoWindow::SetText(const char* cb, InfoWndFormatProcType proc, void* param)
{
	m_pFmtProc = proc;
	m_pParam = param;

	SetTime(INT_MAX);

	if(cb)
		strncpy(m_cbText, cb, 255);
	else
		*m_cbText = 0;

	m_bCentered = false;
}
void CInfoWindow::SetTime(int t)
{
	m_nTimeToDisplay = t;
}

void CInfoWindow::draw(int bFocus)
{
	if(state & SQSH_VISIBLE)
	{
		m_nTimeToDisplay -= frame_time.delta();
		if(m_nTimeToDisplay <= 0)
			Show(false);

		if(*m_cbText)
		{
			terRenderDevice->SetFont(m_hFont);

			if(m_pFmtProc)
				m_pFmtProc(m_cbText, m_pParam);

			Vect2f v1, v2;
			OutTextRect(0, 0 , m_cbText, -1, v1, v2);

			int pos_x = m_bCentered ? terScreenSizeX/2 - (v2.x - v1.x) - 1 : terScreenSizeX - (v2.x - v1.x) - 1;
			int delta_y = y + sy - (v2.y-v1.y);

			if(m_hTexture)
				terRenderDevice->DrawSprite(pos_x - 2, delta_y, v2.x - v1.x + 2, v2.y - v1.y,
					m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, m_hTexture, sColor4c(255,255,255,255));

			terRenderDevice->OutText(pos_x, delta_y, m_cbText, sColor4f(1, 1, 1, 1), -1);
			terRenderDevice->SetFont(0);

			Vect2f va(pos_x - 2, delta_y);
			Vect2f vb(va.x + v2.x-v1.x + 1, va.y + v2.y-v1.y);

			draw_rect_empty(va, vb, sColor4f(1, 1, 1, 1));

		}
	}
}


CHintWindow::CHintWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) {
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont1, HINT_FONT_SIZE);
	cutSceneX = 0;
	cutSceneY = 0;
	cutSceneSX = 800;
}

void CHintWindow::Load(const sqshControl* attr) {
	CShellWindow::Load(attr);
	x = absoluteX(hintX);
	y = absoluteY(hintY);
	sx = absoluteX(hintSX);
//	sy = absoluteY(hintSY);
	cutSceneX = absoluteX(cutSceneHintX);
	cutSceneY = absoluteY(cutSceneHintY);
	cutSceneSX = absoluteX(cutSceneHintSX);
}

CHintWindow::~CHintWindow()
{
	_RELEASE(m_hFont);
}

void CHintWindow::SetText(const char* cb, bool incremental)
{

	SetTime(INT_MAX);

	if (incremental) {
		textData += '\n';
		textData += cb;
	} else {
		textData = cb;
	}

}
void CHintWindow::SetTime(int t)
{
	m_nTimeToDisplay = t;
}

void CHintWindow::draw(int bFocus)
{
	drawHint(false);
}

void CHintWindow::drawHint(bool cutScene) {
	if (state & SQSH_VISIBLE) {
		if (m_nTimeToDisplay > 0) {
			m_nTimeToDisplay -= frame_time.delta();
			if (m_nTimeToDisplay <= 0) {
				Show(false);
			}
		}

		if ( !textData.empty() ) {
			float curX = cutScene ? cutSceneX : x;
			float curY = cutScene ? cutSceneY : y;
			float curSX = cutScene ? cutSceneSX : sx;

			terRenderDevice->SetFont(m_hFont);

			char* strToScr = new char[textData.length() + 1];
			strcpy(strToScr, textData.c_str());

			char* strTestEnterBegin = strToScr;
			char* strTestEnter;
			while ( (strTestEnter = strchr(strTestEnterBegin, '\n')) != NULL ) {
				*strTestEnter = 0;
				formatPlainStr(strTestEnterBegin, curSX);
				*strTestEnter = '\n';
				strTestEnterBegin = strTestEnter + 1;
			}
			formatPlainStr(strTestEnterBegin, curSX);

			if (!cutScene) {
				Vect2f v1, v2;
				OutTextRect(0, 0 , strToScr, -1, v1, v2);
				curY -= (v2.y - v1.y);
			}

			terRenderDevice->OutText(curX, curY, strToScr, sColor4f(1, 1, 1, 1), -1);
			terRenderDevice->SetFont(0);
			delete[] strToScr;
		}
	}
}

CChatInfoWindow::CChatInfoWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) {
	m_hFont = terVisGeneric->CreateFont(sqshShellMainFont1, HINT_FONT_SIZE);
	m_nTimeToDisplay = -1;
}

CChatInfoWindow::~CChatInfoWindow()
{
	_RELEASE(m_hFont);
}

void CChatInfoWindow::addString(const string& newString) {
	textData += '\n';
	textData += newString;
}

void CChatInfoWindow::draw(int bFocus) {
	if (state & SQSH_VISIBLE) {
		if (m_nTimeToDisplay > 0) {
			m_nTimeToDisplay -= frame_time.delta();
			if (m_nTimeToDisplay <= 0) {
				m_nTimeToDisplay = 0;
				Show(false);
			}
		}

		if ( !textData.empty() ) {
			float curY = y;

			terRenderDevice->SetFont(m_hFont);

			char* strToScr = new char[textData.length() + 1];
			strcpy(strToScr, textData.c_str());

			char* strTestEnterBegin = strToScr;
			char* strTestEnter;
			while ( (strTestEnter = strchr(strTestEnterBegin, '\n')) != NULL ) {
				*strTestEnter = 0;
				formatPlainStr(strTestEnterBegin, sx);
				*strTestEnter = '\n';
				strTestEnterBegin = strTestEnter + 1;
			}
			formatPlainStr(strTestEnterBegin, sx);

			Vect2f v1, v2;
			OutTextRect(0, 0 , strToScr, -1, v1, v2);
			curY -= (v2.y - v1.y);

			float alpha = 1;
			if (m_nTimeToDisplay >= 0 &&  m_nTimeToDisplay < CHATINFO_FADE_TIME) {
				alpha *= m_nTimeToDisplay / CHATINFO_FADE_TIME;
			}
			int x1, y1, x2, y2; 
			terRenderDevice->GetClipRect(&x1, &y1, &x2, &y2);
			terRenderDevice->SetClipRect(x, y - sy, x + sx, y);
			terRenderDevice->OutText(x, curY, strToScr, sColor4f(1, 1, 1, alpha), -1);
			terRenderDevice->SetClipRect(x1, y1, x2, y2);

			terRenderDevice->SetFont(0);
			delete[] strToScr;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void CBackgroundWindow::draw(int bFocus)
{
	if((state & SQSH_VISIBLE) == 0)
		return;

	DrawSprite(x, y, sx, sy, m_vTexPos[0].x, m_vTexPos[0].y, m_vTexPos[1].x, m_vTexPos[1].y, m_hTexture);
}

int CBackgroundWindow::HitTest(float xT, float yT) {
	if((state & SQSH_VISIBLE) == 0)
		return 0;
	float xF = xT * (float)terRenderDevice->GetSizeX();
	float yF = yT * (float)terRenderDevice->GetSizeY();

	return xF>=x && xF<=(x+sx) && yT>=m_attr->ystart && yF<=(y+sy);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// effects

bool CShellIconManager::handleEvent(const iEvent* ev)
{
	switch(ev->ID){
		case iEVENT_ID_EFFECT:
			Effect(ev->data,ev->controlID);
			return true;
	}
	return false;
}

int CShellIconManager::EffectControls(CShellWindow* pWnd, int effect)
{
	int eff=0;
	if(pWnd->EffectSupported()&effect)
		pWnd->OnEffectStart(effect),eff=1;

	list<CShellWindow*>::iterator i;
	FOR_EACH(pWnd->m_children, i)
		if( EffectControls(*i, effect) )
			eff=1;
	return eff;
}

void CShellIconManager::UnEffectControls(CShellWindow* pWnd, int effect) {
	if ( pWnd->EffectSupported() & effect ) {
		pWnd->OnEffectStart(0);
	}

	list<CShellWindow*>::iterator i;
	FOR_EACH(pWnd->m_children, i) {
		UnEffectControls(*i, effect);
	}
}

void CShellIconManager::Effect(int effect, CShellWindow* pWnd)
{
	if(!pWnd)
		pWnd = m_pDesktop;

	int eff=0;
	if(pWnd)
		eff = EffectControls(pWnd, effect);

	//установить время работы эффекта
	switch(effect)
	{
	case effectButtonsFadeIn:
	case effectButtonsFadeOut:
		m_fEffectTime = _fEffectButtonTotalTime;
		break;

	case effectCtrlRollIn:
	case effectCtrlRollOut:
//		m_fEffectTime = _fEffectButtonTotalTime;
		m_fEffectTime = _fEffectButtonTime1+_fEffectButtonTime2;
		break;
	}
	if( !eff ) m_fEffectTime = 0;
}
void CShellIconManager::Effect(int effect, int id)
{
	CShellWindow* pWnd = GetWnd(id);
	if(pWnd)
		Effect(effect, pWnd);
}


CScaleResultButton::CScaleResultButton(int id, CShellWindow* pParent, EVENTPROC p) : CScaleButton(id, pParent, p) {
	m_hTexture4 = 0;
	victory = false;
}
CScaleResultButton::~CScaleResultButton() {
	RELEASE(m_hTexture4);
}
void CScaleResultButton::Load(const sqshControl* attr) {
	CScaleButton::Load( attr );
	_RELEASE(m_hTexture4);
	if (strlen(attr->image2.texture))	{
		m_hTexture4 = terVisGeneric->CreateTexture( getImageFileName(&(attr->image2)).c_str() );
		m_vTex4Pos[0] = relativeUV(attr->image2._ix, attr->image2._iy, m_hTexture4);
		m_vTex4Pos[1] = Vect2f(
			attr->sx * (float)terRenderDevice->GetSizeX() / (float)m_hTexture->GetWidth(),
			attr->sy * (float)terRenderDevice->GetSizeY() / (float)m_hTexture->GetHeight() );
	}
}
void CScaleResultButton::draw(int bFocus) {

	if((state & SQSH_VISIBLE) == 0)
		return;

	cTexture* texture = victory ? m_hTexture : m_hTexture4;
	Vect2f	  m_texPos[2];
	m_texPos[0] = victory ? m_vTexPos[0] : m_vTex4Pos[0];
	m_texPos[1] = victory ? m_vTexPos[1] : m_vTex4Pos[1];

	m_ftime += frame_time.delta();

	if( (state & SQSH_VISIBLE) ) // draw button
	{
		int iColor=255,Color3=0;
		float phase = 0;
		float _sx=sx, _sy=sy;
		float _size=size;
		if( m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut )
		{
			if( m_effect==effectButtonsFadeIn )
				phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
			else
				phase = _shellIconManager.m_fEffectTime;

			if ( texture ) {
				iColor=round(255*phase/(_fEffectButtonTime1+_fEffectButtonTime2));
				if( iColor>255 ) iColor=255; else if( iColor<0 ) iColor=0;
				if( phase<0 )
					{ phase=1; if (!OnEffectStop(m_effect)) return; }
				else if( phase<=_fEffectButtonTime1 )
				{
					if( !snd1 ) SND2DPlaySound( "menu_button_switth_1"), snd1=1;
					phase/=_fEffectButtonTime1, _sy=sy*0.1f, _sx=sx*phase, _size=0;
				}
				else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 )
				{
					if( !snd2 ) SND2DPlaySound( "menu_button_switth_2"), snd2=1;
					phase/=_fEffectButtonTime2, _sy=sy*phase, _size=size*phase, Color3=round(128*phase);
				}
				else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
					Color3=128*(1-phase/_fEffectButtonTime3),phase=1;
				else
					{ phase=1; if (!OnEffectStop(m_effect)) return; }

			} else {
				iColor=round(255*phase/(_fEffectButtonTime1+_fEffectButtonTime2)) - 90;
				if( iColor>255 ) iColor=255; else if( iColor<0 ) iColor=0;
				if( phase<0 )
					{ phase=1; if (!OnEffectStop(m_effect)) return; }
				else if( phase<=_fEffectButtonTime1 )
				{
					if( !snd1 ) SND2DPlaySound( "menu_button_switth_1"), snd1=1;
					phase/=_fEffectButtonTime1, _sy=sy*0.1f, _sx=sx*phase, _size=0;
				}
				else if( (phase-=_fEffectButtonTime1)<=(_fEffectButtonTime2 + _fEffectButtonTime3) )
				{
					if( !snd2 ) SND2DPlaySound( "menu_button_switth_2"), snd2=1;
					phase/=(_fEffectButtonTime2 + _fEffectButtonTime3), _sy=sy*phase, _size=size*phase, Color3=round(128*phase);
				}
				else
					{ phase=1; if (!OnEffectStop(m_effect)) return; }
			}


		}
		else
			snd1=snd2=0;
		int _x = x+sx*0.5f-_sx*0.5f;
		int _y = y+sy*0.5f-_sy*0.5f;
		if ( texture ) {
			if (m_hTextureBG) {
				float alpha = float(iColor) / 256.0f * (m_attr->txt_dx ? m_attr->txt_dx : scaleButtonAlpha); 
				terRenderDevice->DrawSprite2(
					_x, _y, _sx, _sy, m_texPos[0].x, m_texPos[0].y, m_texPos[1].x, m_texPos[1].y,
					m_vTexBGPos.x, m_vTexBGPos.y, m_vTexBGSize.x, m_vTexBGSize.y,
					texture, m_hTextureBG,
					mapTextureWeight, alpha, fmodf(m_ftime,1000)/1000,
					COLOR_MOD, ALPHA_ADDBLENDALPHA);
			} else {
				DrawSprite( _x, _y, _sx, _sy, 
					m_texPos[0].x, m_texPos[0].y, m_texPos[1].x, m_texPos[1].y, 
					texture, sColor4c(255,255,255,iColor), fmod(m_ftime,1000.f)/1000.f);
			}
		}
		if( texture && m_hTexture3 && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut) )
		{
			DrawSprite( _x, _y, _sx, _size, 
				0, 0, 1, 1, 
				m_hTexture3, sColor4c(255,255,255,Color3), fmod(m_ftime,1000.f)/1000.f, ALPHA_ADDBLENDALPHA );
		}
		if( m_hTexture2 && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut) )
		{
			if (!texture) {
				if (m_hTexture3) {
					float alpha = float(255-iColor) / 256.0f * (m_attr->txt_dx ? m_attr->txt_dx : scaleButtonAlpha); 
					terRenderDevice->DrawSprite2(
						_x, _y, _sx, _sy, 0, 0, 1, 1,
						0, 0, 1, 1,
						m_hTexture2, m_hTexture3,
						scaleButtonWeight, alpha, fmodf(m_ftime,1000)/1000,
						COLOR_MOD, ALPHA_ADDBLENDALPHA);
				} else {
					terRenderDevice->DrawSprite2(_x, _y, _sx, _sy,
						0, 0, 1, 1, 0, 0, 1, 1, m_hTexture2, m_hTexture3, sColor4c(255,255,255,255-iColor), fmod(m_ftime,1000.f)/1000.f, COLOR_MOD, ALPHA_ADDBLENDALPHA);
				}
			} else {
					terRenderDevice->DrawSprite(_x, _y, _sx, _sy, 
						0, 0, 1, 1, m_hTexture2, sColor4c(255,255,255,255-iColor), fmod(m_ftime,1000.f)/1000.f, ALPHA_ADDBLENDALPHA );
			}
		}
	}

	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//

CCreditsWindow::CCreditsWindow(int id, CShellWindow* pParent, EVENTPROC p) : CShellWindow(id, pParent, p) {
	m_hFont = 0;
	timer = -1;
	maxTime = 0;
}

CCreditsWindow::~CCreditsWindow() {
	_RELEASE(m_hFont);
}

void CCreditsWindow::Load(const sqshControl* attr) {
	CShellWindow::Load(attr);

#ifdef _SINGLE_DEMO_
	textData = qdTextDB::instance().getText("Interface.DemoCredits");
#elif _MULTIPLAYER_DEMO_
	textData = qdTextDB::instance().getText("Interface.MultiplayerDemoCredits");
#else
	textData = qdTextDB::instance().getText(attr->text);
#endif

	if (m_hTexture) {
		float tx = absoluteX(m_attr->image.dx);
		float ty = absoluteY(m_attr->image.dy);
		float tsx = absoluteX(m_attr->image.dsx);
		float tsy = absoluteY(m_attr->image.dsy);

		uv.x = (x - tx) / tsx;
		uv.y = (y - ty) / tsy;
//		dudv.x =  (float)m_hTexture->GetWidth() / tsx;
//		dudv.y = (float)m_hTexture->GetHeight() / tsy;
		dudv.x = 1.0f / tsx;
		dudv.y = 1.0f / tsy;
	}

	_RELEASE(m_hFont);
	m_hFont = terVisGeneric->CreateFont(shell_main_menu_font, shell_main_menu_font_size1);
}

void CCreditsWindow::Show(int bShow) {
	CShellWindow::Show(bShow);
	if (bShow) {
		timer = 0;
		terRenderDevice->SetFont(m_hFont);
		Vect2f v1, v2;
		OutTextRect(0, 0, textData.c_str(), -1, v1, v2);
		terRenderDevice->SetFont(0);
		maxTime = (v2.y - v1.y + sy + 10) / absoluteX(CREDITS_SCROLL_SPEED) * 1000.0f;
	} else {
		timer = -1;
	}
}

void CCreditsWindow::draw(int bFocus)
{
	if( !(state & SQSH_VISIBLE) ) return;

	m_ftime += frame_time.delta();

	timer += frame_time.delta();

	if (timer > maxTime) {
		timer = 0;
	}


//	draw_rect_empty(Vect2i(x,y), Vect2i(x+sx,y+sy), sColor4f(1, 1, 1, 1));


	if(!textData.empty())
	{
		float Alpha;

		if( _shellIconManager.IsEffect() && (m_effect==effectButtonsFadeIn || m_effect==effectButtonsFadeOut)) // draw button
		{
			Alpha=0;
			float phase;
			if( m_effect==effectButtonsFadeIn )
				phase = _fEffectButtonTotalTime-_shellIconManager.m_fEffectTime;
			else
				phase = _shellIconManager.m_fEffectTime;
			if( phase<0 )
				{ Alpha=1.0f; m_effect = 0; }
			else if( phase<=_fEffectButtonTime1 );
			else if( (phase-=_fEffectButtonTime1)<=_fEffectButtonTime2 );
			else if( (phase-=_fEffectButtonTime2)<=_fEffectButtonTime3 )
				Alpha=phase/_fEffectButtonTime3;
			else
				{ Alpha=1.0f; m_effect = 0; }
		}
		else 
			{ Alpha=1.0f; m_effect = 0; }

//		Alpha = 0.8f;

		int x1, y1, x2, y2; 
		terRenderDevice->GetClipRect(&x1, &y1, &x2, &y2);
		terRenderDevice->SetClipRect(x, y, x + sx, y + sy);

		terRenderDevice->SetFont(m_hFont);


		float txtX = x;
		float txtY = y + sy - float(absoluteX(CREDITS_SCROLL_SPEED)) * timer / 1000.0f;
		int txtAlign = m_attr->txt_align;
		if( m_attr->txt_align >= 0 ) {
			txtX += sx/2;
			txtAlign = 0;
		}
		if( m_attr->txt_vert_align == 0 ) {
			Vect2f v1, v2;
			OutTextRect(0, 0 ,textData.c_str(), -1, v1, v2);
			txtY += (sy - (v2.y - v1.y)) / 2.0f;
		}

		if (m_hTexture) {
			float alpha = Alpha * (m_attr->txt_dx ? m_attr->txt_dx : scaleButtonAlpha);
			terRenderDevice->OutText(
				txtX,
				txtY,
				textData.c_str(),
				sColor4f(1, 1, 1, alpha),
				txtAlign,
				ALPHA_ADDBLENDALPHA,
				m_hTexture,
				COLOR_MOD,
				uv,
				dudv,
				fmodf(m_ftime,1000)/1000,
				pushButtonTextureWeight);
		} else {
			OutText(txtX, txtY, textData.c_str(), &sColor4f(1, 1, 1, Alpha), txtAlign );
		}
		terRenderDevice->SetFont(0);

		terRenderDevice->SetClipRect(x1, y1, x2, y2);
	}
	if(m_handler)
		m_handler(this, EVENT_DRAWWND, 0);
}

CReplayPlayerPushButton::CReplayPlayerPushButton(int id, CShellWindow* pParent, EVENTPROC p):CShellPushButton(id, pParent, p)
{
	m_hPopupTexture = 0;
}
CReplayPlayerPushButton::~CReplayPlayerPushButton()
{
	_RELEASE(m_hPopupTexture);
}

void CReplayPlayerPushButton::Load(const sqshControl* attr)
{
	CShellPushButton::Load(attr);
	_RELEASE(m_hPopupTexture);
	m_hPopupTexture = terVisGeneric->CreateTexture(sPopupTexture);
}

void CReplayPlayerPushButton::draw(int bFocus)
{
	if( !(state & SQSH_VISIBLE) ) return;

	if (debug_show_intf_borders) {
		draw_rect_empty( Vect2i(x, y), Vect2i(x + sx,y + sy), sColor4f(1, 1, 0, 1) );
	}

	if(!labelText.empty())
	{
		cFont* font;
		switch(m_attr->font_group)
		{
		case 1:
			font = hFontMainmenu1;
			break;
		case 2:
			font = hFontMainmenu2;
			break;
		case 3:
			font = hFontMainmenu3;
			break;
		case 4:
			font = hFontMainmenu4;
			break;
		default:
			font = hFontMainmenu1;
		}

		terRenderDevice->SetFont(font);

		float centerY = y + sy / 2 - font->GetHeight() / 2;

		string toScr = getValidatedText(labelText, sx);

		terRenderDevice->DrawSprite(x - txtdy, y, sx + txtdy * 2, sy, 0, 0, 1, 1, m_hPopupTexture, sColor4c(255,255,255,255));
		if (m_attr->txt_align == SHELL_ALIGN_CENTER) {
			OutText(x + sx / 2.0f, centerY, toScr.c_str(), &color, m_attr->txt_align);
		} else {
			OutText(x, centerY, toScr.c_str(), &color, m_attr->txt_align);
		}

		terRenderDevice->SetFont(0);
	}
	if( m_handler )
		m_handler(this, EVENT_DRAWWND, 0);
}


//////////////////////////////////////////
#include "silicon.h"
#include "HistoryScene.h"
#include "BGScene.h"

static float nLoadProgress = 0;
static float nLoadProgressBlock = 0;
static float nLoadProgressBlockSize = 1;

extern HistoryScene historyScene;
extern HistoryScene bwScene;
extern BGScene bgScene;

void LoadProgressStart()
{
	nLoadProgress = 0;
	nLoadProgressBlock = 0;
	nLoadProgressBlockSize = 1;
}
void LoadProgressBlock(float val)
{
	nLoadProgressBlock = 0;
	nLoadProgressBlockSize = val - nLoadProgress;
	nLoadProgress = val;
}
void LoadProgressUpdateScene(float val, HistoryScene* scene) {
	nLoadProgressBlock = val;
	scene->preDraw();
	bgScene.preDraw();

	terRenderDevice->Fill(0,0,0);
	terRenderDevice->BeginScene();

	scene->draw();

	bgScene.setProgress((nLoadProgress - nLoadProgressBlockSize * (1 - nLoadProgressBlock)));

	bgScene.draw();

	_shellIconManager.draw();

//	draw_progress(_shellIconManager.m_hTextureProgressBars, 
//		_bar_load.points[0].x, _bar_load.points[0].y, _bar_load.points[1].x, _bar_load.points[1].y, 
//		sColor4c(0, 255, 0, 255), (nLoadProgress - nLoadProgressBlockSize * (1 - nLoadProgressBlock)) );

	terRenderDevice->EndScene();
	terRenderDevice->Flush();
}
void LoadProgressUpdate(float val)
{
	if(historyScene.ready()) {
		LoadProgressUpdateScene(val, &historyScene);
	} else if (bwScene.ready()) {
		LoadProgressUpdateScene(val, &bwScene);
	}
}
