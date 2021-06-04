#pragma once
#include "scaleinterfaces.h"
#include <memory>

class CScaleBar;
class ScaleMgr :
	public IScaleMgr
{
public:
	void SetScalable(IScalable* pscalable);
	//обновляем информацию о масштабе
	void UpdateScaleInfo();

	ScaleMgr(void);
	~ScaleMgr(void);

	bool Create(CFrameWnd* pParent, DWORD barid);
	void Show() const;
	void Hide() const;
	bool IsVisible() const;

	//возвращает указатель на окно
	CWnd* getWindow();

	void dockBar(UINT dockBarID);
private:
	std::auto_ptr<CScaleBar> scaleBar_;
	CFrameWnd* frameWnd_;
};
