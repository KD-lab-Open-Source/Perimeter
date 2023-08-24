///////////////////////DrawTypeFixedPipeline/////////////////
DrawTypeFixedPipeline::DrawTypeFixedPipeline()
{
}

DrawTypeFixedPipeline::~DrawTypeFixedPipeline()
{
}

void DrawTypeFixedPipeline::SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
{
	bool specular=Data->Specular.r>=1/256.0f || 
				  Data->Specular.g>=1/256.0f || 
				  Data->Specular.b>=1/256.0f;
	if(specular)
		Data->mat|=MAT_COLOR_ADD_SPECULAR;
	else
		Data->mat&=~(uint32_t)MAT_COLOR_ADD_SPECULAR;

	bool bump=Data->mat&MAT_BUMP;
	SetMaterial(Data->MaterialAnimPhase,Data->Tex[0],bump?NULL:Data->Tex[1],Data);

	SetStream(Mesh);
}

void DrawTypeFixedPipeline::DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
{
	SetPointLight(Mesh->GetRootNode()->GetLight());

	gb_RenderDevice3D->SetWorldMatXf(Mesh->GetGlobalMatrix());
	if(Data->mat&MAT_TEXMATRIX_STAGE1)
	{
		Mat4f mat;
		MatXf &m=Data->TexMatrix;
		memset(&mat,0,sizeof(mat));
        mat.xx = m.rot()[0][0],	mat.xy = m.rot()[0][1];
        mat.yx = m.rot()[1][0],	mat.yy = m.rot()[1][1];
        mat.zx = m.trans().x,	mat.zy = m.trans().y;
		RDCALL(gb_RenderDevice3D->lpD3DDevice->SetTransform(D3DTS_TEXTURE0, reinterpret_cast<const D3DMATRIX*>(&mat)));
	}

	if(Data->mat&MAT_RENDER_SPHEREMAP)
	{ // сферический мапинг
		Mat4f mat;
		memset(&mat,0,sizeof(mat));
        mat.xx=mat.yy=mat.wx=mat.wy=0.5f;
		RDCALL(gb_RenderDevice3D->lpD3DDevice->SetTransform(D3DTS_TEXTURE1, reinterpret_cast<const D3DMATRIX*>(&mat)));
	}

	DrawPrimitive(Mesh);
}

void DrawTypeFixedPipeline::SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture)
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

	SetMaterial(0,Texture,NULL,&mat);
	gb_RenderDevice3D->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
	SetStream(Mesh);
}

void DrawTypeFixedPipeline::DrawNoMaterialShadow(cObjMesh *Mesh)
{
	gb_RenderDevice3D->SetWorldMatXf(Mesh->GetGlobalMatrix());
	DrawPrimitive(Mesh);
}

bool DrawTypeFixedPipeline::CreateShadowTexture(int xysize)
{
	pShadowMap=GetTexLibrary()->CreateRenderTexture(xysize,xysize,0,false);
	if(!pShadowMap)
	{
		DeleteShadowTexture();
		return false;
	}

	if(Option_ShadowType==SHADOW_MAP)
	{
		HRESULT hr=gb_RenderDevice3D->lpD3DDevice->CreateDepthStencilSurface(xysize, xysize, 
			D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &pZBuffer, NULL);
		if(FAILED(hr))
		{
			DeleteShadowTexture();
			return false;
		}
	}else
		pZBuffer=NULL;
	return true;
}

void DrawTypeFixedPipeline::SetMaterial(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data)
{
	eBlendMode blend=DrawType::SetMaterialSimply(Phase,Texture0,Texture1,Data);

	gb_RenderDevice3D->SetRenderState(D3DRS_SPECULARENABLE,
				(Data->mat&MAT_COLOR_ADD_SPECULAR)?TRUE:FALSE);
	gb_RenderDevice3D->SetRenderState(D3DRS_LIGHTING,TRUE);
	gb_RenderDevice3D->SetRenderState(D3DRS_NORMALIZENORMALS,
				(Data->mat&MAT_LIGHT)?TRUE:FALSE);
	gb_RenderDevice3D->SetTextureStageState( 0, D3DTSS_ALPHAOP, 
			(blend>ALPHA_NONE)?D3DTOP_MODULATE:D3DTOP_DISABLE );
	gb_RenderDevice3D->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	gb_RenderDevice3D->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, 
			(Data->mat&MAT_TEXMATRIX_STAGE1)?D3DTTFF_COUNT2:D3DTTFF_DISABLE);
	gb_RenderDevice3D->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );

	if(Texture1)
	{
		if(Data->mat&MAT_TEXNORMAL_STAGE2)
		{
			gb_RenderDevice3D->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			gb_RenderDevice3D->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
		}else
		{
			gb_RenderDevice3D->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			gb_RenderDevice3D->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		}

		gb_RenderDevice3D->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD);
	}else 
		gb_RenderDevice3D->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

	gb_RenderDevice3D->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );

	D3DMATERIAL9 Material;
	if( Data->mat&MAT_LIGHT )
	{
		Material.Diffuse.r=Data->Diffuse.r;
		Material.Diffuse.g=Data->Diffuse.g;
		Material.Diffuse.b=Data->Diffuse.b;
		Material.Diffuse.a=Data->Diffuse.a;

		Material.Ambient.r=Data->Ambient.r;
		Material.Ambient.g=Data->Ambient.g;
		Material.Ambient.b=Data->Ambient.b;
		Material.Ambient.a=0;

		Material.Specular.r=Data->Specular.r;
		Material.Specular.g=Data->Specular.g;
		Material.Specular.b=Data->Specular.b;
		Material.Specular.a=0;

		Material.Emissive.r=Data->Emissive.r;
		Material.Emissive.g=Data->Emissive.g;
		Material.Emissive.b=Data->Emissive.b;
		Material.Emissive.a=0;

		Material.Power = Data->Power;
	}else
	{
		Material.Diffuse.r=0;
		Material.Diffuse.g=0;
		Material.Diffuse.b=0;
		Material.Diffuse.a=Data->Diffuse.a;

		Material.Ambient.r=Data->Diffuse.r;
		Material.Ambient.g=Data->Diffuse.g;
		Material.Ambient.b=Data->Diffuse.b;
		Material.Ambient.a=1;

		Material.Specular.r=0;
		Material.Specular.g=0;
		Material.Specular.b=0;
		Material.Specular.a=0;

		Material.Emissive.r=0;
		Material.Emissive.g=0;
		Material.Emissive.b=0;
		Material.Emissive.a=0;

		Material.Power = 0;
	}

	RDCALL(gb_RenderDevice3D->lpD3DDevice->SetMaterial(&Material));
}

void DrawTypeFixedPipeline::SetMaterialTilemap(cTileMap *TileMap)
{
}

void DrawTypeFixedPipeline::SetMaterialTilemapShadow()
{
	gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
}

void DrawTypeFixedPipeline::SetTileColor(sColor4f color)
{
	color.r*=0.5f;
	color.g*=0.5f;
	color.b*=0.5f;
	gb_RenderDevice3D->SetRenderState(D3DRS_TEXTUREFACTOR,color.RGBA());
//	if((color.RGBA()&0xFFFFFF)==0xFFFFFF)
//		gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
//	else
		gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
	if(color.GetA()==255)
		gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	else
		gb_RenderDevice3D->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

}

void DrawTypeFixedPipeline::SetPointLight(std::vector<cUnkLight*>& light)
{
	for(int i=0;i<2;i++)
	{
		if(i>=light.size())
		{
			RDCALL(gb_RenderDevice3D->lpD3DDevice->LightEnable(i+1,FALSE));
			continue;
		}

		float mul=4;
		cUnkLight* l=light[i];
		D3DLIGHT9 p;
		p.Type=D3DLIGHT_POINT;
		p.Diffuse.r=p.Diffuse.g=p.Diffuse.b=p.Diffuse.a=0;
		p.Specular.r=p.Specular.g=p.Specular.b=p.Specular.a=0;
		p.Ambient.r=p.Ambient.g=p.Ambient.b=p.Ambient.a=0;

		p.Ambient=*(D3DCOLORVALUE*)&l->GetDiffuse();
		p.Ambient.r*=mul;
		p.Ambient.g*=mul;
		p.Ambient.b*=mul;
		p.Specular=p.Ambient;
		
		p.Position=*(D3DVECTOR*)&l->GetPos();
		p.Direction.x=p.Direction.y=p.Direction.z=1;
		p.Range=10000;
		p.Attenuation0=1;
		p.Attenuation1=0;//1/max(l->GetRadius(),1.0f);
		p.Attenuation2=1/sqr(max(l->GetRadius(),1.0f));
		p.Theta=0;
		p.Phi=0;

		RDCALL(gb_RenderDevice3D->lpD3DDevice->SetLight(i+1,&p));
		RDCALL(gb_RenderDevice3D->lpD3DDevice->LightEnable(i+1,TRUE));
	}
}

void DrawTypeFixedPipeline::BeginDraw(bool use_shadow)
{
}

void DrawTypeFixedPipeline::EndDraw()
{
	for(int i=0;i<2;i++)
	{
		RDCALL(gb_RenderDevice3D->lpD3DDevice->LightEnable(i+1,FALSE));
	}
}
