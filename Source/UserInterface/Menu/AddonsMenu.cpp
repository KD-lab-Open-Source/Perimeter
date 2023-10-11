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

std::string GenerateInfoText(ModMetadata* mod, bool add_description) {
    std::string popupTxt = "&FFFF00" + mod->mod_name;
    popupTxt += "\n\n&AAAAAA";
    popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.MetadataVersion");
    popupTxt += " &FFFFFF" + mod->mod_version;
    if (!mod->mod_authors.empty()) {
        popupTxt += "\n&AAAAAA";
        popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.MetadataAuthors");
        popupTxt += " &FFFFFF" + mod->mod_authors;
    }
    if (!mod->mod_url.empty()) {
        popupTxt += "\n&AAAAAA";
        popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.MetadataSite");
        popupTxt += " &FFFFFF" + mod->mod_url;
    }
    if (!mod->errors.empty()) {
        popupTxt += "\n\n&FF0000";
        popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.Errors");
        popupTxt += "\n" + mod->errors;
    }
    if (add_description && !mod->mod_description.empty()) {
        popupTxt += "\n\n&AAAAAA";
        popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.MetadataDescription");
        popupTxt += "\n&FFFFFF" + mod->mod_description;
    }

    return popupTxt;
}

void updateAddonDescription(CListBoxWindow* list) {
    CTextWindow* descr = dynamic_cast<CTextWindow*>(_shellIconManager.GetWnd(SQSH_MM_ADDONS_DESCR_TXT));
    std::string text;
    int pos = list->GetCurSel();
    if (pos >= 0 && pos < gameModInfoList.size()) {
        auto mod = gameModInfoList[pos].mod;
        text = GenerateInfoText(mod, true);
    }
    
    descr->SetText(text.c_str());
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
    updateAddonDescription(list);
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
            if (info.mod->enabled != info.wantedEnabled) {;
                //Remove any .off that mod might have
                if (info.wantedEnabled) {
                    size_t pos = info.mod->path.rfind(".off");
                    if (pos != std::string::npos) {
                        std::string newPath = info.mod->path;
                        newPath.erase(pos);

                        std::error_code error;
                        std::filesystem::rename(
                                std::filesystem::u8path(info.mod->path),
                                std::filesystem::u8path(newPath),
                                error
                        );
                        if (error) {
                            ErrH.Abort("Can't rename mod to remove '.off': ", XERR_USER, error.value(), error.message().c_str());
                        }
                        
                        info.mod->path = newPath;
                        //Rescan so mod config can be updated
                        scan_resource_paths(newPath);
                    }
                }
                
                //Set mod config enabled state
                std::string path_ini = info.mod->path + PATH_SEP + "mod_config.ini";
                IniManager mod_ini = IniManager(path_ini.c_str(), false);
                mod_ini.put("Mod", "enabled", info.wantedEnabled ? "true" : "false");

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
        updateAddonDescription(list);
    } else if ( code == EVENT_DOUBLECLICK && param == VK_LBUTTON) {
        addonEnableSwitch(list);
    } else if ( code == EVENT_DRAWWND ) {
        Vect2f mousePos = gameShell->mousePosition();
        mousePos.x += 0.5f;
        mousePos.y += 0.5f;

        if (!list->HitTest(mousePos.x, mousePos.y)) {
            return;
        }
        
        Vect2i pt = {
                xm::round(mousePos.x * terRenderDevice->GetSizeX()),
                xm::round(mousePos.y * terRenderDevice->GetSizeY())
        };
        
        int pos = list->ItemFromPoint(pt.y, false);
        if (pos >= 0 && pos < gameModInfoList.size()) {
            ModMetadata* mod = gameModInfoList[pos].mod;
            
            std::string text = GenerateInfoText(mod, false);

            //Draw rect
            terRenderDevice->SetFont(pWnd->m_hFont);
            
            Vect2f v1;
            Vect2f v2;
            terRenderDevice->OutTextRect(0, 0, text.c_str(), -1, v1, v2);

            const float pad = 5;
            Vect2f rectsize = {
                    v2.x - v1.x + pad * 2,
                    v2.y - v1.y + pad * 2
            };
            float x_max = terRenderDevice->GetSizeX() - rectsize.x - pad;
            float y_max = terRenderDevice->GetSizeY() - rectsize.y - pad;
            float y_move = rectsize.y + pad;
            Vect2f rectpos = {
                    min(max(pad, pt.x - rectsize.x / 2.0f), x_max),
                    min(max(pad, pt.y + (mousePos.y < 0 ? -y_move : (pad + 40))), y_max)
            };

            terRenderDevice->DrawRectangle(rectpos.x, rectpos.y, rectsize.x, rectsize.y, sColor4c(32, 32, 32, 72), 0);
            terRenderDevice->DrawRectangle(rectpos.x, rectpos.y, rectsize.x, rectsize.y, sColor4c(255, 255, 255, 255), 1);
            terRenderDevice->OutText(rectpos.x + pad, rectpos.y + pad, text.c_str(), sColor4f(1, 1, 1, 1), -1);
            terRenderDevice->SetFont(nullptr);
            terRenderDevice->FlushPrimitive2D();
        }
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