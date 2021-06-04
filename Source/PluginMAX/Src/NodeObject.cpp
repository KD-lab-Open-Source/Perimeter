#include "SceneMesh.h"

void sNodeObject::Release()							
{ 
	ID=0; 
	name.clear(); 
	parent.clear(); 
	memset(matrix,0,12*sizeof(float)); 
	AnimationVisibility.Release();
	AnimationPosition.Release();
	AnimationRotation.Release();
	AnimationScale.Release();
	AnimationScaleRotation.Release();
}
