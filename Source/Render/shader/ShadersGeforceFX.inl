////////////////////////////GeforceFX///////////////////////////////////

void VSGeforceFXScene::Select(const Mat4f* pmatlight,float shadow_map_size,
							const MatXf* world, std::vector<cUnkLight*>* light)
{
	SetFog();

	SetMatrix(mView, &gb_RenderDevice3D->GetDrawNode()->matView);
	//c0-c3 - матрица преобразования в экранные координаты (view_proj_matrix)
	Mat4f mat;
    
	if(world) {
        Mat4fSetTransposedMatXf(mat, *world);
		SetMatrix(mWorld,&mat);//for light
        Mat4fSetTransposedMatXf(mat, *world);
		mat *= gb_RenderDevice3D->GetDrawNode()->matView;
		SetMatrix(mWorldView,&mat);

        Mat4fSetTransposedMatXf(mat, *world);
		mat *= gb_RenderDevice3D->GetDrawNode()->matViewProj;
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

/* 4
	float fOffsetX = 0.5f + (0.5f / shadow_map_size);
	float fOffsetY = 0.5f + (0.5f / shadow_map_size);
/*/ //16
	float fOffsetX = 0.5f + (0.0f / shadow_map_size);
	float fOffsetY = 0.5f + (0.0f / shadow_map_size);
/**/
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

void VSGeforceFXTileMapScene::SetWorldSize(Vect2f sz)
{
    Vect4f v(1/sz.x,1/sz.y,0,0);
	SetVector(fInvWorldSize,&v);
}

void VSGeforceFXTileMapScene::GetHandle()
{
	VSGeforceFXScene::GetHandle();
	VAR_HANDLE(fInvWorldSize);
}

void VSGeforceFXTileMapScene::RestoreShader()
{
	#include "GeforceFX/o/tile_map_scene.vl"
}

void PSGeforceFXTileMapScene::Restore()
{
	switch(Option_ShadowHint)
	{
	case 0:
		{
		if(Option_ShadowMapSelf4x4)
		{
			#include "GeforceFX/o/tile_map_scene4x4.ph"
		}else
		{
			#include "GeforceFX/o/tile_map_scene2x2.ph"
		}
		break;
		}
	case 1:
		{
		#include "GeforceFX/o/tile_map_scene1.ph"
		break;
		}
	case 2:
		{
		#include "GeforceFX/o/tile_map_scene2.ph"
		break;
		}
	case 3:
		{
		#include "GeforceFX/o/tile_map_scene3.ph"
		break;
		}
	case 4:
		{
		#include "GeforceFX/o/tile_map_scene4.ph"
		break;
		}
	default:
		VISASSERT(0);
	}
	
}


void VSGeforceFXObjectSceneLight::RestoreShader()
{
	#include "GeforceFX/o/object_scene_light.vl"
	#include "GeforceFX/o/object_scene_light_p1.vl"
	#include "GeforceFX/o/object_scene_light_p2.vl"
}

void VSGeforceFXScene::GetHandle()
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

void VSGeforceFXScene::SetFog()
{
    Vect4f v = gb_RenderDevice3D->dtAdvance->GetFogParam();
    SetVector(vFog,&v);
}

void VSGeforceFXObjectSceneLight::GetHandle()
{
	VSGeforceFXScene::GetHandle();

	VAR_HANDLE(vAmbient);
	VAR_HANDLE(vDiffuse);
	VAR_HANDLE(vSpecular);
	VAR_HANDLE(fSpecularPower);
	VAR_HANDLE(vCameraPos);
	VAR_HANDLE(vLightDirection);
}

void VSGeforceFXObjectSceneLight::Select(const Mat4f* pmatlight,float shadow_map_size,
										const MatXf* world, std::vector<cUnkLight*>* light)
{
	if(light)
	{
		if(light->size()>0)
			SetLight(vPointPos0,vPointColor0,vPointParam0,(*light)[0]);

		if(light->size()>1)
			SetLight(vPointPos1,vPointColor1,vPointParam1,(*light)[1]);
	}
	VSGeforceFXScene::Select(pmatlight,shadow_map_size,world,light);
}

void VSGeforceFXObjectSceneLight::SetMaterial(sDataRenderMaterial *Data)
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

void PSGeforceFXObjectSceneLight::Restore()
{
	#include "GeforceFX/o/object_scene_light.ph"
}

void PSGeforceFXObjectSceneLight2::Restore()
{
	#include "GeforceFX/o/object_scene_light2.ph"
}

void VSGeforceFXObjectScene::RestoreShader()
{
	#include "GeforceFX/o/object_scene.vl"
}

void PSGeforceFXObjectScene::Restore()
{
	#include "GeforceFX/o/object_scene.ph"
}

void VSGeforceFXObjectSceneLightT::RestoreShader()
{
	#include "GeforceFX/o/object_scene_lightT.vl"
}

void VSGeforceFXObjectSceneLight2::RestoreShader()
{
	#include "GeforceFX/o/object_scene_light2.vl"
}

void VSGeforceFXObjectSceneLightT::SetTextureTransform(MatXf& m)
{
	Vect4f u(m.rot()[0][0],m.rot()[1][0],m.trans().x,0),
				v(m.rot()[0][1],m.rot()[1][1],m.trans().y,0);
	SetVector(vUtrans,&u);
	SetVector(vVtrans,&v);
}

void VSGeforceFXObjectSceneLightT::GetHandle()
{
	VSGeforceFXObjectSceneLight::GetHandle();
	VAR_HANDLE(vUtrans);
	VAR_HANDLE(vVtrans);
}


void VSGeforceFXObjectSceneBump::Select(const Mat4f* matlight,float shadow_map_size,const MatXf* world, std::vector<cUnkLight*>* light)
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

	VSGeforceFXScene::Select(matlight,shadow_map_size,world,light);
}

void VSGeforceFXObjectSceneBump::GetHandle()
{
	VSGeforceFXScene::GetHandle();
	VAR_HANDLE(mInvWorld);
	VAR_HANDLE(vCameraPos);
	VAR_HANDLE(vLightDirection);
	VAR_HANDLE(vLightDirectionInvWorld);
}

void VSGeforceFXObjectSceneBump::SetMaterial(sDataRenderMaterial *Data)
{
	Vect3f p=gb_RenderDevice3D->GetDrawNode()->GetPos();
    Vect4f cam(p.x,p.y,p.z,0);
    SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
    Vect4f light(l.x,l.y,l.z,0);
    SetVector(vLightDirection,&light);
}

void VSGeforceFXObjectSceneBump::RestoreShader()
{
#include "GeforceFX/o/object_scene_bump.vl"
#include "GeforceFX/o/object_scene_bump_p1.vl"
#include "GeforceFX/o/object_scene_bump_p2.vl"
}

void PSGeforceFXObjectSceneBump::Restore()
{
#include "GeforceFX/o/object_scene_bump.ph"
}

void VSGeforceFXObjectSceneBumpNoShadow::RestoreShader()
{
#include "GeforceFX/o/object_scene_bump_no_shadow.vl"
#include "GeforceFX/o/object_scene_bump_no_shadow_p1.vl"
#include "GeforceFX/o/object_scene_bump_no_shadow_p2.vl"
}

void PSGeforceFXObjectSceneBumpNoShadow::Restore()
{
#include "GeforceFX/o/object_scene_bump_no_shadow.ph"
}
