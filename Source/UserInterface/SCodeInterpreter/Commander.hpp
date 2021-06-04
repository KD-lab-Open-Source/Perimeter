#pragma once

#ifndef _COMMANDER_HPP
#define _COMMANDER_HPP

#include "HistorySceneCamera.h"
#include "Controller.hpp"

class Commander {
	public:
		virtual void startAudio(const string& name) = 0;
		virtual void startVideo(const string& reelName, const string& sndName) = 0;
		virtual void startInterVideo(const string& reelName) = 0;
		virtual void setMusic(const string& path) = 0;
		virtual void addCameraPosition(
				const Vect3f& pivotPosition,
				float psi,
				float theta,
				float distanceToPivot ) = 0; 

		virtual void waitFor(Controller::WaitEventType event) = 0;
		virtual void clearCameraPath() = 0;
		virtual void executeMission(const string& fileName) = 0;
		virtual void showText(const string& text, const string& icon) = 0;
		virtual void hideText() = 0;
		virtual void showPaused() = 0;
		virtual void resetCamera() = 0;
		virtual void cameraFollowNomad(float deltaPsi, float radius, float theta) = 0;
		virtual void finishHistory() = 0;
};

#endif //_COMMANDER_HPP