#include "StdAfxRD.h"
#include "../src/Scene.h"
#include "ForceField.h"
#include "ExternalObj.h"
#include "../src/ObjLibrary.h"

FieldDispatcher* cScene::CreateForceFieldDispatcher(int xmax,int ymax, int zeroLayerHeight, const char* TextureName1, const char* TextureName2)
{ 
	FieldDispatcher *Obj = new FieldDispatcher(xmax, ymax, zeroLayerHeight);
	Obj->SetTexture(0,GetTexLibrary()->GetElement(TextureName1));
	Obj->SetTexture(1,GetTexLibrary()->GetElement(TextureName2));
	this->AttachObj(Obj);
	return Obj;
}

cExternalObj* cScene::CreateExternalObj(ExternalObjFunction func, const char* TextureName1)
{ 
	cExternalObj* Obj = new cExternalObj(func);
	Obj->SetTexture(0,GetTexLibrary()->GetElement(TextureName1));
	this->AttachObj(Obj);
	return Obj;
}
