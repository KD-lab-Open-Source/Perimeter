#pragma once

#include <SDL_mutex.h>

extern THREAD_LOCAL DWORD tls_is_graph;
enum
{
	MT_GRAPH_THREAD=1,
	MT_LOGIC_THREAD=2,
};

#define MTG() xassert(tls_is_graph&MT_GRAPH_THREAD)
#define MTL() xassert(tls_is_graph&MT_LOGIC_THREAD)
#define MT_IS_GRAPH()  (tls_is_graph&MT_GRAPH_THREAD)
#define MT_IS_LOGIC()  (tls_is_graph&MT_LOGIC_THREAD)

#define MTDECLARE(x) SDL_mutex* x;
#define MTINIT(x) x = SDL_CreateMutex()
#define MTDONE(x) SDL_DestroyMutex(x)
#define MTENTER(x) SDL_LockMutex(x)
#define MTLEAVE(x) SDL_UnlockMutex(x)

struct MTEnter
{
    SDL_mutex* pcs;
	MTEnter(SDL_mutex* pcs_)
	{
		pcs=pcs_;
		MTENTER(pcs);
	}

	~MTEnter()
	{
		MTLEAVE(pcs);
	}
};

class MTSection
{
	MTDECLARE(cs);
	int num_lock;
public:
	MTSection(const MTSection& in)
	{
		MTINIT(cs);
		num_lock=0;
	}

	MTSection()
	{
		MTINIT(cs);
		num_lock=0;
	}

	~MTSection()
	{
		MTDONE(cs);
	}

	bool is_lock()const{return num_lock>0;}

	void Lock()
	{
		MTENTER(cs);
		num_lock++;
	}

	void Unlock()
	{
		num_lock--;
		xassert(num_lock>=0);
		MTLEAVE(cs);
	}

	//Not copy this object
	void operator =(const MTSection& in)
	{
	}
};

class MTAuto
{
	MTSection* s;
public:
	MTAuto(MTSection* s_)
		:s(s_)
	{
		s->Lock();
	}

	~MTAuto()
	{
		s->Unlock();
	}
};

/*
	Отключает на области видимости assert`ы связанные с многопоточностью.
*/
class MTAutoSkipAssert
{
	DWORD real_tls;
public:
	MTAutoSkipAssert()
	{
		real_tls=tls_is_graph;
		tls_is_graph=MT_GRAPH_THREAD|MT_LOGIC_THREAD;
	}

	~MTAutoSkipAssert()
	{
		tls_is_graph=real_tls;
	}
};