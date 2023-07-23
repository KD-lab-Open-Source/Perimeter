#ifndef PERIMETER_RENDER_TYPES_H
#define PERIMETER_RENDER_TYPES_H

using vertex_fmt_t = uint8_t;
using indices_t = uint16_t;

enum TILEMAP_DRAW
{
    TILEMAP_ALL,
    TILEMAP_ZEROPLAST,
    TILEMAP_NOZEROPLAST,
};

enum eModeRenderDevice
{
    RENDERDEVICE_MODE_WINDOW	=	1<<0,	// вывод в окно

    //d3d only
    RENDERDEVICE_MODE_RGB16		=	1<<8,	// 16 битный цвет
    RENDERDEVICE_MODE_RGB32		=	1<<9,	// 32 битный цвет
    RENDERDEVICE_MODE_COMPRESS	=	1<<10,	// компресованные текстуры
    RENDERDEVICE_MODE_VSYNC		=	1<<11,	// использовать вертикальную синхронизацию
    RENDERDEVICE_MODE_STRENCIL	=	1<<12,
    RENDERDEVICE_MODE_Z24		=	1<<13,	// 24 битный  
    RENDERDEVICE_MODE_REF		=	1<<14,
    RENDERDEVICE_MODE_MULTITHREAD = 1<<15,
    RENDERDEVICE_MODE_ALPHA		=   1<<16,  //При создании rendertarget использовать alpha 

    //Internal
    RENDERDEVICE_MODE_RETURNERROR=	1<<17, //Только для ChangeSize
    RENDERDEVICE_MODE_ONEBACKBUFFER=1<<18,
};

enum eRenderStateOption
{
    RS_ALPHA_TEST_MODE,
    RS_WIREFRAME,
    //These enums must match value with D3DRS_* equivalent for now
    RS_ZENABLE                  = 7,    /* or TRUE/FALSE for legacy */
    RS_ZWRITEENABLE             = 14,   /* TRUE to enable z writes */
    RS_ZFUNC                    = 23,   /* CMPFUNC */
    RS_CULLMODE                 = 22,   /* Cull mode */
    RS_ALPHABLENDENABLE         = 27,   /* TRUE to enable alpha blending */
    RS_FOGENABLE                = 28,   /* TRUE to enable fog blending */
    RS_FOGCOLOR                 = 34,   /* D3DCOLOR */
    RS_FOGSTART                 = 36,   /* Fog start (for both vertex and pixel fog) */
    RS_FOGEND                   = 37,   /* Fog end      */
    RS_STENCILENABLE            = 52,
    RS_BILINEAR					= 1024,	/* Change min/mag filter */
};

enum eAlphaTestMode {
    ALPHATEST_NONE,
    ALPHATEST_GT_0,
    ALPHATEST_GT_1,
    ALPHATEST_GT_254,
};

enum eCMPFUNC
{
    CMP_LESSEQUAL,
    CMP_GREATER,
    CMP_GREATEREQUAL,
    CMP_ALWAYS,
};

enum eColorMode
{
    COLOR_MOD,
    COLOR_ADD,
    COLOR_MOD2,
    COLOR_MOD4,
};
enum eBlendMode
{
    ALPHA_NONE,
    ALPHA_TEST,
    ALPHA_ADDBLENDALPHA,
    ALPHA_BLEND,
    ALPHA_ADDBLEND,
    ALPHA_SUBBLEND,
    ALPHA_MUL,
};

enum eCullMode
{
    CULL_NONE,
    CULL_CW,
    CULL_CCW,
    CULL_CAMERA
};

enum ePrimitiveType
{
    PT_TRIANGLES,
    PT_TRIANGLESTRIP,
};

#endif //PERIMETER_RENDER_TYPES_H
