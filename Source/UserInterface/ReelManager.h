#pragma once

#ifndef _REELMANAGER_H
#define _REELMANAGER_H

#include "VideoPlayer.h"

class ReelManager {
	public:
		enum SizeType {
			CUSTOM,
			REAL_SIZE,
			FULL_SCREEN
		};
		ReelManager();
		~ReelManager();
		void showModal(const char* videoFileName, const char* soundFileName, bool stopBGMusic = true, int alpha = 255);
		void showPictureModal(const char* pictureFileName, int stableTime);
		bool isVisible() const {
			return visible;
		}
		void hide() {
			visible = false;
		}
		Vect2i pos;
		Vect2i size;
		SizeType sizeType;
	protected:
		bool visible;
		VideoPlayer* player;
		cTexture* bgTexture;
		double startTime;
};

#endif //_REELMANAGER_H
