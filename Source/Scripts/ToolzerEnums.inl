
// Типы воздействия тулзера
enum ToolzerActionID
{
	TOOLZER_NONE,	// никакого воздействия
	TOOLZER_4ZP,	// toolzerAligmentTerrain4ZP
	TOOLZER_VH,	// toolzerAligmentTerrainVariableH
	TOOLZER_H,	// toolzerChangeTerHeight
	TOOLZER_NZP,	// toolzerChangeTerHeightIfNotZP
	TOOLZER_VLD,	// toolzerPlottingVeryLightDam
	TOOLZER_S,	// toolzerPlotingSoot
	TOOLZER_EZ	// toolzerEraseZP
};

// Режимы нанесения повреждений зданиям.
// Задается в ToolzerStepData
enum ToolzerBuildingDamageMode
{
	BUILDING_DAMAGE_CIRCLE,		// повреждения в круге радиусом buildingDamageRadius
	BUILDING_DAMAGE_SQUARE		// повреждения в квадрате buildingDamageRadius на buildingDamageRadius
};

// Фазы тулзера для ToolzerController
enum ToolzerPhaseID
{
	TOOLZER_PHASE_NONE,
				
	TOOLZER_PHASE_DEFAULT,		// универсальное имя

	// для подземных юнитов
	TOOLZER_PHASE_START_MOVE,	// включается в момент начала движения
	TOOLZER_PHASE_MOVE,		// включается во время движения
	TOOLZER_PHASE_END_MOVE		// включается в момент остановки
};

