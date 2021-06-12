// WinVGView.cpp : implementation of the CWinVGView class
//

#include "stdafx.h"
#include "WinVG.h"

#include "MainFrm.h"
#include "WinVGDoc.h"
#include "WinVGView.h"
#include "HierarchyTree.h"
#include "DirectoryTree.h"
#include <mmsystem.h>
#include <list>
#include "ModelInfo.h"
#include "ScreenShotSize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool WinVGIsOldModel();

cInterfaceRenderDevice	*gb_IRenderDevice=0;// устройство вывода
cLogicGeneric			*gb_ILogicGeneric=0;// библиотека логической информации
cVisGeneric				*gb_IVisGeneric=0;	// верхняя библиотека визуализации
cScene					*gb_Scene=0;		// сцена, может содержать мир, объекты и т.д.
cCamera					*gb_Camera=0;
cUnkLight				*gb_ULight1=0;

cIUnkClass				*gb_TileMap=NULL;
int						g_CurTime=0;
double					gd_CurTime=0;
CWinVGView*				pView=NULL;

int num_object_linear=1;
cFrame	FramePhase;
const float frame_period=2000.0f;

cPlane* pPlane=NULL;
QuatF	CameraRotation(0, Vect3f(1,0,0));

bool	g_bPressLighing=false;
string textures_subdir="TEXTURES";

#include "fps.h"

FPS fps;
void SetUseShadow(bool use);
/////////////////////////////////////////////////////////////////////////////
// CWinVGView

IMPLEMENT_DYNCREATE(CWinVGView, CView)

BEGIN_MESSAGE_MAP(CWinVGView, CView)
	//{{AFX_MSG_MAP(CWinVGView)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinVGView construction/destruction

CWinVGView::CWinVGView()
{
	b_viewHologram=false;
	pView=this;

	bWireFrame=0;
	bMouseLDown=bMouseRDown=0;

	m_ScaleTime=1;
	Color.set(50,100,50,255);
	m_ControlSwitch=0;
	m_NumberAnimChain=0;
	m_OldScaleTime=0;
	SkinColor.set(1,0,0,1);
	FramePhase.Set(frame_period,0);
	target_pos.set(1024,1024,400);
	ResetCameraPosition();
	pLightMap=NULL;
	show_basement=false;
	enable_tilemap=false;
}

CWinVGView::~CWinVGView()
{
	gb_FrameWnd->SaveRegistry();
	DoneRenderDevice();
}

/////////////////////////////////////////////////////////////////////////////
// CWinVGView drawing

void OutBigText()
{
	static cFont* pFont=NULL;
	if(pFont==NULL)pFont=gb_IVisGeneric->CreateFont("Fonts\\Arial.font");

	gb_IRenderDevice->SetFont(pFont);

	sColor4f color(1,1,1,1);
	gb_IRenderDevice->OutText(20,20,"color &FFFF00color &FF00FF00 &&color",color);
}

void CWinVGView::SynhronizeObjAndLogic(cIUnkClass *UObj,cIUnkClass *ULogic)
{
	if(ULogic)
	{
		char NameChannel[256];
		cObjectNode* node=dynamic_cast<cObjectNode*>(UObj);
		float phase=FramePhase.GetPhase();
		node->GetCurrentChannel(0,NameChannel);

		cLogicObject* p=dynamic_cast<cLogicObject*>(ULogic);
		p->SetChannel(NameChannel,phase);
		p->Update();
	}
}

void TransformQuaternion(cObjectNodeRoot* UObj, DWORD time)
{
	Vect3f p(0,1,0);
	const nq=3;
	QuatF quat[nq];
	QuatF quat_out;
	for(int i=0;i<nq;i++)
		quat[i].set((i*M_PI*2)/nq,p);

	float f=time/300.0f;
	int n=((int)f)%nq;
	int n1=n+1;
	if(n1>=nq)n1=0;
	float phase=fmodf(f,1);
	quat_out.slerp(quat[n],quat[n1],phase);

	Mat3f pos;
	pos.set(quat_out);

	cIUnkClass *node;
	node=UObj->FindObject("group dulo");
	if(node)
		node->SetRotate(&pos);
}

void CWinVGView::OnDraw(CDC* pDC)
{
	if(gb_Scene)
	{
		double dt;
		int time=clocki();
		dt=((double)time-(double)g_CurTime)*m_ScaleTime;
		if(dt<0 || dt>1000)dt=0;
		double dtime=gd_CurTime+dt;
		int int_time=round(dtime);
		
		FramePhase.AddPhase((float)int_time);
		gb_Scene->dSetTime((float)int_time);
		g_CurTime=time;
		gd_CurTime=dtime-int_time;
	}
	Draw(true);
}

void CWinVGView::Draw(bool show_old_model)
{	
	CWinVGDoc* pDoc = GetDocument();
	if(gb_IRenderDevice==0) return;
	if(pDoc==NULL || gb_FrameWnd==NULL) return;
	ASSERT_VALID(pDoc);

	if(pDoc->m_pDirectoryTree&& !pDoc->m_pDirectoryTree->FileDoubleClick.IsEmpty())
	{
		CString fname=pDoc->m_pDirectoryTree->FileDoubleClick;
		pDoc->m_pDirectoryTree->FileDoubleClick.Empty();
		LoadObject(fname);
		pDoc->m_pHierarchyLogic->TreeUpdate();
		pDoc->m_pHierarchyObj->TreeUpdate();
	}
	
	if( !gb_Scene ) return;


	cObjectNodeRoot* UObj=pDoc->m_pHierarchyObj?dynamic_cast<cObjectNodeRoot*>(pDoc->m_pHierarchyObj->GetRoot()):NULL;
	cLogicObject* ULogicObj=pDoc->m_pHierarchyLogic?dynamic_cast<cLogicObject*>(pDoc->m_pHierarchyLogic->GetRoot()):NULL;
	vector<cIUnkClass*>& hobj=pDoc->m_pHierarchyObj->GetAllObj();
	vector<cIUnkClass*>::iterator it;
/*
	if(UObj)
	{
		static float t=0;
		float r=150;
		t+=dt*1.5e-3f;
		MatXf pos=UObj->GetPosition();
		pos.trans()=target_pos+Vect3f(r*cosf(t),r*sinf(t),0);
		UObj->SetPosition(pos);
	}
*/
	FOR_EACH(hobj,it)
	{
		cIUnkClass* UObj=*it;
		char StrSelectObj[256];
		CTreeCtrl &TreeCtrl=pDoc->m_pHierarchyObj->GetTreeCtrl();
		HTREEITEM hItem=TreeCtrl.GetSelectedItem();
		pDoc->m_pHierarchyObj->GetStringObj(TreeCtrl.GetItemText(hItem),StrSelectObj);

		cObjectNode* node=dynamic_cast<cObjectNode*>(UObj);
		node->SetPhase(FramePhase.GetPhase());
	}

//	if(UObj)
//		TransformQuaternion(UObj,g_CurTime);

//	if(UObj && ULogicObj)
//	{
//		cIUnkClass *node1,*node2;
//		node1=UObj->FindObject("group dulo");
//		node2=ULogicObj->FindObject("dulo");
//		if(node1 && node2)
//		{
//			Mat3f mat;
//			mat.set(fmod(g_CurTime/1000.0,2*M_PI),Y_AXIS);
//			node1->SetRotate(&mat);
//			node2->SetRotate(&mat);
//		}
//	}

	gb_Scene->PreDraw(gb_Camera);
	
//*
	gb_IRenderDevice->Fill(Color.r,Color.g,Color.b,0);
	gb_IRenderDevice->BeginScene();
	
	gb_IRenderDevice->SetRenderState(RS_FILLMODE,bWireFrame?FILL_WIREFRAME:FILL_SOLID);

	cIUnkClass *SelectObj=0;
	
	if(show_basement && UObj)
	{
		gb_IRenderDevice->SetDrawTransform(gb_Camera);
		DrawLocalBorder(dynamic_cast<cObjectNodeRoot*>(UObj));
	}

	gb_Scene->Draw(gb_Camera);

	if(gb_FrameWnd->m_bPressBBox&&UObj)
	{
		sBox6f Box;
		MatXf Matrix; 
		Vect3f Scale;
		Scale=UObj->GetScale();
		Matrix=UObj->GetPosition();
		UObj->GetBoundBox(Box);
		gb_IRenderDevice->DrawBound(Matrix,Box.min*Scale,Box.max*Scale,1,sColor4c(0,0,0,0));
	}

	if(SelectObj) SelectObj->SetColor(0,0,&sColor4f(0,0,0,1));

	if(ULogicObj&&UObj)
	{
		MatXf mat;
		mat=UObj->GetPosition();
		ULogicObj->SetPosition(mat);
		Vect3f scale;
		scale=UObj->GetScale();
		ULogicObj->SetScale(scale);
		SynhronizeObjAndLogic(UObj,ULogicObj);
		char StrSelectObj[256];
		CTreeCtrl &TreeCtrl=pDoc->m_pHierarchyLogic->GetTreeCtrl();
		HTREEITEM hItem=TreeCtrl.GetSelectedItem();
		pDoc->m_pHierarchyLogic->GetStringObj(TreeCtrl.GetItemText(hItem),StrSelectObj);
		cIUnkClass *SelectObj=dynamic_cast<cLogicObject*>(ULogicObj)->FindObject(StrSelectObj);
		if(gb_FrameWnd->m_bPressLogic)
		{
			if(SelectObj) SelectObj->SetAttr(ATTRUNKOBJ_SHADOW);
			if(gb_FrameWnd->m_bPressBound)
				ULogicObj->SetAttr(ATTRUNKOBJ_SHOWBOUND);
			else
				ULogicObj->ClearAttr(ATTRUNKOBJ_SHOWBOUND);
			ULogicObj->Draw(gb_Camera);
			if(SelectObj) SelectObj->ClearAttr(ATTRUNKOBJ_SHADOW);
		}
	}

/*
	if(UObj)
	{
		cObjectNodeRoot* node=dynamic_cast<cObjectNodeRoot*>(UObj);
		node->DrawBadUV(gb_Camera);
	}
*/
/*
	gb_IRenderDevice->SetDrawTransform(gb_Camera);
	gb_IRenderDevice->DrawLine(test_pos,test_pos+Vect3f(1.f,1.f,-1.f),255,255,255,255);
	gb_IRenderDevice->FlushPrimitive3D();
*/
//	OutBigText();
//	if(UObj)TestObject(UObj);

	gb_IRenderDevice->EndScene();

	gb_IRenderDevice->Flush();
/**/
	if(WinVGIsOldModel() && show_old_model)
	{
		gb_IRenderDevice->OutText(20,20,"Old model",255,0,0,"Arial",20,1);
	}
	fps.quant();
	
	static int prev_time=0;

	if(UObj && (clocki()-prev_time>200))
	{
		CString mes;
		float cur_fps=fps.GetFPS();
		mes.Format("FPS: %.2f",cur_fps);
		gb_FrameWnd->m_wndStatusBar.SetPaneText(1, mes);

		float phase=0;
		cObjectNode* node=dynamic_cast<cObjectNode*>(UObj);
		phase=FramePhase.GetPhase();//node->GetPhase();
		mes.Format("phase: %f",phase);
		gb_FrameWnd->m_wndStatusBar.SetPaneText(2, mes);

		mes.Format("%2.2f mpoly/s",cur_fps*gb_IRenderDevice->GetDrawNumberPolygon()/1000000.0f);
		gb_FrameWnd->m_wndStatusBar.SetPaneText(3, mes);

		mes.Format("%i polygon",gb_IRenderDevice->GetDrawNumberPolygon());
		gb_FrameWnd->m_wndStatusBar.SetPaneText(4, mes);

		prev_time=clocki();
	}

	gb_Scene->PostDraw(gb_Camera);

}

/////////////////////////////////////////////////////////////////////////////
// CWinVGView diagnostics

#ifdef _DEBUG
void CWinVGView::AssertValid() const
{
	CView::AssertValid();
}

void CWinVGView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWinVGDoc* CWinVGView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinVGDoc)));
	return (CWinVGDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinVGView message handlers

MatXf TransposeMeshScr(cIUnkClass *UObject,cCamera *Camera1,Vect3f &dPos,Vect3f &dAngle)
{
	MatXf CameraMatrix,LocalMatrix;
	Camera1->GetPosition(&CameraMatrix);
	LocalMatrix=UObject->GetPosition();
	Mat3f InvCamera;
	InvCamera.invert(CameraMatrix.rot());
	MatXf Rot;
	SetPosition(Rot,Vect3f(0,0,0),Vect3f(-dAngle.z,-dAngle.x,dAngle.y));

	LocalMatrix.set(InvCamera*Rot.rot()*CameraMatrix.rot()*LocalMatrix.rot(),
		LocalMatrix.trans()+dPos.x*CameraMatrix.rot().xrow()
		-dPos.y*CameraMatrix.rot().yrow());
	UObject->SetPosition(LocalMatrix);
	return LocalMatrix;
}

void CWinVGView::ResetCameraPosition()
{
	camera_pos.rot().set(0,1,0,0);
	camera_pos.trans().set(1024,1024,1024);
	SetCameraPosition(0,0,1);
}

void CWinVGView::SetCameraPosition(float du,float dv,float dscale)
{
	float dist=target_pos.distance(camera_pos.trans())*dscale;

	QuatF rot;
	rot.mult(camera_pos.rot(),QuatF(du,Vect3f(0,1,0)));
	camera_pos.rot().mult(rot,QuatF(dv,Vect3f(1,0,0)));

	camera_pos.trans()=target_pos+Mat3f(camera_pos.rot())*Vect3f(0,0,-dist);
	Se3f pos=camera_pos;
	MatXf m(pos);
	m.Invert();
	if(gb_Camera)gb_Camera->SetPosition(m);
}

void CWinVGView::SetEffectDirectory(const char* dir)
{
	string textures=dir+string("Textures");
	gb_IVisGeneric->SetEffectLibraryPath(dir,textures.c_str());
}


int CWinVGView::InitRenderDevice(int xScr,int yScr)
{
	gb_IVisGeneric=CreateIVisGeneric();
	gb_IRenderDevice=CreateIRenderDevice();
	SetEffectDirectory(gb_FrameWnd->effect_directory.c_str());

	if(gb_IRenderDevice->Init(xScr,yScr,RENDERDEVICE_MODE_WINDOW,m_hWnd))
		return 1;
//	gb_IVisGeneric->SetStrencilShadow(true);
	
	gb_IVisGeneric->SetData(gb_IRenderDevice);
	// создание сцены
	gb_Scene=gb_IVisGeneric->CreateScene(); 
	gb_Scene->DisableTileMapVisibleTest();
	// создание камеры
	gb_Camera=gb_Scene->CreateCamera();
	gb_Camera->SetAttr(ATTRCAMERA_PERSPECTIVE); // перспектива
	ResetCameraPosition();

	gb_Camera->SetFrustum(							// устанавливается пирамида видимости
		&Vect2f(0.5f,0.5f),							// центр камеры
		&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f),		// видимая область камеры
		&Vect2f(1.f,1.f),							// фокус камеры
		&Vect2f(10.0f,3000.0f)						// ближайший и дальний z-плоскости отсечения
		);

	// создается источник света, иначе кромешная тьма и объектов не видно
	gb_ULight1=gb_Scene->CreateLight(ATTRLIGHT_DIRECTION);
	gb_ULight1->SetPosition(MatXf(Mat3f::ID,Vect3f(0,0,0)));
//	gb_ULight1->SetPosition(&MatXf(Mat3f::ID,Vect3f(100,100,0)));

//	gb_ULight1->SetColor(&sColor4f(0,0,0,1),&sColor4f(0.5,0.5,0.5,1));
	gb_ULight1->SetColor(&sColor4f(1,1,1,1),&sColor4f(1,1,1,1));

	gb_ULight1->SetDirection(Vect3f(0,-1,-1));
	gb_ILogicGeneric=CreateILogicGeneric();

	if(false)
	{
		pPlane=gb_Scene->CreatePlaneObj();
		cTexture* pTexture=gb_IVisGeneric->CreateTexture("TEXTURES\\028.tga");

		float sz=2000.0f;
		MatXf mat(MatXf::ID);
		mat.rot().set(Vect3f(0,1,0),0.1f);
		mat.trans().x=-sz*0.5f;
		mat.trans().y=-sz*0.5f;
		mat.trans().z=-300;

		pPlane->SetPosition(mat);
		pPlane->SetScale(Vect3f(sz,sz,0));
		pPlane->SetTexture(0,pTexture);
		pPlane->SetUV(0,0,3,3);
	}

	static bool first_load_registry=true;
	if(first_load_registry)
	{
		gb_FrameWnd->LoadRegistry();
		first_load_registry=false;
	}

//	gb_IVisGeneric->SetFarDistanceLOD(15);//to use lod
	gb_IVisGeneric->SetShowType(SHOW_TILEMAP,enable_tilemap);

	return 0;
}

void CWinVGView::DoneRenderDevice()
{
	CWinVGDoc* pDoc=GetDocument();
	if(pDoc)
	{
		if(pDoc->m_pHierarchyObj) pDoc->m_pHierarchyObj->ClearRoot();
		if(pDoc->m_pHierarchyLogic) pDoc->m_pHierarchyLogic->ClearRoot();
	}
	RELEASE(gb_ULight1);
	RELEASE(pLightMap);
	RELEASE(gb_TileMap);
	RELEASE(pPlane);

	RELEASE(gb_Camera);
	RELEASE(gb_Scene);
	// закрытие окна вывода
	if(gb_IVisGeneric) gb_IVisGeneric->ClearData();
	RELEASE(gb_IVisGeneric);
	RELEASE(gb_IRenderDevice);
	RELEASE(gb_ILogicGeneric);
}

static void SetExtension(const char *fnameOld,const char *extension,char *fnameNew)
{
	strcpy(fnameNew,fnameOld);
	for(int l=strlen(fnameNew)-1;l>=0&&fnameNew[l]!='\\';l--)
		if(fnameNew[l]=='.')
			break;
	if(l>=0&&fnameNew[l]=='.') 
		strcpy(&fnameNew[l+1],extension);
	else
		strcat(fnameNew,extension);
}

void CWinVGView::LoadObject(LPCSTR fName)
{
	b_viewHologram=false;
//	gb_IVisGeneric->GetLibrary()->Free();
	char fname[256];
	SetExtension(fName,"m3d",fname);
	_strlwr(fname);
	if(gb_ILogicGeneric==0) return;
	CWinVGDoc* pDoc=GetDocument();

	target_pos=Vect3f(1024,1024,400);
	MatXf pos=GetMatrix(target_pos,Vect3f(0,0,0));
	MatXf p1,p2;

	char TexturePath[1024];
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath( fName, drive, dir, fname, ext );

		sprintf(TexturePath,"%s%s%s\\", drive, dir,textures_subdir.c_str());
	}

	if(num_object_linear<2)
	{
		cIUnkClass *UObj=gb_Scene->CreateObject(fname,TexturePath);
		pDoc->m_pHierarchyObj->SetRoot(UObj,fname);
		if(UObj)
		{
			p1=UObj->GetPosition();
			UObj->SetPosition(pos);
		}
	}else
	{
		target_pos=Vect3f(1024,1024,200);
		int num=num_object_linear/2;
		bool first=true;
		for(int x=-num;x<num;x++)
		for(int y=-num;y<num;y++)
		{
			cIUnkClass *UObj=gb_Scene->CreateObject(fname,TexturePath);

			if(first)
				pDoc->m_pHierarchyObj->SetRoot(UObj,fname);
			else
				pDoc->m_pHierarchyObj->AddRoot(UObj);

			first=false;

			if(UObj==NULL)
				break;

			const float mul=500.0f/num_object_linear;
			UObj->SetPosition(GetMatrix(Vect3f(1024+x*mul,1024+y*mul,200),Vect3f(0,0,0)));
			UObj->SetColor(NULL,&sColor4f(1,1,1,1.f),NULL);

		}
	}

	pDoc->m_pHierarchyObj->SetSkinColor(SkinColor);
	vector<cIUnkClass*>& obj=pDoc->m_pHierarchyObj->GetAllObj();
	vector<cIUnkClass*>::iterator it;

	FOR_EACH(obj,it)
	{ 
		cIUnkClass* UObj=*it;
		UObj->SetAttr(ATTRUNKOBJ_SHADOW);
	}


	SetExtension(fName,"l3d",fname);
	_strlwr(fname);

	FILE* ftest=fopen(fname,"rt");
	if(ftest)
	{
		fclose(ftest);
		cIUnkClass* logic_obj=(cIUnkClass*)gb_ILogicGeneric->GetElement(fname);
		pDoc->m_pHierarchyLogic->SetRoot(logic_obj,fname);
	}

	UpdateObjectLight();
	gb_FrameWnd->UpdateList();

	SetScale(gb_FrameWnd->scale_normal);
}

void CWinVGView::UpdateObjectLight()
{
	if(pDoc)
	{
		vector<cIUnkClass*>& obj=pDoc->m_pHierarchyObj->GetAllObj();
		vector<cIUnkClass*>::iterator it;

		FOR_EACH(obj,it)
		{ 
			cIUnkClass* UObj=*it;
			if(g_bPressLighing) UObj->ClearAttr(ATTRUNKOBJ_NOLIGHT);
			else UObj->SetAttr(ATTRUNKOBJ_NOLIGHT);
		}
	}
}

void CWinVGView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if(cx<=0||cy<=0) return;

	dwScrX=cx;
	dwScrY=cy;
	if(gb_IRenderDevice)
	{
		gb_IRenderDevice->ChangeSize(cx,cy,RENDERDEVICE_MODE_WINDOW);
		gb_Camera->Update();
		SetScale(gb_FrameWnd->scale_normal);
	}else
		InitRenderDevice(cx,cy);
/*
	CWinVGDoc* pDoc=GetDocument();
	CString fname;
	if(pDoc&&pDoc->m_pHierarchyObj)
		fname=pDoc->m_pHierarchyObj->GetFileName();
	DoneRenderDevice();
	InitRenderDevice(dwScrX=cx,dwScrY=cy);
	if(!fname.IsEmpty()) LoadObject(fname);
*/
}

void CWinVGView::SetScale(bool normal)
{
	CWinVGDoc* pDoc=GetDocument();

	cIUnkClass* ULogicObj=pDoc->m_pHierarchyLogic->GetRoot();
	vector<cIUnkClass*>& obj=pDoc->m_pHierarchyObj->GetAllObj();
	vector<cIUnkClass*>::iterator it;

	FOR_EACH(obj,it)
	{ 
		cIUnkClass* UObj=*it;
		cObjectNodeRoot* node=dynamic_cast<cObjectNodeRoot*>(UObj);
		float Radius=UObj->GetBoundRadius();
		MatXf Matrix;
		Matrix=UObj->GetPosition();

//		if(num_object_linear==1)
//			UObj->SetPosition(&MatXf(Matrix.rot(),Vect3f(1024,1024,0))); 
		if(Radius<=0)
			Radius=1;
		float mul=400/(num_object_linear*Radius);
		if(normal)
			node->SetScale(Vect3f(mul,mul,mul));
		else
			node->SetScale(Vect3f(1,1,1));

		UObj->SetPosition(UObj->GetPosition());
	} 
}

void CWinVGView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	Vect3f dPos(0,0,0),dAngleGrad(0,0,0);
	float dScale=1;
	CWinVGDoc* pDoc=GetDocument();

	cIUnkClass* ULogicObj=pDoc->m_pHierarchyLogic->GetRoot();
	vector<cIUnkClass*>& obj=pDoc->m_pHierarchyObj->GetAllObj();

	if(nChar==VK_F2)
	{ 
		FramePhase.Set(0,0); 
	}
	else if(nChar==VK_F3)
	{ 
		FramePhase.Set(0,1); 
	}
	else if(nChar==VK_F4)
	{ 
		FramePhase.Set(frame_period,0); 
	}
	else if(nChar=='A')		pDoc->m_pHierarchyObj->OnKeyDown(nChar,nRepCnt,nFlags); // OnKeyDown(); dPos.set(  0,  0,-5);
	else if(nChar=='Z')		pDoc->m_pHierarchyObj->OnKeyDown(nChar,nRepCnt,nFlags);
	else if(nChar=='D')		dPos.set(  0, 1,  0);
	else if(nChar=='C')		dPos.set(  0,-1,  0);
	else if(nChar=='F')		dPos.set( 1,  0,  0);
	else if(nChar=='V')		dPos.set(-1,  0,  0);
	else if(nChar=='S')		dScale=1.03f*nRepCnt;
	else if(nChar=='X')		dScale=0.98f*nRepCnt;
	else if(nChar==VK_DOWN)	dAngleGrad.set( 0,  0,  5);
	else if(nChar==VK_UP)	dAngleGrad.set( 0,  0, -5);
	else if(nChar==VK_LEFT)	dAngleGrad.set( 5,  0, 0);
	else if(nChar==VK_RIGHT)dAngleGrad.set(-5,  0, 0);
	else if(nChar==VK_SUBTRACT)	{ m_ScaleTime*=0.9f; m_ScaleTime=max(1e-3,m_ScaleTime); }
	else if(nChar==VK_ADD)		{ m_ScaleTime*=1.1f; m_ScaleTime=min(1e3,m_ScaleTime); }
	else if(nChar=='P')
	{
		if(m_ScaleTime==0) 
			m_ScaleTime=m_OldScaleTime; 
		else 
			m_OldScaleTime=m_ScaleTime, m_ScaleTime=0;
	}
	else if(nChar=='W') bWireFrame=!bWireFrame;
	else if(nChar=='J')
	{
		pDoc->m_pHierarchyObj->SetSkinColor(SkinColor);
	}
	

	ObjectControl(dPos*(float)nRepCnt,dAngleGrad*(float)nRepCnt,dScale);
}

void CWinVGView::ObjectControl(Vect3f &dPos,Vect3f &dAngle,float dScale)
{
	CWinVGDoc* pDoc=GetDocument();
	switch(m_ControlSwitch)
	{
		case 0:
			{
				vector<cIUnkClass*>& obj=pDoc->m_pHierarchyObj->GetAllObj();
				cLogicObject* lobj=pDoc->m_pHierarchyLogic->GetRoot()?dynamic_cast<cLogicObject*>(pDoc->m_pHierarchyLogic->GetRoot()):NULL;

				vector<cIUnkClass*>::iterator it;
				FOR_EACH(obj,it)
				{
					Vect3f Scale;
					MatXf LocalMatrix=TransposeMeshScr(*it,gb_Camera,dPos,dAngle);
					if(lobj && it==obj.begin())
					{
						lobj->SetPosition(LocalMatrix);
						Scale=lobj->GetScale();
						Scale*=dScale;
						lobj->SetScale(Scale);
					}

					if(fabsf(dScale-1.0f)>FLT_EPS)
					{
						cIUnkClass* unk=*it;
						cObjectNodeRoot* cur=dynamic_cast<cObjectNodeRoot*>(*it);
						Scale=cur->GetScale();
						Scale*=dScale;
						cur->SetScale(Scale);
					}

					(*it)->SetPosition((*it)->GetPosition());
				}
			}
			break;
		case 1:
			if(gb_Camera)
			{
				const float mul=0.01f;
				float du=dAngle.x*mul,dv=dAngle.z*mul;
				SetCameraPosition(du,dv,1/dScale);
			}
			break;
		case 2:
			if(gb_ULight1)
			{
				const float mul=0.01f;
				static float alpha=-M_PI/2,teta=-M_PI/4;
				alpha+=dAngle.x*mul;
				teta+=dAngle.z*mul;

				Vect3f d;
				d.x=cosf(teta)*cosf(alpha);
				d.y=cosf(teta)*sinf(alpha);
				d.z=sinf(teta);

				gb_ULight1->SetDirection(d);
			}
			break;
		default:
			m_ControlSwitch=0;
	}
}

void CWinVGView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(bMouseLDown==0) bMouseLDown=1;
	PointMouseLDown=point;
	CView::OnLButtonDown(nFlags, point);
}

void CWinVGView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(bMouseLDown) bMouseLDown=0;
	CView::OnLButtonUp(nFlags, point);
}

void CWinVGView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if(bMouseRDown==0) bMouseRDown=1;
	PointMouseRDown=point;
	CView::OnRButtonDown(nFlags, point);
}

void CWinVGView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if(bMouseRDown) bMouseRDown=0;
	CView::OnRButtonUp(nFlags, point);
}

void CWinVGView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(bMouseLDown) 
		ObjectControl(Vect3f(0,0,0),Vect3f((float)point.x-PointMouseLDown.x,0,(float)point.y-PointMouseLDown.y),1);
	if(bMouseRDown) 
		ObjectControl(Vect3f((float)point.x-PointMouseRDown.x,(float)point.y-PointMouseRDown.y,0),Vect3f(0,0,0),1);
	PointMouseLDown=point;
	PointMouseRDown=point;

	/*
		Vect2f pos(point.x/(float)gb_IRenderDevice->GetSizeX()-0.5f,
				   point.y/(float)gb_IRenderDevice->GetSizeY()-0.5f);
		gb_Camera->ConvertorCameraToWorld(&test_pos,&pos);
	*/
	
	CView::OnMouseMove(nFlags, point);
}

BOOL CWinVGView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	ObjectControl(Vect3f(0,0,0),Vect3f(0,0,0),1+zDelta/2000.f);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CWinVGView::OnNumObject() 
{
	num_object_linear=num_object_linear==1?10:1;
	CWinVGDoc* pDoc = GetDocument();
	if(pDoc==NULL) return;

	if(pDoc->m_pHierarchyObj->GetFileName().IsEmpty())return;

	LoadObject(pDoc->m_pHierarchyObj->GetFileName());
	pDoc->m_pHierarchyLogic->TreeUpdate();
	pDoc->m_pHierarchyObj->TreeUpdate();
}

void CWinVGView::OnUpdateNumObject(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(num_object_linear!=1);
}

void CWinVGView::UpdateIgnore()
{
	CWinVGDoc* pDoc = GetDocument();
	if(pDoc==NULL || gb_FrameWnd==NULL) return;
	gb_IVisGeneric->SetDrawMeshScreen(gb_FrameWnd->m_bPressObject);
/*
	vector<cIUnkClass*>& hobj=pDoc->m_pHierarchyObj->GetAllObj();
	vector<cIUnkClass*>::iterator it;

	FOR_EACH(hobj,it)
	{
		cIUnkClass* UObj=*it;
		if(gb_FrameWnd->m_bPressObject)
			UObj->ClearAttr(ATTRUNKOBJ_IGNORE);
		else
			UObj->SetAttr(ATTRUNKOBJ_IGNORE);

	}
*/
}

void CWinVGView::ModelInfo() 
{
	CWinVGDoc* pDoc = GetDocument();
	if(pDoc==NULL)return;
	cIUnkClass* UObj=pDoc->m_pHierarchyObj?pDoc->m_pHierarchyObj->GetRoot():NULL;
	if(UObj==NULL)return;

	CModelInfo dlg(this);	
	dlg.Init((cObjectNodeRoot*)UObj);
	dlg.DoModal();
}

void CWinVGView::OnHologram() 
{
	CWinVGDoc* pDoc = GetDocument();
	if(pDoc==NULL) return;

	b_viewHologram=!b_viewHologram;
	vector<cIUnkClass*>& hobj=pDoc->m_pHierarchyObj->GetAllObj();
	vector<cIUnkClass*>::iterator it;

	if(!pLightMap)
		pLightMap=gb_IVisGeneric->CreateTexture("TEXTURES\\lightmap.tga");
	
	RandomGenerator rnd;
	FOR_EACH(hobj,it)
	{
		cObjectNode* UObj=dynamic_cast<cObjectNode*>(*it);
		if(b_viewHologram)
		{
			UObj->SetTexture(pLightMap,NULL);
			sColor4f a(0,0,0,0);
			UObj->SetColor(&a,&SkinColor,&a);
		}else
		{
			UObj->SetTexture(NULL,NULL);
//*
			sColor4f a(0,0,0,0),d(1,1,1,1);
			UObj->SetColor(&a,&d,&a);
/*/
			sColor4f d(rnd.frand(),rnd.frand(),rnd.frand(),1);
			//float f=(it-hobj.begin())/(float)hobj.size();sColor4f d(0,f,0,1);
			UObj->SetColor(&d,&d,&d);
/**/
		}
	}

}

void CWinVGView::OnUpdateHologram(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(b_viewHologram);
}

void terCalcIntersectionRay(float x,float y,Vect3f& v0,Vect3f& v1)
{
	MatXf matrix;

	gb_Camera->ConvertorCameraToWorld(&v1,&Vect2f(x,y));
	if(gb_Camera->GetAttr(ATTRCAMERA_PERSPECTIVE))
	{
		gb_Camera->GetPosition(&matrix);
		v0 = matrix.invXformVect(matrix.trans(),v0);
		v0.negate();
	}else
	{
		v0.x = v1.x;
		v0.y = v1.y;
		v0.z = v1.z + 10.0f;
	}

	Vect3f dir=v1-v0;
	float m=2000.0f/dir.norm();
	dir*=m;
	v1=v0+dir;	
};

void CWinVGView::TestObject(cIUnkClass* UObj)
{	
	int dx=gb_IRenderDevice->GetSizeX();
	int dy=gb_IRenderDevice->GetSizeY();
	cObjectNode* Obj=(cObjectNode*)UObj;

	int x,y;
	for(y=0;y<dy;y+=10)
	for(x=0;x<dx;x+=10)
	{
		Vect3f v0,v1;
		terCalcIntersectionRay(x/(float)dx-0.5f,y/(float)dy-0.5f,v0,v1);

		if(Obj->Intersect(v0,v1))
			gb_IRenderDevice->DrawPixel(x,y,sColor4c(0,255,0,255));
		else
			gb_IRenderDevice->DrawPixel(x,y,sColor4c(0,0,0,255));
	}

}

class EmptyTerraInterface:public TerraInterface
{
public:
	int SizeX(){return 2048;}
	int SizeY(){return 2048;}
	int GetZ(int x,int y)
	{
		return 1;
	}

	float GetZf(int x,int y)
	{
		return 1;
	}

	int GetHZeroPlast() {return 5;}

	int GetReductionZ(int x,int y)
	{
		return 0;
	}
	int GetReductionShift() {return 2;}

	virtual class Column* GetColumn(int player){return NULL;}
	virtual void GetBorder(int player,borderCall call,void* data){}

	virtual void GetTileColor(char* Texture,DWORD pitch,int xstart,int ystart,int xend,int yend,int step)
	{
		for(int y = ystart; y < yend; y += step)
		{
			DWORD* tx=(DWORD*)Texture;
			for (int x = xstart; x < xend; x += step)
			{
			/*
				DWORD c=(x*y>>4)&0xFF;
				DWORD color=0xFF000000+c+(c<<8)+(c<<16);
				*tx = color;
			/*/
				*tx=0xFF808080;
			/**/
				tx++;
			}
			Texture += pitch;
		}
		
	}
};

void SetUseShadow(bool use)
{
	dprintf(use?"use":"not use");
	if(!gb_TileMap)
		gb_TileMap=gb_Scene->CreateMap(new EmptyTerraInterface,0);

	gb_TileMap->PutAttr(ATTRCAMERA_IGNORE,!use);
	gb_IVisGeneric->SetShadowType(SHADOW_MAP_SELF,use?3:0);
}

void CWinVGView::DrawLocalBorder(cObjectNodeRoot* root)
{
	int nVertex;
	Vect3f *Vertex;
	int nIndex;
	short *Index;
	MatXf mat=root->GetPosition();
	mat.rot().scale(root->GetScale());

	root->GetLocalBorder(&nVertex,&Vertex,&nIndex,&Index);

	gb_IRenderDevice->SetNoMaterial(ALPHA_NONE);

	cVertexBuffer<sVertexXYZD>& buf=*gb_IRenderDevice->GetBufferXYZD();

	int ntriangle=0;
	nIndex*=3;

	sVertexXYZD* v=buf.Lock();
	sColor4c c(255,255,0);

	for(int index=0;index<nIndex;index+=3)
	{
		int idx=ntriangle*3;
		for(int i=0;i<3;i++,idx++)
		{
			int cur_index=Index[index+i];
			xassert(cur_index>=0 && cur_index<nVertex);
			v[idx].pos=Vertex[cur_index];
			v[idx].diffuse=c;
		}

		ntriangle++;

		if( ntriangle*3>=buf.GetSize()-4)
		{
			buf.Unlock(ntriangle*3);
			buf.DrawPrimitive(PT_LINELIST,ntriangle,mat);
			v=buf.Lock();
			ntriangle=0;
		}
	}

	buf.Unlock(ntriangle*3);
	if(ntriangle)
		buf.DrawPrimitive(PT_TRIANGLELIST,ntriangle,mat);
	
}

void CWinVGView::OnScreenShoot()
{
	CScreenShotSize dlg;

	static screenshot_x=800;
	dlg.x=screenshot_x;
	if(dlg.DoModal()!=IDOK)
		return;
	screenshot_x=dlg.x;
	if(screenshot_x<16)
		screenshot_x=16;

	cObjectNodeRoot* UObj=pDoc->m_pHierarchyObj?dynamic_cast<cObjectNodeRoot*>(pDoc->m_pHierarchyObj->GetRoot()):NULL;

	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath( UObj?UObj->GetFileName():"screenshot", drive, dir, fname, ext );
	_makepath(path_buffer,drive, dir, fname, ".bmp");

	CFileDialog file(FALSE, "bmp",path_buffer , 0, 
		"*.bmp|*.bmp|All Files (*.*)|*.*||", this );
	if(file.DoModal()!=IDOK)
		return;

	int cx=screenshot_x;
	int cy=round(cx*(dwScrY/(float)dwScrX));

	if(gb_IRenderDevice->ChangeSize(cx,cy,RENDERDEVICE_MODE_WINDOW|RENDERDEVICE_MODE_ALPHA|RENDERDEVICE_MODE_RETURNERROR))
	{
		gb_Camera->Update();
		Draw(false);
		if(!gb_IRenderDevice->SetScreenShot(file.GetPathName()))
		{
			AfxMessageBox("Cannot save screenshoot");
		}
	}else
	{
		AfxMessageBox("Cannot initialize grapics ti save screen shoot");
	}

	gb_IRenderDevice->ChangeSize(dwScrX,dwScrY,RENDERDEVICE_MODE_WINDOW);
	gb_Camera->Update();
	
}
