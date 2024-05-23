#include "StdAfxRD.h"
#include "xmath.h"
#include "VertexFormat.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "SokolResources.h"
#include "DrawBuffer.h"
#include "Font.h"
#include "MeshTri.h"
#include "ObjMesh.h"
#include "TileMap.h"

void cSokolRender::SetNoMaterial(eBlendMode blend, float Phase, cTexture* Texture0, cTexture* Texture1,
                                 eColorMode color_mode) {
    if (Texture0) {
        if (blend == ALPHA_NONE && Texture0->IsAlphaTest()) {
            blend = ALPHA_TEST;
        }
        if (blend <= ALPHA_TEST && Texture0->IsAlpha()) {
            blend = ALPHA_BLEND;
        }
    }

    SetColorMode(color_mode);
    SetBlendState(blend);
    SetTexture(0, Texture0, Phase);
    SetTexture(1, Texture1, Phase);

    if (activePipelineType != PIPELINE_TYPE_MESH) {
        FinishActiveDrawBuffer();
    }
    activePipelineType = PIPELINE_TYPE_MESH;
}

// Sprites / 2d textures

void cSokolRender::DrawSprite2(int x1,int y1,int dx,int dy,
                               float u0,float v0,float du0,float dv0,
                               float u1,float v1,float du1,float dv1,
                               cTexture *Tex1,cTexture *Tex2,float lerp_factor,float alpha,float phase,
                               eColorMode mode,eBlendMode blend_mode) {
    if (!Tex1||!Tex2) return;
    VISASSERT(ActiveScene);
    
    if (blend_mode == ALPHA_NONE && alpha < 1.0f) {
        blend_mode = ALPHA_BLEND;
    }
    
    SetTex2Lerp(lerp_factor);
    SetNoMaterial(blend_mode, phase, Tex2, Tex1, mode);
    
    cInterfaceRenderDevice::DrawSprite2(x1, y1, dx, dy, u0, v0, du0, dv0, u1, v1, du1, dv1, Tex1, Tex2, lerp_factor, alpha, phase, mode, blend_mode);
    SetTex2Lerp(-1);
}

// Text render

void cSokolRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode) {    
    SetNoMaterial(blend_mode, 0, CurrentFont->GetTexture());

    cInterfaceRenderDevice::OutText(x, y, string, color, align, blend_mode);
}

void cSokolRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
                         cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase,float lerp_factor) {
    SetTex2Lerp(lerp_factor);
    SetNoMaterial(blend_mode, phase, pTexture, CurrentFont->GetTexture(), mode);
    
    cInterfaceRenderDevice::OutText(x, y, string, color, align, blend_mode, pTexture, mode, uv, duv, phase, lerp_factor);
    SetTex2Lerp(-1);
}

// Mesh draw

void cSokolRender::BeginDrawMesh(bool obj_mesh, bool use_shadow_) {
    use_shadow = use_shadow_;
    pShadow = GetDrawNode()->FindCildCamera(ATTRCAMERA_SHADOWMAP);
}

void cSokolRender::EndDrawMesh() {
    //TODO
}

void cSokolRender::SetSimplyMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) {
    bool specular=data->Specular.r>=1/256.0f ||
                  data->Specular.g>=1/256.0f ||
                  data->Specular.b>=1/256.0f;
    if (specular) {
        data->mat |= MAT_COLOR_ADD_SPECULAR;
    } else {
        data->mat &= ~static_cast<uint32_t>(MAT_COLOR_ADD_SPECULAR);
    }

    bool bump = data->mat&MAT_BUMP;
    
    cTexture* Texture0 = data->Tex[0];
    eBlendMode blend=ALPHA_NONE;
    if (Texture0) {
        if (Texture0->IsAlphaTest()) {
            blend = ALPHA_TEST;
        } else if (Texture0->IsAlpha() || data->Diffuse.a < 0.99f) {
            blend = ALPHA_BLEND;
        }
    }

    if (data->mat&MAT_LIGHT) {
        SetMaterial(
                SOKOL_MAT_LIGHT,
                data->Diffuse,
                {data->Ambient.r, data->Ambient.g, data->Ambient.b, 0},
                {data->Specular.r, data->Specular.g, data->Specular.b, 0},
                {data->Emissive.r, data->Emissive.g, data->Emissive.b, 0},
                data->Power
        );
    } else {
        SetMaterial(
                SOKOL_MAT_NONE,
                {0, 0, 0, data->Diffuse.a},
                {data->Diffuse.r, data->Diffuse.g, data->Diffuse.b, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                0
        );
    }

    if (data->mat&MAT_ALPHA_SUBBLEND) {
        blend = ALPHA_SUBBLEND;
    } else if(data->mat&MAT_ALPHA_ADDBLENDALPHA) {
        blend = ALPHA_ADDBLENDALPHA;
    } else if(data->mat&MAT_ALPHA_ADDBLEND) {
        blend = ALPHA_ADDBLEND;
    }

    SetColorMode(COLOR_MOD);
    SetBlendState(blend);
    SetTexture(0, Texture0, data->MaterialAnimPhase);
    SetTexture(1, bump ? nullptr: data->Tex[1], data->MaterialAnimPhase);

    if (activePipelineType != PIPELINE_TYPE_MESH) {
        FinishActiveDrawBuffer();
    }
    activePipelineType = PIPELINE_TYPE_MESH;
}

void cSokolRender::DrawNoMaterialMesh(cObjMesh* mesh, sDataRenderMaterial* data) {
    //TODO SetPointLight(mesh->GetRootNode()->GetLight());

    SetWorldMatXf(mesh->GetGlobalMatrix());
    Mat4f& tex0mat = activeTextureTransform[0];
    if(data->mat&MAT_TEXMATRIX_STAGE1) {
        MatXf &m=data->TexMatrix;
        tex0mat.xx = m.rot()[0][0],	tex0mat.xy = m.rot()[0][1];
        tex0mat.yx = m.rot()[1][0],	tex0mat.yy = m.rot()[1][1];
        tex0mat.zx = m.trans().x,	tex0mat.zy = m.trans().y;
    } else {
        tex0mat = Mat4f::ID;
    }

    Mat4f& tex1mat = activeTextureTransform[1];
    if(data->mat&MAT_RENDER_SPHEREMAP) { // сферический мапинг
        Mat4f mat;
        memset(&mat,0,sizeof(mat));
        tex1mat.xx=tex1mat.yy=tex1mat.wx=tex1mat.wy=0.5f;
    } else {
        tex1mat = Mat4f::ID;
    }

    cMeshTri* Tri = mesh->GetTri();
    SubmitDrawBuffer(Tri->db, &Tri->dbr);

    tex0mat = Mat4f::ID;
    tex1mat = Mat4f::ID;
}

void cSokolRender::BeginDrawShadow(bool shadow_map) {
    pShadow = GetDrawNode()->FindCildCamera(ATTRCAMERA_SHADOWMAP);
}

void cSokolRender::EndDrawShadow() {
    //TODO
}

void cSokolRender::SetSimplyMaterialShadow(cObjMesh* mesh, cTexture* texture) {
    sDataRenderMaterial mat;
    float f = 0.5f * kShadow;
    mat.Ambient = sColor4f(0 ,0, 0, 1);
    mat.Diffuse = sColor4f(f, f, f, 1);
    mat.Specular = sColor4f(0, 0, 0, 1);
    mat.Emissive = sColor4f(0, 0, 0, 1);
    mat.Power = 0;
    mat.mat = 0;
    mat.MaterialAnimPhase = 0;

    // SetMaterial(0, texture, NULL, &mat);
    SetTexture(0, texture, 0);

    eBlendMode blend=ALPHA_NONE;
    if(texture && texture->IsAlphaTest())
        blend=ALPHA_TEST;

    if((texture && texture->IsAlpha()) || mat.Diffuse.a<0.99f)
        blend=ALPHA_BLEND;

    if(mat.mat&MAT_ALPHA_SUBBLEND)
        blend=ALPHA_SUBBLEND;
    else if(mat.mat&MAT_ALPHA_ADDBLENDALPHA)
        blend=ALPHA_ADDBLENDALPHA;
    else if(mat.mat&MAT_ALPHA_ADDBLEND)
        blend=ALPHA_ADDBLEND;

    SetBlendState(blend);
    
    // gb_RenderDevice3D->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    // SetStream(Mesh);

    if (activePipelineType != PIPELINE_TYPE_OBJECT_SHADOW) {
        FinishActiveDrawBuffer();
    }
    activePipelineType = PIPELINE_TYPE_OBJECT_SHADOW;
}

void cSokolRender::DrawNoMaterialShadow(cObjMesh* mesh) {
    SetWorldMatXf(mesh->GetGlobalMatrix());
    cMeshTri *Tri = mesh->GetTri();
    SubmitDrawBuffer(Tri->db, &Tri->dbr);
}

void cSokolRender::SetMaterialTilemap(cTileMap *TileMap) {
    float fOffsetX = 0.5f + (0.0f / pShadowMap->GetWidth());
    float fOffsetY = 0.5f + (0.0f / pShadowMap->GetWidth());
    float range = 1;
    float fBias = -0.005f * range;
    Mat4f matTexAdj( 0.5f,     0.0f,     0.0f,  0.0f,
                    0.0f,    -0.5f,     0.0f,  0.0f,
                    0.0f,     0.0f,     range, 0.0f,
                    fOffsetX, fOffsetY, fBias, 1.0f );

    Mat4f matlight = pShadow->matViewProj;
    activeShadowMatrix = matlight * matTexAdj;

    activeCommand.SetTexture(0, pShadowMap->GetFrameImage(0)->sg->image);
    activeCommand.SetTexture(2, pLightMap->GetFrameImage(0)->sg->image);

    TerraInterface* terra = TileMap->GetTerra();
    activeWorldSize = Vect2f(terra->SizeX(), terra->SizeY());

    if (activePipelineType != PIPELINE_TYPE_TILE_MAP) {
        FinishActiveDrawBuffer();
    }
    activePipelineType = PIPELINE_TYPE_TILE_MAP;
}

void cSokolRender::SetMaterialTilemapShadow() {
    if (activePipelineType != PIPELINE_TYPE_OBJECT_SHADOW) {
        FinishActiveDrawBuffer();
    }
    activePipelineType = PIPELINE_TYPE_OBJECT_SHADOW;
}

void cSokolRender::SetTileColor(sColor4f color) {
    if (activeCommandTileColor != color) {
        FinishActiveDrawBuffer();
        activeCommandTileColor = color;
    }
}

bool cSokolRender::CreateShadowTexture(int xysize) {
    DeleteShadowTexture();

    auto SetupTexture = [](SokolTexture2D *texture, sg_pixel_format pixel_format) {
        auto description = texture->desc;
        description->render_target = true;
        description->usage = SG_USAGE_IMMUTABLE;
        description->pixel_format = pixel_format;
        SokolResourceKey resource_key = get_sokol_resource_key_texture(
            description->width, description->height, description->pixel_format);
        texture->image = new SokolResourceTexture(resource_key, sg_make_image(description));
        texture->resource_key = texture->image->key;
        texture->dirty = false;
    };

    pShadowMap=GetTexLibrary()->CreateRenderTexture(xysize, xysize, TEXTURE_RENDER16, false);
    if (!pShadowMap) {
        DeleteShadowTexture();
        return false;
    }
    SetupTexture(pShadowMap->GetFrameImage(0)->sg, SG_PIXELFORMAT_DEPTH);

    pLightMap = GetTexLibrary()->CreateRenderTexture(256, 256, TEXTURE_RENDER32, false);
    if (!pLightMap) {
        DeleteShadowTexture();
        return false;
    }
    SetupTexture(pLightMap->GetFrameImage(0)->sg, SG_PIXELFORMAT_BGRA8);

    return true;
}

void cSokolRender::DeleteShadowTexture() {
    RELEASE(pShadowMap);
    RELEASE(pLightMap);
}

cTexture* cSokolRender::GetShadowMap() {
    return pShadowMap;
}

cTexture* cSokolRender::GetLightMap() {
    return pLightMap;
}
