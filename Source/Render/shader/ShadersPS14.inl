////////////////////////ps.1.4//////////
/*
void VS14Scene::Select(const D3DXMATRIX* pmatlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light)
{
	D3DXMATRIX matShadowModelTex,*matViewProj,mat;
	matViewProj=gb_RenderDevice3D->GetDrawNode()->matViewProj;
	if(world)
	{
		cD3DRender_SetMatrix(mat,*world);
		D3DXMatrixTranspose(&mat,&mat);
		gb_RenderDevice3D->SetVertexShaderConstant(28,&mat);

		D3DXMatrixTranspose(&mat,gb_RenderDevice3D->GetDrawNode()->matView);
		gb_RenderDevice3D->SetVertexShaderConstant(40,&mat);

		cD3DRender_SetMatrix(mat,*world);
		D3DXMatrixMultiplyTranspose(&mat,&mat,matViewProj);
	}else
		D3DXMatrixTranspose(&mat,matViewProj);
	gb_RenderDevice3D->SetVertexShaderConstant(0,&mat);

	D3DXMATRIX matlight;
	if(world)
	{
		cD3DRender_SetMatrix(mat,*world);
		D3DXMatrixMultiply(&matlight,&mat,pmatlight);
	}else
		matlight=*pmatlight;
	D3DXMatrixTranspose(&mat, &matlight);
	gb_RenderDevice3D->SetVertexShaderConstant(4,&mat);

	D3DXMATRIX matTexAdj(	0.5,      0,        0,        0,
							0,        -0.5,     0,        0,
							0,        0,        0,        0,
							0,		  0,		1,        1);


	if(gb_RenderDevice3D->GetTexture(1)==gb_RenderDevice3D->GetTexture(2))
	{
		//Четыре сэмпла выбираются
		float fC1 = 0.1f / shadow_map_size;
		float fC2 = 0.9f / shadow_map_size;

		matTexAdj._41 = 0.5f + fC1;
		matTexAdj._42 = 0.5f + fC1;
		D3DXMatrixMultiplyTranspose( &mat, &matlight, &matTexAdj );
		gb_RenderDevice3D->SetVertexShaderConstant(8,&mat);

		matTexAdj._41 = 0.5f + fC1;
		matTexAdj._42 = 0.5f + fC2;
		D3DXMatrixMultiplyTranspose( &mat, &matlight, &matTexAdj );
		gb_RenderDevice3D->SetVertexShaderConstant(12,&mat);

		matTexAdj._41 = 0.5f + fC2;
		matTexAdj._42 = 0.5f + fC1;
		D3DXMatrixMultiplyTranspose( &mat, &matlight, &matTexAdj );
		gb_RenderDevice3D->SetVertexShaderConstant(16,&mat);

		matTexAdj._41 = 0.5f + fC2;
		matTexAdj._42 = 0.5f + fC2;
		D3DXMatrixMultiplyTranspose( &mat, &matlight, &matTexAdj );
		gb_RenderDevice3D->SetVertexShaderConstant(20,&mat);
	}else
	{
		//Три сэмпла выбираются
		//x=0.5+0.4*sin(2*PI/3*i),y=0.5+0.4*cos(2*PI/3*i)

		matTexAdj._41 = 0.5f + 0.1f/shadow_map_size;
		matTexAdj._42 = 0.5f + 0.1f/shadow_map_size;
		D3DXMatrixMultiplyTranspose( &mat, &matlight, &matTexAdj );
		gb_RenderDevice3D->SetVertexShaderConstant(8,&mat);

		matTexAdj._41 = 0.5f + 0.9f/shadow_map_size;
		matTexAdj._42 = 0.5f + 0.1f/shadow_map_size;
		D3DXMatrixMultiplyTranspose( &mat, &matlight, &matTexAdj );
		gb_RenderDevice3D->SetVertexShaderConstant(12,&mat);

		matTexAdj._41 = 0.5f + 0.5f/shadow_map_size;
		matTexAdj._42 = 0.5f + 0.9f/shadow_map_size;
		D3DXMatrixMultiplyTranspose( &mat, &matlight, &matTexAdj );
		gb_RenderDevice3D->SetVertexShaderConstant(16,&mat);
	}
	
	D3DXVECTOR4	vZBias = D3DXVECTOR4( -0.01f, 0.0f, 0.0f, 0.0f );
	gb_RenderDevice3D->SetVertexShaderConstant(24,&vZBias);

	cVertexShader::Select();
}
*/

void VS14Scene::Select(const D3DXMATRIX* pmatlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light)
{
	//c0-c3 - матрица преобразования в экранные координаты (view_proj_matrix)
	D3DXMATRIX mat;
	if(world)
	{
		cD3DRender_SetMatrix(mat,*world);
		SetMatrix(mWorld,&mat);//for light
		SetMatrix(mView,gb_RenderDevice3D->GetDrawNode()->matView);

		cD3DRender_SetMatrix(mat,*world);
		D3DXMatrixMultiply(&mat,&mat,gb_RenderDevice3D->GetDrawNode()->matViewProj);
	}else
	{
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

	SetMatrix(mWorldToLight,&matlight);

	D3DXMATRIX matTexAdj(	0.5,      0,        0,        0,
							0,        -0.5,     0,        0,
							0,        0,        0,        0,
							0,		  0,		1,        1);

	const float c1=0.1f,c2=0.9f,c3=0.5f;
//	const c1=0.1-0.4f,c2=0.9f+0.4f,c3=0.5f+0.4f;

	if(gb_RenderDevice3D->GetTexture(1)==gb_RenderDevice3D->GetTexture(2))
	{
		//Четыре сэмпла выбираются
		float fC1 = c1 / shadow_map_size;
		float fC2 = c2 / shadow_map_size;

		matTexAdj._41 = 0.5f + fC1;
		matTexAdj._42 = 0.5f + fC1;
		D3DXMatrixMultiply( &mat, &matlight, &matTexAdj );
		SetMatrix(mShadow0,&mat);

		matTexAdj._41 = 0.5f + fC1;
		matTexAdj._42 = 0.5f + fC2;
		D3DXMatrixMultiply( &mat, &matlight, &matTexAdj );
		SetMatrix(mShadow1,&mat);

		matTexAdj._41 = 0.5f + fC2;
		matTexAdj._42 = 0.5f + fC1;
		D3DXMatrixMultiply( &mat, &matlight, &matTexAdj );
		SetMatrix(mShadow2,&mat);

		matTexAdj._41 = 0.5f + fC2;
		matTexAdj._42 = 0.5f + fC2;
		D3DXMatrixMultiply( &mat, &matlight, &matTexAdj );
		SetMatrix(mShadow3,&mat);
	}else
	{
		//Три сэмпла выбираются
		//x=0.5+0.4*sin(2*PI/3*i),y=0.5+0.4*cos(2*PI/3*i)

		matTexAdj._41 = 0.5f + c1/shadow_map_size;
		matTexAdj._42 = 0.5f + c1/shadow_map_size;
		D3DXMatrixMultiply( &mat, &matlight, &matTexAdj );
		SetMatrix(mShadow0,&mat);

		matTexAdj._41 = 0.5f + c2/shadow_map_size;
		matTexAdj._42 = 0.5f + c1/shadow_map_size;
		D3DXMatrixMultiply( &mat, &matlight, &matTexAdj );
		SetMatrix(mShadow1,&mat);

		matTexAdj._41 = 0.5f + c3/shadow_map_size;
		matTexAdj._42 = 0.5f + c2/shadow_map_size;
		D3DXMatrixMultiply( &mat, &matlight, &matTexAdj );
		SetMatrix(mShadow2,&mat);
	}
	
	D3DXVECTOR4	zbias = D3DXVECTOR4( -0.005f, 0.0f, 0.0f, 0.0f );
	SetVector(vZBias,&zbias);

	if(light)
	{
		cVertexShader::Select(min(light->size(),pShader.size()-1));
	}else
		cVertexShader::Select();
}

void VS14Scene::GetHandle()
{
	cVertexShader::GetHandle();
	VAR_HANDLE(mWVP);
	VAR_HANDLE(mWorldToLight);
	VAR_HANDLE(mShadow0);
	VAR_HANDLE(mShadow1);
	VAR_HANDLE(mShadow2);
	VAR_HANDLE(mShadow3);
	VAR_HANDLE(mWorld);
	VAR_HANDLE(mView);
	VAR_HANDLE(vZBias);
}

void VS14ObjectSceneLight::GetHandle()
{
	VS14Scene::GetHandle();

	VAR_HANDLE(vAmbient);
	VAR_HANDLE(vDiffuse);
	VAR_HANDLE(vSpecular);
	VAR_HANDLE(fSpecularPower);
	VAR_HANDLE(vCameraPos);
	VAR_HANDLE(vLightDirection);

	VAR_HANDLE(vPointPos0);
	VAR_HANDLE(vPointColor0);
	VAR_HANDLE(vPointParam0);
	VAR_HANDLE(vPointPos1);
	VAR_HANDLE(vPointColor1);
	VAR_HANDLE(vPointParam1);
}

void VS14ObjectSceneLight::SetMaterial(sDataRenderMaterial *Data)
{
	SetVector(vAmbient,(D3DXVECTOR4*)&Data->Ambient);
	SetVector(vDiffuse,(D3DXVECTOR4*)&Data->Diffuse);
	SetVector(vSpecular,(D3DXVECTOR4*)&Data->Specular);
	SetFloat(fSpecularPower,Data->Power);
	Vect3f p=gb_RenderDevice3D->GetDrawNode()->GetPos();
    D3DXVECTOR4 cam(p.x,p.y,p.z,0);
    SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
    D3DXVECTOR4 light(l.x,l.y,l.z,0);
    SetVector(vLightDirection,&light);
}

void VS14ObjectSceneLight::Select(const D3DXMATRIX* pmatlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light)
{
	if(light)
	{
		if(light->size()>0)
			SetLight(vPointPos0,vPointColor0,vPointParam0,(*light)[0]);

		if(light->size()>1)
			SetLight(vPointPos1,vPointColor1,vPointParam1,(*light)[1]);
	}

	VS14Scene::Select(pmatlight,shadow_map_size,world,light);
}

void VS14TileMapShadow::RestoreShader()
{
#include "ps.1.4/o/tile_map_shadow.vl"
}

void PS14TileMapShadow::Restore()
{
#include "ps.1.4/o/tile_map_shadow.ph"
}

void VS14TileMapScene::GetHandle()
{
	VS14Scene::GetHandle();
	VAR_HANDLE(fInvWorldSize);
}

void VS14TileMapScene::SetWorldSize(Vect2f sz)
{
    D3DXVECTOR4 v(1/sz.x,1/sz.y,0,0);
	SetVector(fInvWorldSize,&v);
}

void VS14TileMapScene::RestoreShader()
{
	#include "ps.1.4/o/tile_map_scene.vl"
}

void PS14TileMapScene::Restore()
{
	#include "ps.1.4/o/tile_map_scene.ph"
}

void VS14ObjectShadow::RestoreShader()
{
	#include "ps.1.4/o/object_shadow.vl"
}

void PS14ObjectShadow::Restore()
{
	#include "ps.1.4/o/object_shadow.ph"
}

void VS14ObjectSceneLight::RestoreShader()
{
	#include "ps.1.4/o/object_scene_light.vl"
	#include "ps.1.4/o/object_scene_light_p1.vl"
	#include "ps.1.4/o/object_scene_light_p2.vl"
}

void PS14ObjectSceneLight::Restore()
{
	#include "ps.1.4/o/object_scene_light.ph"
}

void VS14ObjectScene::SetMaterial(sDataRenderMaterial *Data)
{
	SetVector(vDiffuse,(D3DXVECTOR4*)&Data->Diffuse);
}

void VS14ObjectScene::GetHandle()
{
	VS14Scene::GetHandle();
	VAR_HANDLE(vDiffuse);
}

void VS14ObjectScene::RestoreShader()
{
	#include "ps.1.4/o/object_scene.vl"
}

void PS14ObjectScene::Restore()
{
	#include "ps.1.4/o/object_scene.ph"
}

void VS14ObjectSceneLight2::GetHandle()
{
	VS14ObjectSceneLight::GetHandle();
	VAR_HANDLE(vUtrans);
	VAR_HANDLE(vVtrans);
}

void VS14ObjectSceneLight2::SetTextureTransform(MatXf& m)
{
	D3DXVECTOR4 u(m.rot()[0][0],m.rot()[1][0],m.trans().x,0),
				v(m.rot()[0][1],m.rot()[1][1],m.trans().y,0);
	SetVector(vUtrans,&u);
	SetVector(vVtrans,&v);
}

void VS14ObjectSceneLight2::RestoreShader()
{
	#include "ps.1.4/o/object_scene_light2.vl"
}

void PS14ObjectSceneLight2::Restore()
{
	#include "ps.1.4/o/object_scene_light2.ph"
}

void VS14ObjectSceneBump::SetMaterial(sDataRenderMaterial *Data)
{
	Vect3f p=gb_RenderDevice3D->GetDrawNode()->GetPos();
    D3DXVECTOR4 cam(p.x,p.y,p.z,0);
    SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
    D3DXVECTOR4 light(l.x,l.y,l.z,0);
    SetVector(vLightDirection,&light);

    D3DXVECTOR4 c(0.5,0.5,0.5,0);
	SetVector(c05,&c);
}

void VS14ObjectSceneBump::GetHandle()
{
	VS14ObjectSceneLight::GetHandle();
	VAR_HANDLE(mInvWorld);
	VAR_HANDLE(vCameraPos);
	VAR_HANDLE(vLightDirection);
	VAR_HANDLE(vLightDirectionInvWorld);
	VAR_HANDLE(c05);
}

void VS14ObjectSceneBump::Select(const D3DXMATRIX* matlight,float shadow_map_size,const MatXf* world,vector<cUnkLight*>* light)
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

	VS14ObjectSceneLight::Select(matlight,shadow_map_size,world,light);
}

void VS14ObjectSceneBump::RestoreShader()
{
#include "ps.1.4/o/object_scene_bump.vl"
#include "ps.1.4/o/object_scene_bump_p1.vl"
#include "ps.1.4/o/object_scene_bump_p2.vl"
}

void PS14ObjectSceneBump::Restore()
{
#include "ps.1.4/o/object_scene_bump.ph"
}

void VS14ObjectSceneBumpNoShadow::RestoreShader()
{
	#include "ps.1.4/o/object_scene_bump_no_shadow.vl"
	#include "ps.1.4/o/object_scene_bump_no_shadow_p1.vl"
	#include "ps.1.4/o/object_scene_bump_no_shadow_p2.vl"
}

void PS14ObjectSceneBumpNoShadow::Restore()
{
#include "ps.1.4/o/object_scene_bump_no_shadow.ph"
}
