#include "StdAfx.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "PlayBink.h"

// #include "bink.h"
#include "PerimeterSound.h"
#include "SystemUtil.h"
#include "Runtime.h"
#include "../Render/D3D/RenderDevice.h"
#include "PlayOgg.h"

extern bool terEnableGDIPixel;
#ifdef _DEMO_
#define _NO_BINK_
#endif
#define _NO_BINK_

#ifdef _NO_BINK_

class PlayBinkR:public PlayBink
{
public:
	PlayBinkR(){};
	~PlayBinkR(){}

	bool Init(const char* bink_name){return true;}
	void Draw(int x,int y,int dx,int dy, int alpha = 255){}
	bool CalcNextFrame(){return false;}
	bool IsEnd(){return true;}
	Vect2i GetSize(){return Vect2i(10,10);}
	void SetVolume(float vol){}
};

PlayBink* PlayBink::Create()
{
	return new PlayBinkR;
}

#else

#pragma comment(lib,"binkw32")

class PlayBinkR:public PlayBink
{
public:
	PlayBinkR();
	virtual ~PlayBinkR();

	bool Init(const char* bink_name);
	void Draw(int x,int y,int dx,int dy, int alpha = 255);
	bool CalcNextFrame();
	bool IsEnd();
	Vect2i GetSize();
	void SetVolume(float vol);
protected:
	HBINK Bink;
	bool focus;
	cTexture* pTextureBink;

	void ShowNextFrame();
	void Done();
};

PlayBink* PlayBink::Create()
{
	return new PlayBinkR;
}

//############################################################################
//##                                                                        ##
//##  Good_sleep_us - sleeps for a specified number of MICROseconds.        ##
//##    The task switcher in Windows has a latency of 15 ms.  That means    ##
//##    you can ask for a Sleep of one millisecond and actually get a       ##
//##    sleep of 15 ms!  In normal applications, this is no big deal,       ##
//##    however, with a video player at 30 fps, 15 ms is almost half our    ##
//##    frame time!  The Good_sleep_us function times each sleep and keeps  ##
//##    the average sleep time to what you requested.  It also give more    ##
//##    accuracy than Sleep - Good_sleep_us() uses microseconds instead of  ##
//##    milliseconds.                                                       ##
//##                                                                        ##
//############################################################################

static void Good_sleep_us( S32 microseconds )
{
  static S32 total_sleep=0;
  static S32 slept_in_advance=0;
  static U64 frequency=1000;
  static S32 got_frequency=0;

  //
  // If this is the first time called, get the high-performance timer count.
  //

  if ( !got_frequency )
  {
    got_frequency = 1;
    QueryPerformanceFrequency( ( LARGE_INTEGER* )&frequency );
  }

  total_sleep += microseconds;

  //
  // Have we exceeded our reserve of slept microseconds?
  //

  if (( total_sleep - slept_in_advance ) > 1000)
  {
    U64 start, end;
    total_sleep -= slept_in_advance;

    //
    // Do the timed sleep.
    //

    QueryPerformanceCounter( ( LARGE_INTEGER* )&start );
    Sleep( total_sleep / 1000 );
    QueryPerformanceCounter( ( LARGE_INTEGER* )&end );

    //
    // Calculate delta time in microseconds.
    //

    end = ( (end - start) * (U64)1000000 ) / frequency;

    //
    // Keep track of how much extra we slept.
    //

    slept_in_advance = ( U32 )end - total_sleep;
    total_sleep %= 1000;
  }
}

PlayBinkR::PlayBinkR()
{
	Bink = 0;
	focus=true;
	pTextureBink=NULL;
}

PlayBinkR::~PlayBinkR()
{
	Done();
}

Vect2i PlayBinkR::GetSize()
{
	xassert(Bink);
	return Vect2i((int)Bink->Width,(int)Bink->Height);
}

bool PlayBinkR::Init(const char* bink_name)
{
	Done();
	string error;
	BinkSoundUseDirectSound(SNDGetDirectSound());

	Bink = BinkOpen( bink_name, 0 );
	if (!Bink )
	{
		error="Cannot bink file ";
		error+=bink_name;
		xassert_s(0,error.c_str());
		return false;
	}

	int dx_real=Power2up(Bink->Width);
	int dy_real=Power2up(Bink->Height);

	RELEASE(pTextureBink);
	pTextureBink=gb_VisGeneric->CreateTexture(dx_real,dy_real,false);
	return true;
}

void PlayBinkR::Done()
{
	if(Bink)
	{
		BinkClose( Bink );
		Bink = 0;
	}

	RELEASE(pTextureBink);
}

bool PlayBinkR::CalcNextFrame()
{
	if(focus && !applicationHasFocus())
	{//KillFocus
		BinkPause( Bink, 1 );
	}

	if(!focus && applicationHasFocus())
	{//SetFocus
		BinkPause( Bink, 0 );
	}
	focus=applicationHasFocus();
	if(!BinkWait(Bink))
	{
		ShowNextFrame();
		return true;
	}

	Good_sleep_us( 500 );
	return false;
}

bool PlayBinkR::IsEnd()
{
	return !(Bink->FrameNum < Bink->Frames);
}

void PlayBinkR::ShowNextFrame()
{
	BinkDoFrame(Bink);
	int pitch=0;
	BYTE* ptr=pTextureBink->LockTexture(pitch,Vect2i(0,0),Vect2i((int)Bink->Width,(int)Bink->Height));
	DWORD flags=BINKSURFACE32;//|BINKCOPYALL|BINKSURFACESLOW;
	s32 err=BinkCopyToBuffer( Bink,
					  ptr,
					  pitch,
					  Bink->Height,
					  0,0,
					  flags);
	pTextureBink->UnlockTexture();

	//
	// Are we at the end of the movie?
	//
	if ( Bink->FrameNum != Bink->Frames )
	{
		BinkNextFrame( Bink );
	}
}

void PlayBinkR::Draw(int x,int y,int dx,int dy, int alpha)
{
	terRenderDevice->DrawSprite(x,y,dx,dy,0,0,
		Bink->Width/(float)pTextureBink->GetWidth(),
		Bink->Height/(float)pTextureBink->GetHeight(),
		pTextureBink, sColor4c(255,255,255,alpha));
}

void PlayBinkR::SetVolume(float vol)
{
	vol=clamp(vol,0.0f,2.0f);
	BinkSetVolume(Bink,0,vol*32768);
}

//////////////////////////////
PlayBinkSample::PlayBinkSample()
{
	pos.set(0,0);
	size.set(-1,-1);
	pTextureBackground=NULL;
}

PlayBinkSample::~PlayBinkSample()
{
	RELEASE(pTextureBackground);
}

bool PlayBinkSample::MustInterrupt()
{
	return !terEnableGDIPixel;
}

extern MpegSound gb_Music;

void PlayBinkSample::DoModal(const char* bink_name,const char* sound_name)
{
	pTextureBackground=gb_VisGeneric->CreateTextureScreen();
	if(!pTextureBackground)
	{
		xassert_s(0,"Cannot create background texture to play bink file");
		return;
	}

	bink=PlayBink::Create();
	bink->Init(bink_name);
	bink->SetVolume(terMusicVolume);

	if(sound_name)
		gb_Music.OpenToPlay(sound_name);

	Vect2i real_size=bink->GetSize();
	if(size.x>=0)
	{
		real_size.x=size.x;
	}

	if(size.y>=0)
	{
		real_size.y=size.y;
	}

	MSG msg;
	while(!MustInterrupt() && !bink->IsEnd())
	{
		if ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}else
		{
			if(bink->CalcNextFrame())
			{
				terRenderDevice->Fill(0,0,0,0);
				terRenderDevice->BeginScene();
				int dx=terRenderDevice->GetSizeX();
				int dy=terRenderDevice->GetSizeY();
				terRenderDevice->DrawSprite(0,0,dx,dy,0,0,
					dx/(float)pTextureBackground->GetWidth(),
					dy/(float)pTextureBackground->GetHeight(),
					pTextureBackground);

				bink->Draw(pos.x,pos.y,real_size.x,real_size.y);

				terRenderDevice->EndScene();
				terRenderDevice->Flush();
			}
		}
	}

	delete bink;
	bink=NULL;
	RELEASE(pTextureBackground);

	if(sound_name)
		gb_Music.Stop();
}

#endif 