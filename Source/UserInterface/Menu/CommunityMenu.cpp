#include "StdAfx.h"
#include "MainMenu.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include <SDL.h>

std::string openWebURL;

void onMMCommunityButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_COMMUNITY_SCR);
    }
}

int openWebConfirmationHandler(float, float) {
    hideMessageBox();
    if (!openWebURL.empty()) {
        fprintf(stdout, "Open web: %s\n", openWebURL.c_str());
        if (SDL_OpenURL(openWebURL.c_str())) {
            SDL_PRINT_ERROR("SDL_OpenURL");
        }
        openWebURL.clear();
    }
    return 0;
}

void onMMOpenLinkButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        openWebURL.clear();
        bool addLocale = false;
        bool russian = startsWith(getLocale(), "russian");
        std::string params;
        switch (pWnd->ID) {
            default:
            case SQSH_MM_COMMUNITY_1_BTN:
                openWebURL = "https://kdlab.com/community";
                addLocale = true;
                params = "ref=perimeter";
                break;
            case SQSH_MM_COMMUNITY_2_BTN:
                openWebURL = "https://kdlab.com";
                //Locale is auto-detected by web
                break;
            case SQSH_MM_COMMUNITY_3_BTN:
                openWebURL = "https://kdlab.com/association";
                addLocale = true;
                break;
            case SQSH_MM_COMMUNITY_4_BTN:
                openWebURL = "https://github.com/KD-lab-Open-Source/Perimeter";
                break;
            case SQSH_MM_ADDONS_GUIDE_BTN:
                //TODO change when/if is translated
                openWebURL = "https://github.com/KD-lab-Open-Source/Perimeter/blob/cmake/MODDING.eng.md";
                break;
        }
        if (openWebURL.empty()) {
            return;
        }
        if (addLocale && !russian) {
            //For now there is only russian (without any extra url and /en for english)
            openWebURL += "/en";
        }
        if (!params.empty()) {
            openWebURL += "?" + params;
        }
        
        std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.OpenWeb");
        text += "\n\n" + BreakLongLines(openWebURL.c_str(), 30);
        setupYesNoMessageBox(openWebConfirmationHandler, 0, text);
        showMessageBox();
    }
}

