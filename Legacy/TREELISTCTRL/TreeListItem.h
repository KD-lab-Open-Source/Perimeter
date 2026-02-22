// TreeListItem.h: interface for the CTreeListItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREELISTITEM_H__CD98CFA2_C685_40FE_8F08_66A005EB3845__INCLUDED_)
#define AFX_TREELISTITEM_H__CD98CFA2_C685_40FE_8F08_66A005EB3845__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const unsigned TLIS_SELECTED		=	0x00000001;		// selected
const unsigned TLIS_EXPANDED		=	0x00000002;		// expanded
const unsigned TLIS_EXPANDEDONCE	=	0x00000004;		// expanded once
const unsigned TLIS_FOCUSED			=	0x00000008;		//!	обведен рамкой

const unsigned TLIS_SHOWCHECKBOX	=	0x00000010;		// show check box
const unsigned TLIS_SHOWLOCKBOX		=	0x00000020;		// show lock box
const unsigned TLIS_SHOWTREEIMAGE	=	0x00000040;		// show image

//щелкнули мышкой дважды по той же строке и по тому же столбцу
const unsigned TLIS_READYTOEDIT		=	0x00000080;

const unsigned TLIS_SHOWBKCOLOR		=	0x00000100;		// owner background color
const unsigned TLIS_SHOWTEXTCOLOR	=	0x00000200;		// owner text color

const unsigned TLIS_CHECKED			=	0x00010000;		// [V]
const unsigned TLIS_CHECKED_NULL	=	0x00020000;		// [ ]

const unsigned TLIS_CHECKEDPART		=	0x00030000;		// [?]
const unsigned TLIS_CHECKEDMASK		=	0x00030000;		// TLI_CHECKED | TLI_CHECKED_NULL

const unsigned TLIS_LOCKED			=	0x01000000;		// [L]
const unsigned TLIS_LOCKED_NULL		=	0x02000000;		// [ ]

const unsigned TLIS_LOCKEDPART		=	0x03000000;		// [?]
const unsigned TLIS_LOCKEDMASK		=	0x03000000;		// TLI_LOCKED | TLI_LOCKED_NULL

const unsigned TLIS_ALLSTATE		=	0xFFFFFFFF;

#if _MSC_VER < 1300
#ifndef DWORD_PTR
#define DWORD_PTR DWORD 
#endif //DWORD_PTR
#ifndef ULONG_PTR
#define ULONG_PTR ULONG 
#endif //ULONG_PTR
#endif //_MSC_VER < 1300

class CTreeListCtrl;
class CTreeListItem
{
	friend class CTreeListCtrl;
public:
	CTreeListItem( int nSubItems );
	virtual ~CTreeListItem();

// Members
public:
	CTreeListItem*	m_pParent;
	CTreeListItem*	m_pChild;
	CTreeListItem*	m_pPrev;
	CTreeListItem*	m_pNext;
	int				m_nLevel;
	int				m_nChild;
	int				m_nVisibleChild;

	DWORD			m_dwState;
	DWORD_PTR		m_dwData;

	int				m_nImage;
	int				m_nSelectedImage;
	int				m_nExpandImage;
	int				m_nExpandSelectedImage;

//	CImageList*		m_pImageList;

	CPtrArray		m_arSubItems;

// Oprations
public:
	void		Expand();
	void		Collapse();

	DWORD		GetState( DWORD dwStateMask = TLIS_ALLSTATE ) const;
	void		SetState( DWORD dwAddStatus, DWORD dwRemoveStatus = 0 );

	void		GetImage( int& nImage, int& nExpandImage, 
						int& nSelectedImage, int& nExpandSelectedImage ) const;
	void		SetImage( int nImage, int nExpandImage, int nSelectedImage, int nExpandSelectedImage );

	LPCTSTR		GetText( int nIndex = 0 ) const;
	bool		SetText( LPCTSTR lpszItem, int nIndex = 0 );

	DWORD_PTR	GetData() const;
	void		SetData( DWORD_PTR dwData );

	bool		IsShowTreeImage() const;
	void		ShowTreeImage( bool bShow );

	bool		IsShowCheckBox() const;
	void		ShowCheckBox( bool bShow );

	bool		IsShowLockBox() const;
	void		ShowLockBox( bool bShow );

	bool		GetSelected() const;
	void		SetSelected( bool bSelected = true );

	int			GetCheck() const;
	void		SetCheck( bool bCheck = true );

	int			GetLock() const;
	void		SetLock( bool bLock = true );

	bool		GetFocused() const;
	void		SetFocused(bool bFocused = true);
protected:
	bool		IsReadyToEdit() const;
	void		SetReadyToEdit(bool bSet = true);
};

#endif // !defined(AFX_TREELISTITEM_H__CD98CFA2_C685_40FE_8F08_66A005EB3845__INCLUDED_)
