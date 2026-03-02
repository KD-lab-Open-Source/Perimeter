#include <string>
#include <cstdint>

#include "integrations.h"
#ifdef PERIMETER_INTEGRATION_STEAM
#include "Steam/integration_steam_public.h"
#endif

static integration_store* store;

void integrations::init() {
    close();
#ifdef PERIMETER_INTEGRATION_STEAM
    store = integration_steam_init();
#endif
}

void integrations::close() {
    if (store) {
        delete store;
        store = nullptr;
    }
}

void integrations::quant() {
    if (store) {
        store->quant();
    }
}

integration_store* integrations::get_store() {
    return store;
}

bool integrations::game_will_relaunch() {
#ifdef PERIMETER_INTEGRATION_STEAM
    if (integration_steam_game_will_relaunch()) return true;
#endif
    return false;
}
