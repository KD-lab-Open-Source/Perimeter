#include "StdAfxRD.h"
#include "SystemUtil.h"
#include <SDL.h>

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
            SDL_MinimizeWindow(sdlWindow);
		}
        if (MessageBoxQuestion("Perimeter cVisError, exit?", buf.c_str(), SDL_MESSAGEBOX_ERROR)) {
            ErrH.Abort(buf.c_str());
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


