#pragma once

class TETreeNotifyListener
{
public:
	virtual bool onNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult) = 0;
	virtual bool onCommand(WPARAM wParam, LPARAM lParam)  = 0;
};
