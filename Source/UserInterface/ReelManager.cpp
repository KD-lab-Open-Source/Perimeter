#include "StdAfx.h"
#include "ReelManager.h"
#include "Runtime.h"
#include "AudioPlayer.h"
#include "GameShell.h"
#include "GameShellSq.h"

extern MusicPlayer gb_Music;
//extern GameShell* gameShell;

ReelManager::ReelManager() {
	visible = false;
	pos.set(0, 0);
	size.set(0, 0);
	sizeType = FULL_SCREEN;
	bgTexture = NULL;
}

ReelManager::~ReelManager() {
	RELEASE(bgTexture);
}

void ReelManager::showModal(const char* binkFileName, const char* soundFileName, bool stopBGMusic, int alpha) {
	bink = PlayBink::Create();
	if(!bink->Init(binkFileName))
	{
		delete bink;
		return;
	}
	if (stopBGMusic) {
		gb_Music.Stop();
//		bink->SetVolume(max(terMusicVolume, terSoundVolume));
//	} else {
//		bink->SetVolume(0);
	}
	bink->SetVolume(max(terMusicVolume, terSoundVolume));

	std::string soundPath = soundFileName ? soundFileName : "";

	if (!soundPath.empty() && soundPath != "empty" && stopBGMusic) {
		int ret = gb_Music.OpenToPlay(soundFileName, true);
		xassert(ret);
	}

	Vect2i showSize;
	Vect2i showPos;
	int screenWidth = terRenderDevice->GetSizeX();
	int screenHeight = terRenderDevice->GetSizeY();
	switch (sizeType) {
		case REAL_SIZE:
			showPos = pos;
			showSize = bink->GetSize();
			bgTexture = gb_VisGeneric->CreateTextureScreen();
			if (!bgTexture)	{
				xassert_s(0, "Cannot create background texture to play bink file");
				return;
			}
			break;
		case FULL_SCREEN:
			{
				showSize = bink->GetSize();
				if (alpha < 255) {
					bgTexture = gb_VisGeneric->CreateTextureScreen();
					if (!bgTexture)	{
						xassert_s(0, "Cannot create background texture to play bink file");
						return;
					}
				}
				float realRatio = showSize.x / (float) showSize.y;
				float screenRatio = screenWidth / (float) screenHeight;
				if (realRatio >= screenRatio) {
					showSize.x = screenWidth;
					showSize.y = screenWidth / realRatio;
					showPos.set(0, int((screenHeight - showSize.y) / 2.0f));
				} else {
					showSize.y = screenHeight;
					showSize.x = screenHeight * realRatio;
					showPos.set(int((screenWidth - showSize.x) / 2.0f), 0);
				}
			}
			break;
		default:
			showPos = pos;
			showSize = size;
			bgTexture = gb_VisGeneric->CreateTextureScreen();
			if (!bgTexture)	{
				xassert_s(0, "Cannot create background texture to play bink file");
				return;
			}
	}

	visible = true;
	while (isVisible() && !bink->IsEnd()) {
#ifndef PERIMETER_EXODUS
        //TODO is this necessary under SDL2 in Win32? maybe we should poll SDL2 events?
        MSG msg;
		if ( PeekMessage(&msg, 0, 0, 0, PM_REMOVE) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			continue;
		} 
#endif
        if ( bink->CalcNextFrame() ) {
            terRenderDevice->Fill(0, 0, 0, 0);
            terRenderDevice->BeginScene();
            if (bgTexture) {
                terRenderDevice->DrawSprite(0, 0, screenWidth, screenHeight, 0, 0,
                                            screenWidth / (float)bgTexture->GetWidth(),
                                            screenHeight / (float)bgTexture->GetHeight(),
                                            bgTexture);
            }

            bink->Draw(showPos.x, showPos.y, showSize.x, showSize.y, alpha);

            terRenderDevice->EndScene();
            terRenderDevice->Flush();
        }
	}

	terRenderDevice->Fill(0, 0, 0, 0);
	terRenderDevice->BeginScene();
	terRenderDevice->EndScene();
	terRenderDevice->Flush();

	delete bink;
	bink = NULL;
	RELEASE(bgTexture);

	if (soundFileName && strlen(soundFileName) && stopBGMusic) {
		gb_Music.Stop();
	}
	hide();
}

void ReelManager::showPictureModal(const char* pictureFileName, int stableTime) {

	cTexture* pictureTexture = terVisGeneric->CreateTexture( pictureFileName );

	int screenWidth = terRenderDevice->GetSizeX();
	int screenHeight = terRenderDevice->GetSizeY();
/*
	bgTexture = gb_VisGeneric->CreateTextureScreen();
	if (!bgTexture)	{
		xassert_s(0, "Cannot create background texture to play bink file");
		return;
	}
*/
	startTime = clockf();

	float maxTime = SPLASH_FADE_IN_TIME + stableTime + SPLASH_FADE_OUT_TIME;
	visible = true;
	while (isVisible()) {
#ifndef PERIMETER_EXODUS
        MSG msg;
		if ( PeekMessage(&msg, 0, 0, 0, PM_REMOVE) ) {
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			continue;
		}
#endif
        double timeElapsed = clockf() - startTime;
        if (timeElapsed > SPLASH_REEL_ABORT_DISABLED_TIME) {
            gameShell->reelAbortEnabled = true;
        }
        if ( stableTime < 0 || timeElapsed < maxTime ) {
            terRenderDevice->Fill(0, 0, 0, 0);
            terRenderDevice->BeginScene();
/*
            if (bgTexture) {
                terRenderDevice->DrawSprite(0, 0, screenWidth, screenHeight, 0, 0,
                                            screenWidth / (float)bgTexture->GetWidth(),
                                            screenHeight / (float)bgTexture->GetHeight(),
                                            bgTexture);
            }
*/
            int alpha = 0;
            if (timeElapsed <= SPLASH_FADE_IN_TIME) {
                alpha = xm::round(255.0f * timeElapsed / SPLASH_FADE_IN_TIME);
            } else if (stableTime < 0 || timeElapsed <= (SPLASH_FADE_IN_TIME + stableTime)) {
                alpha = 255;
            } else {
                alpha = xm::round(255.0f * (maxTime - timeElapsed) / SPLASH_FADE_OUT_TIME);
            }
            float dy = screenHeight / (float)screenWidth;
            terRenderDevice->DrawSprite(0, 0, screenWidth, screenHeight, 0, 0,
                                        1, dy, pictureTexture, sColor4c(255, 255, 255, alpha));

            terRenderDevice->EndScene();
            terRenderDevice->Flush();
        } else {
            break;
        }
	}
	terRenderDevice->Fill(0, 0, 0, 0);
	terRenderDevice->BeginScene();
	terRenderDevice->EndScene();
	terRenderDevice->Flush();

	RELEASE(pictureTexture);
//	RELEASE(bgTexture);

	hide();
}
