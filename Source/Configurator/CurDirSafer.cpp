// CurDirSafer.cpp: implementation of the CurDirSafer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CurDirSafer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int BUFFER_LEN = 4096;
CurDirSafer::CurDirSafer()
{}
CurDirSafer::CurDirSafer(const CString &strCurDir){
	m_strCurrDirSave = strCurDir;
}

CurDirSafer::~CurDirSafer()
{
}

void CurDirSafer::set_current()
{
	if(!m_strCurrDirSave.IsEmpty())
		SetCurrentDirectory(m_strCurrDirSave);
}

CString CurDirSafer::GetCurrentDirectory()
{
	CString str;
	::GetCurrentDirectory(BUFFER_LEN, str.GetBufferSetLength(BUFFER_LEN));
	str.ReleaseBuffer();
	return str;
}

void CurDirSafer::save_current(){
	m_strCurrDirSave = CurDirSafer::GetCurrentDirectory();
}
