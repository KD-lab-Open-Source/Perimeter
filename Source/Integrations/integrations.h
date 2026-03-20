#ifndef PERIMETER_INTEGRATIONS_H
#define PERIMETER_INTEGRATIONS_H

enum integration_rich_presence_activity {
    RichPresenceActivityMenu,
    RichPresenceActivityCampaign,
    RichPresenceActivityBattle,
    RichPresenceActivityMultiplayerList,
    RichPresenceActivityMultiplayerWaiting,
    RichPresenceActivityMultiplayerPlaying,
};

struct integration_store_upload_mod_status {
    bool in_progress = false;
    const char* error = nullptr;
};

class integration_store { 
public:
    ///Flag to know if integration requests a shutdown
    bool integration_requested_shutdown = false;
    ///Destructor
    virtual ~integration_store() = default;
    ///Called when closing store
    virtual void close() {}
    ///Called on each frame by the main thread to process any store stuff
    virtual void quant() {}
    ///The string identifying the implementation of this store, this must be a lowercase string constant
    virtual const char* get_store_id() = 0;
    ///Return player name to use as default if any, empty if none or not known
    virtual std::string get_player_name() = 0;
    ///Return locale selected in store if any, empty if no value
    virtual std::string get_selected_locale() = 0;
    ///Returns true if mod uploading is supported on this store
    virtual bool supports_mod_uploading() = 0;
    ///Upload a new or update a mod to store
    ///Returns false if error, true if upload started, the progress will be reported by upload_mod_get_progress()
    virtual integration_store_upload_mod_status upload_mod(struct ModMetadata* mod) = 0;
    ///Upload mod status and progress
    virtual integration_store_upload_mod_status upload_mod_get_progress() = 0;
    ///Returns true if mod is already uploaded and we are simply updating
    virtual bool mod_already_uploaded(struct ModMetadata* mod) = 0;
    ///Processes any mods enabled by store, returns true if restart is required
    virtual bool process_enabled_mods() = 0;
    ///Sets the rich presence state
    virtual void set_rich_presence(
        integration_rich_presence_activity activity,
        const char* activity_str,
        const char* activity_extra,
        const char* group_id,
        size_t group_size
    ) = 0;
};

class integrations {
public:
    static void init();
    static void close();
    ///Called on each frame by the main thread to process any integrations stuff, return false to stop the game loop
    static bool quant();
    ///Return the active store if any
    static integration_store* get_store();
    ///Some integrations may require game shutdown as will be relaunched immediately externally
    ///Returns true if game must shut down asap
    static bool game_will_relaunch();
    ///Sets rich presence state if integration is available
    static void set_rich_presence(
        integration_rich_presence_activity activity,
        const char* activity_extra = nullptr,
        const char* group_id = nullptr,
        size_t group_size = 0
    );
};

#endif //PERIMETER_INTEGRATIONS_H