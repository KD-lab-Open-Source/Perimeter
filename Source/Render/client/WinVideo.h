#ifndef __WINVIDEO_H__
#define __WINVIDEO_H__

/* TODO unused, remove this and WinVideo.cpp

struct IGraphBuilder;
struct IMediaControl;
struct IVideoWindow;
struct IMediaEvent;

struct sWinVideo
{
	sWinVideo()											{ pGraphBuilder=0; pMediaControl=0; pVideoWindow=0; pMediaEvent=0; hWnd=0; }
	~sWinVideo()										{ Close(); }
	static void Init();					// initilize DirectShow Lib
	static void Done();					// uninitilize DirectShow Lib
	// direct	
	void SetWin(void *hWnd,int x=0,int y=0,int xsize=0,int ysize=0);
	int Open(char *fname);				// if it's all OK, then function return 0
	void Play();
	void Stop();
	void Close();
	void WaitEnd();
	int IsComplete();
	// util	
	void HideCursor(int hide=1);		// 1 - hide cursor, 0 - unhide cursor
	void GetSize(int *xsize,int *ysize);
	void SetSize(int xsize,int ysize);
	void FullScreen(int bFullScreen=1);	// 1 - FullScreen, 0 - Window
	int IsPlay();

private:
	IGraphBuilder	*pGraphBuilder;
	IMediaControl	*pMediaControl;
	IVideoWindow	*pVideoWindow;
    IMediaEvent		*pMediaEvent;
	void			*hWnd;
};

#include <vfw.h>		// AVI include

struct sVideoWrite
{
public:
	sVideoWrite();
	~sVideoWrite();
	bool Open(const char* file_name,int sizex,int sizey);
	bool WriteFrame();
protected:
	Vect2i size;
    PAVIFILE         pf; 
    PAVISTREAM       psSmall; 
	BITMAPINFOHEADER bi; 
	int iCurStreamPos;
	IDirect3DTexture9* pRenderTexture;
	IDirect3DSurface9 *pSysSurface;
};

*/

#endif //__WINVIDEO_H__