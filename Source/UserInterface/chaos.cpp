#include "StdAfx.h"
#include "Runtime.h"
#include "terra.h"
#include "chaos.h"
#include "files/files.h"


CChaos::CChaos(int hsize,int vsize,bool bump)
{
	int tile = 5;//Пусть будет нечётная

	ZPlane=NULL;
	ZPlane = terScene->CreateChaos(Vect2f(hsize,vsize),
		"RESOURCE\\EFFECT\\WorldGround.tga","RESOURCE\\EFFECT\\WorldGround01.tga",
		"RESOURCE\\EFFECT\\Chaos\\oceanBump.tga",tile,bump);

    //Get path of world dir
    std::string texturePath = GetTargetName("");
    pSkySphere = nullptr;
    pSkySphere = terScene->CreateSkySpere("RESOURCE\\Models\\FX\\skysphere.M3D", texturePath.c_str(),vMap.H_SIZE);
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



