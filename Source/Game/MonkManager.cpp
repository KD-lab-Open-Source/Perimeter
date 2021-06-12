#include "StdAfx.h"
#include "MonkManager.h"
#include "terra.h"

MonkManager::MonkManager()
:grid(vMap.H_SIZE, vMap.V_SIZE)
{
}

MonkManager::~MonkManager()
{
	xassert(monks.empty());
	xassert(kill_list.empty());
}

terUnitMonk* MonkManager::create(const UnitTemplate& data)
{
	//потом переделать, для более аккуратного использования памяти.
	terUnitMonk* p=new terUnitMonk(data);
	monks.push_back(p);
	p->self_it=--monks.end();
	xassert(*p->self_it==p);
	return p;
}

void MonkManager::kill(terUnitMonk* m)
{
	kill_list.push_back(m);
}

void MonkManager::avatarInterpolation()
{
	terMonkList::iterator it;
	FOR_EACH(monks,it)
	{
		(*it)->avatarInterpolation();
	}
}

void MonkManager::quant()
{
	terMonkList::iterator it;
	FOR_EACH(monks,it)
	{
		(*it)->quant();
	}
	collision();
}

void MonkManager::deleteQuant()
{
	terMonkList::iterator it;
	FOR_EACH(kill_list,it)
	{
		terUnitMonk* p = *it;
		monks.erase(p->self_it);
		delete p;
	}
	kill_list.clear();
}

struct terMonkCollisionOperator
{
	float Radius;
	Vect3f Position;
 	terUnitMonk* ObjectPoint;

	terMonkCollisionOperator(terUnitMonk* p)
	{
		ObjectPoint = p;
		Position = ObjectPoint->position();
		Radius = ObjectPoint->radius();
	}

	void operator()(terUnitMonk* p)
	{
		if(p->player()!= ObjectPoint->player())
		if(Position.distance2(p->position()) < sqr(Radius + p->radius()))
		if(p->alive())
			p->Collision(ObjectPoint);
	}
};


void MonkManager::collision()
{
	terMonkList::iterator it;
	FOR_EACH(monks,it)
	{
		terUnitMonk* p = *it;
		if(p->alive())
		{
			int x = p->position().xi();
			int y = p->position().yi();
			int r = round(p->radius());

			terMonkCollisionOperator op(p);
			grid.Scan(x, y, r, op);
		}
	}
}

void MonkManager::Init()
{
	xassert(monks.empty());
	xassert(kill_list.empty());
}

void MonkManager::Done()
{
	deleteQuant();
	terMonkList::iterator it;
	FOR_EACH(monks,it)
	{
		terUnitMonk* p = *it;
		delete p;
	}
	monks.clear();
}
