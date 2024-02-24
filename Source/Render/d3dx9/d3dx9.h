#ifndef PERIMETER_D3DX9_H
#define PERIMETER_D3DX9_H

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

//Math Methods

#endif //PERIMETER_D3DX9_H
