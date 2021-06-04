#include "StdAfx.h"
#include "PerimeterSound.h"
#include "SoundInternal.h"
#include "C3D.h"

using namespace SND;
bool VirtualSound3D::Init(LPDIRECTSOUNDBUFFER ptr)
{
	pSound=ptr;
	return true;
}
/*
static void dprintf(char *format, ...)
{
	va_list args;
	char    buffer[512];

	va_start(args,format);
	vsprintf(buffer,format,args);
	OutputDebugString(buffer);
}
*/

//////////////////////SoftSound3D//////////////////////////
const float SOUND_CONSTANT = 343.0;
const float FREQUENCY_SCALE = ( DSBFREQUENCY_MAX - DSBFREQUENCY_MIN );

SoftSound3D::SoftSound3D()
{
	pause=false;
	is_cycled=false;

	position.x=position.y=position.z=0;
	velocity.x=velocity.y=velocity.z=0;

	min_distance=1;
	clip_distance=max_distance=1e6;
	is_playing=false;
	volume=1.0f;
	begin_play_sound=0;
	last_start_stop=0;

	BytePerSample=2;
	set_volume=1.0f;
}

SoftSound3D::~SoftSound3D()
{
}

bool SoftSound3D::Init(LPDIRECTSOUNDBUFFER ptr)
{
	if(!VirtualSound3D::Init(ptr))
		return false;

	DSBCAPS caps;
	caps.dwSize=sizeof(caps);
	if(pSound->GetCaps(&caps)!=DS_OK)
		return false;
	dwBufferBytes=caps.dwBufferBytes;

	WAVEFORMATEX wfxFormat;
	DWORD dwSizeWritten=0;
	memset(&wfxFormat,0,sizeof(wfxFormat));
	if(pSound->GetFormat(&wfxFormat,sizeof(wfxFormat),&dwSizeWritten)!=DS_OK)
	{
		return false;
	}

	nSamplesPerSec=wfxFormat.nSamplesPerSec;
	nAvgBytesPerSec=wfxFormat.nAvgBytesPerSec;

	BytePerSample=wfxFormat.nAvgBytesPerSec/wfxFormat.nSamplesPerSec;
	RealFrequency=nSamplesPerSec;

	return true;
}

float SoftSound3D::GetMaxDistance()
{
	return max_distance;
}

float SoftSound3D::GetMinDistance()
{
	return min_distance;
}

bool SoftSound3D::SetMaxDistance(float r)
{
	clip_distance=max_distance=r;
	return true;
}

bool SoftSound3D::SetMinDistance(float r)
{
	min_distance=r;
	return true;
}

Vect3f SoftSound3D::GetPosition()
{
	return position;
}

bool SoftSound3D::SetPosition(const Vect3f& _pos)
{
	position=_pos;
	return true;
}

Vect3f SoftSound3D::GetVelocity()
{
	return velocity;
}

bool SoftSound3D::SetVelocity(const Vect3f& _vel)
{
	velocity=_vel;
	return true;
}

bool SoftSound3D::IsPlaying()
{
	if(is_cycled)
		return is_playing;
	else
	{
		DWORD curpos=GetCurPos(clockf());
		if(!(is_playing && curpos<dwBufferBytes))
		{
			int k=0;
		}

		return is_playing && curpos<dwBufferBytes;
	}
}

bool SoftSound3D::Play(bool cycled)
{
	is_playing=true;
	is_cycled=cycled;

	begin_play_sound=clockf();
	last_start_stop=begin_play_sound-10;

//	if(!is_cycled)dprintf("Play\n");

	RecalculatePos();
/*
	HRESULT hr;
	hr=pSound->SetCurrentPosition(0);
	hr=pSound->Play(0,0,cycled?DSBPLAY_LOOPING:0);
	last_start_stop=begin_play_sound=clockf();
	OutputDebugString("Play\n");
	is_playing=true;
*/
	//SUCCEEDED(hr);
	return true;
}

bool SoftSound3D::Stop()
{
	HRESULT hr;
	hr=pSound->Stop();
	is_playing=false;
//	if(!is_cycled)dprintf("Stop\n");
	//SUCCEEDED(hr);
	return true;
}

bool SoftSound3D::SetFrequency(DWORD dwFrequency)
{
	if(dwFrequency==0)
		RealFrequency=nSamplesPerSec;
	else
		RealFrequency=dwFrequency;
	HRESULT hr;
	hr=pSound->SetFrequency(dwFrequency);
	return true;
}

bool SoftSound3D::SetVolume(float vol)
{
	if(vol>=1)vol=1;
	if(vol<0)vol=0;
	set_volume=vol;
	return true;
}

Vect3f SoftSound3D::VectorToListener()
{
	Vect3f pos=position-snd_listener.position;
	pos.z*=snd_listener.zmultiple;
	pos=snd_listener.rotate*pos;
	return pos;
}

void SoftSound3D::RecalculatePos()
{
	HRESULT hr;
	//Volume
	Vect3f pos=position-snd_listener.position;
	pos.z*=snd_listener.zmultiple;
	pos=snd_listener.rotate*pos;

	float flDistSqrd=pos.norm2();

	float full_volume;
	if(flDistSqrd>sqr(clip_distance) || pause)
	{
		full_volume = 0.0f;
		pSound->SetVolume(DSBVOLUME_MIN);
	}else
	{
		float volume_scale;
		float flDist = sqrtf( flDistSqrd );
		if( flDist <= min_distance )
			volume_scale = 1.0;
		else if( flDist >= max_distance )
			volume_scale = 0.0;
		else
		{
			volume_scale = (1/flDist-1/max_distance)/(1/min_distance-1/max_distance);
			volume_scale *= snd_listener.s_rolloff_factor;
		}

		Vect3f vRelPosNormalized;
		
		if(snd_listener.zmultiple>=1)
		{
			vRelPosNormalized=pos;
			vRelPosNormalized*=1/flDist;
		}else
		{
			vRelPosNormalized=snd_listener.rotate*(position-snd_listener.position);
			vRelPosNormalized.Normalize();
		}
			
		float flFrontScale = - snd_listener.front.dot(vRelPosNormalized);
		if( flFrontScale < 0.0f )
			flFrontScale = 1.0f;
		else
			flFrontScale = 1.0f - 0.25f * flFrontScale;

		full_volume=volume_scale*volume*flFrontScale*set_volume;

		long ds_volume=ToDirectVolumef(clamp(full_volume,0.f,1.0f));
		pSound->SetVolume(ds_volume);

		{
			//Pan
			float pan_scale= - snd_listener.right.dot(vRelPosNormalized);
			const int mulpan=3900;
			int lPan = round( 2*mulpan * pan_scale );
			lPan = clamp( lPan, -mulpan,mulpan);
			pSound->SetPan(lPan );
		}
	}

	//Frequency
/*
	if(snd_listener.s_doppler_factor!=0)
	{
		float flVelSound = vRelPosNormalized.dot(velocity);
		float flVelListener = -vRelPosNormalized.dot(snd_listener.velocity);

		// Check if there is no doppler shift...
		if( -0.001 < flVelSound && flVelSound < 0.001 && 
			-0.001 < flVelListener && flVelListener < 0.001 )
		{
			pSound->SetFrequency(RealFrequency);
		}else
		{
			float flFreqAdjust, flAbsFreqAdjust;
			float flTemp;

			// Get doppler shifted frequency. Formula is:  Fr = Fs( C + Ur ) / ( C - Us ), 
			// where Fr = Frequency to receiver, Fs = Frequency at source, C = speed of
			// sound in medium, Ur = Speed of receiver in direction of source, Us = Speed
			// of source in direction of receiver...
			flTemp = SOUND_CONSTANT * snd_listener.s_doppler_factor / snd_listener.s_distance_factor;
			long lFrequency =  ( LONG )((( flTemp + flVelListener ) / ( flTemp - flVelSound )) * RealFrequency);
			if( lFrequency <= DSBFREQUENCY_MIN )
				lFrequency = DSBFREQUENCY_MIN + 1;
			if( lFrequency >= DSBFREQUENCY_MAX )
				lFrequency = DSBFREQUENCY_MAX - 1;
			flTemp = ( float )RealFrequency;
			flFreqAdjust = ( float )( lFrequency - flTemp ) / flTemp;
			flAbsFreqAdjust = ( float )fabs( flFreqAdjust );


			if( flAbsFreqAdjust > 2.0 )
				flAbsFreqAdjust = 2.0;

			if( flAbsFreqAdjust > 0.001 )//&& flAbsFreqAdjust * 100.0 > g_pSoundMgr->m_nTolerance )
			{
				pSound->SetFrequency( lFrequency );
			}
		}

	}
/**/

	double curtime=clockf();

	if(is_playing)
	if(last_start_stop+0.2f<curtime)
	{//Start/stop section
		DWORD status;
		bool is_playing_real=false;
		hr=pSound->GetStatus(&status);
		if(hr==DS_OK)
		{
			if(status&DSBSTATUS_PLAYING)
				is_playing_real=true;
		}
/*
		if(is_playing_real)
		{
			DWORD dwCurrentPlayCursor;
			if(pSound->GetCurrentPosition(&dwCurrentPlayCursor,NULL)==DS_OK)
			{
				begin_play_sound=curtime-dwCurrentPlayCursor/(double)nAvgBytesPerSec;
			}
		}
*/
		if(full_volume==0.0f && is_playing_real)
		{//Stop
			last_start_stop=curtime;
			hr=pSound->Stop();
		}

		DWORD curpos=GetCurPos(curtime);

		if(full_volume!=0.0f && !is_playing_real)
		{//Start
			last_start_stop=curtime;

			bool bplay=false;

			if(is_cycled)
			{
				curpos=curpos%dwBufferBytes;
				bplay=true;
			}else
			{
				bplay=curpos<dwBufferBytes;
			}

			if(bplay)
			{
				hr=pSound->SetCurrentPosition(curpos);
				hr=pSound->Play(0,0,is_cycled?DSBPLAY_LOOPING:0);
			}
		}

		if(!is_cycled)
		{
			int k=0;
		}

		if(!is_cycled && curpos>=dwBufferBytes)
		{
//			dprintf("Stop of end\n");
			hr=pSound->Stop();
			is_playing=false;
		}
	}
}

DWORD SoftSound3D::GetCurPos(double curtime)
{
	DWORD curpos=(DWORD((curtime-begin_play_sound)*1e-3f*RealFrequency*BytePerSample));
	return curpos;
}

void SoftSound3D::RecalculateVolume()
{
	RecalculatePos();
}
//////////////////////info//////////////////////
/*
	Предположения о том, как работает 3D Sound

	Vect3f front,top,right,pos;//Местоположение слушателя
	Vect3f center;//Местоположение звука
	Vect3f to=center-pos;
	float dist=length(center-pos);

----Rolloff - Скорость затухания звука с расстоянием 
	1.0 - стандартное, 0.0 - нет затухания, 10.0 - максимальное
	Формула по идее встроенная в Listener
	
	flScale = 1.0f - (( sqrt(dist) - pSound->GetMinDist()) / ( pSound0->GetMaxDist() - pSound->GetMinDist()));
	flScale *= g_pSoundMgr->m_fRolloffFactor;
	flScale - умножается на громкость звука, 
	dist - расстояние от звука до слушателя

----doppler - Эффект доплера.
	// Get doppler shifted frequency. Formula is:  Fr = Fs( C + Ur ) / ( C - Us ), 
	// where Fr = Frequency to receiver, Fs = Frequency at source, C = speed of
	// sound in medium, Ur = Speed of receiver in direction of source, Us = Speed
	// of source in direction of receiver...
	flTemp = SOUND_CONSTANT * g_pSoundMgr->m_fDopplerFactor / g_pSoundMgr->m_fDistanceFactor;
	lFrequency =  ( LONG )((( flTemp + flVelListener ) / ( flTemp - flVelSound )) * pInstance->GetOrigFrequency( ));
	if( lFrequency <= DSBFREQUENCY_MIN )
		lFrequency = DSBFREQUENCY_MIN + 1;
	if( lFrequency >= DSBFREQUENCY_MAX )
		lFrequency = DSBFREQUENCY_MAX - 1;
	flTemp = ( float )pInstance->GetFrequency( );
	flFreqAdjust = ( float )( lFrequency - flTemp ) / flTemp;
	flAbsFreqAdjust = ( float )fabs( flFreqAdjust );


	if( flAbsFreqAdjust > 2.0 )
		flAbsFreqAdjust = 2.0;

----Pan
	// Get pan scale factor...
	flScale = -(right*normalize(to));

	lPan = ( LONG )( 100.0 * flScale );

	// Always play a little in the other side...
	lPan = CLAMP( lPan, ( LONG )-50, ( LONG )50 );


*/
