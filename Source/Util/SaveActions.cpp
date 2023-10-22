#include "StdAfx.h"
#include "UnitAttribute.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"
#include "Save.h"

////////////////////////////////////////////////////
//		Actions
/////////////////////////////////////////////////////

REGISTER_CLASS(Action, Action, "Пустое действие, вставлять не надо!")
REGISTER_CLASS(Action, ActionForAI, "---------------")
REGISTER_CLASS(Action, ActionDelay, "Задержка времени")
REGISTER_CLASS(Action, ActionSetCamera, "Установка Камеры")
REGISTER_CLASS(Action, ActionOscillateCamera, "Тряска Камеры")
REGISTER_CLASS(Action, ActionVictory, "Победа")
REGISTER_CLASS(Action, ActionDefeat, "Поражение")

REGISTER_CLASS(Action, ActionTeleportationOut, "Телепортировать Фрейм с мира")
REGISTER_CLASS(Action, ActionKillObject, "Уничтожить объект")
REGISTER_CLASS(Action, ActionInstallFrame, "Инсталлировать фрейм")
REGISTER_CLASS(Action, ActionFrameMove, "Послать фрейм к метке")
REGISTER_CLASS(Action, ActionFrameDetach, "Поднять фрейм")
REGISTER_CLASS(Action, ActionOrderBuilding, "Заказать здание")
REGISTER_CLASS(Action, ActionHoldBuilding, "Заморозить строительство")

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(ActionSellBuilding, AISellFactor, "AISellFactor")
REGISTER_ENUM_ENCLOSED(ActionSellBuilding, AI_SELL_CLOSEST_TO_FRAME, "Ближайший к фрейму")
REGISTER_ENUM_ENCLOSED(ActionSellBuilding, AI_SELL_FAREST_FROM_FRAME, "Удаленный от фрейма")
REGISTER_ENUM_ENCLOSED(ActionSellBuilding, AI_SELL_IF_DAMAGE_GREATER, "Продавать, если урон больше процента")
REGISTER_ENUM_ENCLOSED(ActionSellBuilding, AI_SELL_IF_GUN_CANT_REACH_BUILDINGS, "Продавать, если пушка не может достать до зданий")
END_ENUM_DESCRIPTOR_ENCLOSED(ActionSellBuilding, AISellFactor)
REGISTER_CLASS(Action, ActionSellBuilding, "Продать здание")

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(ActionSwitchGuns, Mode, "Mode")
REGISTER_ENUM_ENCLOSED(ActionSwitchGuns, ON, "Включить")
REGISTER_ENUM_ENCLOSED(ActionSwitchGuns, OFF, "Выключить")
END_ENUM_DESCRIPTOR_ENCLOSED(ActionSwitchGuns, Mode)
REGISTER_CLASS(Action, ActionSwitchGuns, "Включить/выключить пушки")

REGISTER_CLASS(Action, ActionUpgradeOmega, "Апргрейд Омеги")

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(ActionChargeCores, ChargeCoresStrategy, "ChargeCoresStrategy")
REGISTER_ENUM_ENCLOSED(ActionChargeCores, CHARGE_CORES_NONE, "Никакие ядра")
REGISTER_ENUM_ENCLOSED(ActionChargeCores, CHARGE_CORES_INNER, "Внутренние ядра")
REGISTER_ENUM_ENCLOSED(ActionChargeCores, CHARGE_CORES_OUTER, "Внешние ядра")
REGISTER_ENUM_ENCLOSED(ActionChargeCores, CHARGE_CORES_ALL, "Все ядра")
END_ENUM_DESCRIPTOR_ENCLOSED(ActionChargeCores, ChargeCoresStrategy)
REGISTER_CLASS(Action, ActionChargeCores, "Зарядить ядра")

REGISTER_CLASS(Action, ActionSwitchFieldOn, "Включить поле")
REGISTER_CLASS(Action, ActionSquadOrderUnits, "Заказать юнитов в сквад")

REGISTER_CLASS(Action, ActionSquadAttack, "Атаковать сквадом")
REGISTER_CLASS(Action, ActionSquadMove, "Послать сквад в точку объекта по метке")
REGISTER_CLASS(Action, ActionAttackBySpecialWeapon, "Атаковать спецоружием")
REGISTER_CLASS(Action, ActionRepareObjectByLabel, "Отремонтировать объекта по метке")
REGISTER_CLASS(Action, ActionActivateObjectByLabel, "Активировать объект по метке")

REGISTER_CLASS(Action, ActionDeactivateObjectByLabel, "Деактивировать объект по метке")
REGISTER_CLASS(Action, ActionActivateAllSpots, "Активировать все споты")
REGISTER_CLASS(Action, ActionDeactivateAllSpots, "Деактивировать все споты")
REGISTER_CLASS(Action, ActionSetControlEnabled, "Запретить/разрешить управление игрока")
REGISTER_CLASS(Action, ActionMessage, "Сообщение")

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(ActionTask, Type, "Type")
REGISTER_ENUM_ENCLOSED(ActionTask, ASSIGNED, "Назначена")
REGISTER_ENUM_ENCLOSED(ActionTask, COMPLETED, "Выполнена")
REGISTER_ENUM_ENCLOSED(ActionTask, FAILED, "Провалена")
REGISTER_ENUM_ENCLOSED(ActionTask, TO_DELETE, "Удалить")
END_ENUM_DESCRIPTOR_ENCLOSED(ActionTask, Type)
REGISTER_CLASS(Action, ActionTask, "Задача")

REGISTER_CLASS(Action, ActionSetCameraAtObject, "Установить камеру на объект")
REGISTER_CLASS(Action, ActionSetControls, "Установить параметры кнопок")
REGISTER_CLASS(Action, ActionSelectUnit, "Селектировать юнита")
REGISTER_CLASS(Action, ActionProduceBrigadierOrProrab, "Произвести бригадира или прораба")
REGISTER_CLASS(Action, ActionSetInterface, "Включить/выключить интерфейс")
