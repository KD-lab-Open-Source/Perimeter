#ifndef PERIMETER_INTEGRATIONS_H
#define PERIMETER_INTEGRATIONS_H

class integration_store {
public:
    virtual ~integration_store() = default;
    virtual void close() {}
    virtual std::string get_player_name() = 0;
    virtual std::string get_selected_locale() = 0;
};

class integrations {
public:
    static void init();
    static void close();
    static integration_store* get_store();
    static bool game_will_relaunch();
};

#endif //PERIMETER_INTEGRATIONS_H