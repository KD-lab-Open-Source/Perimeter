#pragma once

#include <SDL_mutex.h>

enum
{
	MT_GRAPH_THREAD=1 << 0,
	MT_LOGIC_THREAD=1 << 1,
};

uint32_t MT_GET_TYPE();
void MT_SET_TYPE(uint32_t val);
bool MT_IS_GRAPH();
bool MT_IS_LOGIC();

#define MTG() xassert(MT_IS_GRAPH())
#define MTL() xassert(MT_IS_LOGIC())

#define MTDECLARE(x) SDL_mutex* x
#define MTINIT(x) x = SDL_CreateMutex()
#define MTDONE(x) SDL_DestroyMutex(x)
#define MTENTER(x) SDL_LockMutex(x)
#define MTLEAVE(x) SDL_UnlockMutex(x)

void debug_dump_mt_tls();

struct MTEnter
{
    MTDECLARE(pcs);
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
    MTSection& operator=(MTSection const&) = delete;
};

class MTAuto
{
	MTSection* s;
public:
	explicit MTAuto(MTSection* s_) :s(s_) {
		s->Lock();
	}

	~MTAuto() {
		s->Unlock();
	}
};
