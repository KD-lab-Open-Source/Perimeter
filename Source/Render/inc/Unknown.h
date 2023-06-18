#pragma once


#define UNKNOWNCLASS_USE_ATOMIC


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

#ifdef UNKNOWNCLASS_USE_ATOMIC
#include <atomic>
#endif

#ifdef PERIMETER_DEBUG
//#define C_CHECK_DELETE
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

#endif //C_CHECK_DELETE

class cUnknownHandle;


// базовый класс для всех
// любой класс наследованный как TYPE_CLASS_POINTER, должен уметь удаляться по обращению к Release()
class cUnknownClass
#ifdef C_CHECK_DELETE
: public cCheckDelete
#endif //C_CHECK_DELETE
{
private:
    friend cUnknownHandle;
    eKindUnknownClass	Kind;
    
    //Refcount for this object
#ifdef UNKNOWNCLASS_USE_ATOMIC
    std::atomic<int64_t>
#else
    int64_t
#endif
    m_cRef = 1;
    
    //Handle for this object if any
#ifdef UNKNOWNCLASS_USE_ATOMIC
    std::atomic<cUnknownHandle*>
#else
    cUnknownHandle*
#endif
    currentHandle = nullptr;
    
    void FreeHandle(cUnknownHandle* handle);
public:
	explicit cUnknownClass(int kind=KIND_NULL) : Kind(static_cast<eKindUnknownClass>(kind)) {
    }
    
	virtual ~cUnknownClass();
    
	virtual int64_t Release();
    
	inline bool GetKind(eKindUnknownClass kind) const { return Kind==kind; }
	inline eKindUnknownClass GetKind() const { return Kind; }
	inline int64_t GetRef()	const { return m_cRef; }
#ifdef PERIMETER_DEBUG
    virtual
#endif
    inline int64_t IncRef()	{ return ++m_cRef; }
    inline int64_t DecRef() {
        int64_t r = --m_cRef;
        if (0 <= r) {
            return r;
        } else {
            xassert(0);
            m_cRef = 0;
            return 0;
        }
    }

    int64_t CountHandles() const;
    cUnknownHandle* AcquireHandle();
};

class cUnknownHandle: public cUnknownClass {
private:
    friend cUnknownClass;
    cUnknownClass* ptr;

    explicit cUnknownHandle(cUnknownClass* ptr);

public:
    ~cUnknownHandle() override;

    cUnknownClass* Get() const { return ptr; };
};

template<class TP>
class cAutoRelease {
private:
    TP* ptr;
public:
    explicit cAutoRelease(TP* _ptr): ptr(_ptr) {
        xassert(ptr);
        if (ptr) {
            ptr->IncRef();
        }
    }
    
    ~cAutoRelease() {
        if (ptr) {
            ptr->Release();
        }
        ptr = nullptr;
    }

    cAutoRelease(const cAutoRelease&) = delete;
    cAutoRelease& operator=(cAutoRelease const&) = delete;
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
