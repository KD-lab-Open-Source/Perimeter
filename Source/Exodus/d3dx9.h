#ifndef PERIMETER_D3DX9_H
#define PERIMETER_D3DX9_H

#include "d3dx9math.h"

enum D3DXIMAGE_FILEFORMAT {
    D3DXIFF_BMP         = 0
};

#define D3DX_FILTER_NONE             1
#define D3DX_FILTER_POINT            (2 << 0)
#define D3DX_FILTER_TRIANGLE         (4 << 0)
#define D3DX_FILTER_DITHER           (1 << 19)

//Textures Methods

HRESULT WINAPI D3DXLoadSurfaceFromMemory(IDirect3DSurface9 *dst_surface,
                                         const PALETTEENTRY *dst_palette, const RECT *dst_rect, const void *src_memory,
                                         D3DFORMAT src_format, UINT src_pitch, const PALETTEENTRY *src_palette, const RECT *src_rect,
                                         DWORD filter, D3DCOLOR color_key);
HRESULT D3DXCreateTextureFromFileInMemory(IDirect3DDevice9 *device, const void *srcdata, UINT srcdatasize, struct IDirect3DTexture9 **texture);

HRESULT D3DXCreateCubeTextureFromFile(IDirect3DDevice9 *device,
                                      const char *srcfile, struct IDirect3DCubeTexture9 **cube);

HRESULT D3DXCreateCubeTextureFromFileInMemory(struct IDirect3DDevice9 *device,
                                              const void *data, UINT datasize, struct IDirect3DCubeTexture9 **texture);

HRESULT D3DXSaveSurfaceToFile(const char *destfile, D3DXIMAGE_FILEFORMAT destformat,
                               IDirect3DSurface9 *srcsurface, const PALETTEENTRY *srcpalette, const RECT *srcrect);

//Math Methods

#endif //PERIMETER_D3DX9_H
