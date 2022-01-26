#include "StdAfx.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"
#include "Save.h"

BEGIN_ENUM_DESCRIPTOR(TeleportationType, "TeleportationType")
REGISTER_ENUM(TELEPORTATION_TYPE_ALPHA, "Телепортация с помощью Альфы")
REGISTER_ENUM(TELEPORTATION_TYPE_OMEGA, "Телепортация с помощью Омегиравно")
END_ENUM_DESCRIPTOR(TeleportationType)

BEGIN_ENUM_DESCRIPTOR(PlayerState, "PlayerState")
REGISTER_ENUM(PLAYER_STATE_UNABLE_TO_PLACE_BUILDING, "Не могу установить здание")
REGISTER_ENUM(PLAYER_STATE_UNABLE_TO_PLACE_CORE, "Не могу установить ядро")
END_ENUM_DESCRIPTOR(PlayerState)

/////////////////////////////////////////////////////
//		Conditions
/////////////////////////////////////////////////////

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(ConditionNode, Type, "Type")
REGISTER_ENUM_ENCLOSED(ConditionNode, NORMAL, "да")
REGISTER_ENUM_ENCLOSED(ConditionNode, INVERTED, "НЕ")
END_ENUM_DESCRIPTOR_ENCLOSED(ConditionNode, Type)

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(ConditionSwitcher, Type, "Type")
REGISTER_ENUM_ENCLOSED(ConditionSwitcher, AND, "И")
REGISTER_ENUM_ENCLOSED(ConditionSwitcher, OR, "ИЛИ")
END_ENUM_DESCRIPTOR_ENCLOSED(ConditionSwitcher, Type)

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(ConditionCheckBelligerent, Belligerent, "Belligerent")
REGISTER_ENUM_ENCLOSED(ConditionCheckBelligerent, EXODUS, "Исходники")
REGISTER_ENUM_ENCLOSED(ConditionCheckBelligerent, HARKBACKHOOD, "Возвратники")
REGISTER_ENUM_ENCLOSED(ConditionCheckBelligerent, EMPIRE, "Империя")
END_ENUM_DESCRIPTOR_ENCLOSED(ConditionCheckBelligerent, Belligerent)

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(Trigger, State, "State")
REGISTER_ENUM_ENCLOSED(Trigger, SLEEPING, "Проверяет входные связи")
REGISTER_ENUM_ENCLOSED(Trigger, CHECKING, "Проверяет условия")
REGISTER_ENUM_ENCLOSED(Trigger, WORKING, "Выполняется")
REGISTER_ENUM_ENCLOSED(Trigger, DONE, "Выполнен")
END_ENUM_DESCRIPTOR_ENCLOSED(Trigger, State)

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(TriggerLink, Type, "Type")
REGISTER_ENUM_ENCLOSED(TriggerLink, THIN, "Тонкая")
REGISTER_ENUM_ENCLOSED(TriggerLink, THICK, "Толстая")
END_ENUM_DESCRIPTOR_ENCLOSED(TriggerLink, Type)

REGISTER_CLASS(Condition, Condition, "Не выполняется никогда (для выключения триггеров)")
REGISTER_CLASS(Condition, ConditionSwitcher, "И/ИЛИ")
REGISTER_CLASS(Condition, ConditionOneTime, "--------------------")
REGISTER_CLASS(Condition, ConditionIsPlayerAI, "АИ ли Игрок")
REGISTER_CLASS(Condition, ConditionCheckBelligerent, "Проверка воюющей стороны")

REGISTER_CLASS(Condition, ConditionCreateObject, "Объект создан")
REGISTER_CLASS(Condition, ConditionKillObject, "Объект уничтожен")
REGISTER_CLASS(Condition, ConditionObjectExists, "Объект существует")
REGISTER_CLASS(Condition, ConditionCaptureBuilding, "Захват здания")
REGISTER_CLASS(Condition, ConditionTeleportation, "Произошла телепортация")

REGISTER_CLASS(Condition, ConditionEnegyLevelLowerReserve, "Уровень энергии ниже предела")
REGISTER_CLASS(Condition, ConditionEnegyLevelUpperReserve, "Уровень энергии выше предела")
REGISTER_CLASS(Condition, ConditionEnegyLevelBelowMaximum, "Уровень энергии ниже максимума")
REGISTER_CLASS(Condition, ConditionOutOfEnergyCapacity, "Уровень энергии больше процента емкости")

REGISTER_CLASS(Condition, ConditionNumberOfBuildingByCoresCapacity, "Зданий_1*коэффициент < Зданий_2")
REGISTER_CLASS(Condition, ConditionUnitClassUnderAttack, "Объект атакуют")
REGISTER_CLASS(Condition, ConditionUnitClassIsGoingToBeAttacked, "Объект собираются атаковать")

REGISTER_CLASS(Condition, ConditionSquadGoingToAttack, "Сквад собирается атаковать")

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(ConditionFrameState, State, "State")
REGISTER_ENUM_ENCLOSED(ConditionFrameState, AI_FRAME_STATE_EXIST, "Существует")
REGISTER_ENUM_ENCLOSED(ConditionFrameState, AI_FRAME_STATE_INSTALLED, "Инсталлирован")
REGISTER_ENUM_ENCLOSED(ConditionFrameState, AI_FRAME_STATE_INSTALLING, "Инсталлируется в данный момент")
REGISTER_ENUM_ENCLOSED(ConditionFrameState, AI_FRAME_STATE_POWERED, "Подключен")
REGISTER_ENUM_ENCLOSED(ConditionFrameState, AI_FRAME_STATE_MOVING, "Двигается")
REGISTER_ENUM_ENCLOSED(ConditionFrameState, AI_FRAME_STATE_TELEPORTATION_ENABLED, "Телепортация разрешена")
REGISTER_ENUM_ENCLOSED(ConditionFrameState, AI_FRAME_STATE_TELEPORTATION_STARTED, "Телепортация началась")
REGISTER_ENUM_ENCLOSED(ConditionFrameState, AI_FRAME_STATE_SPIRAL_CHARGING, "Спираль заряжена на X % и более")
END_ENUM_DESCRIPTOR_ENCLOSED(ConditionFrameState, State)
REGISTER_CLASS(Condition, ConditionFrameState, "Состояние фрейма")

REGISTER_CLASS(Condition, ConditionCorridorOmegaUpgraded, "Коридор Омега проапгрейжен")
REGISTER_CLASS(Condition, ConditionSquadSufficientUnits, "квад состоит из юнитов в указанном количестве")

REGISTER_CLASS(Condition, ConditionMutationEnabled, "Мутация разрешена")
REGISTER_CLASS(Condition, ConditionBuildingNearBuilding, "Расстояние от ядра Игрока1 до любого здания Игрока2 меньше определенного")
REGISTER_CLASS(Condition, ConditionPlayerState, "Проверка состояния игрока")
REGISTER_CLASS(Condition, ConditionIsFieldOn, "Поле включено")

REGISTER_CLASS(Condition, ConditionObjectByLabelExists, "Объект по метке существует")
REGISTER_CLASS(Condition, ConditionKillObjectByLabel, "Объект по метке уничтожен")
REGISTER_CLASS(Condition, ConditionObjectNearObjectByLabel, "Возле объекта по метке находится объект указанного типа")
REGISTER_CLASS(Condition, ConditionWeaponIsFiring, "Спецоружие стреляет")

REGISTER_CLASS(Condition, ConditionTimeMatched, "Осталось времени меньше, чем указано")
REGISTER_CLASS(Condition, ConditionMouseClick, "Клик мыши")
REGISTER_CLASS(Condition, ConditionClickOnButton, "Клик по кнопке")
REGISTER_CLASS(Condition, ConditionToolzerSelectedNearObjectByLabel, "Поверхность возле объекта по метке выделена для выравнивания")
REGISTER_CLASS(Condition, ConditionTerrainLeveledNearObjectByLabel, "Поверхность возле объекта по метке выровнена")

REGISTER_CLASS(Condition, ConditionSetSquadWayPoint, "Флажок скваду установлен")

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(ConditionActivateSpot, Type, "Type")
REGISTER_ENUM_ENCLOSED(ConditionActivateSpot, FILTH, "Скверна")
REGISTER_ENUM_ENCLOSED(ConditionActivateSpot, GEO, "Гео")
END_ENUM_DESCRIPTOR_ENCLOSED(ConditionActivateSpot, Type)
REGISTER_CLASS(Condition, ConditionActivateSpot, "Активировался спот")

REGISTER_CLASS(Condition, ConditionOnlyMyClan, "Остался только мой клан")
REGISTER_CLASS(Condition, ConditionSkipCutScene, "Пропустить кат-сцену")
REGISTER_CLASS(Condition, ConditionCutSceneWasSkipped, "Кат-сцена была пропущена")
REGISTER_CLASS(Condition, ConditionDifficultyLevel, "Уровень сложности")

