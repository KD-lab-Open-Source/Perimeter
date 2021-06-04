#include "stdafx.h"
#include "CWnd.h"

void ScreenToClient(HWND hWnd,LPRECT rc)
{
	ScreenToClient(hWnd,(LPPOINT)rc);
	ScreenToClient(hWnd,(LPPOINT)&rc->right);
}

void ClientToScreen(HWND hWnd,LPRECT rc)
{
	ClientToScreen(hWnd,(LPPOINT)rc);
	ClientToScreen(hWnd,(LPPOINT)&rc->right);
}

void MouseTrack(HWND hWnd)
{//Вызывается в WM_MOUSEMOVE для пролучения WM_MOUSELEAVE
	TRACKMOUSEEVENT tme;
	tme.cbSize  = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = hWnd;
	_TrackMouseEvent(&tme);
}


/////////////////////////////CWnd/////////////////
LRESULT CALLBACK CWndWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CWnd* edit=(CWnd*)GetWindowLong(hWnd,GWL_USERDATA);
	if(!edit)
		return true;
	return edit->WindowProc(uMsg,wParam,lParam);
}

CWnd::CWnd()
{
	hwnd=NULL;
	prev_wnd_proc=DefWindowProc;
}

CWnd::~CWnd()
{
}

LRESULT CWnd::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return prev_wnd_proc(hwnd,uMsg,wParam,lParam);
}

bool CWnd::Create(LPCSTR classname,DWORD dwStyle,RECT rc,HWND hWndParent)
{
	WNDCLASSEX wc;
    wc.cbSize=sizeof(wc); 
    wc.style=CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc=CWndWindowProc; 
    wc.cbClsExtra=0;
    wc.cbWndExtra=0;
    wc.hInstance=hInstance;
    wc.hIcon=NULL;
    wc.hCursor=NULL;
    wc.hbrBackground=NULL;
    wc.lpszMenuName=NULL;
    wc.lpszClassName=classname;
    wc.hIconSm=NULL;
	
	static id=1;
	ATOM a=RegisterClassEx(&wc);
	hwnd=CreateWindow(
		classname,
		classname,
		dwStyle,
		rc.left,
		rc.top,
		rc.right-rc.left,
		rc.bottom-rc.top,
		hWndParent,
		(HMENU)id++,
		hInstance,
		NULL
	);

	if(hwnd==NULL)
		return false;
	SetWindowLong(hwnd,GWL_USERDATA,(LONG)this);
	return true;
}

void CWnd::SubclassWindow(HWND h)
{
	hwnd=h;
	if(hwnd==NULL)return;
	SetWindowLong(hwnd,GWL_USERDATA,(LONG)this);
	prev_wnd_proc=(WNDPROC)SetWindowLong(h,GWL_WNDPROC,(DWORD)CWndWindowProc);
}

void CWnd::GetRect(HWND hWnd,LPRECT rc)
{
	GetWindowRect(hWnd,rc);
	ScreenToClient(hwnd,rc);
}
void CWnd::SetPos(HWND hWnd,RECT& rc)
{
	SetWindowPos(hWnd,NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,0);
}

void CWnd::LinkRight(HWND hWnd,RECT& rc_out)
{
	RECT rc,rc_main;
	GetClientRect(hwnd,&rc_main);
	GetRect(hWnd,&rc);
	rc_out.left=rc_main.right-rc.left;
	rc_out.right=rc_main.right-rc.right;
	rc_out.top=rc.top;
	rc_out.bottom=rc.bottom;
}

void CWnd::MoveRight(HWND hWnd,RECT& rc,RECT& rc_main)
{
	RECT rc_out;
	rc_out.left=rc_main.right-rc.left;
	rc_out.right=rc_main.right-rc.right;
	rc_out.top=rc.top;
	rc_out.bottom=rc.bottom;
	SetPos(hWnd,rc_out);
}

void CWnd::LinkRightBottom(HWND hWnd,RECT& rc_out)
{
	RECT rc,rc_main;
	GetClientRect(hwnd,&rc_main);
	GetRect(hWnd,&rc);
	rc_out.left=rc_main.right-rc.left;
	rc_out.right=rc_main.right-rc.right;
	rc_out.top=rc_main.bottom-rc.top;
	rc_out.bottom=rc_main.bottom-rc.bottom;
}

void CWnd::MoveRightBottom(HWND hWnd,RECT& rc,RECT& rc_main)
{
	RECT rc_out;
	rc_out.left=rc_main.right-rc.left;
	rc_out.right=rc_main.right-rc.right;
	rc_out.top=rc_main.bottom-rc.top;
	rc_out.bottom=rc_main.bottom-rc.bottom;
	SetPos(hWnd,rc_out);
}

void CWnd::LinkLRUB(HWND hWnd,RECT& rc_out)
{
	RECT rc,rc_main;
	GetClientRect(hwnd,&rc_main);
	GetRect(hWnd,&rc);
	rc_out.left=rc.left;
	rc_out.right=rc_main.right-rc.right;
	rc_out.top=rc.top;
	rc_out.bottom=rc_main.bottom-rc.bottom;
}

void CWnd::MoveLRUB(HWND hWnd,RECT& rc,RECT& rc_main)
{
	RECT rc_out;
	rc_out.left=rc.left;
	rc_out.right=rc_main.right-rc.right;
	rc_out.top=rc.top;
	rc_out.bottom=rc_main.bottom-rc.bottom;
	SetPos(hWnd,rc_out);
}

void CWnd::LinkLRBottom(HWND hWnd,RECT& rc_out)
{
	RECT rc,rc_main;
	GetClientRect(hwnd,&rc_main);
	GetRect(hWnd,&rc);
	rc_out.left=rc.left;
	rc_out.right=rc_main.right-rc.right;
	rc_out.top=rc_main.bottom-rc.top;
	rc_out.bottom=rc_main.bottom-rc.bottom;
}

void CWnd::MoveLRBottom(HWND hWnd,RECT& rc,RECT& rc_main)
{
	RECT rc_out;
	rc_out.left=rc.left;
	rc_out.right=rc_main.right-rc.right;
	rc_out.top=rc_main.bottom-rc.top;
	rc_out.bottom=rc_main.bottom-rc.bottom;
	SetPos(hWnd,rc_out);
}

void CWnd::LinkBottom(HWND hWnd,RECT& rc_out)
{
	RECT rc,rc_main;
	GetClientRect(hwnd,&rc_main);
	GetRect(hWnd,&rc);
	rc_out.left=rc.left;
	rc_out.right=rc.right;
	rc_out.top=rc_main.bottom-rc.top;
	rc_out.bottom=rc_main.bottom-rc.bottom;
}

void CWnd::MoveBottom(HWND hWnd,RECT& rc,RECT& rc_main)
{
	RECT rc_out;
	rc_out.left=rc.left;
	rc_out.right=rc.right;
	rc_out.top=rc_main.bottom-rc.top;
	rc_out.bottom=rc_main.bottom-rc.bottom;
	SetPos(hWnd,rc_out);
}

/////////////////////CDialog//////////////////////
BOOL CALLBACK CDialogDialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CDialog* edit=(CDialog*)GetWindowLong(hWnd,GWL_USERDATA);
	if(uMsg==WM_INITDIALOG)
	{
		SetWindowLong(hWnd,GWL_USERDATA,lParam);
		edit=(CDialog*)lParam;
		edit->hwnd=hWnd;
		edit->OnInitDialog();
		return false;
	}

	if(!edit)
		return false;
	return edit->WindowProc(uMsg,wParam,lParam);
}

int CDialog::DoModal(WORD id,HWND hParent)
{
	return DialogBoxParam(hInstance,MAKEINTRESOURCE(id),hParent,
		CDialogDialogProc,(DWORD)this);
}

LRESULT CDialog::WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return FALSE;
}

bool CDialog::Create(WORD id,HWND hWndParent)
{
	hwnd=CreateDialogParam(hInstance,MAKEINTRESOURCE(id),hWndParent,CDialogDialogProc,(DWORD)this);
	return hwnd!=NULL;
}


///////////////////////CPropertyDialog//////////////
void CPropertyDialog::AddEdit(CWnd* edit)
{
	edits.push_back(edit);
}

void CPropertyDialog::SizeEdit(RECT rc)
{
	vector<CWnd*>::iterator it;
	FOR_EACH(edits,it)
	{
		SetPos((*it)->hwnd,rc);
	}
}

void CPropertyDialog::ShowEdit(CWnd* edit)
{
	vector<CWnd*>::iterator it;
	FOR_EACH(edits,it)
	{
		CWnd* p=*it;
		int sw;
		sw=(p==edit)?SW_SHOW:SW_HIDE;
		ShowWindow(p->hwnd,sw);
		if(p!=edit)
			p->ClearLink();
	}
/*
	mouse_action=M_ROTATE_CAMERA;
	current_mouse_build=NULL;
	current_size_build=NULL;
	current_begin_speed=NULL;
*/
}

void CPropertyDialog::ApplyEdit()
{
	vector<CWnd*>::iterator it;
	FOR_EACH(edits,it)
	{
		CWnd* p=*it;
		p->Apply();
	}
}
