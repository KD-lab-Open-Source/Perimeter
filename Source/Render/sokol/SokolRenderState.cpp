#include <string>
#include <vector>
#include "xmath.h"
#include "Umath.h"
#include "StdAfxRD.h"
#include "VertexFormat.h"
#include <sokol_gfx.h>
#include "SokolResources.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "SokolRenderPipeline.h"
#include "SokolShaders.h"
#include "xerrhand.h"
#include "Texture.h"
#include "DrawBuffer.h"
#include "RenderTracker.h"

#ifdef GPX
#include <c/gamepix.h>
#endif

int cSokolRender::BeginScene() {
    RenderSubmitEvent(RenderEvent::BEGIN_SCENE, ActiveScene ? "ActiveScene" : "");
    MTG();
    if (ActiveScene) {
        xassert(0);
        return 1;
    }
    ActiveScene = true;

    ResetViewport();

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
    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass_action.colors[0].store_action = SG_STOREACTION_STORE;
    pass_action.colors[0].clear_value = fill_color;
    pass_action.depth.load_action = SG_LOADACTION_CLEAR;
    pass_action.depth.store_action = SG_STOREACTION_DONTCARE;
    pass_action.depth.clear_value = 1.0f;
    pass_action.stencil.load_action = SG_LOADACTION_CLEAR;
    pass_action.stencil.store_action = SG_STOREACTION_DONTCARE;
    pass_action.stencil.clear_value = 0;
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
        auto& viewportPos = command->viewport[0];
        auto& viewportSize = command->viewport[1];
        sg_apply_viewport(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y, true);
        auto& clipPos = command->clip[0];
        auto& clipSize = command->clip[1];
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
        bindings.fs.samplers[pipeline->shader_fs_sampler_slot] = sampler;
        
        //Bind images
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
            bindings.fs.images[fs_slot] = tex->image;
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
                fs_params_name = "normal_texture_fs_params";
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

    ResetViewport();

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
    MT_IS_GRAPH();
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

#ifdef GPX
    gpx()->sys()->frameReady();
#endif

    return 0;
}

SokolBuffer* CreateSokolBuffer(MemoryResource* resource, size_t len, bool dynamic, sg_buffer_type type) {
    MT_IS_GRAPH();
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
    MT_IS_GRAPH();
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
    MT_IS_GRAPH();
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
    memcpy(cmd->viewport, activeCommand.viewport, sizeof(Vect2i) * 2);
    memcpy(cmd->clip, activeCommand.clip, sizeof(Vect2i) * 2);
    
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
            vs_params->tex0_mat = activeTex0Transform;
            vs_params->tex1_mat = activeTex1Transform;
            fs_params->un_color_mode = activeCommandColorMode;
            fs_params->un_tex2_lerp = activeCommandTex2Lerp;
            break;
        }
        case SOKOL_SHADER_ID_normal: {
            auto vs_params = reinterpret_cast<normal_texture_vs_params_t*>(cmd->vs_params);
            auto fs_params = reinterpret_cast<normal_texture_fs_params_t*>(cmd->fs_params);
            shader_set_common_params(vs_params, fs_params);
            vs_params->model = isOrthographicProjSet ? Mat4f::ID : activeCommandW;
            vs_params->tex0_mat = activeTex0Transform;
            fs_params->material = activeGlobalLight ? activeMaterial : SOKOL_MAT_NONE;
            memcpy(fs_params->diffuse, &activeDiffuse, sizeof(float) * 4);
            memcpy(fs_params->ambient, &activeAmbient, sizeof(float) * 4);
            memcpy(fs_params->specular, &activeSpecular, sizeof(float) * 4);
            memcpy(fs_params->emissive, &activeEmissive, sizeof(float) * 4);
            fs_params->spec_power = activePower;
            memcpy(fs_params->light_ambient, &activeLightAmbient, sizeof(float) * 3);
            memcpy(fs_params->light_diffuse, &activeLightDiffuse, sizeof(float) * 3);
            memcpy(fs_params->light_specular, &activeLightSpecular, sizeof(float) * 3);
            memcpy(fs_params->light_dir, &activeLightDir, sizeof(float) * 3);
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
    activeTex0Transform = Mat4f::ID;
    activeTex1Transform = Mat4f::ID;
    
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
    MT_IS_GRAPH();
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
    ResetViewport();
}

void cSokolRender::SetColorMode(eColorMode color_mode) {
    if (activeCommandColorMode != color_mode) {
        FinishActiveDrawBuffer();
        activeCommandColorMode = color_mode;
    }
}

void cSokolRender::SetMaterial(SOKOL_MATERIAL_TYPE material, const sColor4f& diffuse, const sColor4f& ambient,
                               const sColor4f& specular, const sColor4f& emissive, float power) {
    if (activeMaterial != material ||
        activeDiffuse != diffuse ||
        activeAmbient != ambient ||
        activeSpecular != specular ||
        activeEmissive != emissive ||
        activePower != power) {
        FinishActiveDrawBuffer();
        activeMaterial = material;
        activeDiffuse = diffuse;
        activeAmbient = ambient;
        activeSpecular = specular;
        activeEmissive = emissive;
        activePower = power;
    }
}

void cSokolRender::setTexture0Transform(const Mat4f& tex0Transform) {
    if (!activeTex0Transform.eq(tex0Transform, 0)) {
        FinishActiveDrawBuffer();
        activeTex0Transform = tex0Transform;
    }
}

void cSokolRender::setTexture1Transform(const Mat4f& tex1Transform) {
    if (!activeTex1Transform.eq(tex1Transform, 0)) {
        FinishActiveDrawBuffer();
        activeTex1Transform = tex1Transform;
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

void cSokolRender::SetRenderTarget(cTexture* target, SurfaceImage zbuffer) {
    //TODO
}

void cSokolRender::RestoreRenderTarget() {
    //TODO
}

void cSokolRender::SetDrawNode(cCamera *pDrawNode)
{
    if (DrawNode==pDrawNode) return;
    cInterfaceRenderDevice::SetDrawNode(pDrawNode);
    if (DrawNode->GetRenderTarget()) {
        SurfaceImage zbuffer = DrawNode->GetZBuffer();
        SetRenderTarget(DrawNode->GetRenderTarget(), zbuffer);
        uint32_t color=0;
        /* TODO
        if(pDrawNode->GetAttribute(ATTRCAMERA_SHADOW))
        {
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
            if(!zbuffer->sg)
            {
                RDCALL(lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,color,1,0));
            }else
            {
                RDCALL(lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color,
                                          pDrawNode->GetAttribute(ATTRCAMERA_ZINVERT)?0:1, 0));
            }
        }
        */
    }
    else
    {
        RestoreRenderTarget();
    }

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
    activeCommand.viewport[0].set(pDrawNode->vp.X, pDrawNode->vp.Y);
    activeCommand.viewport[1].set(pDrawNode->vp.Width, pDrawNode->vp.Height);
    SetVPMatrix(&pDrawNode->matViewProj);
    activePipelineMode.cull = pDrawNode->GetAttribute(ATTRCAMERA_REFLECTION) == 0 ? CULL_CW : CULL_CCW;
    CameraCullMode = activePipelineMode.cull;
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
        default:
            return 0;
    }
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
        default:
            //Unknown
            break;
    }
    return 0;
}


void cSokolRender::SetGlobalFog(const sColor4f &color,const Vect2f &v) {
    //TODO implement this
}

void cSokolRender::SetGlobalLight(Vect3f *vLight, sColor4f *Ambient, sColor4f *Diffuse, sColor4f *Specular) {
    bool globalLight =  vLight != nullptr && Ambient != nullptr && Diffuse != nullptr && Specular != nullptr;
    if (activeGlobalLight != globalLight) {
        FinishActiveDrawBuffer();
        activeGlobalLight = globalLight;
    }

    if (!activeGlobalLight) {
        return;
    }

    if (activeLightDir != *vLight ||
        activeLightDiffuse != *Diffuse) {
        FinishActiveDrawBuffer();
        activeLightDir = *vLight;
        activeLightDiffuse = *Diffuse;
        activeLightAmbient = *Ambient;
        activeLightSpecular = *Specular;
    }
}

void cSokolRender::ClearZBuffer() {
}
