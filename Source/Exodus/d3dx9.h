#ifndef PERIMETER_D3DX9_H
#define PERIMETER_D3DX9_H

//Attempt to wrap legacy D3DX9 stuff with what we got

#include "xmath.h"

//Type mapping

typedef Vect3f D3DXVECTOR3;
typedef Vect4f D3DXVECTOR4;
typedef CMatrix D3DXMATRIX;

//Textures Methods

HRESULT WINAPI D3DXLoadSurfaceFromMemory(IDirect3DSurface9 *dst_surface,
                                         const PALETTEENTRY *dst_palette, const RECT *dst_rect, const void *src_memory,
                                         D3DFORMAT src_format, UINT src_pitch, const PALETTEENTRY *src_palette, const RECT *src_rect,
                                         DWORD filter, D3DCOLOR color_key);
HRESULT D3DXCreateTextureFromFileInMemory(struct IDirect3DDevice9 *device, const void *srcdata, UINT srcdatasize, struct IDirect3DTexture9 **texture);

//Math Methods

void D3DXMatrixIdentity(D3DXMATRIX* mat);
D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* out, const D3DXMATRIX* m1, const D3DXMATRIX* m2);
D3DXVECTOR4* D3DXVec3Transform(D3DXVECTOR4* out, const D3DXVECTOR3* v, const D3DXMATRIX* m);
D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX* out, float* determinant, const D3DXMATRIX* m);

#endif //PERIMETER_D3DX9_H
