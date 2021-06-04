#pragma once

class cMeshBank;
class cObjTile;
class cObjMesh;

enum eSceneNode
{
	SCENENODE_OBJECTFIRST	=	0,
	SCENENODE_OBJECT_TILEMAP,
	SCENENODE_OBJECT		,
	SCENENODE_OBJECTSPECIAL	,
	SCENENODE_OBJECTSPECIAL2,
	SCENENODE_OBJECTSORT_NOZ,
	MAXSCENENODE			,
	SCENENODE_OBJECTSORT	,
};

struct sViewPort
{
    int		X;
    int		Y;
    int		Width;
    int		Height;
    float	MinZ;
    float	MaxZ;
};

enum eTestVisible
{
	VISIBLE_OUTSIDE=0,
	VISIBLE_INTERSECT=1,
	VISIBLE_INSIDE=2,
};

struct IDirect3DSurface9;

class cCamera : public cUnknownClass, public sAttribute
{
protected:
	friend cScene;
	friend cBaseNode <cCamera>;
	cCamera(cScene *UClass);
	virtual ~cCamera();
public:
	//global function
	void SetCopy(cCamera* pCamera);

	virtual void PreDrawScene();
	virtual void DrawScene();

	virtual void ConvertorWorldToViewPort(const Vect3f *pw,Vect3f *pv,Vect3f *pe);
	virtual void ConvertorWorldToViewPort(const Vect3f *pw,float WorldRadius,Vect3f *pe,int *ScreenRadius);
	virtual void ConvertorWorldToCamera(const Vect3f *pw,Vect3f *pe);
	virtual void ConvertorCameraToWorld(Vect3f *pw,const Vect2f *pe);

	//По точке на экране возвращает луч в глобальных координатах
	virtual void GetWorldRay(const Vect2f& pe,Vect3f& pos,Vect3f& dir);
	// функции позиционирования камеры и изменения ее матрицы
	virtual void SetPosition(const MatXf& matrix);
	virtual void GetPosition(MatXf *Matrix,Vect3f *Pos=0);

	virtual void SetFrustum(const Vect2f *Center,const sRectangle4f *Clipping,const Vect2f *Focus,
						const Vect2f *zPlane//Near and far z plane
						);
	virtual void GetFrustum(Vect2f *Center,sRectangle4f *Clipping,Vect2f *Focus,Vect2f *zPlane);
	virtual void GetPlaneClip(sPlane4f PlaneClip[5],const sRectangle4f *Rect);


	//cIUnkObj
	inline class cScene*& GetScene()						{ return IParent; }
	void GetLighting(Vect3f& l);
	
	virtual void SetAttr(int attribute)						{ sAttribute::SetAttribute(attribute); }
	virtual void ClearAttr(int attribute=0xFFFFFFFF)		{ sAttribute::ClearAttribute(attribute); }
	virtual int GetAttr(int attribute=0xFFFFFFFF)			{ return sAttribute::GetAttribute(attribute); }

	//cCamera
	inline MatXf& GetMatrix()								{ return GlobalMatrix; }
	inline Vect3f& GetPos()									{ return Pos; }

	cCamera* GetRoot()	{return RootCamera;}
	cCamera* GetParent(){return Parent;}
	void AttachChild(cCamera *child);
	cCamera* FindCildCamera(int AttributeCamera);

	// функции для работы с пирамидой видимости
	virtual void SetClip(const sRectangle4f &clip);
	inline const sRectangle4f& GetClip()			{ return Clip; }
	inline const Vect2f& GetZPlane()				{ return zPlane; }
	void SetZPlaneTemp(Vect2f zp)					{ zPlane=zp;Update(); }

	inline float GetFocusX()						{ return Focus.x; }
	inline float GetFocusY()						{ return Focus.y; }
	inline float GetCenterX()						{ return Center.x; }
	inline float GetCenterY()						{ return Center.y; }

	inline eSceneNode GetCameraPass()				{ return camerapass; }

	// функции теста видимости
	eTestVisible TestVisible(const Vect3f &min,const Vect3f &max);
	eTestVisible TestVisibleComplete(const Vect3f &min,const Vect3f &max);
	
	eTestVisible TestVisible(const MatXf &matrix,const Vect3f &min,const Vect3f &max);
	inline eTestVisible TestVisible(const Vect3f &center,float radius=0);

	void Attach(int pos,cIUnkClass *UObject);
	inline void Attach(int pos,cIUnkClass *UObject,const MatXf &m,const Vect3f &min,const Vect3f &max);
	void Attach(class cMeshSortingPhase *pMesh);
	void AttachFirst(cIUnkClass* zpalne);
	void AttachNoRecursive(int pos,cIUnkClass* pbox);

	inline cURenderDevice* GetRenderDevice()					{ return RenderDevice; }
	inline class cD3DRender* GetRenderDevice3D()				{ return (class cD3DRender*)RenderDevice; }
	// инлайновые функции доступа к полям класса
	inline cIUnkClass*& GetDraw(int pos,int number)				{ return DrawArray[pos][number]; }
	inline int GetNumberDraw(int pos)							{ return DrawArray[pos].size(); }

	inline sPlane4f& GetPlaneClip3d(int number)					{ return PlaneClip3d[number]; }
	inline int GetNumberPlaneClip3d()							{ return PlaneClip3d_size; }

	inline const Vect2f& GetFocusViewPort()						{ return FocusViewPort; }
	inline const Vect2f& GetScaleViewPort()						{ return ScaleViewPort; }
	// функции для работы с отрисовкой
	inline const Vect2f& GetRenderSize()						{ return RenderSize; }
	inline const Vect3f& GetWorldI()							{ return WorldI; }
	inline const Vect3f& GetWorldJ()							{ return WorldJ; }
	inline const Vect3f& GetWorldK()							{ return WorldK; }

	inline cTexture* GetRenderTarget()							{ return RenderTarget; }
	inline IDirect3DSurface9* GetZBuffer()						{ return pZBuffer;}
	void SetRenderTarget(cTexture *pTexture,IDirect3DSurface9* pZBuf);

	void EnableGridTest(int grid_dx,int grid_dy,int grid_size);

	
	inline int GetHReflection(){VISASSERT(GetAttr(ATTRCAMERA_REFLECTION));return h_reflection;}

	CMatrix			matProj,matView,matViewProj,matViewProjScr;
	sViewPort		vp;

	void GetFrustumPoint(Vect3f& p00,Vect3f& p01,Vect3f& p10,Vect3f& p11,Vect3f& d00,Vect3f& d01,Vect3f& d10,Vect3f& d11,float rmul=1.0f);
	void DebugDrawFrustum();

	void AttachTestShadow(cObjectNodeRoot* obj){ShadowTestArray.push_back(obj);};
	void Update();

	Vect2f CalculateZMinMax(MatXf& view);

	bool IsBadClip();
	void DrawTestGrid();
protected: 
	void SetHReflection(int h) {h_reflection=h;}
	void UpdateVieport();

	vector<cCamera*>	child;
	cCamera*	Parent;
	cCamera*	RootCamera;

	int h_reflection;

	struct ObjectSort
	{
		float distance;
		cIUnkClass* obj;

		inline ObjectSort(){}
		inline ObjectSort(float d,cIUnkClass* o){distance=d;obj=o;}
	};

	struct ObjectSortByRadius
	{
		inline bool operator()(const ObjectSort& o1,const ObjectSort& o2)
		{
			return o1.distance>o2.distance;
		}
	};

	// первичные значения
	Vect3f				Pos;						// местоположение камеры
	// viewport
	Vect2f				Focus;			
	Vect2f				Center;
	sRectangle4f		Clip;						// Clip.left,Clip.right,Clip.top,Clip.bottom - 0..1 - размеры видимой области
	Vect2f				zPlane;
	Vect2f				OriginalzPlane;

	//new
	MatXf			GlobalMatrix;													// глобальная матрица объекта, относительно мировых координат
	cScene			*IParent;														// интерфейс породивший данный класс

	//cCamera
	cURenderDevice				*RenderDevice;				// устройство растеризации
	cTexture					*RenderTarget;				// поверхность на которую выводится
	IDirect3DSurface9*			pZBuffer;
	Vect2f						RenderSize;					// размеры устройства вывода

	enum
	{
		PlaneClip3d_size=6,
	};
	sPlane4f		PlaneClip3d[PlaneClip3d_size];				// плоскости отсечения

	vector<cIUnkClass*>			DrawArray[MAXSCENENODE];
	vector<ObjectSort>			SortArray;
	vector<cObjectNodeRoot*>	ShadowTestArray;
	Vect2f						FocusViewPort;				// фокус графического окна
	Vect2f						ScaleViewPort;				// коэффициенты неоднородности экрана по осям
	Vect3f						WorldI,WorldJ,WorldK;
protected:
	vector<cMeshSortingPhase*> arSortMaterial;
	void DrawSortMaterial();
	void DrawSortMaterialShadow();
	void DrawSortMaterialShadowStrencil();
	void DrawSortMaterialShadowStrencilOneSide();
	void DrawShadowPlane();

	vector<cIUnkClass*>	arZPlane;
	void ClearZBuffer();
	void ShowClip();

	void CalcClipPlane();

	eSceneNode camerapass;
	void DrawSortObject();
	void DrawObjectFirst();

	Vect2i TestGridSize;
	int TestGridShl;
	BYTE* pTestGrid;
	void InitGridTest(int grid_dx,int grid_dy,int grid_size);
	void CalcTestForGrid();
	inline eTestVisible GridTest(Vect3f p[8]);

	void DrawShadowDebug();
	void Set2DRenderState();
};

////////////////////inline cCamera///////////////////////////////////
inline eTestVisible cCamera::TestVisible(const Vect3f &center,float radius)
{ // для BoundingSphere с центром center и радиусом radius (при radius=0 - тест видимости точки)
	for(int i=0;i<GetNumberPlaneClip3d();i++)
		if(GetPlaneClip3d(i).GetDistance(center)<-radius)
			return VISIBLE_OUTSIDE;
	return VISIBLE_INTERSECT;
}

inline void cCamera::Attach(int pos,cIUnkClass *UObject,const MatXf &m,const Vect3f &min,const Vect3f &max)
{ 
	if(TestVisible(m,min,max))
		DrawArray[pos].push_back(UObject);
	else
		SortArray.push_back( ObjectSort(UObject->GetPosition().trans().distance(GetPos()),UObject) );

	vector<cCamera*>::iterator it;
	FOR_EACH(child,it)
	{
		cCamera* c=*it;
		if(c->GetAttribute(UObject->GetAttr(ATTRCAMERA_REFLECTION|ATTRCAMERA_SHADOW)))
			if(c->TestVisible(m,min,max))
				if( pos!=SCENENODE_OBJECTSORT )
					c->DrawArray[pos].push_back(UObject);
				else
					c->SortArray.push_back( ObjectSort(UObject->GetPosition().trans().distance(GetPos()),UObject) );
	}
}


class cCameraPlanarLight:public cCamera
{
public:
	cCameraPlanarLight(cScene *UClass):cCamera(UClass){};
	virtual void DrawScene();
};