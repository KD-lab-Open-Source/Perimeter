////////////////////////////Geforce3///////////////////////////////////

void VSGeforce3Scene::Select(const Mat4f* pmatlight,float shadow_map_size,
							const MatXf* world, std::vector<cUnkLight*>* light)
{
	SetFog();

	SetMatrix(mView, &gb_RenderDevice3D->GetDrawNode()->matView);
	//c0-c3 - матрица преобразования в экранные координаты (view_proj_matrix)
	Mat4f mat;
	if(world)
	{
        Mat4fSetTransposedMatXf(mat, *world);
		SetMatrix(mWorld,&mat);//for light
        Mat4fSetTransposedMatXf(mat, *world);
		mat = mat * gb_RenderDevice3D->GetDrawNode()->matView;
		SetMatrix(mWorldView,&mat);

        Mat4fSetTransposedMatXf(mat, *world);
        mat = mat * gb_RenderDevice3D->GetDrawNode()->matView;
	}else
	{
		SetMatrix(mView, &gb_RenderDevice3D->GetDrawNode()->matView);

		mat = gb_RenderDevice3D->GetDrawNode()->matViewProj;
	}

	SetMatrix(mWVP,&mat);

	Mat4f matlight;
	if(world) {
        Mat4fSetTransposedMatXf(mat, *world);
		matlight = mat * *pmatlight;
	} else {
        matlight = *pmatlight;
    }

	float fOffsetX = 0.5f + (0.5f / shadow_map_size);
	float fOffsetY = 0.5f + (0.5f / shadow_map_size);

	float range=1;
	//range=(0xFFFFFFFF>>(32-gb_RenderDevice3D->GetZDepth()));
	float fBias    = -0.005f*range;
	Mat4f matTexAdj( 0.5f,     0.0f,     0.0f,  0.0f,
		             0.0f,    -0.5f,     0.0f,  0.0f,
					 0.0f,     0.0f,     range, 0.0f,
					 fOffsetX, fOffsetY, fBias, 1.0f );


	mat = matlight * matTexAdj;

	SetMatrix(mShadow,&mat);
	
	if(light)
	{
		cVertexShader::Select((std::min)(light->size(),pShader.size()-1));
	}else
		cVertexShader::Select();
}

void VSGeforce3TileMapScene::SetWorldSize(Vect2f sz)
{
    Vect4f v(1/sz.x,1/sz.y,0,0);
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
    Vect4f v = gb_RenderDevice3D->dtAdvance->GetFogParam();
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

void VSGeforce3ObjectSceneLight::Select(const Mat4f* pmatlight,float shadow_map_size,
										const MatXf* world, std::vector<cUnkLight*>* light)
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
	SetVector(vAmbient, reinterpret_cast<const Vect4f*>(&Data->Ambient));
	SetVector(vDiffuse, reinterpret_cast<const Vect4f*>(&Data->Diffuse));
	SetVector(vSpecular, reinterpret_cast<const Vect4f*>(&Data->Specular));
    Vect4f sp(Data->Power,Data->Power,Data->Power,Data->Power);
	SetVector(fSpecularPower,&sp);
	Vect3f p=gb_RenderDevice3D->GetDrawNode()->GetPos();
    Vect4f cam(p.x,p.y,p.z,0);
	SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
    Vect4f light(l.x,l.y,l.z,0);
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
	Vect4f u(m.rot()[0][0],m.rot()[1][0],m.trans().x,0),
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


void VSGeforce3ObjectSceneBump::Select(const Mat4f* matlight,float shadow_map_size,const MatXf* world, std::vector<cUnkLight*>* light)
{
	Mat4f mat;
    Mat4fSetTransposedMatXf(mat, *world);
	Mat4fInverse(&mat,NULL,&mat);
	SetMatrix(mInvWorld,&mat);

	Vect3f l;
    Vect3f out;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
	l=-l;

    Vect3fTransformNormal(&out, &l, &mat);
    Vect3fNormalize(&out, &out);
    Vect4f lv(out.x,out.y,out.z,0);
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
	Vect4f cam(p.x,p.y,p.z,0);
	SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
	Vect4f light(l.x,l.y,l.z,0);
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
