#include "StdAfx.h"
#include "MainMenu.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "files/files.h"
#include "GameContent.h"
#include <filesystem>

extern bool menuChangingDone;
bool pendingChanges = false;

struct ModInfo {
    ModMetadata* mod;
    bool wantedEnabled = false;
};

std::vector<ModInfo> gameModInfoList;

void updateAddonsList(CListBoxWindow* list) {
    int selectIndex = list->GetCurSel();
    list->Clear();
    for (const auto& info : gameModInfoList) {
        std::string name;
        if (info.mod->available) {
            name = info.wantedEnabled ? "&FFFFFF" : "&FF0000";
        } else {
            name = "&666666";
        }
        name += info.mod->mod_name;
        list->AddString( name.c_str(), 0 );
    }
    if (0 <= selectIndex) {
        list->SetCurSel(selectIndex);
    }
}

void loadAddonsList() {
    gameModInfoList.clear();
    for (auto& pair : getGameMods()) {
        ModInfo info;
        info.mod = &pair.second;
        info.wantedEnabled = info.mod->enabled;
        gameModInfoList.emplace_back(info);
    }

    CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_LIST));
    CComboWindow* combo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_ENABLE_COMBO));
    if (gameModInfoList.empty()) {
        combo->size = 1;
    } else {
        const ModInfo& info = gameModInfoList[0];
        combo->size = info.mod->available ? combo->Array.size() : 1;
        combo->pos = info.wantedEnabled ? 1 : 0;
        list->SetCurSel(0);
    }
    _shellIconManager.GetWnd(SQSH_MM_ADDONS_APPLY_BTN)->Enable(false);
    
    updateAddonsList(list);
}

void onMMAddonsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_ADDONS_SCR);
    }
}

//Addons back button
int addonsBackConfirmationQuant(float, float) {
    if (menuChangingDone) {
        _shellIconManager.SwitchMenuScreens(SQSH_MM_ADDONS_SCR, SQSH_MM_COMMUNITY_SCR);
        return 0;
    }
    return 1;
}

int addonsApplyConfirmationQuant(float, float) {
    if (menuChangingDone) {
        pendingChanges = false;

        //Apply changes from gameModInfoList
        for (auto& info : gameModInfoList) {
            if (!info.mod->available) {
                continue;
            }
            if (info.mod->enabled != info.wantedEnabled) {
                std::string path = info.mod->path;
                std::string newPath = path;
                if (info.mod->enabled) {
                    newPath += ".off";
                } else {
                    size_t pos = newPath.rfind(".off");
                    if (pos != std::string::npos) {
                        newPath.erase(pos);
                    }
                }
                std::error_code error;
                std::filesystem::rename(
                        std::filesystem::u8path(path),
                        std::filesystem::u8path(newPath),
                        error
                );
                if (error) {
                    ErrH.Abort("Can't change mod enable status: ", XERR_USER, error.value(), error.message().c_str());
                }

                info.mod->enabled = info.wantedEnabled;
            }
        }

        std::vector<std::string> args;
        args.emplace_back("tmp_start_splash=0");
        request_application_restart(&args);
        _shellIconManager.SwitchMenuScreens(SQSH_MM_ADDONS_SCR, RESTART_GAME);
        return 0;
    }
    return 1;
}

int addonsBackConfirmationHandler(float, float) {
    hideMessageBox();
    _shellIconManager.AddDynamicHandler( addonsBackConfirmationQuant, CBCODE_QUANT );
    return 0;
}

int addonsApplyConfirmationHandler(float, float) {
    hideMessageBox();
    _shellIconManager.AddDynamicHandler( addonsApplyConfirmationQuant, CBCODE_QUANT );
    return 0;
}

void onMMAddonsBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() && pendingChanges ) {
        std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.PendingChanges");
        setupYesNoMessageBox(addonsBackConfirmationHandler, 0, text);
        showMessageBox();
    } else {
        onMMBackButton(pWnd, code, param);
    }
}

//Switches current selected addon on/off
void addonEnableSwitch(CListBoxWindow* list) {
    int pos = list->GetCurSel();
    if (pos >= 0 && pos < gameModInfoList.size()) {
        auto& addon = gameModInfoList[pos];
        if (!addon.mod->available) {
            return;
        }
        addon.wantedEnabled = !addon.wantedEnabled;
        
        //Setup enable combo
        CComboWindow* combo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_ENABLE_COMBO));
        combo->pos = addon.wantedEnabled ? 1 : 0;

        updateAddonsList(list);

        pendingChanges = true;
        _shellIconManager.GetWnd(SQSH_MM_ADDONS_APPLY_BTN)->Enable(pendingChanges);
    }
}

void onMMAddonsList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(pWnd);
    if ( code == EVENT_PRESSED ) {
        int pos = list->GetCurSel();
        if (pos >= 0 && pos < gameModInfoList.size()) {
            auto& addon = gameModInfoList[pos];
            CComboWindow* combo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_ENABLE_COMBO));
            combo->Enable(true);
            //Only allow changing combo if loaded
            combo->size = addon.mod->available ? combo->Array.size() : 1;
            combo->pos = addon.wantedEnabled ? 1 : 0;
        }
    } else if ( code == EVENT_DOUBLECLICK && param == VK_LBUTTON) {
        addonEnableSwitch(list);
    }
}

void onMMAddonsApplyButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.ApplyChangesRestart");
        setupYesNoMessageBox(addonsApplyConfirmationHandler, 0, text);
        showMessageBox();
    }
}

void onMMAddonsEnableCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    CComboWindow *pCombo = (CComboWindow*) pWnd;
    if ( code == EVENT_CREATEWND ) {
        pCombo->Array.emplace_back(getItemTextFromBase("Off").c_str());
        pCombo->Array.emplace_back(getItemTextFromBase("On").c_str());
        pCombo->size = pCombo->Array.size();
        pCombo->pos = 0;
    } else if( code == EVENT_UNPRESSED && intfCanHandleInput()) {
        CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_LIST));
        addonEnableSwitch(list);
    } else if (code == EVENT_DRAWWND) {
        //CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_LIST));
        //pWnd->Enable( list->GetCurSel() >= 0 );
    }
}