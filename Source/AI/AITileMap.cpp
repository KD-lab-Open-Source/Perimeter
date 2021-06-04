#include "StdAfx.h"
#include "AITileMap.h"
#include "AIMain.h"
#include "ClusterFind.h"
#include "ForceField.h"
#include "AIPrm.h"
#include "runtime.h"

bool AITile::update(int x,int y)
{
	bool prev_log = completed();
	
	height_min = 255;
	int height_max = 0;
	dig_work = 0;
	dig_less = false;

	int height_avr = 0;
	int hZeroPlast = vMap.hZeroPlast;
	for(int yy = 0;yy < tile_size;yy++)
	{
		int offset = vMap.offsetGBuf(x*tile_size, y*tile_size + yy);
		unsigned char* h_buffer = vMap.GVBuf + offset;
		unsigned short* attr_buffer = vMap.GABuf + offset;

		for(int xx = 0;xx < tile_size;xx++)
		{
			unsigned char h = h_buffer[xx];
			height_avr += h;
			unsigned short attr = attr_buffer[xx];
			
			if(!GRIDTST_TALLER_HZEROPLAST(attr) || (attr & GRIDAT_MASK_HARDNESS) == GRIDAT_MASK_HARDNESS)
				dig_work += abs(h_buffer[xx] - hZeroPlast);
			else
				dig_less = true;

			if(height_min > h)
				height_min = h;
			if(height_max < h)
				height_max = h;
		}
	}

	height = height_avr/tile_area;
	delta_height = height_max - height_min;

	return completed() && prev_log != completed();
}

////////////////////////////////////////////////////////
//			AITileMap
////////////////////////////////////////////////////////
AITileMap::AITileMap(int hsize,int vsize) 
: Map2D<AITile, AITile::tile_size_world_shl>(hsize,vsize)
{ 
	pWalkMap=NULL;

	path_finder = new ClusterFind(sizeX(), sizeY(), terrainPathFind.clusterSize);
	path_finder2 = new ClusterFind(sizeX(), sizeY(), terrainPathFind.clusterSize);
	path_hard_map = new ClusterFind(sizeX(), sizeY(), terrainPathFind.clusterSize);

	InitialUpdate(); 
}
AITileMap::~AITileMap()
{
	RELEASE(pWalkMap);
	delete path_finder;
	delete path_finder2;
	delete path_hard_map;
}

void AITileMap::InitialUpdate()
{
	for(int y=0;y < sizeY();y++)
		for(int x=0;x < sizeX();x++)
			(*this)(x,y).update(x,y);

	rebuildWalkMap(path_finder->GetWalkMap());
	path_finder->Set(terrainPathFind.enableSmoothing);

	memcpy(path_finder2->GetWalkMap(),path_finder->GetWalkMap(),sizeX()*sizeY()*sizeof(path_finder2->GetWalkMap()[0]));
	path_finder2->SetLater(terrainPathFind.enableSmoothing, terrainPathFind.rebuildQuants);

	updateHardMap();
}

void AITileMap::UpdateRect(int x1,int y1,int dx,int dy)
{
	int x2 = w2mFloor(x1 + dx);
	int y2 = w2mFloor(y1 + dy);

	x1 = w2mFloor(x1);
	y1 = w2mFloor(y1);

	for(int y = y1;y <= y2; y++)
	for(int x = x1; x <= x2; x++)
		if((*this)(x,y).update(x,y))
		{
			list<AIPlayer*>::iterator it;
			FOR_EACH(call_back,it)
				(*it)->changeTileState(x,y);
		}
}

void AITileMap::placeBuilding(const Vect2i& v1, const Vect2i& size, bool place)
{
	Vect2i v2 = v1 + size;

	for(int y = v1.y; y < v2.y; y++)
		for(int x = v1.x; x < v2.x; x++)
			(*this)(x,y).building = place;
}

bool AITileMap::readyForBuilding(const Vect2i& v1, const Vect2i& size)
{
	Vect2i v2 = v1 + size;

	for(int y = v1.y; y < v2.y; y++)
		for(int x = v1.x; x < v2.x; x++)
		{
			AITile& tile = (*this)(x,y);
			if(!tile.completed() || tile.building)
				return false;
		}
	return true;
}

//////////////////////////////////////////////////////////////
//		PathFind
//////////////////////////////////////////////////////////////
static const float gmul=1.0f;
static const float gfield=gmul*1000;

class ClusterHeuristic
{
public:
	typedef ClusterFind::Cluster Node;
	Node* end;

	//Предполагаемые затраты на продвижение из pos1 к окончанию
	inline float GetH(Node* pos)
	{
		return sqrtf(sqr(pos->xcenter-end->xcenter)+
					sqr(pos->ycenter-end->ycenter))*gmul;
	}

	inline float operator()(BYTE walk_from,BYTE walk_to)
	{
		if((walk_from^walk_to)&ClusterFind::UP_MASK)
			return gfield;

		return (walk_to&ClusterFind::DOWN_MASK)+gmul;
	}

	//Затраты на продвижение из pos1 в pos2
	inline float GetG(Node* pos1,Node* pos2)
	{
		float mul=(pos2->walk&ClusterFind::DOWN_MASK)+gmul;

		float f=sqrtf(sqr(pos1->xcenter-pos2->xcenter)+
					sqr(pos1->ycenter-pos2->ycenter))*mul;

		if((pos1->walk^pos2->walk)&ClusterFind::UP_MASK)
			f+=gfield;

		return f;
	}

	inline bool IsEndPoint(Node* pos){return pos==end;}
};

class ClusterHeuristicDitch : public ClusterHeuristic
{//Рвы считаются непреодолимыми препятствиями (высота==0)
public:
	enum {
		heuristic_ditch = 127 //Ров на нулевой высоте
	};

	inline float GetG(Node* pos1,Node* pos2)
	{
		if((pos2->walk & ClusterFind::DOWN_MASK) == heuristic_ditch)
			return 1000.0f;
		return ClusterHeuristic::GetG(pos1,pos2);
	}
	inline float operator()(BYTE walk_from,BYTE walk_to)
	{
		if((walk_to & ClusterFind::DOWN_MASK) == heuristic_ditch)
			return 1000.0f;

		return (*(ClusterHeuristic*)this)(walk_from,walk_to);
	}
};

class ClusterHeuristicHard
{
public:
	typedef ClusterFind::Cluster Node;
	Node* end;

	//Предполагаемые затраты на продвижение из pos1 к окончанию
	inline float GetH(Node* pos)
	{
		return sqrtf(sqr(pos->xcenter-end->xcenter)+
					sqr(pos->ycenter-end->ycenter));
	}

	inline float operator()(BYTE walk_from,BYTE walk_to)
	{
		return (walk_to&ClusterFind::DOWN_MASK)*10000.0f+1;
	}

	//Затраты на продвижение из pos1 в pos2
	inline float GetG(Node* pos1,Node* pos2)
	{
		float mul=(pos2->walk&ClusterFind::DOWN_MASK)*10000.0f+1;

		float f=sqrtf(sqr(pos1->xcenter-pos2->xcenter)+
					sqr(pos1->ycenter-pos2->ycenter))*mul;
		return f;
	}

	inline bool IsEndPoint(Node* pos){return pos==end;}
};

bool AITileMap::findPath(const Vect2i& from_w, const Vect2i& to_w, vector<Vect2i>& out_path,PathType type)
{
	Vect2i from = w2m(from_w);
	Vect2i to = w2m(to_w);

	if(!inside(from) || !inside(to))
		return false;

	bool b;
	switch(type)
	{
	case PATH_NORMAL:
		b=path_finder->FindPath(from, to, out_path, ClusterHeuristicDitch());
		break;
	case PATH_HARD:
		b=path_hard_map->FindPath(from, to, out_path, ClusterHeuristicHard());
		break;
	}

	vector<Vect2i>::iterator it;
	FOR_EACH(out_path,it)
		*it = m2w(*it);

	if(!out_path.empty())
		out_path.back() = to_w;

	return b;
}

void AITileMap::rebuildWalkMap(BYTE* walk_map)
{
	int size = sizeY()*sizeX();
	memset(walk_map,0,size);
	for(int i = 0;i < size;i++) {
		if(map()[i].height_min) {
/*
			int dh = map()[i].delta_height;
			if(dh >= terrainPathFind.maxColor)
				dh = terrainPathFind.levelOfDetail;
			else {
				dh = dh*terrainPathFind.levelOfDetail/terrainPathFind.maxColor;
				if(dh >= terrainPathFind.levelOfDetail-2)
					dh = terrainPathFind.levelOfDetail;
			}

			xassert(dh <= terrainPathFind.maxColor);

			if(dh < terrainPathFind.minimizeMinLevel)
				dh = 0;
/*/
			int dh=0;//Не учитывать гористость поверхности 
/**/
			if(!walk_map[i])
				walk_map[i] = dh;
			if(dh==terrainPathFind.levelOfDetail)
			{
				if(i-1>=0)
					walk_map[i-1]=dh;
				if(i+1<size)
					walk_map[i+1]=dh;
				if(i-sizeX()>=0)
					walk_map[i-sizeX()]=dh;
				if(i+sizeX()<size)
					walk_map[i+sizeX()]=dh;
			}
		}
		else 
			walk_map[i] = ClusterHeuristicDitch::heuristic_ditch;
	}


	//Добавить кластер
	if(0 && field_dispatcher)
	{
		xassert(FieldDispatcher::scale == tileSizeShl);

		int map_dx = sizeX(),
			map_dy = sizeY();

		FieldDispatcher::ClusterList::iterator it_cluster;
		FOR_EACH(field_dispatcher->clusters,it_cluster)
		{
			FieldCluster& c=*it_cluster;
			vector<FieldInterval>::iterator it_field;

			FOR_EACH(c,it_field)
			{
				FieldInterval& f=*it_field;
				xassert(f.xl<=f.xr);
				xassert(f.xl>=0 && f.xl<=map_dx);
				xassert(f.xr>=0 && f.xr<=map_dx);

				xassert(f.y>=0 && f.y<map_dy);
				if(f.xr==map_dx)
					f.xr=map_dx-1;

				if( f.xl>=0 && f.xl<map_dx &&
					f.xr>=0 && f.xr<map_dx &&
					f.y>=0 && f.y<map_dy)
				{
					memset(walk_map+f.y*map_dx+f.xl,
						ClusterFind::UP_MASK,f.xr-f.xl+1);
				}
			}
		}
/*
		static bool one=true;
		if(one )//&& !FieldDispatcher->clusters.empty())
		{
			one=false;
			TGAHEAD h;
			h.save3layers("cluster.tga",map_dx,map_dy,
				walk_map,walk_map,walk_map);
			int size=pSquadMap->GetWidth()*pSquadMap->GetHeight();
			BYTE* b=new BYTE[size];
			for(int i=0;i<size;i++)
			{
				int c=walk_map[i];
				c*=64;
				if(c>255)c=255;
				b[i]=c;
			}

			h.save3layers("cluster_mul.tga",map_dx,map_dy,
				b,b,b);
			delete b;
		}
/**/
	}

	if(terrainPathFind.showMap==1)
		updateWalkMap(walk_map);
}

void AITileMap::recalcPathFind()
{
	start_timer_auto(calcPathMap,STATISTICS_GROUP_TOTAL);

	if(path_finder2->SetLaterQuant()){
		swap(path_finder2,path_finder);

		rebuildWalkMap(path_finder2->GetWalkMap());
		path_finder2->SetLater(terrainPathFind.enableSmoothing,terrainPathFind.rebuildQuants);
	}
}

void AITileMap::updateWalkMap(BYTE* walk_map)
{
	if(!pWalkMap)
		pWalkMap=GetTexLibrary()->CreateTexture(sizeX(),sizeY(),false);
	int Pitch;
	BYTE* pBits=pWalkMap->LockTexture(Pitch);
	BYTE mul=255/terrainPathFind.levelOfDetail;

	for(int y=0;y<sizeY();y++)
	{
		sColor4c* p=(sColor4c*)pBits;
		BYTE* pwalk=walk_map+y*sizeX();
		for(int x=0;x<sizeX();x++,p++,pwalk++)
		{
			BYTE c=mul* *pwalk;
			BYTE up=ClusterFind::UP_MASK & *pwalk;
			if(up)
				p->set(0,255,0);
			else
				p->set(c,c,c);
		}

		pBits+=Pitch;
	}
	pWalkMap->UnlockTexture();
}

void AITileMap::drawWalkMap()
{
	if(!( pWalkMap&& terrainPathFind.showMap))
		return;

	terRenderDevice->DrawSprite(0,32,256/*sizeX()*2*/,256/*sizeY()*2*/,0,0,1,1,pWalkMap);

	char str[256];
	cFont* pFont=gb_VisGeneric->CreateDebugFont();
	terRenderDevice->SetFont(pFont);
	sprintf(str,"cluster=%i",path_finder->GetNumCluster());
	terRenderDevice->OutText(0,288,str,sColor4f(1,1,1,1));

	terRenderDevice->SetFont(NULL);
	pFont->Release();
}

void AITileMap::updateHardMap()
{
	BYTE* walk_map=path_hard_map->GetWalkMap();
	int size = sizeY()*sizeX();
	for(int i = 0;i < size;i++)
	{
		walk_map[i]=map()[i].dig_less?1:0;
	}

	path_hard_map->Set(terrainPathFind.enableSmoothing);
	if(terrainPathFind.showMap==2)
		updateWalkMap(walk_map);
}
