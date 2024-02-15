//////////////////////////////DrawTypeMixed////////////////////
DrawTypeMixed::DrawTypeMixed()
{
}

DrawTypeMixed::~DrawTypeMixed()
{
}

void DrawTypeMixed::BeginDraw(bool use_shadow)
{
	gb_RenderDevice3D->dtFixed->BeginDraw(use_shadow);
	gb_RenderDevice3D->dtAdvanceOriginal->BeginDraw(false);
}

void DrawTypeMixed::EndDraw()
{
	gb_RenderDevice3D->dtAdvanceOriginal->EndDraw();
	gb_RenderDevice3D->dtFixed->EndDraw();
	gb_RenderDevice3D->SetVertexShader(NULL);
	gb_RenderDevice3D->SetPixelShader(NULL);
}

void DrawTypeMixed::SetSimplyMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
{
	if(IsBump(Data))
		gb_RenderDevice3D->dtAdvanceOriginal->SetSimplyMaterial(Mesh,Data);
	else
	{
		gb_RenderDevice3D->SetVertexShader(NULL);
		gb_RenderDevice3D->SetPixelShader(NULL);
		gb_RenderDevice3D->dtFixed->SetSimplyMaterial(Mesh,Data);
	}
}

void DrawTypeMixed::DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
{
	if(IsBump(Data))
		gb_RenderDevice3D->dtAdvanceOriginal->DrawNoMaterial(Mesh,Data);
	else
		gb_RenderDevice3D->dtFixed->DrawNoMaterial(Mesh,Data);
}

void DrawTypeMixed::SetSimplyMaterialShadow(cObjMesh *Mesh,cTexture *Texture)
{
	gb_RenderDevice3D->SetVertexShader(NULL);
	gb_RenderDevice3D->SetPixelShader(NULL);
	gb_RenderDevice3D->dtFixed->SetSimplyMaterialShadow(Mesh,Texture);
}

void DrawTypeMixed::DrawNoMaterialShadow(cObjMesh *Mesh)
{
	gb_RenderDevice3D->dtFixed->DrawNoMaterialShadow(Mesh);
}

bool DrawTypeMixed::CreateShadowTexture(int xysize)
{
	return gb_RenderDevice3D->dtFixed->CreateShadowTexture(xysize);
}

void DrawTypeMixed::SetMaterialTilemap(cTileMap *TileMap)
{
	gb_RenderDevice3D->SetVertexShader(NULL);
	gb_RenderDevice3D->SetPixelShader(NULL);
	gb_RenderDevice3D->dtFixed->SetMaterialTilemap(TileMap);
}

void DrawTypeMixed::SetMaterialTilemapShadow()
{
	gb_RenderDevice3D->dtFixed->SetMaterialTilemapShadow();
}

void DrawTypeMixed::SetTileColor(sColor4f color)
{
	gb_RenderDevice3D->dtFixed->SetTileColor(color);
}

cTexture* DrawTypeMixed::GetShadowMap()
{
	return gb_RenderDevice3D->dtFixed->GetShadowMap();
}

IDirect3DSurface9* DrawTypeMixed::GetZBuffer()
{
	return gb_RenderDevice3D->dtFixed->GetZBuffer();
}

cTexture* DrawTypeMixed::GetLightMap()
{
	return gb_RenderDevice3D->dtFixed->GetLightMap();
}
