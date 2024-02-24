#include "StdAfxRD.h"
#include <d3d9.h>

int GetTextureFormatSize(D3DFORMAT f)
{
	switch(f)
	{
case D3DFMT_R8G8B8:	return 24;
case D3DFMT_A8R8G8B8: return 32;
case D3DFMT_X8R8G8B8: return 32;
case D3DFMT_R5G6B5: return 16;
case D3DFMT_X1R5G5B5: return 16;
case D3DFMT_A1R5G5B5: return  16;
case D3DFMT_A4R4G4B4: return  16;
case D3DFMT_R3G3B2: return  8;
case D3DFMT_A8: return  8;
case D3DFMT_A8R3G3B2: return 16;
case D3DFMT_X4R4G4B4: return 16;
case D3DFMT_A2B10G10R10: return 32;
case D3DFMT_A8B8G8R8: return 32;
case D3DFMT_X8B8G8R8: return 32;
case D3DFMT_G16R16: return 32;
case D3DFMT_A2R10G10B10: return 32;
case D3DFMT_A16B16G16R16: return 64;
case D3DFMT_A8P8: return 16;
case D3DFMT_P8: return 8;
case D3DFMT_L8: return 8;
case D3DFMT_L16: return 16;
case D3DFMT_A8L8: return 16;
case D3DFMT_A4L4: return 8;

case D3DFMT_V8U8: return 16;
case D3DFMT_Q8W8V8U8: return 32;
case D3DFMT_V16U16: return 32;
case D3DFMT_Q16W16V16U16: return 64;
case D3DFMT_CxV8U8: return 16;

case D3DFMT_L6V5U5: return 16;
case D3DFMT_X8L8V8U8: return 32;
case D3DFMT_A2W10V10U10: return 32;

case D3DFMT_D16_LOCKABLE: return 16;
case D3DFMT_D32: return 32;
case D3DFMT_D15S1: return 16;
case D3DFMT_D24S8: return 32;
case D3DFMT_D24X8: return 32;
case D3DFMT_D24X4S4: return 32;
case D3DFMT_D32F_LOCKABLE: return 32;
case D3DFMT_D24FS8: return 32;
case D3DFMT_D16: return 16;
case D3DFMT_INDEX16: return 16;
case D3DFMT_INDEX32: return 32;

case D3DFMT_R16F: return 16;
case D3DFMT_G16R16F: return 32;
case D3DFMT_A16B16G16R16F: return 64;
case D3DFMT_R32F: return 32;
case D3DFMT_G32R32F: return 64;
case D3DFMT_A32B32G32R32F: return 128;

case D3DFMT_DXT1: return 4;
case D3DFMT_DXT2:
case D3DFMT_DXT3:
case D3DFMT_DXT4:
case D3DFMT_DXT5: return 8;
default: return 32;
	}
}