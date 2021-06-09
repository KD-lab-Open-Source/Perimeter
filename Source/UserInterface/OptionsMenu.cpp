
#include "StdAfx.h"
#include "Config.h"
#include "Runtime.h"
#include "terra.h"
#include "GenericControls.h"
#include "Universe.h"
#include "UniverseInterface.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "Controls.h"
#include "../Sound/PerimeterSound.h"

#include "HistoryScene.h"
#include "BGScene.h"
#include "../HT/ht.h"
#include "GraphicsOptions.h"

#include "MainMenu.h"
#include "MessageBox.h"

extern BGScene bgScene;
extern HistoryScene historyScene;
extern HistoryScene bwScene;
extern int terShowTips;

extern string getItemTextFromBase(const char *keyStr);

float rejectTimer = 0;

//imgame graphics
void OnIngameGraphicsCustomBtn(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		onLODCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_LOD_COMBO), EVENT_CREATEWND, -1);
		onParticleSlider(_shellIconManager.GetWnd(SQSH_MM_PARTICLE_RATE_SLIDER), EVENT_CREATEWND, -1);
		onOcclusionCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_OCCLUSION_COMBO), EVENT_CREATEWND, -1);
		onPointLightCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_POINT_LIGHT_COMBO), EVENT_CREATEWND, -1);
		onSamplesCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_SHADOWS_SAMPLES_COMBO), EVENT_CREATEWND, -1);
		onBumpCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_BUMP_COMBO), EVENT_CREATEWND, -1);
//		onBumpChaosCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_BUMP_CHAOS_COMBO), EVENT_CREATEWND, -1);
		onCompressCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_COMPRESS_COMBO), EVENT_CREATEWND, -1);
		OnComboGraphicsFurrows(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_FURROWS_COMBO), EVENT_CREATEWND, -1);
		OnComboGraphicsShadows(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_SHADOWS_COMBO), EVENT_CREATEWND, -1);
		OnComboGraphicsReflection(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_REFLECTION_COMBO), EVENT_CREATEWND, -1);

		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_INGAME_CUSTOM_SCR);
	}
}

// graphics
void OnSliderGraphicsGamma(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CSliderWindow *pSlider = (CSliderWindow*) pWnd;
	if( code == EVENT_SLIDERUPDATE )
	{
		terGraphicsGamma = 0.5f+ pSlider->pos*2.f;
		terRenderDevice->SetGamma( terGraphicsGamma );
	}
	else if( code == EVENT_CREATEWND )
		pSlider->pos = (terGraphicsGamma-0.5f)/2.f;
}

void OnComboGraphicsSettings(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( getItemTextFromBase("Fast").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("Performance").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("Max").c_str() );
//			pCombo->Array.push_back( getItemTextFromBase("Extreme").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("Custom").c_str() );
			pCombo->size = pCombo->Array.size();
		}
		pCombo->pos = GraphOptionsManager::getInstance().getType();
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().setType((GraphOptionsManager::SettingsType)pCombo->pos);
	}
}

void OnComboGraphicsResolution(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( "800x600" );
			pCombo->Array.push_back( "1024x768" );
			#ifndef _DEMO_
				pCombo->Array.push_back( "1280x960" );
				pCombo->Array.push_back( "1600x1200" );
			#endif

			pCombo->size = pCombo->Array.size();
		}
		pCombo->pos = GraphOptionsManager::getInstance().getGraphicsOptions().resolution;
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().resolution = pCombo->pos;
	}
}

void OnComboGraphicsColorDepth(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( "32" );
			pCombo->Array.push_back( "16" );
			pCombo->size = 2;
		}
		pCombo->pos = GraphOptionsManager::getInstance().getGraphicsOptions().colorDepth == 32 ? 0 : 1;
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().colorDepth = pCombo->pos == 0 ? 32 : 16;
	}
}

void OnGraphicsCustomBtn(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		onLODCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_LOD_COMBO), EVENT_CREATEWND, -1);
		onParticleSlider(_shellIconManager.GetWnd(SQSH_MM_PARTICLE_RATE_SLIDER), EVENT_CREATEWND, -1);
		onOcclusionCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_OCCLUSION_COMBO), EVENT_CREATEWND, -1);
		onPointLightCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_POINT_LIGHT_COMBO), EVENT_CREATEWND, -1);
		onSamplesCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_SHADOWS_SAMPLES_COMBO), EVENT_CREATEWND, -1);
		onBumpCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_BUMP_COMBO), EVENT_CREATEWND, -1);
		onBumpChaosCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_BUMP_CHAOS_COMBO), EVENT_CREATEWND, -1);
		onCompressCombo(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_COMPRESS_COMBO), EVENT_CREATEWND, -1);
		OnComboGraphicsFurrows(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_FURROWS_COMBO), EVENT_CREATEWND, -1);
		OnComboGraphicsShadows(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_SHADOWS_COMBO), EVENT_CREATEWND, -1);
		OnComboGraphicsReflection(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_REFLECTION_COMBO), EVENT_CREATEWND, -1);

		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_CUSTOM_SCR);
	}
}

int submitResolutionAction(float, float) {
	rejectTimer = 0;
	GraphOptionsManager::getInstance().approveChanges();
	GraphOptionsManager::getInstance().save();
	return 0;
}

int rejectResolutionAction(float, float) {
	if (rejectTimer <= 0) {
		if (GraphOptionsManager::getInstance().hasChanges()) {
			GraphOptionsManager::getInstance().reject();
			OnComboGraphicsSettings(_shellIconManager.GetWnd(SQSH_MM_SETTINGS_COMBO), EVENT_CREATEWND, -1);
			OnComboGraphicsResolution(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_RESOLUTION_COMBO), EVENT_CREATEWND, -1);
			OnComboGraphicsColorDepth(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_COLORDEPTH_COMBO), EVENT_CREATEWND, -1);
			hideMessageBox();
		}
		return 0;
	} else {
		rejectTimer -= frame_time.delta();
	}
	return 1;
}

void OnGraphicsApplyBtn(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_DRAWWND) {
		pWnd->Enable(GraphOptionsManager::getInstance().hasChanges());
	} else if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		GraphOptionsManager::getInstance().apply();
		if (GraphOptionsManager::getInstance().hasConfirmChanges()) {
			setupOkMessageBox(
				submitResolutionAction,
				0,
				qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.KeepChanges"),
				MBOX_OK );

			rejectTimer = 10000;
			_shellIconManager.AddDynamicHandler(rejectResolutionAction, CBCODE_QUANT);

			showMessageBox();
		} else {
			GraphOptionsManager::getInstance().approveChanges();
			GraphOptionsManager::getInstance().save();
		}
	}
}


//custom
void onLODCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( getItemTextFromBase("Low").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("Medium").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("High").c_str() );
			pCombo->size = 3;
		}
		pCombo->pos = GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.landscapeDetails;
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.landscapeDetails = pCombo->pos;
	}
}
void onParticleSlider(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CSliderWindow *pSlider = (CSliderWindow*) pWnd;
	if ( code == EVENT_SLIDERUPDATE ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.particleRate = pSlider->pos;
	} else if( code == EVENT_CREATEWND ) {
		pSlider->pos = GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.particleRate;
	}
}
void onOcclusionCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( getItemTextFromBase("Off").c_str() );
			if (terVisGeneric->PossibilityOcclusion()) {
				pCombo->Array.push_back( getItemTextFromBase("On").c_str() );
			}
			pCombo->size = pCombo->Array.size();
		}
		if (!terVisGeneric->PossibilityOcclusion()) {
			pCombo->Enable(false);
		}
		pCombo->pos = GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.enableOcclusion;
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.enableOcclusion = pCombo->pos;
	}
}
void onPointLightCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( getItemTextFromBase("Off").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("On").c_str() );
			pCombo->size = 2;
		}
		pCombo->pos = GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.enablePointLight;
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.enablePointLight = pCombo->pos;
	}
}
void onSamplesCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			if (terVisGeneric->PossibilityShadowMapSelf4x4()) {
				pCombo->Array.push_back( "4" );
				pCombo->Array.push_back( "16" );
				pCombo->size = 2;
			} else {
				pCombo->Array.push_back( "-" );
				pCombo->size = 1;
			}
		}
		if (!terVisGeneric->PossibilityShadowMapSelf4x4()) {
			pCombo->Enable(false);
		}
		if (GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.shadowSamples == 16) {
			pCombo->pos = 1;
		} else {
			pCombo->pos = 0;
		}
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.shadowSamples = pCombo->pos ? 16 : 0;
	} else if ( code == EVENT_DRAWWND ) {
		pCombo->Enable(terVisGeneric->PossibilityShadowMapSelf4x4() && GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.shadowType == 2);
	}
}
void onBumpCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( getItemTextFromBase("Off").c_str() );
			if (terVisGeneric->PossibilityBump()) {
				pCombo->Array.push_back( getItemTextFromBase("On").c_str() );
			}
			pCombo->size = pCombo->Array.size();
		}
		if (!terVisGeneric->PossibilityBump()) {
			pCombo->Enable(false);
		}
		pCombo->pos = GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.bumpMapping;
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.bumpMapping = pCombo->pos;
	}
}
void onBumpChaosCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( getItemTextFromBase("Off").c_str() );
			if (terVisGeneric->PossibilityBumpChaos()) {
				pCombo->Array.push_back( getItemTextFromBase("On").c_str() );
			}
			pCombo->size = pCombo->Array.size();
		}
		if (!terVisGeneric->PossibilityBumpChaos()) {
			pCombo->Enable(false);
		}
		pCombo->pos = GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.bumpChaos;
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.bumpChaos = pCombo->pos;
	}
}
void onCompressCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( getItemTextFromBase("Off").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("On").c_str() );
			pCombo->size = 2;
		}
		pCombo->pos = GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.compressedTextures;
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.compressedTextures = pCombo->pos;
	}
}

void OnComboGraphicsFurrows(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( getItemTextFromBase("Shadow map").c_str() );
			if (terRenderDevice->IsEnableSelfShadow()) {
				pCombo->Array.push_back( getItemTextFromBase("Self shadow map").c_str() );
			}
			pCombo->size = pCombo->Array.size();
		}
		if (!terRenderDevice->IsEnableSelfShadow()) {
			pCombo->Enable(false);
		}
		pCombo->pos = GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.shadowType - 1;
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.shadowType = pCombo->pos + 1;
	}
}
void OnComboGraphicsShadows(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( getItemTextFromBase("Off").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("Low").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("Medium").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("High").c_str() );
			pCombo->size = 4;
		}
		int qual = GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.shadowQuality; 
		qual = qual ? (qual - 1) : qual;
		pCombo->pos = qual;
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		int qual = pCombo->pos ? (pCombo->pos + 1) : pCombo->pos;
		GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.shadowQuality = qual;
	}
}
void OnComboGraphicsReflection(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if ( code == EVENT_CREATEWND ) {
		if (param != -1) {
			pCombo->Array.push_back( getItemTextFromBase("Off").c_str() );
			pCombo->Array.push_back( getItemTextFromBase("On").c_str() );
			pCombo->size = 2;
		}
		CustomGraphOptions& customOptions = GraphOptionsManager::getInstance().getGraphicsOptions().customOptions;
		if (customOptions.mapReflections == 0 && customOptions.objReflections == 0) {
			pCombo->pos = 0;
		} else {
			pCombo->pos = 1;
		}
	} else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED ) {
		if ( pCombo->pos==0 ) {
			GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.mapReflections = 0;
			GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.objReflections = 0;
		} else if ( pCombo->pos==1 ) {
			GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.mapReflections = 1;
			GraphOptionsManager::getInstance().getGraphicsOptions().customOptions.objReflections = 1;
		}
	}
}

void OnComboGraphicsMode(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if( code == EVENT_CREATEWND )
	{
		pCombo->Array.push_back( "Full Screen" );
		pCombo->Array.push_back( "Windowed" );
		pCombo->size = 2;
	}
}

// sound
void OnComboSoundMusic(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if( code == EVENT_CREATEWND )
	{
		pCombo->Array.push_back( getItemTextFromBase("Off").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("On").c_str() );
		pCombo->size = 2;
		pCombo->pos = terMusicEnable;
	}
	else if( code==EVENT_UNPRESSED || code == EVENT_RUNPRESSED )
	{
		InitSound(terSoundEnable, pCombo->pos, false);
		MusicEnable( pCombo->pos );
	}
}
void OnComboSoundEffects(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if( code == EVENT_CREATEWND )
	{
		pCombo->Array.push_back( getItemTextFromBase("Off").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("On").c_str() );
		pCombo->size = 2;
		pCombo->pos = terSoundEnable;
	}
	else if ( code == EVENT_UNPRESSED || code == EVENT_RUNPRESSED )	{
		InitSound(pCombo->pos, terMusicEnable, false);
		historyScene.setupAudio();
		_shellIconManager.setupAudio();
	}
}
void OnSliderSoundVolume(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CSliderWindow *pSlider = (CSliderWindow*) pWnd;
	if( code == EVENT_SLIDERUPDATE ) {
		SNDSetVolume( terSoundVolume = pSlider->pos );
		historyScene.setupAudio();
		_shellIconManager.setupAudio();
	} else if ( code == EVENT_CREATEWND ) {
		pSlider->sounded = true;
		SNDSetVolume( pSlider->pos = terSoundVolume );
		historyScene.setupAudio();
		_shellIconManager.setupAudio();
	}
}
void OnSliderMusicVolume(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CSliderWindow *pSlider = (CSliderWindow*) pWnd;
	if( code == EVENT_SLIDERUPDATE )
		SetVolumeMusic( terMusicVolume = pSlider->pos );
	else if( code == EVENT_CREATEWND )
		SetVolumeMusic( pSlider->pos = terMusicVolume );
}
// game
void OnSliderAngleSens(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CSliderWindow *pSlider = (CSliderWindow*) pWnd;
	if( code == EVENT_SLIDERUPDATE ) {
		CAMERA_MOUSE_ANGLE_SPEED_DELTA = 0.01f + pSlider->pos * 6.f;
		CAMERA_KBD_ANGLE_SPEED_DELTA = CAMERA_MOUSE_ANGLE_SPEED_DELTA / 30.0f;
	} else if ( code == EVENT_CREATEWND ) {
		pSlider->pos = (CAMERA_MOUSE_ANGLE_SPEED_DELTA - 0.01f) / 6.f;
	}
}
void OnSliderScrollRate(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CSliderWindow *pSlider = (CSliderWindow*) pWnd;
	if( code == EVENT_SLIDERUPDATE )
		CAMERA_SCROLL_SPEED_DELTA = 
		CAMERA_BORDER_SCROLL_SPEED_DELTA = 1+pSlider->pos*19.f;
	else if( code == EVENT_CREATEWND )
		pSlider->pos = (CAMERA_SCROLL_SPEED_DELTA-1)/19.f;
}
void OnComboGameTooltips(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	CComboWindow *pCombo = (CComboWindow*) pWnd;
	if( code == EVENT_CREATEWND )
	{
		pCombo->pos = terShowTips;
		pCombo->Array.push_back( getItemTextFromBase("Off").c_str() );
		pCombo->Array.push_back( getItemTextFromBase("On").c_str() );
		pCombo->size = 2;
	}
	else if( code == EVENT_UNPRESSED )
		terShowTips = pCombo->pos;
}

//main menu options-----------------------
void onMMGameButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_GAME_SCR);
	}
}
void onMMGraphicsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		OnComboGraphicsSettings(_shellIconManager.GetWnd(SQSH_MM_SETTINGS_COMBO), EVENT_CREATEWND, -1);
		OnComboGraphicsResolution(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_RESOLUTION_COMBO), EVENT_CREATEWND, -1);
		OnComboGraphicsColorDepth(_shellIconManager.GetWnd(SQSH_MM_GRAPHICS_COLORDEPTH_COMBO), EVENT_CREATEWND, -1);
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_GRAPHICS_SCR);
	}
}
void onMMSoundButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SOUND_SCR);
	}
}
void OnButtonMMOptions(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_UNPRESSED)
	{
		//if(pWnd->m_pParent->ID==SQSH_MM_SCREEN_INMISSION)
		//	_shellIconManager.LoadControlsGroup(SHELL_LOAD_GROUP_MENU);
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SCREEN_OPTIONS);
	}
}

void OnButtonOptionGame(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if (code == EVENT_UNPRESSED) {
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SCREEN_GAME);
	}
}
void OnButtonOptionGraphics(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_UNPRESSED)
		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SCREEN_GRAPHICS);
}
void OnButtonOptionSound(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_UNPRESSED) {
		CComboWindow *pCombo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_SOUND_SOUNDEFFECTS_COMBO);
		pCombo->pos = terSoundEnable;
		pCombo = (CComboWindow*) _shellIconManager.GetWnd(SQSH_MM_SOUND_MUSIC_COMBO);
		pCombo->pos = terMusicEnable;

		_shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_SCREEN_SOUND);
	}
}
