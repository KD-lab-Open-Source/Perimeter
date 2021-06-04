#pragma once

interface ITriggerProfList
{
	virtual bool show() const = 0;
	virtual bool hide() const = 0;
	virtual bool isVisible() const = 0;
	virtual bool load() = 0;
	virtual bool next() const = 0;
	virtual bool prev() const = 0;
	virtual bool activate() = 0;
	virtual bool canBeActivated() const = 0;
};
