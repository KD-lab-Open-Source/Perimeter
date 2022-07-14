//////////////////////////////Radeon 9700///////////////////////
VS9700ObjectScene::VS9700ObjectScene()
{
}

VS9700ObjectScene::~VS9700ObjectScene()
{
}

void VS9700ObjectScene::RestoreShader()
{
#include "9700/o/object_scene.vl"
}

void VS9700ObjectScene::GetHandle()
{
	VS9700TileMapScene::GetHandle();
	VAR_HANDLE(vPointPos0);
	VAR_HANDLE(vPointColor0);
	VAR_HANDLE(vPointParam0);

	VAR_HANDLE(vPointPos1);
	VAR_HANDLE(vPointColor1);
	VAR_HANDLE(vPointParam1);
}

void VS9700ObjectSceneLight::RestoreShader()
{
#include "9700/o/object_scene_light.vl"
#include "9700/o/object_scene_light_p1.vl"
#include "9700/o/object_scene_light_p2.vl"
}

void VS9700ObjectSceneLight::GetHandle()
{
	VS9700ObjectScene::GetHandle();
	VAR_HANDLE(vAmbient);
	VAR_HANDLE(vDiffuse);
	VAR_HANDLE(vSpecular);
	VAR_HANDLE(fSpecularPower);
	VAR_HANDLE(vCameraPos);
	VAR_HANDLE(vLightDirection);
}

void VS9700ObjectSceneLight::Select(const Mat4f* matlight,float shadow_map_size,const MatXf* world, std::vector<cUnkLight*>* light)
{
	if(light)
	{
		if(light->size()>0)
			SetLight(vPointPos0,vPointColor0,vPointParam0,(*light)[0]);

		if(light->size()>1)
			SetLight(vPointPos1,vPointColor1,vPointParam1,(*light)[1]);
	}

	VS9700ObjectScene::Select(matlight,shadow_map_size,world,light);
}

void VS9700ObjectSceneLight::SetMaterial(sDataRenderMaterial *Data)
{
	SetVector(vAmbient, reinterpret_cast<const Vect4f*>(&Data->Ambient));
	SetVector(vDiffuse, reinterpret_cast<const Vect4f*>(&Data->Diffuse));
	SetVector(vSpecular, reinterpret_cast<const Vect4f*>(&Data->Specular));
	SetFloat(fSpecularPower,Data->Power);
	Vect3f p=gb_RenderDevice3D->GetDrawNode()->GetPos();
    Vect4f cam(p.x,p.y,p.z,0);
    SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
    Vect4f light(l.x,l.y,l.z,0);
    SetVector(vLightDirection,&light);
}

void VS9700ObjectSceneLight2::RestoreShader()
{
#include "9700/o/object_scene_light2.vl"
}

void VS9700ObjectSceneLight2::GetHandle()
{
	VS9700ObjectSceneLight::GetHandle();
	VAR_HANDLE(vUtrans);
	VAR_HANDLE(vVtrans);
}


void VS9700ObjectSceneLight2::SetTextureTransform(MatXf& m)
{
	Vect4f u(m.rot()[0][0],m.rot()[1][0],m.trans().x,0),
				v(m.rot()[0][1],m.rot()[1][1],m.trans().y,0);
	SetVector(vUtrans,&u);
	SetVector(vVtrans,&v);
}


void VS9700ObjectSceneLightT::RestoreShader()
{
#include "9700/o/object_scene_lightT.vl"
}

void VS9700ObjectSceneBump::GetHandle()
{
	VS9700ObjectScene::GetHandle();
	VAR_HANDLE(mInvWorld);
	VAR_HANDLE(vCameraPos);
	VAR_HANDLE(vLightDirection);
	VAR_HANDLE(vLightDirectionInvWorld);

}

void VS9700ObjectSceneBump::Select(const Mat4f* matlight,float shadow_map_size,const MatXf* world, std::vector<cUnkLight*>* light)
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

	VS9700ObjectScene::Select(matlight,shadow_map_size,world,light);
}

void VS9700ObjectSceneBump::SetMaterial(sDataRenderMaterial *Data)
{
	Vect3f p=gb_RenderDevice3D->GetDrawNode()->GetPos();
    Vect4f cam(p.x,p.y,p.z,0);
	SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
    Vect4f light(l.x,l.y,l.z,0);
    SetVector(vLightDirection,&light);
}


void VS9700ObjectSceneBump::RestoreShader()
{
	#include "9700/o/object_scene_bump.vl"
	#include "9700/o/object_scene_bump_p1.vl"
	#include "9700/o/object_scene_bump_p2.vl"
}

void VS9700ObjectSceneBumpNoShadow::RestoreShader()
{
	#include "9700/o/object_scene_bump_no_shadow.vl"
	#include "9700/o/object_scene_bump_no_shadow_p1.vl"
	#include "9700/o/object_scene_bump_no_shadow_p2.vl"
}

void VS9700TileMapShadow::Select(const MatXf& world)
{
	Mat4f mat;
    Mat4fSetTransposedMatXf(mat, world);
	mat *= gb_RenderDevice3D->GetDrawNode()->matViewProj;
	SetMatrix(mWVP,&mat);
	cVertexShader::Select();
}

void VS9700TileMapShadow::RestoreShader()
{
#include "9700/o/tile_map_shadow.vl"
}
void VS9700TileMapShadow::GetHandle()
{
	cVertexShader::GetHandle();
	VAR_HANDLE(mWVP);
}

void PS9700TileMapShadow::Restore()
{
#include "9700/o/tile_map_shadow.ph"
}

void VS9700TileMapScene::RestoreShader()
{
#include "9700/o/tile_map_scene.vl"
}

void VS9700TileMapScene::GetHandle()
{
	cVertexShader::GetHandle();
	VAR_HANDLE(mWVP);
	VAR_HANDLE(mWorldToLight);
	VAR_HANDLE(mShadow);
	VAR_HANDLE(mWorld);
	VAR_HANDLE(mView);

	VAR_HANDLE(fInvWorldSize);
}

void VS9700TileMapScene::SetWorldSize(Vect2f sz)
{
    Vect4f v(1/sz.x,1/sz.y,0,0);
    SetVector(fInvWorldSize,&v);
}

void VS9700TileMapScene::Select(const Mat4f* pmatlight,float shadow_map_size,const MatXf* world, std::vector<cUnkLight*>* light)
{
	//c0-c3 - матрица преобразования в экранные координаты (view_proj_matrix)
	Mat4f mat;
	if(world)
	{
        Mat4fSetTransposedMatXf(mat, *world);
		SetMatrix(mWorld,&mat);//for light
		SetMatrix(mView, &gb_RenderDevice3D->GetDrawNode()->matView);

        Mat4fSetTransposedMatXf(mat, *world);
		mat *= gb_RenderDevice3D->GetDrawNode()->matViewProj;
	}else
	{
		mat = gb_RenderDevice3D->GetDrawNode()->matViewProj;
	}

	SetMatrix(mWVP,&mat);

	Mat4f matlight;
	if(world)
	{
        Mat4fSetTransposedMatXf(mat, *world);
		matlight = mat * *pmatlight;
	} else {
        matlight = *pmatlight;
    }

	SetMatrix(mWorldToLight,&matlight);

	float fTexelOffs = (0.5f / shadow_map_size);
	float fix= 0;//(1.0f / shadow_map_size);
	Mat4f matTexAdj(0.5f,      0.0f,        0.0f,        0.0f,
                    0.0f,     -0.5f,        0.0f,        0.0f,
                    0.0f,      0.0f,        0.0f,        0.0f,
					0.5f + fTexelOffs,0.5f + fTexelOffs+fix,1.0f, 1.0f);

	mat = matlight * matTexAdj;

	SetMatrix(mShadow,&mat);

	if(light)
	{
		cVertexShader::Select((std::min)(light->size(),pShader.size()-1));
	}else
		cVertexShader::Select();
}

void PS9700TileMapScene::Restore()
{
#include "9700/o/tile_map_scene_debug.ph"
}

void PS9700TileMapScene4x4::Restore()
{
if(Option_ShadowMapSelf4x4)
{
	#include "9700/o/tile_map_scene4x4.ph"
}else
{
	#include "9700/o/tile_map_scene2x2.ph"
}
}

void VS9700ObjectShadow::RestoreShader()
{
#include "9700/o/object_shadow.vl"
}

void PS9700AlphaTestShadow::Restore()
{
#include "9700/o/alphatest_shadow.ph"
}

void PS9700ObjectShadow::Restore()
{
#include "9700/o/object_shadow.ph"
}

void PS9700ObjectSceneLight::Restore()
{
if(Option_ShadowMapSelf4x4)
{
	#include "9700/o/object_scene_light4x4.ph"
}else
{
	#include "9700/o/object_scene_light2x2.ph"
}

}

void PS9700ObjectSceneLight2::Restore()
{
if(Option_ShadowMapSelf4x4)
{
	#include "9700/o/object_scene_light24x4.ph"
}else
{
	#include "9700/o/object_scene_light22x2.ph"
}
}

void PS9700ObjectSceneBump::Restore()
{
if(Option_ShadowMapSelf4x4)
{
	#include "9700/o/object_scene_bump4x4.ph"
}else
{
	#include "9700/o/object_scene_bump2x2.ph"
}
}

void PS9700ObjectSceneBumpNoShadow::Restore()
{
#include "9700/o/object_scene_bump_no_shadow.ph"
}
