#include "StdAfx.h"
#include "Runtime.h"
#include "terra.h"
#include "chaos.h"


CChaos::CChaos(int hsize,int vsize,bool bump)
{
	int tile = 5;//Пусть будет нечётная

	ZPlane=NULL;
	ZPlane = terScene->CreateChaos(Vect2f(hsize,vsize),
		"RESOURCE\\EFFECT\\WorldGround.tga","RESOURCE\\EFFECT\\WorldGround01.tga",
		"RESOURCE\\EFFECT\\Chaos\\oceanBump.tga",tile,bump);

	pSkySphere=NULL;
	pSkySphere = terScene->CreateSkySpere("RESOURCE\\Models\\FX\\skysphere.M3D",GetTargetName(""),vMap.H_SIZE);
}

CChaos::~CChaos()
{
	RELEASE(ZPlane);
	RELEASE(pSkySphere);
}

void CChaos::Draw()
{
	if(ZPlane)ZPlane->RenderAllTexture();
}



