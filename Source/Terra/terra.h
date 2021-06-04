#ifndef __TERRA_H__
#define __TERRA_H__


#ifdef _SURMAP_
#include "_xtool.h"
#else
#include "xutil.h"
#endif

//Global Define

const int MAX_H_SIZE_POWER=14;
const int MAX_H_SIZE=1<<MAX_H_SIZE_POWER;

const int MAX_V_SIZE_POWER=14;
const int MAX_V_SIZE=1<<MAX_V_SIZE_POWER;

const int MAX_SURFACE_TYPE = 256;
const int MAX_GEO_SURFACE_TYPE = MAX_SURFACE_TYPE;
const int MAX_DAM_SURFACE_TYPE = MAX_SURFACE_TYPE;
const int SIZE_GEO_PALETTE=MAX_GEO_SURFACE_TYPE*3; // 3-это RGB
const int SIZE_DAM_PALETTE=MAX_DAM_SURFACE_TYPE*3; // 3-это RGB

const int MAX_SURFACE_LIGHTING =128; //максимальная освещенность поверхности от 0 до 127
									 //старший бит идентифицирует Geo-0 или Dam-1

const int VX_FRACTION=5; // 6 младших бит - дробь вокселя
const int VX_FRACTION_MASK= ((1<<VX_FRACTION)-1);

const int MAX_VX_HEIGHT=((1<<(VX_FRACTION + 8))-1);//это 0x3fff//0xFFFF;
const int MIN_VX_HEIGHT=0;
const int MAX_VX_HEIGHT_WHOLE=(MAX_VX_HEIGHT>>VX_FRACTION);

const int MAX_SIMPLE_DAM_SURFACE=10;

const int MAX_RADIUS_CIRCLEARR = 210;//175;

//------Атрибуты------//
//старшие биты: 10 - выровненная поверхность; 11 - зеропласт; 01-гарь; 00-просто поверхность
#define At_ZEROPLAST (0xC0)
//#define At_ZEROPLASTEMPTY (0x40)
#define At_LEVELED (0x80)
//#define At_ZEROPLASTPRESENT (0xC0)
#define At_ZPMASK (0xC0)
#define At_NOTPURESURFACE (0xC0)
#define At_SOOT (0x40)
#define At_SOOTMASK (0xC0)

#define At_SHADOW (0x20)
#define At_SHADOWS (0x20)
#define At_SHADOWV (0x20)
#define At_WATER (0x80)


#ifdef _SURMAP_
void DrawBWCyrcle(int r, unsigned char *bitmap, int xcenter, int ycenter, int sizex, int sizey);
void DrawBWQuadrate(int r, unsigned char * bitmap, int xcenter, int ycenter, int sizex, int sizey);
#endif

//WorldGen param
const int WORLD_TYPE	       =  3;
const int BIZARRE_ROUGHNESS_MAP = 0;
const int BIZARRE_ALT_MAP	= 0;
const int NOISELEVEL	       = 0xFFff;//256;


#include "vmap.h"
#include "tools.h"
//#include "sur_scr.h"
#include "tgai.h"
#include "f3d.h"
#include "geo.h"


#endif // __TERRA_H__
