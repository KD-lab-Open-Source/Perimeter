#include <vector>
#include "xmath.h"
#include "Umath.h"
#include "StdAfxRD.h"
#include "sokol_gfx.h"
#include "SokolResources.h"
#include "IRenderDevice.h"
#include "EmptyRenderDevice.h"
#include "SokolRender.h"
#include "xerrhand.h"
#include "SokolShaders.h"
#include "VertexFormat.h"
#include "Texture.h"

const int PERIMETER_SOKOL_VERTEXES_COUNT = 10240;
const int PERIMETER_SOKOL_POLYGON_COUNT = 20480;

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
    
    FinishActiveCommand();

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
    
    std::vector<SokolBuffer*> owned_buffers;

    //Iterate each command
    for (const auto& command : commands) {
        if (command.owned_buffers) {
            if (command.vertex_buffer) owned_buffers.emplace_back(command.vertex_buffer);
            if (command.index_buffer) owned_buffers.emplace_back(command.index_buffer);
        }
        
        //Nothing to draw
        if (!command.elements) {
            continue;
        }
        
        //Get pipeline
        const SokolPipeline* pipeline = &pipelines[command.vertex_fmt];
        if (pipeline->vertex_fmt <= 0) {
            //Not implemented vertex format
            continue;
        }
        shader_funcs* shader = pipeline->shader_funcs;

        //Calculate camera stuff
        vs_params_t vs_params;
        //hmm_mat4 proj = HMM_Orthographic(0, state->canvas.X, state->canvas.Y, 0, 10, -10);
        //hmm_mat4 view = HMM_Translate(state->camera);
        //vs_params.mvp = HMM_MultiplyMat4(proj, view);

        //Apply pipeline
        sg_apply_pipeline(pipeline->pipeline);
        
        //Apply bindings
        sg_bindings bindings = {};
        
        //Bind vertex and index buffer, ensure they are updated
        if (!command.vertex_buffer) {
            xassert(0);
            continue;
        }
        command.vertex_buffer->update();
        bindings.vertex_buffers[0] = command.vertex_buffer->buffer;
        if (command.index_buffer) {
            command.index_buffer->update();
            bindings.index_buffer = command.index_buffer->buffer;
        }
        
        //Bind images for samplers
        if (pipeline->vertex_fmt & VERTEX_FMT_TEX1) {
            SokolTexture2D* tex = command.textures[0];
            xassert(tex);
            if (tex) {
                int slot = shader->image_slot(SG_SHADERSTAGE_FS, "un_tex0");
                bindings.fs_images[slot] = tex->image;
            }
        }
        if (pipeline->vertex_fmt & VERTEX_FMT_TEX2) {
            SokolTexture2D* tex = command.textures[1];
            xassert(tex);
            if (tex) {
                int slot = shader->image_slot(SG_SHADERSTAGE_FS, "un_tex1");
                bindings.fs_images[slot] = tex->image;
            }
        }
        sg_apply_bindings(&bindings);
        
        //Apply VS uniforms
        sg_range vs_params_range = SG_RANGE(vs_params);
        int vs_params_slot = shader->uniformblock_slot(SG_SHADERSTAGE_VS, "vs_params");
        sg_apply_uniforms(SG_SHADERSTAGE_VS, vs_params_slot, &vs_params_range);

        //Draw
        sg_draw(0, static_cast<int>(command.elements), 1);
    }

    //End pass
    sg_end_pass();
    
    for (auto buf : owned_buffers) {
        delete buf;
    }
    
    commands.clear();

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

void cSokolRender::FinishActiveCommand() {
    if (polygonsCount) {
        commands.emplace_back();
        SokolCommand& cmd = commands.back();
        cmd.vertex_fmt = vertexBuffer.fmt;
        cmd.elements = polygonsCount * 3;
        memcpy(cmd.textures, textures, PERIMETER_SOKOL_TEXTURES * sizeof(SokolTexture2D*));
        //Transfer buffers to command
        cmd.owned_buffers = true;
        cmd.vertex_buffer = vertexBuffer.sg;
        cmd.index_buffer = indexBuffer.sg;
        vertexBuffer.sg = nullptr;
        indexBuffer.sg = nullptr;
    }
    
    vertexesCount = 0;
    polygonsCount = 0;
}

void cSokolRender::SetupVertexBuffer(size_t& NumberVertex, size_t& NumberPolygons, uint32_t vertex_fmt) {
    if (vertexBuffer.sg
    && vertexBuffer.fmt == vertex_fmt
    && NumberVertex + vertexesCount < vertexBuffer.NumberVertex
    && NumberPolygons + polygonsCount < indexBuffer.NumberPolygons) {
        //We can use current buffers

        int nv = NumberVertex;
        int np = NumberPolygons;
        NumberVertex = vertexesCount;
        NumberPolygons = polygonsCount;
        vertexesCount += nv;
        polygonsCount += np;
        return;
    }
    
    FinishActiveCommand();

    vertexesCount = NumberVertex;
    polygonsCount = NumberPolygons;
    NumberVertex = NumberPolygons = 0;
    
    //Create buffers
    DeleteVertexBuffer(vertexBuffer);
    DeleteIndexBuffer(indexBuffer);
    CreateVertexBuffer(vertexBuffer, PERIMETER_SOKOL_VERTEXES_COUNT, vertex_fmt, true);
    CreateIndexBuffer(indexBuffer, PERIMETER_SOKOL_POLYGON_COUNT);
}

void cSokolRender::SetNoMaterial(eBlendMode blend, float Phase, cTexture* Texture0, cTexture* Texture1,
                                 eColorMode color_mode) {
    if (Texture0) {
        int nAllFrame = Texture0->GetNumberFrame();
        int nFrame = 1 < nAllFrame ? static_cast<int>(0.999f * Phase * nAllFrame) : 0;
        textures[0] = Texture0->GetFrameImage(nFrame).sg;
    } else {
        textures[0] = nullptr;
    }
    if (Texture1) {
        int nAllFrame = Texture1->GetNumberFrame();
        int nFrame = 1 < nAllFrame ? static_cast<int>(0.999f * Phase * nAllFrame) : 0;
        textures[1] = Texture1->GetFrameImage(nFrame).sg;
    } else {
        textures[1] = nullptr;
    }
}
