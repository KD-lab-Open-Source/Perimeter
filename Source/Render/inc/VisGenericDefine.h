#ifndef _VISGENERIC_DEFINE_H_
#define _VISGENERIC_DEFINE_H_

#pragma once

enum eAttributeCamera
{
// general
	ATTRCAMERA_IGNORE				=	1<<0,	// камера игнорируется
// private
	ATTRCAMERA_PERSPECTIVE			=	1<<4,	// перспектива
	ATTRCAMERA_REFLECTION			=	1<<16,	// камера рендерит портал-отражение
	ATTRCAMERA_SHADOW				=	1<<17,	// камера рендерит портал-тень
	ATTRCAMERA_SHADOW_STRENCIL		=	1<<18,
	ATTRCAMERA_CLEARZBUFFER			=	1<<19,
	ATTRCAMERA_SHOWCLIP				=	1<<20,
	ATTRCAMERA_SHADOWMAP			=	1<<21,
	ATTRCAMERA_ZMINMAX				=	1<<22,
	ATTRCAMERA_NOT_CALC_PROJ		=	1<<23,
	ATTRCAMERA_ZINVERT				=	1<<24,
	ATTRCAMERA_NOCLEARTARGET		=	1<<25,
};

enum eAttributeUnkObj
{
// general
	ATTRUNKOBJ_IGNORE				=	1<<0,	// объект игнорируется = является невидимымы = не выводится
	ATTRUNKOBJ_DELETED				=	1<<1,
//	ATTRUNKOBJ_TEMP_USED			=	1<<2,		
	ATTRUNKOBJ_CREATED_IN_LOGIC		=	1<<3,	//Объекты созданные в логическом потоке должны удаляться после того как на них нет ссылок в интерполяции
// private

	ATTRUNKOBJ_ADDBLEND				=	1<<7,	// сложение цветов, должен совпадать с MAT_ALPHA_ADDBLEND
	ATTRUNKOBJ_COLLISIONTRACE		=	1<<9,	// учитывать при трассировке
	ATTRUNKOBJ_WIREFRAME			=	1<<11,	// тестовая фича, вывод граней объекта 

	ATTRUNKOBJ_REFLECTION			=	ATTRCAMERA_REFLECTION,//==16 объект может отражаться
	ATTRUNKOBJ_SHADOW				=	ATTRCAMERA_SHADOW,	//==17 объект откидывает правильную тень
	ATTRUNKOBJ_SHOWBOUND			=	1<<19,	// показывать боунд

	ATTRUNKOBJ_IGNORE_NORMALCAMERA	=	1<<20,	// объект не выводится в нормальной камере

	ATTRUNKOBJ_NOLIGHT				=	1<<31,	// объект не освещается источниками света сцены
};

enum eAttributeLight
{
// general
	ATTRLIGHT_IGNORE				=	1<<0,	// источник света игнорируется = является невидимымы = не выводится
// private
	ATTRLIGHT_DIRECTION				=	1<<4,	// напрвленный источник света
	ATTRLIGHT_SPHERICAL				=	1<<5,	// сферический источник света, отбрасывает свет на объекты
	ATTRLIGHT_SPHERICAL_SPRITE		=	1<<6,	// источник света рисуется кучей спрайтов
};

#endif // _VISGENERIC_DEFINE_H_
