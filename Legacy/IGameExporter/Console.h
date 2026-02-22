#ifndef _CONSLOLE_H_
#define _CONSLOLE_H_

void Msg(const char* format,...);
void VMsg(const char *fmt, va_list marker );

void ShowConsole(HINSTANCE hInstance,HWND parent);
void ClearConsole();

#endif //_CONSLOLE_H_