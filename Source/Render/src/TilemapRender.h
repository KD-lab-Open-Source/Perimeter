#ifndef PERIMETER_TILEMAPRENDER_H
#define PERIMETER_TILEMAPRENDER_H

static const int bumpGeoScale[TILEMAP_LOD] = { 1, 2, 3, 4, 5 };
static const int bumpTexScale[TILEMAP_LOD] = { 0, 0, 1, 2, 3 };

struct sPlayerIB
{
#ifdef PERIMETER_D3D9
    IndexPoolPage index;
#endif
    int nindex;
    int player;
};

struct VectDelta: public Vect2i
{
    int delta2;
    Vect2i delta;
    char player;

    enum
    {
        FIX_RIGHT=1,
        FIX_BOTTOM=2,
        FIX_FIXED=4,
        FIX_FIXEMPTY=8,
    };
    char fix;

    void copy_no_fix(const VectDelta& p)
    {
        x=p.x;
        y=p.y;
        delta2=p.delta2;
        delta=p.delta;
        player=p.player;
    }
};

struct sBumpTile
{
    //Only to read
#ifdef PERIMETER_D3D9
    VertexPoolPage vtx;
#endif
    std::vector<sPlayerIB> index;
    Vect2i tile_pos;

    bool init;
    int age, LOD;
    int texPool, texPage;

    enum
    {
        U_LEFT=0,
        U_RIGHT=1,
        U_TOP=2,
        U_BOTTOM=3,
        U_ALL=4,
    };

    char border_lod[U_ALL];
protected:
    float vStart, vStep, uStart, uStep;
public:
    sBumpTile(cTileMap *TileMap, int lod,int xpos,int ypos);
    ~sBumpTile();
#ifdef PERIMETER_D3D9
    D3DLOCKED_RECT *LockTex();
#endif
    uint8_t *LockVB();
    void UnlockTex();
    void UnlockVB();
    void Calc(cTileMap *TileMap,bool update_texture);

    void FindFreeTexture(int& Pool,int& Page,int tex_width,int tex_height);

    inline Vect2f GetUVStart(){return Vect2f(uStart,vStart);};
    inline Vect2f GetUVStep()
    {
        float div=1/(float)(1 << bumpGeoScale[LOD]);
        return Vect2f(uStep * div,vStep*div);
    };

    inline bool IsZeroplast()
    {
        int sz=index.size();
        if(sz>1)
            return true;
        if(sz==1)
            return index[0].player>=0;
        return false;
    }

    inline bool IsOnlyZeroplast()
    {
        return index.size()==1 && index[0].player>=0;
    }
protected:
    void CalcTexture(cTileMap *TileMap);
    void CalcPoint(cTileMap *TileMap);

    int FixLine(VectDelta* points,int ddv,cTileMap *TileMap);

    inline float SetVertexZ(TerraInterface* terra,int x,int y);
    void DeleteIndex();
};

class sTilemapTexturePool
{
#ifdef PERIMETER_D3D9
    LPDIRECT3DTEXTURE9 texture;
#endif
    int tileWidth, tileHeight;
    int tileRealWidth, tileRealHeight;
    int freePages;
    int *freePagesList;

    int texture_width,texture_height;
    std::vector<Vect2i> Pages;//x-xmin, y-ymin
    float ustep,vstep;
public:
    std::vector<sBumpTile*> tileRenderList;

    sTilemapTexturePool(int w, int h);
    ~sTilemapTexturePool();
    int allocPage();
    void freePage(int page);
#ifdef PERIMETER_D3D9
    D3DLOCKED_RECT *lockPage(int page);
#endif
    void unlockPage(int page);
    Vect2f getUVStart(int page);
    inline float getVStep(){return vstep;};
    inline float getUStep(){return ustep;};

    inline int GetTileWidth(){return tileWidth;}
    inline int GetTileHeight(){return tileHeight;}
    inline int IsFree(){return freePages;}
#ifdef PERIMETER_D3D9
    inline LPDIRECT3DTEXTURE9 GetTexture(){return texture;}
#endif

    void GetTextureMemory(int &total,int& free);
protected:
    void CalcPagesPos();
};

class cTileMapRender
{
    cTileMap *TileMap;
    std::vector<sBumpTile*> bumpTiles;
    std::vector<int> bumpDyingTiles;
#ifdef PERIMETER_D3D9
    IDirect3DIndexBuffer9* tilemapIB;
#endif
    int index_offset[TILEMAP_LOD];
    int index_size[TILEMAP_LOD];

    uint8_t* visMap;
    char* vis_lod;

    char* update_stat;
    bool update_in_frame;

    void SaveUpdateStat();

    VectDelta* delta_buffer;
    std::vector<std::vector<sPolygon>> index_buffer;
public:
    void IncUpdate(sBumpTile* pbump);

    cTileMapRender(cTileMap *pTileMap);
    ~cTileMapRender();

    void ClearTilemapPool();
    void RestoreTilemapPool();

    int bumpNumIndex(int lod){return index_size[lod];};
    int bumpIndexOffset(int lod){return index_offset[lod];};

    int bumpNumVertices(int lod);
    void bumpCreateIB(sPolygon* ib, int lod);
    int bumpTileValid(int id);
    int bumpTileAlloc(int lod,int xpos,int ypos);
    void bumpTileFree(int id);
    void bumpTilesDeath();

    void PreDraw(cCamera* DrawNode);
    void DrawBump(cCamera* DrawNode,eBlendMode MatMode,TILEMAP_DRAW tile_draw,bool shadow);

    sBumpTile* GetTile(int k,int n)
    {
        VISASSERT(k>=0 && k<TileMap->GetTileNumber().x);
        VISASSERT(n>=0 && n<TileMap->GetTileNumber().y);
        int bumpTileID = TileMap->GetTile(k, n).bumpTileID;
        if(bumpTileID>=0)
            return bumpTiles[bumpTileID];
        return NULL;
    }

    VectDelta* GetDeltaBuffer(){return delta_buffer;};
    std::vector<std::vector<sPolygon> >& GetIndexBuffer(){return index_buffer;};
};

#endif //PERIMETER_TILEMAPRENDER_H
