#include "StdAfxRD.h"

#include "D3DRender.h"
#include "DrawBuffer.h"
#include "ObjNode.h"
#include "ObjMesh.h"
#include "MeshTri.h"
#include "DrawType.h"
#include "Scene.h"
#include "tilemap/TileMap.h"

#define DEL(p) {if(p)delete p;}

DrawType::~DrawType()
{
	DeleteShadowTexture();
}

void DrawType::DeleteShadowTexture()
{
	RELEASE(pShadowMap);
	RELEASE(pZBuffer);
	RELEASE(pLightMap);
}

void DrawType::SetStream(cObjMesh *Mesh) {
}

void DrawType::DrawPrimitive(cObjMesh *Mesh)
{
	if(Option_DrawMeshScreen)
	{
		cMeshTri *Tri=Mesh->GetTri();
        gb_RenderDevice->SubmitDrawBuffer(Tri->db, &Tri->dbr);
		gb_RenderDevice3D->NumDrawObject++;
	}
}

eBlendMode DrawType::SetMaterialSimply(float Phase,cTexture *Texture0,cTexture *Texture1,sDataRenderMaterial *Data,int texture_offset)
{
	gb_RenderDevice3D->SetTexture(0+texture_offset, Texture0, Phase);
	gb_RenderDevice3D->SetTexture(1+texture_offset, Texture1, Phase);
	gb_RenderDevice3D->SetTextureImage(2+texture_offset, nullptr);

	eBlendMode blend=ALPHA_NONE;
	if(Texture0 && Texture0->IsAlphaTest())
		blend=ALPHA_TEST;

	if((Texture0 && Texture0->IsAlpha()) || Data->Diffuse.a<0.99f)
		blend=ALPHA_BLEND;

		 if(Data->mat&MAT_ALPHA_SUBBLEND)
			blend=ALPHA_SUBBLEND;
	else if(Data->mat&MAT_ALPHA_ADDBLENDALPHA)
			blend=ALPHA_ADDBLENDALPHA;
	else if(Data->mat&MAT_ALPHA_ADDBLEND)
			blend=ALPHA_ADDBLEND;

	gb_RenderDevice3D->SetBlendState(blend);
	return blend;
}

#include "DrawTypeFixed.inl"
#include "DrawTypeRadeon9700.inl"
#include "DrawTypeRadeon8500.inl"
#include "DrawTypeGeforce3.inl"
#include "DrawTypeMixed.inl"
#include "DrawTypeGeforceFX.inl"