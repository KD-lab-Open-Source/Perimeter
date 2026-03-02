#include <string>
#include <cstdint>
#include <map>
#include <list>
#include <algorithm>
#include <filesystem>

#include "SystemUtil.h"
#include "xutl.h"
#include "xmath.h"
#include "xerrhand.h"
#include "tweaks.h"
#include "exodus.h"
#include "DebugUtil.h"
#include "UnitAttribute.h"
#include "GameContent.h"

#include <SDL_misc.h>

#include <steam_api.h>
#include "integrations.h"
#include "integration_steam_public.h"
#include "integration_steam.h"

#include "codepages/codepages.h"
#include "files/files.h"

bool integration_steam_game_will_relaunch() {
    //This code can run before Steam's init
    IniManager ini("Perimeter.ini", false);
    if (ini.getInt("Integrations", "SteamRelaunchIfDirectly")) {
        if (SteamAPI_RestartAppIfNecessary(INTEGRATION_STEAM_APPID)) {
            printf("Steam integration detect game launched directly, restarting to run via Steam\n");
            return true;
        }
    }
    return false;
}

integration_store* integration_steam_init() {
    if (check_command_line("no_steam")) {
        return nullptr;
    }
    IniManager ini("Perimeter.ini", false);
    bool has_steam = *ini.get("Integrations", "Steam") != 0;
    if (has_steam && !ini.getInt("Integrations", "Steam")) {
        return nullptr;
    }
    
    SteamErrMsg errMsg;
    switch (SteamAPI_InitEx(&errMsg)) {
        case k_ESteamAPIInitResult_OK:
            printf("Steam integration init OK\n");
            break;
        case k_ESteamAPIInitResult_FailedGeneric:
            fprintf(stderr, "Steam integration init failed: %s\n", errMsg);
            return nullptr;
        case k_ESteamAPIInitResult_NoSteamClient:
            fprintf(stderr, "Steam integration init can't connect: %s\n", errMsg);
            return nullptr;
        case k_ESteamAPIInitResult_VersionMismatch:
            fprintf(stderr, "Steam integration init version mismatch: %s\n", errMsg);
            return nullptr;
    }
    
    //Store config in ini if absent
    if (!has_steam) {
        ini.put("Integrations", "Steam", "1");
    }
    if (*ini.get("Integrations", "SteamRelaunchIfDirectly") == 0) {
        ini.put("Integrations", "SteamRelaunchIfDirectly", "1");
    }
    ini.save();
    
    integration_steam* steam = new integration_steam();
    return steam;
}

void integration_steam::close() {
    SteamAPI_Shutdown();
}

void integration_steam::quant() {
    if (steam_disconnected) {
        return;
    }
    SteamAPI_RunCallbacks();
}

const char* integration_steam::get_store_id() {
    return "steam";
}

std::string integration_steam::get_player_name() {
    if (steam_disconnected) {
        return {};
    }
    std::string player_name = SteamFriends()->GetPersonaName();
    return player_name;
}

std::string integration_steam::get_selected_locale() {
    if (steam_disconnected) {
        return {};
    }
    std::string selected_locale = SteamApps()->GetCurrentGameLanguage();
    return selected_locale;
}

void integration_steam::upload_mod(ModMetadata* mod) {
    need_accept_workshop_legal_agreement = false;
    //Get mod data
    std::string path_ini = mod->path + PATH_SEP + "mod.ini";
    IniManager mod_ini = IniManager(path_ini.c_str(), false);
    if (mod->mod_name.length() >= k_cchPublishedDocumentTitleMax) {
        fprintf(stderr, "Description is too long, max is '%d'!", k_cchPublishedDocumentDescriptionMax);
        return;
    }
    std::string description;
    //Remove all color codes
    int skip_chars = 0;
    for (char c : mod->mod_description_english) {
        if (c == '&') {
            if (6 == skip_chars) {
                skip_chars = 0;
                description += c;
            } else {
                skip_chars = 6;
            }
        } else if (0 < skip_chars) {
            skip_chars--;
        } else {
            description += c;
        }
    }
    if (!description.empty()) {
        description = convertToUnicode(description, "english", '?', true);
    }
    if (description.empty()) {
        fprintf(stderr, "Mod description is required!");
        return;
    }
    if (description.length() >= k_cchPublishedDocumentDescriptionMax) {
        fprintf(stderr, "Description is too long, max is '%d'!", k_cchPublishedDocumentDescriptionMax);
        return;
    }
    
    //Retrieve file id if set already
    bool new_item = false;
    const char* store_steam_file_id = mod_ini.get("Integrations", "store_steam_file_id");
    if (store_steam_file_id && 0 < strlen(store_steam_file_id)) {
        uint64_t val = strtol(store_steam_file_id, nullptr, 0);
        if (val == k_PublishedFileIdInvalid) {
            fprintf(stderr, "File ID is k_PublishedFileIdInvalid\n");
            return;
        }
        published_file_id = val;
    } else {
        //Create new item and store the published id
        printf("No item ID present, creating new item\n");
        create_item();
        if (published_file_id == k_PublishedFileIdInvalid) {
            return;
        }
        mod_ini.put("Integrations", "store_steam_file_id", std::to_string(published_file_id).c_str());
        mod_ini.save();
        new_item = true;
    }

    printf("Published file id: %" PRIu64"\n", (uint64_t) published_file_id);
    
    //Set data into item
    ISteamUGC* ugc = SteamUGC();
    UGCUpdateHandle_t update_handle = ugc->StartItemUpdate(INTEGRATION_STEAM_APPID, published_file_id);
    //Sets a new title for an item.
    std::string title = mod->mod_name;
    if (!title.empty()) {
        title = convertToUnicode(title, "english", '?', true);
    }
    if (title.empty()) {
        fprintf(stderr, "Mod title is required!");
        return;
    }
    bool ok;
    //Sets the language of the title and description that will be set in this item update.
    ok = ugc->SetItemUpdateLanguage(update_handle, "english");
    if (!ok) {
        fprintf(stderr, "Error occurred setting item language\n");
        return;
    }
    ok = ugc->SetItemTitle(update_handle, title.c_str());
    if (!ok) {
        fprintf(stderr, "Error occurred setting item title\n");
        return;
    }
    //Sets a new description for an item.
    ok = ugc->SetItemDescription(update_handle, description.c_str());
    if (!ok) {
        fprintf(stderr, "Error occurred setting item description\n");
        return;
    }
    //Sets the visibility of an item if new, otherwise leave as is.
    if (new_item) {
        ok = ugc->SetItemVisibility(update_handle, k_ERemoteStoragePublishedFileVisibilityPrivate);
        if (!ok) {
            fprintf(stderr, "Error occurred setting item visibility\n");
            return;
        }
    }
    //Sets the folder that will be stored as the content for an item.
    std::string item_content_path = get_content_root_path_str() + mod->path;
    printf("Uploading mod content at: %s\n", item_content_path.c_str());
    ok = ugc->SetItemContent(update_handle, item_content_path.c_str());
    if (!ok) {
        fprintf(stderr, "Error occurred setting item content\n");
        return;
    }
    //Sets the primary preview image for the item.
    filesystem_entry* mod_preview = get_content_entry(mod->path + "mod.png");
    if (mod_preview) {
        std::string item_preview_path = get_content_root_path_str() + PATH_SEP + mod_preview->path_content;
        printf("Uploading mod preview image at: %s\n", item_preview_path.c_str());
        ok = ugc->SetItemPreview(update_handle, item_preview_path.c_str());
        if (!ok) {
            fprintf(stderr, "Error occurred setting item preview\n");
            return;
        }
    }
    //Start the upload to workshop
    SteamAPICall_t SubmitItemUpdateAPICall = ugc->SubmitItemUpdate(update_handle, nullptr);
    SubmitItemUpdate_callback_called = false;
    SubmitItemUpdate_callback_result = k_EResultNone;
    m_SubmitItemUpdateResult.Set(SubmitItemUpdateAPICall, this, &integration_steam::OnSteamSubmitItemUpdate);
    while (true) {
        SteamAPI_RunCallbacks();
        if (SubmitItemUpdate_callback_called) {
            break;
        }
        uint64 bytesProcessed = 0;
        uint64 bytesTotal = 0;
        EItemUpdateStatus status = ugc->GetItemUpdateProgress(update_handle, &bytesProcessed, &bytesTotal);
        printf(
            "Uploading status '%d' - %" PRIu64 "/%" PRIu64 " bytes\n",
            status, (uint64_t) bytesProcessed, (uint64_t) bytesTotal
        );
        Sleep(1000);
    }
    
    //Print any info relevant to the process result
    if (SubmitItemUpdate_callback_result != k_EResultOK) {
        printf("SubmitItemUpdateResult returned result '%d'\n", SubmitItemUpdate_callback_result);
        return;
    }
    if (need_accept_workshop_legal_agreement) {
        printf(
            "\n\nBy submitting this item, you agree to the workshop terms of service:\n"
            "https://steamcommunity.com/sharedfiles/workshoplegalagreement\n"
            "\n"
            "Finished uploading mod!\n"
            "Opening item in your Steam client to configure and accept the terms of service...\n"
        );
    } else {
        printf(
            "\n\nFinished uploading mod!\n"
            "Opening item in your Steam client to configure...\n"
        );
    }
    if (new_item) {
        printf(
            "NOTE: Item was created with private visibility\n"
            "by default, change it to be visible for others\n"
        );
    }
    std::string steam_item_url = "steam://url/CommunityFilePage/" + std::to_string((uint64_t) published_file_id);
    SDL_OpenURL(steam_item_url.c_str());
}

void integration_steam::OnSteamSubmitItemUpdate(SubmitItemUpdateResult_t* pCallback, bool bIOFailure) {
    SubmitItemUpdate_callback_called = true;
    if (bIOFailure) {
        printf("SubmitItemUpdateResult returned IO failure!");
        return;
    }
    SubmitItemUpdate_callback_result = pCallback->m_eResult;
    need_accept_workshop_legal_agreement |= pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement;
    if (pCallback->m_nPublishedFileId != published_file_id) {
        printf(
            "SubmitItemUpdateResult_t->m_nPublishedFileId '%" PRIu64 "' doesn't match current file id '%" PRIu64 "'",
            (uint64_t) pCallback->m_nPublishedFileId, (uint64_t) published_file_id
        );
    }
}

void integration_steam::create_item() {
    SteamAPICall_t CreateItemAPICall = SteamUGC()->CreateItem(INTEGRATION_STEAM_APPID, k_EWorkshopFileTypeCommunity);
    CreateItem_callback_called = false;
    published_file_id = k_PublishedFileIdInvalid;
    m_CreateItemResult.Set(CreateItemAPICall, this, &integration_steam::OnSteamCreateItem);
    for (int i = 0; i < 60; ++i) {
        SteamAPI_RunCallbacks();
        if (CreateItem_callback_called) {
            break;
        }
        Sleep(500);
    }
    if (!CreateItem_callback_called) {
        printf("Timeout waiting for CreateItem");
        return;
    }
    if (published_file_id == k_PublishedFileIdInvalid) {
        printf("File ID is k_PublishedFileIdInvalid\n");
    }
}

void integration_steam::OnSteamCreateItem(CreateItemResult_t *pCallback, bool bIOFailure) {
    CreateItem_callback_called = true;
    published_file_id = k_PublishedFileIdInvalid;
    if (bIOFailure) {
        printf("CreateItemResult returned IO failure!");
        return;
    }
    if (pCallback->m_eResult != k_EResultOK) {
        printf("CreateItemResult returned result '%d'", pCallback->m_eResult);
        return;
    }
    need_accept_workshop_legal_agreement |= pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement;
    if (pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement) {
        printf("CreateItemResult_t->m_bUserNeedsToAcceptWorkshopLegalAgreement unsupported currently!");
        return;
    }
    published_file_id = pCallback->m_nPublishedFileId;
}

//Callbacks
void integration_steam::OnIPCFailure(IPCFailure_t* failure) {
    (void) failure;
    fprintf(stderr, "Steam IPC Failure\n");
    steam_disconnected = true;
}

void integration_steam::OnSteamShutdown(SteamShutdown_t* callback) {
    (void) callback;
    fprintf(stderr, "Steam shutdown request\n");
    steam_disconnected = true;
}
