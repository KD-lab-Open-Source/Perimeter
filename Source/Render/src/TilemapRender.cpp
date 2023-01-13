#include "StdAfxRD.h"
#include "TileMap.h"
#include "TilemapRender.h"

int cInterfaceRenderDevice::Create(cTileMap *TileMap)
{
#ifdef PERIMETER_D3D9
    if (gb_RenderDevice3D) {
        cTileMapRender* p = new cTileMapRender(TileMap);
        TileMap->SetTilemapRender(p);
        p->RestoreTilemapPool();
        tilemaps.push_back(p);
    }
#endif
    return 0;
}

int cInterfaceRenderDevice::Delete(cTileMap *TileMap)
{
#ifdef PERIMETER_D3D9
    cTileMapRender* p = TileMap->GetTilemapRender();
    if (p==NULL) {
        return true;
    }

    p->ClearTilemapPool();

    for(auto it = tilemaps.cbegin(); it != tilemaps.cend(); ++it) {
        if (*it == p) {
            tilemaps.erase(it);
            delete p;
            return true;
        }
    }

    VISASSERT(0);
#endif
    return false;
}
