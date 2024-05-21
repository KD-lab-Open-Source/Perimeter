#include "StdAfx.h"
#include "GraphicsOptions.h"
#include "GameShellSq.h"
#include "GameShell.h"
#include "Universe.h"
#include "SourceUIResolution.h"
#include "PerimeterShellUI.h"
#include <set>
#include <unordered_set>


extern cInterfaceRenderDevice* terRenderDevice;
extern cVisGeneric* terVisGeneric;
extern cTileMap* terMapPoint;

extern int terObjectReflection;
extern int terMapReflection;
extern int terBitPerPixel;
extern int terScreenSizeX;
extern int terScreenSizeY;
extern bool terEnableBumpChaos;

void SetShadowType(int shadow_map,int shadow_size,bool update);

GraphOptionsManager* GraphOptionsManager::instance = 0;

void CustomGraphOptions::load(const char* sectionName, const char* iniFileName) {
	IniManager iniManager(iniFileName);

	int maxLevelLOD = iniManager.getInt(sectionName, "MapLevelLOD");
    landscapeDetails = LOD_COUNT - 1;
    for (int i = 0; i < LOD_COUNT; ++i) {
        if (maxLevelLOD <= LODS[i]) {
            landscapeDetails = i;
            break;
        }
    }

	mapReflections = iniManager.getInt(sectionName, "MapReflection");
	objReflections = iniManager.getInt(sectionName,"ObjectReflection");

	enableOcclusion = terVisGeneric->PossibilityOcclusion() ? iniManager.getInt(sectionName, "EnableOcclusion") : 0;
	enablePointLight = iniManager.getInt(sectionName, "EnablePointLight");

	shadowQuality = iniManager.getInt(sectionName, "DrawMeshShadow");

	shadowType = terRenderDevice->IsEnableSelfShadow() ? iniManager.getInt(sectionName, "ShadowType") : 1;
	shadowSamples = terVisGeneric->PossibilityShadowMapSelf4x4() ? iniManager.getInt(sectionName, "ShadowSamples") : 4;

	bumpMapping = terVisGeneric->PossibilityBump() ? iniManager.getInt(sectionName, "EnableBump") : 0;
	bumpChaos = terVisGeneric->PossibilityBumpChaos() ? iniManager.getInt(sectionName, "EnableBumpChaos") : 0;
	particleRate = iniManager.getFloat(sectionName, "ParticleRate");
}

void CustomGraphOptions::save(const char* iniFileName) {
	IniManager iniManager(iniFileName);

	iniManager.putInt("Graphics", "MapLevelLOD", LODS[landscapeDetails]);

	iniManager.putInt("Graphics", "MapReflection", mapReflections);
	iniManager.putInt("Graphics","ObjectReflection", objReflections);

	iniManager.putInt("Graphics", "EnableOcclusion", enableOcclusion);
	iniManager.putInt("Graphics", "EnablePointLight", enablePointLight);

	iniManager.putInt("Graphics", "DrawMeshShadow", shadowQuality);

	iniManager.putInt("Graphics", "ShadowType", shadowType);
	iniManager.putInt("Graphics", "ShadowSamples", shadowSamples);

	iniManager.putInt("Graphics", "EnableBump", bumpMapping);
	iniManager.putInt("Graphics", "EnableBumpChaos", bumpChaos);
	iniManager.putFloat("Graphics", "ParticleRate", particleRate);
}

void CustomGraphOptions::apply() {
	terVisGeneric->SetMapLevel(LODS[landscapeDetails]);

	terObjectReflection = objReflections;
	terMapReflection = mapReflections;
	if (terMapPoint) {
		if (mapReflections) {
			terMapPoint->SetAttr(ATTRUNKOBJ_REFLECTION);
		} else {
			terMapPoint->ClearAttr(ATTRUNKOBJ_REFLECTION);
		}

//		gameShell->updateMap();
//		if (universe() && gameShell->GameActive) {
//			universe()->UpdateSkinColor();
//		}
	}

	terVisGeneric->EnableOcclusion(enableOcclusion);
	terVisGeneric->EnablePointLight(enablePointLight);

	SetShadowType(shadowType, shadowQuality, false);
	terVisGeneric->SetShadowMapSelf4x4(shadowSamples == 16);

	terVisGeneric->SetEnableBump(bumpMapping);

	terEnableBumpChaos = bumpChaos;
	if (gameShell) {
		gameShell->recreateChaos();
	}

	terVisGeneric->SetGlobalParticleRate(particleRate);
}

void GraphOptions::load(const char* sectionName, const char* iniFileName) {
	customOptions.load(sectionName, iniFileName);

    IniManager iniManager(iniFileName, false);
    colorDepth = iniManager.getInt("Graphics", "BPP");
    uiAnchor = iniManager.getInt("Graphics", "UIAnchor");
    if (uiAnchor < 0 || uiAnchor >= SHELL_ANCHOR_DEFAULT) {
        uiAnchor = SHELL_ANCHOR_CENTER;
    }
    int grabInputVal = 1;
    iniManager.getInt("Graphics", "GrabInput", grabInputVal);
    grabInput = grabInputVal != 0;
    int fogEnableVal = 1;
    iniManager.getInt("Graphics", "FogEnable", fogEnableVal);
    fogEnable = fogEnableVal != 0;
    int vsyncEnableVal = 1;
    iniManager.getInt("Graphics", "VSync", vsyncEnableVal);
    vsyncEnable = vsyncEnableVal != 0;
    
    std::set<DisplayMode> resSet;
	resolutions.clear();

    //Add current resolution
    if (terFullScreen) {
        resolution.set(true, terScreenIndex, terScreenSizeX, terScreenSizeY, terScreenRefresh);
    } else {
        resolution.set(false, -1, terScreenSizeX, terScreenSizeY, 0);
    }
    resSet.insert(resolution);

    //Get display modes for screens
    Vect2i smallest(0, 0);
    SDL_DisplayMode current;
    for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i){
        int result = SDL_GetCurrentDisplayMode(i, &current);
        if (result != 0) {
            fprintf(stderr, "Could not get display mode for display %d: %s", i, SDL_GetError());
            continue;
        }

        //printf("Display %d: %dx%d %dhz\n", i, current.w, current.h, current.refresh_rate);
        resSet.emplace(true, i, current.w, current.h, current.refresh_rate);
        resSet.emplace(false, -1, current.w, current.h, 0);
        if (smallest.x == 0 || current.w < smallest.x) {
            smallest.x = current.w;
        }
        if (smallest.y == 0 || current.h < smallest.y) {
            smallest.y = current.h;
        }
    }
    
    //Add source resolutions, only if they are smaller than smallest dimensions (user can still resize manually)
    for (const UIResolution& res : getSourceUIResolutions()) {
        resSet.emplace(false, -1, res.x, res.y, 0);
    }
    
    //Dump set into vector and order it, don't add window modes that are smaller
    for (DisplayMode res : resSet) {
        if (!res.fullscreen) {
            if (smallest.x != 0 && smallest.x < res.x) continue;
            if (smallest.y != 0 && smallest.y < res.y) continue;
        }
        resolutions.emplace_back(res);
    }
    
    std::sort(resolutions.begin(), resolutions.end());

#if defined(PERIMETER_DEBUG)
    printf("Current mode %s\n", resolution.text().c_str());
    for (DisplayMode& res : resolutions) {
        printf("- %s\n", res.text().c_str());
    }
#endif
}

void GraphOptions::apply() {
#ifndef GPX
	bool change_depth = terBitPerPixel != colorDepth;
    bool change_display_mode = (terFullScreen != 0) != resolution.fullscreen
            || terVSyncEnable != vsyncEnable;
    if (resolution.fullscreen) {
        change_display_mode |= terScreenRefresh != resolution.refresh
                            || terScreenIndex != resolution.display;
    } else {
        //If window then check if user moved it to another display first
        int windowScreenIndex = SDL_GetWindowDisplayIndex(sdlWindow);
        if (0 <= windowScreenIndex && terScreenIndex != windowScreenIndex) {
            terScreenIndex = windowScreenIndex;
        }
    }

	bool change_size = terScreenSizeX != resolution.x || terScreenSizeY != resolution.y;
	if (change_display_mode || change_size || change_depth) {
		terBitPerPixel = colorDepth;
        terVSyncEnable = vsyncEnable;
        terFullScreen = resolution.fullscreen;
        if (terFullScreen) {
            terScreenIndex = resolution.display;
            terScreenRefresh = resolution.refresh;
        }
        terScreenSizeX = resolution.x;
        terScreenSizeY = resolution.y;
        change_display_mode |= change_size;
        if (gameShell) {
            gameShell->updateResolution(change_depth, change_size, change_display_mode);
        }
	}

    if ((terGrabInput != 0) != grabInput) {
        terGrabInput = grabInput;
        check_command_line_parameter("GrabInput", terGrabInput);
        if (terGrabInput) {
            SDL_SetWindowGrab(sdlWindow, SDL_TRUE);
        } else {
            SDL_SetWindowGrab(sdlWindow, SDL_FALSE);
        }
    }
#endif
    shell_anchor = static_cast<SHELL_ANCHOR>(uiAnchor);
    _shellIconManager.reloadDesktop();
    
	customOptions.apply();
}

void GraphOptions::save(const char* iniFileName) {
	customOptions.save(iniFileName);
	IniManager iniManager(iniFileName);
    iniManager.putInt("Graphics", "Fullscreen", terFullScreen);
    int screenIndex = 0;
    if (0 <= terScreenIndex) {
        screenIndex = terScreenIndex;
    }
    iniManager.putInt("Graphics", "ScreenIndex", screenIndex);
    iniManager.putInt("Graphics", "ScreenSizeX", resolution.x);
    iniManager.putInt("Graphics", "ScreenSizeY", resolution.y);
    iniManager.putInt("Graphics", "ScreenRefresh", terScreenRefresh);
	iniManager.putInt("Graphics", "BPP", colorDepth);
    iniManager.putInt("Graphics", "UIAnchor", uiAnchor);
    iniManager.putInt("Graphics", "VSync", vsyncEnable ? 1 : 0);
    iniManager.putInt("Graphics", "GrabInput", grabInput ? 1 : 0);
    iniManager.putInt("Graphics", "FogEnable", fogEnable ? 1 : 0);
}

void GraphOptionsManager::load() {
	preSettings[FAST].load("FastGraphics", "Perimeter.ini");
	preSettings[PERFORMANCE].load("PerformanceGraphics", "Perimeter.ini");
	preSettings[MAX].load("MaxGraphics", "Perimeter.ini");
//	preSettings[EXTREME].load("ExtremeGraphics", "Perimeter.ini");
	currentOptions.load("Graphics", "Perimeter.ini");
	newOptions = currentOptions;
}

void GraphOptionsManager::save() {
	newOptions = currentOptions;
	currentOptions.save("Perimeter.ini");
}

bool GraphOptionsManager::hasChanges() {
	return newOptions != currentOptions;
}

bool GraphOptionsManager::hasConfirmChanges() {
	return newOptions.resolution != currentOptions.resolution;
}

void GraphOptionsManager::apply() {
	newOptions.apply();
}

void GraphOptionsManager::reject() {
	currentOptions.apply();
	newOptions = currentOptions;
}

void GraphOptionsManager::approveChanges() {
	currentOptions = newOptions;
}

void GraphOptionsManager::reset() {
	newOptions = currentOptions;
}

GraphOptionsManager::SettingsType GraphOptionsManager::getType() {
	for (int i = FAST; i <= MAX; i++) {
		if (newOptions.customOptions == preSettings[i]) {
			return (GraphOptionsManager::SettingsType)i;
		}
	}
	return CUSTOM;
}

void GraphOptionsManager::setType(SettingsType newType) {
	if (newType != CUSTOM) {
		newOptions.customOptions = preSettings[newType];
	}
}
