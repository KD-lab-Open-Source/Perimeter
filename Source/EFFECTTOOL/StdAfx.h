// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__06EF5E76_45B1_48E7_9852_C2E35A862D9E__INCLUDED_)
#define AFX_STDAFX_H__06EF5E76_45B1_48E7_9852_C2E35A862D9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <my_stl.h>

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcview.h>
#include <typeinfo.h>
#include <direct.h>
#include <shlwapi.h>

#include <vector>
#include <list>
#include <map>
#include <mmsystem.h>

#include <afxtempl.h>
using namespace std;

#include "../Render/inc/Umath.h"
#include "../Render/inc/IRenderDevice.h"
#include "../Render/inc/IVisGeneric.h"
#include "../Render/src/NParticleID.h"
#include "../Render/inc/fps.h"

#include <hash_map>

#include "xutil.h"


template<class T> struct TAutoReleaseContainer : public T
{
	~TAutoReleaseContainer(){
		clear();
	}

	void clear(){
		iterator i;
		for(i=begin(); i!=end(); i++)
			delete *i;
		T::clear();
	}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__06EF5E76_45B1_48E7_9852_C2E35A862D9E__INCLUDED_)
