/********************************************************************
	created:	2003/04/11
	created:	11:4:2003   18:30
	filename: 	d:\Projects\Quest\QuestEditor\ScaleInterfaces.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	ScaleInterfaces
	file ext:	h
	author:		Илюха
	
	purpose:	содержит интерфейсы управления масштабом
*********************************************************************/
#pragma once

interface IScaleMgr;
//реализует тот кого можно масштабировать.
interface IScalable
{
	virtual void SetScaleMgr(IScaleMgr* psmgr) = 0;
	virtual void IncreaseScale() = 0;
	virtual void DecreaseScale() = 0;
	virtual	float CurrentScale() = 0;
	virtual void SetScale(float fscale) = 0;
};

//реализует тот, кто масштабирует
interface IScaleMgr
{
	virtual void SetScalable(IScalable* pscalable) = 0;
	//обновляем информацию о масштабе
	virtual void UpdateScaleInfo() = 0;
};