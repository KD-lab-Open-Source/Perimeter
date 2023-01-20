// This file is specific for Apple platforms to compile as ObjC++

//Wrap SokolImpl as ObjC++ for sokol headers
#include "SokolImpl.cpp"

//ObjC Sokol - SDL2 glue code for Metal
#ifdef SOKOL_METAL
#import <QuartzCore/CAMetalLayer.h>
#import <SDL_metal.h>

static CAMetalLayer* sokol_metal_layer;
static id<CAMetalDrawable> sokol_metal_drawable;
static MTLRenderPassDescriptor* sokol_metal_render_pass_descriptor;

const void* sokol_metal_drawable_cb() {
    return (__bridge const void*) sokol_metal_drawable;
}

const void* sokol_metal_renderpass_descriptor_cb() {
    sokol_metal_drawable = [sokol_metal_layer nextDrawable];

    sokol_metal_render_pass_descriptor = [[MTLRenderPassDescriptor alloc] init];
    sokol_metal_render_pass_descriptor.colorAttachments[0].texture = sokol_metal_drawable.texture;
    return (__bridge const void*) sokol_metal_render_pass_descriptor;
}

void sokol_metal_setup_desc(SDL_MetalView view, sg_desc* desc) {
    //Get Metal layer from SDL and add Metal device
    sokol_metal_layer = (__bridge CAMetalLayer*) SDL_Metal_GetLayer(view);
    sokol_metal_layer.device = MTLCreateSystemDefaultDevice();
    
    //Setup Metal specific context
    sg_metal_context_desc& metalctx = desc->context.metal;
    metalctx.device = sokol_metal_layer.device;
    metalctx.renderpass_descriptor_cb = sokol_metal_renderpass_descriptor_cb;
    metalctx.drawable_cb = sokol_metal_drawable_cb;
}
#endif