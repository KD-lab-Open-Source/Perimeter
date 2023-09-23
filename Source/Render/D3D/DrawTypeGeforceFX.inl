//////////////////////////////DrawTypeGeforceFX/////////////////////
int DrawTypeGeforceFX::GetOffsetTextureNumber(bool tilemap)
{
	return tilemap?4:1;
}

DrawTypeGeforceFX::DrawTypeGeforceFX()
{
	ptZBuffer=NULL;
	pShadow=NULL;
	use_shadow=false;

	pVSTileMapScene=new VSGeforceFXTileMapScene;
	pVSTileMapScene->Restore();
	pPSTileMapScene=new PSGeforceFXTileMapScene;
	pPSTileMapScene->Restore();

	pVSObjectScene=new VSGeforceFXObjectScene;
	pVSObjectScene->Restore();
	pVSObjectSceneLight=new VSGeforceFXObjectSceneLight;
	pVSObjectSceneLight->Restore();

	pPSObjectSceneNoLight=new PSGeforceFXObjectScene;
	pPSObjectSceneNoLight->Restore();

	pPSObjectScene=new PSGeforceFXObjectSceneLight;
	pPSObjectScene->Restore();

	pVSObjectSceneLightT=new VSGeforceFXObjectSceneLightT;
	pVSObjectSceneLightT->Restore();

	pVSObjectSceneLight2=new VSGeforceFXObjectSceneLight2;
	pVSObjectSceneLight2->Restore();
	pPSObjectScene2=new PSGeforceFXObjectSceneLight2;
	pPSObjectScene2->Restore();

	pPSObjectSceneBump=new PSGeforceFXObjectSceneBump;
	pPSObjectSceneBump->Restore();
	pVSObjectSceneBump=new VSGeforceFXObjectSceneBump;
	pVSObjectSceneBump->Restore();

	pPSObjectSceneBumpNoShadow=new PSGeforceFXObjectSceneBumpNoShadow;
	pPSObjectSceneBumpNoShadow->Restore();
	pVSObjectSceneBumpNoShadow=new VSGeforceFXObjectSceneBumpNoShadow;
	pVSObjectSceneBumpNoShadow->Restore();

	last_vs=NULL;
	last_ps=NULL;

	fog_param.x=0;
	fog_param.y=1;
	fog_param.z=0;
	fog_param.w=0;

}

DrawTypeGeforceFX::~DrawTypeGeforceFX()
{
	DEL(pVSObjectScene);
	DEL(pPSObjectScene);
	DEL(pVSObjectSceneLight);
	DEL(pVSObjectSceneLightT);

	DEL(pVSTileMapScene);
	DEL(pPSTileMapScene);

	DEL(pVSObjectSceneLight2);
	DEL(pPSObjectScene2);
	DEL(pPSObjectSceneNoLight);

	DEL(pPSObjectSceneBump);
	DEL(pVSObjectSceneBump);
	DEL(pPSObjectSceneBumpNoShadow);
	DEL(pVSObjectSceneBumpNoShadow);
}

void DrawTypeGeforceFX::BeginDraw(bool use_shadow_)
{
	for(int i=0;i<6;i++)
	{
		gb_RenderDevice3D->SetTextureImage(i, nullptr);
	}
	use_shadow=use_shadow_;
	pShadow=gb_RenderDevice3D->GetDrawNode()->FindCildCamera(ATTRCAMERA_SHADOWMAP);

	min_filter=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MINFILTER);
	mag_filter=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MAGFILTER);
	mip_filter=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MIPFILTER);

	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MINFILTER, gb_RenderDevice3D->texture_interpolation);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MAGFILTER, gb_RenderDevice3D->texture_interpolation);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MIPFILTER, gb_RenderDevice3D->texture_interpolation);
	gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	if(pPSObjectScene && use_shadow)
	{
		pPSObjectScene->Select();
		pVSObjectScene->Select(&pShadow->matViewProj,pShadowMap->GetWidth(),&MatXf::ID,NULL);
	}
}

void DrawTypeGeforceFX::EndDraw()
{
	for(int i=0;i<6;i++)
	{
		gb_RenderDevice3D->SetTextureImage(i, nullptr);
	}

	gb_RenderDevice3D->SetVertexShader(NULL);
	gb_RenderDevice3D->SetPixelShader(NULL);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MINFILTER, min_filter);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MAGFILTER, mag_filter);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MIPFILTER, mip_filter);
	
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

void DrawTypeGeforceFX::BeginDrawShadow()
{
	pShadow=gb_RenderDevice3D->GetDrawNode()->FindCildCamera(ATTRCAMERA_SHADOWMAP);

	min_filter=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MINFILTER);
	mag_filter=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MAGFILTER);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	gb_RenderDevice3D->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
}

void DrawTypeGeforceFX::EndDrawShadow()
{
	gb_RenderDevice3D->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE |
		                                              D3DCOLORWRITEENABLE_GREEN |
													  D3DCOLORWRITEENABLE_RED);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_MINFILTER, min_filter);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_MAGFILTER, mag_filter);
}

void DrawTypeGeforceFX::SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
{
	if(!use_shadow && !IsBump(Data))
	{
		gb_RenderDevice3D->SetVertexShader(NULL);
		gb_RenderDevice3D->SetPixelShader(NULL);
		gb_RenderDevice3D->dtFixed->SetSimplyMaterial(Mesh,Data);
		return;
	}

	SetMaterial(Data->MaterialAnimPhase,Data->Tex[0],Data->Tex[1],Data);
	if(use_shadow) {
        TextureImage teximg(ptZBuffer);
        gb_RenderDevice3D->SetTextureImage(0, &teximg);
    } else {
        gb_RenderDevice3D->SetTextureImage(0, nullptr);
    }
	
	SetStream(Mesh);
}

void DrawTypeGeforceFX::DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
{
	bool is_bump=IsBump(Data);
	if(!use_shadow && !is_bump)
	{
		gb_RenderDevice3D->dtFixed->DrawNoMaterial(Mesh,Data);
		return;
	}

	std::vector<cUnkLight*>* point=&Mesh->GetRootNode()->GetLight();
	last_ps->Select();
	if(pShadowMap && pShadow)
		last_vs->Select(&pShadow->matViewProj,pShadowMap->GetWidth(),&Mesh->GetGlobalMatrix(),point);
	else
		last_vs->Select(&gb_RenderDevice3D->GetDrawNode()->matViewProj,1,&Mesh->GetGlobalMatrix(),point);
		

	DrawPrimitive(Mesh);
}

void DrawTypeGeforceFX::SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture)
{
	sDataRenderMaterial mat;
	float f=0.5f*gb_RenderDevice3D->kShadow;
	mat.Ambient=sColor4f(0,0,0,1);
	mat.Diffuse=sColor4f(f,f,f,1);
	mat.Specular=sColor4f(0,0,0,1);
	mat.Emissive=sColor4f(0,0,0,1);
	mat.Power=0;
	mat.mat=0;
	mat.MaterialAnimPhase=0;

	DrawTypeFixedPipeline::SetMaterial(0,Texture,NULL,&mat);
	gb_RenderDevice3D->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	SetStream(Mesh);
}

void DrawTypeGeforceFX::DrawNoMaterialShadow(cObjMesh *Mesh)
{
	gb_RenderDevice3D->SetWorldMatXf(Mesh->GetGlobalMatrix());
	DrawPrimitive(Mesh);
}

bool DrawTypeGeforceFX::CreateShadowTexture(int xysize)
{
	DeleteShadowTexture();

	bool is_truecolor=gb_RenderDevice->GetRenderMode()&RENDERDEVICE_MODE_RGB32;
	pShadowMap=GetTexLibrary()->CreateRenderTexture(xysize,xysize,TEXTURE_RENDER16,false);
	if(!pShadowMap)
	{
		DeleteShadowTexture();
		return false;
	}

	HRESULT hr=gb_RenderDevice3D->lpD3DDevice->CreateTexture(xysize, xysize, 1, D3DUSAGE_DEPTHSTENCIL, 
		D3DFMT_D16, 
		D3DPOOL_DEFAULT, &ptZBuffer,0);
	if(FAILED(hr))
	{
		DeleteShadowTexture();
		return false;
	}

	RDCALL(ptZBuffer->GetSurfaceLevel(0,&pZBuffer));

	pLightMap=GetTexLibrary()->CreateRenderTexture(256,256,TEXTURE_RENDER32,false);
	if(!pLightMap)
	{
		DeleteShadowTexture();
		return false;
	}
	return true;
}

void DrawTypeGeforceFX::SetMaterial(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data)
{
	eBlendMode blend=DrawType::SetMaterialSimply(Phase,Texture0,Texture1,Data,GetOffsetTextureNumber());

	if(Data->mat&MAT_BUMP)
	{
		Vect4f specular(Data->Specular.r,Data->Specular.g,Data->Specular.b,Data->Power);
		gb_RenderDevice3D->SetPixelShaderConstant(4, reinterpret_cast<const Vect4f*>(&Data->Ambient));
		gb_RenderDevice3D->SetPixelShaderConstant(5, reinterpret_cast<const Vect4f*>(&Data->Diffuse));
		gb_RenderDevice3D->SetPixelShaderConstant(6,&specular);
	}

	bool is_bump=IsBump(Data);
	if(is_bump)
	{
		if(use_shadow)
		{
			last_ps=pPSObjectSceneBump;
			last_vs=pVSObjectSceneBump;
		}else
		{
			last_ps=pPSObjectSceneBumpNoShadow;
			last_vs=pVSObjectSceneBumpNoShadow;
		}
		last_vs->SetTextureTransform(Data->TexMatrix);
	}else
	if(Data->mat&MAT_TEXNORMAL_STAGE2)
	{
		gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_MINFILTER, gb_RenderDevice3D->texture_interpolation);
		gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_MIPFILTER, gb_RenderDevice3D->texture_interpolation);
		gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_MAGFILTER, gb_RenderDevice3D->texture_interpolation);
		
		last_vs=pVSObjectSceneLight2;
		last_ps=pPSObjectScene2;
		last_vs->SetTextureTransform(Data->TexMatrix);
	}else
	{
		bool light=Data->mat&MAT_LIGHT;
		last_vs=(!light)?pVSObjectScene:pVSObjectSceneLight;
		last_ps=(!light)?pPSObjectSceneNoLight:pPSObjectScene;

		if(Data->mat&MAT_TEXMATRIX_STAGE1)
		{
			last_vs=pVSObjectSceneLightT;
			last_vs->SetTextureTransform(Data->TexMatrix);
		}
	}

	last_vs->SetMaterial(Data);
}

void DrawTypeGeforceFX::SetMaterialTilemap(cTileMap *TileMap)
{
	pVSTileMapScene->Select(&pShadow->matViewProj,pShadowMap->GetWidth(),NULL,NULL);
	pPSTileMapScene->Select();
//	gb_RenderDevice3D->SetTexture(0,ptZBuffer);
//	gb_RenderDevice3D->SetTexture(pLightMap,0,2);

	int offset=GetOffsetTextureNumber(true);

	for(int ss=0;ss<offset;ss++)
	{
		gb_RenderDevice3D->SetSamplerState(ss, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		gb_RenderDevice3D->SetSamplerState(ss, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		gb_RenderDevice3D->SetSamplerState(ss, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        TextureImage teximg(ptZBuffer);
        gb_RenderDevice3D->SetTextureImage(0,&teximg);
	}

	gb_RenderDevice3D->SetTexture(offset+1,pLightMap,0);

	TerraInterface* terra=TileMap->GetTerra();
	pVSTileMapScene->SetWorldSize(Vect2f(terra->SizeX(),terra->SizeY()));
}

void DrawTypeGeforceFX::SetMaterialTilemapShadow()
{
	gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
}

void DrawTypeGeforceFX::DeleteShadowTexture()
{
	DrawTypeFixedPipeline::DeleteShadowTexture();
	RELEASE(ptZBuffer);
}

void DrawTypeGeforceFX::SetTileColor(sColor4f c)
{
	Vect4f cf(c.r,c.g,c.b,c.a);
	gb_RenderDevice3D->SetPixelShaderConstant(2,&cf);
}


void DrawTypeGeforceFX::SetFog(const sColor4f &color,const Vect2f &v)
{
	fog_param.x=v.x;
	fog_param.y=v.y/(v.y-v.x);
	fog_param.z=-1/(v.y-v.x);
}

