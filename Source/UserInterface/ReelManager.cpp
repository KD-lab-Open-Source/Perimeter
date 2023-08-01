#include "StdAfx.h"
#include "ReelManager.h"
#include "Runtime.h"
#include "AudioPlayer.h"
#include "GameShell.h"
#include "GameShellSq.h"
#include "CameraManager.h"

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

void ReelManager::showModal(const char* videoFileName, const char* soundFileName, bool stopBGMusic, int alpha) {
	player = new VideoPlayer();
	if(!player->Init(videoFileName))
	{
		delete player;
		return;
	}
    //Not required to set volume as its globally set from terSoundVolume
	if (stopBGMusic) {
		gb_Music.Stop();
//		player->SetVolume(max(terMusicVolume, terSoundVolume));
//	} else {
//		player->SetVolume(0);
	}
	//player->SetVolume(max(terMusicVolume, terSoundVolume));

	std::string soundPath = soundFileName ? soundFileName : "";

	if (!soundPath.empty() && soundPath != "empty" && stopBGMusic) {
		int ret = gb_Music.OpenToPlay(soundFileName, true);
        if (!ret) {
            fprintf(stderr, "showModal error opening sound %s\n", soundFileName);
        }
	}

    Vect2i picSize;
	Vect2i showPos;
	switch (sizeType) {
		case REAL_SIZE:
			showPos = pos;
			player->getSize(picSize);
			bgTexture = gb_VisGeneric->CreateTextureScreen();
			if (!bgTexture)	{
				xassert_s(0, "Cannot create background texture to play video file");
				return;
			}
			break;
		case FULL_SCREEN:
			{
                showPos = pos;
				player->getSize(picSize);
				if (alpha < 255) {
					bgTexture = gb_VisGeneric->CreateTextureScreen();
					if (!bgTexture)	{
						xassert_s(0, "Cannot create background texture to play video file");
						return;
					}
				}
			}
			break;
		default:
			showPos = pos;
            picSize = size;
			bgTexture = gb_VisGeneric->CreateTextureScreen();
			if (!bgTexture)	{
				xassert_s(0, "Cannot create background texture to play video file");
				return;
			}
	}

    terRenderDevice->SetDrawNode(terCamera->GetCamera());
	visible = true;
	while (isVisible() && !player->IsEnd()) {
        app_event_poll();
        
        if (player->Update() ) {
            Vect2f showSize = picSize;
            float screenWidth = static_cast<float>(terRenderDevice->GetSizeX());
            float screenHeight = static_cast<float>(terRenderDevice->GetSizeY());
            if (sizeType == FULL_SCREEN) {
                float sx = screenWidth / showSize.x;
                float sy = screenHeight / showSize.y;
                float scale = min(sx, sy);
                showSize = picSize * scale;
                if (sx < sy) {
                    showPos.set(0, static_cast<int>((screenHeight - showSize.y) / 2.0f));
                } else {
                    showPos.set(static_cast<int>((screenWidth - showSize.x) / 2.0f), 0);
                }
            }
            
            terRenderDevice->Fill(0, 0, 0, 0);
            terRenderDevice->BeginScene();
            if (bgTexture) {
                terRenderDevice->DrawSprite(0, 0, screenWidth, screenHeight, 0, 0,
                                            screenWidth / static_cast<float>(bgTexture->GetWidth()),
                                            screenHeight / static_cast<float>(bgTexture->GetHeight()),
                                            bgTexture);
            }

            player->Draw(showPos.x, showPos.y, static_cast<int>(showSize.x), static_cast<int>(showSize.y), alpha);

            terRenderDevice->EndScene();
            terRenderDevice->Flush();
        }
	}

	terRenderDevice->Fill(0, 0, 0, 0);
	terRenderDevice->BeginScene();
	terRenderDevice->EndScene();
	terRenderDevice->Flush();

	delete player;
    player = NULL;
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
		xassert_s(0, "Cannot create background texture to play video file");
		return;
	}
*/
	startTime = clockf();

	float maxTime = SPLASH_FADE_IN_TIME + stableTime + SPLASH_FADE_OUT_TIME;
	visible = true;
	while (isVisible()) {
        app_event_poll();
        
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
