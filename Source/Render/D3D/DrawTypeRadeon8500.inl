///////////////////////DrawTypeRadeon8500//////////////////////
DrawTypeRadeon8500::DrawTypeRadeon8500()
{
	use_shadow=false;

	const int dx=2048;
	pLookupMap=GetTexLibrary()->CreateTexture(dx,1,false);
	int Pitch;
	uint32_t* pBuf=(uint32_t*)pLookupMap->LockTexture(Pitch);
	for(int i = 0; i < dx; i++ )
	{
		*pBuf++ = D3DCOLOR_RGBA( i & 0xFF, (i & 0xFF00) >> 3, 0, 0 );
	}
	pLookupMap->UnlockTexture();

	pVSTileMapShadow=new VS14TileMapShadow;
	pVSTileMapShadow->Restore();
	pPSTileMapShadow=new PS14TileMapShadow;
	pPSTileMapShadow->Restore();

	pVSTileMapScene=new VS14TileMapScene;
	pVSTileMapScene->Restore();
	pPSTileMapScene=new PS14TileMapScene;
	pPSTileMapScene->Restore();

	pPSObjectShadow=new PS14ObjectShadow;
	pPSObjectShadow->Restore();
	pVSObjectShadow=new VS14ObjectShadow;
	pVSObjectShadow->Restore();

	pPSObjectScene=new PS14ObjectSceneLight;
	pPSObjectScene->Restore();
	pVSObjectSceneLight=new VS14ObjectSceneLight;
	pVSObjectSceneLight->Restore();

	pPSObjectSceneNoLight=new PS14ObjectScene;
	pPSObjectSceneNoLight->Restore();
	pVSObjectScene=new VS14ObjectScene;
	pVSObjectScene->Restore();

	pPSObjectScene2=new PS14ObjectSceneLight2;
	pPSObjectScene2->Restore();
	pVSObjectSceneLight2=new VS14ObjectSceneLight2;
	pVSObjectSceneLight2->Restore();

	pPSObjectSceneBump=new PS14ObjectSceneBump;
	pPSObjectSceneBump->Restore();
	pVSObjectSceneBump=new VS14ObjectSceneBump;
	pVSObjectSceneBump->Restore();

	pPSObjectSceneBumpNoShadow=new PS14ObjectSceneBumpNoShadow;
	pPSObjectSceneBumpNoShadow->Restore();
	pVSObjectSceneBumpNoShadow=new VS14ObjectSceneBumpNoShadow;
	pVSObjectSceneBumpNoShadow->Restore();

	last_vs=NULL;
	last_ps=NULL;
}

DrawTypeRadeon8500::~DrawTypeRadeon8500()
{
	RELEASE(pLookupMap);
	DEL(pVSTileMapShadow);
	DEL(pPSTileMapShadow);
	DEL(pVSTileMapScene);
	DEL(pPSTileMapScene);

	DEL(pPSObjectShadow);
	DEL(pVSObjectShadow);

	DEL(pPSObjectScene);
	DEL(pVSObjectSceneLight);

	DEL(pPSObjectSceneNoLight);
	DEL(pVSObjectScene);

	DEL(pPSObjectScene2);
	DEL(pVSObjectSceneLight2);

	DEL(pPSObjectSceneBump);
	DEL(pVSObjectSceneBump);
	DEL(pPSObjectSceneBumpNoShadow);
	DEL(pVSObjectSceneBumpNoShadow);
}

void DrawTypeRadeon8500::BeginDraw(bool use_shadow_)
{
	use_shadow=use_shadow_;
	pShadow=gb_RenderDevice3D->GetDrawNode()->FindCildCamera(ATTRCAMERA_SHADOWMAP);

	min_filter=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MINFILTER);
	mag_filter=gb_RenderDevice3D->GetSamplerState(1,D3DSAMP_MAGFILTER);
}

void DrawTypeRadeon8500::EndDraw()
{
	gb_RenderDevice3D->SetTexture(NULL,0,2);
	gb_RenderDevice3D->SetTexture(NULL,0,3);
	gb_RenderDevice3D->SetTexture(NULL,0,4);
	gb_RenderDevice3D->SetTexture(NULL,0,5);
	gb_RenderDevice3D->SetTexture(NULL,0,6);

	gb_RenderDevice3D->SetVertexShader(NULL);
	gb_RenderDevice3D->SetPixelShader(NULL);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MINFILTER, min_filter);
	gb_RenderDevice3D->SetSamplerState(1, D3DSAMP_MAGFILTER, mag_filter);
}

void DrawTypeRadeon8500::BeginDrawShadow()
{
	dither_enable=gb_RenderDevice3D->GetRenderState(D3DRS_DITHERENABLE);
	gb_RenderDevice3D->SetRenderState(D3DRS_DITHERENABLE,FALSE);
	BeginDraw(true);
}

void DrawTypeRadeon8500::EndDrawShadow()
{
	EndDraw();
	gb_RenderDevice3D->SetRenderState(D3DRS_DITHERENABLE,dither_enable);
}

void DrawTypeRadeon8500::SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
{
	if(!use_shadow && !IsBump(Data))
	{
		gb_RenderDevice3D->SetVertexShader(NULL);
		gb_RenderDevice3D->SetPixelShader(NULL);
		gb_RenderDevice3D->dtFixed->SetSimplyMaterial(Mesh,Data);
		return;
	}

	cTexture* pTex1=Data->Tex[1];
	if(!pTex1 || ((Data->mat&MAT_BUMP) && !Option_EnableBump))
	{
		pTex1=pShadowMap;
		gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	}else
	{
		gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	}

	SetMaterial(Data->MaterialAnimPhase,Data->Tex[0],pTex1,Data);
	SetStream(Mesh);

    gb_RenderDevice3D->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 4, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 4, D3DSAMP_MAGFILTER, D3DTEXF_POINT );

	gb_RenderDevice3D->SetTexture(2,pShadowMap,0);
	gb_RenderDevice3D->SetTexture(3,pShadowMap,0);
	gb_RenderDevice3D->SetTexture(4,pShadowMap,0);
	gb_RenderDevice3D->SetTexture(5,pLightMap,0);
}

void DrawTypeRadeon8500::DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
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

void DrawTypeRadeon8500::SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture)
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

	SetMaterialSimply(0,Texture,pShadowMap,&mat,GetOffsetTextureNumber());
    TextureImage teximg(GetTilemapShadow0());
	gb_RenderDevice3D->SetTextureImage(1, &teximg);

	pPSObjectShadow->Select();
	SetStream(Mesh);
}

void DrawTypeRadeon8500::DrawNoMaterialShadow(cObjMesh *Mesh)
{
	pVSObjectShadow->Select(Mesh->GetGlobalMatrix());
	DrawPrimitive(Mesh);
}

bool DrawTypeRadeon8500::CreateShadowTexture(int xysize)
{
	pShadowMap=GetTexLibrary()->CreateRenderTexture(xysize,xysize,TEXTURE_RENDER32,false);
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

void DrawTypeRadeon8500::SetMaterialTilemap(cTileMap *TileMap)
{
	cTexture* pShadowMap=pShadow->GetRenderTarget();

	Vect3f pos(100,100,0);
	Vect4f out;
    pShadow->matViewProj.xform(pos, out);

	gb_RenderDevice3D->SetTexture(1,pShadowMap,0);
	gb_RenderDevice3D->SetTexture(2,pShadowMap,0);
	gb_RenderDevice3D->SetTexture(3,pShadowMap,0);
	gb_RenderDevice3D->SetTexture(4,pShadowMap,0);
	gb_RenderDevice3D->SetTexture(5,pLightMap,0);

	pVSTileMapScene->Select(&pShadow->matViewProj,pShadowMap->GetWidth(),NULL,NULL);
	pPSTileMapScene->Select();

    gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 4, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    gb_RenderDevice3D->SetSamplerState( 4, D3DSAMP_MAGFILTER, D3DTEXF_POINT );

	TerraInterface* terra=TileMap->GetTerra();
	pVSTileMapScene->SetWorldSize(Vect2f(terra->SizeX(),terra->SizeY()));
}

void DrawTypeRadeon8500::SetMaterialTilemapShadow()
{
	pVSTileMapShadow->Select();
	pPSTileMapShadow->Select();
}

void DrawTypeRadeon8500::SetTileColor(sColor4f c)
{
	Vect4f cf(c.r,c.g,c.b,c.a);
	gb_RenderDevice3D->SetPixelShaderConstant(2,&cf);
}

void DrawTypeRadeon8500::SetMaterial(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data)
{
	eBlendMode blend=DrawType::SetMaterialSimply(Phase,Texture0,Texture1,Data);

	if(Data->mat&MAT_BUMP)
	{
//		gb_RenderDevice3D->SetPixelShaderConstant(20,(Vect4f*)&Data->Ambient);
		gb_RenderDevice3D->SetPixelShaderConstant(5, reinterpret_cast<const Vect4f*>(&Data->Diffuse));
		gb_RenderDevice3D->SetPixelShaderConstant(6, reinterpret_cast<const Vect4f*>(&Data->Specular));
//		gb_RenderDevice3D->SetPixelShaderConstant(23,&Vect4f(0,0,0,Data->Power));
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
//		gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_MINFILTER, gb_RenderDevice3D->texture_interpolation);
//		gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_MIPFILTER, gb_RenderDevice3D->texture_interpolation);
//		gb_RenderDevice3D->SetSamplerState(2, D3DSAMP_MAGFILTER, gb_RenderDevice3D->texture_interpolation);
		
		last_vs=pVSObjectSceneLight2;
		last_ps=pPSObjectScene2;
		last_vs->SetTextureTransform(Data->TexMatrix);
	}else
	{
		bool light=Data->mat&MAT_LIGHT;
		last_vs=(!light)?pVSObjectScene:pVSObjectSceneLight;
		last_ps=(!light)?pPSObjectSceneNoLight:pPSObjectScene;
/*
		if(Data->mat&MAT_TEXMATRIX_STAGE1)
		{
			last_vs=pVSObjectSceneLightT;
			last_vs->SetTextureTransform(Data->TexMatrix);
		}
*/
	}

	last_vs->SetMaterial(Data);
}

