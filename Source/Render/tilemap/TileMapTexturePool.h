#ifndef PERIMETER_TILEMAPTEXTUREPOOL_H
#define PERIMETER_TILEMAPTEXTUREPOOL_H

class cTilemapTexturePool
{
    class cTexture* texture;
    int tileWidth, tileHeight;
    int tileRealWidth, tileRealHeight;
    int freePages;
    int *freePagesList;

    std::vector<Vect2i> Pages;//x-xmin, y-ymin
    float ustep,vstep;
public:
    static const int TEXTURE_BORDER = 1;
    
    std::vector<struct sBumpTile*> tileRenderList;

    cTilemapTexturePool(int w, int h, size_t texture_size);
    ~cTilemapTexturePool();
    int allocPage();
    void freePage(int page);
    void* lockPage(int page, int& Pitch);
    void unlockPage(int page);
    Vect2f getUVStart(int page);
    inline float getVStep(){return vstep;};
    inline float getUStep(){return ustep;};

    inline int GetTileWidth(){return tileWidth;}
    inline int GetTileHeight(){return tileHeight;}
    inline bool IsFree(){return 0 < freePages;}
    inline cTexture* GetTexture() { return texture; }

    //void GetTextureMemory(int &total,int& free);
protected:
    void CalcPagesPos();
};


#endif //PERIMETER_TILEMAPTEXTUREPOOL_H
