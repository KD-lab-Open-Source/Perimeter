#pragma once

class CTreeListCtrl;
class CTreeListItem;

struct CONTROL_CUSTOM_DRAW_INFO {
	HDC hdc;///< Девайс контекст
	RECT rc;///< Прямоугольник перерисовки
	COLORREF clrBckgrnd;///< Цвет фона
	HBITMAP hbmpBckgrnd;///< картинка, которую положим на фон
};

struct ITEM_CUSTOM_DRAW_INFO {
	HDC hdc;				///< Девайс контекст
	RECT rc;				///< Прямоугольник перерисовки
	COLORREF clrTextBk;		///< Цвет фона под текстом
	COLORREF clrText;		///< Цвет текста
	CTreeListItem* item;	///< Узел, который прорисовывается
	int iRow;				///< Номер строки
	bool useBackgrndClr;	///< Использовать ли clrTextBk.Если у контрола есть 
							///< стиль TLC_BKGNDIMAGE, то false
};

struct SUBITEM_CUSTOM_DRAW_INFO {
	HDC hdc;				///< Девайс контекст
	RECT rc;				///< Прямоугольник перерисовки
	RECT rcText;			///< Прямоугольник, в который будет рисоваться текст
	COLORREF clrText;		///< Цвет текста
	COLORREF clrTextBk;		///< Цвет фона под текстом
	UINT uTextFormat;			///< Выравнивание теста
	CTreeListItem* item;	///< Узел, который прорисовывается
	int iSubItem;			///< Subitem
	bool useBackgrndClr;	///< Использовать ли clrTextBk.Если у контрола есть 
							///< стиль TLC_BKGNDIMAGE, то false
};

class ICustomDrawNotifyListener
{
public:
/*!
 	все функции возвращают значения зарезервированные для прорисовки контролов в
	Windows. См. документацию по NM_CUSTOMDRAW
*/
	virtual DWORD onPrepaint(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onPostpaint(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onPreerase(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onPosterase(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi) = 0;

	virtual DWORD onItemPrepaint(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onItemPostpaint(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onItemPreerase(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi) = 0;
	virtual DWORD onItemPosterase(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi) = 0;

	virtual DWORD onSubitemPrepaint(CTreeListCtrl& source, SUBITEM_CUSTOM_DRAW_INFO* pscdi) = 0;
	virtual DWORD onSubitemPostpaint(CTreeListCtrl& source, SUBITEM_CUSTOM_DRAW_INFO* pscdi) = 0;

	virtual ~ICustomDrawNotifyListener();
};

inline ICustomDrawNotifyListener::~ICustomDrawNotifyListener(){}
