#pragma once

enum eKindUnknownClass
{	// unsigned short
	KIND_NULL				=		0,
//	1	..	7	- интерфейсы классов
	KIND_UI_VISGENERIC		=		1,				// базовая библиотека визуализации
	KIND_UI_RENDERDEVICE	=		2,				// интерфейс для работы с растеризацией
//	8	..	15	- классы для работы с текстурами
//	16	..	23	- классы-объекты наследованы от cIUnkObj
	KIND_OBJ_NODE_ROOT		=		15,
	KIND_OBJ_NODE			=		16,				// cObjectNode - узел-группа объектов 
	KIND_OBJMESH			=		18,				// cObjMesh - класс объектa
	KIND_LIGHT				=		19,				// cUnkLight - источники света
	KIND_POLYGRID			=		20,				// cPolyGrid - регулярная полигональная сетка
	KIND_SPRITE2D			=		21,				// cSprite2d - двумерный экранный спрайт
	KIND_TILEMAP			=		22,				// cTileMap -  карта мира
	KIND_SPRITE_NODE		=		23,				// cSpriteNode - node-спрайт
	// классы спецэффектов высокого уровня реализации
	KIND_PIPE				=		24,				// cPipe - класс "водопровод" - связанные линии
	KIND_GLOBALFOG			=		27,				// cGlobalFog - класс глобального тумана
	KIND_OBJLIGHT			=		28,				// cObjLight -  класс-подобъект источника света
	KIND_ENVGRID			=		29,				// cEnvGrid -  класс полигональная карта горизонта
	KIND_EXTERNALOBJ		=		30,				// cExternalObj -  класс внешних вызовов
	KIND_FLARE				=		31,				// cFlare -  класс линзовых эффектов
	KIND_PARTICLE			=		32,				// cParticle -  класс эффектов системы частиц
	KIND_FONT				=		34,				// cFont - класс фонт
	KIND_LINE3D				=		39,				// cLine3d - Эффект линий
//	50	..	63	- прочие классы 
	KIND_SCENE				=		50,				// cScene - класс сцены 
	KIND_CAMERA				=		51,				// cCamera - класс камера
	KIND_ARRAYCAMERA		=		52,				// cUnkClassDynArrayPointer - класс накопитель cCamera
	KIND_ARRAYSCENE			=		53,				// cUnkClassDynArrayPointer - класс накопитель cScene
	KIND_DRAWNODE			=		54,				// cDrawNode - древовидный класс списоков растеризации
	KIND_TEXTURE			=		55,				// cTexture - класс текстур
	KIND_LOGIC_OBJ			=		56,				// класс логических объектов
	KIND_LOGIC_BOUND		=		57,				// класс логических боунд-объектов 
	KIND_FORCEFIELD			=		58,				// класс диспетчер регионов FieldDispatcher
	KIND_ELASTICSPHERE		=		59,				// класс для работы с ElasticSphere
	KIND_ELASTICLINK		=		60,				// класс для работы с OscillatingSphere
//	64	..	128	- классы библиотеки
	KIND_LIB_TEXTURE		=		64,				// cTextureLibrary 
	KIND_LIB_OBJECT			=		65,				// cObjectLibrary
	KIND_LIB_LOGIC			=		67,				// cLogicLibrary
	KIND_MAX
};

#ifndef _FINAL_VERSION_
#define C_CHECK_DELETE
#endif 

#ifdef C_CHECK_DELETE
class cCheckExit
{
public:
	class cCheckDelete* root;
	cCheckExit():root(0) {}
	~cCheckExit();
};

class cCheckDelete
{
	static cCheckExit root;
	cCheckDelete *next,*prev;
public:
	cCheckDelete();
	virtual ~cCheckDelete();

	static cCheckDelete* GetDebugRoot(){return root.root;}
	inline cCheckDelete* GetDebugNext(){return next;}
};

#endif C_CHECK_DELETE

// базовый класс для всех
// любой класс наследованный как TYPE_CLASS_POINTER, должен уметь удаляться по обращению к Release()
class cUnknownClass
#ifdef C_CHECK_DELETE
: public cCheckDelete
#endif C_CHECK_DELETE
{	
	int					m_cRef;
	eKindUnknownClass	Kind;
public:
	cUnknownClass(int kind=KIND_NULL)
	{ 
		m_cRef=1; 
		Kind=eKindUnknownClass(kind);
	}
	virtual ~cUnknownClass()								
	{ 
	}
	virtual int Release()
	{ 
		if(DecRef()>0) 
			return m_cRef;
		delete this;
		return 0;
	}
	inline int GetKind(int kind) const								{ return Kind==kind; }
	inline int GetKind() const										{ return Kind; }
	inline int GetRef()	const										{ return m_cRef; }
	inline int IncRef()												{ return ++m_cRef; }
	inline int DecRef()												{ return --m_cRef; }
};

#define RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

template <class xvector>
void remove_null_element(xvector& ar)
{
	int cur=0;
	for(int i=0;i<ar.size();i++)
	if(ar[i])
	{
		ar[cur]=ar[i];
		cur++;
	}

	ar.resize(cur);
}
