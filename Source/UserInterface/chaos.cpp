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

    //Get path of world, remove absolute path since we might need to access .pak textures
    std::string texturePath = GetTargetName("");
    if (startsWith(texturePath, get_content_root_path())) {
        string_replace_all(texturePath, get_content_root_path(), "");
    }
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



