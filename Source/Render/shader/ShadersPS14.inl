////////////////////////ps.1.4//////////

void VS14Scene::Select(const Mat4f* pmatlight,float shadow_map_size,const MatXf* world, std::vector<cUnkLight*>* light)
{
	//c0-c3 - матрица преобразования в экранные координаты (view_proj_matrix)
	Mat4f mat;
	if(world)
	{
        Mat4fSetTransposedMatXf(mat, *world);
		SetMatrix(mWorld,&mat);//for light
		SetMatrix(mView,&gb_RenderDevice3D->GetDrawNode()->matView);

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
	}else
		matlight=*pmatlight;

	SetMatrix(mWorldToLight,&matlight);

	Mat4f matTexAdj(	0.5,      0,        0,        0,
							0,        -0.5,     0,        0,
							0,        0,        0,        0,
							0,		  0,		1,        1);

	const float c1=0.1f,c2=0.9f,c3=0.5f;
//	const c1=0.1-0.4f,c2=0.9f+0.4f,c3=0.5f+0.4f;

	if(gb_RenderDevice3D->GetTextureD3D(1) == gb_RenderDevice3D->GetTextureD3D(2))
	{
		//Четыре сэмпла выбираются
		float fC1 = c1 / shadow_map_size;
		float fC2 = c2 / shadow_map_size;

		matTexAdj.wy = 0.5f + fC1;
		matTexAdj.wz = 0.5f + fC1;
        mat = matlight * matTexAdj;
		SetMatrix(mShadow0,&mat);

		matTexAdj.wy = 0.5f + fC1;
		matTexAdj.wz = 0.5f + fC2;
        mat = matlight * matTexAdj;
		SetMatrix(mShadow1,&mat);

		matTexAdj.wy = 0.5f + fC2;
		matTexAdj.wz = 0.5f + fC1;
        mat = matlight * matTexAdj;
		SetMatrix(mShadow2,&mat);

		matTexAdj.wy = 0.5f + fC2;
		matTexAdj.wz = 0.5f + fC2;
        mat = matlight * matTexAdj;
		SetMatrix(mShadow3,&mat);
	}else
	{
		//Три сэмпла выбираются
		//x=0.5+0.4*xm::sin(2*PI/3*i),y=0.5+0.4*xm::cos(2*PI/3*i)

		matTexAdj.wy = 0.5f + c1/shadow_map_size;
		matTexAdj.wz = 0.5f + c1/shadow_map_size;
        mat = matlight * matTexAdj;
		SetMatrix(mShadow0,&mat);

		matTexAdj.wy = 0.5f + c2/shadow_map_size;
		matTexAdj.wz = 0.5f + c1/shadow_map_size;
        mat = matlight * matTexAdj;
		SetMatrix(mShadow1,&mat);

		matTexAdj.wy = 0.5f + c3/shadow_map_size;
		matTexAdj.wz = 0.5f + c2/shadow_map_size;
        mat = matlight * matTexAdj;
		SetMatrix(mShadow2,&mat);
	}
	
	Vect4f	zbias = Vect4f( -0.005f, 0.0f, 0.0f, 0.0f );
	SetVector(vZBias,&zbias);

	if(light)
	{
		cVertexShader::Select((std::min)(light->size(),pShader.size()-1));
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

void VS14ObjectSceneLight::Select(const Mat4f* pmatlight,float shadow_map_size,const MatXf* world, std::vector<cUnkLight*>* light)
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
    Vect4f v(1/sz.x,1/sz.y,0,0);
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
	SetVector(vDiffuse, reinterpret_cast<const Vect4f*>(&Data->Diffuse));
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
	Vect4f u(m.rot()[0][0],m.rot()[1][0],m.trans().x,0),
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
    Vect4f cam(p.x,p.y,p.z,0);
    SetVector(vCameraPos,&cam);
	Vect3f l;
	gb_RenderDevice3D->GetDrawNode()->GetLighting(l);
    Vect4f light(l.x,l.y,l.z,0);
    SetVector(vLightDirection,&light);

    Vect4f c(0.5,0.5,0.5,0);
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

void VS14ObjectSceneBump::Select(const Mat4f* matlight,float shadow_map_size,const MatXf* world, std::vector<cUnkLight*>* light)
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
