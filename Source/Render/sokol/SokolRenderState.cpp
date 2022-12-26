#include <vector>
#include "xmath.h"
#include "Umath.h"
#include "sokol_gfx.h"
#include "SokolResources.h"
#include "IRenderDevice.h"
#include "EmptyRenderDevice.h"
#include "SokolRender.h"
#include "xerrhand.h"
#include "SokolShaders.h"
#include "VertexFormat.h"

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

    //Iterate each command
    for (const auto& command : commands) {
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
        bindings.index_buffer = command.index_buffer->buffer;
        
        //Bind images for samplers
        if (pipeline->vertex_fmt & VERTEX_FMT_TEX1) {
            xassert(command.texture_1);
            if (command.texture_1) {
                int slot = shader->image_slot(SG_SHADERSTAGE_FS, "un_tex0");
                bindings.fs_images[slot] = command.texture_1->image;
            }
        }
        if (pipeline->vertex_fmt & VERTEX_FMT_TEX2) {
            xassert(command.texture_2);
            if (command.texture_1) {
                int slot = shader->image_slot(SG_SHADERSTAGE_FS, "un_tex1");
                bindings.fs_images[slot] = command.texture_2->image;
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
    commands.clear();

    //End pass
    sg_end_pass();

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
