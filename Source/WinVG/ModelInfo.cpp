// ModelInfo.cpp : implementation file
//

#include "StdAfx.h"
#include "WinVG.h"
#include "ModelInfo.h"
#include "../Render/src/MeshBank.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelInfo dialog


CModelInfo::CModelInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CModelInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelInfo)
	fname = _T("");
	material_num = 0;
	object_num = 0;
	//}}AFX_DATA_INIT
	object=NULL;
}


void CModelInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelInfo)
	DDX_Control(pDX, IDC_TEXTURES, textures);
	DDX_Text(pDX, IDC_EDIT_FILENAME, fname);
	DDX_Text(pDX, IDC_EDIT_MATERIALNUM, material_num);
	DDX_Text(pDX, IDC_EDIT_OBJECTNUM, object_num);
	//}}AFX_DATA_MAP

	if(!pDX->m_bSaveAndValidate)
	{
		cAllMeshBank* root=object->GetRoot();
		vector<string> txt;

		for(int i=0;i<root->GetNumberMeshBank();i++)
		{
			cMeshBank* p=root->GetMeshBank(i);

			for(int j=0;j<NUMBER_OBJTEXTURE;j++)
			{
				cTexture* pt=p->GetTexture(j);
				if(pt)
				{
					const char* c=pt->GetName();

					for(int k=0;k<txt.size();k++)
					if(stricmp(txt[k].c_str(),c)==0)
					{
						break;
					}

					if(k==txt.size())
						txt.push_back(c);
				}
			}
		}

		for(i=0;i<txt.size();i++)
			textures.AddString(txt[i].c_str());
	}
}


BEGIN_MESSAGE_MAP(CModelInfo, CDialog)
	//{{AFX_MSG_MAP(CModelInfo)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelInfo message handlers
void CModelInfo::Init(cObjectNodeRoot* obj)
{
	object=obj;
	fname=obj->GetFileName();
	material_num=obj->GetMaterialNum();
	object_num=obj->GetObjectNum();
}
