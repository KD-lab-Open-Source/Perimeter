#ifndef PERIMETER_INTEGRATION_STEAM_H
#define PERIMETER_INTEGRATION_STEAM_H

static const uint32_t INTEGRATION_STEAM_APPID = 2530170;

class integration_steam: public integration_store {
    bool steam_disconnected = false;
    PublishedFileId_t published_file_id = k_PublishedFileIdInvalid; 
    bool need_accept_workshop_legal_agreement = false;

    void OnSteamCreateItem(CreateItemResult_t *pCallback, bool bIOFailure);
    CCallResult<integration_steam, CreateItemResult_t> m_CreateItemResult = {};
    bool CreateItem_callback_called = false;
    void create_item();
    

    void OnSteamSubmitItemUpdate(SubmitItemUpdateResult_t *pCallback, bool bIOFailure);
    CCallResult<integration_steam, SubmitItemUpdateResult_t> m_SubmitItemUpdateResult = {};
    bool SubmitItemUpdate_callback_called = false;
    EResult SubmitItemUpdate_callback_result = k_EResultNone;
    
    bool process_workshop_mod(PublishedFileId_t item_id);
    
    //Callbacks from steam
    STEAM_CALLBACK(integration_steam, OnIPCFailure, IPCFailure_t);
    STEAM_CALLBACK(integration_steam, OnSteamShutdown, SteamShutdown_t);    
public:
    void close() override;
    void quant() override;
    const char* get_store_id() override;
    std::string get_player_name() override;
    std::string get_selected_locale() override;
    void upload_mod(struct ModMetadata* mod) override;
    bool process_enabled_mods() override;
    ///Sets the rich presence state
    void set_rich_presence(
        integration_rich_presence_activity activity,
        const char* activity_str,
        const char* activity_extra,
        const char* group_id,
        size_t group_size
    ) override;
};

#endif //PERIMETER_INTEGRATION_STEAM_H