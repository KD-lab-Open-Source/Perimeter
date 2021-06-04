/********************************************************************
	created:	2002/09/24
	created:	24:9:2002   18:12
	filename: 	D:\Projects\Quest\QuestEditor\te_helpers.cpp
	file path:	D:\Projects\Quest\QuestEditor
	file base:	te_helpers
	file ext:	cpp
	Powerd by:	Илюха
	
	purpose:	невстроенные вспомогательные функции
*********************************************************************/
#include "stdafx.h"

#include <algorithm>

#include <xmath.h>

#include "te_helpers.h"
#include "geometry_helpers.h"

namespace te_h{


BYTE check_inters(const CPoint& a, const CPoint& b, const CPoint* pts, 
				  float* pkx, float *pky)
{
	for(BYTE i = 0; i < 3; ++i)
	{
		if(cross_segms(a, b, pts[i], pts[i+1],pkx, pky))
			return i;
	}
	//какая-то одна сторона должна пересекаться. Если не первые 3 то точно последняя
	VERIFY(cross_segms(a,b, pts[3], pts[0], pkx, pky));
	return 3;
}

int sign(int value){
	return value < 0? -1 : 1;
}

int calcLineEquation(POINT const& a, POINT const& b, POINT const& p)
{
	int const A = b.y - a.y;
	int const B = b.x - a.x;
	int const C = -A*a.x + B*a.y;
	return A*p.x - B*p.y + C;
}

bool pointInTrg(CPoint const& test, TRIANGLE const& trg)
{
	int testRes = calcLineEquation(trg[0], trg[1], test);
	int vertexRes = calcLineEquation(trg[0], trg[1], trg[2]);
	if (sign(testRes) != sign(vertexRes))
		return false;
	testRes = calcLineEquation(trg[1], trg[2], test);
	vertexRes = calcLineEquation(trg[1], trg[2], trg[0]);
	if (sign(testRes) != sign(vertexRes))
		return false;
	testRes = calcLineEquation(trg[2], trg[0], test);
	vertexRes = calcLineEquation(trg[2], trg[0], trg[1]);
	if (sign(testRes) != sign(vertexRes))
		return false;
	return true;
}

void getLinkTriangle(CPoint const& pt_from, CPoint const& pt_to, TRIANGLE &trg)
{
	Vect2f ort_f(pt_to.x - pt_from.x, pt_to.y - pt_from.y);
	ort_f.normalize(10.f);//получаем  единичные вектор * параметр функции
	Vect2f normal(ort_f.y, -ort_f.x);

	const CPoint ort_x2(round(ort_f.x)<<1, round(ort_f.y)<<1);
	//точка, к которой будет сходиться стрела
	trg[0] = pt_to;
	trg[1] = CPoint(pt_to - ort_x2);
	trg[2] = CPoint(pt_to - ort_x2);

	trg[1].x += round(normal.x)>>1;
	trg[1].y += round(normal.y)>>1;

	trg[2].x += round(-normal.x)>>1;
	trg[2].y += round(-normal.y)>>1;
}

CPoint const getLinkBegin(Trigger const& ownedTrigger, TriggerLink const& link)
{
	return get_center(ownedTrigger.boundingRect()) + link.parentOffset();
}

CPoint const getLinkEnd(TriggerLink const& link)
{
	return get_center(link.child->boundingRect()) + link.childOffset();
}

CSize getLinkOffset(const CPoint& ptFrom, const CPoint& ptTo, 
										const RECT& bound)
{
	CPoint pts[4];
	pts[0] = CPoint(bound.left, bound.top);
	pts[1] = CPoint(bound.left, bound.bottom);
	pts[2] = CPoint(bound.right, bound.bottom);
	pts[3] = CPoint(bound.right, bound.top);

	float kx = 0.f;

	BYTE bSide = check_inters(ptFrom, ptTo,pts, &kx, NULL);
	ASSERT(0.f<= kx&&kx<=1.f);

	CSize dir = ptTo - ptFrom;
	return CPoint(ptFrom.x + kx*dir.cx, ptFrom.y + kx*dir.cy) - get_center(bound);
}


}//end of namespace te_h