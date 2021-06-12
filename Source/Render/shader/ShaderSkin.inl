void VSSkin::Select(const D3DXMATRIX* pmatlight,float shadow_map_size,
							vector<MatXf>& world,vector<cUnkLight*>* light,int blend_num)
{
	SetMatrix(mView,gb_RenderDevice3D->GetDrawNode()->matView);
	//c0-c3 - матрица преобразования в экранные координаты (view_proj_matrix)
	D3DXMATRIX mat;

	for(int i=0;i<world.size();i++)
	{
		MatXf& w=world[i];
		cD3DRender_SetMatrix(mat,w);
		SetMatrix4x3(mWorldM,i,&mat);//for light
	}

	SetMatrix(mVP,gb_RenderDevice3D->GetDrawNode()->matViewProj);
	
	xassert(blend_num>=1 && blend_num<=pShader.size());
	cVertexShader::Select(blend_num-1);
}

void VSSkin::RestoreShader()
{
	#include "Skin/o/object_scene_light_w1.vl"
	#include "Skin/o/object_scene_light_w2.vl"
	#include "Skin/o/object_scene_light_w3.vl"
	#include "Skin/o/object_scene_light_w4.vl"
}

void VSSkin::GetHandle()
{
	cVertexShader::GetHandle();
	VAR_HANDLE(mVP);
	VAR_HANDLE(mWorldM);
	VAR_HANDLE(mView);

	VAR_HANDLE(vPointPos0);
	VAR_HANDLE(vPointColor0);
	VAR_HANDLE(vPointParam0);
	VAR_HANDLE(vPointPos1);
	VAR_HANDLE(vPointColor1);
	VAR_HANDLE(vPointParam1);

	VAR_HANDLE(vAmbient);
	VAR_HANDLE(vDiffuse);
	VAR_HANDLE(vSpecular);
	VAR_HANDLE(fSpecularPower);
	VAR_HANDLE(vCameraPos);
	VAR_HANDLE(vLightDirection);
}

void VSSkin::SetMaterial(sDataRenderMaterial *Data)
{
	SetVector(vAmbient,(D3DXVECTOR4*)&Data->Ambient);
	SetVector(vDiffuse,(D3DXVECTOR4*)&Data->Diffuse);
	SetVector(vSpecular,(D3DXVECTOR4*)&Data->Specular);
    D3DXVECTOR4 sp(Data->Power,Data->Power,Data->Power,Data->Power);
	SetVector(fSpecularPower,&sp);
	Vect3f p=gb_RenderDevice3D->GetDrawNode()->GetPos();
    D3DXVECTOR4 cam(p.x,p.y,p.z,0);
    SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
    D3DXVECTOR4 light(l.x,l.y,l.z,0);
    SetVector(vLightDirection,&light);
}

void PSSkin::Restore()
{
	#include "Skin/o/object_scene_light.ph"
}


///////////////////////////////////////bump////////////////////////////
void VSSkinBump::Select(const D3DXMATRIX* pmatlight,float shadow_map_size,
							vector<MatXf>& world,vector<cUnkLight*>* light,int blend_num)
{
	SetMatrix(mView,gb_RenderDevice3D->GetDrawNode()->matView);
	//c0-c3 - матрица преобразования в экранные координаты (view_proj_matrix)
	D3DXMATRIX mat;

	for(int i=0;i<world.size();i++)
	{
		MatXf& w=world[i];
		cD3DRender_SetMatrix(mat,w);
		SetMatrix4x3(mWorldM,i,&mat);//for light
	}

	SetMatrix(mVP,gb_RenderDevice3D->GetDrawNode()->matViewProj);
	
	xassert(blend_num>=1 && blend_num<=pShader.size());
	cVertexShader::Select(blend_num-1);
}

void VSSkinBump::RestoreShader()
{
	#include "Skin/o/object_scene_bump_w1.vl"
	#include "Skin/o/object_scene_bump_w2.vl"
	#include "Skin/o/object_scene_bump_w3.vl"
	#include "Skin/o/object_scene_bump_w4.vl"
}

void VSSkinBump::GetHandle()
{
	cVertexShader::GetHandle();
	VAR_HANDLE(mVP);
	VAR_HANDLE(mWorldM);
	VAR_HANDLE(mView);

	VAR_HANDLE(vPointPos0);
	VAR_HANDLE(vPointColor0);
	VAR_HANDLE(vPointParam0);
	VAR_HANDLE(vPointPos1);
	VAR_HANDLE(vPointColor1);
	VAR_HANDLE(vPointParam1);

	VAR_HANDLE(vAmbient);
	VAR_HANDLE(vDiffuse);
	VAR_HANDLE(vSpecular);
	VAR_HANDLE(fSpecularPower);
	VAR_HANDLE(vCameraPos);
	VAR_HANDLE(vLightDirection);
}

void VSSkinBump::SetMaterial(sDataRenderMaterial *Data)
{
	SetVector(vAmbient,(D3DXVECTOR4*)&Data->Ambient);
	SetVector(vDiffuse,(D3DXVECTOR4*)&Data->Diffuse);
	SetVector(vSpecular,(D3DXVECTOR4*)&Data->Specular);
    D3DXVECTOR4 sp(Data->Power,Data->Power,Data->Power,Data->Power);
    SetVector(fSpecularPower,&sp);
	Vect3f p=gb_RenderDevice3D->GetDrawNode()->GetPos();
    D3DXVECTOR4 cam(p.x,p.y,p.z,0);
    SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
    D3DXVECTOR4 light(l.x,l.y,l.z,0);
    SetVector(vLightDirection,&light);
}

void PSSkinBump::Restore()
{
	#include "Skin/o/object_scene_bump.ph"
}

void PSSkinBump::SetMaterial(sDataRenderMaterial *Data)
{
	gb_RenderDevice3D->SetPixelShaderConstant(20,(D3DXVECTOR4*)&Data->Ambient);
	gb_RenderDevice3D->SetPixelShaderConstant(21,(D3DXVECTOR4*)&Data->Diffuse);
	D3DXVECTOR4 specular(Data->Specular.r,Data->Specular.g,Data->Specular.b,Data->Power);
	gb_RenderDevice3D->SetPixelShaderConstant(22,&specular);
	//gb_RenderDevice3D->SetPixelShaderConstant(22,&D3DXVECTOR4(1,1,1,Data->Power));
}
