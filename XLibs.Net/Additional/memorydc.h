// MemoryDC.h: interface for the CMemoryDC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYDC_H__832EE094_E57E_11D5_A1D0_000021D79DF3__INCLUDED_)
#define AFX_MEMORYDC_H__832EE094_E57E_11D5_A1D0_000021D79DF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace axl
{

	class CMemoryDC
	{
	public:

		CMemoryDC()
		{
			Reset();
		}

		CMemoryDC(CDC* _pDC,const tagSIZE* _psz,long _nbits=0)
		{
			Reset();
			Create(_pDC,_psz,_nbits);
		}

		CMemoryDC(CDC* _pDC,DWORD _nID)
		{
			Reset();
			Create(_pDC,_nID);
		}

		virtual ~CMemoryDC()
		{
			Destroy();
		}

		void Reset()
		{
			pmDC = 0;
			pmBitmap = 0;
			pbBitmap = 0;
		}

		bool Create(CDC* _pDC,const tagSIZE* _psz,long _nbits=0)
		{
			if(!Allocate()) return false;
			if(!pmDC->CreateCompatibleDC(_pDC)) return false;
			if(!_nbits)
			{
				if(!pmBitmap->CreateCompatibleBitmap(_pDC,_psz->cx,_psz->cy))
					return false;
			}
			else
			{
				if(!pmBitmap->CreateBitmap(_psz->cx,_psz->cy,1,_nbits,0))
					return false;
			}
			if(!(pbBitmap = pmDC->SelectObject(pmBitmap)))
				return false;
			return true;
		}

		bool Create(CDC* _pDC,DWORD _nID)
		{
			if(!Allocate())
				return false;
			if(!pmDC->CreateCompatibleDC(_pDC))
				return false;
			if(!pmBitmap->LoadBitmap(_nID)) 
				return false;
			if(!(pbBitmap = pmDC->SelectObject(pmBitmap)))
				return false;
			return true;
		}	

		bool Destroy()
		{
			if(!IsValid())
				return false;
			pmDC->SelectObject(pbBitmap);
			if(!Free()) return false;
			return true;
		}

		bool Allocate()
		{
			pmDC = new CDC;
			if(!pmDC) return false;
			pmBitmap = new CBitmap;
			if(!pmBitmap) return false;
			return true;
		}

		bool Free()
		{
			if(pmDC) delete pmDC;
			if(pmBitmap) delete pmBitmap;
			Reset();
			return true;
		}

		bool Fill(COLORREF _col)
		{
			CSize __sz;
			GetSize(&__sz);
			CRect __rc(0,0,__sz.cx,__sz.cy);
			pmDC->FillRect(&__rc,&CBrush(_col));
			return true;
		}

		bool IsValid()
		{
			if(!pmDC || !pmBitmap) return false;
			return true;
		}

		bool GetSize(SIZE* _psz)
		{
			if(!_psz) return true;
			BITMAP __bm;
			if(!pmBitmap->GetBitmap(&__bm)) return false;
			_psz->cx = __bm.bmWidth;
			_psz->cy = __bm.bmHeight;
			return true;
		}

		long GetHeight()
		{
			SIZE _psz;
			if(!GetSize(&_psz)) return -1;
			return _psz.cy;
		}

		long GetWidth()
		{
			SIZE _psz;
			if(!GetSize(&_psz)) return -1;
			return _psz.cx;
		}

		CDC* GetDC()
		{
			return pmDC;
		}

		CDC* operator->()
		{
			return pmDC;
		}
		private:		
		CDC*		pmDC;
		CBitmap*	pmBitmap;
		CBitmap*	pbBitmap;
	};

}

#endif // !defined(AFX_MEMORYDC_H__832EE094_E57E_11D5_A1D0_000021D79DF3__INCLUDED_)
