/********************************************************************
	created:	2003/05/23
	created:	23:5:2003   17:04
	filename: 	d:\Projects\Quest\QuestEditor\TEConditionColoredEleDrawer.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	TEConditionColoredEleDrawer
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Выбираем цвета элементов в соответствии с условиями
				цвета связей в соответствии с типом
*********************************************************************/
#pragma once
#include "TEEngine/Drawers/BaseTEDrawer.h"
//#include <vector>
#include <hash_map>
#include "TEEngine/te_consts.h"

class TEConditionColoredEleDrawer :
	public BaseTEDrawer
{
private:
	typedef struct tagBRUSH_INDX_ENTRY : public std::pair<int, HBRUSH> {
		typedef std::pair<int, HBRUSH> _Base;
		tagBRUSH_INDX_ENTRY():_Base(0, NULL){}
		tagBRUSH_INDX_ENTRY(int key, COLORREF clr):_Base(key, CreateSolidBrush(clr)){}
		bool operator == (int type) const {
			return first == type;
		}
	} BRUSH_INDEX_ENTRY;

	struct LinkPen 
	{
		HPEN pen_;
//		LinkPen(COLORREF crColor);
		~LinkPen();
		operator HPEN();
	};
	struct Brush
	{
		HBRUSH brush_;
		~Brush();
		operator HBRUSH();
	};
	//	typedef CBrush BrushVector_t[LINK_TYPES_COUNT];
	//	typedef LinkPen PenVector_t[LINK_TYPES_COUNT];
	typedef Brush BrushVector_t[LINK_TYPES_COUNT];
	typedef LinkPen PenVector_t[LINK_TYPES_COUNT];
	typedef std::vector<BRUSH_INDEX_ENTRY> BrushIdx_t;

public:
	TEConditionColoredEleDrawer(void);
	~TEConditionColoredEleDrawer(void);
protected:
	virtual HBRUSH	SelectTriggerBackBrush(Trigger const& trigger) const;
	virtual HBRUSH	SelectLinkBrushColor(TriggerLink const& link) const;
	virtual HPEN	SelectLinkPenColor(TriggerLink const& link) const;
protected:
	HBRUSH getHiddenObjectBrush() const;

	BOOL setInversedCondBrush(COLORREF color);
	BOOL setInversedCondBrush(HBRUSH brush);

	BOOL setDonedLinkBrush(COLORREF color);
	BOOL setDonedLinkBrush(HBRUSH brush);

	BOOL setDonedLinkPen(HPEN hpen);
	BOOL setDonedLinkPen(int nPenStyle, int nWidth, COLORREF crColor);

	HBRUSH getLinkTypeBrush(int type) const{
		ASSERT(0<=type&&type<LINK_TYPES_COUNT);
		return linkBrushs_[type];
	}
	HPEN getLinkTypePen(int type) const{
		ASSERT(0<=type&&type<LINK_TYPES_COUNT);
		return linkPens_[type];
	}
private:
	static BrushIdx_t const initBrushIdx();
private:
	static BrushVector_t linkBrushs_;
	static PenVector_t linkPens_;
	static BrushIdx_t brushIndex_;
};
