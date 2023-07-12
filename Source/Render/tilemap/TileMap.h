#ifndef PERIMETER_TILEMAP_H
#define PERIMETER_TILEMAP_H

class cScene;

typedef std::vector<Vect2s> Vect2sVect;

const int TILEMAP_SHL  = 6;
const int TILEMAP_SIZE = 1<<TILEMAP_SHL;
const int TILEMAP_LOD=5;

//TILEMAP_LOD в соответствии с ним нужно исправить ATTRTILE_DRAWLODxxx
enum eAttributeTile
{
	ATTRTILE_DRAWLOD		=   1<<0,
	ATTRTILE_UPDATELOD		=	1<<1,	
	ATTRTILE_UPDATE_POINT	=	1<<2,
};

struct sTile : public sAttribute
{
	int bumpTileID;
	uint8_t zmin,zmax;

	std::vector<std::vector<Vect2s> > region_point;//region_point[player][point]

	sTile()								
	{
		bumpTileID = -1;
		zmin=255;zmax=0;
	}

	inline int GetDraw()			{ return GetAttribute(ATTRTILE_DRAWLOD); }
	inline int GetUpdate()			{ return GetAttribute(ATTRTILE_UPDATELOD); }
	inline void SetDraw()			{ SetAttribute(ATTRTILE_DRAWLOD); }

	inline void ClearDraw()			{ ClearAttribute(ATTRTILE_DRAWLOD); }
	inline void ClearUpdate()		{ ClearAttribute(ATTRTILE_UPDATELOD); }
};

typedef std::vector<std::vector<Vect2s>* > CurrentRegion;
typedef void (*UpdateMapFunction)(const Vect2i& pos1, const Vect2i& pos2,void* data);

class cTileMapRender;
class Column;
class cTileMap : public cUnkObj
{
	friend class cScene;

	sTile*			Tile;
	Vect2i			TileSize;		// размер одного тайла
	Vect2i			TileNumber;		// число тайлов по осям
    size_t TexturePoolSize;

	Vect3d			tilesize;

	cTileMapRender* pTileMapRender = nullptr;
	
	cCamera*		ShadowDrawNode;
	cCamera*		LightDrawNode;
	int				zeroplastnumber;
	std::vector<sColor4f> zeroplast_color;

	std::vector<Column*> columns;
	class TerraInterface* terra;

	struct UpdateRect
	{
		Vect2i p1,p2;
	};
	std::vector<UpdateRect> update_rect;
	MTDECLARE(lock_update_rect);

	struct DebugRect
	{
		Vect2i p1,p2;
		float time;
	};
	bool enable_debug_rect;
	float debug_fade_interval;
	std::list<DebugRect> debug_rect;
public:
	Mat4f			matLightMap;

	cTileMap(cScene* pScene,TerraInterface* terra);
	virtual ~cTileMap();
	// общие интерфейсные функции унаследованы от cUnkObj
	virtual void PreDraw(cCamera *UCamera);
	virtual void Draw(cCamera *UCamera);
	virtual void UpdateMap(const Vect2i& pos1, const Vect2i& pos2);
	void UpdateMap(const Vect2i& pos, float radius) { UpdateMap(pos - Vect2i(radius, radius), pos + Vect2i(radius, radius)); }
	void Animate(float dt);
	// общие интерфейсные функции cTileMap
	const Vect2i& GetTileSize()const					{ return TileSize; }
    const Vect2i& GetTileNumber()const					{ return TileNumber; }
    size_t GetTexturePoolSize() const { return TexturePoolSize; }
	sTile& GetTile(int i,int j)							{ return Tile[i+j*GetTileNumber().x]; }
	int GetZeroplastNumber()	const					{ return zeroplastnumber; }

	Column** GetColumn() { VISASSERT(columns.size()==zeroplastnumber); return zeroplastnumber?&columns[0]:NULL; }
	std::vector<Vect2s>* GetCurRegion(Vect2i tile_pos,int player)
	{
		VISASSERT(tile_pos.x>=0 && tile_pos.x<TileNumber.x);
		VISASSERT(tile_pos.y>=0 && tile_pos.y<TileNumber.y);
		VISASSERT(player>=0 && player<zeroplastnumber);
		return &GetTile(tile_pos.x,tile_pos.y).region_point[player];
	}

	sColor4f GetZeroplastColor(int player)
	{
		if(player==-1)//player=-1 - world
			return sColor4f(1,1,1,1);
		return zeroplast_color[player];
	}

	void SetZeroplastColor(int player,const sColor4f& color)
	{
		VISASSERT(player>=0 && player<zeroplastnumber);
		zeroplast_color[player]=color;
	}

	void SetTilemapRender(cTileMapRender* p) {
        VISASSERT(p == nullptr || pTileMapRender == nullptr);
        pTileMapRender=p;
    };
	cTileMapRender* GetTilemapRender(){return pTileMapRender;}

	TerraInterface* GetTerra(){return terra;}

	Vect2f CalcZ(cCamera *DrawNode);
	void DrawLightmapShadow(cCamera *DrawNode);
	cTexture* GetShadowMap();
	cTexture* GetLightMap();

#ifdef PERIMETER_D3D9
    IDirect3DSurface9* GetZBuffer();
#endif
	void FixShadowMapCamera(cCamera *DrawNode);

    //Величина visMap должна быть TileMap->GetTileNumber().x*visMapDy=TileMap->GetTileNumber().y
    static void calcVisMap(cCamera *DrawNode, Vect2i TileNumber, Vect2i TileSize, uint8_t* visMap, bool clear);
    static void calcVisMap(cCamera *DrawNode, Vect2i TileNumber,Vect2i TileSize, Mat3f& direction,sBox6f& box);

    void RegisterUpdateMap(UpdateMapFunction f,void* data);
	void UnRegisterUpdateMap(UpdateMapFunction f,void* data);
protected:
	struct FixShadowDrawNodeParam
	{
		sBox6f box;
		MatXf LightMatrix;
	} fix_shadow;

	/////////////to initialize
	void SetBuffer(const Vect2i &size,int zeroplastnumber);

	////////////////////
	void CreateLightmap();

	void AddLightCamera(cCamera *DrawNode);
	void AddPlanarCamera(cCamera *DrawNode,bool light);
	void AddFixedLightCamera(cCamera *DrawNode);
	void CalcZMinMax(int x,int y);

	void CalcShadowMapCamera(cCamera *DrawNode);
	void CalcShadowMapCameraProective(cCamera *DrawNode);

	int LightMapType;
	int CheckLightMapType();

	void BuildRegionPoint();

	Vect3f To3D(const Vect2f& pos);
	void DrawLines();

	struct UpdateMapData
	{
		UpdateMapFunction f;
		void* data;
		bool operator==(const UpdateMapData& d)
		{
			return f==d.f && data==d.data;
		}
	};
	std::vector<UpdateMapData> func_update_map;
};

class cCamera;

#endif //PERIMETER_TILEMAP_H
