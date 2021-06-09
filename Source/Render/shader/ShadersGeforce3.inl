////////////////////////////Geforce3///////////////////////////////////

void VSGeforce3Scene::Select(const D3DXMATRIX* pmatlight,float shadow_map_size,
							const MatXf* world,vector<cUnkLight*>* light)
{
	SetFog();

	SetMatrix(mView,gb_RenderDevice3D->GetDrawNode()->matView);
	//c0-c3 - матрица преобразования в экранные координаты (view_proj_matrix)
	D3DXMATRIX mat;
	if(world)
	{
		cD3DRender_SetMatrix(mat,*world);
		SetMatrix(mWorld,&mat);//for light
		cD3DRender_SetMatrix(mat,*world);
		D3DXMatrixMultiply(&mat,&mat,gb_RenderDevice3D->GetDrawNode()->matView);
		SetMatrix(mWorldView,&mat);

		cD3DRender_SetMatrix(mat,*world);
		D3DXMatrixMultiply(&mat,&mat,gb_RenderDevice3D->GetDrawNode()->matViewProj);
	}else
	{
		SetMatrix(mView,gb_RenderDevice3D->GetDrawNode()->matView);

		mat=*(D3DXMATRIX*)gb_RenderDevice3D->GetDrawNode()->matViewProj;
	}

	SetMatrix(mWVP,&mat);

	D3DXMATRIX matlight;
	if(world)
	{
		cD3DRender_SetMatrix(mat,*world);
		D3DXMatrixMultiply(&matlight,&mat,pmatlight);
	}else
		matlight=*pmatlight;

	float fOffsetX = 0.5f + (0.5f / shadow_map_size);
	float fOffsetY = 0.5f + (0.5f / shadow_map_size);

	float range=1;
	//range=(0xFFFFFFFF>>(32-gb_RenderDevice3D->GetZDepth()));
	float fBias    = -0.005f*range;
	D3DXMATRIX matTexAdj( 0.5f,     0.0f,     0.0f,  0.0f,
		                  0.0f,    -0.5f,     0.0f,  0.0f,
						  0.0f,     0.0f,     range, 0.0f,
						  fOffsetX, fOffsetY, fBias, 1.0f );


	D3DXMatrixMultiply(&mat, &matlight, &matTexAdj);

	SetMatrix(mShadow,&mat);
	
	if(light)
	{
		cVertexShader::Select(min(light->size(),pShader.size()-1));
	}else
		cVertexShader::Select();
}

void VSGeforce3TileMapScene::SetWorldSize(Vect2f sz)
{
    D3DXVECTOR4 v(1/sz.x,1/sz.y,0,0);
	SetVector(fInvWorldSize,&v);
}

void VSGeforce3TileMapScene::GetHandle()
{
	VSGeforce3Scene::GetHandle();
	VAR_HANDLE(fInvWorldSize);
}

void VSGeforce3TileMapScene::RestoreShader()
{
	#include "Geforce3/o/tile_map_scene.vl"
}

void PSGeforce3TileMapScene::Restore()
{
	#include "Geforce3/o/tile_map_scene.ph"
}


void VSGeforce3ObjectSceneLight::RestoreShader()
{
	#include "Geforce3/o/object_scene_light.vl"
	#include "Geforce3/o/object_scene_light_p1.vl"
	#include "Geforce3/o/object_scene_light_p2.vl"
}

void VSGeforce3Scene::GetHandle()
{
	cVertexShader::GetHandle();
	VAR_HANDLE(mWVP);
	VAR_HANDLE(mShadow);
	VAR_HANDLE(mWorld);
	VAR_HANDLE(mView);

	VAR_HANDLE(vFog);
	VAR_HANDLE(mWorldView);

	VAR_HANDLE(vPointPos0);
	VAR_HANDLE(vPointColor0);
	VAR_HANDLE(vPointParam0);
	VAR_HANDLE(vPointPos1);
	VAR_HANDLE(vPointColor1);
	VAR_HANDLE(vPointParam1);
}

void VSGeforce3Scene::SetFog()
{
    D3DXVECTOR4 v = gb_RenderDevice3D->dtAdvance->GetFogParam();
	SetVector(vFog,&v);
}

void VSGeforce3ObjectSceneLight::GetHandle()
{
	VSGeforce3Scene::GetHandle();

	VAR_HANDLE(vAmbient);
	VAR_HANDLE(vDiffuse);
	VAR_HANDLE(vSpecular);
	VAR_HANDLE(fSpecularPower);
	VAR_HANDLE(vCameraPos);
	VAR_HANDLE(vLightDirection);
}

void VSGeforce3ObjectSceneLight::Select(const D3DXMATRIX* pmatlight,float shadow_map_size,
										const MatXf* world,vector<cUnkLight*>* light)
{
	if(light)
	{
		if(light->size()>0)
			SetLight(vPointPos0,vPointColor0,vPointParam0,(*light)[0]);

		if(light->size()>1)
			SetLight(vPointPos1,vPointColor1,vPointParam1,(*light)[1]);
	}
	VSGeforce3Scene::Select(pmatlight,shadow_map_size,world,light);
}

void VSGeforce3ObjectSceneLight::SetMaterial(sDataRenderMaterial *Data)
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

void PSGeforce3ObjectSceneLight::Restore()
{
	#include "Geforce3/o/object_scene_light.ph"
}

void PSGeforce3ObjectSceneLight2::Restore()
{
	#include "Geforce3/o/object_scene_light2.ph"
}

void VSGeforce3ObjectScene::RestoreShader()
{
	#include "Geforce3/o/object_scene.vl"
}

void PSGeforce3ObjectScene::Restore()
{
	#include "Geforce3/o/object_scene.ph"
}

void VSGeforce3ObjectSceneLightT::RestoreShader()
{
	#include "Geforce3/o/object_scene_lightT.vl"
}

void VSGeforce3ObjectSceneLight2::RestoreShader()
{
	#include "Geforce3/o/object_scene_light2.vl"
}

void VSGeforce3ObjectSceneLightT::SetTextureTransform(MatXf& m)
{
	D3DXVECTOR4 u(m.rot()[0][0],m.rot()[1][0],m.trans().x,0),
				v(m.rot()[0][1],m.rot()[1][1],m.trans().y,0);
	SetVector(vUtrans,&u);
	SetVector(vVtrans,&v);
}

void VSGeforce3ObjectSceneLightT::GetHandle()
{
	VSGeforce3ObjectSceneLight::GetHandle();
	VAR_HANDLE(vUtrans);
	VAR_HANDLE(vVtrans);
}


void VSGeforce3ObjectSceneBump::Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light)
{
	D3DXMATRIX mat;
	cD3DRender_SetMatrix(mat,*world);
	D3DXMatrixInverse(&mat,NULL,&mat);
	SetMatrix(mInvWorld,&mat);

	Vect3f l;
	D3DXVECTOR3 out;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
	l=-l;

	D3DXVec3TransformNormal(&out,(D3DXVECTOR3*)&l,&mat);
	D3DXVec3Normalize(&out,&out);
    D3DXVECTOR4 lv(out.x,out.y,out.z,0);
	SetVector(vLightDirectionInvWorld,&lv);

	if(light)
	{
		if(light->size()>0)
			SetLight(vPointPos0,vPointColor0,vPointParam0,(*light)[0]);

		if(light->size()>1)
			SetLight(vPointPos1,vPointColor1,vPointParam1,(*light)[1]);
	}

	VSGeforce3Scene::Select(matlight,shadow_map_size,world,light);
}

void VSGeforce3ObjectSceneBump::GetHandle()
{
	VSGeforce3Scene::GetHandle();
	VAR_HANDLE(mInvWorld);
	VAR_HANDLE(vCameraPos);
	VAR_HANDLE(vLightDirection);
	VAR_HANDLE(vLightDirectionInvWorld);
}

void VSGeforce3ObjectSceneBump::SetMaterial(sDataRenderMaterial *Data)
{
	Vect3f p=gb_RenderDevice3D->GetDrawNode()->GetPos();
	D3DXVECTOR4 cam(p.x,p.y,p.z,0);
	SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
	D3DXVECTOR4 light(l.x,l.y,l.z,0);
	SetVector(vLightDirection,&light);
}

void VSGeforce3ObjectSceneBump::RestoreShader()
{
#include "Geforce3/o/object_scene_bump.vl"
#include "Geforce3/o/object_scene_bump_p1.vl"
#include "Geforce3/o/object_scene_bump_p2.vl"
}

void PSGeforce3ObjectSceneBump::Restore()
{
#include "Geforce3/o/object_scene_bump.ph"
}

void VSGeforce3ObjectSceneBumpNoShadow::RestoreShader()
{
#include "Geforce3/o/object_scene_bump_no_shadow.vl"
#include "Geforce3/o/object_scene_bump_no_shadow_p1.vl"
#include "Geforce3/o/object_scene_bump_no_shadow_p2.vl"
}

void PSGeforce3ObjectSceneBumpNoShadow::Restore()
{
#include "Geforce3/o/object_scene_bump_no_shadow.ph"
}
