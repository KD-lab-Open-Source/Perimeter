#include "StdAfx.h"
#include "MainMenu.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "files/files.h"
#include <filesystem>

extern bool menuChangingDone;
bool pendingChanges = false;

struct AddonInfo {
    std::string path;
    std::string name;
    bool currentlyEnabled = false;
    bool wantedEnabled = false;
};

std::vector<AddonInfo> addonList;

void loadAddonsList() {
    addonList.clear();
    for (const auto& entry : get_content_entries_directory("mods")) {
        if (entry->is_directory) {
            std::filesystem::path entry_path = std::filesystem::u8path(entry->path_content);
            std::string name = entry_path.filename().u8string();
            AddonInfo info;
            if (endsWith(name, ".off")) {
                name.erase(name.rfind(".off"));
                info.currentlyEnabled = false;
            } else {
                info.currentlyEnabled = true;
            }
            info.path = entry->path_content;
            info.name = name;
            info.wantedEnabled = info.currentlyEnabled;
            addonList.emplace_back(info);
        }
    }

    CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_LIST));
    CComboWindow* combo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_ENABLE_COMBO));
    if (addonList.empty()) {
        combo->Enable(false);
    } else {
        combo->Enable(true);
        combo->pos = addonList[0].wantedEnabled ? 1 : 0;
        list->SetCurSel(0);
    }
    _shellIconManager.GetWnd(SQSH_MM_ADDONS_APPLY_BTN)->Enable(false);
}

void updateAddonsList(CListBoxWindow* list) {
    int selectIndex = list->GetCurSel();
    list->Clear();
    for (const auto& addon : addonList) {
        std::string name = addon.wantedEnabled ? "&FFFFFF" : "&FF0000";
        name += addon.name;
        list->AddString( name.c_str(), 0 );
    }
    if (0 <= selectIndex) {
        list->SetCurSel(selectIndex);
    }
}

void onMMAddonsButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        loadAddonsList();
        CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_LIST));
        updateAddonsList(list); 
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_ADDONS_SCR);
    }
}

//Addons back button
int addonsBackConfirmationQuant(float, float) {
    if (menuChangingDone) {
        _shellIconManager.SwitchMenuScreens(SQSH_MM_ADDONS_SCR, SQSH_MM_START_SCR);
        return 0;
    }
    return 1;
}

int addonsApplyConfirmationQuant(float, float) {
    if (menuChangingDone) {
        pendingChanges = false;

        //Apply changes from addonList
        for (auto addon : addonList) {
            if (addon.currentlyEnabled != addon.wantedEnabled) {
                std::string newPath = addon.path;
                if (addon.currentlyEnabled) {
                    newPath += ".off";
                } else {
                    size_t pos = newPath.rfind(".off");
                    newPath.erase(pos);
                }
                std::error_code error;
                std::filesystem::rename(
                        std::filesystem::u8path(addon.path),
                        std::filesystem::u8path(newPath),
                        error
                );
                if (error) {
                    ErrH.Abort("Can't copy new profile: ", XERR_USER, error.value(), error.message().c_str());
                }

                addon.currentlyEnabled = addon.wantedEnabled;
            }
        }

        request_application_restart();
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
    if (pos >= 0 && pos < addonList.size()) {
        auto& addon = addonList[pos];
        addon.wantedEnabled = !addon.wantedEnabled;
        
        //Setup enable combo
        CComboWindow* combo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_ENABLE_COMBO));
        //combo->Enable(true);
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
        if (pos >= 0 && pos < addonList.size()) {
            auto& addon = addonList[pos];
            CComboWindow* combo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_ENABLE_COMBO));
            combo->Enable(true);
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