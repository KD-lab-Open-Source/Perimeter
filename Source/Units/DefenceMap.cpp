#include "StdAfx.h"
#include "DefenceMap.h"
#include "AIMain.h"
#include "ClusterFind.h"
#include "ForceField.h"
#include "AIPrm.h"
#include "runtime.h"

DefenceMap::DefenceMap(int hsize,int vsize) 
: Map2D<BYTE, 4>(hsize, vsize, 0)
{ 
	path_finder = new ClusterFind(sizeX(), sizeY(), defenceMapPathFind.clusterSize);
	path_finder2 = new ClusterFind(sizeX(), sizeY(), defenceMapPathFind.clusterSize);

	rebuildWalkMap(path_finder->GetWalkMap());
	path_finder->Set(defenceMapPathFind.enableSmoothing);

	memcpy(path_finder2->GetWalkMap(),path_finder->GetWalkMap(),sizeX()*sizeY()*sizeof(path_finder2->GetWalkMap()[0]));
	path_finder2->SetLater(defenceMapPathFind.enableSmoothing, defenceMapPathFind.rebuildQuants);
}

DefenceMap::~DefenceMap()
{
	delete path_finder;
	delete path_finder2;
}

void DefenceMap::startRecalcMap()
{
	xassert(path_finder2->ready());
	swap(path_finder2,path_finder);

	rebuildWalkMap(path_finder2->GetWalkMap());
	path_finder2->SetLater(defenceMapPathFind.enableSmoothing,defenceMapPathFind.rebuildQuants);
}

bool DefenceMap::recalcMapQuant()
{
	return path_finder2->SetLaterQuant();
}

void DefenceMap::addGun(const Vect2f& positionWorld, float radiusWorld)
{
	Vect2i position = w2m(positionWorld);
	int radius = w2m(round(radiusWorld) + defenceMapPathFind.gunExtraRadius);
	for(int y = -radius; y <= radius; y++){
		int yc = position.y + y;
		if(yc < 0 || yc >= sizeY())
			continue;
		int delta = round(sqrtf(sqr(radius) - sqr(y)));
		int xl = clamp(position.x - delta, 0, sizeX() - 1);
		int xr = clamp(position.x + delta, 0, sizeX() - 1);
		for(int x = xl; x <= xr; x++){
			BYTE& byte = (*this)(x, yc);
			byte |= (byte + 1) & ClusterFind::DOWN_MASK;
		}
	}
}

//////////////////////////////////////////////////////////////
//		PathFind
//////////////////////////////////////////////////////////////
static const float gmul=1.0f;

class ClusterHeuristicMulti
{
public:
	typedef ClusterFind::Cluster Node;

	ClusterHeuristicMulti() : center_(Vect2i::ZERO) {}

	void addEnd(const Vect2i& position, Node* node)
	{
		center_ *= ends.size();
		ends.push_back(End(position, node));
		center_ += position;
		center_ /= ends.size();
	}

	//Предполагаемые затраты на продвижение из pos1 к окончанию
	float GetH(Node* pos)
	{
		return sqrtf(sqr(pos->xcenter - center_.xi())+
					sqr(pos->ycenter - center_.yi()))*gmul;
	}

	float operator()(BYTE walk_from,BYTE walk_to)
	{
		if((walk_from^walk_to)&ClusterFind::UP_MASK)
			return 1000.0f;

		return (walk_to&ClusterFind::DOWN_MASK)+gmul;
	}

	//Затраты на продвижение из pos1 в pos2
	float GetG(Node* pos1,Node* pos2)
	{
		float mul=(pos2->walk&ClusterFind::DOWN_MASK)+gmul;

		float f=sqrtf(sqr(pos1->xcenter-pos2->xcenter)+
					sqr(pos1->ycenter-pos2->ycenter))*mul;

		if((pos1->walk^pos2->walk)&ClusterFind::UP_MASK)
			f+=1000.0f;

		return f;
	}

	bool IsEndPoint(Node* node)
	{
		EndList::const_iterator ni;
		FOR_EACH(ends, ni)
			if(ni->node == node)
				return true;
		return false;
	}

	const Vect2i& to(Node* node)
	{
		EndList::const_iterator ni;
		FOR_EACH(ends, ni)
			if(ni->node == node)
				return ni->position;
		xassert(0);
		return Vect2i::ZERO;
	}

private:
	struct End {
		Vect2i position;
		Node* node;
		End(const Vect2i& p, Node* n) : position(p), node(n) {}
	};
	typedef vector<End> EndList;
	EndList ends;
	Vect2f center_;
};

bool DefenceMap::findPathToPoint(const Vect2i& from_w, const Vect2i& to_w, vector<Vect2i>& out_path)
{
	Vect2i from = w2m(from_w);
	if(!inside(from))
		return false;

	Vect2iVect tos;
	Vect2i to = w2m(to_w);
	if(inside(to))
		tos.push_back(to);

	if(tos.empty())
		return false;

	if(!path_finder->FindPathMulti(from, tos, out_path, ClusterHeuristicMulti()))
		return false;

	vector<Vect2i>::iterator it;
	FOR_EACH(out_path,it)
		*it = m2w(*it);

	if(!out_path.empty()){
		out_path.back() = to_w;
		return true;
	}
	else
		return false;
}

terUnitBase* DefenceMap::findPathToTarget(const Vect2i& from_w, const UnitList& targets, vector<Vect2i>& out_path)
{
	Vect2i from = w2m(from_w);
	if(!inside(from))
		return 0;

	int dist, distMin = INT_INF;
	terUnitBase* unitMin = 0;
	Vect2iVect tos;
	UnitList::const_iterator ui;
	FOR_EACH(targets, ui){
		Vect2i to_w = (*ui)->position2D();
		if(distMin > (dist = (from_w - to_w).norm2())){
			distMin = dist;
			unitMin = *ui;
		}
		Vect2i to = w2m(to_w);
		if(inside(to))
			tos.push_back(to);
	}

	if(distMin < defenceMapPathFind.ignorePathFindMinDistance2)
		return unitMin;

	if(tos.empty())
		return 0;

	if(!path_finder->FindPathMulti(from, tos, out_path, ClusterHeuristicMulti()))
		return 0;

	vector<Vect2i>::iterator it;
	FOR_EACH(out_path,it)
		*it = m2w(*it);

	if(!out_path.empty()){
		float dist, distMin = FLT_INF;
		Vect2i pos = out_path.back();
		terUnitBase* target = 0;
		UnitList::const_iterator ui;
		FOR_EACH(targets, ui){
			if(distMin > (dist = (*ui)->position2D().distance2(pos))){
				distMin = dist;
				target = *ui;
			}
		}
		out_path.back() = target->position2D();
		return target;
	}
	else
		return targets.front();
}

void DefenceMap::rebuildWalkMap(BYTE* walk_map)
{
	memcpy(walk_map, map(), sizeY()*sizeX());
}

void DefenceMap::analizeChaos()
{
	const BYTE value = 64;

	for(int x = 0; x < sizeX(); x++)
		(*this)(x, 0) = (*this)(x, sizeY() - 1) = value;
	for(int y = 0; y < sizeY(); y++)
		(*this)(0, y) = (*this)(sizeX() - 1, y) = value;

	xassert(tileSize == AITileMap::tileSize);
	for(int y = 1; y < sizeY() - 1; y++)
		for(int x = 1; x < sizeX() - 1; x++)
			if(!(*ai_tile_map)(x, y).height_min)
				(*this)(x, y) = value;

	for(int step = value - 1; step > value - defenceMapPathFind.chaosSmoothSteps; step--)
		for(int y = 1; y < sizeY() - 1; y++)
			for(int x = 1; x < sizeX() - 1; x++)
				if(!(*this)(x, y) && ((*this)(x - 1, y) > step || (*this)(x + 1, y) > step || (*this)(x, y - 1) > step || (*this)(x, y + 1) > step))
					(*this)(x, y) = step;
}

void DefenceMap::analizeField(int playerID)
{
	const BYTE value = 64;

	if(field_dispatcher){
		xassert(FieldDispatcher::scale == tileSizeShl);

		int delta = w2m(defenceMapPathFind.fieldExtraRadius);

		FieldDispatcher::ClusterList::iterator ci;
		FOR_EACH(field_dispatcher->clusters, ci){
			FieldCluster& cluster = *ci;
			if(cluster.get_player_id() != playerID || cluster.empty())
				continue;

			int yc = cluster.front().y - 1;
			int xl, xr;
			bool beginUp = true;
			vector<FieldInterval>::iterator ii;
			FOR_EACH(cluster, ii){
				FieldInterval& in = *ii;
				
				if(yc != in.y - 1){
					int y1 = clamp(yc + 1, 0, sizeY() - 1);
					int y2 = clamp(yc + delta, 0, sizeY() - 1);
					for(int y = y1; y <= y2; y++)
						memset(&(*this)(xl, y), value, xr - xl + 1);
					beginUp = true;
				}
				
				xl = clamp(in.xl - delta, 0, sizeX() - 1);
				xr = clamp(in.xr + delta, 0, sizeX() - 1);
				yc = clamp(in.y, 0, sizeY() - 1);
				memset(&(*this)(xl, yc), value, xr - xl + 1);
	
				if(beginUp){
					int y1 = clamp(yc - delta, 0, sizeY() - 1);
					int y2 = clamp(yc - 1, 0, sizeY() - 1);
					for(int y = y1; y <= y2; y++)
						memset(&(*this)(xl, y), value, xr - xl + 1);
					beginUp = false;
				}
			}
		}
	}
}

void DefenceMap::showDebugInfo()
{
	for(int y = 0; y < sizeY(); y++)
		for(int x = 0; x < sizeX(); x++){
			int byte = path_finder->GetWalkMap()[x + y*sizeX()];
			if(byte)
				show_vector(Vect3f(m2w(x), m2w(y), vMap.hZeroPlast), XCOL(CYAN, clamp(byte*30, 0, 255)));
		}
}

