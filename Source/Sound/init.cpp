#include "StdAfx.h"
#include "PerimeterSound.h"
#include "SoundInternal.h"
#include "C3D.h"
#include "SoundScript.h"

static LPDIRECTSOUND8 g_pDS = 0;
static bool g_enable_sound = false;
static bool g_enable_voices = true;

SND3DListener snd_listener;

static float global_volume=1;
static char sound_directory[260]="";

namespace SND {
FILE* snd_error=NULL;

const float DB_MIN=-10000.0f;
const float DB_MAX=0;
const float DB_SIZE=10000.0f;

int FromDirectVolume(long vol)
{
	double v=exp((exp(((vol-DB_MIN)/(double)DB_SIZE)*log(10.))-1.0)*log(2.0)*8/9.0)-1;
	
	return round(v);
}

long ToDirectVolume(int vol)
{
	double t1=9.0*log(double(vol + 1))/(log(2.0)*8) + 1.0;
	double t2=log10(t1)*DB_SIZE;

	int v = DB_MIN + round(t2);
	return v;
}

long ToDirectVolumef(float vol);

long ToPan(double vol)
{
	double sgn=vol>0?1:-1;

	vol=fabs(vol);
	if(vol>1)vol=1;
	vol=(1-vol)*255;

	int v = DB_MIN + round(
		log10(
			  9.0*log(double(vol + 1))/(log(2.0)*8) + 1.0
			 )*DB_SIZE
		);
	return round(-sgn*v);
}
/*
long ToDirectVolumef(float volume)
{
	long vol=ToDirectVolume(volume*255.0f);
	vol += round((1.0f - global_volume) * (float)(-DB_SIZE - vol));
	return vol;
}
/*/
long ToDirectVolumef(float volume)
{
	long vol=ToDirectVolume(volume*255.0f);
	long vol1=ToDirectVolume(global_volume*255.0f);
	vol+=vol1;
	if(vol<DB_MIN)
		vol=DB_MIN;
	
	return vol;
}
/**/

static float width2d=1,power2d_width=1;

int PanByX(float x)
{
	if(x<0)x=0;
	if(x>width2d)x=width2d;
	double xx=(2.0f*x-width2d)*power2d_width;

	return ToPan(xx);
}

void logs(const char *format, ...)
{
	if(snd_error)
	{
	  va_list args;
	  char    buffer[512];
	  va_start(args,format);
	  vsprintf(buffer,format,args);
	  fprintf(snd_error,buffer);
	}
}


int pause_level=0;

LARGE_INTEGER timer_frequency;
void InitTimer()
{
	QueryPerformanceFrequency(&timer_frequency);
}

double GetTime()
{
	LARGE_INTEGER PerformanceCount;
	QueryPerformanceCounter(&PerformanceCount);
	return PerformanceCount.QuadPart/(double)timer_frequency.QuadPart;
}

};

using namespace SND;

void SNDSetLocDataDirectory(LPCSTR dir)
{
	SNDScript::setLocDataPath(dir);
}

void SNDSetBelligerentIndex(int idx)
{
	SNDScript::setBelligerentIndex(idx);
}

void* SNDGetDirectSound()
{
	return g_pDS;
}

void SNDEnableSound(bool enable)
{
	g_enable_sound = enable && g_pDS;
	if(!enable)
		SNDStopAll();
}

void SNDEnableVoices(bool enable)
{
	g_enable_voices = enable;

	if(!enable)
		script2d.StopAllVoices();
}

bool SNDIsSoundEnabled()
{
	return g_enable_sound;
}
bool SNDIsVoicesEnabled() {
	return g_enable_voices;
}

void SND2DPanByX(float width,float power)
{
	width2d=width;
	power2d_width=power/width;
}


void SNDSetSoundDirectory(LPCSTR dir)
{
	strncpy(sound_directory,dir,sizeof(sound_directory));
	sound_directory[sizeof(sound_directory)-1]=0;
}

LPCSTR SNDGetSoundDirectory()
{
	return sound_directory;
}

bool SNDInitSound(HWND hWnd,bool bEnable3d,bool _soft3d)
{
	SNDReleaseSound();

	snd_hWnd=hWnd;
	initclock();

	if(false)
	{
		InitTimer();
		timeBeginPeriod(1);
		double beg1=GetTime()*1000.0;
		double beg=clockf();

		DWORD beg2=timeGetTime();

		const int maxi=1000000;
		for(int i=0;i<maxi;i++)
		{
			double d=GetTime();
		}

		char s[256];

		double end=clockf();
		double end1=GetTime()*1000.0;
		DWORD end2=timeGetTime();

		double dd=((end1-beg1)-(end-beg))/(end-beg);
		sprintf(s,"time0=%f ms,time1=%f,time2=%i,error01=%f \n",
					end-beg,end1-beg1,end2-beg2,dd);
		OutputDebugString(s);

		timeEndPeriod(1);
	}

	HRESULT hr;

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate8( NULL, &g_pDS, NULL ) ) )
	{
		logs("Cannot create DirectSoundCreate8\n");
        return false;
	}

    // Set DirectSound coop level 
    if( FAILED( hr = g_pDS->SetCooperativeLevel( snd_hWnd, DSSCL_PRIORITY ) ) )
	{
		logs("Cannot SetCooperativeLevel\n");
		SNDReleaseSound();
        return false;
	}
    
    // Get the primary buffer 
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    DSBUFFERDESC        dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;
       
    if(FAILED(hr=g_pDS->CreateSoundBuffer(&dsbd,&pDSBPrimary,NULL)))
	{
		logs("Cannot CreateSoundBuffer primary\n");
		SNDReleaseSound();
        return false;
	}

	struct FORMATS
	{
		int cannels;
		int herz;
		int bits;
	} formats[]=
	{
		{1,22050,8},
		{2,22050,8},
		{1,22050,16},
		{2,22050,16},
		{1,44100,16},
		{2,44100,16},
	};

	for(int i=SIZE(formats)-1;i>=0;i--)
	{
		WAVEFORMATEX wfx;
		ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
		wfx.wFormatTag      = WAVE_FORMAT_PCM; 
		wfx.nChannels       = formats[i].cannels; 
		wfx.nSamplesPerSec  = formats[i].herz; 
		wfx.wBitsPerSample  = formats[i].bits; 
		wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

		hr = pDSBPrimary->SetFormat(&wfx);
		if(!FAILED(hr))
			break;
	}

	SAFE_RELEASE(pDSBPrimary);
	if(FAILED(hr))
	{
		logs("Cannot set format primary sound buffer\n");
		SNDReleaseSound();
		return false;
	}


	pause_level = 0;
	SNDEnableSound(true);
	static DWORD n=4294948478;
	float f=n;
	return true;
}

void SNDReleaseSound()
{
	if(snd_hWnd==NULL)return;

	script3d.RemoveAll();
	script2d.RemoveAll();

	SAFE_RELEASE(g_pDS);
	if(snd_error)
	{
		fclose(snd_error);
		snd_error=NULL;
	}

	snd_hWnd=NULL;
}

bool SNDEnableErrorLog(LPCSTR file)
{
	if(snd_error)fclose(snd_error);
	snd_error=fopen(file,"wt");
	if(snd_error)fprintf(snd_error,"Log started\n");
	return snd_error!=NULL;
}

void SNDSetVolume(float volume)
{
	global_volume=volume;
	if(global_volume>1)global_volume=1;
	if(global_volume<0)global_volume=0;

	SNDUpdateAllSoundVolume();
}

float SNDGetVolume()
{
	return global_volume;
}


bool SND3DScriptDisable(LPCSTR name)
{
	return script3d.RemoveScript(name);
}

void SND3DScriptDisableAll()
{
	script3d.RemoveAll();
}

bool SND2DScriptDisable(LPCSTR name)
{
	return script2d.RemoveScript(name);
}

void SND2DScriptDisableAll()
{
	script2d.RemoveAll();
}

bool SNDScriptPrmEnable(const SoundScriptPrm& prm)
{
	if(prm.is3D)
		return script3d.AddScript(&prm);
	else
		return script2d.AddScript(&prm);
}

bool SNDScriptPrmEnableAll()
{
	vector<SoundScriptPrm>::const_iterator i;
	FOR_EACH(soundScriptTable().table, i)
		SNDScriptPrmEnable(*i);

	return true;
}

bool RestoreBuffer(LPDIRECTSOUNDBUFFER pDSB)
{
	//Вообще-то после Restore для hardvare buffer неплохо 
	//бы его преречитать
    HRESULT hr;
    DWORD dwStatus;
    if( FAILED( hr = pDSB->GetStatus( &dwStatus ) ) )
        return false;

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
        do 
        {
            hr = pDSB->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( (hr = pDSB->Restore()) );
    }

	return true;
}


LPDIRECTSOUNDBUFFER SNDLoadSound(LPCSTR fxname,DWORD dwCreationFlags)
{
	HRESULT hr;
	if(!g_enable_sound)
		return NULL;

//	char fname[260];
//	sprintf(fname,"%s%s.wav",sound_directory,fxname);

    char* fname = const_cast<char*>(fxname);

    LPDIRECTSOUNDBUFFER* apDSBuffer     = NULL;
    DWORD                dwDSBufferSize = NULL;
    CWaveFile*           pWaveFile      = NULL;

	LPDIRECTSOUNDBUFFER pDSBuffer=NULL;

    VOID*   pDSLockedBuffer      = NULL; // Pointer to locked buffer memory
    DWORD   dwDSLockedBufferSize = 0;    // Size of the locked DirectSound buffer
    DWORD   dwWavDataRead        = 0;    // Amount of data read from the wav file 

    pWaveFile = new CWaveFile();
    if( pWaveFile == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto LFail;
    }

    pWaveFile->Open( fname, NULL, WAVEFILE_READ );

    if( pWaveFile->GetSize() == 0 )
    {
        // Wave is blank, so don't create it.
        hr = E_FAIL;
        goto LFail;
    }

    // Make the DirectSound buffer the same size as the wav file
    dwDSBufferSize = pWaveFile->GetSize();

    // Create the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = dwCreationFlags;
    dsbd.dwBufferBytes   = dwDSBufferSize;
    dsbd.guid3DAlgorithm = DS3DALG_DEFAULT;
//	dsbd.guid3DAlgorithm = DS3DALG_HRTF_FULL;
    dsbd.lpwfxFormat     = pWaveFile->m_pwfx;

    // DirectSound is only guarenteed to play PCM data.  Other
    // formats may or may not work depending the sound card driver.
    hr = g_pDS->CreateSoundBuffer( &dsbd, &pDSBuffer, NULL );

    if( FAILED(hr) )
        goto LFail;

    // Make sure we have focus, and we didn't just switch in from
    // an app which had a DirectSound device
    if( FAILED( hr = RestoreBuffer( pDSBuffer) ) ) 
        goto LFail;

    // Lock the buffer down
    if( FAILED( hr = pDSBuffer->Lock( 0, dwDSBufferSize, 
                                 &pDSLockedBuffer, &dwDSLockedBufferSize, 
                                 NULL, NULL, 0L ) ) )
        goto LFail;

    // Reset the wave file to the beginning 
    pWaveFile->ResetFile();

    if( FAILED( hr = pWaveFile->Read( (BYTE*) pDSLockedBuffer,
                                        dwDSLockedBufferSize, 
                                        &dwWavDataRead ) ) )
	{
		pDSBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );
        goto LFail;
	}

	// Unlock the buffer, we don't need it anymore.
    pDSBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );

    SAFE_DELETE( pWaveFile );
    return pDSBuffer;

LFail:
    // Cleanup
    SAFE_DELETE( pWaveFile );
    //SAFE_DELETE( pDSBuffer ); //pDSBuffer is abstract, calling delelte is UB
    if (pDSBuffer) {
        pDSBuffer->Release();
        (pDSBuffer) = NULL;
    }
    return NULL;
}

void SNDUpdateAllSoundVolume()
{
	SNDScript::MapScript::iterator it;
	FOR_EACH(script3d.map_script,it)
	{
		ScriptParam* sp=(*it).second;
		MTAuto lock(sp->GetLock());
		vector<SNDOneBuffer>::iterator itb;
		FOR_EACH(sp->GetBuffer(),itb)
		{
			(*itb).RecalculateVolume();
		}
	}

	FOR_EACH(script2d.map_script,it)
	{
		ScriptParam* sp=(*it).second;
		MTAuto lock(sp->GetLock());
		vector<SNDOneBuffer>::iterator itb;
		FOR_EACH(sp->GetBuffer(),itb)
		{
			(*itb).RecalculateVolume();
		}
	}
}

///////////////////////SND3DListener////////////////////////
SND3DListener::SND3DListener()
:velocity(0,0,0)
{
	rotate=invrotate=Mat3f::ID;
	position.x=position.y=position.z=0;
	//mat=MatXf::ID;
	s_distance_factor=1.0;//DS3D_DEFAULTDISTANCEFACTOR
	s_doppler_factor=1.0;//DS3D_DEFAULTDOPPLERFACTOR
	s_rolloff_factor=1.0;//DS3D_DEFAULTROLLOFFFACTOR

	velocity.x=velocity.y=velocity.z=0;

	front.x=0;
	front.y=0;
	front.z=1.0;
	top.x=0;
	top.y=1.0;
	top.z=0;
	right.x=-1.0;
	right.y=0;
	right.z=0;

	zmultiple=1.0f;
}

SND3DListener::~SND3DListener()
{
}

bool SND3DListener::SetDistanceFactor(float f)
{
	s_distance_factor=f;
	return true;
}

bool SND3DListener::SetDopplerFactor(float f)
{
	s_doppler_factor=f;
	return true;
}

bool SND3DListener::SetRolloffFactor(float f)
{
	s_rolloff_factor=f;
	return true;
}

bool SND3DListener::SetPos(const MatXf& _mat)
{
	Vect3f t=_mat.trans();

	rotate=_mat.rot();
	invrotate.invert(rotate);
	position=invrotate*t;
	position=-position;

//	front=invrotate*Vect3f(0,0,1.0f);;
//	top=invrotate*Vect3f(0,1.0f,0);
//	right=invrotate*Vect3f(-1.0,0,0);

	return true;
}

bool SND3DListener::SetVelocity(const Vect3f& _velocity)
{
	velocity=_velocity;
	return true;
}

HRESULT SNDOneBuffer::RecalculatePos()
{
	if(!used)return S_OK;
	//Vect3f p=snd_listener.mat*pos;
	Vect3f p=snd_listener.rotate*(pos-snd_listener.position);
	p3DBuffer->SetPosition(pos);//p);

//	Vect3f v=snd_listener.mat.rot()*velocity;
	Vect3f v=snd_listener.rotate*velocity;
	HRESULT hr;
	FAIL(hr=p3DBuffer->SetVelocity(v));
	p3DBuffer->RecalculatePos();

	return hr;

}

void SNDOneBuffer::RecalculateVolume()
{
	if(!used)return;
	
	if(p3DBuffer)
	{
		p3DBuffer->SetVolume(volume);
		p3DBuffer->RecalculateVolume();
	}else
	{
		HRESULT hr;
		long vol=ToDirectVolumef(volume);
		FAIL(hr=buffer->SetVolume(vol));
	}
}

HRESULT SNDOneBuffer::PlayPreprocessing()
{
	HRESULT hr=S_OK;
	if(script->delta_enable)
	{
		if(script->delta_random)
		{
			int frequency=nSamplesPerSec;
			float dw=script->delta_up-script->delta_down;
			float mul=frand()*dw+script->delta_down;
			frequency=round(frequency*mul);

			if(frequency<0 || frequency>1)
				logs("Invalid frequency=%f\n",frequency);

			if(p3DBuffer)
				hr=p3DBuffer->SetFrequency(frequency)?S_OK:E_FAIL;
			else
				hr=buffer->SetFrequency(frequency);
		}
	}

	return hr;
}


bool SND3DListener::Update()
{
	SNDScript::MapScript::iterator it;
	FOR_EACH(script3d.map_script,it)
	{
		ScriptParam* sp=(*it).second;
		MTAuto lock(sp->GetLock());
		sp->RecalculateClipDistance();

		vector<SNDOneBuffer>::iterator itb;
		FOR_EACH(sp->GetBuffer(),itb)
		{
			(*itb).RecalculatePos();
		}
	}

	return true;
}

///////////////////////SND3DSound////////////////////////
SND3DSound::SND3DSound()
{
	script=NULL;
	cur_buffer=-1;
}

void SND3DSound::Destroy()
{
	if(g_pDS && script && cur_buffer>=0)
	{
		Stop();
		MTAuto lock(script->GetLock());
		SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
		ASSERT(s.pSound==this);
		s.used=false;
		s.pSound=NULL;
	}

	script=NULL;
	cur_buffer=-1;
}

SND3DSound::~SND3DSound()
{
	Destroy();
}

bool SND3DSound::Init(LPCSTR name)
{
	Destroy();
	if(!g_enable_sound)return false;
	bool b=script3d.FindFree(name,script,cur_buffer);
	if(!b)return false;

	MTAuto lock(script->GetLock());
	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
	s.pSound=this;

	s.pos.x=s.pos.y=s.pos.z=0;
	s.velocity.x=s.velocity.y=s.velocity.z=0;
	s.volume=script->def_volume;

	s.p3DBuffer->SetMinDistance(script->radius);
	s.p3DBuffer->SetMaxDistance(script->max_radius);

	return true;
}

void SND3DSound::SetRealVolume(float vol)
{
	if(!g_enable_sound || script==NULL)return;
	MTAuto lock(script->GetLock());
	AssertValid();
	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
	if(vol>1)vol=1;
	if(vol<0)vol=0;
	s.volume=vol;
	s.RecalculateVolume();
}

void SND3DSound::SetVolume(float vol)
{
	if(!g_enable_sound || script==NULL)return;

	float v=vol*(script->def_volume-script->min_volume)+script->min_volume;
	SetRealVolume(v);
}

bool SND3DSound::SetRealFrequency(DWORD frequency)
{
	if(!g_enable_sound || script==NULL)return false;
	MTAuto lock(script->GetLock());
	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
	HRESULT hr;
	hr=s.p3DBuffer->SetFrequency(frequency);
	return SUCCEEDED(hr);
}

bool SND3DSound::SetFrequency(float frequency)
{
	if(!g_enable_sound || script==NULL)return false;
	MTAuto lock(script->GetLock());
	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
	return s.SetFrequency(frequency);
}

void SND3DSound::AssertValid()
{
	ASSERT(script);
	ASSERT(cur_buffer>=0 && cur_buffer<script->GetBuffer().size());
}

void SND3DSound::SetPos(const Vect3f& pos)
{
	if(script==NULL)return;
	MTAuto lock(script->GetLock());
	AssertValid();
	Vect3f& p=script->GetBuffer()[cur_buffer].pos;
	p=pos;
}

void SND3DSound::SetVelocity(const Vect3f& velocity)
{
	if(script==NULL)return;
	MTAuto lock(script->GetLock());
	AssertValid();
	script->GetBuffer()[cur_buffer].velocity=velocity;
}

bool SND3DSound::Play(bool cycled)
{
	if(script==NULL || !g_enable_sound)
		return false;
	MTAuto lock(script->GetLock());
	AssertValid();
	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];

	s.PlayPreprocessing();
	s.RecalculatePos();
	s.RecalculateVolume();

	LPDIRECTSOUNDBUFFER buffer=s.buffer;
	s.played_cycled=cycled;

	s.begin_play_time=clockf();
	return s.p3DBuffer->Play(cycled);
}

bool SND3DSound::Stop()
{
	if(script==NULL || !g_enable_sound)return true;
	MTAuto lock(script->GetLock());
	AssertValid();
	return script->GetBuffer()[cur_buffer].p3DBuffer->Stop();
}

bool SND3DSound::IsPlayed()
{
	if(script==NULL)return false;
	MTAuto lock(script->GetLock());
	return script->GetBuffer()[cur_buffer].p3DBuffer->IsPlaying();
}

////////////////////////////////////////////////////////////

bool SNDScript::FindFree(LPCSTR name,ScriptParam*& script,int& nfree)
{
	script=NULL;
	nfree=-1;

	ScriptParam* p=Find(name);

	if(p==NULL)
	{
		logs("sound %s not found\n",name);
		return false;
	}
	MTAuto lock(p->GetLock());
	if(p->GetSounds().empty())
		return false;

	vector<SNDOneBuffer>& sb=p->GetBuffer();

	for(int i=0;i<sb.size();i++)
	{
		SNDOneBuffer* p=&sb[i];
		if(!p->used)
		{
			nfree=i;
			break;
		}

		if(p->pSound==NULL && p->pause_level==0)
		{
			if(p->p3DBuffer)
			{
				if(!p->p3DBuffer->IsPlaying())
				{
					nfree=i;
					break;
				}
			}else
			{
				DWORD status;
				if(p->buffer->GetStatus(&status)==DS_OK)
				{
					if((status&DSBSTATUS_PLAYING)==0)
					{
						nfree=i;
						break;
					}
				}
			}
		}
	}

	if(0){
		int played=0,real_played=0;
		for(int i=0;i<sb.size();i++)
		{
			SNDOneBuffer* p=&sb[i];
			if(p->pSound==NULL && p->pause_level==0)
			{
				if(p->p3DBuffer)
				{
					if(p->p3DBuffer->IsPlaying())
						played++;
				}

				DWORD status;
				if(p->buffer->GetStatus(&status)==DS_OK)
				{
					if(status&DSBSTATUS_PLAYING)
					{
						real_played++;
					}
				}
			}
		}
		logs("name=%s size=%i, played=%i, real=%i\n",p->sound_name,sb.size(),played,real_played);
	}

	if(nfree<0)
	{
		if(b2d && sb.size()>=p->max_num_sound)
			return false;
		if(!b2d && p->max_num_sound==1 && sb.size()>0)
			return false;

		SNDOneBuffer s;
		s.script=p;

		nfree=sb.size();
		sb.push_back(s);
	}

	if(nfree<0)
		return false;

	SNDOneBuffer& s=sb[nfree];
	s.used=true;
	s.pSound=NULL;
	s.script=p;

	script=p;

	if(s.buffer==NULL || p->GetSounds().size()>1)
	{
		HRESULT hr;
		
		int n = 0;

		if(p->belligerent_dependency){
			n = p->belligerentIndex(belligerentIndex);
			int size = (belligerentIndex < soundScriptTable().belligerentPrefix.size() - 1) ? p->belligerentIndex(belligerentIndex + 1) - n : p->GetSounds().size() - n;

			if(size)
				n += (rand()%size);
		}
		else
			n = rand()%p->GetSounds().size();

		xassert(n >= 0 && n < p->GetSounds().size());
		if(n < 0) n = 0;
		if(n >= p->GetSounds().size()) n = p->GetSounds().size() - 1;

		SAFE_RELEASE(s.buffer);

		hr=g_pDS->DuplicateSoundBuffer(p->GetSounds()[n],&s.buffer);
		if(FAILED(hr))
			return false;

		WAVEFORMATEX wfxFormat;
		DWORD dwSizeWritten=0;
		memset(&wfxFormat,0,sizeof(wfxFormat));
		hr=s.buffer->GetFormat(&wfxFormat,sizeof(wfxFormat),&dwSizeWritten);
		if(FAILED(hr))
		{
			SAFE_RELEASE(s.buffer);
			return false;
		}
		s.nSamplesPerSec=wfxFormat.nSamplesPerSec;
		s.nAvgBytesPerSec=wfxFormat.nAvgBytesPerSec;

		if(!b2d)
		{
			s.p3DBuffer=new SoftSound3D;

			if(!s.p3DBuffer->Init(s.buffer))
				return false;
		}

	}

	return true;
}

bool SND3DPlaySound(LPCSTR name,
					const Vect3f* pos,
					const Vect3f* velocity//По умолчанию объект считается неподвижным
					)
{
	if(!g_enable_sound)
		return true;
	if(pos==NULL)return false;
	
	ScriptParam* script;
	int nfree;

	bool b=script3d.FindFree(name,script,nfree);
	if(!b)
		return false;

	MTAuto lock(script->GetLock());

	HRESULT hr;
	bool ok;
	SNDOneBuffer& s=script->GetBuffer()[nfree];
	s.pos=*pos;

	if(!s.p3DBuffer->SetMinDistance(script->radius))
		goto Fail;
	if(!s.p3DBuffer->SetMaxDistance(script->max_radius))
		goto Fail;

	if(velocity)
		s.velocity=*velocity;
	else
		s.velocity.x=s.velocity.y=s.velocity.z=0;

	s.volume=script->def_volume;;

	s.PlayPreprocessing();

	hr=s.RecalculatePos();
	if(FAILED(hr))goto Fail;
	s.RecalculateVolume();

	RestoreBuffer(s.buffer);

	s.begin_play_time=clockf();
	ok=s.p3DBuffer->Play(false);
	s.played_cycled=false;
	if(!ok)goto Fail;

	return true;
Fail:
/*
	if(hr==DSERR_BUFFERLOST)
	{
		int k=0;
	}
	if(hr==DSERR_INVALIDCALL)
	{
		int k=0;
	}
	if(hr==DSERR_INVALIDPARAM)
	{
		int k=0;
	}
	if(hr==DSERR_PRIOLEVELNEEDED)
	{
		int k=0;
	}
*/
	s.used=false;
	return false;
}

/////////////////////////2D//////////////////////////
bool SND2DPlaySound(LPCSTR name,float x,DWORD frequency)
{
	if(!g_enable_sound || !name)
		return false;
	ScriptParam* script;
	int nfree;

	bool b=script2d.FindFree(name,script,nfree);
	if(!b || (!g_enable_voices && script->language_dependency)) return false;

	MTAuto lock(script->GetLock());

	HRESULT hr;
	SNDOneBuffer& s=script->GetBuffer()[nfree];
	s.pos.x=x;
	s.volume=script->def_volume;
	RestoreBuffer(s.buffer);

	s.RecalculateVolume();

	hr=s.buffer->SetFrequency(frequency>=0?frequency:0);
	if(FAILED(hr))goto Fail;

	s.PlayPreprocessing();

	hr=s.buffer->SetPan(PanByX(x));
	if(FAILED(hr))goto Fail;
	hr=s.buffer->SetCurrentPosition(0);
	if(FAILED(hr))goto Fail;

	s.begin_play_time=clockf();
	hr=s.buffer->Play(0,0,0);
	s.played_cycled=false;
	if(FAILED(hr))goto Fail;

	return true;
Fail:
	s.used=false;
	return false;
}

SND2DSound::SND2DSound()
{
	script=NULL;
	cur_buffer=-1;
}

SND2DSound::~SND2DSound()
{
	Destroy();
}

bool SND2DSound::Init(LPCSTR name)
{
	Destroy();
	if(!g_enable_sound)return false;
	bool b=script2d.FindFree(name,script,cur_buffer);
	if(!b)return false;

	MTAuto lock(script->GetLock());

	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
	s.pSound=this;

	s.pos.x=s.pos.y=s.pos.z=0;
	s.velocity.x=s.velocity.y=s.velocity.z=0;
	s.volume=script->def_volume;
	return true;
}

bool SND2DSound::Play(bool cycled)
{
	if(script==NULL || !g_enable_sound || (!g_enable_voices && script->language_dependency))return false;
	HRESULT hr;

	MTAuto lock(script->GetLock());
	AssertValid();
	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
	s.PlayPreprocessing();

	s.RecalculateVolume();

	LPDIRECTSOUNDBUFFER buffer=s.buffer;
	hr=buffer->SetCurrentPosition(0);
  	hr=buffer->Play(0,0,cycled?DSBPLAY_LOOPING:0);
	s.begin_play_time=clockf();
	s.played_cycled=cycled;
	return !FAILED(hr);
}

bool SND2DSound::Stop()
{
	if(script==NULL || !g_enable_sound)return true;
	MTAuto lock(script->GetLock());
	AssertValid();
	return !FAILED(script->GetBuffer()[cur_buffer].buffer->Stop());
}

bool SND2DSound::IsPlayed() const
{
	if(script==NULL)return false;
	MTAuto lock(script->GetLock());

	LPDIRECTSOUNDBUFFER buffer=script->GetBuffer()[cur_buffer].buffer;
	if(buffer==NULL)return false;
	DWORD status;
	if(buffer->GetStatus(&status)==DS_OK)
	{
		if(status&DSBSTATUS_PLAYING)
			return true;
	}
	return false;
}

bool SND2DSound::SetPos(float x)
{
	if(!g_enable_sound || script==NULL)return false;
	MTAuto lock(script->GetLock());

	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
	HRESULT hr;
	hr=s.buffer->SetPan(PanByX(x));
	return SUCCEEDED(hr);
}

bool SND2DSound::SetFrequency(float frequency)
{
	if(!g_enable_sound || script==NULL)return false;
	MTAuto lock(script->GetLock());

	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];

	return s.SetFrequency(frequency);
}

bool SND2DSound::SetRealFrequency(DWORD frequency)
{
	if(!g_enable_sound || script==NULL)return false;
	MTAuto lock(script->GetLock());

	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
	HRESULT hr;
	hr=s.buffer->SetFrequency(frequency);
	return SUCCEEDED(hr);
}

void SND2DSound::SetVolume(float vol)
{
	if(!g_enable_sound || script==NULL)return;
	float v=vol*(script->def_volume-script->min_volume)+script->min_volume;
	SetRealVolume(v);
}

void SND2DSound::SetRealVolume(float vol)
{
	if(!g_enable_sound || script==NULL)return;
	MTAuto lock(script->GetLock());

	AssertValid();
	SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
	if(vol>1)vol=1;
	if(vol<0)vol=0;
	s.volume=vol;
	s.RecalculateVolume();
}

void SND2DSound::AssertValid()
{
	ASSERT(script);
	ASSERT(cur_buffer>=0 && cur_buffer<script->GetBuffer().size());
}

void SND2DSound::Destroy()
{
	if(g_pDS && script && cur_buffer>=0)
	{
		Stop();
		MTAuto lock(script->GetLock());

		SNDOneBuffer& s=script->GetBuffer()[cur_buffer];
		ASSERT(s.pSound==this);
		s.used=false;
		s.pSound=NULL;
	}
}


void SNDStopAll()
{
	script3d.StopAllPlayed();
	script2d.StopAllPlayed();
}
////////////////////////Pause///////////////////////////

void SNDPausePush()
{
	pause_level++;

	script3d.PauseAllPlayed(pause_level);
	script2d.PauseAllPlayed(pause_level);
}

void SNDPausePop()
{
	if(pause_level==0)return;

	script3d.PlayByLevel(pause_level);
	script2d.PlayByLevel(pause_level);
	pause_level--;
}

int SNDGetPushLevel()
{
	return pause_level;
}

