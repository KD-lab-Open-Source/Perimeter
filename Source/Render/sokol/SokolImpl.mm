// This file is specific for Apple platforms to compile as ObjC++

//Wrap SokolImpl as ObjC++ for sokol headers
#include "SokolImpl.cpp"

//ObjC Sokol - SDL2 glue code for Metal
#ifdef SOKOL_METAL
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <SDL_syswm.h>

//TODO This should be stored in a metal_context struct like in d3d11 impl
static id<MTLDevice> mtl_device;
static MTKView* mtk_view;
#if TARGET_OS_IPHONE
static id mtk_view_controller;
#endif

static bool is_capturing_frame = false;

void sokol_metal_setup(SDL_Window* sdl_window, sg_desc* desc, sg_swapchain* swapchain, uint32_t ScreenHZ) {
    //Get window from SDL which we will use to associate Metal stuff
    SDL_SysWMinfo wm_info;
    SDL_VERSION(&wm_info.version);
    SDL_GetWindowWMInfo(sdl_window, &wm_info);
#if TARGET_OS_IPHONE
    UIWindow* window = wm_info.info.uikit.window
#else
    NSWindow* window = wm_info.info.cocoa.window;
#endif

    // MTKView and Metal device
    mtl_device = MTLCreateSystemDefaultDevice();
    mtk_view = [[MTKView alloc] init];
    mtk_view.paused = true;
    mtk_view.enableSetNeedsDisplay = false;
    [mtk_view setPreferredFramesPerSecond:ScreenHZ];
    [mtk_view setDevice: mtl_device];
    [mtk_view setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [mtk_view setDepthStencilPixelFormat:MTLPixelFormatDepth32Float_Stencil8];
    [mtk_view setSampleCount:(NSUInteger) swapchain->sample_count];
#if TARGET_OS_IPHONE
    [mtk_view setContentScaleFactor:1.0f];
    [mtk_view setUserInteractionEnabled:YES];
    [mtk_view setMultipleTouchEnabled:YES];
    [window addSubview:mtk_view];
    mtk_view_controller = [[UIViewController<MTKViewDelegate> alloc] init];
    [mtk_view_controller setView:mtk_view];
    [window setRootViewController:mtk_view_controller];
    [window makeKeyAndVisible];
#else
    [window setContentView:mtk_view];
    CGSize drawable_size = { (CGFloat) swapchain->width, (CGFloat) swapchain->height };
    [mtk_view setDrawableSize:drawable_size];
    [[mtk_view layer] setMagnificationFilter:kCAFilterNearest];
    NSApp.activationPolicy = NSApplicationActivationPolicyRegular;
    [NSApp activateIgnoringOtherApps:YES];
    [window makeKeyAndOrderFront:nil];
#endif
    
    //Setup Metal specific context
    desc->environment.metal.device = (__bridge const void*) mtl_device;
}

void sokol_metal_render(sg_swapchain* swapchain, void (*callback)()) {
    @autoreleasepool {
        swapchain->metal.current_drawable = (__bridge const void*) [mtk_view currentDrawable];
        swapchain->metal.depth_stencil_texture = (__bridge const void*) [mtk_view depthStencilTexture];
        swapchain->metal.msaa_color_texture = (__bridge const void*) [mtk_view multisampleColorTexture];
        callback();
    }
}

void sokol_metal_draw() {
    [mtk_view draw];
}

void sokol_metal_destroy(sg_swapchain* swapchain) {
    swapchain->metal.msaa_color_texture = nullptr;
    swapchain->metal.depth_stencil_texture = nullptr;
    swapchain->metal.current_drawable = nullptr;
#if TARGET_OS_IPHONE
    mtk_view_controller = nullptr;
#endif
    mtk_view = nullptr;
    mtl_device = nullptr;
}

void sokol_metal_start_frame_capture() {
    if (is_capturing_frame) {
        MTLCaptureManager* capture_manager = [MTLCaptureManager sharedCaptureManager];
        [capture_manager stopCapture];
        exit(1);
    }

    MTLCaptureManager* capture_manager = [MTLCaptureManager sharedCaptureManager];
    if (![capture_manager supportsDestination:MTLCaptureDestinationGPUTraceDocument]) {
        printf("sokol_metal_start_frame_capture: unsupported destination\n");
        return;
    }

    MTLCaptureDescriptor* capture_descriptor = [MTLCaptureDescriptor new];
    capture_descriptor.captureObject = mtl_device;
    capture_descriptor.destination = MTLCaptureDestinationGPUTraceDocument;
    capture_descriptor.outputURL = [NSURL fileURLWithPath:@"/tmp/Perimeter.gputrace"];

    NSError* error;
    BOOL result = [capture_manager startCaptureWithDescriptor:capture_descriptor error:&error];
    if (!result) {
        printf("sokol_metal_start_frame_capture: start capture error: %s\n", error.localizedDescription.UTF8String);
    }

    is_capturing_frame = true;
}

#endif