// DirectoryTree.cpp : implementation file
//

#include "stdafx.h"
#include "WinVG.h"
#include "DirectoryTree.h"
#include "io.h"
#include "WinVGDoc.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirectoryTree

IMPLEMENT_DYNCREATE(CDirectoryTree, CTreeView)

CDirectoryTree::CDirectoryTree()
{
	strcpy(FileExtension,"*.*");
	FileDoubleClick.Empty();
}

CDirectoryTree::~CDirectoryTree()
{
}


BEGIN_MESSAGE_MAP(CDirectoryTree, CTreeView)
	//{{AFX_MSG_MAP(CDirectoryTree)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirectoryTree diagnostics

#ifdef _DEBUG
void CDirectoryTree::AssertValid() const
{
	CTreeView::AssertValid();
}

void CDirectoryTree::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDirectoryTree message handlers

void CDirectoryTree::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
	
	char StrSelectObj[256]="";
	CTreeCtrl &TreeCtrl=GetTreeCtrl();
	HTREEITEM hItemSelected=TreeCtrl.GetSelectedItem();
	if(hItemSelected) strcpy(StrSelectObj,TreeCtrl.GetItemText(hItemSelected));
	TreeCtrl.DeleteAllItems();
	char strPath[256];
	GetCurrentDirectory(256,strPath);
	HTREEITEM hItemRoot=TreeCtrl.InsertItem(strPath);

	ExpandDir(hItemRoot,"");
	
	if(StrSelectObj[0])
	{
		hItemSelected=FindItem(StrSelectObj);
		if(hItemSelected)
			TreeCtrl.SelectItem(hItemSelected);
	}
}

void CDirectoryTree::ExpandDir(HTREEITEM hItemRoot,LPCSTR dir)
{
	CTreeCtrl &TreeCtrl=GetTreeCtrl();
	_finddata_t fileinfo;
	int number,err,i;
	CString sdir=dir;
	vector<string> Directory;

	for(err=0,number=_findfirst(sdir+"*.*",&fileinfo);number>0&&err==0;err=_findnext(number,&fileinfo))
		if(fileinfo.attrib&_A_SUBDIR)
		{
			if(strcmp(fileinfo.name,".")==0)
				continue;
			if(strcmp(fileinfo.name,"..")==0)
				continue;

			string name=_strupr(fileinfo.name);
			Directory.push_back(name);
		}
	_findclose( number );

	sort(Directory.begin(),Directory.end());
	
	for(i=0;i<Directory.size();i++)
	{
		HTREEITEM h=TreeCtrl.InsertItem(Directory[i].c_str(),hItemRoot);
		TreeCtrl.SetItemData(h,I_DIRECTORY);
	}

	Directory.clear();

	for(err=0,number=_findfirst(sdir+FileExtension,&fileinfo);number>0&&err==0;err=_findnext(number,&fileinfo))
	if((fileinfo.attrib&_A_SUBDIR)==0)
	{
		string name=_strlwr(fileinfo.name);
		Directory.push_back(name);
	}

	_findclose( number );

	sort(Directory.begin(),Directory.end());
	
	for(i=0;i<Directory.size();i++)
	{
		HTREEITEM h=TreeCtrl.InsertItem(Directory[i].c_str(),hItemRoot);
		TreeCtrl.SetItemData(h,I_FILE);
	}

	TreeCtrl.Expand(hItemRoot,TVE_EXPAND);
}

CString CDirectoryTree::GetPath(HTREEITEM hItem)
{
	CTreeCtrl &TreeCtrl=GetTreeCtrl();
	HTREEITEM hcur=hItem;
	CString dir="";
	do
	{
		DWORD d=TreeCtrl.GetItemData(hcur);
		if(d==I_FILE)
			dir=TreeCtrl.GetItemText(hcur);
		else
			dir=TreeCtrl.GetItemText(hcur)+"\\"+dir;
	}while(hcur=TreeCtrl.GetParentItem(hcur));
	return dir;
}

void CDirectoryTree::ExpandDir(HTREEITEM hItemRoot)
{
	CTreeCtrl &TreeCtrl=GetTreeCtrl();
	TreeCtrl.SetItemData(hItemRoot,I_DIRECTORY_EXPAND);

	CString dir=GetPath(hItemRoot);

	ExpandDir(hItemRoot,dir);
}

HTREEITEM CDirectoryTree::FindItem(char *StrFile,HTREEITEM hItem)
{
	char str[256]="";
	CTreeCtrl &TreeCtrl=GetTreeCtrl();
	if(hItem==0) hItem=TreeCtrl.GetNextItem(NULL,TVGN_ROOT);
	if(stricmp(str,StrFile)==0)
		return hItem;
	for(hItem=TreeCtrl.GetNextItem(hItem,TVGN_CHILD);hItem;hItem=TreeCtrl.GetNextItem(hItem,TVGN_NEXT))
	{
		HTREEITEM hItemFind;
		if(hItemFind=FindItem(StrFile,hItem))
			return hItemFind;
	}
	return 0;
}

void CDirectoryTree::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CTreeView::OnLButtonDblClk(nFlags, point);

	char StrSelectObj[256];
	CTreeCtrl &TreeCtrl=GetTreeCtrl();
	HTREEITEM hItem=TreeCtrl.GetSelectedItem();
	if(hItem)
	{
		DWORD d=TreeCtrl.GetItemData(hItem);
		switch(d)
		{
		case I_FILE:
			{
				strcpy(StrSelectObj,TreeCtrl.GetItemText(hItem));
				const char *ext1=GetFileExtension(StrSelectObj);
				const char *ext2=GetFileExtension(FileExtension);
				if(ext1&&ext2&&stricmp(ext1,ext2)==0)
				{
					FileDoubleClick=GetPath(hItem);
					pDoc->SetPathName(FileDoubleClick,FALSE);
				}
			}
			break;
		case I_DIRECTORY:
			ExpandDir(hItem);
			break;
		}

	}
}
