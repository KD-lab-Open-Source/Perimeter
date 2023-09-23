#include "StdAfxRD.h"
#include "D3DRender.h"
#include "DrawBuffer.h"
#include "Scene.h"
#include "ObjNode.h"
#include "ObjMesh.h"
#include "MeshTri.h"
#include "tilemap/TileMap.h"

#include "AnimChannel.h"
#include "Font.h"

#include "Silicon.h"

void cD3DRender::SetRenderTarget(cTexture* target,LPDIRECT3DSURFACE9 pZBuffer)
{
    FlushActiveDrawBuffer();
	for( int nPasses=0; nPasses<nSupportTexture; nPasses++ ) 
	{
		lpD3DDevice->SetTexture( nPasses, CurrentTexture[nPasses]=0 );
	}

	LPDIRECT3DSURFACE9 lpDDSurface= nullptr;
    TextureImage* frm = target->GetFrameImage(0);
	RDCALL(frm->d3d->GetSurfaceLevel(0,&lpDDSurface));
	RDCALL(lpD3DDevice->SetRenderTarget(0,lpDDSurface));
	RDCALL(lpD3DDevice->SetDepthStencilSurface(pZBuffer));

	if(!pZBuffer)
	{
		SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
		SetRenderState( D3DRS_ZWRITEENABLE, FALSE ); 
	}

	lpDDSurface->Release();
}

void cD3DRender::RestoreRenderTarget()
{
    FlushActiveDrawBuffer();
	RDCALL(lpD3DDevice->SetRenderTarget(0,lpBackBuffer));
	RDCALL(lpD3DDevice->SetDepthStencilSurface(lpZBuffer));
	SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	SetRenderState( D3DRS_ZWRITEENABLE, TRUE ); 
}

void cD3DRender::SetDrawNode(cCamera *pDrawNode)
{
	if (DrawNode==pDrawNode||lpD3DDevice==0) return;
    FlushActiveDrawBuffer();
    cInterfaceRenderDevice::SetDrawNode(pDrawNode);
	if(DrawNode->GetRenderTarget())
	{
		LPDIRECT3DSURFACE9 pZBuffer=DrawNode->GetZBuffer();
		SetRenderTarget(DrawNode->GetRenderTarget(),pZBuffer);
		uint32_t color=0;
		if(pDrawNode->GetAttribute(ATTRCAMERA_SHADOW))
		{
			if(Option_ShadowType==SHADOW_MAP_SELF && 
				(dtAdvance->GetID()==DT_RADEON8500 ||
				dtAdvance->GetID()==DT_RADEON9700
				))
			{
				color=D3DCOLOR_RGBA(0,0,0,255);
				kShadow=0.25f;
			}else
			if(CurrentMod4==D3DTOP_MODULATE4X)
			{
				color=D3DCOLOR_RGBA(63,63,63,255);
				kShadow=0.25f;
			}
			else if(CurrentMod4==D3DTOP_MODULATE2X)
			{
				color=D3DCOLOR_RGBA(127,127,127,255);
				kShadow=0.5f;
			}else
			{
				color=D3DCOLOR_RGBA(255,255,255,255);
				kShadow=1.f;
			}
		}else
		{
//			color=D3DCOLOR_RGBA(63,63,63,255);
//			kShadow=0.25f;
			color=D3DCOLOR_RGBA(255,255,255,255);
			kShadow=1.3f;
		}

		if(!pDrawNode->GetAttribute(ATTRCAMERA_NOCLEARTARGET))
		{
			if(!pZBuffer)
			{
				RDCALL(lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,color,1,0));
			}else
			{
				RDCALL(lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 
					pDrawNode->GetAttribute(ATTRCAMERA_ZINVERT)?0:1, 0));
			}
		}
	}
	else
	{
		RestoreRenderTarget();
	}

	SetDrawTransform(pDrawNode);
}

void cD3DRender::SetDrawTransform(class cCamera *pDrawNode)
{
    FlushActiveDrawBuffer();
    isOrthographicProjSet = false;
	RDCALL(lpD3DDevice->SetTransform(D3DTS_PROJECTION,reinterpret_cast<const D3DMATRIX*>(&pDrawNode->matProj)));
	RDCALL(lpD3DDevice->SetTransform(D3DTS_VIEW,reinterpret_cast<const D3DMATRIX*>(&pDrawNode->matView)));
	RDCALL(lpD3DDevice->SetViewport((D3DVIEWPORT9*)&pDrawNode->vp));
    if (pDrawNode->GetAttribute(ATTRCAMERA_REFLECTION) == 0) {
        CameraCullMode = CULL_CW;    // прямое изображение
    } else {
        CameraCullMode = CULL_CCW;    // отраженное изображение
    }
    SetRenderState(RS_CULLMODE, CameraCullMode);
}

void cD3DRender::SetWorldMat4f(const Mat4f* matrix) {
    FlushActiveDrawBuffer();
    if (isOrthographicProjSet) {
        isOrthographicProjSet = false;
        //We need to restore 3D projection/view matrixes
        if (DrawNode) {
            RDCALL(lpD3DDevice->SetTransform(D3DTS_PROJECTION,reinterpret_cast<const D3DMATRIX*>(&DrawNode->matProj)));
            RDCALL(lpD3DDevice->SetTransform(D3DTS_VIEW,reinterpret_cast<const D3DMATRIX*>(&DrawNode->matView)));
        }
    }
    if (!matrix) matrix = &Mat4f::ID;
    RDCALL(lpD3DDevice->SetTransform(D3DTS_WORLD, reinterpret_cast<const D3DMATRIX*>(matrix)));
};

void cD3DRender::BeginDrawMesh(bool obj_mesh, bool use_shadow) {
    VISASSERT(!dtDrawActive);
    FlushActiveDrawBuffer();
    dtDrawActive = obj_mesh ? dtFixed : dtAdvance;
    dtDrawActive->BeginDraw(use_shadow);
}

void cD3DRender::EndDrawMesh() {
    VISASSERT(dtDrawActive);
    if (!dtDrawActive) return;
    dtDrawActive->EndDraw();
    dtDrawActive = nullptr;
    FlushActiveDrawBuffer();
}

void cD3DRender::SetSimplyMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) {
    VISASSERT(dtDrawActive);
    if (!dtDrawActive) return;
    dtDrawActive->SetSimplyMaterial(mesh, data);
}

void cD3DRender::DrawNoMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) {
    VISASSERT(dtDrawActive);
    if (!dtDrawActive) return;
    dtDrawActive->DrawNoMaterial(mesh, data);
}

void cD3DRender::BeginDrawShadow(bool shadow_map) {
    VISASSERT(!dtDrawShadowActive);
    FlushActiveDrawBuffer();
    dtDrawShadowActive = shadow_map ? dtAdvance : dtFixed;
    dtDrawShadowActive->BeginDrawShadow();
}

void cD3DRender::EndDrawShadow() {
    VISASSERT(dtDrawShadowActive);
    if (!dtDrawShadowActive) return;
    dtDrawShadowActive->EndDrawShadow();
    dtDrawShadowActive = nullptr;
    FlushActiveDrawBuffer();
}

void cD3DRender::SetSimplyMaterialShadow(cObjMesh* mesh, cTexture* texture) {
    VISASSERT(dtDrawShadowActive);
    if (!dtDrawShadowActive) return;
    dtDrawShadowActive->SetSimplyMaterialShadow(mesh, texture);
}

void cD3DRender::DrawNoMaterialShadow(cObjMesh* mesh) {
    VISASSERT(dtDrawShadowActive);
    if (!dtDrawShadowActive) return;
    dtDrawShadowActive->DrawNoMaterialShadow(mesh);
}

void cD3DRender::SetMaterialTilemap(cTileMap *TileMap) {
    dtAdvance->SetMaterialTilemap(TileMap);
}

void cD3DRender::SetMaterialTilemapShadow() {
    dtAdvance->SetMaterialTilemapShadow();
}

void cD3DRender::SetTileColor(sColor4f color) {
    dtAdvance->SetTileColor(color);
}

void cD3DRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode)
{
	if(CurrentFont==0)
	{
		VISASSERT(CurrentFont!=0);
		return;
	}
    FlushActiveDrawBuffer();
    
	SetTexture(0,CurrentFont->GetTexture(),0);
	SetTextureImage(1, nullptr);
	SetTextureImage(2, nullptr);
	SetBlendState(blend_mode);

	SetRenderState(D3DRS_SPECULARENABLE,FALSE);
	SetRenderState(D3DRS_LIGHTING,FALSE);
	SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);


	SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );

	SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
	SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );

	if(CurrentFont->GetScale().x>1.01f || CurrentFont->GetScale().y>1.01f)
		SetRenderState(RS_BILINEAR,1);
	else
		SetRenderState(RS_BILINEAR,0);
    
    cInterfaceRenderDevice::OutText(x,y,string,color,align,blend_mode);
    FlushActiveDrawBuffer();
}

void cD3DRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                         cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase,float lerp_factor) {
    FlushActiveDrawBuffer();
    
    SetNoMaterial(blend_mode, phase, pTexture, CurrentFont->GetTexture(), mode);
    
    uint32_t index1=GetTextureStageState(1, D3DTSS_TEXCOORDINDEX);
    SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
    SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);

    SetRenderState(RS_BILINEAR,0);

    sColor4c lerp(255*lerp_factor,255*lerp_factor,255*lerp_factor,255*(1-lerp_factor));
    SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATECOLOR_ADDALPHA);
    SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
    SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
    SetRenderState(D3DRS_TEXTUREFACTOR, ConvertColor(lerp));

    SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
    SetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
    SetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);

	SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE);
    SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
    SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);

    cInterfaceRenderDevice::OutText(x,y,string,color,align,blend_mode,pTexture,mode,uv,duv,phase,lerp_factor);
    FlushActiveDrawBuffer();

    SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
    SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,index1);

    SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

    SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

    SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
}

void cD3DRender::DrawSprite2(int x1,int y1,int dx,int dy,
                             float u0,float v0,float du0,float dv0,
                             float u1,float v1,float du1,float dv1,
                             cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha,float phase,
                             eColorMode mode,eBlendMode blend_mode)
{
    if (!Tex1 || !Tex2) return;
    VISASSERT(bActiveScene);
    int x2=x1+dx,y2=y1+dy;
    if(dx>=0) { if(x2<xScrMin||x1>xScrMax) return; }
    else if(x1<xScrMin||x2>xScrMax) return;
    if(dy>=0) { if(y2<yScrMin||y1>yScrMax) return; }
    else if(y1<yScrMin||y2>yScrMax) return;

    FlushActiveDrawBuffer();

    if (blend_mode == ALPHA_NONE && alpha < 1.0f) {
        blend_mode = ALPHA_BLEND;
    }
    SetNoMaterial(blend_mode,phase,Tex2,Tex1,mode);

    UseOrthographicProjection();

    uint32_t index1=GetTextureStageState(1, D3DTSS_TEXCOORDINDEX);
    SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
    SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);

    sColor4c lerp(255*lerp_factor,255*lerp_factor,255*lerp_factor,255*(1-lerp_factor));

    SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATECOLOR_ADDALPHA);
    SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
    SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
    SetRenderState(D3DRS_TEXTUREFACTOR, ConvertColor(lerp));

    SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
    SetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
    SetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);

    SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE);
    SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE);
    SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);

    cInterfaceRenderDevice::DrawSprite2(x1, y1, dx, dy, u0, v0, du0, dv0, u1, v1, du1, dv1, Tex1, Tex2, lerp_factor, alpha, phase, mode, blend_mode);
    FlushActiveDrawBuffer();

    SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
    SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,index1);

    SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

    SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

    SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

}

void cD3DRender::Draw(class ElasticSphere *es) {
    cInterfaceRenderDevice::Draw(es);

    int is = es->theta_size;
    int js = es->psi_size;
    NumberPolygon += (is+1) * (js+1) + (is+2) * (js-1) - 2;
	NumDrawObject++;
}
