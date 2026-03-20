#include "StdAfx.h"
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "files/files.h"
#include "GameContent.h"
#include <filesystem>

#include "integrations.h"
#include "MainMenu.h"

extern bool menuChangingDone;
bool pendingChanges = false;
bool isPublishMode = false;

struct ModInfo {
    ModMetadata* mod;
    bool wantedEnabled = false;
};

std::vector<ModInfo> gameModInfoList;
//We need a place to store the metadata's where the ModInfo's point to in publish mode
std::vector<ModMetadata> gameModPublish;

CListBoxWindow* getModListWindow() {
    return dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(isPublishMode ? SQSH_MM_MOD_PUBLISH_LIST : SQSH_MM_MOD_LIST_LIST));
}

CTextWindow* getModDescriptionWindow() {
    return dynamic_cast<CTextWindow*>(_shellIconManager.GetWnd(isPublishMode ? SQSH_MM_MOD_PUBLISH_DESCR_TXT : SQSH_MM_MOD_LIST_DESCR_TXT));
}

void updateAddonsList(CListBoxWindow* list) {
    int selectIndex = list->GetCurSel();
    list->Clear();
    integration_store* store = nullptr;
    if (isPublishMode) {
        store = integrations::get_store();
        if (!store) {
            return;
        }
    }
    for (const auto& info : gameModInfoList) {
        std::string name;
        if (info.mod->available) {
            if (isPublishMode) {
                name = info.wantedEnabled ? (store->mod_already_uploaded(info.mod) ? "&00FF00" : "&FFFFFF") : "&FF0000";
            } else {
                name = info.wantedEnabled ? "&FFFFFF" : "&FF0000";
            }
        } else {
            name = "&666666";
        }
        name += string_remove_color_codes(info.mod->mod_name.c_str());
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
    if (!mod->mod_origin.empty()) {
        popupTxt += "\n&AAAAAA";
        popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.MetadataOrigin");
        const char* origin_text = qdTextDB::instance().getText(("Interface.Menu.Mods.ModOrigin." + mod->mod_origin).c_str());
        popupTxt += " &FFFFFF" + (origin_text ? origin_text : mod->mod_origin);
    }
    if (!mod->mod_authors.empty()) {
        popupTxt += "\n&AAAAAA";
        popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.MetadataAuthors");
        popupTxt += " &FFFFFF" + mod->mod_authors;
    }
    if (!mod->mod_license.empty()) {
        popupTxt += "\n&AAAAAA";
        popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.MetadataLicense");
        popupTxt += " &FFFFFF" + mod->mod_license;
    }
    if (!mod->mod_url.empty()) {
        popupTxt += "\n&AAAAAA";
        popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.MetadataSite");
        popupTxt += " &FFFFFF" + mod->mod_url;
    }
    if (isPublishMode) {
        integration_store* store = integrations::get_store();
        if (store) {
            popupTxt += "\n\n&AAAAAA";
            popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.Published");
            popupTxt += " &FFFFFF";
            popupTxt += qdTextDB::instance().getText(
                store->mod_already_uploaded(mod) ? "Interface.Menu.ButtonLabels.YES" : "Interface.Menu.ButtonLabels.NO"
            );
        }
    }
    if (!mod->errors.empty()) {
        popupTxt += "\n\n&FF0000";
        popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.Errors");
        for (std::string error : mod->errors) {
            if (startsWith(error, "TEXT=")) {
                error = qdTextDB::instance().getText(error.substr(5).c_str());
            }
            popupTxt += "\n" + error;
        }
    }
    if (add_description && !mod->mod_description.empty()) {
        popupTxt += "\n\n&AAAAAA";
        popupTxt += qdTextDB::instance().getText("Interface.Menu.Mods.MetadataDescription");
        popupTxt += "\n&FFFFFF" + mod->mod_description;
    }

    return popupTxt;
}

void updateAddonDescription(CListBoxWindow* list) {
    CComboWindow* combo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_MOD_LIST_ENABLE_COMBO));
    std::string text;
    int pos = list->GetCurSel();
    if (pos >= 0 && pos < gameModInfoList.size()) {
        auto info = gameModInfoList[pos];
        text = GenerateInfoText(info.mod, true);
        combo->Enable(!isPublishMode);
        if (isPublishMode) {
            _shellIconManager.GetWnd(SQSH_MM_MOD_PUBLISH_PUBLISH_BTN)->Enable(info.mod->enabled);
        } else {
            _shellIconManager.GetWnd(SQSH_MM_MOD_PUBLISH_PUBLISH_BTN)->Enable(false);
            //Only allow changing combo if loaded
            combo->size = info.mod->available ? combo->Array.size() : 1;
            combo->pos = info.wantedEnabled ? 1 : 0;
        }
    } else {
        combo->Enable(false);
        _shellIconManager.GetWnd(SQSH_MM_MOD_PUBLISH_PUBLISH_BTN)->Enable(false);
    }
    
    getModDescriptionWindow()->SetText(text.c_str());
}

void loadModList(int current_id) {
    pendingChanges = false;
    isPublishMode = false;
    gameModInfoList.clear();
    gameModPublish.clear();
    if (current_id == SQSH_MM_MOD_LIST_SCR) {
        for (auto& pair : getGameMods()) {
            ModInfo info;
            info.mod = &pair.second;
            info.wantedEnabled = info.mod->enabled;
            gameModInfoList.emplace_back(info);
        }
    } else if (current_id == SQSH_MM_MOD_PUBLISH_SCR) {
        isPublishMode = true;
        ModMetadata parsed;
        scan_resource_paths(convert_path_content("mods/publish"));
        for (const auto& entry: get_content_entries_directory("mods/publish")) {
            if (entry->is_directory) {
                parseModInPath(parsed, entry->path_content.c_str(), MOD_ORIGIN_GAME_MODS);
    
                //If is ET skip it, we don't allow uploading ET
                if (parsed.is_content_et) {
                    continue;
                }
                
                //Check if all files are allowed
                if (parsed.enabled && containsDisallowedFilesMod(parsed, true)) {
                    parsed.available = false;
                    parsed.enabled = false;
                    parsed.errors.emplace_back("TEXT=Interface.Menu.Mods.ErrorDisallowedFile");
                }
                
                //Store parsed mod into publish vector and keep ptr in info
                ModInfo info;
                info.mod = &gameModPublish.emplace_back(parsed);
                info.wantedEnabled = info.mod->enabled;
                gameModInfoList.emplace_back(info);
            }
        }
    }

    _shellIconManager.GetWnd(SQSH_MM_MOD_PUBLISH_PUBLISH_BTN)->Enable(false);
    _shellIconManager.GetWnd(SQSH_MM_MOD_LIST_APPLY_BTN)->Enable(false);
    CListBoxWindow* list = getModListWindow();
    CComboWindow* combo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_MOD_LIST_ENABLE_COMBO));
    if (gameModInfoList.empty()) {
        combo->size = 1;
    } else {
        const ModInfo& info = gameModInfoList[0];
        combo->size = info.mod->available ? combo->Array.size() : 1;
        combo->pos = info.wantedEnabled ? 1 : 0;
        list->SetCurSel(0);
        _shellIconManager.GetWnd(SQSH_MM_MOD_PUBLISH_PUBLISH_BTN)->Enable(true);
    }
    
    updateAddonsList(list);
    updateAddonDescription(list);
}

void onMMModMenuButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_MOD_MENU_SCR);
    }
}

void onMMModListButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_MOD_LIST_SCR);
    }
}

void onMMModPublishButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if ( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        if (integrations::get_store() == nullptr) {
            std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.NoStoreAvailable");
            setupOkMessageBox(nullptr, 0, text, MBOX_BACK);
            showMessageBox();
        } else if (!integrations::get_store()->supports_mod_uploading()) {
            std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.StoreUnsupportedFeature");
            setupOkMessageBox(nullptr, 0, text, MBOX_BACK);
            showMessageBox();
        } else {
            _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_MOD_PUBLISH_SCR);
        }
    }
}

//Mod list back button
int addonsBackConfirmationQuant(float, float) {
    if (menuChangingDone) {
        _shellIconManager.SwitchMenuScreens(SQSH_MM_MOD_LIST_SCR, SQSH_MM_MOD_MENU_SCR);
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
        _shellIconManager.SwitchMenuScreens(SQSH_MM_MOD_LIST_SCR, RESTART_GAME);
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

void onMMModListBackButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
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
    if (!isPublishMode && pos >= 0 && pos < gameModInfoList.size()) {
        auto& addon = gameModInfoList[pos];
        if (!addon.mod->available) {
            return;
        }
        addon.wantedEnabled = !addon.wantedEnabled;
        
        //Setup enable combo
        CComboWindow* combo = dynamic_cast<CComboWindow*>(_shellIconManager.GetWnd(SQSH_MM_MOD_LIST_ENABLE_COMBO));
        combo->pos = addon.wantedEnabled ? 1 : 0;

        updateAddonsList(list);

        pendingChanges = true;
        _shellIconManager.GetWnd(SQSH_MM_MOD_LIST_APPLY_BTN)->Enable(pendingChanges);
    }
}

void onMMModListList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(pWnd);
    if ( code == EVENT_PRESSED ) {
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

void onMMModListApplyButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.Confirmations.ApplyChangesRestart");
        setupYesNoMessageBox(addonsApplyConfirmationHandler, 0, text);
        showMessageBox();
    }
}

void onMMModListEnableCombo(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    CComboWindow *pCombo = (CComboWindow*) pWnd;
    if ( code == EVENT_CREATEWND ) {
        pCombo->Array.emplace_back(getItemTextFromBase("Off").c_str());
        pCombo->Array.emplace_back(getItemTextFromBase("On").c_str());
        pCombo->size = pCombo->Array.size();
        pCombo->pos = 0;
    } else if( code == EVENT_UNPRESSED && intfCanHandleInput()) {
        addonEnableSwitch(getModListWindow());
    }
}

int modPublishProgressQuant(float, float) {
    integration_store* store = integrations::get_store();
    if (!isPublishMode || !store) {
        hideMessageBox();
        return 0;
    }
    
    integration_store_upload_mod_status status = store->upload_mod_get_progress();
    if (status.error) {
        //Error happened
        printf("integration_store::upload_mod_get_progress returned error '%s'\n", status.error);
        std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.UnknownError");
        text = text + "\n" + status.error;
        setupOkMessageBox(nullptr, 0, text, MBOX_OK, true);
        updateMessageBoxButtonsVisibility();
        return 0;
    } else if (status.in_progress) {
        //Wait for next quant to check again
        return 1;
    } else {
        //Done!
        hideMessageBox();
        return 0;
    }
}

int modPublishConfirmationHandler(float, float) {
    CListBoxWindow* list = getModListWindow();
    int pos = list->GetCurSel();
    integration_store* store = integrations::get_store();
    if (isPublishMode && store && store->supports_mod_uploading() && pos >= 0 && pos < gameModInfoList.size()) {
        auto& mod = gameModInfoList[pos];
        if (!mod.mod->available || !mod.mod->enabled || !mod.mod->errors.empty()) {
            return 0;
        }
        integration_store_upload_mod_status status = store->upload_mod(mod.mod);
        if (status.in_progress) {
            setupOkMessageBox(nullptr, 0, qdTextDB::instance().getText("Interface.Menu.Messages.Publishing"), MBOX_OK, false);
            _shellIconManager.AddDynamicHandler( modPublishProgressQuant, CBCODE_QUANT );
        } else {
            printf("integration_store::upload_mod returned error '%s'\n", status.error);
            std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.UnknownError");
            text = text + "\n" + status.error;
            setupOkMessageBox(nullptr, 0, text, MBOX_OK, true);
        }
        updateMessageBoxButtonsVisibility();
        return 1;
    }
    return 0;
}

void onMMModPublishPublishButton(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    integration_store* store = integrations::get_store();
    if( code == EVENT_UNPRESSED && intfCanHandleInput() && isPublishMode && store && store->supports_mod_uploading()) {
        CListBoxWindow* list = getModListWindow();
        int pos = list->GetCurSel();
        if (pos < 0 || pos >= gameModInfoList.size()) {
            return;
        }
        auto& mod = gameModInfoList[pos];
        if (!mod.mod->available || !mod.mod->enabled || !mod.mod->errors.empty()) {
            return;
        }
        std::string text_id = "Interface.Menu.Messages.Confirmations.PublishMod.";
        text_id += store->get_store_id(); 
        std::string text = qdTextDB::instance().getText(text_id.c_str());
        if (text.empty()) {
            return;
        }
        setupYesNoMessageBox(modPublishConfirmationHandler, 0, text);
        showMessageBox();
    }
}