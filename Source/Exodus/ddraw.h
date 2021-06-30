#ifndef PERIMETER_DDRAW_H
#define PERIMETER_DDRAW_H

#include "windows.h"

//This file contains parts of WINE project source code for d3dx9math licensed under GPLv2

//Structs copied from WINE project ddraw.h

/* set at CreateSurface() time to indicate cubic environment map */
#define DDSCAPS2_CUBEMAP                0x00000200

typedef struct _DDCOLORKEY
{
    DWORD	dwColorSpaceLowValue;/* low boundary of color space that is to
                                      * be treated as Color Key, inclusive
				      */
    DWORD	dwColorSpaceHighValue;/* high boundary of color space that is
                                       * to be treated as Color Key, inclusive
				       */
} DDCOLORKEY;

typedef struct _DDSCAPS2 {
    DWORD	dwCaps;	/* capabilities of surface wanted */
    DWORD   dwCaps2; /* additional capabilities */
    DWORD   dwCaps3; /* reserved capabilities */
    union {
        DWORD dwCaps4; /* more reserved capabilities */
        DWORD dwVolumeDepth;
    } DUMMYUNIONNAME1;
} DDSCAPS2;

typedef struct _DDPIXELFORMAT {
    DWORD	dwSize;                 /* 0: size of structure */
    DWORD	dwFlags;                /* 4: pixel format flags */
    DWORD	dwFourCC;               /* 8: (FOURCC code) */
    union {
        DWORD	dwRGBBitCount;          /* C: how many bits per pixel */
        DWORD	dwYUVBitCount;          /* C: how many bits per pixel */
        DWORD	dwZBufferBitDepth;      /* C: how many bits for z buffers */
        DWORD	dwAlphaBitDepth;        /* C: how many bits for alpha channels*/
        DWORD	dwLuminanceBitCount;
        DWORD	dwBumpBitCount;
        DWORD	dwPrivateFormatBitCount;
    } DUMMYUNIONNAME1;
    union {
        DWORD	dwRBitMask;             /* 10: mask for red bit*/
        DWORD	dwYBitMask;             /* 10: mask for Y bits*/
        DWORD	dwStencilBitDepth;
        DWORD	dwLuminanceBitMask;
        DWORD	dwBumpDuBitMask;
        DWORD	dwOperations;
    } DUMMYUNIONNAME2;
    union {
        DWORD	dwGBitMask;             /* 14: mask for green bits*/
        DWORD	dwUBitMask;             /* 14: mask for U bits*/
        DWORD	dwZBitMask;
        DWORD	dwBumpDvBitMask;
        struct {
            WORD	wFlipMSTypes;
            WORD	wBltMSTypes;
        } MultiSampleCaps;
    } DUMMYUNIONNAME3;
    union {
        DWORD   dwBBitMask;             /* 18: mask for blue bits*/
        DWORD   dwVBitMask;             /* 18: mask for V bits*/
        DWORD	dwStencilBitMask;
        DWORD	dwBumpLuminanceBitMask;
    } DUMMYUNIONNAME4;
    union {
        DWORD	dwRGBAlphaBitMask;	/* 1C: mask for alpha channel */
        DWORD	dwYUVAlphaBitMask;	/* 1C: mask for alpha channel */
        DWORD	dwLuminanceAlphaBitMask;
        DWORD	dwRGBZBitMask;		/* 1C: mask for Z channel */
        DWORD	dwYUVZBitMask;		/* 1C: mask for Z channel */
    } DUMMYUNIONNAME5;
    /* 20: next structure */
} DDPIXELFORMAT;

typedef struct _DDSURFACEDESC2
{
    DWORD	dwSize;		/* 0: size of the DDSURFACEDESC2 structure*/
    DWORD	dwFlags;	/* 4: determines what fields are valid*/
    DWORD	dwHeight;	/* 8: height of surface to be created*/
    DWORD	dwWidth;	/* C: width of input surface*/
    union {
        LONG	lPitch;	      /*10: distance to start of next line (return value only)*/
        DWORD   dwLinearSize; /*10: formless late-allocated optimized surface size */
    } DUMMYUNIONNAME1;
    union {
        DWORD	dwBackBufferCount;/* 14: number of back buffers requested*/
        DWORD	dwDepth;
    } DUMMYUNIONNAME5;
    union {
        DWORD	dwMipMapCount;/* 18:number of mip-map levels requested*/
        DWORD	dwRefreshRate;/* 18:refresh rate (used when display mode is described)*/
        DWORD   dwSrcVBHandle;/* 18:source used in VB::Optimize */
    } DUMMYUNIONNAME2;
    DWORD	dwAlphaBitDepth;/* 1C:depth of alpha buffer requested*/
    DWORD	dwReserved;	/* 20:reserved*/
    void *lpSurface;	/* 24:pointer to the associated surface memory*/
    union {
        DDCOLORKEY	ddckCKDestOverlay; /* 28: CK for dest overlay use*/
        DWORD 		dwEmptyFaceColor;  /* 28: color for empty cubemap faces */
    } DUMMYUNIONNAME3;
    DDCOLORKEY	ddckCKDestBlt;	/* 30: CK for destination blt use*/
    DDCOLORKEY	ddckCKSrcOverlay;/* 38: CK for source overlay use*/
    DDCOLORKEY	ddckCKSrcBlt;	/* 40: CK for source blt use*/

    union {
        DDPIXELFORMAT	ddpfPixelFormat;/* 48: pixel format description of the surface*/
        DWORD 		dwFVF;	/* 48: vertex format description of vertex buffers */
    } DUMMYUNIONNAME4;
    DDSCAPS2	ddsCaps;  /* 68: DDraw surface caps */
    DWORD		dwTextureStage; /* 78: stage in multitexture cascade */
} DDSURFACEDESC2;

#endif //PERIMETER_DDRAW_H
