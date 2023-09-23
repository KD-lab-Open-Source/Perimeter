#include <string>
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
#include "SokolShaders.h"
#include "xerrhand.h"
#include "Texture.h"
#include "DrawBuffer.h"
#include "RenderTracker.h"

int cSokolRender::BeginScene() {
    RenderSubmitEvent(RenderEvent::BEGIN_SCENE, ActiveScene ? "ActiveScene" : "");
    MTG();
    if (ActiveScene) {
        xassert(0);
        return 1;
    }
    ActiveScene = true;

    return cInterfaceRenderDevice::BeginScene();
}

int cSokolRender::EndScene() {
    RenderSubmitEvent(RenderEvent::END_SCENE, ActiveScene ? "" : "NotActiveScene");
    MTG();
    if (!ActiveScene) {
        xassert(0);
        return 1;
    }

    //Make sure there is nothing left to send as command
    FinishActiveDrawBuffer();
    
    ActiveScene = false;

    //Begin pass
    sg_pass_action pass_action = {};
    pass_action.colors[0].action = SG_ACTION_CLEAR;
    pass_action.colors[0].value = fill_color;
    pass_action.depth.action = SG_ACTION_CLEAR;
    pass_action.depth.value = 1.0f;
    pass_action.stencil.action = SG_ACTION_CLEAR;
    pass_action.stencil.value = 0;
    sg_begin_default_pass(&pass_action, ScreenSize.x, ScreenSize.y);

    //Iterate each command
    for (auto& command : commands) {
#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
        RenderSubmitEvent(RenderEvent::PROCESS_COMMAND, "", command);
#endif
        //Nothing to draw
        if (3 > command->vertices) {
            xassert(0);
            continue;
        }

        //Apply viewport/clip
        auto& clipPos = command->clipPos;
        auto& clipSize = command->clipSize;
        sg_apply_scissor_rect(clipPos.x, clipPos.y, clipSize.x, clipSize.y, true);
        
        //Get pipeline
        const SokolPipeline* pipeline = pipelines.count(command->pipeline_id) ? pipelines[command->pipeline_id] : nullptr;
        if (pipeline == nullptr) {
            //Not implemented vertex format
            xxassert(0, "cSokolRender::EndScene missing pipeline for " + std::to_string(command->pipeline_id));
            continue;
        }
#if defined(PERIMETER_DEBUG) && 0
        printf("id: 0x%X fmt: 0x%X vtx: %d idx: %d\n",
               command->pipeline_id, pipeline->vertex_fmt,
               command->vertices, command->indices
        );
        printf("tex0 0x%X 0x%X tex1 0x%X 0x%X\n",
               command->textures[0], command->textures[0]?command->textures[0]->image.id:0,
               command->textures[1], command->textures[1]?command->textures[1]->image.id:0
        );
#endif
        shader_funcs* shader_funcs = pipeline->shader_funcs;
        
        //Check amount is correct
        switch (pipeline->type) {
            case PIPELINE_TYPE_TRIANGLE:
                xassert(command->indices % 3 == 0);
                break;
#ifdef PERIMETER_DEBUG
            case PIPELINE_TYPE_LINE_STRIP:
#endif
            case PIPELINE_TYPE_TRIANGLESTRIP:
                break;
            case PIPELINE_TYPE_TERRAIN:
            case PIPELINE_TYPE_MAX:
                xassert(0);
                break;
        }

        //Apply pipeline
        if (sg_query_pipeline_state(pipeline->pipeline) != SG_RESOURCESTATE_VALID) {
            xxassert(0, "cSokolRender::EndScene not valid state");
            continue;
        }
        sg_apply_pipeline(pipeline->pipeline);
        
        //Apply bindings
        sg_bindings bindings = {};
        
        //Bind vertex and index buffer, ensure they are updated
        if (!command->vertex_buffer) {
            xxassert(0, "cSokolRender::EndScene missing vertex_buffer");
            continue;
        }
#ifdef PERIMETER_DEBUG
        if (sg_query_buffer_state(command->vertex_buffer->buffer) != SG_RESOURCESTATE_VALID) {
            xxassert(0, "cSokolRender::EndScene vertex_buffer not valid state");
            continue;
        }
#endif
        bindings.vertex_buffers[0] = command->vertex_buffer->buffer;
        xassert(command->indices);
        if (!command->index_buffer) {
            xxassert(0, "cSokolRender::EndScene missing index_buffer");
            continue;
        }
#ifdef PERIMETER_DEBUG
        if (sg_query_buffer_state(command->index_buffer->buffer) != SG_RESOURCESTATE_VALID) {
            xxassert(0, "cSokolRender::EndScene index_buffer not valid state");
            continue;
        }
#endif
        bindings.index_buffer = command->index_buffer->buffer;
        
        //Bind images for samplers
        for (int i = 0; i < PERIMETER_SOKOL_TEXTURES; ++i) {
            int fs_slot = pipeline->shader_fs_texture_slot[i];
            if (fs_slot < 0) continue;
            SokolTexture2D* tex = command->sokol_textures[i];
            if (!tex) {
                tex = emptyTexture;
                if (!tex) {
                    xxassert(0, "cSokolRender::EndScene sampler tex missing");
                    continue;
                }
            }
            if (tex->dirty) {
                tex->update();
            }
#ifdef PERIMETER_DEBUG
            if (sg_query_image_state(tex->image) != SG_RESOURCESTATE_VALID) {
                xxassert(0, "cSokolRender::EndScene sampler tex not valid state");
                continue;
            }
#endif
            bindings.fs_images[fs_slot] = tex->image;
        }
        sg_apply_bindings(&bindings);
        
        //Apply VS uniforms
        SOKOL_SHADER_ID shader_id = shader_funcs->get_id();
        const char* vs_params_name = nullptr;
        const char* fs_params_name = nullptr;
        switch (shader_id) {
            case SOKOL_SHADER_ID_color_tex1:
            case SOKOL_SHADER_ID_color_tex2:
                vs_params_name = "color_texture_vs_params";
                fs_params_name = "color_texture_fs_params";
                break;
            case SOKOL_SHADER_ID_normal:
                vs_params_name = "normal_texture_vs_params";
                break;
            case SOKOL_SHADER_ID_terrain:
                vs_params_name = "terrain_vs_params";
                fs_params_name = "terrain_fs_params";
                break;
            default:
            case SOKOL_SHADER_ID_NONE:
                xassert(0);
        }

        if (vs_params_name) {
            int vs_params_slot = shader_funcs->uniformblock_slot(SG_SHADERSTAGE_VS, vs_params_name);
            xxassert(0 <= vs_params_slot, "No vs slot found");
            xxassert(command->vs_params, "No vs parameters set in command");
            sg_apply_uniforms(SG_SHADERSTAGE_VS, vs_params_slot, sg_range { command->vs_params, command->vs_params_len });
        }
        if (fs_params_name) {
            int fs_params_slot = shader_funcs->uniformblock_slot(SG_SHADERSTAGE_FS, fs_params_name);
            xxassert(0 <= fs_params_slot, "No fs slot found");
            xxassert(command->fs_params, "No fs parameters set in command");
            sg_apply_uniforms(SG_SHADERSTAGE_FS, fs_params_slot, sg_range { command->fs_params, command->fs_params_len });
        }

        //Draw
        sg_draw(static_cast<int>(command->base_elements), static_cast<int>(command->indices), 1);
    }

    //End pass
    sg_end_pass();

    return cInterfaceRenderDevice::EndScene();
}

int cSokolRender::Fill(int r, int g, int b, int a) {
    RenderSubmitEvent(RenderEvent::FILL);
    if (ActiveScene) {
        xassert(0);
        EndScene();
    }

#ifdef PERIMETER_DEBUG
    if (r == 0 && g == 0 && b == 0) {
        r = 64;
        g = b = 32;
    }
#endif

    fill_color.r = static_cast<float>(r) / 255.f;
    fill_color.g = static_cast<float>(g) / 255.f;
    fill_color.b = static_cast<float>(b) / 255.f;
    fill_color.a = static_cast<float>(a) / 255.f;

    return 0;
}

int cSokolRender::Flush(bool wnd) {
    RenderSubmitEvent(RenderEvent::FLUSH_SCENE);
    if (!sdl_window) {
        xassert(0);
        return -1;
    }
    if (ActiveScene) {
        xassert(0);
        EndScene();
    }

    sg_commit();

    SDL_GL_SwapWindow(sdl_window);

    ClearCommands();

    xassert(!activeDrawBuffer || !activeDrawBuffer->written_vertices);

    return 0;
}

SokolBuffer* CreateSokolBuffer(MemoryResource* resource, size_t len, bool dynamic, sg_buffer_type type) {
    xassert(!resource->locked);
    xassert(len <= resource->data_len);
    sg_buffer_desc desc = {};
    desc.size = len;
    desc.type = type;
    desc.usage = dynamic ? SG_USAGE_STREAM : SG_USAGE_IMMUTABLE;
    if (type == SG_BUFFERTYPE_VERTEXBUFFER) {
        desc.label = "CreateVertexBuffer";
    } else if (type == SG_BUFFERTYPE_INDEXBUFFER) {
        desc.label = "CreateIndexBuffer";
    } else {
        desc.label = "CreateSokolBuffer";
    }
    if (desc.usage == SG_USAGE_IMMUTABLE) {
        xassert(resource->data);
        xassert(!resource->burned);
        desc.data = {resource->data, len};
        resource->burned = true;
        resource->dirty = false;
    } else {
        resource->dirty = true;
    }

    SokolBuffer* buffer = new SokolBuffer(&desc);
    
    return buffer;
}

void cSokolRender::FinishActiveDrawBuffer() {
    if (!activeDrawBuffer || !activeDrawBuffer->written_vertices) {
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
        RenderSubmitEvent(RenderEvent::FINISH_ACTIVE_DRAW_BUFFER, "No/Empty", activeDrawBuffer);
#endif
        return;
    }
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
    RenderSubmitEvent(RenderEvent::FINISH_ACTIVE_DRAW_BUFFER, "Finish", activeDrawBuffer);
#endif
    xassert(!activeDrawBuffer->IsLocked());

    if (0 == activeCommand.vertices && 0 == activeCommand.indices) {
        activeCommand.base_elements = 0;
        activeCommand.vertices = activeDrawBuffer->written_vertices;
        activeCommand.indices = activeDrawBuffer->written_indices;
    }
    
    CreateCommand(&activeDrawBuffer->vb, activeDrawBuffer->written_vertices, &activeDrawBuffer->ib, activeDrawBuffer->written_indices);

    activeDrawBuffer->PostDraw();
    activeDrawBuffer = nullptr;
}

void cSokolRender::CreateCommand(VertexBuffer* vb, size_t vertices, IndexBuffer* ib, size_t indices) {
    if (0 == vertices) vertices = activeCommand.vertices;
    if (0 == indices) indices = activeCommand.indices;
    PIPELINE_TYPE pipelineType = activePipelineType;
#ifdef PERIMETER_DEBUG
    if (WireframeMode) pipelineType = PIPELINE_TYPE_LINE_STRIP;
#endif
    
    pipeline_id_t pipeline_id = GetPipelineID(
            pipelineType,
            vb->fmt,
            activePipelineMode
    );
    if (pipelines.count(pipeline_id) == 0) {
        RegisterPipeline(pipeline_id);
    }
    const SokolPipeline* pipeline = pipelines.count(pipeline_id) ? pipelines[pipeline_id] : nullptr;
    if (!pipeline) {
        xxassert(0, "CreateCommand: No pipeline found");
        return;
    }

#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
    std::string label = "Pipeline: " + std::to_string(pipeline_id);
    RenderSubmitEvent(RenderEvent::CREATE_COMMAND, label.c_str());
#endif

#ifdef PERIMETER_DEBUG
    if (vb->fmt & VERTEX_FMT_TEX1) {
        if (activeCommand.sokol_textures[0] != emptyTexture) {
            xassert(activeCommand.sokol_textures[0]);
        }
        
    }
    if (vb->fmt & VERTEX_FMT_TEX2) {
        xassert(activeCommand.sokol_textures[1]);
    }
#endif

    //Update buffers
    if (vb->data && (vb->sg == nullptr || vb->dirty)) {
        size_t len = vertices * vb->VertexSize;
        if (vb->sg == nullptr) {
            vb->sg = CreateSokolBuffer(vb, len, vb->dynamic, SG_BUFFERTYPE_VERTEXBUFFER);
        }
        if (vb->dynamic) {
            vb->sg->update(vb, len);
        }
    }
    if (ib->data && (!ib->sg || ib->dirty)) {
        size_t len = indices * sizeof(indices_t);
        if (ib->sg == nullptr) {
            ib->sg = CreateSokolBuffer(ib, len, ib->dynamic, SG_BUFFERTYPE_INDEXBUFFER);
        }
        if (ib->dynamic) {
            ib->sg->update(ib, len);
        }
    }
    xassert(activeCommand.vertices <= vertices);
    xassert((activeCommand.base_elements + activeCommand.indices) <= indices);
    
    //Create command to be send
    SokolCommand* cmd = new SokolCommand();
    cmd->pipeline_id = pipeline_id;
    cmd->shader_id = pipeline->shader_id;
    for (int i = 0; i < PERIMETER_SOKOL_TEXTURES; ++i) {
        cmd->SetTexture(i, activeCommand.sokol_textures[i]);
    }
    cmd->base_elements = activeCommand.base_elements;
    cmd->vertices = activeCommand.vertices;
    cmd->indices = activeCommand.indices;
    cmd->clipPos = activeCommand.clipPos;
    cmd->clipSize = activeCommand.clipSize;
    
    //Set shader params
    cmd->CreateShaderParams();
    switch (cmd->shader_id) {
        default:
        case SOKOL_SHADER_ID_NONE:
            xassert(0);
            break;
        case SOKOL_SHADER_ID_color_tex1:
        case SOKOL_SHADER_ID_color_tex2: {
            auto vs_params = reinterpret_cast<color_texture_vs_params_t*>(cmd->vs_params);
            auto fs_params = reinterpret_cast<color_texture_fs_params_t*>(cmd->fs_params);
            shader_set_common_params(vs_params, fs_params);
            fs_params->un_color_mode = activeCommandColorMode;
            fs_params->un_tex2_lerp = activeCommandTex2Lerp;
            break;
        }
        case SOKOL_SHADER_ID_normal: {
            auto vs_params = reinterpret_cast<normal_texture_vs_params_t*>(cmd->vs_params);
            auto fs_params = reinterpret_cast<normal_texture_fs_params_t*>(cmd->fs_params);
            shader_set_common_params(vs_params, fs_params);
            break;
        }
        case SOKOL_SHADER_ID_terrain: {
            auto vs_params = reinterpret_cast<terrain_vs_params_t*>(cmd->vs_params);
            auto fs_params = reinterpret_cast<terrain_fs_params_t*>(cmd->fs_params);
            shader_set_common_params(vs_params, fs_params);
            fs_params->un_tile_color = activeCommandTileColor.v;
            break;
        }
    }
    
    //Transfer buffers to command
    cmd->owned_vertex_buffer = vb->dynamic;
    cmd->owned_index_buffer = ib->dynamic;
    cmd->vertex_buffer = vb->sg;
    cmd->index_buffer = ib->sg;
    if (cmd->owned_vertex_buffer) {
        vb->sg = nullptr;
        vb->burned = false;
    }
    if (cmd->owned_index_buffer) {
        ib->sg = nullptr;
        ib->burned = false;
    }
    activeCommand.base_elements = 0;
    activeCommand.vertices = 0;
    activeCommand.indices = 0;
    
    //Submit command
    commands.emplace_back(cmd);

#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
    label = "Submit - Pipeline: " + std::to_string(pipeline_id)
            + " ColM: " + std::to_string(activeCommand.fs_color_mode)
            + " OwVB: " + std::to_string(cmd->owned_vertex_buffer)
            + " OwIB: " + std::to_string(cmd->owned_index_buffer)
            + " Vtxs: " + std::to_string(cmd->vertices)
            + " Idxs: " + std::to_string(cmd->indices)
            + " Tex0: " + std::to_string(reinterpret_cast<size_t>(cmd->sokol_textures[0]))
            + " Tex1: " + std::to_string(reinterpret_cast<size_t>(cmd->sokol_textures[1]));
    RenderSubmitEvent(RenderEvent::CREATE_COMMAND, label.c_str(), cmd);
#endif
}

void cSokolRender::SetActiveDrawBuffer(DrawBuffer* db) {
    if (activeDrawBuffer && activeDrawBuffer != db) {
        //Submit previous buffer first
        if (activeDrawBuffer->IsLocked()) {
            activeDrawBuffer->Unlock();
        }
        FinishActiveDrawBuffer();
    }
    cInterfaceRenderDevice::SetActiveDrawBuffer(db);
    if (activeDrawBuffer) {
        activePipelineType = getPipelineType(activeDrawBuffer->primitive);
    }
    activeCommand.base_elements = 0;
    activeCommand.vertices = 0;
    activeCommand.indices = 0;
}

void cSokolRender::SubmitDrawBuffer(DrawBuffer* db, DrawRange* range) {
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
    RenderSubmitEvent(RenderEvent::SUBMIT_DRAW_BUFFER, "", db);
#endif
    if (activeDrawBuffer != nullptr && activeDrawBuffer != db) {
        //We need to submit internal render buffer first
        FinishActiveDrawBuffer();
    }
    activePipelineType = getPipelineType(db->primitive);
    activeDrawBuffer = db;
    if (activeDrawBuffer) {
        activeCommand.vertices = activeDrawBuffer->written_vertices;
        if (range) {
            activeCommand.base_elements = range->offset;
            activeCommand.indices = range->len;
        } else {
            activeCommand.base_elements = 0;
            activeCommand.indices = activeDrawBuffer->written_indices;
        }
    }
    FinishActiveDrawBuffer();
}

void cSokolRender::SubmitBuffers(ePrimitiveType primitive, VertexBuffer* vb, size_t vertices, IndexBuffer* ib, size_t indices, DrawRange* range) {
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
    RenderSubmitEvent(RenderEvent::SUBMIT_DRAW_BUFFER, "", db);
#endif
    if (activeDrawBuffer != nullptr) {
        //We need to submit internal render buffer first
        FinishActiveDrawBuffer();
    }
    activePipelineType = getPipelineType(primitive);
    activeDrawBuffer = nullptr;
    activeCommand.base_elements = range ? range->offset : 0;
    activeCommand.vertices = vertices;
    activeCommand.indices = range ? range->len : indices;
    CreateCommand(vb, vertices, ib, indices);
}

void cSokolRender::SetVPMatrix(const Mat4f* matrix) {
#ifdef PERIMETER_RENDER_TRACKER_MATRIX
    RenderSubmitEvent(RenderEvent::SET_VIEWPROJ_MATRIX);
#endif
    if (!matrix) matrix = &Mat4f::ID;
    if (isOrthographicProjSet || !activeCommandVP.eq(*matrix, 0)) {
        FinishActiveDrawBuffer();
    }
    isOrthographicProjSet = false;
    activeCommandVP = *matrix;
}

void cSokolRender::SetWorldMat4f(const Mat4f* matrix) {
#ifdef PERIMETER_RENDER_TRACKER_MATRIX
    RenderSubmitEvent(RenderEvent::SET_WORLD_MATRIX);
#endif
    if (!matrix) matrix = &Mat4f::ID;
    if (isOrthographicProjSet || !activeCommandW.eq(*matrix, 0)) {
        FinishActiveDrawBuffer();
    }
    isOrthographicProjSet = false;
    activeCommandW = *matrix;
}

void cSokolRender::SetTex2Lerp(float lerp) {
    if (activeCommandTex2Lerp != lerp) {
        FinishActiveDrawBuffer();
    }
    activeCommandTex2Lerp = lerp;
}

void cSokolRender::UseOrthographicProjection() {
#ifdef PERIMETER_RENDER_TRACKER_MATRIX
    RenderSubmitEvent(RenderEvent::SET_VIEWPROJ_MATRIX, "Orthographic");
#endif
    if (!isOrthographicProjSet) {
        FinishActiveDrawBuffer();
    }
    isOrthographicProjSet = true;
}

void cSokolRender::SetColorMode(eColorMode color_mode) {
    if (activeCommandColorMode != color_mode) {
        FinishActiveDrawBuffer();
        activeCommandColorMode = color_mode;
    }
}

void cSokolRender::SetBlendState(eBlendMode blend) {
    if (activePipelineMode.blend != blend) {
        FinishActiveDrawBuffer();
        activePipelineMode.blend = blend;
    }
}

void cSokolRender::SetTextureImage(uint32_t slot, TextureImage* texture_image) {
    xassert(slot < GetMaxTextureSlots());
    SokolTexture2D* tex = texture_image ? texture_image->sg : nullptr;
    //Required as sometimes empty slot must be used with color_tex1 shader
    if (!tex && slot == 0) {
        tex = emptyTexture;
    }
    if (activeCommand.sokol_textures[slot] != tex) {
        FinishActiveDrawBuffer();
        activeCommand.SetTexture(slot, tex);
    }
}

uint32_t cSokolRender::GetMaxTextureSlots() {
    return PERIMETER_SOKOL_TEXTURES;
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
#ifdef PERIMETER_RENDER_TRACKER_MATRIX
    RenderSubmitEvent(RenderEvent::SET_VIEWPROJ_MATRIX);
#endif
    FinishActiveDrawBuffer();
    SetClipRect(
        pDrawNode->vp.X,
        pDrawNode->vp.Y,
        pDrawNode->vp.X + pDrawNode->vp.Width,
        pDrawNode->vp.Y + pDrawNode->vp.Height
    );
    SetVPMatrix(&pDrawNode->matViewProj);
    activePipelineMode.cull = pDrawNode->GetAttribute(ATTRCAMERA_REFLECTION) == 0 ? CULL_CW : CULL_CCW;
}

uint32_t cSokolRender::GetRenderState(eRenderStateOption option) {
    switch(option) {
        case RS_WIREFRAME:
            return WireframeMode;
        case RS_ZWRITEENABLE:
            return activePipelineMode.depth_write;
        case RS_CULLMODE:
            return activePipelineMode.cull;
        case RS_ALPHA_TEST_MODE:
            return activeCommandAlphaTest;
        case RS_BILINEAR:
            return 1;
    }
    return 0;
}

int cSokolRender::SetRenderState(eRenderStateOption option, uint32_t value) {
    VISASSERT(ActiveScene);
    switch(option) {
        case RS_ZWRITEENABLE: {
            if (value) {
                SetRenderState(RS_CULLMODE, CameraCullMode);
            } else {
                SetRenderState(RS_CULLMODE, CULL_NONE);
            }
            bool state = value != 0;
            if (state != activePipelineMode.depth_write) {
                FinishActiveDrawBuffer();
                activePipelineMode.depth_write = state;
            }
            break;
        }
        case RS_ZFUNC: {
            eCMPFUNC depth_cmp = static_cast<eCMPFUNC>(value);
            if (depth_cmp != activePipelineMode.depth_cmp) {
                FinishActiveDrawBuffer();
                activePipelineMode.depth_cmp = depth_cmp;
            }
            break;
        }
        case RS_WIREFRAME: {
            bool state = value != 0;
            if (state != WireframeMode) {
                FinishActiveDrawBuffer();
                WireframeMode = state;
            }
            break;
        }
        case RS_CULLMODE: {
            eCullMode cull = static_cast<eCullMode>(value);
            if (cull >= CULL_CAMERA) cull = CameraCullMode;
            if (cull != activePipelineMode.cull) {
                FinishActiveDrawBuffer();
                activePipelineMode.cull = cull;
            }
            break;
        }
        case RS_ALPHA_TEST_MODE:
            activeCommandAlphaTest = static_cast<eAlphaTestMode>(value);
            break;
        case RS_BILINEAR:
            //Useless as we can't change globally
            break;
    }
    return 0;
}


void cSokolRender::SetGlobalFog(const sColor4f &color,const Vect2f &v) {
    //TODO implement this
}

void cSokolRender::SetGlobalLight(Vect3f *vLight, sColor4f *Ambient, sColor4f *Diffuse, sColor4f *Specular) {
    //TODO implement this
}

void cSokolRender::ClearZBuffer() {
}
