#pragma once

#ifndef _CONTROLLER_HPP
#define _CONTROLLER_HPP

#include "Frame.hpp"

class Controller {
	public:
		enum WaitEventType {
			CONTROL_SUBMIT_EVENT,
			END_OF_AUDIO_EVENT,
			END_OF_VIDEO_EVENT,
			END_OF_CAMERA_EVENT,
			BEGIN_OF_CAMERA_EVENT,
			POINT_OF_CAMERA_EVENT,
			MISSION_VICTORY_EVENT
		};

		virtual void waitFor(WaitEventType event) = 0;
		virtual void stopFrame(const Frame* frame) = 0;
		virtual void nomadToFrame(Frame* frame) = 0;
		virtual void setYearDuration(float duration) = 0;
		virtual void eventOccured(WaitEventType event) = 0;
		virtual void setDelay(float delay) = 0;
		virtual void breakSkipping() = 0;

		virtual void executeFakeMission() = 0;

		virtual void rollback() = 0;

		virtual void setNormalSpeedMode(bool normal) = 0;
		virtual bool isNormalSpeedMode() const = 0;

		virtual Frame* getNomadFrame() = 0;

		//returns true if reinterperted from begin
		virtual bool goToYear(int year) = 0;
		virtual bool goToMission(int number) = 0;
		virtual int getCurrentYear() const = 0;
};

#endif //_CONTROLLER_HPP