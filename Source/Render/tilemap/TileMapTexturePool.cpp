#include <vector>
#include "StdAfxRD.h"
#include "TileMapTexturePool.h"

cTilemapTexturePool::cTilemapTexturePool(int w, int h, size_t texture_size)
{
    tileWidth = w;
    tileHeight = h;

    tileRealWidth= tileWidth + TEXTURE_BORDER * 2;
    tileRealHeight= tileHeight + TEXTURE_BORDER * 2;
    ustep= 1.0f / static_cast<float>(texture_size);
    vstep= 1.0f / static_cast<float>(texture_size);

    texture = new cTexture;
    texture->SetNumberMipMap(1);
    texture->SetAttribute(TEXTURE_ALPHA_BLEND);
    texture->New(1);
    texture->SetTimePerFrame(0);
    texture->SetWidth(static_cast<int>(texture_size));
    texture->SetHeight(static_cast<int>(texture_size));
    gb_RenderDevice->CreateTexture(texture, nullptr);

    xassert(tileRealWidth < texture->GetWidth() && tileRealHeight < texture->GetHeight());

    CalcPagesPos();

    freePagesList = new int[Pages.size()];
    freePages = Pages.size();
    for (int i = 0; i < Pages.size(); i++) {
        freePagesList[i] = i;
    }
}

cTilemapTexturePool::~cTilemapTexturePool() {
    delete[] freePagesList;
    if (texture) {
        texture->Release();
    }
}

void cTilemapTexturePool::CalcPagesPos()
{
    int h = texture->GetHeight();
    int w = texture->GetWidth();
    for(int y = 0; y <= h - tileRealHeight; y += tileRealHeight) {
        for(int x = 0; x <= w - tileRealHeight; x += tileRealWidth) {
            Vect2i page;
            page.x=x;
            page.y=y;
            Pages.emplace_back(page);
        }
    }
}

int cTilemapTexturePool::allocPage() {
    if (!freePages) return -1;
    freePages--;
    return freePagesList[freePages];
}

void cTilemapTexturePool::freePage(int page) {
    freePagesList[freePages] = page;
    freePages++;
}

void* cTilemapTexturePool::lockPage(int page, int& Pitch) {
    Vect2i pos=Pages[page];
    return gb_RenderDevice->LockTextureRect(texture, Pitch, pos, { tileRealWidth, tileRealHeight });
}

void cTilemapTexturePool::unlockPage(int page)
{
    texture->UnlockTexture();
}

Vect2f cTilemapTexturePool::getUVStart(int page)
{
    VISASSERT(page>=0 && page<Pages.size());
    Vect2i pos=Pages[page];
    return Vect2f((pos.x + TEXTURE_BORDER) * ustep, (pos.y + TEXTURE_BORDER) * vstep);
}
