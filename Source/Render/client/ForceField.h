#ifndef __FORCE_FIELD_H__
#define __FORCE_FIELD_H__

// Local Header Files STL
#include <vector> 
#include <list>
#include "..\..\Util\Map2D.h"

typedef vector<Vect2s> Vect2sVect;

struct FieldInterval
{
	short xl, xr, y;
	FieldInterval(){}
	FieldInterval(short xl_, short xr_, short y_) : xl(xl_), xr(xr_), y(y_) {}
};

class FieldCluster : public vector<FieldInterval>
{
public:
	static float ZeroGround;
	static bool enable_transparency_;

	enum ANIMATE_TYPE
	{
		FIELD_STOPPED,
		FIELD_STARTING,
		FIELD_STARTED,
		FIELD_STOPPING
	};
	
	enum TRANSPATENT_TYPE
	{
		TT_TRANSPARENT_ADD	= 1<<0,
		TT_OPAQUE			= 1<<1,
		TT_TRANSPARENT_MOD	= 1<<2,
	};

	FieldCluster();
	void clear();

	void switchON();
	void switchOFF();
	bool started() const { return animate_type != FIELD_STOPPED; }
	bool started_logic() const { return animate_type_logic != FIELD_STOPPED; }

	int inside(int x, int y);

	void set_attribute(int _player_id, int _cluster_id){ player_id = _player_id; cluster_id = _cluster_id; }
	int get_attribute(){ return (cluster_id & 0xff) | ((player_id & 0xff) << 8); };

	int get_cluster_id(){ return cluster_id; }
	int get_player_id(){ return player_id; }

	const Vect2sVect& border() const { return border_; }

	//SetTransparent,SetDiffuseColor - не правильно изменяет свойства при включенном куполе
	void SetColor(const sColor4c& c) { Diffuse = c; }
	int GetColor() const { return CurrentDiffuse.RGBA(); }

	void setTransparent(bool b) { is_transparent = b; }
	bool isTransparent() const { return is_transparent || enable_transparency_; }

	void Animate(float dt);
	TRANSPATENT_TYPE GetTT() const { return ttype; }

	static int get_cluster_id(int attribute){ return attribute & 0xff; }
	static int get_player_id(int attribute){ return (attribute >> 8) & 0xff; }
	void logicQuant();
private:
	Vect2sVect border_;

	int player_id;
	int cluster_id;
	
	sColor4c Diffuse, CurrentDiffuse;
	bool is_transparent;
	
	float fade_timer;
	float fade_timer_logic;
	ANIMATE_TYPE animate_type;
	ANIMATE_TYPE animate_type_logic;
	TRANSPATENT_TYPE ttype;

	friend class FieldDispatcher;
	friend void FieldDispatcherBorderCall(void* data,Vect2f& p);
};

///////////////////////////////////////////////////////////////////////////////////////
//  Верхняя полусфера нормалей с равномерным 
//  распределением
//  cell_size - расстояние, на котором определяют
//  dz_x, dz_y. Обычно удвоенный размер сетки
///////////////////////////////////////////////////////////////////////////////////////
template <int cell_size, int normals_per_Pi_shift = 8, int atan_max = 2048>
class NormalsBox
{
	enum {
		n_max = (1 << ((normals_per_Pi_shift - 1) - 1))
	};

	Vect3f* normals;
	char* atan_table;

public:
	NormalsBox()
	{
		atan_table = new char[atan_max*2 + 1];
		atan_table += atan_max;
		for(int i = -atan_max; i <= atan_max; i++)
			atan_table[i] = n_max + round(atan((float)i/cell_size)*n_max/(M_PI/2));

		normals = new Vect3f[(2*n_max + 1) << normals_per_Pi_shift];
		for(int y = 0; y < 2*n_max + 1; y++)
			for(int x = 0; x < 2*n_max + 1; x++){
				if(x || y){
					float nx = (float)(x - n_max)*(M_PI/2)/n_max;
					float ny = (float)(y - n_max)*(M_PI/2)/n_max;
					Mat3f M(Vect3f(ny, -nx, 0), sqrtf(sqr(nx) + sqr(ny)));
					normals[x + (y << normals_per_Pi_shift)] = M*Vect3f::K;
					}
				else
					normals[x + (y << normals_per_Pi_shift)] = Vect3f::K;
				}
	}
	~NormalsBox()
	{
		atan_table -= atan_max;
		delete atan_table;
		delete normals;
	}
	const Vect3f& operator()(int dz_x, int dz_y) const
	{
		if(dz_x < -atan_max)
			dz_x = -atan_max;
		if(dz_x > atan_max)
			dz_x = atan_max;
		dz_x = atan_table[dz_x];

		if(dz_y < -atan_max)
			dz_y = -atan_max;
		if(dz_y > atan_max)
			dz_y = atan_max;
		dz_y = atan_table[dz_y];

		return normals[dz_x + (dz_y << normals_per_Pi_shift)];
	}
};

class FieldDispatcher : public cIUnkObj
{
public:
	typedef list<FieldCluster> ClusterList;

	enum { 
		POTENTIAL = 1,

		scale = 4, // масштаб поля
		tile_scale = 6, // масштаб тайлов

		half = (scale ? 1 << (scale - 1) : 0)
		};

	struct Cell 
	{
		float field;
		float height_initial;
		float velocity;
		FieldCluster* cluster;

		Cell() { clear(); }
		Cell(float h) { clear(); height_initial = h; }
		void clear();
		void specify(const Vect2i& delta, const Vect2i& tangenta, const FieldCluster* cluster);
		bool specified() const { return specify_error != max_error; }
		void integrate(float& height);

		Vect2s specify_delta;
		int specify_error;
		Vect3f specify_normal;
		enum { max_error = 0x7fffffff };
	};
	
	ClusterList clusters;


	FieldDispatcher(int xmax, int ymax, int zeroLayerHeight);
	virtual ~FieldDispatcher();

	FieldCluster* createCluster(int player_id, int cluster_id) { clusters.push_back(FieldCluster()); clusters.back().set_attribute(player_id, cluster_id); return &clusters.back(); }
	void removeCluster(FieldCluster* cluster);
	void setBorder(FieldCluster* cluster, const class Region& region);
	void setBorder(FieldCluster* cluster, class CycledHermite& hermite);
	void setBorder(FieldCluster* cluster, const Vect2sVect& border);
	void switchON(FieldCluster* cluster);
	void switchOFF(FieldCluster* cluster);
	void switchDELETE(FieldCluster* cluster);
	void setCluster(FieldCluster* cluster, const class Column& column);

	void enableTransparency(bool enable_transparency) { FieldCluster::enable_transparency_ = enable_transparency; }

	int getIncludingCluster(const Vect3f& r);
	float getGraphZ(float x,float y);
	int calcPenalty(const Vect3f& center, float radius, float feedback_factor, Vect3f& r, Vect3f&f);
	
	bool checkPlace(const Vect2f& pos, const Vect2f& delta);
	Vect2f findPlace(const Vect2f& pos0, const Vect2f& delta);

	int castRay(const Vect3f& origin, const Vect3f& direction_or_point, Vect3f& intersection, Vect3f& normal); 

	//---------------------------------------

	int mapSizeX() const { return map_.sizeX(); }
	int mapSizeY() const { return map_.sizeY(); }
	float* height_array(){ return map_height.map();}

	Cell& map(int x, int y) { return map_(x, y); } 
	const Cell& map(int x, int y) const { return map_(x, y); } 

	const Vect3f& normal(int x, int y) const { return normals(height(x + 1, y) - height(x - 1, y), height(x, y + 1) - height(x, y - 1)); }
	
	float height(int x, int y) const { return map_height(x, y); } 
	float height_initial(int x, int y) const { return map(x, y).height_initial; } 

	FieldCluster* getCluster(int x, int y) const { return map(x, y).cluster; } 
	int attribute(int x,int y){ FieldCluster* p = getCluster(x,y); if(p) return p->get_attribute(); return 0; }
	
	int w2m(int x) const { return map_.w2m(x); } // World to map
	int m2w(int x) const  { return map_.m2w(x); }


	int tileMapSizeX() const { return tile_map_.sizeX(); }
	int tileMapSizeY() const { return tile_map_.sizeY(); }

	BYTE tileMap(int x, int y) const { return tile_map_(x, y); } 

	int w2t(int x) const { return tile_map_.w2m(x); } // World to tile
	int t2w(int x) const  { return tile_map_.m2w(x); }

	int m2t(int x) const { return w2t(m2w(x)); } // Map to tile
	int t2m(int x) const { return w2m(t2w(x)); } // Tile to map

	void showDebugInfo() const;

	//---------------------------------------
	// cUnkObj Interface
	virtual void PreDraw(cCamera *UCamera);
	virtual void Draw(cCamera *UCamera);
	virtual void Animate(float dt);
	void Update(){ UpdateTile(); }
	void SetTexture(int n, cTexture *pTexture);
	inline class cTexture* GetTexture(int n=0)				{ return Texture[n]; }
	inline class cFrame* GetFrame()							{ return Frame; }

	void logicQuant();
	float InterpolationFactor()const{return interpolation_factor;}
	void clearCluster(FieldCluster* cluster);
private:
	void evolveField();

	typedef Map2D<Cell, scale> Map;
	typedef Map2D<float, scale> HMap;
	Map map_;
	HMap map_height;

	float inv_scale_shl;
	
	NormalsBox <1 << (scale + 1)> normals; // Normals
	
	cTexture	*Texture[2];
	cFrame	*Frame;

	typedef Map2D<BYTE, tile_scale> TileMap;
	TileMap tile_map_;
	
	BYTE tile_global;

	class FFDData* pDrawData; //Используется Draw(class FieldDispatcher *rd);

	double prev_real_time;

	Vect2f w2m(const Vect2f& v) { return Vect2f(w2m(v.x), w2m(v.y)); }
	
	void calcHeight(FieldCluster* cluster);
	void UpdateTile();

	int ray_cell_intersection(int x, int y, float t_zero, const Vect3f& origin, const Vect3f& direction, Vect3f& intersection, Vect3f& normal);
	
	HMap *hmap[3];
	HMap *hmap_prev,*hmap_cur,*hmap_logic;
	int hmap_index;
	void hmapRotate();
	//Везде, где обращаются из графического потока к hmap_prev,hmap_cur,hmap_logic
	//хотя, если lock не сделать, ничего фатального не случится.
	MTDECLARE(hmap_lock)

	inline float interpolateHeight(int x,int y,float t,float t_)
	{
		float h0=(*hmap_prev)(x,y);
		float h1=(*hmap_cur)(x,y);
		return h0*t+h1*t_;
	}

	float interpolation_factor;

	friend class cD3DRender;
};

extern FieldDispatcher* field_dispatcher;

#endif // __FORCE_FIELD_H__
