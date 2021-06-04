#include "stdafx.h"
#include "TEEngine/te_consts.h"
#include "testatuscoloredeledrawer.h"

TEStatusColoredEleDrawer::TEStatusColoredEleDrawer(void)
{
}

TEStatusColoredEleDrawer::~TEStatusColoredEleDrawer(void)
{
}

HBRUSH	TEStatusColoredEleDrawer::SelectTriggerBackBrush(
						Trigger const& trigger) const
{
	switch(trigger.state()) 
	{
		case Trigger::SLEEPING:
			{
				static CBrush s_brInactiveEle(INACTIVE_ELE_CLR);
				return s_brInactiveEle;
			}
			break;
		case Trigger::CHECKING:
			{
				static CBrush s_brWaiting(WAITING_ELE_CLR);
				return s_brWaiting;
			}
			break;
		case Trigger::WORKING:
			{
				static CBrush s_br(WORKING_ELE_CLR);
				return s_br;
			}
			break;
		case Trigger::DONE:
			{
				static CBrush s_br(DONED_ELE_CLR);
				return s_br;
			}
			break;
	}
	ASSERT(0);
	return NULL;
}

HBRUSH	TEStatusColoredEleDrawer::SelectLinkBrushColor(
													TriggerLink const& link) const
{
	if (link.active())
	{
		static CBrush s_brAcitve(ACTIVE_LINK_CLR);
		return s_brAcitve;
	}
	else
	{
		static CBrush s_brInactive(INACTIVE_LINK_CLR);
		return s_brInactive;
	}
}

HPEN	TEStatusColoredEleDrawer::SelectLinkPenColor(TriggerLink const& link) const
{
	if (link.active())
	{
		static CPen s_penAcitve(PS_SOLID, 2, ACTIVE_LINK_CLR);
		return s_penAcitve;
	}
	else
	{
		static CPen s_penInactive(PS_SOLID, 2, INACTIVE_LINK_CLR);
		return s_penInactive;
	}
}

void TEStatusColoredEleDrawer::draw(CDC* pdc, 
									TriggerEditorView* source, 
									IDrawData* pdd, 
									CRect const& rcPaint) const
{
	BaseTEDrawer::draw(pdc, source, pdd, rcPaint);
	DrawChain(source, pdc, rcPaint, **pdd->getDrawedChains().first);
}