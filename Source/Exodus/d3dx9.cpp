#include <cstdio>
#include <windows.h>
#include <d3d9.h>
#include "d3dx9.h"

HRESULT D3DXLoadSurfaceFromMemory(IDirect3DSurface9 *dst_surface,
                                         const PALETTEENTRY *dst_palette, const RECT *dst_rect, const void *src_memory,
                                         D3DFORMAT src_format, UINT src_pitch, const PALETTEENTRY *src_palette, const RECT *src_rect,
                                         DWORD filter, D3DCOLOR color_key)
{
    //TODO
    return 1;
}

HRESULT D3DXCreateTextureFromFileInMemory(IDirect3DDevice9 *device, const void *srcdata, UINT srcdatasize, struct IDirect3DTexture9 **texture) {
    //TODO
    return 1;
}

HRESULT D3DXCreateCubeTextureFromFile(IDirect3DDevice9 *device,
                                      const char *srcfile, struct IDirect3DCubeTexture9 **cube) {
    //TODO
    return 1;
}

HRESULT D3DXSaveSurfaceToFile(const char *destfile, D3DXIMAGE_FILEFORMAT destformat,
                              IDirect3DSurface9 *srcsurface, const PALETTEENTRY *srcpalette, const RECT *srcrect) {
    //TODO
    return 1;
}
