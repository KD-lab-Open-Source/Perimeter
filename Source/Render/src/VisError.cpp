#include "StdAfxRD.h"
cVisError VisError;

cVisError& cVisError::operator << (int a)
{
	char bNew[256];
	sprintf(bNew,"%d",a);
	buf+=bNew;
	return *this;
}

cVisError& cVisError::operator << (float a)
{
	char bNew[256];
	sprintf(bNew,"%f",a);
	buf+=bNew;
	return *this;
}

cVisError& cVisError::operator << (const char *a)
{
	if(strcmp(a,VERR_END)==0)
	{// конец потока
		if(gb_RenderDevice && gb_RenderDevice->IsFullScreen())
			ShowWindow(gb_RenderDevice->GetWindowHandle(),SW_MINIMIZE);
		if(MessageBox(NULL,buf.c_str(),"cVisGeneric::ErrorMessage()",MB_OKCANCEL)==IDOK)
			exit(1);
		buf.clear();
	}else
		buf+=a;
	return *this;
}

cVisError& cVisError::operator << (string& a)
{
	return (*this)<<a.c_str();
}


