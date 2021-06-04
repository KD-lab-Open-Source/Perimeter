#ifndef _TRUST_MAP_
#define _TRUST_MAP_

#include "Handle.h"
#include "Grid2d.h"
#include "Region.h"

class terPlayer;
struct terTerraformGeneral;
typedef list<ShareHandle<terTerraformGeneral> > TerraformList;

class SharedObject: public ShareHandleBase
{
public:
	SharedObject() { alive = true; }
	~SharedObject(){}

	bool Alive(){ return alive; }

	virtual void Kill();  // Вызывается главным владельцем объекта, после ее вызова гарантированно
	// работает только Alive(). В производных функциях (которые обязательно должны вызывать базовую) должны очищаться
	// собственные Shared Links объекта, чтобы исключить образование циклических остатков.

private:
	bool alive;
};


const int TERRAFORM_ELEMENT_SIZE_SHIFT = 4;
const int TERRAFORM_ELEMENT_SIZE = 1 << TERRAFORM_ELEMENT_SIZE_SHIFT;
const int TERRAFORM_ELEMENT_SIDE = TERRAFORM_ELEMENT_SIZE/2;


enum terTerraformStatus
{
	TERRAFORM_STATUS_NONE = 0,
	TERRAFORM_STATUS_DIG = 1,
	TERRAFORM_STATUS_FILL = 2,
	TERRAFORM_STATUS_PREPARED = 4,
	TERRAFORM_STATUS_RESOURCE = 8,
	TERRAFORM_STATUS_CLEAN = 32
};

enum terTerraformType
{
	TERRAFORM_TYPE_FULL=1,
	TERRAFORM_TYPE_BORDER=2,

	TERRAFORM_TYPE_ABYSS_FULL=4,
	TERRAFORM_TYPE_ABYSS_BORDER=8,

	TERRAFORM_TYPE_OTHER=16,

	TERRAFORM_TYPE_GARBAGE=32,
//Битовая маска - для быстрой проверки в FindNear
};

struct terTerraformGeneral : GridElementType, SharedObject
{
	terTerraformType Type;
	TerraformList::iterator it_self;//Если Type==TERRAFORM_STATUS_NONE то неиницализирован.

	int PositionX,PositionY;

	int CollisionCount;
	int ID;
	int Status;
	terUnitBase* UnitPoint;

	terPlayer* Player;

	terTerraformGeneral(int id,int x,int y,terPlayer* player);
	virtual ~terTerraformGeneral();

	virtual void Show(){}

	void ShowRect(sColor4c color,bool line1,bool line2);

	//Осторожно, модифицирует внутри TerraformsDigFill,TerraformsOther
	void Quant(class terTerraformDispatcher* dispatcher,bool first);

	virtual int GetDigPosition(int& cx,int& cy){ return NULL; };
	virtual int GetFillPosition(int& cx,int& cy){ return NULL; };
	virtual int GetZeroPosition(int& cx,int& cy){ return NULL; };

	virtual int CheckDigPosition(int cx,int cy){ return NULL; };
	virtual int CheckFillPosition(int cx,int cy){ return NULL; };
	virtual int CheckZeroPosition(int cx,int cy){ return NULL; };

	virtual int GetClusterAttribute(int cx,int cy,int sx,int sy,int& prepared_zero,int& resource_zero,int& dig_layer,int& fill_layer,int& ground_zero,int& cell,int& zero_present){ return NULL; };

	virtual int GetHeight(){ return NULL; };

	bool IsDigFill(){return (Status & (TERRAFORM_STATUS_DIG|TERRAFORM_STATUS_FILL))?true:false;}
	void ChangeStatus(terTerraformDispatcher* dispatcher,int begin_status,int end_status);
};

typedef Grid2D<terTerraformGeneral, 5, GridSingleList<terTerraformGeneral> > terTrustGrid;


//--------------------------------------------------

struct terTerraformGarbage : terTerraformGeneral
{
	terTerraformGarbage(int id,int x,int y, terPlayer* player);
	void Show();
};

//--------------------------------------------------

struct terTerraformFull : terTerraformGeneral
{
	int Height;

	terTerraformFull(int id,int x,int y, terPlayer* player);

	void Show();

	int GetDigPosition(int& cx,int& cy);
	int GetFillPosition(int& cx,int& cy);
	int GetZeroPosition(int& cx,int& cy);

	int CheckDigPosition(int cx,int cy);
	int CheckFillPosition(int cx,int cy);
	int CheckZeroPosition(int cx,int cy);

	int GetClusterAttribute(int cx,int cy,int sx,int sy,int& prepared_zero,int& resource_zero,int& dig_layer,int& fill_layer,int& ground_zero,int& cell,int& zero_present);
	int GetHeight(){ return Height; };
};

struct terTerraformBorder : terTerraformFull
{
	terTerraformBorder(int id,int x,int y, terPlayer* player);

	int GetDigPosition(int& cx,int& cy);
	int GetFillPosition(int& cx,int& cy);
	int GetZeroPosition(int& cx,int& cy);

	int CheckDigPosition(int cx,int cy);
	int CheckFillPosition(int cx,int cy);
	int CheckZeroPosition(int cx,int cy);

	int GetClusterAttribute(int cx,int cy,int sx,int sy,int& prepared_zero,int& resource_zero,int& dig_layer,int& fill_layer,int& ground_zero,int& cell,int& zero_present);
};

//--------------------------------------------------

struct terTerraformLevelFull : terTerraformGeneral
{
	int Height;

	terTerraformLevelFull(int id,int x,int y, terPlayer* player) : terTerraformGeneral(id, x, y, player) {}
	
	void Show();
	int GetDigPosition(int& cx,int& cy);
	int GetFillPosition(int& cx,int& cy);
	int CheckDigPosition(int cx,int cy);
	int CheckFillPosition(int cx,int cy);
	int GetClusterAttribute(int cx,int cy,int sx,int sy,int& prepared_zero,int& resource_zero,int& dig_layer,int& fill_layer,int& ground_zero,int& cell,int& zero_present);
	int GetHeight(){ return Height; };
protected:
	bool IsOkDig(int x,int y);
	void AddFillDig(int x,int y,int& dig_layer,int& fill_layer);
};

struct terTerraformLevelBorder : terTerraformLevelFull
{
	MetaLockRegionDispatcher RegionPoint;

	terTerraformLevelBorder(int id,int x,int y, terPlayer* player) : terTerraformLevelFull( id, x, y, player) {}

	int GetDigPosition(int& cx,int& cy);
	int GetFillPosition(int& cx,int& cy);
	int CheckDigPosition(int cx,int cy);
	int CheckFillPosition(int cx,int cy);
	int GetClusterAttribute(int cx,int cy,int sx,int sy,int& prepared_zero,int& resource_zero,int& dig_layer,int& fill_layer,int& ground_zero,int& cell,int& zero_present);
};

//--------------------------------------------

struct terTerraformAbyssFull : terTerraformLevelFull
{
	terTerraformAbyssFull(int id,int x,int y,terPlayer* player);
};

struct terTerraformAbyssBorder : terTerraformLevelBorder
{
	terTerraformAbyssBorder(int id,int x,int y,terPlayer* player);
};

//--------------------------------------------------

const int TRUST_MAP_GARBAGE_SIZE = 16;
const int TRUST_MAP_GARBAGE_TEST_SIZE = 32;
const int TRUST_MAP_GARBAGE_LEVEL = 128;

class terTerraformDispatcher
{
public:
	terTerraformDispatcher(terPlayer* player);
	~terTerraformDispatcher();
	void Clear();
	void Show();

	terTerraformGeneral* AddElement(terTerraformType type, int x, int y, int id);
	void DeleteElement(terTerraformGeneral* element);

	void ScanTrustMap(int& request_digger,int& request_filler);
	void ScanTrustMap(int& request_digger,int& request_filler,int id);

	terTerraformGeneral* FindNearDigger(int x,int y);
	terTerraformGeneral* FindNearFiller(int x,int y);

	terTerraformGeneral* FindNearDigger(int x,int y,int id);
	terTerraformGeneral* FindNearFiller(int x,int y,int id);

	void GetWorkAreaStats(int& zero_request,int& zero_complete,int& abyss_request,int& abyss_complete);

	terTrustGrid TrustGrid; // Сетка не содержит мертвых элементов
	terPlayer* GetPlayer(){return Player;}
private:
	int Count;
	terPlayer* Player;

	TerraformList TerraformsDigFill,TerraformsOther;
	friend terTerraformGeneral;

	terTerraformGeneral* FindNear(int TerraformTypes, terTerraformStatus status, int x,int y, int id = -1);
	TerraformList::iterator DeleteElement(TerraformList::iterator ti);

	int zero_request,abyss_request;
	int zero_complete,abyss_complete;
};


// Adding Op for Region::scan
class terTerraformAddOp
{
	terTerraformType Type;
	int ID;
	terTerraformDispatcher& TrustMap;

public:
	terTerraformAddOp(terTerraformDispatcher& trust_map, terTerraformType type, int id) : TrustMap(trust_map) { Type = type; ID = id; }
	void operator()(int x, int y) { TrustMap.AddElement(Type, x, y, ID); }
};

#endif