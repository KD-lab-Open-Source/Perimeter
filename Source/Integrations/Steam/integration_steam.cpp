#include <string>
#include <cstdint>
#include <map>
#include <list>
#include <algorithm>
#include <filesystem>
#include <cinttypes>

#include "SystemUtil.h"
#include "xbuffer.h"
#include "xutl.h"
#include "xmath.h"
#include "xerrhand.h"
#include "tweaks.h"
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

extern bool content_debug_flag;

bool integration_steam_game_will_relaunch() {
    //This code can run before Steam's init
    if (check_command_line("no_steam")) {
        return false;
    }
    IniManager ini("Perimeter.ini", false);
    if (ini.getInt("Integrations", "SteamRelaunchIfDirectly")) {
        if (SteamAPI_RestartAppIfNecessary(INTEGRATION_STEAM_APPID)) {
            printf("Steam integration detect game launched directly, restarting to run via Steam\n");
            return true;
        }
    }
    return false;
}

const char* string_of_result(EResult result) {
    switch (result) {
        case k_EResultNone: return "None";
        case k_EResultOK: return "OK";
        case k_EResultFail: return "Fail"; 
        case k_EResultNoConnection: return "NoConnection";
        case k_EResultInvalidPassword: return "InvalidPassword";
        case k_EResultLoggedInElsewhere: return "LoggedInElsewhere";
        case k_EResultInvalidProtocolVer: return "InvalidProtocolVer";
        case k_EResultInvalidParam: return "InvalidParam";
        case k_EResultFileNotFound: return "FileNotFound";
        case k_EResultBusy: return "Busy";
        case k_EResultInvalidState: return "InvalidState";
        case k_EResultInvalidName: return "InvalidName";
        case k_EResultInvalidEmail: return "InvalidEmail";
        case k_EResultDuplicateName: return "DuplicateName";
        case k_EResultAccessDenied: return "AccessDenied";
        case k_EResultTimeout: return "Timeout";
        case k_EResultBanned: return "Banned";
        case k_EResultAccountNotFound: return "AccountNotFound";
        case k_EResultInvalidSteamID: return "InvalidSteamID";
        case k_EResultServiceUnavailable: return "ServiceUnavailable";
        case k_EResultNotLoggedOn: return "NotLoggedOn";
        case k_EResultPending: return "Pending";
        case k_EResultEncryptionFailure: return "EncryptionFailure";
        case k_EResultInsufficientPrivilege: return "InsufficientPrivilege";
        case k_EResultLimitExceeded: return "LimitExceeded";
        case k_EResultRevoked: return "Revoked";
        case k_EResultExpired: return "Expired";
        case k_EResultAlreadyRedeemed: return "AlreadyRedeemed";
        case k_EResultDuplicateRequest: return "DuplicateRequest";
        case k_EResultAlreadyOwned: return "AlreadyOwned";
        case k_EResultIPNotFound: return "IPNotFound";
        case k_EResultPersistFailed: return "PersistFailed";
        case k_EResultLockingFailed: return "LockingFailed";
        case k_EResultLogonSessionReplaced: return "LogonSessionReplaced";
        case k_EResultConnectFailed: return "ConnectFailed";
        case k_EResultHandshakeFailed: return "HandshakeFailed";
        case k_EResultIOFailure: return "IOFailure";
        case k_EResultRemoteDisconnect: return "RemoteDisconnect";
        case k_EResultShoppingCartNotFound: return "ShoppingCartNotFound";
        case k_EResultBlocked: return "Blocked";
        case k_EResultIgnored: return "Ignored";
        case k_EResultNoMatch: return "NoMatch";
        case k_EResultAccountDisabled: return "AccountDisabled";
        case k_EResultServiceReadOnly: return "ServiceReadOnly";
        case k_EResultAccountNotFeatured: return "AccountNotFeatured";
        case k_EResultAdministratorOK: return "AdministratorOK";
        case k_EResultContentVersion: return "ContentVersion";
        case k_EResultTryAnotherCM: return "TryAnotherCM";
        case k_EResultPasswordRequiredToKickSession: return "PasswordRequiredToKickSession";
        case k_EResultAlreadyLoggedInElsewhere: return "AlreadyLoggedInElsewhere";
        case k_EResultSuspended: return "Suspended";
        case k_EResultCancelled: return "Cancelled";
        case k_EResultDataCorruption: return "DataCorruption";
        case k_EResultDiskFull: return "DiskFull";
        case k_EResultRemoteCallFailed: return "RemoteCallFailed";
        case k_EResultPasswordUnset: return "PasswordUnset";
        case k_EResultExternalAccountUnlinked: return "ExternalAccountUnlinked";
        case k_EResultPSNTicketInvalid: return "PSNTicketInvalid";
        case k_EResultExternalAccountAlreadyLinked: return "ExternalAccountAlreadyLinked";
        case k_EResultRemoteFileConflict: return "RemoteFileConflict";
        case k_EResultIllegalPassword: return "IllegalPassword";
        case k_EResultSameAsPreviousValue: return "SameAsPreviousValue";
        case k_EResultAccountLogonDenied: return "AccountLogonDenied";
        case k_EResultCannotUseOldPassword: return "CannotUseOldPassword";
        case k_EResultInvalidLoginAuthCode: return "InvalidLoginAuthCode";
        case k_EResultAccountLogonDeniedNoMail: return "AccountLogonDeniedNoMail";
        case k_EResultHardwareNotCapableOfIPT: return "HardwareNotCapableOfIPT"; 
        case k_EResultIPTInitError: return "IPTInitError"; 
        case k_EResultParentalControlRestricted: return "ParentalControlRestricted";
        case k_EResultFacebookQueryError: return "FacebookQueryError";
        case k_EResultExpiredLoginAuthCode: return "ExpiredLoginAuthCode";
        case k_EResultIPLoginRestrictionFailed: return "IPLoginRestrictionFailed";
        case k_EResultAccountLockedDown: return "AccountLockedDown";
        case k_EResultAccountLogonDeniedVerifiedEmailRequired: return "AccountLogonDeniedVerifiedEmailRequired";
        case k_EResultNoMatchingURL: return "NoMatchingURL";
        case k_EResultBadResponse: return "BadResponse";
        case k_EResultRequirePasswordReEntry: return "RequirePasswordReEntry";
        case k_EResultValueOutOfRange: return "ValueOutOfRange";
        case k_EResultUnexpectedError: return "UnexpectedError";
        case k_EResultDisabled: return "Disabled";
        case k_EResultInvalidCEGSubmission: return "InvalidCEGSubmission";
        case k_EResultRestrictedDevice: return "RestrictedDevice";
        case k_EResultRegionLocked: return "RegionLocked";
        case k_EResultRateLimitExceeded: return "RateLimitExceeded";
        case k_EResultAccountLoginDeniedNeedTwoFactor: return "AccountLoginDeniedNeedTwoFactor";
        case k_EResultItemDeleted: return "ItemDeleted";
        case k_EResultAccountLoginDeniedThrottle: return "AccountLoginDeniedThrottle";
        case k_EResultTwoFactorCodeMismatch: return "TwoFactorCodeMismatch";
        case k_EResultTwoFactorActivationCodeMismatch: return "TwoFactorActivationCodeMismatch";
        case k_EResultAccountAssociatedToMultiplePartners: return "AccountAssociatedToMultiplePartners";
        case k_EResultNotModified: return "NotModified";
        case k_EResultNoMobileDevice: return "NoMobileDevice";
        case k_EResultTimeNotSynced: return "TimeNotSynced";
        case k_EResultSmsCodeFailed: return "SmsCodeFailed";
        case k_EResultAccountLimitExceeded: return "AccountLimitExceeded";
        case k_EResultAccountActivityLimitExceeded: return "AccountActivityLimitExceeded";
        case k_EResultPhoneActivityLimitExceeded: return "PhoneActivityLimitExceeded";
        case k_EResultRefundToWallet: return "RefundToWallet";
        case k_EResultEmailSendFailure: return "EmailSendFailure";
        case k_EResultNotSettled: return "NotSettled";
        case k_EResultNeedCaptcha: return "NeedCaptcha";
        case k_EResultGSLTDenied: return "GSLTDenied";
        case k_EResultGSOwnerDenied: return "GSOwnerDenied";
        case k_EResultInvalidItemType: return "InvalidItemType";
        case k_EResultIPBanned: return "IPBanned";
        case k_EResultGSLTExpired: return "GSLTExpired";
        case k_EResultInsufficientFunds: return "InsufficientFunds";
        case k_EResultTooManyPending: return "TooManyPending";
        case k_EResultNoSiteLicensesFound: return "NoSiteLicensesFound";
        case k_EResultWGNetworkSendExceeded: return "WGNetworkSendExceeded";
        case k_EResultAccountNotFriends: return "AccountNotFriends";
        case k_EResultLimitedUserAccount: return "LimitedUserAccount";
        case k_EResultCantRemoveItem: return "CantRemoveItem";
        case k_EResultAccountDeleted: return "AccountDeleted";
        case k_EResultExistingUserCancelledLicense: return "ExistingUserCancelledLicense";
        case k_EResultCommunityCooldown: return "CommunityCooldown";
        case k_EResultNoLauncherSpecified: return "NoLauncherSpecified";
        case k_EResultMustAgreeToSSA: return "MustAgreeToSSA";
        case k_EResultLauncherMigrated: return "LauncherMigrated";
        case k_EResultSteamRealmMismatch: return "SteamRealmMismatch";
        case k_EResultInvalidSignature: return "InvalidSignature";
        case k_EResultParseFailure: return "ParseFailure";
        case k_EResultNoVerifiedPhone: return "NoVerifiedPhone";
        case k_EResultInsufficientBattery: return "InsufficientBattery";
        case k_EResultChargerRequired: return "ChargerRequired";
        case k_EResultCachedCredentialInvalid: return "CachedCredentialInvalid";
        case K_EResultPhoneNumberIsVOIP: return "PhoneNumberIsVOIP";
        case k_EResultNotSupported: return "NotSupported";
        case k_EResultFamilySizeLimitExceeded: return "FamilySizeLimitExceeded";
        case k_EResultOfflineAppCacheInvalid: return "OfflineAppCacheInvalid";
        case k_EResultTryLater: return "TryLater";
    }
    return "???";
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

bool integration_steam::supports_mod_uploading() {
    return true;
}

bool integration_steam::mod_already_uploaded(ModMetadata* mod) {
    std::string path_ini = mod->path + PATH_SEP + "mod.ini";
    IniManager mod_ini = IniManager(path_ini.c_str(), false);
    const char* store_steam_file_id = mod_ini.get("Integrations", "store_steam_file_id");
    return store_steam_file_id && 0 < strlen(store_steam_file_id);
}

integration_store_upload_mod_status integration_steam::upload_mod(ModMetadata* mod) {
    integration_store_upload_mod_status status;
    //Get mod data
    std::string path_ini = mod->path + PATH_SEP + "mod.ini";
    IniManager mod_ini = IniManager(path_ini.c_str(), false);
    
    //Trim long text and remove all color codes
    std::string title = string_remove_color_codes(mod->mod_name.c_str());
    std::string description = string_remove_color_codes(mod->mod_description_english.c_str());
    if (!title.empty()) {
        title = convertToUnicode(title, "english", '?', true);
    }
    if (!description.empty()) {
        description = convertToUnicode(description, "english", '?', true);
    }
    if (title.empty()) {
        fprintf(stderr, "Mod title is empty after conversion!");
        status.error = "ModTitleInvalid";
        return status;
    }
    if (description.empty()) {
        fprintf(stderr, "Mod description is empty after conversion!");
        status.error = "ModDescriptionInvalid";
        return status;
    }
    if (title.length() >= k_cchPublishedDocumentTitleMax) {
        title = title.substr(0, k_cchPublishedDocumentTitleMax);
    }
    if (description.length() >= k_cchPublishedDocumentDescriptionMax) {
        description = description.substr(0, k_cchPublishedDocumentDescriptionMax);
    }
    
    //Retrieve file id if set already
    bool new_item = false;
    const char* store_steam_file_id = mod_ini.get("Integrations", "store_steam_file_id");
    published_file_id = k_PublishedFileIdInvalid;
    if (store_steam_file_id && 0 < strlen(store_steam_file_id)) {
        uint64_t val = strtoull(store_steam_file_id, nullptr, 0);
        if (val == k_PublishedFileIdInvalid) {
            fprintf(stderr, "File ID in file is k_PublishedFileIdInvalid\n");
        } else {
            published_file_id = val;
        }
    } 

    if (published_file_id == k_PublishedFileIdInvalid) {
        //Create new item and store the published id
        printf("No item ID present, creating new item\n");
        create_item();
        if (published_file_id == k_PublishedFileIdInvalid) {
            fprintf(stderr, "File ID is k_PublishedFileIdInvalid\n");
            status.error = "ErrorGettingFileID";
            return status;
        }
        mod_ini.put("Integrations", "store_steam_file_id", std::to_string(published_file_id).c_str());
        mod_ini.save();
        new_item = true;
    }

    printf("Published file id to upload: %" PRIu64 "\n", (uint64_t) published_file_id);
    
    //Set data into item
    ISteamUGC* ugc = SteamUGC();
    UGCUpdateHandle_t update_handle = ugc->StartItemUpdate(INTEGRATION_STEAM_APPID, published_file_id);
    bool ok;
    //Sets the language of the title and description that will be set in this item update.
    ok = ugc->SetItemUpdateLanguage(update_handle, "english");
    if (!ok) {
        fprintf(stderr, "Error occurred setting item language\n");
        status.error = "ErrorSettingItemLanguage";
        return status;
    }
    ok = ugc->SetItemTitle(update_handle, title.c_str());
    if (!ok) {
        fprintf(stderr, "Error occurred setting item title\n");
        status.error = "ErrorSettingItemTitle";
        return status;
    }
    //Sets a new description for an item.
    ok = ugc->SetItemDescription(update_handle, description.c_str());
    if (!ok) {
        fprintf(stderr, "Error occurred setting item description\n");
        status.error = "ErrorSettingItemDescription";
        return status;
    }
    //Sets the visibility of an item if new, otherwise leave as is.
    if (new_item) {
        ok = ugc->SetItemVisibility(update_handle, k_ERemoteStoragePublishedFileVisibilityPrivate);
        if (!ok) {
            fprintf(stderr, "Error occurred setting item visibility\n");
            status.error = "ErrorSettingItemVisibility";
            return status;
        }
    }
    //Sets the folder that will be stored as the content for an item.
    std::string item_content_path = get_content_root_path_str();
    if (item_content_path.empty()) {
        item_content_path = std::filesystem::current_path().u8string();
    }
    terminate_with_char(item_content_path, PATH_SEP);
    item_content_path += mod->path;
    printf("Uploading mod content at: %s\n", item_content_path.c_str());
    ok = ugc->SetItemContent(update_handle, item_content_path.c_str());
    if (!ok) {
        fprintf(stderr, "Error occurred setting item content\n");
        status.error = "ErrorSettingItemContent";
        return status;
    }
    //Sets the primary preview image for the item.
    filesystem_entry* mod_preview = get_content_entry(mod->path + "mod.png");
    if (mod_preview) {
        std::string item_preview_path = get_content_root_path_str() + PATH_SEP + mod_preview->path_content;
        printf("Uploading mod preview image at: %s\n", item_preview_path.c_str());
        ok = ugc->SetItemPreview(update_handle, item_preview_path.c_str());
        if (!ok) {
            fprintf(stderr, "Error occurred setting item preview\n");
            status.error = "ErrorSettingItemPreview";
            return status;
        }
    }
    //Start the upload to workshop
    SteamAPICall_t SubmitItemUpdateAPICall = ugc->SubmitItemUpdate(update_handle, nullptr);
    SubmitItemUpdate_callback_called = false;
    SubmitItemUpdate_callback_result = k_EResultNone;
    m_SubmitItemUpdateResult.Set(SubmitItemUpdateAPICall, this, &integration_steam::OnSteamSubmitItemUpdate);
    status.in_progress = true;
    return status;
}

integration_store_upload_mod_status integration_steam::upload_mod_get_progress() {
    integration_store_upload_mod_status status;
    if (!SubmitItemUpdate_callback_called) {
        /*
        uint64 bytesProcessed = 0;
        uint64 bytesTotal = 0;
        EItemUpdateStatus status = ugc->GetItemUpdateProgress(update_handle, &bytesProcessed, &bytesTotal);
        printf(
            "Uploading status '%d' - %" PRIu64 "/%" PRIu64 " bytes\n",
            status, (uint64_t) bytesProcessed, (uint64_t) bytesTotal
        );
        */
        status.in_progress = true;
        return status;
    }
    
    status.in_progress = false;
    
    //Print any info relevant to the process result
    if (SubmitItemUpdate_callback_result != k_EResultOK) {
        printf("Steam SubmitItemUpdateResult returned result: %d - %s\n", SubmitItemUpdate_callback_result, string_of_result(SubmitItemUpdate_callback_result));
        status.error = string_of_result(SubmitItemUpdate_callback_result);
        return status;
    }
    
    printf("Finished uploading mod! Opening item page to setup further\n");
    std::string steam_item_url = "steam://url/CommunityFilePage/" + std::to_string((uint64_t) published_file_id);
    SteamFriends()->ActivateGameOverlayToWebPage(steam_item_url.c_str());
    return status;
}

void integration_steam::OnSteamSubmitItemUpdate(SubmitItemUpdateResult_t* pCallback, bool bIOFailure) {
    SubmitItemUpdate_callback_called = true;
    SubmitItemUpdate_callback_result = pCallback->m_eResult;
    if (bIOFailure) {
        fprintf(stderr, "Steam SubmitItemUpdateResult returned IO failure!");
        return;
    }
    if (pCallback->m_nPublishedFileId != published_file_id) {
        fprintf(
            stderr,
            "Steam SubmitItemUpdateResult_t->m_nPublishedFileId '%" PRIu64 "' doesn't match current file id '%" PRIu64 "'",
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
        printf("Steam CreateItemResult returned IO failure!");
        return;
    }
    if (pCallback->m_eResult != k_EResultOK) {
        printf("Steam CreateItemResult returned result: %d - %s", pCallback->m_eResult, string_of_result(pCallback->m_eResult));
        return;
    }
    published_file_id = pCallback->m_nPublishedFileId;
}

bool integration_steam::process_enabled_mods() {
    if (steam_disconnected) {
        return false;
    }
    ISteamUGC* ugc = SteamUGC();
    uint32_t num_item_allocated = ugc->GetNumSubscribedItems();
    if (0 == num_item_allocated) {
        return false;
    }
    PublishedFileId_t* subscribed_items = static_cast<PublishedFileId_t*>(malloc(sizeof(PublishedFileId_t) * num_item_allocated));
    uint32_t num_item = ugc->GetSubscribedItems(subscribed_items, num_item_allocated);
    if (num_item != num_item_allocated) {
        printf("GetSubscribedItems returned %u expected %u\n", num_item, num_item_allocated);
    }
    if (num_item > num_item_allocated) {
        num_item = num_item_allocated;
    }
    for (uint32_t i = 0; i < num_item; ++i) {
        this->process_workshop_mod(subscribed_items[i]);
    }
    
    //Free the array
    free(subscribed_items);
    
    return false;
}

void integration_steam::set_rich_presence(
    integration_rich_presence_activity activity,
    const char* activity_str,
    const char* activity_extra,
    const char* group_id,
    size_t group_size
) {
    if (steam_disconnected) {
        return;
    }
    ISteamFriends* frens = SteamFriends();
    frens->SetRichPresence("activity", activity_str);
    frens->SetRichPresence("extra", activity_extra);
    frens->SetRichPresence("steam_display", activity_extra ? "#Status_Extra" : "#Status");
    if (group_id && 0 < group_size) {
        frens->SetRichPresence("steam_player_group", group_id);
        frens->SetRichPresence("steam_player_group_size", std::to_string(group_size).c_str());
    }  else {
        frens->SetRichPresence("steam_player_group", nullptr);
        frens->SetRichPresence("steam_player_group_size", nullptr);
    }
}

bool integration_steam::process_workshop_mod(PublishedFileId_t item_id) {
    static char item_path[4096];
    if (item_id == k_PublishedFileIdInvalid) {
        return false;
    }
    ISteamUGC* ugc = SteamUGC();
    uint32 item_state = ugc->GetItemState(item_id);
    if (!(item_state & k_EItemStateInstalled)) {
        fprintf(stderr, "Item '%" PRIu64 "' is subscribed but not installed, skipping\n", (uint64_t) item_id);
        return false;
    }
    if (item_state & k_EItemStateLegacyItem) {
        fprintf(stderr, "Item '%" PRIu64 "' is marked as legacy, not supported\n", (uint64_t) item_id);
        return false;
    }

    if (!ugc->GetItemInstallInfo(item_id, nullptr, item_path, sizeof(item_path), nullptr)) {
        return false;
    }
    
    std::string mod_content_path = "mods" PATH_SEP_STR "steam_" + std::to_string(item_id);
    printf("Adding workshop item as mod at '%s' as '%s'\n", item_path, mod_content_path.c_str());
    //We set these options so that the remove_dir_content is set true and mod enable/disable works properly
    filesystem_scan_options scanOptions;
    scanOptions.verbose = content_debug_flag;
    scanOptions.replace_files = true;
    scanOptions.replace_dirs = false;
    scanOptions.remove_dir_content = true;
    scanOptions.prepend_destination_to_path = true;
    scan_resource_paths(mod_content_path, item_path, &scanOptions);
    ModMetadata mod;
    parseModInPath(mod, mod_content_path.c_str(), get_store_id());
    addPendingMod(mod);
    return true;
}

//Callbacks
void integration_steam::OnIPCFailure(IPCFailure_t* failure) {
    (void) failure;
    fprintf(stderr, "Steam IPC Failure\n");
    steam_disconnected = true;
}

void integration_steam::OnSteamShutdown(SteamShutdown_t* callback) {
    (void) callback;
    fprintf(stderr, "Steam shutdown requested\n");
    steam_disconnected = true;
    integration_requested_shutdown = true;
}

