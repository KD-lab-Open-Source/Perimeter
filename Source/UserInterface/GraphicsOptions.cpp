#include "StdAfx.h"
#include "GraphicsOptions.h"
#include "GameShellSq.h"
#include "GameShell.h"
#include "Universe.h"


extern cInterfaceRenderDevice* terRenderDevice;
extern cVisGeneric* terVisGeneric;
extern cTileMap* terMapPoint;

extern int terObjectReflection;
extern int terMapReflection;
extern int terBitPerPixel;
extern int terScreenSizeX;
extern int terScreenSizeY;
extern bool terEnableBumpChaos;

STARFORCE_API void SetShadowType(int shadow_map,int shadow_size,bool update);

GraphOptionsManager* GraphOptionsManager::instance = 0;

void CustomGraphOptions::load(const char* sectionName, const char* iniFileName) {
	IniManager iniManager(iniFileName);

	landscapeDetails = iniManager.getInt(sectionName, "MapLevelLOD");
	if (landscapeDetails <= LODS[0]) {
		landscapeDetails = 0;
	} else if (landscapeDetails > LODS[0] && landscapeDetails <= LODS[1]) {
		landscapeDetails = 1;
	} else {
		landscapeDetails = 2;
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

	compressedTextures = iniManager.getFloat(sectionName, "FavoriteLoadDDS");
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

	iniManager.putInt("Graphics", "FavoriteLoadDDS", compressedTextures);
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

	gb_VisGeneric->SetFavoriteLoadDDS(compressedTextures);
}

void GraphOptions::load(const char* sectionName, const char* iniFileName) {
	customOptions.load(sectionName, iniFileName);
	IniManager iniManager(iniFileName);

	resolution = iniManager.getInt("Graphics", "ScreenSizeX");
	for (int i = 0; i < RESOLUTION_COUNT; i++) {
		if (resolution == RESOLUTIONS[i].x) {
			resolution = i;
			break;
		}
	}
	colorDepth = iniManager.getInt("Graphics", "BPP");
}
void GraphOptions::apply() {
	bool change_depth=terBitPerPixel!=colorDepth;
	bool change_size=terRenderDevice->GetSizeX() != RESOLUTIONS[resolution].x || terRenderDevice->GetSizeY() != RESOLUTIONS[resolution].y;
	if (change_size || change_depth)
	{
		terBitPerPixel = colorDepth;
		gameShell->updateResolution(RESOLUTIONS[resolution].x, RESOLUTIONS[resolution].y,change_depth,change_size);
	}
	customOptions.apply();
}

void GraphOptions::save(const char* iniFileName) {
	customOptions.save(iniFileName);
	IniManager iniManager(iniFileName);
	iniManager.putInt("Graphics", "ScreenSizeX", RESOLUTIONS[resolution].x);
	iniManager.putInt("Graphics", "ScreenSizeY", RESOLUTIONS[resolution].y);
	iniManager.putInt("Graphics", "BPP", colorDepth);
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
