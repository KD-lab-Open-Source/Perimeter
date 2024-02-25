#ifndef PERIMETER_SOKOLD3D_H
#define PERIMETER_SOKOLD3D_H

#include <d3d11.h>
#include <dxgi.h>

struct sokol_d3d_context {
    HWND hwnd = nullptr;
    DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* device_context = nullptr;
    IDXGISwapChain* swap_chain = nullptr;
    ID3D11Texture2D* render_target = nullptr;
    ID3D11RenderTargetView* render_target_view = nullptr;
    ID3D11Texture2D* depth_stencil_buffer = nullptr;
    ID3D11DepthStencilView* depth_stencil_view = nullptr;
};

#endif //PERIMETER_SOKOLD3D_H
