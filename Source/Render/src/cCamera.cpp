#include "StdAfxRD.h"
#include "cCamera.h"
#include "Scene.h"
#include "MeshBank.h"
#include "ObjMesh.h"
#include <algorithm>
#include "TileMap.h"
#include "font.h"

class CameraShader
{
public:
	PSShowMap* pShowMap;

	CameraShader()
	{
		pShowMap=new PSShowMap;
		pShowMap->Restore();
	}

	~CameraShader()
	{
		if(pShowMap)delete pShowMap;
	}

};

void cVisGeneric::InitShaders()
{
	if(!shaders)
		shaders=new CameraShader;
}

void cVisGeneric::ReleaseShaders()
{
	if(shaders)
	{
		delete shaders;
		shaders=NULL;
	}
}

void GetStack(string& out);
cCamera::cCamera(cScene *UClass) : cUnknownClass(KIND_DRAWNODE)
{
	Identity(GlobalMatrix);
	IParent=NULL;
	
	Pos.set(0.f,0.f,0.f);
	Center.set(0.5f,0.5f);
	zPlane.set(1,1e6);
	OriginalzPlane=zPlane;

	Clip.set(0.01f,0.01f,0.99f,0.99f);

	RenderTarget=0;
	pZBuffer=NULL;

	RenderDevice=0;

	Focus.set(1,1);
	FocusViewPort.set(0,0);
	ScaleViewPort.set(0,0);
	RenderSize.set(0,0);

	camerapass=SCENENODE_OBJECT;

	pTestGrid=NULL;
	TestGridShl=0;
	RootCamera=this;
	Parent=NULL;

	VISASSERT(gb_RenderDevice);
	RenderDevice=gb_RenderDevice;
	GetScene() = UClass;

	h_reflection=0;
}
cCamera::~cCamera()
{
	PreDrawScene();

	delete pTestGrid;
}

void TempDrawShadow(cCamera* p);

void cCamera::DrawScene()
{
	if(IsBadClip())return;
	if(!Parent)
	{
		RenderDevice->FlushPrimitive3D();
		RenderDevice->SetRenderState(RS_ZFUNC,CMP_LESSEQUAL);
  		RenderDevice->SetRenderState(RS_BILINEAR,TRUE);
	}

	vector<cCamera*>::iterator it_c;
	FOR_EACH(child,it_c)
	if(!(*it_c)->GetAttribute(ATTRCAMERA_SHADOW_STRENCIL))
		(*it_c)->DrawScene();

	RenderDevice->SetDrawNode(this);

	if(!Option_ShowType[SHOW_REDLECTION] && GetAttribute(ATTRCAMERA_REFLECTION))
		return;
	if(!Option_ShowType[SHOW_SHADOW] && GetAttribute(ATTRCAMERA_SHADOW|ATTRCAMERA_SHADOWMAP))
		return;
	if(!Option_ShowType[SHOW_NORMAL] && !Parent)
	{
		Set2DRenderState();
		return;
	}

	if(GetAttribute(ATTRCAMERA_CLEARZBUFFER) || 
		(!Parent && FindCildCamera(ATTRCAMERA_REFLECTION)) )
		ClearZBuffer();
	if(GetAttribute(ATTRCAMERA_SHOWCLIP))
		ShowClip();

	DrawObjectFirst();

	if(GetAttribute(ATTRUNKOBJ_NOLIGHT)==0) 
	{ // объекты освещаются
		Vect3f vLight(0,0,0);
		sColor4f Ambient(0,0,0,0),Diffuse(0,0,0,0),Specular(0,0,0,0);
		if(GetScene()->GetLighting(Ambient,Diffuse,Specular,vLight))
			RenderDevice->SetGlobalLight(&vLight,&Ambient,&Diffuse,&Specular);
		else
			RenderDevice->SetGlobalLight(NULL);
	}else // объекты не освещаются
		RenderDevice->SetGlobalLight(NULL);

	RenderDevice->SetRenderState( RS_ZWRITEENABLE, TRUE );

	DWORD fogenable=GetRenderDevice3D()->GetRenderState(D3DRS_FOGENABLE);
	if(GetAttribute(ATTRCAMERA_SHADOW|ATTRCAMERA_SHADOWMAP|ATTRCAMERA_SHADOW_STRENCIL))
		RenderDevice->SetRenderState(RS_FOGENABLE,FALSE);
	
//	if(GetAttribute(ATTRCAMERA_ZMINMAX))
//	{
//		zPlane=OriginalzPlane;
//		Update();
//	}

	{
		int number_draw=GetNumberDraw(SCENENODE_OBJECT_TILEMAP);
		for( int nObj=0; nObj<number_draw; nObj++ )
			GetDraw(SCENENODE_OBJECT_TILEMAP,nObj)->Draw(this);
	}
	
	if(Option_ShowType[SHOW_OBJECT])
	{
		if(GetAttribute(ATTRCAMERA_SHADOW_STRENCIL))
			DrawSortMaterialShadowStrencil();
		else
		if(GetAttribute(ATTRCAMERA_SHADOW|ATTRCAMERA_SHADOWMAP))
			DrawSortMaterialShadow();
		else 
			DrawSortMaterial();
	}
	
	if(Option_ShowType[SHOW_OBJECT])
	{
		for(int nType=SCENENODE_OBJECT;nType<MAXSCENENODE;nType++)
		{
			camerapass=(eSceneNode)nType;
			if( nType == SCENENODE_OBJECTSPECIAL )
			{
				if(!Parent)
				{

					vector<cIUnkClass*>::iterator it;
					FOR_EACH(arZPlane,it)
					{
						(*it)->Draw(this);
					}

				}

				RenderDevice->SetRenderState( RS_CULLMODE, D3DCULL_NONE );
			}

			if(nType==SCENENODE_OBJECTSORT_NOZ)
			{
			}else
			{
				vector<cIUnkClass*>& obj=DrawArray[nType];
				vector<cIUnkClass*>::iterator it;
		
				FOR_EACH(obj,it)
				{
					(*it)->Draw(this);
				}
			}

			if( nType == SCENENODE_OBJECTSPECIAL )
			{
				DrawSortObject();

				camerapass=SCENENODE_OBJECTSORT_NOZ;
				vector<cIUnkClass*>& obj=DrawArray[SCENENODE_OBJECTSORT_NOZ];
				if(!obj.empty())
				{
					vector<cIUnkClass*>::iterator it;

					DWORD zfunc=gb_RenderDevice3D->GetRenderState(D3DRS_ZFUNC);
					gb_RenderDevice3D->SetRenderState( D3DRS_ZFUNC, D3DCMP_GREATER);
					DWORD zwrite=gb_RenderDevice3D->GetRenderState(D3DRS_ZWRITEENABLE);
					gb_RenderDevice3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
					DWORD fogenable=gb_RenderDevice3D->GetRenderState(D3DRS_FOGENABLE);
					gb_RenderDevice3D->SetRenderState(D3DRS_FOGENABLE,FALSE);
					gb_RenderDevice3D->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
			
					FOR_EACH(obj,it)
					{
						(*it)->Draw(this);
					}

					gb_RenderDevice3D->SetRenderState( D3DRS_ZFUNC, zfunc );
					gb_RenderDevice3D->SetRenderState( D3DRS_ZWRITEENABLE, zwrite );
					gb_RenderDevice3D->SetRenderState( D3DRS_FOGENABLE,fogenable);
				}

				RenderDevice->SetRenderState( RS_CULLMODE, -1 );
			}
		}
	}else
	{
			int number_draw=GetNumberDraw(SCENENODE_OBJECT);
			for( int nObj=0; nObj<number_draw; nObj++ )
			if(GetDraw(SCENENODE_OBJECT,nObj)->GetKind()==KIND_TILEMAP)
			{
				GetDraw(SCENENODE_OBJECT,nObj)->Draw(this);
			}

	}

	if(!Parent)
	if(Option_DebugShowShadowVolume)
	{
		RenderDevice->SetRenderState( RS_CULLMODE, D3DCULL_NONE );
		RenderDevice->SetNoMaterial(ALPHA_BLEND);
		
		for(int i=0;i<ShadowTestArray.size();i++)
			ShadowTestArray[i]->DrawShadow(this);
		RenderDevice->SetRenderState( RS_CULLMODE, -1 );
	}

	RenderDevice->FlushPrimitive3D();

	if(Option_ShowRenderTextureDBG==3)
		TempDrawShadow(this);
	
	RenderDevice->SetRenderState(RS_FOGENABLE,fogenable);

	FOR_EACH(child,it_c)
	if((*it_c)->GetAttribute(ATTRCAMERA_SHADOW_STRENCIL))
		(*it_c)->DrawScene();
	
  	if(!Parent)
	{
		DrawShadowDebug();
		Set2DRenderState();
	}


	if(!Parent && false)
	{
		cCamera* pShadow=FindCildCamera(ATTRCAMERA_SHADOWMAP);
		if(pShadow)
		{
			cFont* pFont=gb_VisGeneric->CreateDebugFont();
			RenderDevice->SetFont(pFont);
			char s[128];
			sprintf(s,"ZMin=%f",pShadow->zPlane.x);
			RenderDevice->OutText(800,10,s,sColor4f(0,0,1,1));
			sprintf(s,"ZMax=%f",pShadow->zPlane.y);
			RenderDevice->OutText(800,30,s,sColor4f(0,0,1,1));
			sprintf(s,"pos=%.0f,%.0f,%.0f",Pos.x,Pos.y,Pos.z);
			RenderDevice->OutText(800,50,s,sColor4f(0,0,1,1));

			RenderDevice->SetFont(NULL);
			pFont->Release();
		}
	}

//	if(!Parent)DrawTestGrid();
}

void cCamera::Set2DRenderState()
{
  	RenderDevice->SetRenderState(RS_BILINEAR,FALSE);
  	RenderDevice->SetRenderState(RS_ZFUNC,CMP_ALWAYS);
	RenderDevice->SetRenderState(RS_FOGENABLE,FALSE);
}

void cCamera::PreDrawScene()
{
	child.clear();

	for(int j=0;j<MAXSCENENODE;j++)
		DrawArray[j].clear();

	SortArray.clear();
	arSortMaterial.clear();
	arZPlane.clear();
	ShadowTestArray.clear();
}

void cCamera::SetClip(const sRectangle4f &clip) 
{
	Clip=clip;
	Update();
}

/*
Ortho
2/w  0    0           0
0    2/h  0           0
0    0    1/(zf-zn)   0
0    0    zn/(zn-zf)  1

Perspective
2*zn/w  0       0              0
0       2*zn/h  0              0
0       0       zf/(zf-zn)     1
0       0       zn*zf/(zn-zf)  0
*/

void cCamera::Update() 
{ 
	UpdateVieport();

	if(!GetAttribute(ATTRCAMERA_NOT_CALC_PROJ))
	{
		memset(&matProj,0,sizeof(matProj));
		if(GetAttribute(ATTRCAMERA_PERSPECTIVE))
		{
			float zc=zPlane.y/(zPlane.y-zPlane.x);
			matProj._11=2*Focus.x*ScaleViewPort.x/(Clip.xmax()-Clip.xmin());
			matProj._22=2*Focus.y*ScaleViewPort.y/(Clip.ymax()-Clip.ymin());
			matProj._33=zc;
			matProj._43=-zc*zPlane.x;

			matProj._31=0;
			matProj._32=0;
			matProj._34=1;
		}else
		{
			matProj._11=2*Focus.x*ScaleViewPort.x/(Clip.xmax()-Clip.xmin());
			matProj._22=2*Focus.y*ScaleViewPort.y/(Clip.ymax()-Clip.ymin());
			matProj._33=1/(zPlane.y-zPlane.x);
			matProj._43=zPlane.x/(zPlane.x-zPlane.y);
			matProj._41=0;
			matProj._42=0;
			matProj._44=1;
		}
	}

	vp.X = round((GetCenterX()+Clip.xmin())*RenderSize.x);
	vp.Y = round((GetCenterY()+Clip.ymin())*RenderSize.y);
	vp.Width = round((GetCenterX()+Clip.xmax())*RenderSize.x)-vp.X;
	vp.Height = round((GetCenterY()+Clip.ymax())*RenderSize.y)-vp.Y;
	vp.MinZ=0; vp.MaxZ=1;

	matView.set( 
		GetMatrix().rot()[0][0], GetMatrix().rot()[1][0], GetMatrix().rot()[2][0], 0,
		GetMatrix().rot()[0][1], GetMatrix().rot()[1][1], GetMatrix().rot()[2][1], 0,
		GetMatrix().rot()[0][2], GetMatrix().rot()[1][2], GetMatrix().rot()[2][2], 0,
		GetMatrix().trans().x,   GetMatrix().trans().y,   GetMatrix().trans().z,   1 );
	matViewProj = matView * matProj;

	CMatrix matScr( 
		+vp.Width/2,			0,					0,				0,
		0,					-vp.Height/2,			0,				0,
		0,						0,				vp.MaxZ-vp.MinZ,	0,
		vp.X+vp.Width/2,	vp.Y+vp.Height/2,	vp.MinZ,			1 );
	matViewProjScr = matViewProj*matScr;

	WorldI=GetMatrix().rot().xrow();
	WorldJ=GetAttribute(ATTRCAMERA_REFLECTION)?-GetMatrix().rot().yrow():GetMatrix().rot().yrow();
	WorldK=GetMatrix().rot().zrow();
	// определение местоположения камеры в мировом пространстве
	GetPos() = GetMatrix().invXformPoint(Vect3f(0,0,0));

	CalcClipPlane();
}

/*
Perspective
2*zn/w  0       0              0
0       2*zn/h  0              0
0       0       zf/(zf-zn)     1
0       0       zn*zf/(zn-zf)  0

Orto
2/w  0    0           0
0    2/h  0           0
0    0    1/(zf-zn)   0
0    0    zn/(zn-zf)  1
*/
void cCamera::CalcClipPlane()
{
	if(GetAttribute(ATTRCAMERA_PERSPECTIVE))
	{
		sPlane4f* v=PlaneClip3d;
		Vect3f p00,p01,p10,p11,d00,d01,d10,d11;
		GetFrustumPoint(p00,p01,p10,p11,d00,d01,d10,d11);

		if(GetAttribute(ATTRCAMERA_REFLECTION)!=0)
		{
			v[0].Set(d00,d01,d11);			// far z
			v[1].Set(p00,p11,p01);			// near z
			v[2].Set(GetPos(),p00,p10);		// left
			v[3].Set(GetPos(),p11,p01);		// right
			v[4].Set(GetPos(),p10,p11);		// top
			v[5].Set(GetPos(),p01,p00);		// bottom
		}
		else
		{
			v[0].Set(d00,d11,d01);			// far z
			v[1].Set(p00,p01,p11);			// near z
			v[2].Set(GetPos(),p10,p00);		// left
			v[3].Set(GetPos(),p01,p11);		// right
			v[4].Set(GetPos(),p11,p10);		// top
			v[5].Set(GetPos(),p00,p01);		// bottom
		}
	}else
	{
		//float div=GetPos().z/GetFocusViewPort().x;
		//Vect2f w(GetFocusViewPort().x/GetScaleViewPort().x*div,GetFocusViewPort().y/GetScaleViewPort().y*div);
		Vect2f w(1/(Focus.x*ScaleViewPort.x),1/(Focus.y*ScaleViewPort.y));

		Vect3f p00,p01,p11,p10,d00,d01,d10,d11;
		GetMatrix().invXformPoint(Vect3f(Clip.xmin()*w.x,Clip.ymin()*w.y,zPlane.x),p00);
		GetMatrix().invXformPoint(Vect3f(Clip.xmax()*w.x,Clip.ymin()*w.y,zPlane.x),p01);
		GetMatrix().invXformPoint(Vect3f(Clip.xmax()*w.x,Clip.ymax()*w.y,zPlane.x),p11);
		GetMatrix().invXformPoint(Vect3f(Clip.xmin()*w.x,Clip.ymax()*w.y,zPlane.x),p10);

		GetMatrix().invXformPoint(Vect3f(Clip.xmin()*w.x,Clip.ymin()*w.y,zPlane.y),d00);
		GetMatrix().invXformPoint(Vect3f(Clip.xmax()*w.x,Clip.ymin()*w.y,zPlane.y),d01);
		GetMatrix().invXformPoint(Vect3f(Clip.xmin()*w.x,Clip.ymax()*w.y,zPlane.y),d10);
		GetMatrix().invXformPoint(Vect3f(Clip.xmax()*w.x,Clip.ymax()*w.y,zPlane.y),d11);

		PlaneClip3d[0].Set(d00,d11,d01);			// far z
		PlaneClip3d[1].Set(p00,p01,p11);			// near z
		PlaneClip3d[2].Set(d10,p00,p10);			// left
		PlaneClip3d[3].Set(d01,p11,p01);			// right
		PlaneClip3d[4].Set(d11,p10,p11);			// top
		PlaneClip3d[5].Set(d00,p01,p00);			// bottom
	}
}

void cCamera::GetFrustumPoint(Vect3f& p00,Vect3f& p01,Vect3f& p10,Vect3f& p11,Vect3f& d00,Vect3f& d01,Vect3f& d10,Vect3f& d11,float rmul)
{
	VISASSERT(GetAttribute(ATTRCAMERA_PERSPECTIVE));
	float rx,ry;
	rx=1;ry=RenderSize.y/(float)RenderSize.x;
	rx*=rmul;ry*=rmul;

	float cx=(Clip.xmin()+Clip.xmax())*0.5f,cy=(Clip.ymin()+Clip.ymax())*0.5f;
	float xi=Clip.xmin()-cx,xa=Clip.xmax()-cx;
	float yi=Clip.ymin()-cy,ya=Clip.ymax()-cy;

	GetMatrix().invXformPoint(Vect3f(xi*rx,yi*ry,GetFocusX())*zPlane.x,p00);
	GetMatrix().invXformPoint(Vect3f(xa*rx,yi*ry,GetFocusX())*zPlane.x,p01);
	GetMatrix().invXformPoint(Vect3f(xa*rx,ya*ry,GetFocusX())*zPlane.x,p11);
	GetMatrix().invXformPoint(Vect3f(xi*rx,ya*ry,GetFocusX())*zPlane.x,p10);
	GetMatrix().invXformPoint(Vect3f(xi*rx,yi*ry,          1)*zPlane.y,d00);
	GetMatrix().invXformPoint(Vect3f(xa*rx,yi*ry,          1)*zPlane.y,d01);
	GetMatrix().invXformPoint(Vect3f(xi*rx,ya*ry,          1)*zPlane.y,d10);
	GetMatrix().invXformPoint(Vect3f(xa*rx,ya*ry,          1)*zPlane.y,d11);
}

void cCamera::DebugDrawFrustum()
{
	Vect3f p00,p01,p10,p11,d00,d01,d10,d11;
	GetFrustumPoint(p00,p01,p10,p11,d00,d01,d10,d11,0.95f);
	Vect3f pt00,pt01,pt10,pt11;
	float b=0.5f,m=0.5f;
	pt00=p00*b+d00*m;
	pt01=p01*b+d01*m;
	pt10=p10*b+d10*m;
	pt11=p11*b+d11*m;

	sColor4c color(255,255,255,255);
	gb_RenderDevice->DrawLine(pt00,pt01,color);
	gb_RenderDevice->DrawLine(pt01,pt11,color);
	gb_RenderDevice->DrawLine(pt11,pt10,color);
	gb_RenderDevice->DrawLine(pt10,pt00,color);
}

void cCamera::GetPlaneClip(sPlane4f PlaneClip[5],const sRectangle4f *Rect) 
{ 
	if(GetAttribute(ATTRCAMERA_PERSPECTIVE))
	{
		float cx=(Clip.xmin()+Clip.xmax())*0.5f,cy=(Clip.ymin()+Clip.ymax())*0.5f;
		float xi=Rect->xmin()-cx,xa=Rect->xmax()-cx;
		float yi=-(Rect->ymin()-cy),ya=-(Rect->ymax()-cy);

		Vect2f focus(zPlane.x/(GetScaleViewPort().x*GetFocusX()),zPlane.x/(GetScaleViewPort().y*GetFocusY()));
		Vect3f Center=GetMatrix().invXformPoint(Vect3f(0,0,0));
		Vect3f p00=GetMatrix().invXformPoint(Vect3f(xi*focus.x,yi*focus.y,zPlane.x),p00);
		Vect3f p01=GetMatrix().invXformPoint(Vect3f(xa*focus.x,yi*focus.y,zPlane.x),p01);
		Vect3f p11=GetMatrix().invXformPoint(Vect3f(xa*focus.x,ya*focus.y,zPlane.x),p11);
		Vect3f p10=GetMatrix().invXformPoint(Vect3f(xi*focus.x,ya*focus.y,zPlane.x),p10);
		PlaneClip[0].Set(p00,p11,p01);				// near z
		PlaneClip[1].Set(Center,p00,p10);			// left
		PlaneClip[2].Set(Center,p11,p01);			// right
		PlaneClip[3].Set(Center,p10,p11);			// top
		PlaneClip[4].Set(Center,p01,p00);			// bottom
	}else
	{
		float div=GetPos().z/GetFocusViewPort().x;
		Vect2f wCamera(GetFocusViewPort().x/GetScaleViewPort().x*div,GetFocusViewPort().y/GetScaleViewPort().y*div);
		Vect3f p010=GetMatrix().invXformPoint(Vect3f(Rect->xmax()*wCamera.x,Rect->ymin()*wCamera.y,0),p010);
		Vect3f p100=GetMatrix().invXformPoint(Vect3f(Rect->xmin()*wCamera.x,Rect->ymax()*wCamera.y,0),p100);
		Vect3f p001=GetMatrix().invXformPoint(Vect3f(Rect->xmin()*wCamera.x,Rect->ymin()*wCamera.y,zPlane.x),p001);
		Vect3f p011=GetMatrix().invXformPoint(Vect3f(Rect->xmax()*wCamera.x,Rect->ymin()*wCamera.y,zPlane.x),p011);
		Vect3f p111=GetMatrix().invXformPoint(Vect3f(Rect->xmax()*wCamera.x,Rect->ymax()*wCamera.y,zPlane.x),p111);
		Vect3f p101=GetMatrix().invXformPoint(Vect3f(Rect->xmin()*wCamera.x,Rect->ymax()*wCamera.y,zPlane.x),p101);
		PlaneClip[0].Set(p001,p111,p011);			// near z
		PlaneClip[1].Set(p100,p001,p101);			// left
		PlaneClip[2].Set(p010,p111,p011);			// right
		PlaneClip[3].Set(p100,p101,p111);			// top
		PlaneClip[4].Set(p010,p011,p001);			// bottom
	}
}
void cCamera::SetPosition(const MatXf& matrix)
{ 
	GetMatrix()=matrix; 

	Update();
}
void cCamera::GetPosition(MatXf *matrix,Vect3f *pos)
{ 
	if(matrix) 
		*matrix=GetMatrix(); 
	if(pos) 
		*pos=Pos; 
}

void cCamera::SetFrustum(const Vect2f *center,const sRectangle4f *clip,const Vect2f *focus,const Vect2f *zplane)
{
	if(clip) Clip=*clip;
	if(center) Center=*center;
	if(focus) Focus=*focus;
	if(zplane)
		OriginalzPlane=zPlane=*zplane;

	Update();
}

void cCamera::GetFrustum(Vect2f *center,sRectangle4f *clip,Vect2f *focus,Vect2f *zplane)
{
	if(clip) *clip=Clip; 
	if(center) center->set(GetCenterX(),GetCenterY());
	if(focus) focus->set(GetFocusX(),GetFocusY()); 
	if(zplane) *zplane=zPlane; 
}

void cCamera::UpdateVieport()
{
	// обновление данных ViewPort
	if(RenderTarget==0)
		RenderSize.set(RenderDevice->GetSizeX(),RenderDevice->GetSizeY());
	else
		RenderSize.set(RenderTarget->GetWidth(),RenderTarget->GetHeight());

	FocusViewPort.set(GetFocusX()*RenderSize.x,GetFocusY()*RenderSize.x);
	ScaleViewPort.set(1,RenderSize.x/RenderSize.y);
}

void cCamera::SetRenderTarget(cTexture *pTexture,LPDIRECT3DSURFACE9 pZBuf)
{
	RenderTarget=pTexture;
	pZBuffer=pZBuf;
	UpdateVieport();
}

void cCamera::ConvertorWorldToViewPort(const Vect3f *pw,Vect3f *pv,Vect3f *pe)
{
	Vect3f pv0,pe0;
	if(pw==0) return;
	if(pv==0) pv=&pv0;
	if(pe==0) pe=&pe0;
	matViewProjScr.Convert( *pw, *pv, *pe );
}

void cCamera::ConvertorWorldToViewPort(const Vect3f *pw,float WorldRadius,Vect3f *pe,int *ScreenRadius)
{
	Vect3f pv,pe0;
	if(pw==0) return;
	if(pe==0) pe=&pe0;
	matViewProjScr.Convert( *pw, pv, *pe );
	if(ScreenRadius) *ScreenRadius=round(WorldRadius*GetFocusViewPort().x/pv.z);
}

void cCamera::ConvertorWorldToCamera(const Vect3f *pw,Vect3f *pe)
{
	if(pw==0||pe==0) return;
	float d;
	GetMatrix().xformPoint( *pw, *pe );
	if( pe->z ) d=1/pe->z; else d=1e30f;
	pe->x=GetScaleViewPort().x*pe->x*d;
	pe->y=GetScaleViewPort().y*pe->y*d;
}

void cCamera::ConvertorCameraToWorld(Vect3f *pw,const Vect2f *pe)
{
	float x,y;
	float cx=(Clip.xmin()+Clip.xmax())*0.5f,
		  cy=(Clip.ymin()+Clip.ymax())*0.5f;
	x=pe->x-cx-(Center.x-0.5f);
	y=pe->y-cy-(Center.y-0.5f);

	Vect2f focus;
	if(GetAttribute(ATTRCAMERA_PERSPECTIVE))
		focus.set(zPlane.x/(GetScaleViewPort().x*GetFocusX()),
				  zPlane.x/(GetScaleViewPort().y*GetFocusY()));
	else
		focus.set(1/(GetScaleViewPort().x*GetFocusX()),1/(GetScaleViewPort().y*GetFocusY()));

	GetMatrix().invXformPoint(Vect3f(x*focus.x,-y*focus.y,zPlane.x),*pw);
}

void cCamera::GetWorldRay(const Vect2f& pos_in,Vect3f& pos,Vect3f& dir)
{
	if(GetAttribute(ATTRCAMERA_PERSPECTIVE))
	{
		ConvertorCameraToWorld(&pos,&pos_in);
		dir=pos-GetPos();
		dir.normalize();
	}else
	{
		ConvertorCameraToWorld(&pos,&pos_in);
		dir=GetWorldK();
	}
}

void cCamera::SetCopy(cCamera* DrawNode)
{
	DrawNode->Attribute=Attribute;
	DrawNode->Pos=Pos;
	DrawNode->Focus=Focus;
	DrawNode->Center=Center;
	DrawNode->Clip=Clip;
	DrawNode->zPlane=zPlane;

	DrawNode->GlobalMatrix=GlobalMatrix;
	DrawNode->IParent=IParent;

	DrawNode->Pos=Pos;
	DrawNode->matProj=matProj;
	DrawNode->matView=matView;
	DrawNode->matViewProj=matViewProj;
	DrawNode->zPlane=zPlane;

	int i;
	for(i=0;i<PlaneClip3d_size;i++)
		DrawNode->PlaneClip3d[i]=PlaneClip3d[i];

	DrawNode->RenderDevice=RenderDevice;
	DrawNode->FocusViewPort=FocusViewPort;
	DrawNode->ScaleViewPort=ScaleViewPort;
	DrawNode->RenderSize=RenderSize;
	DrawNode->RenderTarget=GetRenderTarget();

	for(i=0;i<MAXSCENENODE;i++)
		DrawNode->DrawArray[i].clear();
	DrawNode->SortArray.clear();
	DrawNode->arSortMaterial.clear();
	DrawNode->arZPlane.clear();
	
}

void cCamera::DrawSortObject()
{
	camerapass=SCENENODE_OBJECTSORT;
	RenderDevice->SetRenderState( RS_ZWRITEENABLE, FALSE );
//	RenderDevice->SetRenderState(RS_ZFUNC,CMP_LESSEQUAL);
//	RenderDevice->SetRenderState( RS_CULLMODE, D3DCULL_NONE );

	stable_sort(SortArray.begin(),SortArray.end(),ObjectSortByRadius());

	DWORD fogenable=GetRenderDevice3D()->GetRenderState(D3DRS_FOGENABLE);
	RenderDevice->SetRenderState(RS_FOGENABLE,FALSE);

	vector<ObjectSort>::iterator it;
	FOR_EACH( SortArray, it )
	{
		it->obj->Draw(this);
	}

	if(false)
	{
				vector<cIUnkClass*>& obj=DrawArray[SCENENODE_OBJECTSORT_NOZ];
				vector<cIUnkClass*>::iterator it;
		
				FOR_EACH(obj,it)
				{
					(*it)->Draw(this);
				}
	}

	RenderDevice->SetRenderState(RS_FOGENABLE,fogenable);
	RenderDevice->SetRenderState( RS_ZWRITEENABLE, TRUE );
//	RenderDevice->SetRenderState( RS_CULLMODE, -1 );
}

void cCamera::AttachNoRecursive(int pos,cIUnkClass* UObject)
{
	if(!UObject->GetAttr(ATTRUNKOBJ_IGNORE_NORMALCAMERA))
		DrawArray[pos].push_back(UObject);
}

void cCamera::Attach(int pos,cIUnkClass *UObject)
{ 
	vector<cCamera*>::iterator it_c;
	if( pos!=SCENENODE_OBJECTSORT )
	{
		if(!UObject->GetAttr(ATTRUNKOBJ_IGNORE_NORMALCAMERA))
			DrawArray[pos].push_back(UObject);
		FOR_EACH(child,it_c)
		if((*it_c)->GetAttribute(UObject->GetAttr(ATTRCAMERA_REFLECTION|ATTRCAMERA_SHADOW)))
			(*it_c)->DrawArray[pos].push_back(UObject);
	}else
	{
		Vect3f cur_pos=UObject->GetCenterObject();
		float distance=cur_pos.distance(GetPos());
		if(!UObject->GetAttr(ATTRUNKOBJ_IGNORE_NORMALCAMERA))
			SortArray.push_back( ObjectSort(distance,UObject) );
		FOR_EACH(child,it_c)
		if((*it_c)->GetAttribute(UObject->GetAttr(ATTRCAMERA_REFLECTION|ATTRCAMERA_SHADOW)))
		{
			float distance=cur_pos.distance((*it_c)->GetPos());
			(*it_c)->SortArray.push_back( ObjectSort(distance,UObject) );
		}
	}
}

void cCamera::Attach(cMeshSortingPhase *sp)
{
	arSortMaterial.push_back(sp);
}

struct SortMaterialByNodeBank
{
	inline bool operator ()(const cMeshSortingPhase* s1,const cMeshSortingPhase* s2)
	{
		if(s1->pBank<s2->pBank)
			return true;
		if(s1->pBank==s2->pBank)
		{
			if(s1->channel<s2->channel)
				return true;
			if(s1->channel==s2->channel)
			{
				if(s1->diffuse<s2->diffuse)
					return true;
				if(s1->diffuse==s2->diffuse)
				{
					if(s1->ambient<s2->ambient)
						return true;
					if(s1->ambient==s2->ambient)
					{
						if(s1->attribute<s1->attribute)
							return true;
						if(s1->attribute==s1->attribute)
							if(s1->phase<s2->phase)
								return true;
					}
				}
			}
		}
		return false;
	}
};

void cCamera::DrawSortMaterial()
{
	vector<cMeshSortingPhase*>& ar=RootCamera->arSortMaterial;
	if(ar.empty())
		return;

	sort(ar.begin(),ar.end(),SortMaterialByNodeBank());

	sDataRenderMaterial Data;
	int change_mat=1,draw_object=0;

	cMeshSortingPhase* cur_mat=ar.front();

	cur_mat->GetMaterial(&Data);

	DrawType* draw=gb_RenderDevice3D->dtAdvance;

	cCamera* pShadow=FindCildCamera(ATTRCAMERA_SHADOWMAP);

	bool use_shadow=pShadow && !GetAttribute(ATTRCAMERA_REFLECTION) && Option_IsShadowMap;
	draw->BeginDraw(use_shadow);
	draw->SetSimplyMaterial(cur_mat->GetFront(),&Data);

	if(GetAttribute(ATTRCAMERA_REFLECTION))
	{
		vector<cMeshSortingPhase*>::iterator it;
		FOR_EACH(ar,it)
		{
			cMeshSortingPhase* s=*it;

			if(cur_mat->pBank!=s->pBank || cur_mat->channel!=s->channel ||
				cur_mat->phase!=s->phase || cur_mat->diffuse!=s->diffuse ||
				cur_mat->ambient!=s->ambient ||
				cur_mat->attribute!=s->attribute)
			{
				cur_mat=s;
				cur_mat->GetMaterial(&Data);

				draw->SetSimplyMaterial(cur_mat->GetFront(),&Data);
				change_mat++;
			}

			for(cObjMesh* pMesh=s->GetFront();pMesh;pMesh=pMesh->GetNextSorting())
			{
				if(pMesh->GetGlobalMatrix().trans().z<GetHReflection())
					continue;
				
				draw->DrawNoMaterial(pMesh,&Data);
				draw_object++;
			}
		}
	}else
	{
		vector<cMeshSortingPhase*>::iterator it;
		FOR_EACH(ar,it)
		{
			cMeshSortingPhase* s=*it;

			if(cur_mat->pBank!=s->pBank || cur_mat->channel!=s->channel ||
			   cur_mat->phase!=s->phase || cur_mat->diffuse!=s->diffuse ||
			   cur_mat->attribute!=s->attribute)
			{
				cur_mat=s;
				cur_mat->GetMaterial(&Data);

				draw->SetSimplyMaterial(cur_mat->GetFront(),&Data);
				change_mat++;
			}

			for(cObjMesh* pMesh=s->GetFront();pMesh;pMesh=pMesh->GetNextSorting())
			{
				draw->DrawNoMaterial(pMesh,&Data);
				draw_object++;
			}
		}
	}

	draw->EndDraw();
}

struct SortMaterialByShadowTexture
{
	inline bool operator ()(const cMeshSortingPhase* s1,const cMeshSortingPhase* s2)
	{
		cTexture* t1=s1->pBank->GetMaterial()->GetAttribute(MAT_ALPHA_TEST)?s1->pBank->GetTexture(0):0;
		cTexture* t2=s2->pBank->GetMaterial()->GetAttribute(MAT_ALPHA_TEST)?s2->pBank->GetTexture(0):0;

		if(t1<t2)
			return true;
		return false;
	}
};

void cCamera::DrawSortMaterialShadow()
{
	vector<cMeshSortingPhase*>& ar=RootCamera->arSortMaterial;
	sort(ar.begin(),ar.end(),SortMaterialByShadowTexture());
	cMeshBank *CurBank=NULL;

	DrawType* draw=gb_RenderDevice3D->dtFixed;
	if(GetAttribute(ATTRCAMERA_SHADOWMAP))
		draw=gb_RenderDevice3D->dtAdvance;

	int change_mat=0,draw_object=0;
	draw->BeginDrawShadow();

	vector<cMeshSortingPhase*>::iterator it;
	FOR_EACH(ar,it)
	{
		cMeshSortingPhase& s=**it;

		if(CurBank!=s.pBank)
		{
			CurBank=s.pBank;

			cTexture* Texture=CurBank->GetMaterial()->GetAttribute(MAT_ALPHA_TEST)?CurBank->GetTexture(0):0;
			draw->SetSimplyMaterialShadow(s.GetFront(),Texture);
			change_mat++;
		}

		for(cObjMesh* pMesh=s.GetFront();pMesh;pMesh=pMesh->GetNextSorting())
		{
			if(pMesh->GetBank()->GetMaterial()->GetAttribute(MAT_IS_BLEND))
				continue;
			if(pMesh->GetAttr(ATTRCAMERA_SHADOW))
				draw->DrawNoMaterialShadow(pMesh);
			draw_object++;
		}
	}

	draw->EndDrawShadow();
}

void cCamera::DrawSortMaterialShadowStrencil()
{
	cMeshBank *CurBank=NULL;

	cD3DRender* rd=GetRenderDevice3D();

	rd->SetNoMaterial(ALPHA_NONE);

    // Disable z-buffer writes (note: z-testing still occurs), and enable the
    // stencil-buffer
    rd->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
    rd->SetRenderState( D3DRS_STENCILENABLE, TRUE );

    // Dont bother with interpolating color
    rd->SetRenderState( D3DRS_SHADEMODE,     D3DSHADE_FLAT );

    // Set up stencil compare fuction, reference value, and masks.
    // Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true.
    // Note: since we set up the stencil-test to always pass, the STENCILFAIL
    // renderstate is really not needed.
    rd->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
    rd->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
    rd->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );

    // If ztest passes, inc/decrement stencil buffer value
    rd->SetRenderState( D3DRS_STENCILREF,       0x1 );
    rd->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
    rd->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

    // Make sure that no pixels get drawn to the frame buffer
    rd->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    rd->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
    rd->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

    // Draw front-side of shadow volume in stencil/z only
    rd->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
    rd->SetRenderState( D3DRS_STENCILPASS,D3DSTENCILOP_INCR );
	DrawSortMaterialShadowStrencilOneSide();
    // Now reverse cull order so back sides of shadow volume are written.

    // Decrement stencil buffer value
	rd->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
    rd->SetRenderState( D3DRS_STENCILPASS,D3DSTENCILOP_DECR );
	DrawSortMaterialShadowStrencilOneSide();

    // Restore render states
    rd->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
    rd->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
    rd->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );
    rd->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
    rd->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	DrawShadowPlane();
}

void cCamera::DrawSortMaterialShadowStrencilOneSide()
{
	cCamera* root=GetRoot();
	for(int i=0;i<root->ShadowTestArray.size();i++)
		root->ShadowTestArray[i]->DrawShadow(this);
}

void cCamera::DrawShadowPlane()
{
	cD3DRender* rd=GetRenderDevice3D();
	DWORD fog=rd->GetRenderState( D3DRS_FOGENABLE);
    // Set renderstates (disable z-buffering, enable stencil, disable fog, and
    // turn on alphablending)
    rd->SetRenderState( D3DRS_ZENABLE,          FALSE );
    rd->SetRenderState( D3DRS_STENCILENABLE,    TRUE );
    rd->SetRenderState( D3DRS_FOGENABLE,        FALSE );
    rd->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    rd->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    rd->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    rd->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    rd->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    rd->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    rd->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    rd->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    rd->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );

    // Only write where stencil val >= 1 (count indicates # of shadows that
    // overlap that pixel)
    rd->SetRenderState( D3DRS_STENCILREF,  0x1 );
    rd->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
    rd->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

    // Draw a big, gray square
	sColor4c diffuse(0,0,0,0x7f);
	cVertexBuffer<sVertexXYZWD>* buf=rd->GetBufferXYZWD();
	sVertexXYZWD* pv=buf->Lock();
	int x1=vp.X,y1=vp.Y;
	int x2=x1+vp.Width,y2=y1+vp.Height;
	pv[0].x=x1; pv[0].y=y1; pv[0].z=0.001f; pv[0].w=0.001f; pv[0].diffuse=diffuse;
	pv[2].x=x1; pv[2].y=y2; pv[1].z=0.001f; pv[1].w=0.001f; pv[1].diffuse=diffuse;
	pv[1].x=x2; pv[1].y=y1; pv[2].z=0.001f; pv[2].w=0.001f; pv[2].diffuse=diffuse;

	pv[3].x=x2; pv[3].y=y1; pv[3].z=0.001f; pv[3].w=0.001f; pv[3].diffuse=diffuse;
	pv[5].x=x1; pv[5].y=y2; pv[4].z=0.001f; pv[4].w=0.001f; pv[4].diffuse=diffuse;
	pv[4].x=x2; pv[4].y=y2; pv[5].z=0.001f; pv[5].w=0.001f; pv[5].diffuse=diffuse;
	const int npoint=6;
	buf->Unlock(npoint);
	buf->DrawPrimitive(PT_TRIANGLELIST,npoint/3);

//    rd->SetFVF( SHADOWVERTEX::FVF );
//    rd->SetStreamSource( 0, m_pBigSquareVB, 0, sizeof(SHADOWVERTEX) );
//    rd->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Restore render states
    rd->SetRenderState( D3DRS_ZENABLE,          TRUE );
    rd->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
    rd->SetRenderState( D3DRS_FOGENABLE,        fog );
    rd->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}

void cCamera::AttachFirst(cIUnkClass* zpalne)
{
	arZPlane.push_back(zpalne);
}

eTestVisible cCamera::GridTest(Vect3f p[8])
{
	for(int i=0;i<8;i++)
	{
		int x=round(p[i].x)>>TestGridShl,y=round(p[i].y)>>TestGridShl;
		if(x<0 || x>=TestGridSize.x || y<0 || y>=TestGridSize.y)
			continue;
		if(pTestGrid[x+y*TestGridSize.x])
			return VISIBLE_INTERSECT;
	}

	return VISIBLE_OUTSIDE;
}

eTestVisible cCamera::TestVisible(const MatXf &matrix,const Vect3f &min,const Vect3f &max)
{ // для BoundingBox с границами min && max
	Vect3f	p[8];
	matrix.xformPoint(Vect3f(min.x,min.y,min.z),p[0]);
	matrix.xformPoint(Vect3f(max.x,min.y,min.z),p[1]);
	matrix.xformPoint(Vect3f(min.x,max.y,min.z),p[2]);
	matrix.xformPoint(Vect3f(max.x,max.y,min.z),p[3]);
	matrix.xformPoint(Vect3f(min.x,min.y,max.z),p[4]);
	matrix.xformPoint(Vect3f(max.x,min.y,max.z),p[5]);
	matrix.xformPoint(Vect3f(min.x,max.y,max.z),p[6]);
	matrix.xformPoint(Vect3f(max.x,max.y,max.z),p[7]);

	if(RootCamera->pTestGrid)
		return RootCamera->GridTest(p);

	for(int i=0;i<GetNumberPlaneClip3d();i++)
	{
		if(GetPlaneClip3d(i).GetDistance(p[0])>=0) continue;
		if(GetPlaneClip3d(i).GetDistance(p[1])>=0) continue;
		if(GetPlaneClip3d(i).GetDistance(p[2])>=0) continue;
		if(GetPlaneClip3d(i).GetDistance(p[3])>=0) continue;
		if(GetPlaneClip3d(i).GetDistance(p[4])>=0) continue;
		if(GetPlaneClip3d(i).GetDistance(p[5])>=0) continue;
		if(GetPlaneClip3d(i).GetDistance(p[6])>=0) continue;
		if(GetPlaneClip3d(i).GetDistance(p[7])>=0) continue;
		return VISIBLE_OUTSIDE;
	}
	return VISIBLE_INTERSECT;
}

//*
eTestVisible cCamera::TestVisible(const Vect3f &min,const Vect3f &max)
{ // для BoundingBox с границами min && max, заданными в глобальным координатах
	for(int i=0;i<GetNumberPlaneClip3d();i++)
	{
		sPlane4f& p=GetPlaneClip3d(i);
		if(p.GetDistance(min)>=0) continue;
		if(p.GetDistance(Vect3f(max.x,min.y,min.z))>=0) continue;
		if(p.GetDistance(Vect3f(min.x,max.y,min.z))>=0) continue;
		if(p.GetDistance(Vect3f(max.x,max.y,min.z))>=0) continue;
		if(p.GetDistance(Vect3f(min.x,min.y,max.z))>=0) continue;
		if(p.GetDistance(Vect3f(max.x,min.y,max.z))>=0) continue;
		if(p.GetDistance(Vect3f(min.x,max.y,max.z))>=0) continue;
		if(p.GetDistance(max)>=0) continue;
		return VISIBLE_OUTSIDE;
	}
	return VISIBLE_INTERSECT;
}
/**/
/*
int cCamera::TestVisible(const Vect3f &vmin,const Vect3f &vmax)
{ // для BoundingBox с границами min && max, заданными в глобальным координатах
	Vect3f v[8];
	matViewProj.Convert(vmin,v[0]);
	matViewProj.Convert(Vect3f(vmax.x,vmin.y,vmin.z),v[1]);
	matViewProj.Convert(Vect3f(vmin.x,vmax.y,vmin.z),v[2]);
	matViewProj.Convert(Vect3f(vmax.x,vmax.y,vmin.z),v[3]);
	matViewProj.Convert(Vect3f(vmin.x,vmin.y,vmax.z),v[4]);
	matViewProj.Convert(Vect3f(vmax.x,vmin.y,vmax.z),v[5]);
	matViewProj.Convert(Vect3f(vmin.x,vmax.y,vmax.z),v[6]);
	matViewProj.Convert(vmax,v[7]);

	const float MinX=-1;
	const float MaxX=+1;
	const float MinY=-1;
	const float MaxY=+1;
	float MinZ=vp.MinZ,MaxZ=vp.MaxZ;

	for(int i=0;i<8;i++)
	{
		Vect3f& c=v[i];
		if(c.x<MinX || c.x>MaxX)continue;
		if(c.y<MinY || c.y>MaxY)continue;
		if(c.z<MinZ || c.z>MaxZ)continue;
		return 0;
	}
	return 1;
}
/**/
//*
eTestVisible cCamera::TestVisibleComplete(const Vect3f &min,const Vect3f &max)
{
	VISASSERT(GetNumberPlaneClip3d()==6);
	float m, n;
	int i;
	eTestVisible result = VISIBLE_INSIDE;
	float dx=max.x-min.x;
	float dy=max.y-min.y;
	float dz=max.z-min.z;
	VISASSERT(dx>=0 && dy>=0 && dz>=0);

	for (i = 0; i < GetNumberPlaneClip3d(); i++)
	{
		sPlane4f& p=GetPlaneClip3d(i);
		m = (min.x * p.A) + (min.y * p.B) + (min.z * p.C) + p.D;
		n = (dx * ABS(p.A)) + (dy * ABS(p.B)) + (dz * ABS(p.C));

		if (m + n < 0) return VISIBLE_OUTSIDE;
		if (m - n < 0) result = VISIBLE_INTERSECT;

    }
	return result;
}
/**/

void cCamera::ClearZBuffer()
{
	cD3DRender* render=GetRenderDevice3D();
	RDCALL(render->lpD3DDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1, 0));
}

void cCamera::ShowClip()
{
	RenderDevice->DrawRectangle(vp.X,vp.Y,vp.Width-1,vp.Height-1, sColor4c(255,255,255,255),true);
}

cCamera* cCamera::FindCildCamera(int AttributeCamera)
{
	if(GetAttribute(AttributeCamera))
		return this;

	cCamera* p=NULL;
	vector<cCamera*>::iterator it_c;
	FOR_EACH(child,it_c)
	{
		p=(*it_c)->FindCildCamera(AttributeCamera);
		if(p)return p;
	}

	return p;
}

void cCamera::DrawObjectFirst()
{
	int number_draw=GetNumberDraw(SCENENODE_OBJECTFIRST);
	for( int nObj=0; nObj<number_draw; nObj++ )
		GetDraw(SCENENODE_OBJECTFIRST,nObj)->Draw(this);
}

#include "TileMap.h"
void cCamera::EnableGridTest(int grid_dx,int grid_dy,int grid_size)
{
	InitGridTest(grid_dx,grid_dy,grid_size);
	VISASSERT(!Parent && pTestGrid);
	CalcTestForGrid();
}

void cCamera::InitGridTest(int grid_dx,int grid_dy,int grid_size)
{
	if(pTestGrid)
	{
		if(TestGridSize.x==grid_dx && TestGridSize.y==grid_dy)
			return;
		delete pTestGrid;
	}

	TestGridSize.x=grid_dx;
	TestGridSize.y=grid_dy;

	TestGridShl=0;
	while(grid_size>1)
	{
		TestGridShl++;
		grid_size=grid_size>>1;
	}
	VISASSERT(grid_size==1);
	
	pTestGrid=new BYTE[TestGridSize.x*TestGridSize.y];
}

void cCamera::CalcTestForGrid()
{
	Vect2i TileSize(1<<TestGridShl,1<<TestGridShl);
	calcVisMap(this,TestGridSize,TileSize,pTestGrid,true);

	vector<cCamera*>::iterator it_c;
	FOR_EACH(child,it_c)
	{
		if((*it_c)->GetAttribute(ATTRCAMERA_SHADOWMAP))
			calcVisMap(*it_c,TestGridSize,TileSize,pTestGrid,false);
	}

}

void cCamera::DrawTestGrid()
{
	if(pTestGrid==NULL)
		return;
	int dx=TestGridSize.x,dy=TestGridSize.y;
	int minx=14,miny=639;

	int r,g,b,a=80;
	r=g=b=255;

	int mul=dx>32?2:4;

	for(int y=0;y<dy;y++)
	for(int x=0;x<dx;x++)
	{
		if(pTestGrid[y*dx+x])
		{
			int xx=x*mul+minx,yy=y*mul+miny;
			gb_RenderDevice3D->DrawRectangle(xx,yy,mul,mul,sColor4c(r,g,b,a));
		}
	}
}

void cCamera::AttachChild(cCamera *c)
{
	child.push_back(c);
	c->Parent=this;
	c->RootCamera=RootCamera;
}

void cCamera::DrawShadowDebug()
{
	if(Option_ShowRenderTextureDBG)
	{
		cCamera* pShadow=FindCildCamera((Option_ShowRenderTextureDBG!=2)?ATTRCAMERA_SHADOWMAP:ATTRCAMERA_SHADOW);
		if(pShadow && pShadow->GetRenderTarget())
		{
			DWORD fogenable=gb_RenderDevice3D->GetRenderState(D3DRS_FOGENABLE);
			RenderDevice->SetRenderState(RS_FOGENABLE,FALSE);

			if(Option_ShowRenderTextureDBG!=4)
			{
				gb_RenderDevice3D->SetPixelShader(NULL);
				const int size=256;
				float mi=0.0f,ma=1.0f;
				gb_RenderDevice3D->DrawSprite(0,60,size,size,
						mi,mi,ma-mi,ma-mi,pShadow->GetRenderTarget());
			}else
			{
				const int size=256;
				float mi=0.0f,ma=1.0f;
				int x1=0,y1=60;
				int x2=x1+size,y2=y1+size;
				float u1=mi,v1=mi;
				float du=ma-mi,dv=ma-mi;

				RenderDevice->SetNoMaterial(ALPHA_NONE,0);
				gb_VisGeneric->GetShaders()->pShowMap->Select();
				gb_RenderDevice3D->SetTexture(0,gb_RenderDevice3D->dtAdvance->GetTZBuffer());

				sColor4c ColorMul=sColor4c(255,255,255,255);
				cVertexBuffer<sVertexXYZWDT1>& BufferXYZWDT1=*gb_RenderDevice3D->GetBufferXYZWDT1();
				sVertexXYZWDT1* v=BufferXYZWDT1.Lock(4);
				v[0].z=v[1].z=v[2].z=v[3].z=0.001f;
				v[0].w=v[1].w=v[2].w=v[3].w=0.001f;
				v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=ColorMul;
				v[0].x=v[1].x=-0.5f+(float)x1; v[0].y=v[2].y=-0.5f+(float)y1; 
				v[3].x=v[2].x=-0.5f+(float)x2; v[1].y=v[3].y=-0.5f+(float)y2; 
				v[0].u1()=u1;    v[0].v1()=v1;
				v[1].u1()=u1;    v[1].v1()=v1+dv;
				v[2].u1()=u1+du; v[2].v1()=v1;
				v[3].u1()=u1+du; v[3].v1()=v1+dv;
				BufferXYZWDT1.Unlock(4);

				BufferXYZWDT1.DrawPrimitive(PT_TRIANGLESTRIP,2);
			}

			gb_RenderDevice3D->SetPixelShader(NULL);
			RenderDevice->SetRenderState(RS_FOGENABLE,fogenable);
		}
	}
}

void cCamera::GetLighting(Vect3f& l)
{
	GetScene()->GetLighting(&l);
}

Vect2f cCamera::CalculateZMinMax(MatXf& view)
{
	VISASSERT(!Parent);
	Vect2f zx(1e20f,-1e20f);
	cCamera* root=GetRoot();
	vector<cObjectNodeRoot*>::iterator it;
	FOR_EACH(root->ShadowTestArray,it)
	{
		float r=(*it)->GetRadius();
		const Vect3f& pos=(*it)->GetGlobalMatrix().trans();
		Vect3f out=view*pos;

		zx.x=min(zx.x,out.z-r);
		zx.y=max(zx.x,out.z+r);
	}

	return zx;
}

void cVisGeneric::DrawInfo()
{
	if(Option_ShowType[SHOW_INFO])
	{
		cFont* pFont=gb_VisGeneric->CreateDebugFont();
		gb_RenderDevice->SetFont(pFont);

		char s[128];
		sprintf(s,"1 normal camera - %s",Option_ShowType[SHOW_NORMAL]?"show":"hide");
		gb_RenderDevice->OutText(800,10,s,sColor4f(1,1,0,1));
		
		sprintf(s,"2 shadow camera - %s",Option_ShowType[SHOW_SHADOW]?"show":"hide");
		gb_RenderDevice->OutText(800,30,s,sColor4f(1,1,0,1));

		sprintf(s,"3 reflection camera - %s",Option_ShowType[SHOW_REDLECTION]?"show":"hide");
		gb_RenderDevice->OutText(800,50,s,sColor4f(1,1,0,1));

		sprintf(s,"4 tilemap - %s",Option_ShowType[SHOW_TILEMAP]?"show":"hide");
		gb_RenderDevice->OutText(800,70,s,sColor4f(1,1,0,1));

		sprintf(s,"5 object - %s",Option_ShowType[SHOW_OBJECT]?"show":"hide");
		gb_RenderDevice->OutText(800,90,s,sColor4f(1,1,0,1));

		gb_RenderDevice->SetFont(NULL);
		pFont->Release();
	}	
}

bool cCamera::IsBadClip()
{
	Vect2f sz;
	if(RenderTarget==0)
		sz.set(RenderDevice->GetSizeX(),RenderDevice->GetSizeY());
	else
		sz.set(RenderTarget->GetWidth(),RenderTarget->GetHeight());

	if(vp.X<0 || vp.Y<0)
		return true;
	if(vp.X+vp.Width>sz.x)
		return true;
	if(vp.Y+vp.Height>sz.y)
		return true;

	return false;
}

///////////////////////////////cCameraPlanarLight/
void cCameraPlanarLight::DrawScene()
{
	RenderDevice->SetDrawNode(this);
	RenderDevice->SetGlobalLight(NULL);

	RenderDevice->SetRenderState( RS_ZWRITEENABLE, FALSE );
	DWORD ZFUNC=gb_RenderDevice3D->GetRenderState(D3DRS_ZFUNC);
	gb_RenderDevice3D->SetRenderState( D3DRS_ZFUNC, CMP_ALWAYS );
	

	DWORD fogenable=GetRenderDevice3D()->GetRenderState(D3DRS_FOGENABLE);
	RenderDevice->SetRenderState(RS_FOGENABLE,FALSE);
	
	RenderDevice->Draw(GetScene());

	gb_RenderDevice3D->SetRenderState( D3DRS_ZFUNC, ZFUNC );
	RenderDevice->SetRenderState(RS_FOGENABLE,fogenable);
}

void TempDrawShadow(cCamera* camera)
{
	if(!camera->GetAttribute(ATTRCAMERA_SHADOWMAP))
		return;

	gb_RenderDevice3D->SetPixelShader(NULL);
	gb_RenderDevice3D->SetVertexShader(NULL);
	gb_RenderDevice3D->SetFVF(sVertexXYZWD::fmt);

	gb_RenderDevice3D->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	gb_RenderDevice3D->SetRenderState(D3DRS_ZFUNC,D3DCMP_GREATER);

	for(int c=255;c>=0;c-=4)
	{
		sVertexXYZWD Vertex[4];
		float xOfs=0,yOfs=0;
		const int size=camera->vp.Width;
		Vertex[0].x=xOfs;     Vertex[0].y=yOfs;     
		Vertex[1].x=xOfs;     Vertex[1].y=yOfs+size;
		Vertex[2].x=xOfs+size;Vertex[2].y=yOfs;     
		Vertex[3].x=xOfs+size;Vertex[3].y=yOfs+size;
		for(int i=0;i<4;i++)
		{
			Vertex[i].z=Vertex[i].w=c/256.0f;
			Vertex[i].diffuse.set(c,c,c,255);
		}
		gb_RenderDevice3D->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(Vertex[0]));
	}

	gb_RenderDevice3D->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	gb_RenderDevice3D->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
}
