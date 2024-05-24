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

#ifdef PERIMETER_DEBUG
class FrameCaptureManager final {
public:
    void StartCapture() {
        if (_is_capturing_frame) {
            return;
        }

        MTLCaptureManager* capture_manager = [MTLCaptureManager sharedCaptureManager];
        if (![capture_manager supportsDestination:MTLCaptureDestinationGPUTraceDocument]) {
            fprintf(stderr, "FrameCaptureManager: unsupported destination\n");
            return;
        }

        MTLCaptureDescriptor* capture_descriptor = [MTLCaptureDescriptor new];
        capture_descriptor.captureObject = mtl_device;
        capture_descriptor.destination = MTLCaptureDestinationGPUTraceDocument;
        const auto current_time = static_cast<unsigned long>([NSDate date].timeIntervalSince1970);
        _path = [[NSString stringWithFormat:@"/tmp/Perimeter_%lu.gputrace", current_time] copy];
        capture_descriptor.outputURL = [NSURL fileURLWithPath:_path];

        NSError* error;
        BOOL result = [capture_manager startCaptureWithDescriptor:capture_descriptor error:&error];
        if (!result) {
            fprintf(stderr, "FrameCaptureManager: start capture error: %s\n", error.localizedDescription.UTF8String);
            return;
        }

        _is_capturing_frame = true;
    }

    void StopCapture() {
        if (!_is_capturing_frame) {
            return;
        }

        MTLCaptureManager* capture_manager = [MTLCaptureManager sharedCaptureManager];
        [capture_manager stopCapture];

        fprintf(stderr, "FrameCaptureManager: frame saved to file: %s\n", _path.UTF8String);
        _is_capturing_frame = false;

        [_path release];
        _path = nil;
    }

private:
    bool _is_capturing_frame = false;
    NSString* _path = nil;
};

FrameCaptureManager gb_frame_capture_manager;

void sokol_metal_capture_frame() {
    gb_frame_capture_manager.StartCapture();
}
#endif

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
#ifdef PERIMETER_DEBUG
    gb_frame_capture_manager.StopCapture();
#endif
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

#endif