#include <algorithm>
#include <array>
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
#include "RenderUtils.h"

#ifdef GPX
#include <c/gamepix.h>
#endif

#ifdef SOKOL_D3D11
#include "SokolD3D.h"
#endif

#ifdef SOKOL_METAL
void sokol_metal_render(sg_swapchain* swapchain, void (*callback)());
void sokol_metal_draw();

//According to sokol metal example the sokol render calls must be wrapped by @autoreleasepool
void sokol_metal_render_callback() {
    cSokolRender* sokolRender = reinterpret_cast<cSokolRender*>(gb_RenderDevice);
    sokolRender->DoSokolRendering();
}

#ifdef PERIMETER_DEBUG
void sokol_metal_capture_frame();
#endif

#endif

//How many frames to store the resources until freed
#ifndef GPX
const uint32_t MAX_POOLED_RESOURCES_LIFE = 10000;
#else
const uint32_t MAX_POOLED_RESOURCES_LIFE = 32000;
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
    ClearActiveBufferAndPassAction();
    xassert(activeDrawBuffer == nullptr);
    xassert(activeCommand.pass_action == nullptr);

    ActiveScene = false;

#ifdef SOKOL_METAL
    sokol_metal_render(&swapchain, &sokol_metal_render_callback);
#else
    DoSokolRendering();
#endif

    return cInterfaceRenderDevice::EndScene();
}

void cSokolRender::DoSokolRendering() {
#ifdef PERIMETER_DEBUG
    if (is_capturing_frame) {
        is_capturing_frame = false;
#ifdef SOKOL_METAL
        sokol_metal_capture_frame();
#endif
    }
#endif

    for (auto& target : { shadowMapRenderTarget, lightMapRenderTarget }) {
        if (target != nullptr) {
            ProcessRenderPass(target->render_pass, target->commands);
        }
    }

    //Create the pass, clear all buffers
    sg_pass swapchain_pass = {};
    swapchain_pass.label = "PassSwapchain";
    swapchain_pass.swapchain = swapchain;
    swapchain_pass.action.colors[0].load_action = SG_LOADACTION_CLEAR;
    swapchain_pass.action.colors[0].store_action = SG_STOREACTION_STORE;
    swapchain_pass.action.colors[0].clear_value = fill_color;
    swapchain_pass.action.depth.load_action = SG_LOADACTION_CLEAR;
    swapchain_pass.action.depth.store_action = SG_STOREACTION_DONTCARE;
    swapchain_pass.action.depth.clear_value = 1.0f;
    swapchain_pass.action.stencil.load_action = SG_LOADACTION_CLEAR;
    swapchain_pass.action.stencil.store_action = SG_STOREACTION_DONTCARE;
    swapchain_pass.action.stencil.clear_value = 0;

    ProcessRenderPass(swapchain_pass, commands);
}

void cSokolRender::ProcessRenderPass(sg_pass& render_pass, const std::vector<SokolCommand*>& pass_commands) {
    sg_begin_pass(&render_pass);

    //Iterate each command
    for (auto& command : pass_commands) {
#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
        RenderSubmitEvent(RenderEvent::PROCESS_COMMAND, "", command);
#endif
        //Change pass if set
        if (command->pass_action) {
            sg_end_pass();
            memcpy(&render_pass.action, command->pass_action, sizeof(sg_pass_action));
            sg_begin_pass(&render_pass);
        }

        //Nothing to draw
        if (3 > command->vertices) {
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
        const SokolPipeline* pipeline = command->pipeline;
        if (pipeline == nullptr) {
            //Not implemented vertex format
            xxassert(0, "cSokolRender::ProcessRenderPass missing pipeline");
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
        shader_funcs* shader_funcs = pipeline->context.shader_funcs;
        
        //Check amount is correct
        switch (pipeline->context.primitive_type) {
            case PT_TRIANGLES:
                xassert(command->indices % 3 == 0);
                break;
            case PT_TRIANGLESTRIP:
                break;
        }

        //Apply pipeline
        if (sg_query_pipeline_state(pipeline->pipeline) != SG_RESOURCESTATE_VALID) {
            xxassert(0, "cSokolRender::ProcessRenderPass not valid state");
            continue;
        }
        sg_apply_pipeline(pipeline->pipeline);
        
        //Apply bindings
        sg_bindings bindings = {};
        
        //Bind vertex and index buffer, ensure they are updated
        if (!command->vertex_buffer) {
            xxassert(0, "cSokolRender::ProcessRenderPass missing vertex_buffer");
            continue;
        }
#ifdef PERIMETER_DEBUG
        if (sg_query_buffer_state(command->vertex_buffer->res) != SG_RESOURCESTATE_VALID) {
            xxassert(0, "cSokolRender::ProcessRenderPass vertex_buffer not valid state");
            continue;
        }
#endif
        bindings.vertex_buffers[0] = command->vertex_buffer->res;
        xassert(command->indices);
        if (!command->index_buffer) {
            xxassert(0, "cSokolRender::ProcessRenderPass missing index_buffer");
            continue;
        }
#ifdef PERIMETER_DEBUG
        if (sg_query_buffer_state(command->index_buffer->res) != SG_RESOURCESTATE_VALID) {
            xxassert(0, "cSokolRender::ProcessRenderPass index_buffer not valid state");
            continue;
        }
#endif
        bindings.index_buffer = command->index_buffer->res;
        if (pipeline->shader_fs_sampler_slot != -1) {
            bindings.fs.samplers[pipeline->shader_fs_sampler_slot] = sampler;
        }
        if (pipeline->shader_fs_shadow_sampler_slot != -1) {
            bindings.fs.samplers[pipeline->shader_fs_shadow_sampler_slot] = shadow_sampler;
        }
        
        //Bind images
        for (int i = 0; i < PERIMETER_SOKOL_TEXTURES; ++i) {
            int fs_slot = pipeline->shader_fs_texture_slot[i];
            if (fs_slot < 0) continue;
            SokolResourceImage* image = command->sokol_images[i];
            if (!image) {
                image = emptyTexture->image;
            }
#ifdef PERIMETER_DEBUG
            if (sg_query_image_state(image->res) != SG_RESOURCESTATE_VALID) {
                xxassert(0, "cSokolRender::ProcessRenderPass sampler image not valid state");
                continue;
            }
#endif
            bindings.fs.images[fs_slot] = image->res;
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
            case SOKOL_SHADER_ID_object_shadow:
                vs_params_name = "object_shadow_vs_params";
                break;
            case SOKOL_SHADER_ID_only_texture:
                vs_params_name = "only_texture_vs_params";
                break;
            case SOKOL_SHADER_ID_tile_map:
                vs_params_name = "tile_map_vs_params";
                fs_params_name = "tile_map_fs_params";
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

    //Commit it
    sg_commit();

    //Swap the window
#ifdef PERIMETER_SOKOL_GL
    SDL_GL_SwapWindow(sdl_window);
#endif
#ifdef SOKOL_D3D11
    uint32_t sync_interval = RenderMode & RENDERDEVICE_MODE_VSYNC ? 1 : 0;
    d3d_context->swap_chain->Present(sync_interval, 0);
#endif
#ifdef SOKOL_METAL
    sokol_metal_draw();
#endif

    ClearPooledResources(MAX_POOLED_RESOURCES_LIFE);
    ClearAllCommands();

    xassert(!activeDrawBuffer || !activeDrawBuffer->written_vertices);

#ifdef GPX
    gpx()->sys()->frameReady();
#endif

    return 0;
}

#ifdef PERIMETER_DEBUG
void cSokolRender::StartCaptureFrame() {
    is_capturing_frame = true;
}
#endif

void cSokolRender::ClearActiveBufferAndPassAction() {
    if (activeDrawBuffer) {
        //Send out any active DB before we set a pass action
        //otherwise it might be renderer after setting new pass
        FinishActiveDrawBuffer();
    }
    if (activeCommand.pass_action) {
        //Active command has a pass action already and there was no active DB
        //Create a empty command
        CreateCommandEmpty();
        if (activeCommand.pass_action) {
            xassert(0);
            delete activeCommand.pass_action;
            activeCommand.pass_action = nullptr;
        }
    }
}

void cSokolRender::ClearZBuffer() {
    //Keep color and stencil buffers, clear depth buffer
    ClearActiveBufferAndPassAction();
    sg_pass_action* action = new sg_pass_action();
    action->colors[0].load_action = SG_LOADACTION_LOAD;
    action->colors[0].store_action = SG_STOREACTION_STORE;
    action->depth.load_action = SG_LOADACTION_CLEAR;
    action->depth.store_action = SG_STOREACTION_DONTCARE;
    action->depth.clear_value = 1.0f;
    action->stencil.load_action = SG_LOADACTION_LOAD;
    action->stencil.store_action = SG_STOREACTION_DONTCARE;
    activeCommand.pass_action = action;
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

void cSokolRender::PrepareSokolBuffer(SokolBuffer*& buffer_ptr, MemoryResource* resource, size_t len, bool dynamic, sg_buffer_type type) {
    MT_IS_GRAPH();
    xassert(!resource->locked);
    xassert(len <= resource->data_len);

    SokolResourceBuffer* buffer;
    if (dynamic) {
        SokolResourceKey resource_key = get_sokol_resource_key_buffer(len, type);
        auto nh = bufferPool.extract(resource_key);
        if (nh.empty()) {
            sg_buffer_desc desc = {};
            desc.size = len;
            desc.type = type;
            desc.usage = SG_USAGE_STREAM;
            if (type == SG_BUFFERTYPE_VERTEXBUFFER) {
                desc.label = "VertexBufferStream";
            } else if (type == SG_BUFFERTYPE_INDEXBUFFER) {
                desc.label = "IndexBufferStream";
            }

            sg_buffer sg_buffer = sg_make_buffer(&desc);
            buffer = new SokolResourceBuffer(
                resource_key,
                sg_buffer
            );
        } else {
            buffer = nh.mapped().resource;
            xassert(buffer->res.id != SG_INVALID_ID);
            xassert(buffer->pooled);
            buffer->burned = false;
            buffer->pooled = false;
        }

        resource->dirty = true;
    } else {
        sg_buffer_desc desc = {};
        desc.size = len;
        desc.type = type;
        desc.usage = SG_USAGE_IMMUTABLE;
        if (type == SG_BUFFERTYPE_VERTEXBUFFER) {
            desc.label = "VertexBufferImmutable";
        } else if (type == SG_BUFFERTYPE_INDEXBUFFER) {
            desc.label = "IndexBufferImmutable";
        }
        xassert(buffer_ptr == nullptr);
        xassert(resource->data);
        desc.data = {resource->data, len};
        resource->dirty = false;

        buffer = new SokolResourceBuffer(
            SokolResourceKeyNone,
            sg_make_buffer(&desc)
        );
        buffer->burned = true;
    }
    xassert(buffer != nullptr);

    if (buffer_ptr == nullptr) {
        buffer_ptr = new SokolBuffer(buffer);
    } else if (buffer_ptr->buffer == nullptr) {
        //Buffer exists but the resource no, recreate it
        buffer_ptr->buffer = buffer;
    } else {
        xassert(0);
    }
}

void cSokolRender::PrepareSokolTexture(SokolTexture2D* tex) {
    //Remove current burned image that is dirty
    if (tex->dirty && tex->resource_key != SokolResourceKeyNone && tex->image && tex->image->burned) {
        StorePooledResource(imagePool, tex->image);
        tex->image->DecRef();
        tex->image = nullptr;
    }

    //Setup image resource
    if (!tex->image) {
        if (!tex->desc) {
            xassert(tex->desc);
            return;
        }
        
        sg_image_desc*& desc = tex->desc;
        if (!tex->label.empty()) {
            desc->label = tex->label.c_str();
        }
        if (desc->usage == SG_USAGE_IMMUTABLE) {
            tex->resource_key = SokolResourceKeyNone;
        } else {
            xassert(tex->data);
            tex->resource_key = get_sokol_resource_key_texture(
                    desc->width,
                    desc->height,
                    desc->pixel_format
            );
        }
        
        //Get already created resource if exists or create
        auto nh = imagePool.extract(tex->resource_key);
        if (nh.empty()) {
            tex->image = new SokolResourceImage(
                tex->resource_key,
                sg_make_image(desc)
            );
        } else {
            tex->image = nh.mapped().resource;
            xassert(tex->image->res.id != SG_INVALID_ID);
            xassert(tex->image->pooled);
            tex->image->burned = false;
            tex->image->pooled = false;
        }

        if (desc->usage == SG_USAGE_IMMUTABLE) {
            tex->image->burned = true;
            
            //We no longer need desc or data as this is immutable
            tex->FreeImages();
            tex->FreeData();

            delete desc;
            desc = nullptr;
        } else {
            tex->dirty = true;
        }        
    }
    
    //Update the texture if is not immutable
    if (tex->dirty && tex->resource_key != SokolResourceKeyNone) {
        tex->update();
    }
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

void cSokolRender::CreateCommandEmpty() {
    xassert(ActiveScene);
    MT_IS_GRAPH();
    
#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
    std::string label = "Pipeline: Empty";
    RenderSubmitEvent(RenderEvent::CREATE_COMMAND, label.c_str());
#endif

    //Create command to be send
    SokolCommand* cmd = new SokolCommand();
    memcpy(cmd->viewport, activeCommand.viewport, sizeof(Vect2i) * 2);
    memcpy(cmd->clip, activeCommand.clip, sizeof(Vect2i) * 2);

    //Pass the pass action
    if (activeCommand.pass_action) {
        cmd->pass_action = activeCommand.pass_action;
        activeCommand.pass_action = nullptr;
    }

    //Submit command
    commands.emplace_back(cmd);

#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
    label = "Submit"
            + " Vtxs: " + std::to_string(cmd->vertices)
            + " Idxs: " + std::to_string(cmd->indices)
            + " Tex0: " + std::to_string(reinterpret_cast<size_t>(cmd->sokol_images[0]))
            + " Tex1: " + std::to_string(reinterpret_cast<size_t>(cmd->sokol_images[1]));
    RenderSubmitEvent(RenderEvent::CREATE_COMMAND, label.c_str(), cmd);
#endif
}

void cSokolRender::CreateCommand(VertexBuffer* vb, size_t vertices, IndexBuffer* ib, size_t indices) {
    xassert(ActiveScene);
    if (!vb) {
        //Never supposed to happenKenji Tsuruta
        xassert(vb);
        return;
    }
    MT_IS_GRAPH();
    if (0 == vertices) vertices = activeCommand.vertices;
    if (0 == indices) indices = activeCommand.indices;
    PIPELINE_TYPE pipelineType = activePipelineType;

    SokolPipelineContext pipeline_context;
    pipeline_context.pipeline_type = pipelineType;
    pipeline_context.pipeline_mode = activePipelineMode;
    pipeline_context.primitive_type = activeDrawBuffer->primitive;
    pipeline_context.vertex_fmt = vb->fmt;
    SokolPipeline* pipeline = GetPipeline(pipeline_context);
    if (!pipeline) {
        xxassert(0, "CreateCommand: No pipeline found");
        return;
    }

#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
    RenderSubmitEvent(RenderEvent::CREATE_COMMAND, "Start");
#endif

    //Update buffers
    if (vb->data && (vb->sg == nullptr || vb->dirty)) {
        size_t len = vertices * vb->VertexSize;
        if (vb->sg == nullptr || vb->sg->buffer == nullptr) {
            PrepareSokolBuffer(vb->sg, vb, len, vb->dynamic, SG_BUFFERTYPE_VERTEXBUFFER);
        }
        if (vb->dynamic) {
            vb->sg->update(vb, len);
        }
    }
    if (ib->data && (!ib->sg || ib->dirty)) {
        size_t len = indices * sizeof(indices_t);
        if (ib->sg == nullptr || ib->sg->buffer == nullptr) {
            PrepareSokolBuffer(ib->sg, ib, len, ib->dynamic, SG_BUFFERTYPE_INDEXBUFFER);
        }
        if (ib->dynamic) {
            ib->sg->update(ib, len);
        }
    }
    xassert(activeCommand.vertices <= vertices);
    xassert((activeCommand.base_elements + activeCommand.indices) <= indices);
    xassert(0 < activeCommand.vertices);
    
    //Create command to be send
    SokolCommand* cmd = new SokolCommand();
    cmd->pipeline = pipeline;
    for (int i = 0; i < PERIMETER_SOKOL_TEXTURES; ++i) {
        SokolTexture2D* tex = activeCommandTextures[i];
        if (tex == nullptr) {
            cmd->SetImage(i, nullptr);
        } else {
            if (!tex->image || tex->dirty) {
                PrepareSokolTexture(tex);
            }
            cmd->SetImage(i, tex->image);
        }
    }
    cmd->base_elements = activeCommand.base_elements;
    cmd->vertices = activeCommand.vertices;
    cmd->indices = activeCommand.indices;
    memcpy(cmd->viewport, activeCommand.viewport, sizeof(Vect2i) * 2);
    memcpy(cmd->clip, activeCommand.clip, sizeof(Vect2i) * 2);
    
    //Set shader params
    cmd->CreateShaderParams();
    switch (cmd->pipeline->shader_id) {
        default:
        case SOKOL_SHADER_ID_NONE:
            xassert(0);
            break;
        case SOKOL_SHADER_ID_color_tex1:
        case SOKOL_SHADER_ID_color_tex2: {
            auto vs_params = reinterpret_cast<color_texture_vs_params_t*>(cmd->vs_params);
            auto fs_params = reinterpret_cast<color_texture_fs_params_t*>(cmd->fs_params);
            shader_set_common_params(vs_params, fs_params);
            vs_params->tex0_mat = activeTextureTransform[0];
            vs_params->tex1_mat = activeTextureTransform[1];
            fs_params->un_color_mode = activeCommandColorMode;
            fs_params->un_tex2_lerp = activeCommandTex2Lerp;
            break;
        }
        case SOKOL_SHADER_ID_normal: {
            auto vs_params = reinterpret_cast<normal_texture_vs_params_t*>(cmd->vs_params);
            auto fs_params = reinterpret_cast<normal_texture_fs_params_t*>(cmd->fs_params);
            shader_set_common_params(vs_params, fs_params);
            vs_params->model = isOrthographicProjSet ? Mat4f::ID : activeCommandW;
            vs_params->tex0_mat = activeTextureTransform[0];
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
        case SOKOL_SHADER_ID_object_shadow: {
            auto vs_params = reinterpret_cast<object_shadow_vs_params_t*>(cmd->vs_params);
            vs_params->un_mvp = isOrthographicProjSet ? orthoVP : (activeCommandW * activeCommandVP);
            break;
        }
        case SOKOL_SHADER_ID_only_texture: {
            auto vs_params = reinterpret_cast<only_texture_vs_params_t*>(cmd->vs_params);
            vs_params->un_mvp = isOrthographicProjSet ? orthoVP : (activeCommandW * activeCommandVP);
            break;
        }
        case SOKOL_SHADER_ID_tile_map: {
            auto vs_params = reinterpret_cast<tile_map_vs_params_t*>(cmd->vs_params);
            vs_params->un_mvp = isOrthographicProjSet ? orthoVP : (activeCommandW * activeCommandVP);
            vs_params->un_shadow = activeShadowMatrix;
            memcpy(vs_params->un_inv_world_size, &activeWorldSize, sizeof(float) * 2);

            auto fs_params = reinterpret_cast<tile_map_fs_params_t*>(cmd->fs_params);
            memcpy(&fs_params->un_tile_color, &activeCommandTileColor, sizeof(float) * 4);
            break;
        }
    }
    
    //Transfer buffers to command
    vb->sg->buffer->IncRef();
    ib->sg->buffer->IncRef();
    cmd->vertex_buffer = vb->sg->buffer;
    cmd->index_buffer = ib->sg->buffer;
    activeCommand.base_elements = 0;
    activeCommand.vertices = 0;
    activeCommand.indices = 0;

    //Pass the pass action
    if (activeCommand.pass_action) {
        cmd->pass_action = activeCommand.pass_action;
        activeCommand.pass_action = nullptr;
    }
    
    //Submit command
    if (activeRenderTarget != nullptr) {
        activeRenderTarget->commands.emplace_back(cmd);
    } else {
        commands.emplace_back(cmd);
    }

#ifdef PERIMETER_RENDER_TRACKER_COMMANDS
    label = "Submit"
            + " Vtxs: " + std::to_string(cmd->vertices)
            + " Idxs: " + std::to_string(cmd->indices)
            + " Tex0: " + std::to_string(reinterpret_cast<size_t>(cmd->sokol_images[0]))
            + " Tex1: " + std::to_string(reinterpret_cast<size_t>(cmd->sokol_images[1]));
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
    activeCommand.base_elements = 0;
    activeCommand.vertices = 0;
    activeCommand.indices = 0;
    //Those that are dynamic should have their buffer released since we wil recreate later 
    if (db->vb.dynamic && db->vb.sg) {
        db->vb.sg->release_buffer();
    }
    if (db->ib.dynamic && db->ib.sg) {
        db->ib.sg->release_buffer();
    }
}

void cSokolRender::SubmitDrawBuffer(DrawBuffer* db, DrawRange* range) {
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
    RenderSubmitEvent(RenderEvent::SUBMIT_DRAW_BUFFER, "", db);
#endif
    if (activeDrawBuffer != nullptr && activeDrawBuffer != db) {
        //We need to submit internal render buffer first
        FinishActiveDrawBuffer();
    }
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
    DrawBuffer* db = GetDrawBuffer(vb->fmt, primitive, 1024 * 20);
    size_t indices_count = range ? range->len : indices;

    void* vb_dst = nullptr;
    indices_t* ib_dst = nullptr;
    db->LockRaw(vertices, indices_count, vb_dst, ib_dst, false);
    size_t written_vertices = db->written_vertices;
    db->lock_written_vertices = vertices;
    db->lock_written_indices = indices_count;
    
    //Write vertices
    memcpy(vb_dst, vb->data, vertices * vb->VertexSize);

    //Write indices
    indices_t* ib_src = static_cast<indices_t*>(ib->data);
    if (range) ib_src += range->offset;
    copy_indices_list(ib_dst, ib_src, indices_count, written_vertices);
    
    db->Unlock();

    SubmitDrawBuffer(db, nullptr);
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

void cSokolRender::SetTextureTransform(uint32_t slot, const Mat4f& transform) {
    xassert(slot < GetMaxTextureSlots());
    if (!activeTextureTransform[slot].eq(transform, 0)) {
        FinishActiveDrawBuffer();
        activeTextureTransform[slot] = transform;
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
    SokolTexture2D* tex = texture_image != nullptr ? texture_image->sg : nullptr;
    if (activeCommandTextures[slot] != tex) {
        FinishActiveDrawBuffer();
        activeCommandTextures[slot] = tex;
    }
}

uint32_t cSokolRender::GetMaxTextureSlots() {
    return PERIMETER_SOKOL_TEXTURES;
}

void cSokolRender::SetRenderTarget(cTexture* target, SurfaceImage zbuffer) {
    if (target == shadowMapRenderTarget->texture) {
        activeRenderTarget = shadowMapRenderTarget;
        return;
    }

    if (target == lightMapRenderTarget->texture) {
        activeRenderTarget = lightMapRenderTarget;
        return;
    }

    xxassert(false, "Unexpected render target");
}

void cSokolRender::RestoreRenderTarget() {
    activeRenderTarget = nullptr;
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
            return activePipelineMode.wireframe_mode;
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
            if (state != activePipelineMode.wireframe_mode) {
                FinishActiveDrawBuffer();
                activePipelineMode.wireframe_mode = state;
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

    if ((vLight && activeLightDir != *vLight) ||
        (Diffuse && activeLightDiffuse != *Diffuse) ||
        (Ambient && activeLightAmbient != *Ambient) ||
        (Specular && activeLightSpecular != *Specular)) {
        FinishActiveDrawBuffer();
        if (vLight) activeLightDir = *vLight;
        if (Diffuse) activeLightDiffuse = *Diffuse;
        if (Ambient) activeLightAmbient = *Ambient;
        if (Specular) activeLightSpecular = *Specular;
    }
}
