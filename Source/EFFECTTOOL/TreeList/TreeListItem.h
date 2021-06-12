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

const unsigned TLIS_SHOWCHECKBOX	=	0x00001000;		// show check box
const unsigned TLIS_SHOWLOCKBOX		=	0x00002000;		// show lock box
const unsigned TLIS_SHOWTREEIMAGE	=	0x00004000;		// show image

const unsigned TLIS_CHECKED			=	0x00010000;		// [V]
const unsigned TLIS_CHECKED_NULL	=	0x00020000;		// [ ]

const unsigned TLIS_CHECKEDPART		=	0x00030000;		// [?]
const unsigned TLIS_CHECKEDMASK		=	0x00030000;		// TLI_CHECKED | TLI_CHECKED_NULL

const unsigned TLIS_LOCKED			=	0x00040000;		// [L]
const unsigned TLIS_LOCKED_NULL		=	0x00080000;		// [ ]

const unsigned TLIS_LOCKEDPART		=	0x000C0000;		// [?]
const unsigned TLIS_LOCKEDMASK		=	0x000C0000;		// TLI_LOCKED | TLI_LOCKED_NULL


const unsigned TLIS_STATEIMAGEMASK	=	0x00F00000;


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
class CTLCDragWnd;
class CTreeListItem
{
	friend class CTreeListCtrl;
	friend class CTLCDragWnd;
protected:
	CTreeListItem( int nSubItems );
	virtual ~CTreeListItem();

// Members
protected:
	CTreeListItem*	m_pParent;
	CTreeListItem*	m_pChild;
	CTreeListItem*	m_pPrev;
	CTreeListItem*	m_pNext;
	int				m_nLevel;
	int				m_nChild;
	mutable int		m_nVisibleChild;

	DWORD			m_dwState;
	DWORD_PTR		m_dwData;

	int				m_nImage;
	int				m_nSelectedImage;
	int				m_nExpandImage;
	int				m_nExpandSelectedImage;

	CPtrArray		m_arSubItems;

// Oprations
public:
	DWORD		GetState( DWORD dwStateMask = TLIS_ALLSTATE ) const;
	void		GetImage( int& nImage, int& nExpandImage, 
						int& nSelectedImage, int& nExpandSelectedImage ) const;
	int			GetImage() const;

	LPCTSTR		GetText( int nIndex = 0 ) const;

	DWORD_PTR	GetData() const;

	bool		IsShowTreeImage() const;
	bool		IsShowCheckBox() const;
	bool		IsShowLockBox() const;


	bool		GetSelected() const;
	int			GetCheck() const;
	int			GetLock() const;
	bool		GetFocused() const;
	DWORD		GetStateImage() const;
	bool		GetExpanded() const;

	static DWORD IndexToStateImageMask(DWORD index);
	static DWORD StateImageMaskToIndex(DWORD state);

protected:
	bool		SetText( LPCTSTR lpszItem, int nIndex = 0 );
	void		SetData( DWORD_PTR dwData );
	void		SetImage( int nImage, 
							int nExpandImage, 
							int nSelectedImage, 
							int nExpandSelectedImage );
	void		SetImage( int nImage );

	void		ShowTreeImage( bool bShow );
	void		ShowCheckBox( bool bShow );
	void		ShowLockBox( bool bShow );


	void		Expand();
	void		Collapse();

	void		SetState( DWORD dwAddStatus, DWORD dwRemoveStatus = 0 );

	void		SetSelected( bool bSelected = true );
	void		SetCheck( bool bCheck = true );
	void		SetLock( bool bLock = true );
	void		SetFocused(bool bFocused = true);
	void		SetStateImage(DWORD num);

};

#endif // !defined(AFX_TREELISTITEM_H__CD98CFA2_C685_40FE_8F08_66A005EB3845__INCLUDED_)
