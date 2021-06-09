#ifndef __DEFENCE_MAP_H__
#define __DEFENCE_MAP_H__

#include "Map2D.h"
#include "GenericControls.h"

class ClusterFind;

class DefenceMap : public Map2D<BYTE, 4>
{
public:
	DefenceMap(int hsize, int vsize);
	~DefenceMap();

	void analizeChaos();
	void analizeField(int playerID);
	void addGun(const Vect2f& position, float radius); 

	int calcDefenceFactor(const Vect2f& position, float radius); 

	bool findPathToPoint(const Vect2i& from_w, const Vect2i& to_w, vector<Vect2i>& out_path);
	terUnitBase* findPathToTarget(const Vect2i& from_w, const UnitList& targets, vector<Vect2i>& out_path);

	void startRecalcMap();
	bool recalcMapQuant(); // return recalcStarted_

	void showDebugInfo();

protected:
	ClusterFind* path_finder;
	ClusterFind* path_finder2;

	void rebuildWalkMap(BYTE* walk_map);
};


#endif //__DEFENCE_MAP_H__