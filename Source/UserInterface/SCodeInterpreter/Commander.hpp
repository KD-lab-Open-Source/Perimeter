#pragma once

#ifndef _COMMANDER_HPP
#define _COMMANDER_HPP

#include "HistorySceneCamera.h"
#include "Controller.hpp"

class Commander {
	public:
		virtual void startAudio(const std::string& name) = 0;
		virtual void startVideo(const std::string& reelName, const std::string& sndName) = 0;
		virtual void startInterVideo(const std::string& reelName) = 0;
		virtual void setMusic(const std::string& path) = 0;
		virtual void addCameraPosition(
				const Vect3f& pivotPosition,
				float psi,
				float theta,
				float distanceToPivot ) = 0; 

		virtual void waitFor(Controller::WaitEventType event) = 0;
		virtual void clearCameraPath() = 0;
		virtual void executeMission(const std::string& fileName) = 0;
		virtual void showText(const std::string& text, const std::string& icon) = 0;
		virtual void hideText() = 0;
		virtual void showPaused() = 0;
		virtual void resetCamera() = 0;
		virtual void cameraFollowNomad(float deltaPsi, float radius, float theta) = 0;
		virtual void finishHistory() = 0;
};

#endif //_COMMANDER_HPP