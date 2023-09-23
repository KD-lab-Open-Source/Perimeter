#include "StdAfxRD.h"
#include "PoolManager.h"
#include "TileMap.h"
#include "TileMapTexturePool.h"
#include "TileMapBumpTile.h"
#include "TileMapRender.h"
#include "FileImage.h"

#ifdef PERIMETER_D3D9
#include "D3DRender.h"
#endif

int cInterfaceRenderDevice::CreateTilemap(cTileMap *TileMap)
{
    cTileMapRender* p = new cTileMapRender(TileMap);
    TileMap->SetTilemapRender(p);
    p->RestoreTilemapPool();
    return 0;
}

int cInterfaceRenderDevice::DeleteTilemap(cTileMap *TileMap)
{
    cTileMapRender* p = TileMap->GetTilemapRender();

    if (p) {
        p->ClearTilemapPool();
        TileMap->SetTilemapRender(nullptr);
        delete p;
    }

    return true;
}

cTileMapRender::cTileMapRender(cTileMap *pTileMap)
{
    tilemap=pTileMap;

    int dxy= tilemap->GetTileNumber().x * tilemap->GetTileNumber().y;
    visMap=new uint8_t[dxy];
    vis_lod=new char[dxy];
    for(int i=0;i<dxy;i++)
        vis_lod[i]=-1;

    update_stat=NULL;
//	update_stat=new char[dxy*TILEMAP_LOD];
    update_in_frame=false;

    int ddv=TILEMAP_SIZE+1;
    delta_buffer=new VectDelta[ddv*ddv];
}

cTileMapRender::~cTileMapRender()
{
    delete[] visMap;
    delete[] vis_lod;

    delete[] update_stat;
    delete[] delta_buffer;
    
    ClearTilemapPool();
}

void cTileMapRender::ClearTilemapPool()
{
    for (int y=0; y < tilemap->GetTileNumber().y; y++) {
        for (int x = 0; x < tilemap->GetTileNumber().x; x++) {
            sTile& Tile = tilemap->GetTile(x, y);
            int& bumpTileID = Tile.bumpTileID;
            bumpTileID = -1;
        }
    }

    bumpDyingTiles.clear();
    
    for(int i=0;i<bumpTiles.size();i++)
    {
        delete bumpTiles[i];
    }
    bumpTiles.clear();

    if (tilemapIB.data) {
        gb_RenderDevice->DeleteIndexBuffer(tilemapIB);
    }
    
    int i;
    for(i=0;i<bumpTexPools.size();i++) {
        delete bumpTexPools[i];
    }
    bumpTexPools.clear();
    if (vertexPoolManager) {
        delete vertexPoolManager;
        vertexPoolManager = nullptr;
    }
    if (indexPoolManager) {
        delete indexPoolManager;
        indexPoolManager = nullptr;
    }
}

void cTileMapRender::RestoreTilemapPool()
{
    if (!vertexPoolManager) {
        vertexPoolManager = new VertexPoolManager();
    }
    if (!indexPoolManager) {
        indexPoolManager = new IndexPoolManager();
    }

    int cur_offset=0;
    Vect2i TileSize=tilemap->GetTileSize();

    for (int iLod = 0; iLod < TILEMAP_LOD; iLod++)
    {
        int xs = TileSize.x >> bumpGeoScale[iLod];
        int ys = TileSize.y >> bumpGeoScale[iLod];

        index_offset[iLod]=cur_offset;
        index_size[iLod]=6*xs*ys;//?????? может 2 а не 6
        cur_offset+=index_size[iLod];
    }

    if (!tilemapIB.data) {
        gb_RenderDevice->CreateIndexBuffer(tilemapIB, cur_offset, false);
        //TODO use indites_t* and set them one by one instead of sPolygon set()
        sPolygon *pIndex = reinterpret_cast<sPolygon*>(gb_RenderDevice->LockIndexBuffer(tilemapIB));
        for (int iLod = 0; iLod < TILEMAP_LOD; iLod++)
        {
            bumpCreateIB(pIndex+bumpIndexOffset(iLod)/3, iLod);
        }
        gb_RenderDevice->UnlockIndexBuffer(tilemapIB);
    }
}

void cTileMapRender::bumpCreateIB(sPolygon *ib, int lod)
{
    Vect2i TileSize=tilemap->GetTileSize();
    VISASSERT(TileSize.x==TileSize.y);

    int dd = (TileSize.x >> bumpGeoScale[lod]);
    int ddv=dd+1;
    for(int y=0;y<dd;y++) {
        for (int x = 0; x < dd; x++) {
            int base = x + y * ddv;
            ib++->set(base, base + ddv, base + 1);
            ib++->set(base + ddv, base + ddv + 1, base + 1);
        }
    }

}

void cTileMapRender::PreDraw(cCamera* DrawNode)
{
    for(int y=0; y < tilemap->GetTileNumber().y; y++)
        for(int x=0; x < tilemap->GetTileNumber().x; x++)
        {
            sTile &Tile = tilemap->GetTile(x, y);
            int &bumpTileID = Tile.bumpTileID;
            if(!Tile.GetAttribute(ATTRTILE_DRAWLOD))
            {
                bumpTileFree(bumpTileID);
                bumpTileID = -1;
            }

            Tile.ClearAttribute(ATTRTILE_DRAWLOD);
        }

    if(update_stat)
    {
        SaveUpdateStat();
        update_in_frame=false;
        int dxy= tilemap->GetTileNumber().x * tilemap->GetTileNumber().y;
        memset(update_stat,0,dxy*TILEMAP_LOD);
    }

    bumpTilesDeath();
}

int cTileMapRender::bumpNumVertices(int lod)
{
    int xs = (tilemap->GetTileSize().x >> bumpGeoScale[lod]);
    int ys = (tilemap->GetTileSize().y >> bumpGeoScale[lod]);

    return (xs+1)*(ys+1);
}

int cTileMapRender::bumpTileValid(int id)
{
    return ((id >= 0) && (id < bumpTiles.size()) && (bumpTiles[id]));
}

cTilemapTexturePool* cTileMapRender::FindFreeTexturePool(int tex_width, int tex_height) {
    int i;
    for (i = 0; i < bumpTexPools.size(); i++) {
        if (bumpTexPools[i] && bumpTexPools[i]->IsFree()
            && bumpTexPools[i]->GetTileWidth() == tex_width
            && bumpTexPools[i]->GetTileHeight() == tex_height) {
            break;
        }
    }
    
    if (i >= bumpTexPools.size()) {
        for (i = 0; i < bumpTexPools.size(); i++) {
            if (bumpTexPools[i] == nullptr) break;
        }
        if (i >= bumpTexPools.size()) {
            i = bumpTexPools.size();
            bumpTexPools.push_back(nullptr);
        }
        bumpTexPools[i] = new cTilemapTexturePool(tex_width, tex_height, tilemap->GetTexturePoolSize());
    }

    return bumpTexPools[i];
}

int cTileMapRender::bumpTileAlloc(int lod,int xpos,int ypos)
{

    int w = tilemap->GetTileSize().x >> bumpTexScale[lod];
    int h = tilemap->GetTileSize().y >> bumpTexScale[lod];
    cTilemapTexturePool* pool = FindFreeTexturePool(w, h);
    sBumpTile* tile = new sBumpTile(tilemap, pool, lod, xpos, ypos);
    int i;
    for (i = 0; i < bumpTiles.size(); i++) {
        if (!bumpTiles[i]) {
            break;
        }
    }
    if (i >= bumpTiles.size()) {
        i = bumpTiles.size();
        bumpTiles.push_back(nullptr);
    }
    bumpTiles[i] = tile;
    return i;
}

void cTileMapRender::bumpTileFree(int id)
{
    if (bumpTileValid(id))
        bumpDyingTiles.push_back(id);
}

void cTileMapRender::bumpTilesDeath()
{
    for (int i = 0; i < bumpDyingTiles.size(); i++)
    {
        int tile=bumpDyingTiles[i];
        if (++bumpTiles[tile]->age > 4)
        {
            delete bumpTiles[tile];
            bumpTiles[tile] = NULL;
            bumpDyingTiles.erase(bumpDyingTiles.begin() + i);
            i--;
        }
    }
}

void cTileMapRender::IncUpdate(sBumpTile* pbump)
{
    if(update_stat)
    {
        int dx=tilemap->GetTileNumber().x;
        int dxy= dx * tilemap->GetTileNumber().y;
        update_in_frame=true;
        update_stat[pbump->LOD*dxy+pbump->tile_pos.y*dx+pbump->tile_pos.x]++;
    }
}

void cTileMapRender::SaveUpdateStat()
{
    static int tga_num=-1;
    tga_num++;
    if(!update_in_frame)return;
    int dx=tilemap->GetTileNumber().x,dy=tilemap->GetTileNumber().y;
    int dxy=dx*dy;

    uint8_t* buf=new uint8_t[dxy * 4];
    memset(buf,0,dxy*4);
    for(int y=0;y<dy;y++)
        for(int x=0;x<dx;x++)
        {
            uint8_t* color=&buf[(y * dx + x) * 4];
            int num_update=0;
            int lod=-1;
            bool change_lod=false;
            for(int i=0;i<TILEMAP_LOD;i++)
            {
                char u=update_stat[i*dxy+y*dx+x];
                num_update+=u;
                if(u)
                {
                    if(lod==-1)
                        lod=i;
                    else
                        change_lod=true;
                }
            }

            if(change_lod)
                color[0]=128;
            color[1]=min(num_update,4)*63;
        }

    char fname[100];
    sprintf(fname,"tga\\%05i.tga",tga_num);
    SaveTGA(fname,dx,dy,buf,4);
    delete[] buf;
}
void cTileMapRender::DrawBump(cCamera* DrawNode,eBlendMode MatMode,TILEMAP_DRAW tile_draw,bool shadow)
{
    cCamera* pShadowMapCamera=DrawNode->FindCildCamera(ATTRCAMERA_SHADOWMAP);
    int reflection = DrawNode->GetAttribute(ATTRCAMERA_REFLECTION);
    cCamera* pNormalCamera=DrawNode->GetRoot();
    cTileMapRender* render=tilemap->GetTilemapRender();
    bool use_shadow_map=false;
    
    //TODO remove this once D3D9 specifics are removed
    if (shadow || reflection) {
#ifdef PERIMETER_D3D9
        if (!gb_RenderDevice3D) {
            return;
        }
#else
        return;
#endif
    }

    Vect3f dcoord(
            tilemap->GetTileSize().x * tilemap->GetScale().x,
            tilemap->GetTileSize().y * tilemap->GetScale().y,
            255 * tilemap->GetScale().z),
            coord(0, 0, 0);

    gb_RenderDevice->SetWorldMatXf(MatXf::ID);

    // setup rendering
    if(tilemap->GetShadowMap() && !shadow && Option_ShadowType == SHADOW_MAP &&
       //gb_RenderDevice3D->dtAdvance==gb_RenderDevice3D->dtFixed && 
       !reflection)
    {
        gb_RenderDevice->SetNoMaterial(MatMode, 0, 0, tilemap->GetShadowMap(), COLOR_MOD);

#ifdef PERIMETER_D3D9
        if (gb_RenderDevice3D) {
            cTexture* pShadowMap=pShadowMapCamera->GetRenderTarget();
            int width=pShadowMap->GetWidth();
            float tOffs = 0;//(0.5f / width);
            Mat4f matTexAdj(0.5f,    0.0f,        0.0f,        0.0f,
                            0.0f,   -0.5f,        0.0f,        0.0f,
                            0.0f,    0.0f,        0.0f,        0.0f,
                            0.5f+tOffs,0.5f +tOffs, 1.0f,        1.0f);
    
            Mat4f mat = pShadowMapCamera->matViewProj * matTexAdj;

            // set texture coord 1
            gb_RenderDevice3D->SetTextureTransform(1, &mat);

            gb_RenderDevice3D->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
            gb_RenderDevice3D->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
            gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
            gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

            gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MINFILTER, gb_RenderDevice3D->texture_interpolation);
            gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MAGFILTER, gb_RenderDevice3D->texture_interpolation);
        }
#endif
    }else
    if(tilemap->GetShadowMap() && !shadow && (Option_ShadowType != SHADOW_MAP && Option_ShadowType != SHADOW_MAP_PERSPECTIVE) && !reflection)
    {

        gb_RenderDevice->SetNoMaterial(MatMode, 0, 0, tilemap->GetShadowMap(), COLOR_MOD4);

#ifdef PERIMETER_D3D9
        if (gb_RenderDevice3D) {
            // set texture coord 1
            gb_RenderDevice3D->SetTextureTransform(1, &tilemap->matLightMap);

            gb_RenderDevice3D->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
            gb_RenderDevice3D->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
            gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
            gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        }
#endif

    } else
    {
        gb_RenderDevice->SetNoMaterial(MatMode);
    }
    
#ifdef PERIMETER_D3D9
    if (gb_RenderDevice3D) {
        gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
        //gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

        gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    }
#endif

    cTileMap::calcVisMap(DrawNode, tilemap->GetTileNumber(), tilemap->GetTileSize(), visMap, true);

#ifdef PERIMETER_D3D9
    uint32_t minfilter1=gb_RenderDevice3D->GetSamplerState(1, D3DSAMP_MINFILTER);
    uint32_t magfilter1=gb_RenderDevice3D->GetSamplerState(1, D3DSAMP_MAGFILTER);
#endif

    uint32_t zfunc=gb_RenderDevice->GetRenderState(RS_ZFUNC);
    uint32_t cull=gb_RenderDevice->GetRenderState(RS_CULLMODE);
//	gb_RenderDevice->SetRenderState( RS_CULLMODE, CULL_NONE );

    if(shadow)
    {
        if(DrawNode->GetAttribute(ATTRCAMERA_ZINVERT))
        {
            gb_RenderDevice->SetRenderState(RS_ZFUNC,CMP_GREATEREQUAL);
            gb_RenderDevice->SetRenderState(RS_CULLMODE,CULL_CCW);
        }
        
        gb_RenderDevice->SetMaterialTilemapShadow();
    } else {
        if(pShadowMapCamera && !reflection)
        {
            gb_RenderDevice->SetMaterialTilemap(tilemap);
            use_shadow_map=true;
        }
    }

//start_timer(Calc_TileMap, 1);

    // clear render lists
    int i;
    for (i = 0; i < bumpTexPools.size(); i++)
        bumpTexPools[i]->tileRenderList.clear();

    // create/update tiles' vertices/textures
    int n,k;
    int dn=tilemap->GetTileNumber().y,dk=tilemap->GetTileNumber().x;
    float lod_focus=Option_MapLevel * pNormalCamera->GetFocusViewPort().x;
    float DistLevelDetail[TILEMAP_LOD] = { // was: 1,2,4,6
            0.5f * lod_focus,
            1.5f * lod_focus,
            3.0f * lod_focus,
            6.0f * lod_focus,
            12.0f* lod_focus
    };

    for (n = 0; n < dn; n++, coord.x = 0, coord.y += dcoord.y)
        for (k = 0; k < dk; k++, coord.x += dcoord.x)
//*
            if(visMap[k+n*dk])
/*/
	if (DrawNode->TestVisible(coord, coord+dcoord))
/**/
            {
                // process visible tile
                sTile &Tile = tilemap->GetTile(k, n);
                int &bumpTileID = Tile.bumpTileID;

                // calc LOD считается всегда по отгошению к прямой камере для 
                // избежания случая 2 разных LOD в одно время 
                float dist = pNormalCamera->GetPos().distance(coord + dcoord/2);
                int iLod;
                for (iLod = 0; iLod < TILEMAP_LOD; iLod++)
                    if (dist < DistLevelDetail[iLod]) break;
                if (iLod >= TILEMAP_LOD) iLod = TILEMAP_LOD-1;
                vis_lod[k+n*dk]=iLod;

                // create/update render tile
                if (render->bumpTileValid(bumpTileID)
                    && render->bumpTiles[bumpTileID]->LOD != iLod && !shadow)
                {
                    // LOD changed, free old tile and allocate new
                    bumpTileFree(bumpTileID);
                    bumpTileID = bumpTileAlloc(iLod,k,n);
                } else if (!bumpTileValid(bumpTileID))
                {
                    // no tile assigned, allocate one
                    bumpTileID = bumpTileAlloc(iLod,k,n);
                }

                sBumpTile *bumpTile = bumpTiles[bumpTileID];
                xassert(bumpTile);
                // set flags, add tile to list
                Tile.SetDraw();

                bool add=false;
                switch(tile_draw)
                {
                    case TILEMAP_ALL:
                        add=true;
                        break;
                    case TILEMAP_ZEROPLAST:
                        add=bumpTile->IsZeroplast();
                        break;
                    case TILEMAP_NOZEROPLAST:
                        add=!bumpTile->IsOnlyZeroplast();
                        break;
                }

                if (add) {
                    bumpTile->ToList();
                }
            }else
                vis_lod[k+n*dk]=-1;

//stop_timer(Calc_TileMap, 1);

    tilemap->GetTerra()->LockColumn();
    for (n = 0; n < dn; n++)
        for (k = 0; k < dk; k++)
        {
            sTile &Tile = tilemap->GetTile(k, n);
            int bumpTileID = Tile.bumpTileID;
            if(bumpTileID<0)continue;
            sBumpTile *bumpTile = bumpTiles[bumpTileID];
            int LOD=bumpTile->LOD;
            bool update_line=false;
            if(k>0)
            {
                char lod=vis_lod[k-1+n*dk];
                char& cur_lod=bumpTile->border_lod[sBumpTile::U_LEFT];
                if(lod>=LOD && cur_lod!=lod)
                {
                    cur_lod=lod;
                    update_line=true;
                }
            }

            if(k<dk-1)
            {
                char lod=vis_lod[k+1+n*dk];
                char& cur_lod=bumpTile->border_lod[sBumpTile::U_RIGHT];
                if(lod>=LOD && cur_lod!=lod)
                {
                    cur_lod=lod;
                    update_line=true;
                }
            }

            if(n>0)
            {
                char lod=vis_lod[k+(n-1)*dk];
                char& cur_lod=bumpTile->border_lod[sBumpTile::U_TOP];
                if(lod>=LOD && cur_lod!=lod)
                {
                    cur_lod=lod;
                    update_line=true;
                }
            }

            if(n<dn-1)
            {
                char lod=vis_lod[k+(n+1)*dk];
                char& cur_lod=bumpTile->border_lod[sBumpTile::U_BOTTOM];
                if(lod>=LOD && cur_lod!=lod)
                {
                    cur_lod=lod;
                    update_line=true;
                }
            }

            if((!bumpTile->init) || Tile.GetUpdate() || update_line)
            {
                bumpTile->Calc(!bumpTile->init || Tile.GetUpdate());
                Tile.ClearUpdate();
            }

        }
    tilemap->GetTerra()->UnlockColumn();

//	if(pNormalCamera==DrawNode)
//		gb_RenderDevice->SetRenderState(RS_WIREFRAME,1);
    int first_texture_number = 0;
#ifdef PERIMETER_D3D9
    uint32_t tss_colorarg2 = 0;
    uint32_t tss_alphaarg2 = 0;
    if (gb_RenderDevice3D) {
        tss_colorarg2 = gb_RenderDevice3D->GetTextureStageState(0, D3DTSS_COLORARG2);
        tss_alphaarg2 = gb_RenderDevice3D->GetTextureStageState(0, D3DTSS_ALPHAARG2);
        gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

        if (use_shadow_map) {
            first_texture_number = gb_RenderDevice3D->dtAdvance->GetOffsetTextureNumber(true);
        }
    }
#endif

#ifdef PERIMETER_DEBUG
    int st_VBSw = 0, st_Poly = 0;
    int nTiles = 0;
    VertexBuffer* lastVB = nullptr;
#endif
    VertexPoolManager* vtxPoolMan = render->GetVertexPool();
    IndexPoolManager* idxPoolMan = render->GetIndexPool();
    for (cTilemapTexturePool* curpool : bumpTexPools) {
        if (curpool->tileRenderList.empty()) {
            continue;
        }

        if (shadow) {
#ifdef PERIMETER_D3D9
            TextureImage teximg(gb_RenderDevice3D->dtAdvance->GetTilemapShadow0());
            gb_RenderDevice3D->SetTextureImage(0, &teximg);
#endif
        } else {
            gb_RenderDevice->SetTextureImage(first_texture_number, curpool->GetTexture()->GetFrameImage(0));
        }

        for (sBumpTile* bumpTile : curpool->tileRenderList) {
            int iLod = bumpTile->LOD;
            int nVertex = bumpNumVertices(iLod);
            VertexBuffer* vb = vtxPoolMan->GetBuffer(bumpTile->vtx);

            int nPolygon=0;
            for (int j=0;j<bumpTile->index.size();j++) {
                sPlayerIB& pib=bumpTile->index[j];
                if(pib.player==-1)
                {
                    if(tile_draw==TILEMAP_ZEROPLAST)
                        continue;
                }else
                {
                    if(tile_draw==TILEMAP_NOZEROPLAST)
                        continue;
                }

                sColor4f color=tilemap->GetZeroplastColor(pib.player);
                if (MatMode!=ALPHA_BLEND) {
                    color.a = 1;
                }

                gb_RenderDevice->SetTileColor(color);
                size_t ib_len = 0;
                int nIndex = 0;
                int nIndexOffset = 0;
                IndexBuffer* ib = nullptr;
                if (pib.nindex==-1) {
                    nIndex = bumpNumIndex(iLod);
                    nIndexOffset = bumpIndexOffset(iLod);
                    ib = &tilemapIB;
                    ib_len = ib->NumberIndices;
                } else {
                    ib_len = idxPoolMan->GetPoolSize(pib.index);
                    nIndex = pib.nindex;
                    nIndexOffset = idxPoolMan->GetBaseIndex(pib.index);
                    ib = idxPoolMan->GetBuffer(pib.index);
                }
                nPolygon=nIndex/3;
                DrawRange range;
                range.offset = nIndexOffset;
                range.len = nIndex;
                gb_RenderDevice->SubmitBuffers(PT_TRIANGLES, vb, nVertex, ib, ib_len, &range);
            }

#ifdef PERIMETER_DEBUG
            if (vb != lastVB) {
                lastVB = vb;
                st_VBSw++;
            }
            nTiles++;
            st_Poly += nPolygon;
#endif

            if(false)
                if(pNormalCamera==DrawNode)
                {
                    int x=bumpTile->tile_pos.x;
                    int y=bumpTile->tile_pos.y;
                    int xs = tilemap->GetTileSize().x;
                    int ys = tilemap->GetTileSize().y;

                    int z=tilemap->GetTile(x, y).zmax;

                    Vect3f v00(xs*x    ,ys*y    ,z),
                            v01(xs*(x+1),ys*y    ,z),
                            v10(xs*x    ,ys*(y+1),z),
                            v11(xs*(x+1),ys*(y+1),z);

                    sColor4c color(0,0,255,255);
                    gb_RenderDevice->DrawLine(v00,v01,color);
                    gb_RenderDevice->DrawLine(v01,v11,color);
                    gb_RenderDevice->DrawLine(v11,v10,color);
                    gb_RenderDevice->DrawLine(v10,v00,color);
                }
        }
    }

#ifdef PERIMETER_D3D9
#if 0
    if(!shadow)
    {
        gb_RenderDevice3D->SaveStates();
    }
#endif

    // restore state
    if (gb_RenderDevice3D) {
        gb_RenderDevice3D->SetTextureStageState(0,D3DTSS_COLORARG2,tss_colorarg2);
        gb_RenderDevice3D->SetTextureStageState(0,D3DTSS_ALPHAARG2,tss_alphaarg2);
    
        gb_RenderDevice3D->SetSamplerState( 0, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP );
        gb_RenderDevice3D->SetSamplerState( 0, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP );
        gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP );
        gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP );
        gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_MINFILTER, minfilter1);
        gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_MAGFILTER, magfilter1);
    
        gb_RenderDevice3D->SetVertexShader(NULL);
        gb_RenderDevice3D->SetPixelShader(NULL);
    
        gb_RenderDevice3D->SetTexture(NULL,0,3);
        gb_RenderDevice3D->SetTexture(NULL,0,4);
        gb_RenderDevice3D->SetTexture(NULL,0,5);
    }
#endif

    gb_RenderDevice->SetRenderState(RS_ZFUNC,zfunc);
    gb_RenderDevice->SetRenderState(RS_CULLMODE,cull);
    //gb_RenderDevice3D->SetRenderState(RS_WIREFRAME,0);
/*
char msg[256];
int st_mem = 0;
for (i = 0; i < bumpTexPools.size(); i++)
	st_mem += bumpTexPools[i]->tileWidth * BUMP_TEXPOOL_H * 4;
for (i = 0; i < bumpVtxPools.size(); i++)
	st_mem += bumpVtxPools[i]->totalPages * bumpVtxPools[i]->pageSize * bumpVtxPools[i]->vertexSize;
sprintf(msg, "tiles=%d vbsw=%d texsw=%d poly=%d texpools=%d mem=%d",
	nTiles, st_VBSw, st_TexSw, st_Poly, bumpTexPools.size(), st_mem);
OutText(0, 80, msg);
/**/
    if(false)//shadow)//pNormalCamera==DrawNode)
    {
        int dx=tilemap->GetTileNumber().x,dy=tilemap->GetTileNumber().y;
        int minx=9,miny=888 - 256;

        int r,g,b,a=80;
        if(reflection)
        {
            r=255;
            g=b=0;
        }else
        {
            r=0;
            g=b=255;
        }

        int mul=dx>32?2:4;

        for(int y=0;y<dy;y++)
            for(int x=0;x<dx;x++)
            {
                if(visMap[y*dx+x])
                {
                    int xx=x*mul+minx,yy=y*mul+miny;
                    gb_RenderDevice->DrawRectangle(xx,yy,mul,mul,sColor4c(r,g,b,a));
                }
            }
    }

    /*
	if(false)//shadow)
	{
		CMesh cmesh;
		calcCMesh(pNormalCamera, tilemap->GetTileNumber(), tilemap->GetTileSize(), cmesh);
		drawCMesh(cmesh);
		gb_RenderDevice->FlushPrimitive3D();
	}
    */
}
