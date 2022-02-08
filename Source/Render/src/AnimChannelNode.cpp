#include "StdAfxRD.h"
#include "AnimChannelNode.h"
#ifdef PERIMETER_DEBUG_ASSERT
#include <cmath>
#endif

cAnimChainNode::cAnimChainNode()
{
	Time=0;
}
cAnimChainNode::~cAnimChainNode()
{
}

void cAnimChainNode::GetMatrix(float phase,MatXf &Matrix)
{
	VISASSERT(GetNumberMatrix());
	if(GetNumberMatrix()==1)
		Matrix=GetMatrix(0).mat;
	else 
	{
		float CurrentTime=phase*Time;
/*
		int i = BinaryFind( CurrentTime, KeyMatrix );
/*/
		int i=phase*(GetNumberMatrix()-1);
		if(i>GetNumberMatrix()-2)
			i=GetNumberMatrix()-2;
		if(i<0)i=0;
/**/
		sKeyMatrix *a=&GetMatrix(i),*b=&GetMatrix(i+1);
		float f=(CurrentTime-a->time)/(b->time-a->time);
		MatrixInterpolate(Matrix,a->mat,b->mat,f);
/*
		Matrix.rot()[0][0]=LinearInterpolate(a->mat.rot()[0][0],b->mat.rot()[0][0],f);
		Matrix.rot()[0][1]=LinearInterpolate(a->mat.rot()[0][1],b->mat.rot()[0][1],f);
		Matrix.rot()[0][2]=LinearInterpolate(a->mat.rot()[0][2],b->mat.rot()[0][2],f);
		Matrix.rot()[1][0]=LinearInterpolate(a->mat.rot()[1][0],b->mat.rot()[1][0],f);
		Matrix.rot()[1][1]=LinearInterpolate(a->mat.rot()[1][1],b->mat.rot()[1][1],f);
		Matrix.rot()[1][2]=LinearInterpolate(a->mat.rot()[1][2],b->mat.rot()[1][2],f);
		Matrix.rot()[2][0]=LinearInterpolate(a->mat.rot()[2][0],b->mat.rot()[2][0],f);
		Matrix.rot()[2][1]=LinearInterpolate(a->mat.rot()[2][1],b->mat.rot()[2][1],f);
		Matrix.rot()[2][2]=LinearInterpolate(a->mat.rot()[2][2],b->mat.rot()[2][2],f);
		Matrix.trans().x=LinearInterpolate(a->mat.trans().x,b->mat.trans().x,f);
		Matrix.trans().y=LinearInterpolate(a->mat.trans().y,b->mat.trans().y,f);
		Matrix.trans().z=LinearInterpolate(a->mat.trans().z,b->mat.trans().z,f);
*/
	}
	VISASSERT(std::isfinite(Matrix.rot()(1,1))&&std::isfinite(Matrix.trans().x));
}

void cAnimChainNode::GetVisible(float phase,int &visible)
{
	VISASSERT(GetNumberVisible());
	if(GetNumberVisible()==1)
		visible=GetVisible(0).visible;
	else
	{
		float CurrentTime=phase*Time;
		int i = BinaryFind( CurrentTime, KeyVisible );
		if( CurrentTime>=GetVisible(i+1).time ) visible=GetVisible(i+1).visible;
		else visible=GetVisible(i).visible;
	}
}

cAnimChannelNode::~cAnimChannelNode ()
{ 
	clear(); 
}
