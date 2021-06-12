/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl : Main control class drived from CWnd class
// Writter       : TigerX 
// Email         : idemail@yahoo.com
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_TREELISTCTRL_H__55B6E4E9_1E57_4FEE_AA05_77C052ACE9B5__INCLUDED_)
#define AFX_TREELISTCTRL_H__55B6E4E9_1E57_4FEE_AA05_77C052ACE9B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeListCtrl.h : header file

#include "TreeListItem.h"
#include "TreeListColumnInfo.h"
#include "ICustomDrawNotifyListener.h"

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl window
// #include "../AttribEditorCtrl/Resource_AttribEditorCtrl.h"
#define IDB_TREELIST_BITMAP             8000
#define IDC_CURSOR_SIZE                 8002
#define IDB_TREELIST_CHECK              8003
#define IDB_TREELIST_LOCK               8004
#define IDC_CURSOR_BUTTON               8005
#define IDC_CURSOR_CHECK                8006
#define IDC_CURSOR_LOCK                 8007
#define IDB_TREELIST_TREE               8008
#define IDB_TREELIST_SORT               8009
#define IDB_TREELIST_HEADER             8010
#define IDB_TREELIST_DISABLE            8011
#define IDC_CURSOR_VERT                 8012
#define IDC_CURSOR_HORZ                 8013
#define IDC_CURSOR_CROSS                8014
#define IDC_CURSOR_DROP                 8015
#define IDC_CURSOR_STOP                 8016
#define IDC_CURSOR_SPLIT                8017


#define TREELISTCTRL_CLASSNAME		_T("TreeListCtrl.TigerX_And_KB-I")

// TreeListCtrl Styles
#define TLC_TREELIST				0x00000001				// TreeList or List
#define TLC_BKGNDIMAGE				0x00000002				// image background
#define TLC_BKGNDCOLOR				0x00000004				// colored background ( not client area )
#define TLC_DOUBLECOLOR				0x00000008				// double color background

#define TLC_MULTIPLESELECT			0x00000010				// single or multiple select
#define TLC_SHOWSELACTIVE			0x00000020				// show active column of selected item
#define TLC_SHOWSELALWAYS			0x00000040				// show selected item always
#define TLC_SHOWSELFULLROWS			0x00000080				// show selected item in fullrow mode

#define TLC_HEADER					0x00000100				// show header
#define TLC_HGRID					0x00000200				// show horizonal grid lines
#define TLC_VGRID					0x00000400				// show vertical grid lines
#define TLC_TGRID					0x00000800				// show tree horizonal grid lines ( when HGRID & VGRID )

#define TLC_HGRID_EXT				0x00001000				// show extention horizonal grid lines
#define TLC_VGRID_EXT				0x00002000				// show extention vertical grid lines
#define TLC_HGRID_FULL				0x00004000				// show full horizonal grid lines
#define TLC_READONLY				0x00008000				// read only

#define TLC_TREELINE				0x00010000				// show tree line
#define TLC_ROOTLINE				0x00020000				// show root line
#define TLC_BUTTON					0x00040000				// show expand/collapse button [+]
#define TLC_CHECKBOX				0x00080000				// show check box
#define TLC_LOCKBOX					0x00100000				// show lock box
#define TLC_IMAGE					0x00200000				// show image
#define TLC_HOTTRACK				0x00400000				// show hover text

#define TLC_DRAG					0x01000000				// drag support
#define TLC_DROP					0x02000000				// drop support
#define TLC_DROPTHIS				0x04000000				// drop on this support
#define TLC_DROPROOT				0x08000000				// drop on root support

#define TLC_HEADDRAGDROP			0x10000000				// head drag drop
#define TLC_HEADFULLDRAG			0x20000000				// head full drag
#define TLC_NOAUTOCHECK				0x40000000				// do NOT auto set checkbox of parent & child
#define TLC_NOAUTOLOCK				0x80000000				// do NOT auto set lockbox of parent & child

// TreeListCtrl State
#define TLS_SUBCLASSFROMCREATE		0x00000001				// subclass when creating
#define TLS_OFFSCREENBUFFER			0x00000002				// use off screen buffer
#define TLS_FOCUS					0x00000004				// focus
#define TLS_CAPTURE					0x00000008				// capture
#define TLS_MODIFY					0x00000010				// modify
#define TLS_MODIFYING				0x00000020				// ending modify
#define TLS_DRAG					0x00000040				// drag

// TreeListItem Consts
CTreeListItem* const TLI_ROOT = (CTreeListItem*)(ULONG_PTR)0xFFFF0001;	// root item
CTreeListItem* const TLI_LAST = (CTreeListItem*)(ULONG_PTR)0xFFFF0002;	// last child item
CTreeListItem* const TLI_FIRST = (CTreeListItem*)(ULONG_PTR)0xFFFF0003;	// first child item
CTreeListItem* const TLI_SORT = (CTreeListItem*)(ULONG_PTR)0xFFFF0004;	// sort item

// TreeListCtrl TreeLine Formats
#define TLL_TOP						0x0001					// top line
#define TLL_LEFT					0x0002					// left line
#define TLL_RIGHT					0x0004					// right line
#define TLL_BOTTOM					0x0008					// bottom line

// TreeListCtrl TreeButton Formats
#define TLB_PLUS					0x0001					// plus button
#define TLB_MINUS					0x0002					// minus button
#define TLB_UNKNOW					0x0004					// unknow button

// TreeListCtrl HitTest Position
#define TLHT_ONITEMSPACE			0x0001					// on space
#define TLHT_ONITEMBUTTON			0x0002					// on button
#define TLHT_ONITEMCHECKBOX			0x0004					// on checkbox
#define TLHT_ONITEMLOCKBOX			0x0008					// on lockbox
#define	TLHT_ONITEMIMAGE			0x0010					// on icon
#define TLHT_ONITEMTEXT				0x0020					// on item text

#define TLHT_ONITEM	( TLHT_ONITEMSPACE | TLHT_ONITEMBUTTON | TLHT_ONITEMCHECKBOX | TLHT_ONITEMLOCKBOX | TLHT_ONITEMIMAGE  | TLHT_ONITEMTEXT )

// TreeListCtrl Given Consts
#define TLGN_ROOT					0x0000
#define TLGN_NEXT					0x0001
#define TLGN_PREVIOUS				0x0002
#define TLGN_PARENT					0x0003
#define TLGN_CHILD					0x0004
#define TLGN_FIRSTVISIBLE			0x0005
#define TLGN_NEXTVISIBLE			0x0006
#define TLGN_PREVIOUSVISIBLE		0x0007
#define TLGN_DROPHILITE				0x0008
#define TLGN_CARET					0x0009
#define TLGN_NEXTSELECT				0x000A

// TreeListCtrl Next Item Consts
#define TLNI_ABOVE					0x0100					// find next item form down to up
#define TLNI_BELOW					0x0200					// find next item form up to down

// TreeListCtrl SetColumnWidth Consts
#define TLSCW_AUTOSIZE              -1						// autosize column width by item width
#define TLSCW_AUTOSIZE_USEHEADER    -2						// autosize column width by item width and header width
#define TLSCW_AUTOSIZEV				-3						// autosize column width by visible item width
#define TLSCW_AUTOSIZEV_USEHEADER	-4						// autosize column width by visible item width and header width

// TreeListCtrl Message
#define TLN_FIRST					(NM_LAST - 1U)
// перед изменением выделения узла 
#define TLN_SELCHANGING				(TLN_FIRST - 0)			
// после изменения выделения узла
#define TLN_SELCHANGED				(TLN_FIRST - 1)			
//тоже самое, что и TVN_GETDISPINFO
#define TLN_GETDISPINFO				(TLN_FIRST - 2)
//тоже самое, что и TVN_SETDISPINFO
#define TLN_SETDISPINFO				(TLN_FIRST - 3)
//тоже самое, что и TVN_ITEMEXPANDING		expanding an item
#define TLN_ITEMEXPANDING			(TLN_FIRST - 4)
//тоже самое, что и TVN_ITEMEXPANDED		an item was expanded
#define TLN_ITEMEXPANDED			(TLN_FIRST - 5)
//тоже самое, что и TVN_BEGINDRAG
#define TLN_BEGINDRAG				(TLN_FIRST - 6)
//тоже самое, что и TVN_BEGINRDRAG
#define TLN_BEGINRDRAG				(TLN_FIRST - 7)
//тоже самое, что и TVN_DELETEITEM			// delete an item
#define TLN_DELETEITEM				(TLN_FIRST - 8)
// entry a build in control
//если верунли 0, значит происходит автоматическая настройка контрола
//если не 0 - значит настройку выполнил пользователь
#define TLN_BEGINCONTROL			(TLN_FIRST - 9)
// leave a build in control
#define TLN_ENDCONTROL				(TLN_FIRST - 10)
//тоже самое, что и TVN_BEGINLABELEDIT
#define TLN_BEGINLABELEDIT			(TLN_FIRST - 11)
//тоже самое, что и TVN_ENDLABELEDIT
#define TLN_ENDLABELEDIT			(TLN_FIRST - 12)
//! Запрашивает тип контрола, которым будем редактировать элемент
#define TLN_REQUESTCTRLTYPE			(TLN_FIRST - 13)

//! Удаляем все узлы. Приходит, если контрол работает как список
#define TLN_DELETEALLITEMS			(TLN_FIRST - 14)

//Сообщаем, что нажали кнопку
#define TLN_KEYDOWN					(TLN_FIRST - 15)

#define	TLN_SUBITEMUPDATED			(TLN_FIRST-20)			// while a column of an item updating
#define TLN_ITEMUPDATED				(TLN_FIRST-21)			// after a column of an item updated
#define TLN_ITEMFINISHED			(TLN_FIRST-22)			// after all columns of an item updated

#define TLN_DRAGENTER				(TLN_FIRST-23)			// drag enter the control
#define TLN_DRAGLEAVE				(TLN_FIRST-24)			// drag leave the control
#define TLN_DRAGOVER				(TLN_FIRST-25)			// drag over the control
#define TLN_DROP					(TLN_FIRST-26)			// drop on the control

#define TLN_ITEMCHECK				(TLN_FIRST-27)			// check or uncheck an item
#define TLN_ITEMLOCK				(TLN_FIRST-28)			// lock or unlock an item
#define TLN_ITEMIMAGE				(TLN_FIRST-29)			// image or unimage an item

// TreeListItem Expand Consts
#define TLE_EXPAND					0						// expand the item
#define TLE_COLLAPSE				1						// collapse the item
#define TLE_TOGGLE					2						// toggle between expand & collapse
#define TLE_COLLAPSERESET			3						// collapse the item then reset it

// TreeListCtrl Consts
#define TLL_HEIGHT					16						// default height of the item
#define TLL_WIDTH					16						// default width of the item
#define TLL_BORDER					 2						// border width of the item
#define TLL_BUTTON					 4						// button width (half)

#define DROP_NONE					0						// drop on nothing
#define DROP_SELF					1						// drop on drag item or it's child
#define DROP_ITEM					2						// drop on item
#define DROP_ROOT					3						// drop on root


// Draw Consts
#define DEFAULT_HRGN				(HRGN)0x0000			// default hrgn

class CTreeListCtrl;
class CTreeListItem;
class CTreeListHeaderCtrl;
class CTreeListTipCtrl;
class CTreeListStaticCtrl;
class CTreeListEditCtrl;
class CTreeListComboCtrl;
class CTLHDragWnd;
class EditItemCtrlBase;

// Struct for Notify
typedef struct _NMTREELIST
{
    NMHDR			hdr;									// default struct
	CTreeListItem*	pItem;									// item pointer
	int				iCol;									// colimn index
} NMTREELIST, FAR* LPNMTREELIST;

typedef struct _NMTREELISTDRAG
{
	NMHDR			hdr;									// default struct
	CTreeListCtrl*	pSource;								// source control
} NMTREELISTDRAG, FAR*LPNMTREELISTDRAG;

typedef struct _NMTREELISTDROP
{
	NMHDR			hdr;									// default struct
	CTreeListCtrl*	pSource;								// source control
	CTreeListItem*	pItem;									// item pointer
} NMTREELISTDROP, FAR* LPNMTREELISTDROP;

typedef struct _NMTREELISTKEYDOWN {
	NMHDR hdr;
	UINT wVKey;
	UINT nRepCnt;
	UINT flags;
} NMTREELISTKEYDOWN, *LPNMTREELISTKEYDOWN;

typedef struct _NMTREELISTCTRL {
	NMHDR hdr;
	CTreeListItem	*pItem;
	int				iCol;
	CWnd			*pEditControl;//контрол, которым производится редактирование			
}NMTREELISTCTRL,*LPNMTREELISTCTRL;
typedef BOOL (WINAPI *lpfnUpdateLayeredWindow)(	HWND hWnd, HDC hdcDst, POINT *pptDst, 
												SIZE *psize,HDC hdcSrc, POINT *pptSrc, 
												COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags );

typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)( HWND hwnd, COLORREF crKey, BYTE xAlpha, DWORD dwFlags );

#define DRAG_DELAY INT_MAX

bool TestForDrag(CWnd* pWnd, CRect const& rect, UINT nDragDelay);

class AFX_EXT_CLASS CTreeListCtrl : public CWnd
{
	DECLARE_DYNCREATE(CTreeListCtrl)

	friend class CTreeListHeaderCtrl;
	friend class CTreeListTipCtrl;
	friend class CTreeListStaticCtrl;
	friend class CTreeListEditCtrl;
	friend class CTreeListComboCtrl;
	friend class CTLHDragWnd;
	friend class CTLCDragWnd;
	friend class CTLCDropWnd;

//	static UINT const DRAG_DELAY;//через какое время drag and drop начнется 
					//даже без движения

// Construction
public:
	CTreeListCtrl();
	virtual ~CTreeListCtrl();
	bool Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );

protected:
	bool Create();
	// back ground colors of normal status
	COLORREF getBkColorNonClientArea() const;		// background color ( none client area )
	COLORREF getBkColorSingleColorTree() const;		// background color ( single color tree )
	COLORREF getBkColorSingleColor() const;			// background color ( single color )
	COLORREF getBkColorDoubleColor() const;			// background color ( double color )
	// back ground colors of disable status
	COLORREF getBkColorDisabledNonClientArea() const;	// background color ( none client area )
	COLORREF getBkColorDisabledSingleColorTree() const;	// background color ( single color tree )
	COLORREF getBkColorDisabledSingleColor() const;			// background color ( single color )
	COLORREF getBkColorDisabledDoubleColor() const;			// background color ( double color )

	//! back ground colors of selected item
	//! background color of selected
	COLORREF getBkColorSelectedItem() const; 
	//! background color of current selected row
	COLORREF getBkColorSelectedRow() const;	
	//! background color of current selected column
	COLORREF getBkColorSelectedColumn() const;
	//! background color of no focus selected row
	COLORREF getBkColorSelectedItemNoFocus() const;

	// text colors
	//! color of selected text
	COLORREF getSelectedTextColor() const;
	//! color of current selected row text
	COLORREF getSelectedRowTextColor() const;
	//! color of current selected column text
	COLORREF getSelectedColumnTextColor() const;
	//! color of no focus selected text
	COLORREF getSelectedTextNoFocusColor() const;	

	// normal text color
	//! color of text
	COLORREF getNormalTextColor() const;
	//! color of hover text
	COLORREF getHoverTextColor() const;				

	// grid colors
	// color of grid
	COLORREF getGridColor() const;					
	// color of tree line
	COLORREF getTreeLineColor() const;				
// Members
protected:
	// back ground colors of normal status
	COLORREF			m_crBkColorNonClientArea;	// background color ( none client area )
	COLORREF			m_crBkColorSingleColorTree;	// background color ( single color tree )
	COLORREF			m_crBkColorSingleColor;		// background color ( single color )
	COLORREF			m_crBkColorDoubleColor;				// background color ( double color )

	// back ground colors of disable status
	COLORREF			m_crBkColorDisabledNonClientArea;				// background color ( none client area )
	COLORREF			m_crBkColorDisabledSingleColorTree;			// background color ( single color tree )
	COLORREF			m_crBkColorDisabledSingleColor;			// background color ( single color )
	COLORREF			m_crBkColorDisabledDoubleColor;			// background color ( double color )

	// back ground colors of selected item
	COLORREF			m_crBkColorSeletedItem;		// background color of selected
	COLORREF			m_crBkColorSelectedRow;			// background color of current selected row
	COLORREF			m_crBkColorSelectedColumn;		// background color of current selected column
	COLORREF			m_crBkColorSelectedItemNoFocus;		// background color of no focus selected row

	// text colors
	COLORREF			m_SelectedTextColor;			// color of selected text
	COLORREF			m_SelectedRowTextColor;		// color of current selected row text
	COLORREF			m_SelectedColumnTextColor;		// color of current selected column text
	COLORREF			m_SelectedTextNoFocusColor;	// color of no focus selected text

	// normal text color
	COLORREF			m_NormalTextColor;					// color of text
	COLORREF			m_HoverTextColor;				// color of hover text

	// grid colors
	COLORREF			m_GridColor;					// color of grid
	COLORREF			m_TreeLineColor;				// color of tree line

	// tree list style
	DWORD				m_dwStyle;					// style of tree list control

	// font & palette
	CFont		m_Font;		// font of control
	CPalette	m_Palette;	// palette of control

	// cursor
	HCURSOR		m_hButton;	// hcursor of button
	HCURSOR		m_hCheck;	// hcursor of checkbox
	HCURSOR		m_hLock;	// hcursor of lockbox

	HCURSOR		m_hDrop;	// hcursor of drop
	HCURSOR		m_hStop;	// hcursor of stop
	HCURSOR		m_hArrow;	// hcursor of arrow

	// draw rects
	CRect	m_rcClient;		// rect of control client ( include header )
	CRect	m_rcHeader;		// rect of control header 
	CRect	m_rcTreeList;	// rect of treelist

	CRect	m_rcFocus;		// rect of focus item

protected:
	// build in controls
	CTreeListStaticCtrl		m_wndStatic;			// Build in Static Control
	CTreeListEditCtrl		m_wndEdit;				// Build in Edit Control
	CTreeListComboCtrl		m_wndCombo;				// Build in ComboBox Control
	CWnd*					m_ptrCustomEditControl;

	// objects of control
	CTreeListHeaderCtrl		m_wndHeader;			// TreeList Header Control
	CTreeListTipCtrl		m_wndTip;				// TreeList Tip Control

private:
	// image of control
	CBitmap				m_bmpBkBitmap;				// Bitmap of Background ( Default )
	CBitmap				m_bmpBkDisable;				// Bitmap of Disabled Background ( Default )
	CImageList			m_imgCheck;					// CheckBox Image List ( Default )
	CImageList			m_imgLock;					// LockBox Image List ( Default )
	CImageList			m_imgTree;					// Tree Image List ( Default )

	CImageList*			m_pImageList;				// ImageList ( Delault equ &m_imgTree )

	DWORD				m_dwState;					// State of the Control

private:
	// column information of control
	CPtrArray			m_arColumns;				// All Columns
	CArray<int, int>	m_arShows;					// All Show Columns
	CArray<int, int>	m_arSorts;					// All Sort Columns

	// row information of control
	CPtrArray			m_arSelects;				// Selected Items
	int					m_iSelCol;					// Selected Column
	int					m_nItemHeight;				// Height of 
	CTreeListItem*		m_pRootItem;				// Root Item
	CTreeListItem*		m_pFocusItem;				// Focus item
	CTreeListItem*		m_pShiftSelectionBase;		//! узел-основа для выделения с шифтом

	int					m_iModifyCol;				// Modify col
	CTreeListItem*		m_pModifyItem;				// Modify item
	CTreeListItem*		m_pUpdateItem;				// Update item
	DWORD				m_dwRequestedCtrlType;

	// drag drop
	CTLCDragWnd*		m_pDragWnd;					// Drag Window
	CTLCDropWnd*		m_pDropWnd;					// Drop Window
	CPoint				m_ptBegin;					// Point of Begin Drag
	CPoint				m_ptDelta;					// Delta of Drag

	CTreeListItem*		m_pTargetItem;				// Drop Target Item
	CTreeListCtrl*		m_pTargetWnd;				// Target Window
	CTreeListItem*		m_pHoverItem;				// Hover Item
	int					m_iHoverItem;				// Hover Item

	BOOL				m_bInLayout;
	BOOL				m_bAllowRedraw;

	ICustomDrawNotifyListener*	m_ptrCustomDrawNotifyListener;
	mutable CONTROL_CUSTOM_DRAW_INFO*	m_ptrControlCustomDrawInfo;
	mutable ITEM_CUSTOM_DRAW_INFO*		m_ptrItemCustomDrawInfo;
	mutable SUBITEM_CUSTOM_DRAW_INFO*   m_ptrSubitemCustomDrawInfo;
// Attributes
public:
	//***************************types***************************************
	enum eSelectAction {SI_SELECT, SI_DESELECT};
	//! Различные константы
	enum {
		TLCC_NONOTIFY,
		TLCC_NOTIFY
	};

	// ***************************** CTreeCtrl ******************************
	int				GetCount() const;

	CImageList*		GetImageList();							// retrieve image list of global treelist
	void			SetImageList( CImageList* pImageList );	// set image list of global treelist

	CTreeListItem* GetNextItem( CTreeListItem const* pItem, UINT nCode ) const;

	bool		ItemHasChildren( CTreeListItem const* pItem ) const;

	CTreeListItem* GetChildItem( CTreeListItem const* pItem ) const;
	CTreeListItem* GetNextSiblingItem( CTreeListItem const* pItem ) const;
	CTreeListItem* GetPrevSiblingItem( CTreeListItem const* pItem ) const;
	CTreeListItem* GetParentItem( CTreeListItem const* pItem ) const;

	CTreeListItem* GetFirstVisibleItem() const;
	CTreeListItem* GetNextVisibleItem( CTreeListItem const* pItem ) const;
	CTreeListItem* GetPrevVisibleItem( CTreeListItem const* pItem ) const;

	CTreeListItem* GetSelectedItem() const;
	CTreeListItem* GetNextSelectedItem( CTreeListItem const* pItem ) const;

	CTreeListItem* GetRootItem() const;

	DWORD	GetItemState( CTreeListItem* pItem, DWORD nStateMask = 0xFFFFFFFF );
	void	SetItemState( CTreeListItem* pItem, DWORD dwAddState, DWORD dwRemoveState = 0 );

	void	GetItemImage( CTreeListItem* pItem, int& nImage, int& nExpandImage, int& nSelectedImage, int& nExpandSelectedImage );
	void	SetItemImage( CTreeListItem* pItem, int nImage, int nExpandImage, int nSelectedImage, int nExpandSelectedImage );

	LPCTSTR	GetItemText( CTreeListItem const* pItem, int nSubItem = 0 ) const;
	bool	SetItemText( CTreeListItem* pItem, LPCTSTR lpszText, int nSubItem = 0 );
	bool	SetItemText( CTreeListItem* pItem, int nSubItem, LPCTSTR lpszText);

	DWORD_PTR GetItemData( CTreeListItem const* pItem ) const;
	void	  SetItemData( CTreeListItem* pItem, DWORD_PTR dwData );

	int		GetVisibleCount() const;

	int		SetItemHeight( int cyHeight = -1 );		// set height of row ( automatic )
	int		GetItemHeight() const;						// retrieve height of row

	COLORREF GetBkColor( int nColor = 0 );
	COLORREF SetBkColor( COLORREF clr, int nColor = 0 );

	COLORREF GetTextColor();
	COLORREF SetTextColor( COLORREF clr );
	
	int		GetCheck( CTreeListItem* pItem );
	void	SetCheck( CTreeListItem* pItem, bool bCheck = TRUE );

	int		GetLock( CTreeListItem* pItem );
	void	SetLock( CTreeListItem* pItem, bool bLock = TRUE );

	bool	GetItemRect( CTreeListItem const* pItem, LPRECT lpRect ) const;
	bool	GetItemRect( CTreeListItem const* pItem, 
						int iSubItem, 
						LPRECT lpRect, 
						bool bTextOnly ) const;
	
	bool	EnsureVisible( CTreeListItem* pItem, int iSubItem );

	// ***************************** CListCtrl ******************************
	int		GetItemCount();
	int		GetNextItem( int nItem, int nFlags );

	POSITION	   GetFirstSelectedItemPosition() const;
	CTreeListItem* GetNextSelectedItem( POSITION& pos ) const;

	int GetStringWidth( LPCTSTR lpsz );

	DWORD	SetColumnFormat( int nCol, DWORD dwAdd, DWORD dwRemove );
	DWORD	GetColumnFormat( int nCol, DWORD dwMask = 0xFFFFFFFF );

	DWORD	SetColumnModify( int nCol, DWORD dwModify );
	DWORD	GetColumnModify( int nCol );

	int		SetColumnWidth( int nCol, int nWidth, int nMin = 0, int nMax = 0 );
	int		GetColumnWidth( int nCol );

	int		SetColumnImage( int nCol, int iImage );
	int		GetColumnImage( int nCol );

	bool	SetColumnText( int nCol, LPCTSTR lpszText );
	LPCTSTR GetColumnText( int nCol );

	bool	SetColumnDefaultText( int nCol, LPCTSTR lpszText );
	LPCTSTR GetColumnDefaultText( int nCol );

	DWORD	GetItemState( int nItem, DWORD nStateMask = 0xFFFFFFFF );
	void	SetItemState( int nItem, DWORD dwAddState, DWORD dwRemoveState = 0 );

	void	GetItemImage( int nItem, int& nImage, int& nExpandImage, int& nSelectedImage, int& nExpandSelectedImage );
	void	SetItemImage( int nItem, int nImage, int nExpandImage, int nSelectedImage, int nExpandSelectedImage );

	LPCTSTR	GetItemText( int nItem, int nSubItem = 0 ) const;
	bool	SetItemText( int nItem, int nSubItem, LPCTSTR lpszText );

	DWORD_PTR	GetItemData( int nItem ) const;
	void		SetItemData( int nItem, DWORD_PTR dwData );

	BOOL	GetViewRect( LPRECT lpRect );
	int		GetTopIndex();
	int		GetCountPerPage();
	bool	GetOrigin( LPPOINT lpPoint );

	UINT	GetSelectedCount();
	bool	SetColumnOrderArray( int iCount, LPINT piArray );
	bool	GetColumnOrderArray( LPINT piArray, int iCount = -1 );

	CTreeListHeaderCtrl* GetHeaderCtrl();

	int GetSelectionMark();
	int SetSelectionMark( int iIndex );

	ICustomDrawNotifyListener* SetCustomDrawNotifyListener(
		ICustomDrawNotifyListener* pcdnl);
	ICustomDrawNotifyListener* GetCustomDrawNotifyListener() const;
	// *************************** CTreeListCtrl ****************************
	void	SetFont(CFont*, BOOL = TRUE);

	void	SetFont();
	CFont*	GetFont();

	DWORD	SetStyle( DWORD dwStyle );
	DWORD	GetStyle() const;

	DWORD	GetState( DWORD nStateMask = 0xFFFFFFFF ) const;
	void	SetState( DWORD dwAddState, DWORD dwRemoveState = 0 );

	int		GetSelectColumn();

	int		GetColumnCount();

	void	ExchangeItem( CTreeListItem* pItemA, CTreeListItem* pItemB );
	void	MoveAfterItem( CTreeListItem* pItem, CTreeListItem* pAfter = NULL );

	bool	DragEnter( CTreeListCtrl* pTreeListCtrl );
	bool	DragLeave( CTreeListCtrl* pTreeListCtrl );
	int		DragOver( CTreeListCtrl* pTreeListCtrl, CPoint point, CTreeListItem** pp = NULL );
	int		Drop( CTreeListCtrl* pTreeListCtrl, CPoint point );

	void	AllowRedraw( BOOL bAllow = TRUE );

protected:
	int		GetWidth() const;								// retrieve display high of control
	int		GetHeight() const;							// retrieve display width of control

	int		GetColumns() const;							// retrieve columns

	int		GetVisibleCount( CTreeListItem const* pParent ) const;

	bool	GetItemRectTree( CTreeListItem const* pItem, LPRECT lpRect ) const;
	bool	GetItemRectMain( CTreeListItem const* pItem, LPRECT lpRect ) const;
	bool	GetItemBkgrndRect(CTreeListItem const* pItem, 
							CRect const& rcItem, 
							LPRECT lpRect) const;	
protected:
	CTreeListItem*	GetFocusedItem() const;
	void			SetFocusedItem(CTreeListItem* pFocusItem);
	void			SetModifiedItem(CTreeListItem* pItem);
	CTreeListItem*	GetModifiedItem() const;
	//!\brief Назначает узел, от которого будет идти выделение 
	//! при выдеделении с шифтом
	void			SetShiftSelectionBase(CTreeListItem* pitem);
	//! Возвращает узел, от которого будет идти выделение 
	//! при выдеделении с шифтом
	CTreeListItem*  GetShiftSelectionBase() const;
	//возвращает номер в списке выбранных
	INT_PTR			FindInSelected(CTreeListItem const* pItem) const;
	bool			IsColumnVisible(int iCol) const;
	INT_PTR			FindInShowedColumns(int iCol) const;
private:
	void	SetControlCustomDrawInfo(CONTROL_CUSTOM_DRAW_INFO* pccdi) const;
	CONTROL_CUSTOM_DRAW_INFO const* GetControlCustomDrawInfo() const;
	void	SetItemCustomDrawInfo(ITEM_CUSTOM_DRAW_INFO* picdi) const;
	ITEM_CUSTOM_DRAW_INFO const* GetItemCustomDrawInfo() const;

	void SetSubitemCustomDrawInfo(SUBITEM_CUSTOM_DRAW_INFO* pscdi) const;
	SUBITEM_CUSTOM_DRAW_INFO const* GetSubitemCustomDrawInfo() const;

	void	StatChildAdd( CTreeListItem* pItem, int nChild, int nVisibleChild );
	void	StatChildDel( CTreeListItem* pItem, int nChild, int nVisibleChild );
	void	StatExpand( CTreeListItem* pItem );
	void	StatCollapse( CTreeListItem* pItem );
	void	SetItemParentStatus( CTreeListItem* pItem, DWORD dwMask );
	void	SetItemChildStatus( CTreeListItem* pItem, DWORD dwAdd, DWORD dwRemove = 0 );

	void	SetItemCheck( CTreeListItem* pItem, bool bAutoCheck = TRUE );
	void	SetItemLock( CTreeListItem* pItem, bool bAutoLock = TRUE );

	bool	SelectItemAux(CTreeListItem* pItem, int iSubItem  = 0 , 
		eSelectAction eAction  = SI_SELECT);
	//! пытаемся снять выделение сразу со всех узлов, исключая указанный
	void	DeselectAll(CTreeListItem const* pItem);

	void	ShiftSelection(CTreeListItem* pItem, int iSubItembool,
							bool bClearSelection = true);
	//! Выбор элементов в ответ на нажатие клавши клавиатуры
	void	SelectItemOnKeyPressing(CTreeListItem* pItem, int iSubItem);

	enum eItemLayout{EIL_ABOVE, EIL_BELOW};
	eItemLayout IsItemAbove(CTreeListItem* pBaseItem, CTreeListItem* pTestItem);

	bool	KeyDownHandler(UINT_PTR nChar, UINT_PTR nRepCnt, UINT_PTR nFlags);
// Operations
public:
	bool	InsertColumn( LPCTSTR lpszColumnHeading, DWORD dwFormat = TLF_DEFAULT_LEFT, int nWidth = 0, int iImage = -1, int nMin = 0, int nMax = 0);
	bool	DeleteColumn( int iCol );

	int		InsertItem( int nItem, LPCTSTR lpszItem );
	bool	DeleteItem( int nItem );

	CTreeListItem*	InsertItem( LPCTSTR lpszItem, CTreeListItem* pParent = TLI_ROOT, CTreeListItem* pInsertAfter = TLI_LAST );
	bool			DeleteItem( CTreeListItem* pItem = TLI_ROOT );
	bool			DeleteAllItems();

	bool	Expand( CTreeListItem* pItem, int nCode );
	bool	Select( CTreeListItem* pItem, int nCode );

	CTreeListItem*	HitTest( CPoint pt, int* pFlag = NULL, int* pSubItem = NULL, CRect* prcText = NULL );

	void	SelectItem( CTreeListItem* pItem, int iSubItem = 0, 
		eSelectAction eAction = SI_SELECT, bool bClearLastSelected = TRUE );
	void	ActiveItem( CTreeListItem* pItem, int iSubItem = 0 );

	void	SetTargetItem( CTreeListItem* pItem = NULL );
	void	SetHoverItem( CTreeListItem* pItem = NULL );
	CTreeListItem* GetDropTargetItem() const;
protected:
	CTreeListItem*	InsertItemNew	( LPCTSTR lpszItem, CTreeListItem* pParent );
	CTreeListItem*	InsertItemFirst	( LPCTSTR lpszItem, CTreeListItem* pParent );
	CTreeListItem*	InsertItemNext	( LPCTSTR lpszItem, CTreeListItem* pParent, CTreeListItem* pInsertAfter );
	CTreeListItem*	GetLastChildItem( CTreeListItem* pParent = TLI_ROOT );
	CTreeListItem*	GetSortChildItem( CTreeListItem* pParent = TLI_ROOT );
	CTreeListItem*	GetValidChildItem( CTreeListItem* pParent, CTreeListItem* pChild);

	bool			Expand( CTreeListItem* pItem, int nCode, int iSubItem );

	CTreeListItem*	HitTest( CPoint pt, int* pFlag, int* pSubItem, CRect* prcText, CRect rcItem, CTreeListItem* pItem );

private:
	CTreeListItem*	InsertItemX( CTreeListItem* pParent, CTreeListItem* pNewItem );
	bool			DeleteItemX( CTreeListItem* pItem, int flag );
	bool			DeleteItemFast( CTreeListItem* pItem, int flag);
	bool			DeleteAllListItems();

	CTreeListItem*	GetFirstShowItem( int nShowItem ) const;
	CTreeListItem*	GetNextShowItem( CTreeListItem const* pItem, bool bChild = TRUE ) const;
	CTreeListItem*	GetNextVisibleItem( CTreeListItem const* pItem, bool bChild ) const;
	CTreeListItem*	GetPrevVisibleItem( CTreeListItem const* pItem, bool bChild ) const;
	CTreeListItem*	GetLastVisibleItem( CTreeListItem const* pItem ) const;

	CTreeListItem*	HitTestTree( CPoint pt, int* pFlag, CRect* prcText, CRect rcColumn, CTreeListItem* pItem );
	CTreeListItem*	HitTestMain( CPoint pt, int* pFlag, CRect* prcText, CRect rcColumn, CTreeListItem* pItem );
	CTreeListItem*	HitTestList( CPoint pt, int* pFlag, CRect* prcText, CRect rcColumn, CTreeListItem* pItem );

	CTreeListItem*  GetTreeListItem( int nItem ) const;
	int				GetTreeListItem( CTreeListItem* pItem ) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	bool	SetCtrlFocus( CWnd* pWnd, bool bFocus = TRUE );

	bool	BeginModify( CTreeListItem* pItem, int iCol );
	bool	UpdateModify();
	bool	FinishModify();
	bool	CancelModify();

	CWnd*	GetControl( CTreeListItem* pItem, int iCol );

protected:
	void	FillNMHDR(NMHDR& hdr, DWORD dwMessage);
	LRESULT	Notify( DWORD dwMessage, CTreeListItem* pItem, int iCol );
	LRESULT NotifyDrop( DWORD dwMessage, CTreeListCtrl* pSource, CTreeListItem* pItem );
	LRESULT NotifyControl(DWORD dwMessage, CTreeListItem* pItem, int iCol, CWnd* pCtrl);
	LRESULT NotifyKeydown(UINT nChar, UINT nRepCount, UINT nFlags);

	void	Layout();
	void	DrawCtrl( CDC* pDC );
	void	DrawBkgnd( CDC* pDC, CRect const& rcClip );
	void	DrawBkgndBmp( CDC* pDC, RECT const& rcClip, HBITMAP hBkgnd );
	void	DrawItems( CDC* pDC, CRect rcClip );
	void	DrawItem( CDC* pDC, CRect rcItem, CTreeListItem* pItem );
	void	DrawGrid( CDC* pDC, CRect rcItem );
	void	DrawItemBkgnd( CDC* pDC, CRect const& rcItem, CTreeListItem* pItem );
	void	DrawItemExclude( CDC* pDC, CRect rcItem, CTreeListItem* pItem );
	void	DrawItemTree( CDC* pDC, CRect rcColumn, CTreeListItem* pItem, int iCol );
	int		DrawCheckBox( CDC* pDC, CRect const& rcColumn, CRect const& rcGraph, 
			CTreeListItem const* pItem, int iCol, int nPerfix );
	int		DrawLockBox( CDC* pDC, CRect const& rcColumn, CRect const& rcGraph, 
			CTreeListItem const* pItem, int iCol, int nPerfix );
	int		DrawItemImage( CDC* pDC, CRect const& rcColumn, CRect const& rcGraph, 
			CTreeListItem const* pItem, int iCol, int nPerfix );
	void	DrawItemMain( CDC* pDC, CRect rcColumn, CTreeListItem* pItem, int iCol );
	void	DrawItemList( CDC* pDC, CRect rcColumn, CTreeListItem* pItem, int iCol );
	void	DrawItemTreeLine( CDC* pDC, CRect rcColumn, CRect rcTree, DWORD dwFormat );
	void	DrawItemTreeButton( CDC* pDC, CRect rcColumn, CRect rcTree, DWORD dwFormat );
	void	DrawItemTreeText( CDC* pDC, CRect rcBkgnd, CTreeListItem* pItem, int iCol );
	void	DrawItemListText( CDC* pDC, CRect rcBkgnd, CTreeListItem* pItem, int iCol );
	void	DrawItemText( CDC* pDC, CRect const& rcBkgnd, 
						CRect rcText, CTreeListItem* pItem, int iCol );

	void	GetCellBkColor(int iRow, COLORREF* color) const;

	void	PrepareInfoForControlPaintCycle(CONTROL_CUSTOM_DRAW_INFO* pcdi,
											CDC* pDC, CRect const& rect);
	void	PrepareInfoForItemPaintCycle(ITEM_CUSTOM_DRAW_INFO* pcdi,
										CDC* pDC, 
										int iRow, 
										CTreeListItem const* pItem,
										CRect const& rcItem);
	void PrepareInfoForSubItemPaintCycle(SUBITEM_CUSTOM_DRAW_INFO* pcdi,
										CDC* pDC, 
										CTreeListItem const* pItem,
										int iSubItem);
	void	SetAllScroll();

	bool	PreTranslateKeyDownMessage( MSG* pMsg );
	bool	PreTranslateKeyDownMessage2( MSG* pMsg );
	
	int		GetPrevModifyCol();
	int		GetNextModifyCol();

	bool	UpdateModifyColumn();
	bool	UpdateModifyColumns();

	bool	BeginControl( CTreeListItem* pItem, int iCol );
	bool	CancelControl();
	bool	RestoreControl( CTreeListItem* pItem, int iCol );

	bool	BeginStatic( CTreeListItem* pItem, int iCol, CRect rcText );
	bool	CancelStatic();
	bool	RestoreStatic( CTreeListItem* pItem, int iCol, CRect rcText );

	bool	BeginEdit( CTreeListItem* pItem, int iCol, CRect rcText );
	bool	CancelEdit();
	bool	RestoreEdit( CTreeListItem* pItem, int iCol, CRect rcText );

	bool	BeginCombo( CTreeListItem* pItem, int iCol, CRect rcText );
	bool	CancelCombo();
	bool	RestoreCombo( CTreeListItem* pItem, int iCol, CRect rcText);

	void	ShowCtrl(CWnd* pCtrl);
	void	RestoreCtrl( CWnd* pCtrl, CTreeListItem* pItem, 
						int iCol, CRect const& rcText);
	bool	CancelCtrl(CWnd* pCtrl);
	void	EndModify();
	
	bool	BeginDraging( CPoint point );
	bool	EndDraging( CPoint point );
	bool	DoDraging( CPoint point );
	bool	EndDraging();

private:
	void	SetHorzScroll( CRect* pPreClient );
	void	SetVertScroll( CRect* pPreClient );

	bool	IsInSelectsTree( CTreeListItem* pItem );

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCancelMode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTCTRL_H__55B6E4E9_1E57_4FEE_AA05_77C052ACE9B5__INCLUDED_)
