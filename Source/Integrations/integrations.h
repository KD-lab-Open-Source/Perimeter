#ifndef PERIMETER_INTEGRATIONS_H
#define PERIMETER_INTEGRATIONS_H

class integration_store {
public:
    virtual ~integration_store() = default;
    virtual void close() {}
    ///Called on each frame by the main thread to process any store stuff
    virtual void quant() {}
    ///The string identifying the implementation of this store, this must be a lowercase string constant
    virtual const char* get_store_id() = 0;
    ///Return player name to use as default if any, empty if none or not known
    virtual std::string get_player_name() = 0;
    ///Return locale selected in store if any, empty if no value
    virtual std::string get_selected_locale() = 0;
    ///Upload a new or update a mod to store
    virtual void upload_mod(struct ModMetadata* mod) = 0;
    ///Processes any mods enabled by store, returns true if restart is required
    virtual bool process_enabled_mods() = 0;
};

class integrations {
public:
    static void init();
    static void close();
    ///Called on each frame by the main thread to process any integrations stuff
    static void quant();
    ///Return the active store if any
    static integration_store* get_store();
    ///Some integrations may require game shutdown as will be relaunched immediately externally
    ///Returns true if game must shut down asap
    static bool game_will_relaunch();
};

#endif //PERIMETER_INTEGRATIONS_H