// TreeListColumnInfo.h: interface for the CTreeListColumnInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREELISTCOLUMNINFO_H__A20B2FC5_7522_4EFC_915D_9B4280E1DCC5__INCLUDED_)
#define AFX_TREELISTCOLUMNINFO_H__A20B2FC5_7522_4EFC_915D_9B4280E1DCC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TLF_ALIGN_LEFT			0x00000001		// Text of Caption Align Left
#define TLF_ALIGN_RIGHT			0x00000002		// Text of Caption Align Right
#define	TLF_ALIGN_CENTER		0x00000004		// Text of Caption Align Center
#define TLF_ALIGN_SPACE			0x00000008		// Text of Caption Include Space

#define TLF_VALIGN_TOP			0x00000010		// Text of Caption Align Top
#define TLF_VALIGN_BOTTOM		0x00000020		// Text of Caption Align Bottom
#define TLF_VALIGN_CENTER		0x00000040		// Text of Caption Align Center

#define TLF_TEXTALIGN_LEFT		0x00000100		// Text of Column Align Right
#define TLF_TEXTALIGN_RIGHT		0x00000200		// Text of Column Align Right
#define TLF_TEXTALIGN_CENTER	0x00000400		// Text of Column Align Right
#define TLF_TEXTALIGN_SPACE		0x00000800		// Text of Column Include Space

#define TLF_TEXTVALIGN_TOP		0x00001000		// Text of Column Align Top
#define TLF_TEXTVALIGN_BOTTOM	0x00002000		// Text of Column Align Bottom
#define TLF_TEXTVALIGN_CENTER	0x00004000		// Text of Column Align Center

#define TLF_IMAGEALIGN_LEFT		0x00010000		// Image of Caption Align Left
#define TLF_IMAGEALIGN_RIGHT	0x00020000		// Image of Caption Align Right
#define TLF_IMAGEALIGN_CENTER	0x00040000		// Image of Caption Align Center

#define TLF_CAPTION_TEXT		0x00100000		// Show Caption Text
#define TLF_CAPTION_SORT		0x00200000		// Show Caption Sort Icon
#define TLF_CAPTION_IMAGE		0x00400000		// Show Caption Image

#define TLF_SORT_ASC			0x01000000		// ASC Sorting
#define TLF_SORT_DESC			0x02000000		// DESC Sorting

#define TLF_SORT_MASK			0x03000000		// ( TLF_SORT_ASC | TLF_SORT_DESC )

#define TLF_HIDDEN				0x10000000		// Hidden Column
#define TLF_NOSIZE				0x20000000		// Fixed Width Column
#define TLF_NODRAG				0x30000000		// Support Drag
#define TLF_NODROP				0x40000000		// Support Drop

#define TLF_DEFAULT_LEFT	(TLF_ALIGN_CENTER | TLF_VALIGN_CENTER | TLF_TEXTALIGN_LEFT   | TLF_TEXTVALIGN_CENTER | TLF_CAPTION_TEXT)
												// Default Align Left
#define TLF_DEFAULT_RIGHT	(TLF_ALIGN_CENTER | TLF_VALIGN_CENTER | TLF_TEXTALIGN_RIGHT  | TLF_TEXTVALIGN_CENTER | TLF_CAPTION_TEXT)
												// Default Align Reght
#define TLF_DEFAULT_CENTER	(TLF_ALIGN_CENTER | TLF_VALIGN_CENTER | TLF_TEXTALIGN_CENTER | TLF_TEXTVALIGN_CENTER | TLF_CAPTION_TEXT)
												// Default Align Center
#define TLH_SORT_NO				0x0000			// No Sort
#define TLH_SORT_ASC			0x0001			// ASC Sorting of Master Column
#define TLH_SORT_DESC			0x0002			// DESC Sorting of Master Column
#define TLH_SORT_ASC2			0x0003			// ASC Sorting of Other Column
#define TLH_SORT_DESC2			0x0004			// DESC Sorting of Other Column

//TreeListCtrl Edit Consts
#define TLM_STATIC				0
#define TLM_EDIT				1
#define TLM_COMBO				2
#define TLM_CUSTOM				3
#define TLM_REQUEST				4

class CTreeListColumnInfo  
{
public:
	CTreeListColumnInfo();
	virtual ~CTreeListColumnInfo();

//	void		NormalizeWidth();

// Members
public:
	DWORD		m_dwFormat;		// Format of Column
	DWORD		m_dwModify;		// Modify of Column

	int			m_nMin;			// Min Width
	int			m_nMax;			// Max Width
	int			m_nWidth;		// Cur Width

	CString		m_strCaption;	// Caption Text
	int			m_iImage;		// Index of Image

	CString		m_strDefault;	// Default Text

protected:
	int			m_nOldWidth;	// Width before Resize
	int			m_nTrackPos;	// Position of Track

	// int	(__cdecl *m_fnCompare)(const void* pItem1, const void* pItem2);

public:
	void BeginTrack( int nTrackPos );
	void EndTrack( int nTrackPos );
	void DoTrack( int nTrackPos );

	UINT GetTextFormat();
	UINT GetCaptionFormat();

	DWORD	SetFormat( DWORD dwAdd, DWORD dwRemove = 0 );
	DWORD	GetFormat( DWORD dwMask = 0xFFFFFFFF );

	DWORD	SetModify( DWORD dwModify );
	DWORD	GetModify();

	int		SetWidth( int nWidth, int nMin = 0, int nMax = 0 );
	int		GetWidth();

	int		SetImage( int nImage );
	int		GetImage();

	bool	SetText( LPCTSTR lpszText );
	LPCTSTR GetText();

	bool	SetDefaultText( LPCTSTR lpszText );
	LPCTSTR GetDefaultText();

	int		GetMinWidth();
	int		GetMaxWidth();
};

#endif // !defined(AFX_TREELISTCOLUMNINFO_H__A20B2FC5_7522_4EFC_915D_9B4280E1DCC5__INCLUDED_)
