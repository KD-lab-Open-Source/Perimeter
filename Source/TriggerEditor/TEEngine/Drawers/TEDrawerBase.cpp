#include "stdafx.h"
#include "tedrawerbase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TEDrawerBase::TEDrawerBase(void)
{
}

TEDrawerBase::~TEDrawerBase(void)
{
}

void TEDrawerBase::draw(CDC* pdc, TriggerEditorView* pwnd, CRect const& rcPaint) const
{

}

void TEDrawerBase::drawDraggedRect(HDC dc, CRect const&r, HPEN hpen) const
{

}
