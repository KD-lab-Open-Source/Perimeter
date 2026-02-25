#ifndef PERIMETER_INTEGRATIONSTEAM_H
#define PERIMETER_INTEGRATIONSTEAM_H

static const uint32_t INTEGRATION_STEAM_APPID = 2530170;

class integration_steam: public integration_store {
public:
    static bool game_will_relaunch();
    static integration_steam* init();
    void close() override;
    std::string get_player_name() override;
    std::string get_selected_locale() override;
};

#endif //PERIMETER_INTEGRATIONSTEAM_H