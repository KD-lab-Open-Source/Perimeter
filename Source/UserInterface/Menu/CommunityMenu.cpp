#include "StdAfx.h"
#include "MainMenu.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include <SDL.h>

void onMMCommunityButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_COMMUNITY_SCR);
    }
}

void onMMCommunityLinkButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        std::string url;
        bool addLocale = false;
        std::string params;
        switch (pWnd->ID) {
            default:
            case SQSH_MM_COMMUNITY_1_BTN:
                url = "https://kdlab.com/community";
                addLocale = true;
                params = "ref=perimeter";
                break;
            case SQSH_MM_COMMUNITY_2_BTN:
                url = "https://kdlab.com";
                //Locale is auto detected by web
                break;
            case SQSH_MM_COMMUNITY_3_BTN:
                url = "https://kdlab.com/association";
                addLocale = true;
                break;
            case SQSH_MM_COMMUNITY_4_BTN:
                url = "https://github.com/KD-lab-Open-Source/Perimeter";
                break;
        }
        if (addLocale && !startsWith(getLocale(), "russian")) {
            //For now there is only russian (without any extra url and /en for english)
            url += "/en";
        }
        if (!params.empty()) {
            url += "?" + params;
        }
        if (!url.empty()) {
            if (SDL_OpenURL(url.c_str())) {
                SDL_PRINT_ERROR("SDL_OpenURL");
            }
        }
    }
}

