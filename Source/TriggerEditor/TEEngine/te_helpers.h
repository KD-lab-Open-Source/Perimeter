/********************************************************************
	created:	2002/09/17
	created:	17:9:2002   12:41
	filename: 	D:\Projects\Quest\QuestEditor\te_helpers.h
	file path:	D:\Projects\Quest\QuestEditor
	file base:	te_helpers
	file ext:	h
	Powerd by:	Илюха
	
	purpose:	вспомогательные функции для редактора триггеров
*********************************************************************/
#ifndef _TE_HELPERS_HEADER_
#define _TE_HELPERS_HEADER_

#include "TriggerExport.h"

namespace te_h{

//!\brief определяет какую сторону перескает луч в прямоугольнике
//! луч исходит из точки ВНУТРИ!!! прямоугольника
BYTE check_inters(const CPoint& a, const CPoint& b, const CPoint* pts,
								float* pkx, float* pky);


inline CPoint const get_center(RECT const& r)
{
	return CPoint((r.right + r.left)>>1, (r.bottom + r.top)>>1);
}

inline CPoint const get_center(CPoint const& a, CPoint const& b)
{
	return CPoint((a.x + b.x)>>1, (a.y + b.y)>>1);
}

inline CPoint const get_lt(RECT const& r)
{
	return CPoint(r.left, r.top);
}
inline bool IsKeyPressed(int keyID)
{
	return GetAsyncKeyState(keyID)>>15 != 0;
}

typedef CPoint TRIANGLE[3];
void getLinkTriangle(CPoint const& pt_from, CPoint const& pt_to, TRIANGLE &trg);
bool pointInTrg( CPoint const& test, TRIANGLE const& trg);


//! Вычисляет точку, где связь начинается
CPoint const getLinkBegin(Trigger const& ownedTrigger, 
						  TriggerLink const& link);
//! Вычисляет точку, где связь кончается
CPoint const getLinkEnd(TriggerLink const& link);
//! Перерасчитывает номер ячейки
//void recalcCellNumber(Trigger& trigger, CPoint const& p, int cell_w, int cell_h);

//! вычисляем отступ от центра bound до точки присоединения линка
CSize getLinkOffset(const CPoint& ptFrom, const CPoint& ptTo, const RECT& bound);
}//namespace te_h


#endif//_TE_HELPERS_HEADER_