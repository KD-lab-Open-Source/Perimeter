// TreeListDC.cpp: implementation of the CTreeListDC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TreeListDC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTreeListDC::CTreeListDC( CDC* pDC ) :
	m_pOldBitmap( NULL ),
	m_pDC( NULL ),
	m_rcRect( 0, 0, 0, 0 ),
	m_bPrint( FALSE )
{
	ASSERT( pDC != NULL );

	m_pDC		= pDC;
	m_bPrint	= pDC->IsPrinting();

	if( m_bPrint )
	{
		m_bPrinting	= pDC->m_bPrinting;
		m_hDC		= pDC->m_hDC;
		m_hAttribDC	= pDC->m_hAttribDC;
	}
	else
	{
		pDC->GetClipBox( &m_rcRect );
		CreateCompatibleDC( pDC );
		m_Bitmap.CreateCompatibleBitmap( pDC, m_rcRect.Width(), m_rcRect.Height() );
		m_pOldBitmap = SelectObject( &m_Bitmap );
		SetWindowOrg( m_rcRect.left, m_rcRect.top );
	}
}

CTreeListDC::~CTreeListDC()
{
	ASSERT_VALID( m_pDC );

	if( m_bPrint )
	{
		m_hDC		= NULL;
		m_hAttribDC = NULL;
	}
	else
	{
		m_pDC->BitBlt( m_rcRect.left, m_rcRect.top, m_rcRect.Width(), m_rcRect.Height(), 
					   this, m_rcRect.left, m_rcRect.top, SRCCOPY );
		SelectObject( m_pOldBitmap );
	}
}
