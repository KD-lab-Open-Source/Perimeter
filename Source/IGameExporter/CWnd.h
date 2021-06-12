#pragma once

class CWnd
{
	WNDPROC prev_wnd_proc;
public:
	HWND hwnd;

	CWnd();
	virtual ~CWnd();

	//Для окон, которые хранят указатели на параметры
	virtual void ClearLink(){};	//Очистить указатель
	virtual void Apply(){};		//Обновить данные по указателю

	void Invalidate(){InvalidateRect(hwnd,NULL,FALSE);}
protected:
	friend LRESULT CALLBACK CWndWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
	bool Create(LPCSTR classname,DWORD dwStyle,RECT rc,HWND hWndParent);
	void SubclassWindow(HWND h);

	//Функции для изменения размеров child окна
	//Link вызывается при инициализации, Move в OnSize
	void LinkRight(HWND hWnd,RECT& rc);
	void MoveRight(HWND hWnd,RECT& rc,RECT& rc_main);
	void LinkRightBottom(HWND hWnd,RECT& rc);
	void MoveRightBottom(HWND hWnd,RECT& rc,RECT& rc_main);
	void LinkLRBottom(HWND hWnd,RECT& rc);
	void MoveLRBottom(HWND hWnd,RECT& rc,RECT& rc_main);
	void LinkBottom(HWND hWnd,RECT& rc);
	void MoveBottom(HWND hWnd,RECT& rc,RECT& rc_main);
	void LinkLRUB(HWND hWnd,RECT& rc);
	void MoveLRUB(HWND hWnd,RECT& rc,RECT& rc_main);

	void GetRect(HWND hWnd,LPRECT rc);
	void SetPos(HWND hWnd,RECT& rc);

};

class CDialog:public CWnd
{
	friend BOOL CALLBACK CDialogDialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
public:
	int DoModal(WORD id,HWND hParent);
	bool Create(WORD id,HWND hWndParent);
protected:
	virtual void OnInitDialog(){}
	LRESULT WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam);
};

class CPropertyDialog:public CDialog
{
	vector<CWnd*> edits;
public:
	virtual void ShowEdit(CWnd* edit);
	virtual void AddEdit(CWnd* edit);
	virtual void SizeEdit(RECT rc);
	virtual void ApplyEdit();
};

void ScreenToClient(HWND hWnd,LPRECT rc);
void ClientToScreen(HWND hWnd,LPRECT rc);
void MouseTrack(HWND hWnd);
