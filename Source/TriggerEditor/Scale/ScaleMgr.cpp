#include "stdafx.h"
#include "resource.h"
#include "scalemgr.h"
#include "ScaleBar.h"


ScaleMgr::ScaleMgr(void):
frameWnd_(NULL)
{
}

ScaleMgr::~ScaleMgr(void)
{
}

void ScaleMgr::SetScalable(IScalable* pscalable)
{
	scaleBar_->SetScalable(pscalable);
}
//обновляем информацию о масштабе
void ScaleMgr::UpdateScaleInfo()
{
	scaleBar_->UpdateScaleInfo();
}

bool ScaleMgr::Create(CFrameWnd* pParent, DWORD id)
{
	frameWnd_ = pParent;

	scaleBar_.reset(new CScaleBar);
	if (!scaleBar_->Create(frameWnd_, id))
		return false;
	scaleBar_->EnableDocking(CBRS_ALIGN_ANY);
	return true;
}

void ScaleMgr::dockBar(UINT dockBarID)
{
	frameWnd_->DockControlBar(scaleBar_.get(), dockBarID);
}

bool ScaleMgr::IsVisible() const{
	return (IsWindowVisible(scaleBar_->m_hWnd) == TRUE);
}

void ScaleMgr::Show() const
{
	frameWnd_->ShowControlBar(scaleBar_.get(), TRUE, FALSE); 
}

void ScaleMgr::Hide() const
{
	frameWnd_->ShowControlBar(scaleBar_.get(), FALSE, FALSE); 
}

//возвращает указатель на окно
CWnd* ScaleMgr::getWindow()
{
	return scaleBar_.get();
}
