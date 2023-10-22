#include "StdAfxSound.h"
#include "PerimeterSound.h"
#include "SoundInternal.h"
#include "../Render/inc/RenderMT.h"
#include "Sample.h"
#include "C3D.h"
#include "SoftwareBuffer.h"


using namespace SND;
bool VirtualSound3D::Init(SND_Sample* ptr)
{
	pSound=ptr;
    pSound->external_looped_restart = true;
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

	set_volume=1.0f;
}

SoftSound3D::~SoftSound3D()
{
}

bool SoftSound3D::Init(SND_Sample* ptr)
{
    return VirtualSound3D::Init(ptr);
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
    return is_playing;
}

bool SoftSound3D::Play(bool cycled)
{
    is_cycled=pSound->looped = cycled;
	is_playing=pSound->play() != SND_NO_CHANNEL;

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
    is_playing=false;
    pause=false;
    return pSound->stop();
}

bool SoftSound3D::SetFrequency(float frequency) {
	pSound->frequency = frequency == 0 ? 1.0f : frequency;
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
	//Volume
	Vect3f pos=position-snd_listener.position;
	pos.z*=snd_listener.zmultiple;
	pos=snd_listener.rotate*pos;

	float flDistSqrd=pos.norm2();

	if (flDistSqrd>sqr(clip_distance) || pause) {
		pSound->volume = 0.0f;
	} else {
		float volume_scale;
		float flDist = xm::sqrt(flDistSqrd);
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

		float full_volume=volume_scale*volume*flFrontScale*set_volume;
		pSound->volume = clamp(full_volume,0.f,1.0f);

		{
			//Pan
			float pan_scale= - snd_listener.right.dot(vRelPosNormalized);
            //TODO review if this is correct compared to original DS code
            /*
            const int mulpan=3900;
            int lPan = xm::round( 2*mulpan * pan_scale );
            lPan = clamp( lPan, -mulpan,mulpan);
            pSound->SetPan(lPan );
            */
			pSound->pan = clamp(pan_scale + 0.5f, 0.0f, 1.0f);
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
			flAbsFreqAdjust = ( float )xm::abs( flFreqAdjust );


			if( flAbsFreqAdjust > 2.0 )
				flAbsFreqAdjust = 2.0;

			if( flAbsFreqAdjust > 0.001 )//&& flAbsFreqAdjust * 100.0 > g_pSoundMgr->m_nTolerance )
			{
				pSound->SetFrequency( lFrequency );
			}
		}

	}
/**/

	if(is_playing) {//Start/stop section
        bool is_playing_real=pSound->isPlaying();

/*      TODO commented originally, remove?
		if(is_playing_real)
		{
			DWORD dwCurrentPlayCursor;
			if(pSound->GetCurrentPosition(&dwCurrentPlayCursor,NULL)==DS_OK)
			{
				begin_play_sound=curtime-dwCurrentPlayCursor/(double)nAvgBytesPerSec;
			}
		}
*/
        
        /* TODO use this instead of pausing channel once seeking is impl in SDL_mixer
        if (full_volume == 0.0f && is_playing_real) {
            //Stop
            last_start_stop=curtime;
            pSound->stop();
            is_playing_real = false;
        } else {
            //Start
            is_playing_real = pSound->play() != SND_NO_CHANNEL;
        }
        */

        //Stop cycled ones that have no volume to free channel
        if (is_playing_real && is_cycled && pSound->volume <= SND::EFFECT_VOLUME_THRESHOLD) {
            pSound->stop();
            is_playing_real = false;
        }

        //Update any change of effects we may have made
        if (is_playing_real) {
            pSound->updateEffects();
        }

        //if(!is_cycled && curpos>=1.0f) //TODO once seeking is done use something like this
		if(!is_playing_real)
		{
            if (is_cycled) {
                //Sometimes cycled ones can be stopped when frequency changes or volume is zero
                if (SND::EFFECT_VOLUME_THRESHOLD <= pSound->volume) {
                    //Volume is back, play it
                    pSound->play();
                }
            } else {
                Stop();
            }
		}
	}
}

void SoftSound3D::RecalculateVolume()
{
	RecalculatePos();
}

void SoftSound3D::Pause(bool p) {
    pause = p;
    if (pause) {
        pSound->pause();
    } else {
        pSound->resume();
    }
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
	
	flScale = 1.0f - (( xm::sqrt(dist) - pSound->GetMinDist()) / ( pSound0->GetMaxDist() - pSound->GetMinDist()));
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
	flAbsFreqAdjust = ( float )xm::abs( flFreqAdjust );


	if( flAbsFreqAdjust > 2.0 )
		flAbsFreqAdjust = 2.0;

----Pan
	// Get pan scale factor...
	flScale = -(right*normalize(to));

	lPan = ( LONG )( 100.0 * flScale );

	// Always play a little in the other side...
	lPan = CLAMP( lPan, ( LONG )-50, ( LONG )50 );


*/
