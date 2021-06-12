//////////////////////////////DrawTypeRadeon9700/////////////////////
DrawTypeRadeon9700::DrawTypeRadeon9700()
{
	pShadow=NULL;
	pShadowMap=NULL;

	pShowMap=new PSShowMap;
	pShowMap->Restore();
	//pVSObjectShadow=new VS9700TileMapShadow;
	pVSObjectShadow=new VS9700ObjectShadow;
	pVSObjectShadow->Restore();
	//pPSObjectShadow=new PSOrtoTileMapShadow;
	pPSObjectShadow=new PS9700ObjectShadow;
	pPSObjectShadow->Restore();
	pPSObjectShadowAlphaTest=new PS9700AlphaTestShadow;
	pPSObjectShadowAlphaTest->Restore();

	pVSObjectScene=new VS9700ObjectScene;
	pVSObjectScene->Restore();
	pVSObjectSceneLight=new VS9700ObjectSceneLight;
	pVSObjectSceneLight->Restore();

	pPSObjectScene=new PS9700ObjectSceneLight;
						//PS9700TileMapScene9700_4x4;
						//PS9700TileMapScene;//
	pPSObjectScene->Restore();

	pVSObjectSceneT=new VS9700ObjectSceneLightT;
	pVSObjectSceneT->Restore();



	pVSTileMapShadow=new VS9700TileMapShadow;
	pVSTileMapShadow->Restore();
	pPSTileMapShadow=new PS9700TileMapShadow;
	pPSTileMapShadow->Restore();

	pVSTileMapScene=new VS9700TileMapScene;
	pVSTileMapScene->Restore();
	//pPSTileMapScene=new PSOrtoTileMapScene;
	pPSTileMapScene=new PS9700TileMapScene4x4;
	pPSTileMapScene->Restore();

	pVSObjectSceneLight2=new VS9700ObjectSceneLight2;
	pVSObjectSceneLight2->Restore();
	pPSObjectScene2=new PS9700ObjectSceneLight2;
	pPSObjectScene2->Restore();

	pPSTileMapSceneInv=NULL;

	pVSObjectSceneBump=new VS9700ObjectSceneBump;
	pVSObjectSceneBump->Restore();
	pPSObjectSceneBump=new PS9700ObjectSceneBump;
	pPSObjectSceneBump->Restore();

	pVSObjectSceneBumpNoShadow=new VS9700ObjectSceneBumpNoShadow;
	pVSObjectSceneBumpNoShadow->Restore();
	pPSObjectSceneBumpNoShadow=new PS9700ObjectSceneBumpNoShadow;
	pPSObjectSceneBumpNoShadow->Restore();

	last_vs=0;
	last_ps=0;
	use_shadow=false;
}

DrawTypeRadeon9700::~DrawTypeRadeon9700()
{
	DEL(pShowMap);
	DEL(pVSObjectShadow);
	DEL(pPSObjectShadow);
	DEL(pVSObjectScene);
	DEL(pPSObjectScene);
	DEL(pPSObjectShadowAlphaTest);
	DEL(pVSObjectSceneLight);
	DEL(pVSObjectSceneT);

	DEL(pVSTileMapShadow);
	DEL(pPSTileMapShadow);
	DEL(pVSTileMapScene);
	DEL(pPSTileMapScene);
	DEL(pPSTileMapSceneInv);

	DEL(pVSObjectSceneLight2);
	DEL(pPSObjectScene2);

	DEL(pPSObjectSceneBump);
	DEL(pVSObjectSceneBump);
	DEL(pPSObjectSceneBumpNoShadow);
	DEL(pVSObjectSceneBumpNoShadow);
}

void DrawTypeRadeon9700::BeginDraw(bool use_shadow_)
{
	use_shadow=use_shadow_;
	pShadow=gb_RenderDevice3D->GetDrawNode()->FindCildCamera(ATTRCAMERA_SHADOWMAP);

	min_filter=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MINFILTER);
	mag_filter=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MAGFILTER);

	gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	pPSObjectScene->Select();
	if(use_shadow)
	{
		VISASSERT(pShadowMap==pShadow->GetRenderTarget());
//		pVSObjectScene->Select(pShadow->matViewProj,pShadowMap->GetWidth(),&MatXf::ID);
	}
}

void DrawTypeRadeon9700::EndDraw()
{
	gb_RenderDevice3D->SetVertexShader(NULL);
	gb_RenderDevice3D->SetPixelShader(NULL);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MINFILTER, min_filter);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MAGFILTER, mag_filter);

	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

void DrawTypeRadeon9700::SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
{
	if(!use_shadow && !IsBump(Data))
	{
		gb_RenderDevice3D->SetVertexShader(NULL);
		gb_RenderDevice3D->SetPixelShader(NULL);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MINFILTER, gb_RenderDevice3D->texture_interpolation);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MAGFILTER, gb_RenderDevice3D->texture_interpolation);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MIPFILTER, gb_RenderDevice3D->texture_interpolation);
		gb_RenderDevice3D->dtFixed->SetSimplyMaterial(Mesh,Data);
		return;
	}

	if(use_shadow)
	{
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	}

	SetMaterial(Data->MaterialAnimPhase,Data->Tex[0],pShadowMap,Data);
	gb_RenderDevice3D->SetTexture(Data->Tex[1],Data->MaterialAnimPhase,3);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	gb_RenderDevice3D->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	SetStream(Mesh);
}

void DrawTypeRadeon9700::DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
{
	bool is_bump=IsBump(Data);
	if(!use_shadow && !is_bump)
	{
		gb_RenderDevice3D->dtFixed->DrawNoMaterial(Mesh,Data);
		return;
	}

	vector<cUnkLight*>* point=&Mesh->GetRootNode()->GetLight();
	last_ps->Select();
	if(pShadowMap && pShadow)
		last_vs->Select(pShadow->matViewProj,pShadowMap->GetWidth(),&Mesh->GetGlobalMatrix(),point);
	else
		last_vs->Select(gb_RenderDevice3D->GetDrawNode()->matViewProj,1,&Mesh->GetGlobalMatrix(),point);
		

	DrawPrimitive(Mesh);
}

void DrawTypeRadeon9700::SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture)
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

	SetMaterialSimply(0,Texture,NULL,&mat,GetOffsetTextureNumber());
	if(Texture)
		pPSObjectShadowAlphaTest->Select();
	else
		pPSObjectShadow->Select();
	SetStream(Mesh);
}

void DrawTypeRadeon9700::DrawNoMaterialShadow(cObjMesh *Mesh)
{
	pVSObjectShadow->Select(Mesh->GetGlobalMatrix());
	DrawPrimitive(Mesh);
}

bool DrawTypeRadeon9700::CreateShadowTexture(int xysize)
{
	DeleteShadowTexture();
	pShadowMap=GetTexLibrary()->CreateRenderTexture(xysize,xysize,TEXTURE_RENDER_SHADOW_9700,false);
	if(!pShadowMap)
	{
		DeleteShadowTexture();
		return false;
	}

	HRESULT hr=gb_RenderDevice3D->lpD3DDevice->CreateDepthStencilSurface(xysize, xysize, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &pZBuffer, NULL);
	if(FAILED(hr))
	{
		DeleteShadowTexture();
		return false;
	}


	pLightMap=GetTexLibrary()->CreateRenderTexture(256,256,TEXTURE_RENDER32,false);
	if(!pLightMap)
	{
		DeleteShadowTexture();
		return false;
	}

	return true;
}

void DrawTypeRadeon9700::SetMaterial(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data)
{
	eBlendMode blend=DrawType::SetMaterialSimply(Phase,Texture0,Texture1,Data);

	if(Data->mat&MAT_BUMP)
	{
		gb_RenderDevice3D->SetPixelShaderConstant(20,(D3DXVECTOR4*)&Data->Ambient);
		gb_RenderDevice3D->SetPixelShaderConstant(21,(D3DXVECTOR4*)&Data->Diffuse);
		gb_RenderDevice3D->SetPixelShaderConstant(22,(D3DXVECTOR4*)&Data->Specular);
        D3DXVECTOR4 power = D3DXVECTOR4(0,0,0,Data->Power);
		gb_RenderDevice3D->SetPixelShaderConstant(23,&power);
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
	}else
	if(Data->mat&MAT_TEXNORMAL_STAGE2)
	{
		gb_RenderDevice3D->SetSamplerState(3, D3DSAMP_MINFILTER, gb_RenderDevice3D->texture_interpolation);
		gb_RenderDevice3D->SetSamplerState(3, D3DSAMP_MIPFILTER, gb_RenderDevice3D->texture_interpolation);
		gb_RenderDevice3D->SetSamplerState(3, D3DSAMP_MAGFILTER, gb_RenderDevice3D->texture_interpolation);
		
		last_vs=pVSObjectSceneLight2;
		last_ps=pPSObjectScene2;
		last_vs->SetTextureTransform(Data->TexMatrix);
	}else
	{
		bool light=Data->mat&MAT_LIGHT;
		last_vs=(!light)?pVSObjectScene:pVSObjectSceneLight;
		last_ps=pPSObjectScene;

		if(Data->mat&MAT_TEXMATRIX_STAGE1)
		{
			last_vs=pVSObjectSceneT;
			last_vs->SetTextureTransform(Data->TexMatrix);
		}
	}

	last_vs->SetMaterial(Data);
}

void DrawTypeRadeon9700::SetMaterialTilemap(cTileMap *TileMap)
{
	cTexture* pShadowMap=pShadow->GetRenderTarget();
	pVSTileMapScene->Select(pShadow->matViewProj,pShadowMap->GetWidth(),NULL,NULL);

	if(pShadow->GetAttribute(ATTRCAMERA_ZINVERT))
		pPSTileMapSceneInv->Select();
	else
		pPSTileMapScene->Select();

	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	gb_RenderDevice3D->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	gb_RenderDevice3D->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	gb_RenderDevice3D->SetTexture(pShadowMap,0,1);
	gb_RenderDevice3D->SetTexture(pLightMap,0,3);

	TerraInterface* terra=TileMap->GetTerra();
	pVSTileMapScene->SetWorldSize(Vect2f(terra->SizeX(),terra->SizeY()));
}

void DrawTypeRadeon9700::SetMaterialTilemapShadow()
{
	pVSTileMapShadow->Select();
	pPSTileMapShadow->Select();
}

void DrawTypeRadeon9700::SetTileColor(sColor4f c)
{
	D3DXVECTOR4 cf(c.r,c.g,c.b,c.a);
	gb_RenderDevice3D->SetPixelShaderConstant(8,&cf);
}
