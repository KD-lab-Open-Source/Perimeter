#include <SDL.h>
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
		if(gb_RenderDevice && gb_RenderDevice->IsFullScreen()) {
#ifdef PERIMETER_EXODUS
            SDL_MinimizeWindow(fromHWND(gb_RenderDevice->GetWindowHandle()));
#else
			ShowWindow(gb_RenderDevice->GetWindowHandle(),SW_MINIMIZE);
#endif
		}
        if (MessageBoxQuestion("Perimeter cVisGeneric::ErrorMessage()", buf.c_str(), SDL_MESSAGEBOX_ERROR)) {
            exit(1);
        }
		buf.clear();
	}else
		buf+=a;
	return *this;
}

cVisError& cVisError::operator << (std::string& a)
{
	return (*this)<<a.c_str();
}


