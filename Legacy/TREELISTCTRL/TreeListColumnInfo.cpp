// TreeListColumnInfo.cpp: implementation of the CTreeListColumnInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TreeListColumnInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTreeListColumnInfo::CTreeListColumnInfo() :
	m_dwFormat( TLF_DEFAULT_LEFT ),
	m_dwModify( TLM_STATIC ),
	m_nMin( 0 ),				// 最小宽度
	m_nMax( 0 ),				// 最大宽度
	m_nWidth( 30 ),				// 当前宽度
	m_strCaption( _T("") ),		// 显示标题
	m_iImage( -1 ),				// 图标索引
	m_nOldWidth( 0 ),
	m_nTrackPos( 0 )
// 	m_fnCompare( NULL )
{

}

CTreeListColumnInfo::~CTreeListColumnInfo()
{

}

void CTreeListColumnInfo::BeginTrack( int nTrackPos )
{
	ASSERT( m_nOldWidth == 0 );
	ASSERT( m_nTrackPos == 0 );

	m_nOldWidth = m_nWidth;
	m_nTrackPos = nTrackPos;
}

void CTreeListColumnInfo::EndTrack( int nTrackPos )
{
	int nWidth = m_nOldWidth + ( nTrackPos - m_nTrackPos );

	if( nWidth < m_nMin )
		nWidth = m_nMin;

	if( m_nMax != 0 && nWidth > m_nMax )
		nWidth = m_nMax;

	m_nWidth	= nWidth;
	m_nOldWidth = 0;
	m_nTrackPos = 0;
}

void CTreeListColumnInfo::DoTrack( int nTrackPos )
{
	int nWidth = m_nOldWidth + ( nTrackPos - m_nTrackPos );

	if( nWidth < m_nMin )
		nWidth = m_nMin;

	if( m_nMax != 0 && nWidth > m_nMax )
		nWidth = m_nMax;

	m_nWidth = nWidth;
}

UINT CTreeListColumnInfo::GetTextFormat()
{
	UINT dwFormat = DT_SINGLELINE;

	if( m_dwFormat&TLF_TEXTALIGN_LEFT )
	{
		dwFormat |= DT_LEFT;
		dwFormat |= DT_WORD_ELLIPSIS;
	}
	else if( m_dwFormat&TLF_TEXTALIGN_RIGHT )
	{
		dwFormat |= DT_RIGHT;
	}
	else if( m_dwFormat&TLF_TEXTALIGN_CENTER )
	{
		dwFormat |= DT_CENTER;
	}
	else
	{
		dwFormat |= DT_LEFT;
		dwFormat |= DT_WORD_ELLIPSIS;
	}

	if( m_dwFormat&TLF_TEXTVALIGN_TOP )
		dwFormat |= DT_TOP;
	else if( m_dwFormat&TLF_TEXTVALIGN_BOTTOM )
		dwFormat |= DT_BOTTOM;
	else if( m_dwFormat&TLF_TEXTVALIGN_CENTER )
		dwFormat |= DT_VCENTER;
	else
		dwFormat |= DT_VCENTER;

	return dwFormat;
}

UINT CTreeListColumnInfo::GetCaptionFormat()
{
	UINT dwFormat = DT_SINGLELINE;

	if( m_dwFormat&TLF_ALIGN_LEFT )
	{
		dwFormat |= DT_LEFT;
		dwFormat |= DT_WORD_ELLIPSIS;
	}
	else if( m_dwFormat&TLF_ALIGN_RIGHT )
	{
		dwFormat |= DT_RIGHT;
	}
	else if( m_dwFormat&TLF_TEXTALIGN_CENTER )
	{
		dwFormat |= DT_CENTER;
	}
	else
	{
		dwFormat |= DT_CENTER;
	}

	if( m_dwFormat&TLF_VALIGN_TOP )
		dwFormat |= DT_TOP;
	else if( m_dwFormat&TLF_VALIGN_BOTTOM )
		dwFormat |= DT_BOTTOM;
	else if( m_dwFormat&TLF_VALIGN_CENTER )
		dwFormat |= DT_VCENTER;
	else
		dwFormat |= DT_VCENTER;

	return dwFormat;
}

DWORD CTreeListColumnInfo::SetFormat( DWORD dwAdd, DWORD dwRemove )
{
	DWORD dwOld = m_dwFormat;
	m_dwFormat |= dwAdd;
	m_dwFormat &= ~dwRemove;
	return dwOld;
}

DWORD CTreeListColumnInfo::GetFormat( DWORD dwMask )
{
	return m_dwFormat&dwMask;
}

DWORD CTreeListColumnInfo::SetModify( DWORD dwModify )
{
	DWORD dwOld = m_dwModify;
	m_dwModify = dwModify;
	return dwOld;
}

DWORD CTreeListColumnInfo::GetModify()
{
	return m_dwModify;
}

int CTreeListColumnInfo::SetWidth( int nWidth, int nMin, int nMax )
{
	if( 0 > nMin )
		nMin = 0;

	if( 0 > nMax )
		nMax = 0;

	if( nMin > nWidth )
		nWidth = nMin;

	if( nMax != 0 )
	{
		if( nMin > nMax )
			nMax = nMin;

		if( nWidth > nMax )
			nWidth = nMax;
	}

	m_nMin = nMin;
	m_nMax = nMax;
	m_nWidth = nWidth;

	return 0;
}

int CTreeListColumnInfo::GetWidth()
{
	return m_nWidth;
}

int CTreeListColumnInfo::SetImage( int iImage )
{
	int iOld = m_iImage;
	m_iImage = iImage;
	return iOld;
}

int CTreeListColumnInfo::GetImage()
{
	return m_iImage;
}

bool CTreeListColumnInfo::SetText( LPCTSTR lpszText )
{
	m_strCaption = lpszText;
	return true;
}

LPCTSTR CTreeListColumnInfo::GetText( )
{
	return m_strCaption;
}

bool CTreeListColumnInfo::SetDefaultText( LPCTSTR lpszText )
{
	m_strDefault = lpszText;
	return TRUE;
}

LPCTSTR CTreeListColumnInfo::GetDefaultText()
{
	return m_strDefault;
}

int CTreeListColumnInfo::GetMinWidth()
{
	return m_nMin;
}

int CTreeListColumnInfo::GetMaxWidth()
{
	return m_nMax;
}
