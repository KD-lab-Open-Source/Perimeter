#include "StdAfxRD.h"
#include "D3DRender.h"
#include "tilemap/TileMap.h"
#include "tilemap/TileMapRender.h"
#include "tilemap/TileMapTexturePool.h"
#include "Scene.h"

#include "../../Game/Region.h"

#include <set>


//
// **************** TILEMAP - SHARED ****************
//
/*
void cD3DRender::GetTilemapTextureMemory(int& total,int& free)
{
	std::vector<sTilemapTexturePool*>::iterator it;
	total=free=0;
	FOR_EACH(bumpTexPools,it)
	{
		int cur_total,cur_free;
		(*it)->GetTextureMemory(cur_total,cur_free);
		total+=cur_total;
		free+=cur_free;
	}
}

void sTilemapTexturePool::GetTextureMemory(int &total,int& free)
{
	total=texture_width*texture_height*4;
	free=freePages*tileWidth*tileHeight*4;
}
*/

int cD3DRender::CreateTilemap(cTileMap *TileMap)
{
    int err = cInterfaceRenderDevice::CreateTilemap(TileMap);
    if (err == 0) {
        cTileMapRender* p = TileMap->GetTilemapRender();
        xassert(p);
        if (p) {
            tilemaps.push_back(p);
        }
    }
    return err;
}

int cD3DRender::DeleteTilemap(cTileMap *TileMap)
{
    cTileMapRender* p = TileMap->GetTilemapRender();
    if (p) {
        auto removed = std::remove(tilemaps.begin(), tilemaps.end(), p);
        tilemaps.erase(removed, tilemaps.end());
    }
    return cInterfaceRenderDevice::DeleteTilemap(TileMap);
}

void cD3DRender::ClearTilemapPool()
{
    for (auto tilemap : tilemaps) {
        tilemap->ClearTilemapPool();
    }
}

void cD3DRender::RestoreTilemapPool()
{
	for (auto tilemap : tilemaps) {
        tilemap->RestoreTilemapPool();
    }
}
