#include "StdAfxRD.h"
#include "VulkanRender.h"
#include "VulkanPipeline.h"
#include "VulkanResources.h"
#include "FileImage.h"
#include "Texture.h"
#include "RenderTracker.h"
#include "Umath.h"
#include "xmath.h"
#include <SDL_vulkan.h>
#include <vector>
#include <set>
#include <cstring>
#include <fstream>
#include "xutl.h"

#define VK_CHECK(call, msg) do {                                \
    VkResult _vr = (call);                                      \
    if (_vr != VK_SUCCESS) {                                    \
        fprintf(stderr, "Vulkan: %s - VkResult=%d (%s:%d)\n",   \
                msg, (int)_vr, __FILE__, __LINE__);             \
    }                                                           \
} while(0)

bool cVulkanRender::profilingEnabled = false;

void cVulkanRender::SetProfilingEnabled(bool on) {
    profilingEnabled = on;
    if (on) {
        // s_ps is file-static in VulkanRenderState.cpp. We can't reach it
        // directly, but ProfReport initializes t0 on first call after 5s.
        // Initialize it lazily in ProfReport's elapsed check.
    }
}

cVulkanRender::cVulkanRender() {
    activeVP    = Mat4f::ID;
    activeWorld = Mat4f::ID;
    orthoVP     = Mat4f::ID;
    activeVPVk  = GetVkClipFix();
    orthoVPVk   = GetVkClipFix();
    for (int i = 0; i < VULKAN_TEXTURES; i++) activeTexTransform[i] = Mat4f::ID;
    activeShadowMatrix = Mat4f::ID;
}

cVulkanRender::~cVulkanRender() {
    Done();
}

eRenderDeviceSelection cVulkanRender::GetRenderSelection() const {
    return DEVICE_VULKAN;
}

uint32_t cVulkanRender::GetWindowCreationFlags() const {
    return cInterfaceRenderDevice::GetWindowCreationFlags() | SDL_WINDOW_VULKAN;
}

// ---- Init ---------------------------------------------------------------

int cVulkanRender::Init(int xScr, int yScr, int mode, SDL_Window* wnd, int RefreshHz) {
    RenderSubmitEvent(RenderEvent::INIT, "Vulkan start");

    int res = cInterfaceRenderDevice::Init(xScr, yScr, mode, wnd, RefreshHz);
    if (res != 0) return res;

    if (!CreateInstance())         { fprintf(stderr, "Vulkan: CreateInstance failed\n");        return 1; }
#if VK_ENABLE_VALIDATION
    if (validationEnabled) SetupDebugMessenger();
#endif
    if (!CreateSurface())          { fprintf(stderr, "Vulkan: CreateSurface failed\n");         return 1; }
    if (!PickPhysicalDevice())     { fprintf(stderr, "Vulkan: PickPhysicalDevice failed\n");    return 1; }
    if (!CreateLogicalDevice())    { fprintf(stderr, "Vulkan: CreateLogicalDevice failed\n");   return 1; }
    if (!CreateSwapchain())        { fprintf(stderr, "Vulkan: CreateSwapchain failed\n");       return 1; }
    if (!CreateSwapchainImageViews()) { fprintf(stderr, "Vulkan: ImageViews failed\n");         return 1; }
    if (!CreateDepthResources())   { fprintf(stderr, "Vulkan: DepthResources failed\n");       return 1; }
    if (!CreateRenderPasses())     { fprintf(stderr, "Vulkan: RenderPasses failed\n");         return 1; }
    if (!CreateFramebuffers())     { fprintf(stderr, "Vulkan: Framebuffers failed\n");         return 1; }
    if (!CreateCommandPool())      { fprintf(stderr, "Vulkan: CommandPool failed\n");          return 1; }
    if (!CreateDescriptorSetLayout()) { fprintf(stderr, "Vulkan: DescSetLayout failed\n");     return 1; }
    if (!CreatePipelineLayout())   { fprintf(stderr, "Vulkan: PipelineLayout failed\n");       return 1; }
    {
        std::vector<uint8_t> cacheData;
        {
            std::ifstream f("vulkan_pipeline_cache.bin", std::ios::binary | std::ios::ate);
            if (f.is_open()) {
                size_t sz = (size_t)f.tellg();
                if (sz > 0) {
                    cacheData.resize(sz);
                    f.seekg(0, std::ios::beg);
                    f.read(reinterpret_cast<char*>(cacheData.data()), sz);
                }
            }
        }
        VkPipelineCacheCreateInfo pcci = {};
        pcci.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        if (!cacheData.empty()) {
            pcci.initialDataSize = cacheData.size();
            pcci.pInitialData    = cacheData.data();
        }
        vkCreatePipelineCache(device, &pcci, nullptr, &pipelineCache);
    }
    if (!CreateSamplers())         { fprintf(stderr, "Vulkan: Samplers failed\n");             return 1; }
    if (!CreateFrameData())        { fprintf(stderr, "Vulkan: FrameData failed\n");            return 1; }
    if (!LoadShaderModules())      { fprintf(stderr, "Vulkan: Shaders failed\n");              return 1; }
    if (!CreateEmptyTexture())     { fprintf(stderr, "Vulkan: EmptyTexture failed\n");         return 1; }
    WarmupPipelines();

    orthoVP = Mat4f::ID;
    SetOrthographic(orthoVP, ScreenSize.x, -ScreenSize.y, 10, -10);
    orthoVPVk = orthoVP * GetVkClipFix();

    swapchainCommands.reserve(16384);

    printf("Vulkan render initialized (%dx%d)\n", xScr, yScr);
    profilingEnabled = check_command_line("vulkan_profile") != nullptr;
    if (!profilingEnabled) {
        for (int i = 1; i < app_argc; i++)
            if (app_argv[i] == "--vulkan_profile") { profilingEnabled = true; break; }
    }
    {
        bool noBatch = check_command_line("vulkan_no_batch") != nullptr;
        if (!noBatch) {
            for (int i = 1; i < app_argc; i++)
                if (app_argv[i] == "--vulkan_no_batch") { noBatch = true; break; }
        }
        if (noBatch) {
            printf("Vulkan: deferred batching disabled (--vulkan_no_batch)\n");
            m_batchingEnabled = false;
        }
    }
    RenderSubmitEvent(RenderEvent::INIT, "Vulkan done");
    return 0;
}

// ---- Done ---------------------------------------------------------------

int cVulkanRender::Done() {
    RenderSubmitEvent(RenderEvent::DONE, "Vulkan start");

    if (device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device);
    }

    int ret = cInterfaceRenderDevice::Done();

    ClearAllCommands();
    pendingTextureUploads.clear();
    pendingBufferUploads.clear();
    for (auto& d : pendingTextureDeletions)
        delete d.tex;
    pendingTextureDeletions.clear();
    for (auto& d : pendingBufferDeletions)
        delete d.buf;
    pendingBufferDeletions.clear();
    ClearPipelines();

    SavePipelineCache();

    // DeleteShadowTexture also destroys the per-target framebuffers
    // (which VulkanRenderTarget's destructor doesn't own).
    DeleteShadowTexture();

    DestroyShaderModules();
    DestroyFrameData();

    delete emptyTexture;      emptyTexture = nullptr;

    if (sampler       != VK_NULL_HANDLE) vkDestroySampler(device, sampler, nullptr);
    sampler = VK_NULL_HANDLE;

    if (pipelineCache       != VK_NULL_HANDLE) vkDestroyPipelineCache(device, pipelineCache, nullptr);
    if (pipelineLayout      != VK_NULL_HANDLE) vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    if (descriptorSetLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    pipelineCache = VK_NULL_HANDLE;
    pipelineLayout = VK_NULL_HANDLE;
    descriptorSetLayout = VK_NULL_HANDLE;

    if (commandPool != VK_NULL_HANDLE) vkDestroyCommandPool(device, commandPool, nullptr);
    commandPool = VK_NULL_HANDLE;

    CleanupSwapchain();

    if (device   != VK_NULL_HANDLE) { vkDestroyDevice(device, nullptr);   device = VK_NULL_HANDLE; }
    if (surface  != VK_NULL_HANDLE) { vkDestroySurfaceKHR(instance, surface, nullptr); surface = VK_NULL_HANDLE; }
#if VK_ENABLE_VALIDATION
    if (debugMessenger != VK_NULL_HANDLE) {
        auto fn = (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (fn) fn(instance, debugMessenger, nullptr);
        debugMessenger = VK_NULL_HANDLE;
    }
#endif
    if (instance != VK_NULL_HANDLE) { vkDestroyInstance(instance, nullptr); instance = VK_NULL_HANDLE; }

    RenderSubmitEvent(RenderEvent::DONE, "Vulkan done");
    return ret;
}

// ---- CreateInstance -----------------------------------------------------

bool cVulkanRender::CreateInstance() {
    // Extensions required by SDL
    uint32_t extCount = 0;
    SDL_Vulkan_GetInstanceExtensions(sdl_window, &extCount, nullptr);
    std::vector<const char*> extensions(extCount);
    SDL_Vulkan_GetInstanceExtensions(sdl_window, &extCount, extensions.data());

    // Enumerate available instance extensions
    uint32_t availExtCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &availExtCount, nullptr);
    std::vector<VkExtensionProperties> availExts(availExtCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availExtCount, availExts.data());

    auto hasExt = [&](const char* name) {
        for (auto& e : availExts)
            if (strcmp(e.extensionName, name) == 0) return true;
        return false;
    };

    std::vector<const char*> layers;

#if VK_ENABLE_VALIDATION
    // Check if validation layer and its companion extension exist at runtime.
    // Without the Vulkan SDK installed they won't be present; skip silently so
    // the instance can still be created.
    uint32_t availLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&availLayerCount, nullptr);
    std::vector<VkLayerProperties> availLayers(availLayerCount);
    vkEnumerateInstanceLayerProperties(&availLayerCount, availLayers.data());

    bool hasValidation = false;
    for (auto& l : availLayers)
        if (strcmp(l.layerName, "VK_LAYER_KHRONOS_validation") == 0) { hasValidation = true; break; }

    bool hasDebugUtils = hasExt(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    if (hasValidation && hasDebugUtils) {
        layers.push_back("VK_LAYER_KHRONOS_validation");
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        validationEnabled = true;
    } else {
        fprintf(stderr, "Vulkan: validation layers not available (hasLayer=%d hasDebugUtils=%d) - running without\n",
                (int)hasValidation, (int)hasDebugUtils);
    }
#endif

    VkApplicationInfo appInfo = {};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "Perimeter";
    appInfo.applicationVersion = VK_MAKE_VERSION(3, 1, 0);
    appInfo.pEngineName        = "PerimeterEngine";
    appInfo.apiVersion         = VK_API_VERSION_1_2;

    VkInstanceCreateInfo ci = {};
    ci.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ci.pApplicationInfo        = &appInfo;
    ci.enabledExtensionCount   = (uint32_t)extensions.size();
    ci.ppEnabledExtensionNames = extensions.data();
    ci.enabledLayerCount       = (uint32_t)layers.size();
    ci.ppEnabledLayerNames     = layers.empty() ? nullptr : layers.data();

    VkResult vr = vkCreateInstance(&ci, nullptr, &instance);
    if (vr != VK_SUCCESS) {
        fprintf(stderr, "Vulkan: vkCreateInstance failed: %d\n", (int)vr);
        return false;
    }
    return true;
}

#if VK_ENABLE_VALIDATION
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
    VkDebugUtilsMessageTypeFlagsEXT             /*type*/,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*                                       /*pUserData*/)
{
    const char* sev = (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)   ? "ERROR"
                    : (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) ? "WARN"
                    :                                                               "INFO";
    fprintf(stderr, "[VK-%s] %s\n", sev, pCallbackData->pMessage);
    return VK_FALSE;
}
#endif

// ---- SetupDebugMessenger ------------------------------------------------

bool cVulkanRender::SetupDebugMessenger() {
#if VK_ENABLE_VALIDATION
    VkDebugUtilsMessengerCreateInfoEXT ci = {};
    ci.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    ci.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    ci.pfnUserCallback = VulkanDebugCallback;

    auto fn = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (fn) fn(instance, &ci, nullptr, &debugMessenger);
#endif
    return true;
}

// ---- CreateSurface ------------------------------------------------------

bool cVulkanRender::CreateSurface() {
    return SDL_Vulkan_CreateSurface(sdl_window, instance, &surface) == SDL_TRUE;
}

// ---- PickPhysicalDevice -------------------------------------------------

static bool QueueFamiliesComplete(uint32_t gfx, uint32_t prs) {
    return gfx != UINT32_MAX && prs != UINT32_MAX;
}

bool cVulkanRender::PickPhysicalDevice() {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    if (count == 0) return false;
    std::vector<VkPhysicalDevice> devs(count);
    vkEnumeratePhysicalDevices(instance, &count, devs.data());

    for (auto& pd : devs) {
        // Find queue families
        uint32_t qfc = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &qfc, nullptr);
        std::vector<VkQueueFamilyProperties> qfp(qfc);
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &qfc, qfp.data());

        uint32_t gfx = UINT32_MAX, prs = UINT32_MAX;
        for (uint32_t i = 0; i < qfc; i++) {
            if (qfp[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) gfx = i;
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(pd, i, surface, &presentSupport);
            if (presentSupport) prs = i;
            if (QueueFamiliesComplete(gfx, prs)) break;
        }
        if (!QueueFamiliesComplete(gfx, prs)) continue;

        // Check swapchain support
        uint32_t fmtCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &fmtCount, nullptr);
        uint32_t pmCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(pd, surface, &pmCount, nullptr);
        if (fmtCount == 0 || pmCount == 0) continue;

        physicalDevice  = pd;
        graphicsFamily  = gfx;
        presentFamily   = prs;
        vkGetPhysicalDeviceProperties(pd, &deviceProperties);
        vkGetPhysicalDeviceMemoryProperties(pd, &deviceMemoryProperties);
        uboAlignment = (uint32_t)deviceProperties.limits.minUniformBufferOffsetAlignment;
        if (uboAlignment == 0) uboAlignment = VULKAN_UBO_ALIGNMENT_FALLBACK;
        printf("Vulkan: selected GPU: %s (UBO alignment = %u bytes)\n",
               deviceProperties.deviceName, uboAlignment);
        return true;
    }
    return false;
}

// ---- CreateLogicalDevice ------------------------------------------------

bool cVulkanRender::CreateLogicalDevice() {
    std::set<uint32_t> uniqueQueues = { graphicsFamily, presentFamily };
    std::vector<VkDeviceQueueCreateInfo> qcis;
    float priority = 1.0f;
    for (uint32_t qf : uniqueQueues) {
        VkDeviceQueueCreateInfo ci = {};
        ci.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        ci.queueFamilyIndex = qf;
        ci.queueCount       = 1;
        ci.pQueuePriorities = &priority;
        qcis.push_back(ci);
    }

    const char* deviceExts[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    VkPhysicalDeviceFeatures features = {};
    features.fillModeNonSolid    = VK_TRUE;  // wireframe
    features.depthClamp          = VK_FALSE;

    VkDeviceCreateInfo ci = {};
    ci.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    ci.queueCreateInfoCount    = (uint32_t)qcis.size();
    ci.pQueueCreateInfos       = qcis.data();
    ci.enabledExtensionCount   = 1;
    ci.ppEnabledExtensionNames = deviceExts;
    ci.pEnabledFeatures        = &features;

    if (vkCreateDevice(physicalDevice, &ci, nullptr, &device) != VK_SUCCESS) return false;
    vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentFamily,  0, &presentQueue);
    return true;
}

// ---- CreateSwapchain ----------------------------------------------------

bool cVulkanRender::CreateSwapchain() {
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);

    // Choose format
    uint32_t fmtCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &fmtCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(fmtCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &fmtCount, formats.data());
    VkSurfaceFormatKHR chosen = formats[0];
    for (auto& f : formats) {
        if (f.format == VK_FORMAT_B8G8R8A8_UNORM &&
            f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            chosen = f; break;
        }
    }
    swapchainFormat     = chosen.format;
    swapchainColorSpace = chosen.colorSpace;

    // Choose present mode
    uint32_t pmCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &pmCount, nullptr);
    std::vector<VkPresentModeKHR> modes(pmCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &pmCount, modes.data());
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR; // always available
    bool wantVsync = (RenderMode & RENDERDEVICE_MODE_VSYNC) != 0;
    if (!wantVsync) {
        for (auto m : modes) {
            if (m == VK_PRESENT_MODE_MAILBOX_KHR) { presentMode = m; break; }
        }
        if (presentMode == VK_PRESENT_MODE_FIFO_KHR) {
            for (auto m : modes) {
                if (m == VK_PRESENT_MODE_IMMEDIATE_KHR) { presentMode = m; break; }
            }
        }
    }

    // Choose extent
    if (caps.currentExtent.width != UINT32_MAX) {
        swapchainExtent = caps.currentExtent;
    } else {
        swapchainExtent.width  = std::clamp((uint32_t)ScreenSize.x,
                                            caps.minImageExtent.width,
                                            caps.maxImageExtent.width);
        swapchainExtent.height = std::clamp((uint32_t)ScreenSize.y,
                                            caps.minImageExtent.height,
                                            caps.maxImageExtent.height);
    }

    uint32_t imageCount = caps.minImageCount + 1;
    if (caps.maxImageCount > 0) imageCount = std::min(imageCount, caps.maxImageCount);

    VkSwapchainCreateInfoKHR ci = {};
    ci.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    ci.surface          = surface;
    ci.minImageCount    = imageCount;
    ci.imageFormat      = swapchainFormat;
    ci.imageColorSpace  = swapchainColorSpace;
    ci.imageExtent      = swapchainExtent;
    ci.imageArrayLayers = 1;
    ci.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    ci.preTransform     = caps.currentTransform;
    ci.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    ci.presentMode      = presentMode;
    ci.clipped          = VK_TRUE;

    uint32_t qfi[] = { graphicsFamily, presentFamily };
    if (graphicsFamily != presentFamily) {
        ci.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        ci.queueFamilyIndexCount = 2;
        ci.pQueueFamilyIndices   = qfi;
    } else {
        ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    if (vkCreateSwapchainKHR(device, &ci, nullptr, &swapchain) != VK_SUCCESS) return false;

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
    return true;
}

// ---- CreateSwapchainImageViews ------------------------------------------

bool cVulkanRender::CreateSwapchainImageViews() {
    swapchainImageViews.resize(swapchainImages.size());
    for (size_t i = 0; i < swapchainImages.size(); i++) {
        swapchainImageViews[i] = CreateImageView(swapchainImages[i],
                                                  swapchainFormat,
                                                  VK_IMAGE_ASPECT_COLOR_BIT);
        if (swapchainImageViews[i] == VK_NULL_HANDLE) return false;
    }
    return true;
}

// ---- FindDepthFormat ----------------------------------------------------

VkFormat cVulkanRender::FindDepthFormat() const {
    VkFormat candidates[] = { VK_FORMAT_D32_SFLOAT,
                               VK_FORMAT_D32_SFLOAT_S8_UINT,
                               VK_FORMAT_D24_UNORM_S8_UINT };
    for (auto fmt : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, fmt, &props);
        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            return fmt;
    }
    return VK_FORMAT_D32_SFLOAT;
}

// ---- CreateDepthResources -----------------------------------------------

bool cVulkanRender::CreateDepthResources() {
    depthFormat = FindDepthFormat();
    if (!CreateImage(swapchainExtent.width, swapchainExtent.height, 1,
                     depthFormat, VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     depthImage, depthMemory)) return false;

    VkImageAspectFlags aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT ||
        depthFormat == VK_FORMAT_D24_UNORM_S8_UINT)
        aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;

    depthView = CreateImageView(depthImage, depthFormat, aspect);
    return depthView != VK_NULL_HANDLE;
}

// ---- CreateRenderPasses -------------------------------------------------

bool cVulkanRender::CreateRenderPasses() {
    // ---- Main render pass (swapchain: color + depth) -------------------
    {
        VkAttachmentDescription colorAtt = {};
        colorAtt.format         = swapchainFormat;
        colorAtt.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAtt.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAtt.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAtt.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAtt.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAtt.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAtt.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription depthAtt = {};
        depthAtt.format         = depthFormat;
        depthAtt.samples        = VK_SAMPLE_COUNT_1_BIT;
        depthAtt.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAtt.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAtt.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAtt.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAtt.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAtt.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
        VkAttachmentReference depthRef = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

        VkSubpassDescription sp = {};
        sp.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        sp.colorAttachmentCount    = 1;
        sp.pColorAttachments       = &colorRef;
        sp.pDepthStencilAttachment = &depthRef;

        VkSubpassDependency dep = {};
        dep.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dep.dstSubpass    = 0;
        dep.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                          | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dep.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                          | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dep.srcAccessMask = 0;
        dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                          | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkAttachmentDescription atts[] = { colorAtt, depthAtt };
        VkRenderPassCreateInfo ci = {};
        ci.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        ci.attachmentCount = 2;
        ci.pAttachments    = atts;
        ci.subpassCount    = 1;
        ci.pSubpasses      = &sp;
        ci.dependencyCount = 1;
        ci.pDependencies   = &dep;
        if (vkCreateRenderPass(device, &ci, nullptr, &mainRenderPass) != VK_SUCCESS)
            return false;
    }

    // ---- Shadow map render pass (depth only) ---------------------------
    {
        VkAttachmentDescription depthAtt = {};
        depthAtt.format         = VK_FORMAT_D32_SFLOAT;
        depthAtt.samples        = VK_SAMPLE_COUNT_1_BIT;
        depthAtt.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAtt.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        depthAtt.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAtt.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAtt.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAtt.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

        VkAttachmentReference depthRef = { 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

        VkSubpassDescription sp = {};
        sp.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        sp.pDepthStencilAttachment = &depthRef;

        VkSubpassDependency deps[2] = {};
        deps[0].srcSubpass    = VK_SUBPASS_EXTERNAL;
        deps[0].dstSubpass    = 0;
        deps[0].srcStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        deps[0].dstStageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        deps[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        deps[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        deps[1].srcSubpass    = 0;
        deps[1].dstSubpass    = VK_SUBPASS_EXTERNAL;
        deps[1].srcStageMask  = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        deps[1].dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        deps[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        deps[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        VkRenderPassCreateInfo ci = {};
        ci.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        ci.attachmentCount = 1;
        ci.pAttachments    = &depthAtt;
        ci.subpassCount    = 1;
        ci.pSubpasses      = &sp;
        ci.dependencyCount = 2;
        ci.pDependencies   = deps;
        if (vkCreateRenderPass(device, &ci, nullptr, &shadowRenderPass) != VK_SUCCESS)
            return false;
    }

    // ---- Lightmap render pass (color only, no depth) -------------------
    {
        VkAttachmentDescription colorAtt = {};
        colorAtt.format         = VK_FORMAT_R8G8B8A8_UNORM;
        colorAtt.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAtt.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAtt.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAtt.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAtt.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAtt.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAtt.finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference colorRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

        VkSubpassDescription sp = {};
        sp.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        sp.colorAttachmentCount = 1;
        sp.pColorAttachments    = &colorRef;

        VkSubpassDependency deps[2] = {};
        deps[0].srcSubpass    = VK_SUBPASS_EXTERNAL;
        deps[0].dstSubpass    = 0;
        deps[0].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        deps[0].dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        deps[0].srcAccessMask = 0;
        deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        deps[1].srcSubpass    = 0;
        deps[1].dstSubpass    = VK_SUBPASS_EXTERNAL;
        deps[1].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        deps[1].dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        deps[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        deps[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        VkRenderPassCreateInfo ci = {};
        ci.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        ci.attachmentCount = 1;
        ci.pAttachments    = &colorAtt;
        ci.subpassCount    = 1;
        ci.pSubpasses      = &sp;
        ci.dependencyCount = 2;
        ci.pDependencies   = deps;
        if (vkCreateRenderPass(device, &ci, nullptr, &lightmapRenderPass) != VK_SUCCESS)
            return false;
    }

    return true;
}

// ---- CreateFramebuffers -------------------------------------------------

bool cVulkanRender::CreateFramebuffers() {
    swapchainFramebuffers.resize(swapchainImageViews.size());
    for (size_t i = 0; i < swapchainImageViews.size(); i++) {
        VkImageView attachments[] = { swapchainImageViews[i], depthView };
        VkFramebufferCreateInfo ci = {};
        ci.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        ci.renderPass      = mainRenderPass;
        ci.attachmentCount = 2;
        ci.pAttachments    = attachments;
        ci.width           = swapchainExtent.width;
        ci.height          = swapchainExtent.height;
        ci.layers          = 1;
        if (vkCreateFramebuffer(device, &ci, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS)
            return false;
    }
    return true;
}

// ---- CreateCommandPool --------------------------------------------------

bool cVulkanRender::CreateCommandPool() {
    VkCommandPoolCreateInfo ci = {};
    ci.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    ci.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    ci.queueFamilyIndex = graphicsFamily;
    return vkCreateCommandPool(device, &ci, nullptr, &commandPool) == VK_SUCCESS;
}

// ---- CreateDescriptorSetLayout ------------------------------------------

bool cVulkanRender::CreateDescriptorSetLayout() {
    // Binding 0: UBO (VS params) - non-dynamic
    // Binding 1: UBO (FS params) - non-dynamic
    // Binding 2..5: combined image sampler (4 texture slots)
    VkDescriptorSetLayoutBinding bindings[6] = {};
    for (int i = 0; i < 2; i++) {
        bindings[i].binding         = i;
        bindings[i].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        bindings[i].descriptorCount = 1;
        bindings[i].stageFlags      = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    for (int i = 2; i < 6; i++) {
        bindings[i].binding         = i;
        bindings[i].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[i].descriptorCount = 1;
        bindings[i].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    VkDescriptorSetLayoutCreateInfo ci = {};
    ci.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    ci.bindingCount = 6;
    ci.pBindings    = bindings;
    return vkCreateDescriptorSetLayout(device, &ci, nullptr, &descriptorSetLayout) == VK_SUCCESS;
}

// ---- CreatePipelineLayout -----------------------------------------------

bool cVulkanRender::CreatePipelineLayout() {
    VkPipelineLayoutCreateInfo ci = {};
    ci.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    ci.setLayoutCount = 1;
    ci.pSetLayouts    = &descriptorSetLayout;
    return vkCreatePipelineLayout(device, &ci, nullptr, &pipelineLayout) == VK_SUCCESS;
}

// ---- CreateSamplers -----------------------------------------------------

bool cVulkanRender::CreateSamplers() {
    // Regular filtering sampler
    VkSamplerCreateInfo ci = {};
    ci.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    ci.magFilter               = VK_FILTER_LINEAR;
    ci.minFilter               = VK_FILTER_LINEAR;
    ci.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    ci.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    ci.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    ci.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    ci.anisotropyEnable        = VK_FALSE;
    ci.maxAnisotropy           = 1.0f;
    ci.compareEnable           = VK_FALSE;
    ci.compareOp               = VK_COMPARE_OP_ALWAYS;
    ci.minLod                  = 0.0f;
    ci.maxLod                  = VK_LOD_CLAMP_NONE;
    ci.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    if (vkCreateSampler(device, &ci, nullptr, &sampler) != VK_SUCCESS) return false;

    return true;
}

// ---- CreateFrameData ----------------------------------------------------

bool cVulkanRender::CreateFrameData() {
    VkCommandBufferAllocateInfo cbai = {};
    cbai.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbai.commandPool        = commandPool;
    cbai.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbai.commandBufferCount = VULKAN_FRAMES_IN_FLIGHT;
    VkCommandBuffer cbs[VULKAN_FRAMES_IN_FLIGHT];
    if (vkAllocateCommandBuffers(device, &cbai, cbs) != VK_SUCCESS) return false;

    VkSemaphoreCreateInfo sci = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VkFenceCreateInfo fci = {};
    fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fci.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // UBO pool descriptor pool sizes
    VkDescriptorPoolSize poolSizes[2] = {};
    poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSizes[0].descriptorCount = VULKAN_DESC_POOL_SETS * 2;
    poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = VULKAN_DESC_POOL_SETS * VULKAN_TEXTURES;

    VkDescriptorPoolCreateInfo dpci = {};
    dpci.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dpci.flags         = 0;
    dpci.maxSets       = VULKAN_DESC_POOL_SETS;
    dpci.poolSizeCount = 2;
    dpci.pPoolSizes    = poolSizes;

    for (int i = 0; i < VULKAN_FRAMES_IN_FLIGHT; i++) {
        frames[i].commandBuffer = cbs[i];
        if (vkCreateSemaphore(device, &sci, nullptr, &frames[i].imageAvailSemaphore) != VK_SUCCESS) return false;
        if (vkCreateSemaphore(device, &sci, nullptr, &frames[i].renderDoneSemaphore) != VK_SUCCESS) return false;
        if (vkCreateFence(device, &fci, nullptr, &frames[i].fence) != VK_SUCCESS) return false;
        if (vkCreateDescriptorPool(device, &dpci, nullptr, &frames[i].descriptorPool) != VK_SUCCESS) return false;

        // UBO buffer
        VkBufferCreateInfo bi = {};
        bi.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bi.size        = VULKAN_UBO_POOL_SIZE;
        bi.usage       = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(device, &bi, nullptr, &frames[i].uboBuffer) != VK_SUCCESS) return false;

        VkMemoryRequirements mr;
        vkGetBufferMemoryRequirements(device, frames[i].uboBuffer, &mr);
        VkMemoryAllocateInfo mai = {};
        mai.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        mai.allocationSize = mr.size;
        mai.memoryTypeIndex = FindMemoryType(mr.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (vkAllocateMemory(device, &mai, nullptr, &frames[i].uboMemory) != VK_SUCCESS) return false;
        vkBindBufferMemory(device, frames[i].uboBuffer, frames[i].uboMemory, 0);
        vkMapMemory(device, frames[i].uboMemory, 0, VULKAN_UBO_POOL_SIZE, 0, &frames[i].uboMapped);

        // Dynamic vertex ring buffer
        {
            VkBufferCreateInfo vbi = {};
            vbi.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            vbi.size        = VULKAN_DYN_VTX_POOL_SIZE;
            vbi.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            vbi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            if (vkCreateBuffer(device, &vbi, nullptr, &frames[i].dynVtxBuffer) != VK_SUCCESS) return false;
            VkMemoryRequirements vmr;
            vkGetBufferMemoryRequirements(device, frames[i].dynVtxBuffer, &vmr);
            VkMemoryAllocateInfo vmai = {};
            vmai.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            vmai.allocationSize  = vmr.size;
            vmai.memoryTypeIndex = FindMemoryType(vmr.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            if (vkAllocateMemory(device, &vmai, nullptr, &frames[i].dynVtxMemory) != VK_SUCCESS) return false;
            vkBindBufferMemory(device, frames[i].dynVtxBuffer, frames[i].dynVtxMemory, 0);
            vkMapMemory(device, frames[i].dynVtxMemory, 0, VULKAN_DYN_VTX_POOL_SIZE, 0, &frames[i].dynVtxMapped);
        }

        // Dynamic index ring buffer
        {
            VkBufferCreateInfo ibi = {};
            ibi.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            ibi.size        = VULKAN_DYN_IDX_POOL_SIZE;
            ibi.usage       = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            ibi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            if (vkCreateBuffer(device, &ibi, nullptr, &frames[i].dynIdxBuffer) != VK_SUCCESS) return false;
            VkMemoryRequirements imr;
            vkGetBufferMemoryRequirements(device, frames[i].dynIdxBuffer, &imr);
            VkMemoryAllocateInfo imai = {};
            imai.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            imai.allocationSize  = imr.size;
            imai.memoryTypeIndex = FindMemoryType(imr.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            if (vkAllocateMemory(device, &imai, nullptr, &frames[i].dynIdxMemory) != VK_SUCCESS) return false;
            vkBindBufferMemory(device, frames[i].dynIdxBuffer, frames[i].dynIdxMemory, 0);
            vkMapMemory(device, frames[i].dynIdxMemory, 0, VULKAN_DYN_IDX_POOL_SIZE, 0, &frames[i].dynIdxMapped);
        }

        // Staging ring buffer for mid-frame texture/buffer uploads
        {
            VkBufferCreateInfo sbi = {};
            sbi.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            sbi.size        = VULKAN_STAGE_POOL_SIZE;
            sbi.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            sbi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            if (vkCreateBuffer(device, &sbi, nullptr, &frames[i].stageBuffer) != VK_SUCCESS) return false;
            VkMemoryRequirements smr;
            vkGetBufferMemoryRequirements(device, frames[i].stageBuffer, &smr);
            VkMemoryAllocateInfo smai = {};
            smai.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            smai.allocationSize  = smr.size;
            smai.memoryTypeIndex = FindMemoryType(smr.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            if (vkAllocateMemory(device, &smai, nullptr, &frames[i].stageMemory) != VK_SUCCESS) return false;
            vkBindBufferMemory(device, frames[i].stageBuffer, frames[i].stageMemory, 0);
            vkMapMemory(device, frames[i].stageMemory, 0, VULKAN_STAGE_POOL_SIZE, 0, &frames[i].stageMapped);
        }

        // Instance data ring buffer for instanced draws
        static const size_t VULKAN_INST_POOL_SIZE = 64 * 1024 * 1024; // 64 MB
        {
            VkBufferCreateInfo ibi2 = {};
            ibi2.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            ibi2.size        = VULKAN_INST_POOL_SIZE;
            ibi2.usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            ibi2.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            if (vkCreateBuffer(device, &ibi2, nullptr, &frames[i].instBuffer) != VK_SUCCESS) return false;
            VkMemoryRequirements imr2;
            vkGetBufferMemoryRequirements(device, frames[i].instBuffer, &imr2);
            VkMemoryAllocateInfo imai2 = {};
            imai2.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            imai2.allocationSize  = imr2.size;
            imai2.memoryTypeIndex = FindMemoryType(imr2.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            if (vkAllocateMemory(device, &imai2, nullptr, &frames[i].instMemory) != VK_SUCCESS) return false;
            vkBindBufferMemory(device, frames[i].instBuffer, frames[i].instMemory, 0);
            vkMapMemory(device, frames[i].instMemory, 0, VULKAN_INST_POOL_SIZE, 0, &frames[i].instMapped);
        }
    }
    return true;
}

// ---- LoadShaderModules --------------------------------------------------

VkShaderModule cVulkanRender::LoadShaderModule(const char* spvPath) const {
    std::ifstream file(spvPath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        fprintf(stderr, "Vulkan: cannot open shader '%s'\n", spvPath);
        return VK_NULL_HANDLE;
    }
    size_t sz = (size_t)file.tellg();
    std::vector<char> buf(sz);
    file.seekg(0);
    file.read(buf.data(), sz);

    VkShaderModuleCreateInfo ci = {};
    ci.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = sz;
    ci.pCode    = reinterpret_cast<const uint32_t*>(buf.data());
    VkShaderModule mod = VK_NULL_HANDLE;
    VK_CHECK(vkCreateShaderModule(device, &ci, nullptr, &mod),
             "vkCreateShaderModule");
    return mod;
}

bool cVulkanRender::LoadShaderModules() {
    // Shader SPV files are in the same directory as the executable
    // They are compiled by the build system (CMake + glslc)
    struct ShaderPair {
        VULKAN_SHADER_ID id;
        const char* vert;
        const char* frag;
    };
    ShaderPair pairs[] = {
        { VULKAN_SHADER_ID_mesh_color_tex1,    "shaders_vk/mesh_color_tex1.vert.spv",    "shaders_vk/mesh_color_tex1.frag.spv" },
        { VULKAN_SHADER_ID_mesh_color_tex2,    "shaders_vk/mesh_color_tex2.vert.spv",    "shaders_vk/mesh_color_tex2.frag.spv" },
        { VULKAN_SHADER_ID_mesh_normal_tex1,   "shaders_vk/mesh_normal_tex1.vert.spv",   "shaders_vk/mesh_normal_tex1.frag.spv" },
        { VULKAN_SHADER_ID_mesh_tex1,          "shaders_vk/mesh_tex1.vert.spv",          "shaders_vk/mesh_tex1.frag.spv" },
        { VULKAN_SHADER_ID_shadow_tex1,        "shaders_vk/shadow_tex1.vert.spv",        "shaders_vk/shadow_tex1.frag.spv" },
        { VULKAN_SHADER_ID_shadow_normal_tex1, "shaders_vk/shadow_normal_tex1.vert.spv", "shaders_vk/shadow_normal_tex1.frag.spv" },
        { VULKAN_SHADER_ID_tile_map,           "shaders_vk/tile_map.vert.spv",           "shaders_vk/tile_map.frag.spv" },
    };
    int missing = 0;
    for (auto& p : pairs) {
        shaderModules[p.id][0] = LoadShaderModule(p.vert);
        shaderModules[p.id][1] = LoadShaderModule(p.frag);
        bool ok = shaderModules[p.id][0] != VK_NULL_HANDLE &&
                  shaderModules[p.id][1] != VK_NULL_HANDLE;
        fprintf(stderr, "Vulkan: shader id=%d vert=%s frag=%s -> %s\n",
                (int)p.id, p.vert, p.frag, ok ? "OK" : "FAILED");
        if (!ok) missing++;
    }

    // Instanced mesh shader variants
    ShaderPair instPairs[] = {
        { VULKAN_SHADER_ID_mesh_color_tex1,   "shaders_vk/mesh_color_tex1_inst.vert.spv",   "shaders_vk/mesh_color_tex1_inst.frag.spv" },
        { VULKAN_SHADER_ID_mesh_color_tex2,   "shaders_vk/mesh_color_tex2_inst.vert.spv",   "shaders_vk/mesh_color_tex2_inst.frag.spv" },
        { VULKAN_SHADER_ID_mesh_normal_tex1,  "shaders_vk/mesh_normal_tex1_inst.vert.spv",  "shaders_vk/mesh_normal_tex1_inst.frag.spv" },
        { VULKAN_SHADER_ID_mesh_tex1,         "shaders_vk/mesh_tex1_inst.vert.spv",         "shaders_vk/mesh_tex1_inst.frag.spv" },
        { VULKAN_SHADER_ID_shadow_tex1,       "shaders_vk/shadow_tex1_inst.vert.spv",       "shaders_vk/shadow_tex1_inst.frag.spv" },
        { VULKAN_SHADER_ID_shadow_normal_tex1,"shaders_vk/shadow_normal_tex1_inst.vert.spv","shaders_vk/shadow_normal_tex1_inst.frag.spv" },
    };
    for (auto& p : instPairs) {
        shaderModulesInst[p.id][0] = LoadShaderModule(p.vert);
        shaderModulesInst[p.id][1] = LoadShaderModule(p.frag);
        bool ok = shaderModulesInst[p.id][0] != VK_NULL_HANDLE &&
                  shaderModulesInst[p.id][1] != VK_NULL_HANDLE;
        fprintf(stderr, "Vulkan: inst shader id=%d vert=%s frag=%s -> %s\n",
                (int)p.id, p.vert, p.frag, ok ? "OK" : "FAILED");
        if (!ok) missing++;
    }
    if (missing > 0) {
        fprintf(stderr,
                "Vulkan: %d shader(s) failed to load - affected draws will be "
                "silently skipped (HUD/terrain may appear invisible).\n",
                missing);
    }
    return true; // non-fatal if some are missing (pipelines will fail gracefully)
}

// ---- CreateEmptyTexture -------------------------------------------------

bool cVulkanRender::CreateEmptyTexture() {
    VkImage img = VK_NULL_HANDLE;
    VkDeviceMemory mem = VK_NULL_HANDLE;
    if (!CreateImage(1, 1, 1, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, img, mem)) {
        return false;
    }
    VkImageView view = CreateImageView(img, VK_FORMAT_R8G8B8A8_UNORM,
                                       VK_IMAGE_ASPECT_COLOR_BIT, 1);

    auto* imgRes    = new VulkanImageResource();
    imgRes->device  = device;
    imgRes->image   = img;
    imgRes->memory  = mem;
    imgRes->view    = view;
    imgRes->format  = VK_FORMAT_R8G8B8A8_UNORM;
    imgRes->width   = 1;
    imgRes->height  = 1;
    imgRes->is_depth= false;

    emptyTexture = new VulkanTexture2D(VK_FORMAT_R8G8B8A8_UNORM, 1, 1, false, false, 1);
    emptyTexture->image_res = imgRes;

    const uint8_t white[4] = { 255, 255, 255, 255 };
    UploadTexture(emptyTexture, white, 4);
    return emptyTexture->image_res != nullptr;
}

// ---- CleanupSwapchain ---------------------------------------------------

void cVulkanRender::CleanupSwapchain() {
    for (auto fb : swapchainFramebuffers) vkDestroyFramebuffer(device, fb, nullptr);
    swapchainFramebuffers.clear();

    if (depthView   != VK_NULL_HANDLE) vkDestroyImageView(device, depthView, nullptr);
    if (depthImage  != VK_NULL_HANDLE) vkDestroyImage(device, depthImage, nullptr);
    if (depthMemory != VK_NULL_HANDLE) vkFreeMemory(device, depthMemory, nullptr);
    depthView = VK_NULL_HANDLE; depthImage = VK_NULL_HANDLE; depthMemory = VK_NULL_HANDLE;

    if (mainRenderPass      != VK_NULL_HANDLE) vkDestroyRenderPass(device, mainRenderPass, nullptr);
    if (shadowRenderPass    != VK_NULL_HANDLE) vkDestroyRenderPass(device, shadowRenderPass, nullptr);
    if (lightmapRenderPass  != VK_NULL_HANDLE) vkDestroyRenderPass(device, lightmapRenderPass, nullptr);
    mainRenderPass = shadowRenderPass = lightmapRenderPass = VK_NULL_HANDLE;

    for (auto iv : swapchainImageViews) vkDestroyImageView(device, iv, nullptr);
    swapchainImageViews.clear();
    swapchainImages.clear();

    if (swapchain != VK_NULL_HANDLE) { vkDestroySwapchainKHR(device, swapchain, nullptr); swapchain = VK_NULL_HANDLE; }
}

// ---- RecreateSwapchain --------------------------------------------------

bool cVulkanRender::RecreateSwapchain() {
    vkDeviceWaitIdle(device);
    ClearAllCommands();
    // Pipelines are render-pass-compatible across recreation (same format,
    // same sample count - swapchain resize doesn't change either).  Destroying
    // them here would cause a full-pipeline-creation hitch on the next frame.
    CleanupSwapchain();
    return CreateSwapchain()
        && CreateSwapchainImageViews()
        && CreateDepthResources()
        && CreateRenderPasses()
        && CreateFramebuffers();
}

// ---- ChangeSize ---------------------------------------------------------

bool cVulkanRender::ChangeSize(int xScr, int yScr, int mode) {
    ScreenSize = { xScr, yScr };
    SetOrthographic(orthoVP, ScreenSize.x, -ScreenSize.y, 10, -10);
    orthoVPVk = orthoVP * GetVkClipFix();
    return RecreateSwapchain();
}

// ---- DestroyFrameData ---------------------------------------------------

void cVulkanRender::DestroyFrameData() {
    for (int i = 0; i < VULKAN_FRAMES_IN_FLIGHT; i++) {
        auto& f = frames[i];
        if (f.uboMapped)            { vkUnmapMemory(device, f.uboMemory); f.uboMapped = nullptr; }
        if (f.uboBuffer != VK_NULL_HANDLE) vkDestroyBuffer(device, f.uboBuffer, nullptr);
        if (f.uboMemory != VK_NULL_HANDLE) vkFreeMemory(device, f.uboMemory, nullptr);
        if (f.dynVtxMapped)         { vkUnmapMemory(device, f.dynVtxMemory); f.dynVtxMapped = nullptr; }
        if (f.dynVtxBuffer != VK_NULL_HANDLE) vkDestroyBuffer(device, f.dynVtxBuffer, nullptr);
        if (f.dynVtxMemory != VK_NULL_HANDLE) vkFreeMemory(device, f.dynVtxMemory, nullptr);
        if (f.dynIdxMapped)         { vkUnmapMemory(device, f.dynIdxMemory); f.dynIdxMapped = nullptr; }
        if (f.dynIdxBuffer != VK_NULL_HANDLE) vkDestroyBuffer(device, f.dynIdxBuffer, nullptr);
        if (f.dynIdxMemory != VK_NULL_HANDLE) vkFreeMemory(device, f.dynIdxMemory, nullptr);
        if (f.stageMapped)          { vkUnmapMemory(device, f.stageMemory); f.stageMapped = nullptr; }
        if (f.stageBuffer != VK_NULL_HANDLE) vkDestroyBuffer(device, f.stageBuffer, nullptr);
        if (f.stageMemory != VK_NULL_HANDLE) vkFreeMemory(device, f.stageMemory, nullptr);
        if (f.instMapped)           { vkUnmapMemory(device, f.instMemory); f.instMapped = nullptr; }
        if (f.instBuffer != VK_NULL_HANDLE) vkDestroyBuffer(device, f.instBuffer, nullptr);
        if (f.instMemory != VK_NULL_HANDLE) vkFreeMemory(device, f.instMemory, nullptr);
        if (f.descriptorPool != VK_NULL_HANDLE) vkDestroyDescriptorPool(device, f.descriptorPool, nullptr);
        if (f.fence          != VK_NULL_HANDLE) vkDestroyFence(device, f.fence, nullptr);
        if (f.renderDoneSemaphore != VK_NULL_HANDLE) vkDestroySemaphore(device, f.renderDoneSemaphore, nullptr);
        if (f.imageAvailSemaphore != VK_NULL_HANDLE) vkDestroySemaphore(device, f.imageAvailSemaphore, nullptr);
        for (auto& [sbuf, smem] : f.stagingBuffers) {
            vkDestroyBuffer(device, sbuf, nullptr);
            vkFreeMemory(device, smem, nullptr);
        }
        f = {};
    }
}

// ---- DestroyShaderModules -----------------------------------------------

void cVulkanRender::DestroyShaderModules() {
    for (int i = 0; i < VULKAN_SHADER_ID_COUNT; i++) {
        for (int j = 0; j < 2; j++) {
            if (shaderModules[i][j] != VK_NULL_HANDLE) {
                vkDestroyShaderModule(device, shaderModules[i][j], nullptr);
                shaderModules[i][j] = VK_NULL_HANDLE;
            }
            if (shaderModulesInst[i][j] != VK_NULL_HANDLE) {
                vkDestroyShaderModule(device, shaderModulesInst[i][j], nullptr);
                shaderModulesInst[i][j] = VK_NULL_HANDLE;
            }
        }
    }
}

// ---- Resource helpers ---------------------------------------------------

uint32_t cVulkanRender::FindMemoryType(uint32_t typeBits, VkMemoryPropertyFlags props) const {
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((typeBits & (1u << i)) &&
            (deviceMemoryProperties.memoryTypes[i].propertyFlags & props) == props)
            return i;
    }
    return UINT32_MAX;
}

bool cVulkanRender::CreateImage(uint32_t w, uint32_t h, uint32_t mips, VkFormat fmt,
                                VkImageTiling tiling, VkImageUsageFlags usage,
                                VkMemoryPropertyFlags memProps,
                                VkImage& image, VkDeviceMemory& memory) const {
    VkImageCreateInfo ci = {};
    ci.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ci.imageType     = VK_IMAGE_TYPE_2D;
    ci.format        = fmt;
    ci.extent        = { w, h, 1 };
    ci.mipLevels     = mips;
    ci.arrayLayers   = 1;
    ci.samples       = VK_SAMPLE_COUNT_1_BIT;
    ci.tiling        = tiling;
    ci.usage         = usage;
    ci.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (vkCreateImage(device, &ci, nullptr, &image) != VK_SUCCESS) return false;

    VkMemoryRequirements mr;
    vkGetImageMemoryRequirements(device, image, &mr);
    VkMemoryAllocateInfo ai = {};
    ai.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    ai.allocationSize  = mr.size;
    ai.memoryTypeIndex = FindMemoryType(mr.memoryTypeBits, memProps);
    if (vkAllocateMemory(device, &ai, nullptr, &memory) != VK_SUCCESS) return false;
    vkBindImageMemory(device, image, memory, 0);
    return true;
}

VkImageView cVulkanRender::CreateImageView(VkImage image, VkFormat fmt,
                                           VkImageAspectFlags aspect, uint32_t mips) const {
    VkImageViewCreateInfo ci = {};
    ci.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.image                           = image;
    ci.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    ci.format                          = fmt;
    ci.subresourceRange.aspectMask     = aspect;
    ci.subresourceRange.baseMipLevel   = 0;
    ci.subresourceRange.levelCount     = mips;
    ci.subresourceRange.baseArrayLayer = 0;
    ci.subresourceRange.layerCount     = 1;
    VkImageView view = VK_NULL_HANDLE;
    VK_CHECK(vkCreateImageView(device, &ci, nullptr, &view),
             "vkCreateImageView");
    return view;
}

VkCommandBuffer cVulkanRender::BeginSingleTimeCommands() const {
    VkCommandBufferAllocateInfo ai = {};
    ai.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    ai.commandPool        = commandPool;
    ai.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    ai.commandBufferCount = 1;
    VkCommandBuffer cb;
    VK_CHECK(vkAllocateCommandBuffers(device, &ai, &cb),
             "BeginSingleTimeCommands vkAllocateCommandBuffers");
    VkCommandBufferBeginInfo bi = {};
    bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cb, &bi);
    return cb;
}

void cVulkanRender::EndSingleTimeCommands(VkCommandBuffer cb) const {
    vkEndCommandBuffer(cb);
    VkSubmitInfo si = {};
    si.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers    = &cb;
    VkResult vr = vkQueueSubmit(graphicsQueue, 1, &si, VK_NULL_HANDLE);
    VK_CHECK(vr, "EndSingleTimeCommands vkQueueSubmit");
    VK_CHECK(vkQueueWaitIdle(graphicsQueue),
             "EndSingleTimeCommands vkQueueWaitIdle");
    vkFreeCommandBuffers(device, commandPool, 1, &cb);
}

void cVulkanRender::TransitionImageLayout(VkCommandBuffer cb, VkImage image,
                                          VkImageLayout oldLayout, VkImageLayout newLayout,
                                          VkImageAspectFlags aspect, uint32_t mips, uint32_t baseMip) const {
    VkImageMemoryBarrier barrier = {};
    barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                       = oldLayout;
    barrier.newLayout                       = newLayout;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = aspect;
    barrier.subresourceRange.baseMipLevel   = baseMip;
    barrier.subresourceRange.levelCount     = mips;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;

    VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
               newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
                              | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
               newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }

    vkCmdPipelineBarrier(cb, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

// ---- UBO pool ----------------------------------------------------------

size_t cVulkanRender::AllocUBO(uint32_t frame, size_t size) {
    // Use the device's real UBO alignment, fall back to
    // 256 if the device hasn't been queried yet.
    const size_t align = uboAlignment ? uboAlignment : VULKAN_UBO_ALIGNMENT_FALLBACK;
    size_t aligned = (size + align - 1) & ~(size_t)(align - 1);
    size_t offset = frames[frame].uboOffset;
    if (offset + aligned > VULKAN_UBO_POOL_SIZE) {
        static bool warned = false;
        if (!warned) {
            warned = true;
            fprintf(stderr, "VulkanRender: UBO pool overflow (%zu MB) - draw skipped. "
                    "Raise VULKAN_UBO_POOL_SIZE if this is sustained.\n",
                    VULKAN_UBO_POOL_SIZE / (1024 * 1024));
        }
        return SIZE_MAX;
    }
    frames[frame].uboOffset += aligned;
    return offset;
}

void cVulkanRender::WriteUBO(uint32_t frame, size_t offset, const void* data, size_t size) {
    if (frames[frame].uboMapped) {
        memcpy(static_cast<uint8_t*>(frames[frame].uboMapped) + offset, data, size);
    }
}

// ---- Dynamic VTX/IDX ring buffer pools ---------------------------------

size_t cVulkanRender::AllocDynVtx(uint32_t frame, size_t byteSize, size_t alignment) {
    auto& f = frames[frame];
    size_t off = (f.dynVtxOffset + alignment - 1) & ~(alignment - 1);
    if (off + byteSize > VULKAN_DYN_VTX_POOL_SIZE) {
        // Caller falls back to a per-draw VulkanBuffer; warn once so we
        // notice if the ring is chronically too small without spamming the log.
        static bool warned = false;
        if (!warned) {
            warned = true;
            fprintf(stderr, "VulkanRender: dynamic VTX pool overflow (%zu MB) - "
                    "falling back to per-draw allocations. Raise "
                    "VULKAN_DYN_VTX_POOL_SIZE if this is sustained.\n",
                    VULKAN_DYN_VTX_POOL_SIZE / (1024 * 1024));
        }
        return SIZE_MAX;
    }
    f.dynVtxOffset = off + byteSize;
    return off;
}

size_t cVulkanRender::AllocDynIdx(uint32_t frame, size_t byteSize) {
    auto& f = frames[frame];
    constexpr size_t align = sizeof(indices_t);
    size_t off = (f.dynIdxOffset + align - 1) & ~(align - 1);
    if (off + byteSize > VULKAN_DYN_IDX_POOL_SIZE) {
        static bool warned = false;
        if (!warned) {
            warned = true;
            fprintf(stderr, "VulkanRender: dynamic IDX pool overflow (%zu MB) - "
                    "falling back to per-draw allocations. Raise "
                    "VULKAN_DYN_IDX_POOL_SIZE if this is sustained.\n",
                    VULKAN_DYN_IDX_POOL_SIZE / (1024 * 1024));
        }
        return SIZE_MAX;
    }
    f.dynIdxOffset = off + byteSize;
    return off;
}

// ---- Descriptor set allocation -----------------------------------------

VkDescriptorSet cVulkanRender::AllocDescriptorSet(uint32_t frame) {
    VkDescriptorSetAllocateInfo ai = {};
    ai.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    ai.descriptorPool     = frames[frame].descriptorPool;
    ai.descriptorSetCount = 1;
    ai.pSetLayouts        = &descriptorSetLayout;
    VkDescriptorSet ds = VK_NULL_HANDLE;
    if (vkAllocateDescriptorSets(device, &ai, &ds) != VK_SUCCESS) {
        static bool warned = false;
        if (!warned) {
            warned = true;
            fprintf(stderr, "VulkanRender: descriptor set allocation failed "
                            "(pool size=%u exhausted, raise VULKAN_DESC_POOL_SETS)\n",
                    VULKAN_DESC_POOL_SETS);
        }
    }
    return ds;
}

// ---- Misc ---------------------------------------------------------------

int cVulkanRender::Fill(int r, int g, int b, int a) {
    fillColor = { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
    needClearColor = true;
    return 0;
}

void cVulkanRender::ClearZBuffer() {
    FlushActiveCommand();
    VulkanCommand cmd;
    cmd.clearDepth = true;
    cmd.hasViewport = hasViewport;
    if (hasViewport) {
        cmd.viewport[0] = activeViewportPos;
        cmd.viewport[1] = activeViewportSize;
    }
    GetActiveCommands().push_back(std::move(cmd));
}

int cVulkanRender::SetGamma(float, float, float) { return 0; }
bool cVulkanRender::SetScreenShot(const char*) { return false; }
bool cVulkanRender::IsEnableSelfShadow() { return shadowMapRenderTarget != nullptr; }

cTexture* cVulkanRender::GetShadowMap() {
    return shadowMapRenderTarget ? shadowMapRenderTarget->texture : nullptr;
}
cTexture* cVulkanRender::GetLightMap() {
    return lightMapRenderTarget ? lightMapRenderTarget->texture : nullptr;
}

union SurfaceImage cVulkanRender::GetShadowZBuffer() {
    SurfaceImage si;
    si.ptr = nullptr;
    if (shadowMapRenderTarget && shadowMapRenderTarget->texture) {
        auto* frames0 = shadowMapRenderTarget->texture->GetFrameImage(0);
        if (frames0) si.vk = frames0->vk;
    }
    return si;
}

int cVulkanRender::GetClipRect(int* xmin, int* ymin, int* xmax, int* ymax) {
    if (xmin) *xmin = activeClipPos.x;
    if (ymin) *ymin = activeClipPos.y;
    if (xmax) *xmax = activeClipPos.x + activeClipSize.x;
    if (ymax) *ymax = activeClipPos.y + activeClipSize.y;
    return 0;
}

int cVulkanRender::SetClipRect(int xmin, int ymin, int xmax, int ymax) {
    int w = xmax - xmin;
    int h = ymax - ymin;
    if (hasClip
     && activeClipPos.x  == xmin && activeClipPos.y  == ymin
     && activeClipSize.x == w    && activeClipSize.y == h) {
        return 0;
    }
    // The active draw buffer captures clip state when CreateVulkanCommand runs.
    // Without flush here, geometry queued from the previous clip would be
    // emitted with the new clip - HUD/UI elements will be clipped to the
    // wrong rectangle (invisible).
    FlushActiveCommand();
    activeClipPos.x  = xmin; activeClipPos.y  = ymin;
    activeClipSize.x = w;    activeClipSize.y = h;
    hasClip = true;
    return 0;
}

void cVulkanRender::UseOrthographicProjection() {
    if (!isOrthographicProjSet) {
        FlushActiveCommand();
        isOrthographicProjSet = true;
    }
    activeViewportPos.set(0, 0);
    activeViewportSize.set(ScreenSize.x, ScreenSize.y);
    hasViewport = true;
    // Clear any clip rect left over from 3D drawing so HUD elements are
    // not scissored by a stale panel clip rect (like Sokol's ResetViewport).
    hasClip = false;
}

void cVulkanRender::SetWorldMat4f(const Mat4f* mat) {
    const Mat4f& src = mat ? *mat : Mat4f::ID;
    if (isOrthographicProjSet || !activeWorld.eq(src, 0)) {
        FlushActiveCommand();
    }
    isOrthographicProjSet = false;
    activeWorld = src;
}

uint32_t cVulkanRender::GetMaxTextureSlots() {
    return VULKAN_TEXTURES;
}

void cVulkanRender::SetGlobalFog(const sColor4f&, const Vect2f&) {}

void cVulkanRender::SetGlobalLight(Vect3f* vLight, sColor4f* Ambient,
                                    sColor4f* Diffuse, sColor4f* Specular) {
    activeGlobalLight = true;
    if (vLight)   activeLightDir     = *vLight;
    if (Ambient)  activeLightAmbient = *Ambient;
    if (Diffuse)  activeLightDiffuse = *Diffuse;
    if (Specular) activeLightSpecular= *Specular;
}

void cVulkanRender::SetRenderTarget(cTexture* target, union SurfaceImage zbuffer) {
    if (!target) { RestoreRenderTarget(); return; }
    if (shadowMapRenderTarget && shadowMapRenderTarget->texture == target) {
        activeRenderTarget = shadowMapRenderTarget;
    } else if (lightMapRenderTarget && lightMapRenderTarget->texture == target) {
        activeRenderTarget = lightMapRenderTarget;
    }
}

void cVulkanRender::RestoreRenderTarget() {
    activeRenderTarget = nullptr;
}
