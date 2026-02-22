#ifndef _ISETTING_H

#define _ISETTING_H

#pragma once

class LocaleMngr;

class ISetting
{
public:
	virtual ~ISetting() = 0;

	//! Считать значение из его(значения) хранилища
	virtual bool readFromStorage() = 0;
	//! Записать значение в хранилище
	virtual bool saveToStorage() const = 0;

	//! Организуем обмент между контролом в диалоге и значением
	virtual void DDX_Exchange(CDataExchange* pDX) = 0;
	//! Инициализация котрола относящегося к настройке(например заполнить комбобокс)
	virtual void initControl(CWnd* parent, LocaleMngr const& locMngr) const = 0;
//	//! Назначаем получателя уведомления об изменении значения
//	virtual void setChangeListener(ISettingChangeListener* listener) = 0;
};

#endif
