#include <vector>
#include "xmath.h"
#include "Umath.h"
#include "StdAfxRD.h"
#include "VertexFormat.h"
#include "sokol_gfx.h"
#include "SokolResources.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "SokolRenderPipeline.h"
#include "xerrhand.h"
#include "SokolShaders.h"
#include "VertexFormat.h"
#include "Texture.h"

const int PERIMETER_SOKOL_POLYGON_COUNT = 10240;
const int PERIMETER_SOKOL_VERTEXES_COUNT = PERIMETER_SOKOL_POLYGON_COUNT * 4;
static_assert(PERIMETER_SOKOL_VERTEXES_COUNT <= std::numeric_limits<indices_t>::max());

int cSokolRender::BeginScene() {
    if (ActiveScene) {
        xassert(0);
        return 1;
    }
    ActiveScene = true;

    return 1;
}

int cSokolRender::EndScene() {
    if (!ActiveScene) {
        xassert(0);
        return 1;
    }
    ActiveScene = false;

    FinishCommand();

    //Begin pass
    sg_pass_action pass_action = {};
    pass_action.colors[0].action = SG_ACTION_CLEAR;
    pass_action.colors[0].value = {
            fill_color.r,
            fill_color.g,
            fill_color.b,
            fill_color.a
    };
    sg_begin_default_pass(&pass_action, ScreenSize.x, ScreenSize.y);
    
    sg_apply_viewport(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y, true);
    sg_apply_scissor_rect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y, true);

    //Iterate each command
    for (const auto& command : commands) {        
        //Nothing to draw
        if (!command->vertices) {
            continue;
        }
        
        //Get pipeline
        const SokolPipeline* pipeline = pipelines[command->pipeline_id];
        if (pipeline == nullptr) {
            //Not implemented vertex format
            xxassert(0, "cSokolRender::EndScene missing pipeline for " + std::to_string(command->pipeline_id));
            continue;
        }
        shader_funcs* shader = pipeline->shader_funcs;

        //Apply pipeline
        sg_apply_pipeline(pipeline->pipeline);
        
        //Apply bindings
        sg_bindings bindings = {};
        
        //Bind vertex and index buffer, ensure they are updated
        if (!command->vertex_buffer) {
            xxassert(0, "cSokolRender::EndScene missing vertex_buffer");
            continue;
        }
        if (!command->index_buffer) {
            xxassert(0, "cSokolRender::EndScene missing index_buffer");
            continue;
        }
        command->vertex_buffer->update(command->vertices * command->vertex_size);
        bindings.vertex_buffers[0] = command->vertex_buffer->buffer;
        command->index_buffer->update(command->indices * sizeof(indices_t));
        bindings.index_buffer = command->index_buffer->buffer;
        
        //Bind images for samplers
        int slot_tex0 = shader->image_slot(SG_SHADERSTAGE_FS, "un_tex0");
        if (0 <= slot_tex0) {
            SokolTexture2D* tex = emptyTexture;
            if (pipeline->vertex_fmt & VERTEX_FMT_TEX1) {
                tex = command->textures[0];
            }
            if (tex) {
                if (tex->data) tex->update();
                bindings.fs_images[slot_tex0] = tex->image;
            } else {
                xxassert(0, "cSokolRender::EndScene missing texture0");
                continue;
            }
        }
        if (pipeline->vertex_fmt & VERTEX_FMT_TEX2) {
            SokolTexture2D* tex = command->textures[1];
            xassert(tex);
            if (tex) {
                if (tex->data) tex->update();
                int slot = shader->image_slot(SG_SHADERSTAGE_FS, "un_tex1");
                bindings.fs_images[slot] = tex->image;
            } else {
                xxassert(0, "cSokolRender::EndScene missing texture1");
                continue;
            }
        }
        sg_apply_bindings(&bindings);
        
        //Apply VS uniforms
        int vs_params_slot = shader->uniformblock_slot(SG_SHADERSTAGE_VS, "vs_params");
        if (0 <= vs_params_slot) {
            vs_params_t vs_params;
            //Camera/projection
            if (command->vs_mvp) {
                vs_params.un_mvp = *command->vs_mvp;
            } else if (DrawNode) {
                vs_params.un_mvp = DrawNode->matViewProj;
            } else {
                xxassert(0, "cSokolRender::EndScene missing mvp");
                continue;
            }
            sg_range vs_params_range = SG_RANGE(vs_params);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, vs_params_slot, &vs_params_range);
        }

        //Apply FS uniforms, currently only for shader with 2 texures
        if (pipeline->vertex_fmt & VERTEX_FMT_TEX2) {
            int fs_params_slot = shader->uniformblock_slot(SG_SHADERSTAGE_FS, "fs_params");
            if (0 <= fs_params_slot) {
                fs_params_t fs_params;
                fs_params.un_mode = command->fs_mode;
                sg_range fs_params_range = SG_RANGE(fs_params);
                sg_apply_uniforms(SG_SHADERSTAGE_FS, fs_params_slot, &fs_params_range);
            }
        }

        //Draw
        sg_draw(0, static_cast<int>(command->indices), 1);
    }

    //End pass
    sg_end_pass();

    ClearCommands();

    return 1;
}

int cSokolRender::Fill(int r, int g, int b, int a) {
    if (ActiveScene) {
        xassert(0);
        EndScene();
    }

    fill_color.r = r / 255.f;
    fill_color.g = g / 255.f;
    fill_color.b = b / 255.f;
    fill_color.a = a / 255.f;

    return 0;
}

int cSokolRender::Flush(bool wnd) {
    if (ActiveScene) {
        xassert(0);
        EndScene();
    }

    sg_commit();

    SDL_GL_SwapWindow(sdlWindow);

    return 0;
}

void cSokolRender::FinishCommand() {
    if (!activeCommand.indices) {
        return;
    }
    
    //Create command to be send
    SokolCommand* cmd = new SokolCommand();
    cmd->pipeline_id = GetPipelineID(
        activePipelineType,
        vertexBuffer.fmt,
        activePipelineBlend,
        activePipelineCull,
        activePipelineFaceCCW
    );
    cmd->vertices = activeCommand.vertices;
    cmd->indices = activeCommand.indices;
    cmd->vertex_size = vertexBuffer.VertexSize;
    memcpy(cmd->textures, activeCommand.textures, PERIMETER_SOKOL_TEXTURES * sizeof(SokolTexture2D*));
    cmd->owned_mvp = activeCommand.owned_mvp;
    cmd->vs_mvp = activeCommand.vs_mvp;
    cmd->fs_mode = activeCommand.fs_mode;
    
    //Transfer buffers to command
    cmd->owned_buffers = true;
    cmd->vertex_buffer = vertexBuffer.sg;
    cmd->index_buffer = indexBuffer.sg;
    vertexBuffer.sg = nullptr;
    indexBuffer.sg = nullptr;
    
    //Submit command and recreate active command 
    commands.emplace_back(cmd);
    
    activeCommand.Clear();
    CreateVertexBuffer(vertexBuffer, PERIMETER_SOKOL_VERTEXES_COUNT, vertexBuffer.fmt, true);
    CreateIndexBuffer(indexBuffer, PERIMETER_SOKOL_POLYGON_COUNT);
}

void cSokolRender::PrepareBuffers(size_t NumberVertex, size_t NumberIndices, vertex_fmt_t vertex_fmt) {
    if (indexBuffer.sg && vertexBuffer.sg
    && vertexBuffer.fmt == vertex_fmt
    && NumberVertex + activeCommand.vertices < vertexBuffer.NumberVertex
    && NumberIndices + activeCommand.indices < indexBuffer.NumberIndices) {
        //We can use current buffers
        return;
    }

    FinishCommand();
    
    //Recreate buffers
    if (!vertexBuffer.sg || vertexBuffer.fmt != vertex_fmt) {
        DeleteVertexBuffer(vertexBuffer);
        CreateVertexBuffer(vertexBuffer, PERIMETER_SOKOL_VERTEXES_COUNT, vertex_fmt, true);
    }
    if (!indexBuffer.sg) {
        CreateIndexBuffer(indexBuffer, PERIMETER_SOKOL_POLYGON_COUNT);
    }
}

void cSokolRender::SetVSUniformMatrix(Mat4f* mat, bool command_owned) {
    if (activeCommand.vs_mvp != mat) {
        FinishCommand();
    }
    activeCommand.owned_mvp = command_owned;
    activeCommand.vs_mvp = mat;
}

void cSokolRender::SetFSUniformMode(int mode) {
    if (activeCommand.fs_mode != mode) {
        FinishCommand();
    }
    activeCommand.fs_mode = mode;
}

void cSokolRender::SetNoMaterial(eBlendMode blend, float Phase, cTexture* Texture0, cTexture* Texture1,
                                 eColorMode color_mode) {
    SokolTexture2D* tex0 = emptyTexture;
    SokolTexture2D* tex1 = nullptr; 
    if (Texture0) {
        if (blend == ALPHA_NONE && Texture0->IsAlphaTest()) {
            blend = ALPHA_TEST;
        } 
        if (blend <= ALPHA_TEST && Texture0->IsAlpha()) {
            blend = ALPHA_BLEND;
        }
        int nAllFrame = Texture0->GetNumberFrame();
        int nFrame = 1 < nAllFrame ? static_cast<int>(0.999f * Phase * nAllFrame) : 0;
        tex0 = Texture0->GetFrameImage(nFrame).sg;
    }
    if (Texture1) {
        int nAllFrame = Texture1->GetNumberFrame();
        int nFrame = 1 < nAllFrame ? static_cast<int>(0.999f * Phase * nAllFrame) : 0;
        tex1 = Texture1->GetFrameImage(nFrame).sg;
    }
    
    if (activePipelineBlend != blend
     || tex0 != activeCommand.textures[0]
     || tex1 != activeCommand.textures[1]) {
        FinishCommand();
        activePipelineBlend = blend;
        activeCommand.textures[0] = tex0;
        activeCommand.textures[1] = tex1;
    }
}


void cSokolRender::SetDrawNode(cCamera *pDrawNode)
{
    if (DrawNode==pDrawNode) return;
    cInterfaceRenderDevice::SetDrawNode(pDrawNode);
    /* TODO
    if (DrawNode->GetRenderTarget()) {
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
    */

    SetDrawTransform(pDrawNode);
}

void cSokolRender::SetDrawTransform(class cCamera *pDrawNode)
{
    viewportPos.x = pDrawNode->vp.X;
    viewportPos.y = pDrawNode->vp.Y;
    viewportSize.x = pDrawNode->vp.Width;
    viewportSize.y = pDrawNode->vp.Height;
    bool face_ccw = pDrawNode->GetAttribute(ATTRCAMERA_REFLECTION) != 0;
    if (!activePipelineCull || activePipelineFaceCCW != face_ccw) {
        FinishCommand();
        activePipelineCull = true;
        activePipelineFaceCCW = face_ccw;
    }
}

