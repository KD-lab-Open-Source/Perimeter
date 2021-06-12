// TreeListDC.h: interface for the CTreeListDC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREELISTDC_H__7A2CD42B_DCF8_4D12_85D0_4E9BB2B63761__INCLUDED_)
#define AFX_TREELISTDC_H__7A2CD42B_DCF8_4D12_85D0_4E9BB2B63761__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTreeListDC : public CDC
{
public:
	CTreeListDC( CDC* pDC );
	virtual ~CTreeListDC();
	
	CDC* operator->()	{	return ( CDC* )this;	}
	operator CDC*()		{	return ( CDC* )this;	}

private:
    CBitmap  m_Bitmap;			// offscreen
    CBitmap* m_pOldBitmap;		// onscreen
    CDC*     m_pDC;				// render device
    CRect    m_rcRect;			// render rect
    BOOL     m_bPrint;
};

#endif // !defined(AFX_TREELISTDC_H__7A2CD42B_DCF8_4D12_85D0_4E9BB2B63761__INCLUDED_)
