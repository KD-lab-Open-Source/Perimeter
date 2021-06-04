#include "stdafx.h"
#include <stdio.h>
#include "Console.h"
#include <malloc.h>

//#include "meshres.h"

char* buffer=NULL;
int cur=0,size=0;

static HWND richedit;
static RECT richedit_rc;

void ClearConsole()
{
	if(buffer)
		free(buffer);
	buffer=NULL;
	cur=size=0;
}

static void ScreenToClient(HWND hwnd,RECT* rc)
{
	ScreenToClient(hwnd,(POINT*)rc);
	ScreenToClient(hwnd,((POINT*)rc)+1);
}

static void GetDlgClientRect(HWND hDlg,HWND h,RECT* rc)
{
	GetWindowRect(h,rc);
	ScreenToClient(hDlg,rc);
}

static void OnSize(WPARAM flag,int cx,int cy)
{
	MoveWindow(richedit,0,richedit_rc.top,
		cx,cy-richedit_rc.top,TRUE);
}

static 
BOOL CALLBACK DialogProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_SYSCOMMAND:
		if (LOWORD(wParam) == SC_CLOSE)
		{
			EndDialog(hwnd,1);
		}
		break;
	case WM_INITDIALOG:
		richedit=GetDlgItem(hwnd,IDC_EDIT);
		SetWindowText(richedit,buffer);
		GetDlgClientRect(hwnd,richedit,&richedit_rc);
		return TRUE;

	case WM_SIZE:
		OnSize(wParam,LOWORD(lParam),HIWORD(lParam));
		break;
	}
	return FALSE;
}



void ShowConsole(HINSTANCE hInstance,HWND parent)
{
	if(buffer==NULL)return;
	DialogBox(hInstance,MAKEINTRESOURCE(IDD_CONSOL),parent,(DLGPROC)DialogProc);
	delete buffer;
	buffer=NULL;
	cur=size=0;
}


void AddText(LPCTSTR text)
{
	const bufadd=0x1000;
	int len=strlen(text);

	if(buffer==NULL)
	{
		size=bufadd;
		buffer=(char*)malloc(bufadd);
	}

	if(size<cur+len+1)
	{
		size+=bufadd;
		buffer=(char*)realloc(buffer,size);
	}

	strcpy(buffer+cur,text);
	cur+=len;
}

void Msg(const char* format,...)
{
	va_list args;
	va_start(args,format);
	VMsg(format,args);
}

void VMsg(const char *fmt, va_list marker)
{
	char buffer[256];
	char buffer1[256];
	vsprintf( buffer, fmt, marker);
	
	for(char *p=buffer,*p1=buffer1;*p;p++,p1++)
	{
		if(*p=='\n')
			*p1++='\r';

		*p1=*p;
	}

	*p1=0;

	AddText(buffer1);
}
