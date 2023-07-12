#ifndef PERIMETER_TILEMAPBUMPTILE_H
#define PERIMETER_TILEMAPBUMPTILE_H

static const int bumpGeoScale[TILEMAP_LOD] = { 1, 2, 3, 4, 5 };
static const int bumpTexScale[TILEMAP_LOD] = { 0, 0, 1, 2, 3 };

#define BUMP_VTXTYPE   sVertexXYZT1

class cTileMap;

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

struct sPlayerIB
{
    IndexPoolPage index;
    int nindex;
    int player;
};

struct sBumpTile
{
    //Only to read
    VertexPoolPage vtx;
    std::vector<sPlayerIB> index;
    Vect2i tile_pos;

    bool init;
    int age, LOD;

    class cTileMap *tilemap;
    class cTilemapTexturePool* texPool;
    int texPage = 0;

    enum
    {
        U_LEFT=0,
        U_RIGHT=1,
        U_TOP=2,
        U_BOTTOM=3,
        U_ALL=4,
    };

    char border_lod[U_ALL];

    void ToList();

protected:
    float vStart, vStep, uStart, uStep;
public:
    sBumpTile(cTileMap* TileMap, cTilemapTexturePool* pool, int lod, int xpos, int ypos);
    ~sBumpTile();
    uint8_t* LockTex(int& Pitch);
    uint8_t* LockVB();
    void UnlockTex();
    void UnlockVB();
    void Calc(bool update_texture);

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
    void CalcTexture();
    void CalcPoint();

    int FixLine(VectDelta* points, int ddv);

    inline float SetVertexZ(TerraInterface* terra,int x,int y);
    void DeleteIndex();
};

#endif //PERIMETER_TILEMAPBUMPTILE_H
