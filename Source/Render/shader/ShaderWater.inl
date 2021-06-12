VSWater::VSWater()
{
	technique=0;
}

void VSWater::SetSpeed(Vect2f scale,Vect2f offset)
{
	D3DXVECTOR4 s(scale.x,scale.y,offset.x,offset.y);
	SetVector(uvScaleOffset,&s);
}

void VSWater::SetSpeed1(Vect2f scale,Vect2f offset)
{
	D3DXVECTOR4 s(scale.x,scale.y,offset.x,offset.y);
	SetVector(uvScaleOffset1,&s);
}

void VSWater::SetSpeedSky(Vect2f scale,Vect2f offset)
{
	D3DXVECTOR4 s(scale.x,scale.y,offset.x,offset.y);
	SetVector(uvScaleOffsetSky,&s);
}

void VSWater::SetWorldScale(Vect2f scale)
{
	D3DXVECTOR4 s(scale.x,scale.y,0,0);
	SetVector(worldScale,&s);
}

void VSWater::Select()
{
	SetMatrix(mVP,gb_RenderDevice3D->GetDrawNode()->matViewProj);
	Vect3f p=gb_RenderDevice3D->GetDrawNode()->GetPos();
    D3DXVECTOR4 cam(p.x,p.y,p.z,0);
	SetVector(vCameraPos,&cam);
	cVertexShader::Select();
}

void VSWater::GetHandle()
{
	cVertexShader::GetHandle();
	VAR_HANDLE(mVP);
	VAR_HANDLE(uvScaleOffset);
	VAR_HANDLE(uvScaleOffset1);
	VAR_HANDLE(uvScaleOffsetSky);
	VAR_HANDLE(worldScale);
	VAR_HANDLE(vCameraPos);
}

void VSWater::RestoreShader()
{
	if(technique==2)
	{
		#include "Water/o/water_cube.vl"
	}else
	if(technique==1)
	{
		#include "Water/o/water_easy.vl"
	}else
	{
		#include "Water/o/water.vl"
	}
}

PSWater::PSWater()
{
	technique=0;
}

void PSWater::Restore()
{
	if(technique==2)
	{
		#include "Water/o/water_cube.ph"
	}else
	if(technique==1)
	{
		#include "Water/o/water_easy.ph"
	}else
	{
		#include "Water/o/water.ph"
	}
}

void PSWater::SetTechnique(int t)
{
	technique=t;
	Restore();
}

void VSWater::SetTechnique(int t)
{
	technique=t;
	Restore();
}