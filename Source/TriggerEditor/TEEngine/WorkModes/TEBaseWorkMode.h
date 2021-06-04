/********************************************************************
	created:	2003/05/21
	created:	21:5:2003   17:12
	file base:	TEBaseWorkMode
	file ext:	h
	Powerd by:	Илюха при идейном руководстве и моральной поддержке of Scmorr
	
	purpose:	Реализация основного режима работы для редактора триггеров
*********************************************************************/
#pragma once

class TriggerEditorView;
class TEBaseWorkModeDrawerBase;


class MakeLinkActive;
class MoveTEElementPos;
class AddLink;
class AddElement;

#include "TriggerExport.h"

#include "TESingleChainWorkMode.h"
#include "TEEngine/UndoManager/teundomanager.h"
#include <memory>
#include <map>


class ClipBoardDataInserter;

class TEBaseWorkMode :
	public TESingleChainWorkMode
{
	friend class ClipBoardDataInserter;

	typedef std::map<TriggerChain const*, TEUndoManager> UndoBuffers;
	typedef TESingleChainWorkMode Base;
	enum eLinkAnchorTest {LAT_NONE, LAT_BEGIN, LAT_END};
	enum eBetweenPlotsLayout{
		BPL_NOWERE,
		BPL_VERT,
		BPL_HORZ,
		BPL_VERT_HORZ,
	};
	friend class MakeLinkActive;
	friend class MoveTEElementPos;
	friend class AddLink;
	friend class AddElement;
public:
	TEBaseWorkMode(TriggerEditorLogic* logic);
	virtual ~TEBaseWorkMode(void);

	void OnLButtonDown(TriggerEditorView* source, UINT nFlags, CPoint point);
	void OnLButtonUp(TriggerEditorView* source, UINT nFlags, CPoint point);
	void OnLButtonDblClk(TriggerEditorView* source, UINT nFlags, CPoint point);

	void OnRButtonDown(TriggerEditorView* source, UINT nFlags, CPoint point);
	void OnRButtonUp(TriggerEditorView* source, UINT nFlags, CPoint point);

	void OnMouseMove(TriggerEditorView* source, UINT nFlags, CPoint point);

	void OnKeyDown(TriggerEditorView* source, UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyUp(TriggerEditorView* source, UINT nChar, UINT nRepCnt, UINT nFlags);

	void OnPaint(TriggerEditorView* source, CDC* pdc, CRect const& rcPaint);

	BOOL OnCommand(TriggerEditorView* source, WPARAM wParam, LPARAM lParam);

	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	bool OnStart();
	bool OnEnd();

	BOOL OnDropData(TriggerEditorView* source, VOID* pData, const CPoint& point);
	bool CanDrop(TriggerEditorView* source, VOID* pData, const CPoint& point);

	virtual bool OnCommandUpdateUI(TriggerEditorView *source, CCmdUI* pCmdUI);

	bool setDrawer(TEBaseWorkModeDrawerBase* ptrDrawer);
	int getHilitedLinksTriggerIndex() const;
	Trigger const* getHilitedLinksTrigger() const;

	virtual void setTriggerChain(TriggerChain *chain);
protected:
	//! setters/getters
	void setHilitedLinksTriggerIndex(int idx);
	void setMouseMoved(bool bVal);
	bool getMouseMoved() const;

	void setLinkOwnerIndex(int idx);
	int getLinkOwnerIndex() const;
	void setActiveLinkIndex(int idx);
	int getActiveLinkIndex() const;

	void setCurrentPoint(CPoint const& point);
	CPoint const& getCurrentPoint() const;

	void setStartMovementPoint(CPoint const& point);
	CPoint const& getStartMovementPoint() const;

	void setDrawedLinkType(int newType);
	int getDrawedLinkType() const;

	void setLinkAnchorTestResult(eLinkAnchorTest eVal);
	eLinkAnchorTest getLinkAnchorTestResult() const;

	void setActiveLinkType(int iVal);
	int getActiveLinkType() const;

	void setBetweenPlotsLayoutTestResult(eBetweenPlotsLayout eVal);
	eBetweenPlotsLayout getBetweenPlotsLayoutTestResult() const;

	static eLinkAnchorTest findLinkAnchor(TriggerLink const& link, const CPoint& p);

	void activateLink(int linkIndex, int linkOwnerIndex);
	void OnTeLinkDelete(TriggerEditorView* source);
	void OnTEDeleteTrigger(TriggerEditorView* source);
	void OnTeMakeActive(TriggerEditorView* source);
	void OnClearStarts(TriggerEditorView* source);
	void OnTeLinkType(UINT id, TriggerEditorView* source);
	void OnAutoRestart(TriggerEditorView* source);
	void OnInsertColumns(TriggerEditorView* source);
	void OnInsertRows(TriggerEditorView* source);
	void OnDeleteColumn(TriggerEditorView* source);
	void OnDeleteRow(TriggerEditorView* source);
	void OnTEHiliteLinks(TriggerEditorView* source);
	void OnEditUndo(TriggerEditorView* source);
	void OnEditRedo(TriggerEditorView* source);
	void onTERename(TriggerEditorView* source);
	void onTEColor(TriggerEditorView* source);
	void onEditCut(TriggerEditorView* source);
	void onEditCopy(TriggerEditorView* source);
	void onEditPaste(TriggerEditorView* source);

	void onUpdateEditCut(TriggerEditorView* source, CCmdUI* pCmdUI);
	void onUpdateEditCopy(TriggerEditorView* source, CCmdUI* pCmdUI);
	void onUpdateEditPaste(TriggerEditorView* source, CCmdUI* pCmdUI);

	void onEditSelectAll(TriggerEditorView* source);
	void onUpdateEditSelectAll(TriggerEditorView* source, CCmdUI* pCmdUI);

	bool canPlugLink(int childTriggerIndex, int parentTriggerIndex,
					TriggerChain const& chain) const;

	void ModifyMenu(CMenu* pmenu, UINT menuResId, TriggerEditorView* source);

	//! Попадает ли точка в ячейки между элементами
	bool TestForBetweenPlots(TriggerEditorView* source, CPoint p);
	void clearActive(TriggerChain &chain);

	enum eUndoOperation {UO_NOT, UO_UNDO};
	void moveLinkEnd(CPoint const& point);

	//! Настройка
	bool initTrigger(TriggerEditorView* source, 
					Trigger* trigger, 
					int actionIndex);
	//! Добавляет к прямоугольнику стратегии, указанный прямоугольник
	static void add2StrategyRect(TriggerChain &chain, 
							RECT const& rect4Add);

	void changeActiveLinkType(int type);
	void changeAutoRestart4ActiveLink();

	CString getUniqueName(CString const& sourceName) const;

	bool moveDraggedTitles(CPoint const& point) const;
	void drawDraggedTitles(TriggerEditorView* source) const;

	void moveSelection(TriggerEditorView* source, CPoint const& point);
	void deleteSelection();
	void deleteSelection(TEUndoManager::Bunch& undoBunch, 
						TEUndoManager::Bunch& redoBunch);

	bool copySelectionToClipBuffer();
	bool insertTriggerClipBuffer();


	//! \brief Переименовывает элементы, исходя из набора имен в цепочке,
	//! и обновляет их имена в связях
	void renameTriggersForChain(TriggerChain const& chain,
						TriggerList& triggers);
	void renameTrigger(TriggerList& triggers,
						Trigger& trigger, LPCTSTR newName);

	//! Сдвигает индексы элементов к нулю.Самый левый и самый верхний индексы = 0
	void makeTriggerCellIndexesRelative(TriggerList& triggers) const;

private:
	void* getDraggedData(void* pData) const;
	TEUndoManager& getUndoManager(TriggerChain const*chain);
private:
	//! была ли сдвинута мышь
	bool isMouseMoved_;
	//! Индекс елемент, владеющего выбранной связью
	int linkOwnerIndex_;
	//! Индекс активной связи
	int activeLinkIndex_;
	//! Текущая точка для операций перетаскивания и добавления связей
	CPoint currentPoint_;
	//! Тип линка, который мы тянем
	int drawedLinkType_;
	//! куда попали при клике мышью на линк
	eLinkAnchorTest linkAnchorTestResult_;
	//! тип выделенного линка
	int activeLinkType_;
	//! классик, который содержит данные для прорисовки
	std::auto_ptr<DataKeeper> dataKeeper_;

	//! класс производящий отрисовку
	TEBaseWorkModeDrawerBase *drawer_;

	//! Если кликнули правой кнопкой не по элементу и не по связи, то куда.
	eBetweenPlotsLayout betweenPlotsLayoutTestResult_;

	//Элемент, все связи к которому и от которого рисуем спец. цветом
	int hilitedLinksTriggerIndex_;
	//undo-действие для текущей операции
	TEUndoManager::Action undo_;
	UndoBuffers undoBuffers_;
};
