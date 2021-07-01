#pragma once
#include "GenericControls.h"
#include "IronClusterUnit.h"

typedef Grid2D<terUnitMonk, 5, GridVector<terUnitMonk, 8> > terMonkGridType;
typedef std::list<terUnitMonk*> terMonkList;

class MonkManager
{
	terMonkList monks;
	terMonkList kill_list;
public:
	terMonkGridType grid;

	MonkManager();
	~MonkManager();

	void Init();
	void Done();

	terUnitMonk* create(const UnitTemplate& data);
	void kill(terUnitMonk* m);

	void avatarInterpolation();
	void quant();
	void deleteQuant();
protected:
	void collision();
};
