// TextureDirectory.cpp : implementation file
//

#include "stdafx.h"
#include "WinVG.h"
#include "TextureDirectory.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextureDirectory dialog

extern string textures_subdir;

CTextureDirectory::CTextureDirectory(CWnd* pParent /*=NULL*/)
	: CDialog(CTextureDirectory::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextureDirectory)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTextureDirectory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextureDirectory)
	DDX_Control(pDX, IDC_DIRECTORY, directory);
	//}}AFX_DATA_MAP

	if(!pDX->m_bSaveAndValidate)
	{
		char strPath[260];
		GetCurrentDirectory(256,strPath);
		strcat(strPath,"\\*.*");

		_finddata_t fileinfo;
		int ff=_findfirst(strPath,&fileinfo);
		if(ff>0)
		while(_findnext(ff,&fileinfo)==0)
		if(fileinfo.attrib&_A_SUBDIR)
		{
			if(strcmp(fileinfo.name,".")==0)
				continue;
			if(strcmp(fileinfo.name,"..")==0)
				continue;

			_strupr(fileinfo.name);
			directory.AddString(fileinfo.name);
		}
		_findclose( ff );
		directory.SetWindowText(textures_subdir.c_str());
	}
}


BEGIN_MESSAGE_MAP(CTextureDirectory, CDialog)
	//{{AFX_MSG_MAP(CTextureDirectory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextureDirectory message handlers

void CTextureDirectory::OnOK() 
{
	CString str;
	directory.GetWindowText(str);
	textures_subdir=str;
	
	CDialog::OnOK();
}
