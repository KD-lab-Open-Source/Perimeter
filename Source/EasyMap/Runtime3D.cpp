#include "stdafx.h"
#include "Runtime3D.h"
#include <queue>
#include "fps.h"
HWND g_hWnd=NULL;

Runtime3D* g_runtime=NULL;
cInterfaceRenderDevice	*terRenderDevice=0;
cUnkLight				*gb_ULight1=0;
cVisGeneric				*gb_IVisGeneric=0;
cScene					*terScene=0;
cCamera					*gb_Camera=0;

BOOL g_bActive=FALSE;

void SetCameraPosition(cCamera *UCamera,const MatXf &Matrix)
{
	MatXf ml=MatXf::ID;
	ml.rot()[2][2]=-1;

	MatXf mr=MatXf::ID;
	mr.rot()[1][1]=-1;
	MatXf CameraMatrix;
	CameraMatrix=mr*ml*Matrix;

	UCamera->SetPosition(CameraMatrix);
}

int InitRenderDevice(int xScr,int yScr)
{
	gb_IVisGeneric=CreateIVisGeneric();
	terRenderDevice=CreateIRenderDevice();
	if(terRenderDevice->Init(xScr,yScr,RENDERDEVICE_MODE_RGB32|RENDERDEVICE_MODE_WINDOW,g_hWnd))
		return 1;
	gb_IVisGeneric->SetData(terRenderDevice);
	// создание сцены
	terScene=gb_IVisGeneric->CreateScene(); 
	// создание камеры
	gb_Camera=terScene->CreateCamera();
	gb_Camera->SetAttr(ATTRCAMERA_PERSPECTIVE); // перспектива
	MatXf CameraMatrix;
	Identity(CameraMatrix);
	Vect3f CameraPos(0,0,-512);
	SetPosition(CameraMatrix,CameraPos,Vect3f(0,0,0));
	SetCameraPosition(gb_Camera,CameraMatrix);

	gb_Camera->SetFrustum(							// устанавливается пирамида видимости
		&Vect2f(0.5f,0.5f),							// центр камеры
		&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f),		// видимая область камеры
		&Vect2f(1.f,1.f),							// фокус камеры
		&Vect2f(10.0f,3000.0f)
		);

	// создается источник света, иначе кромешная тьма и объектов не видно
	gb_ULight1=terScene->CreateLight(ATTRLIGHT_DIRECTION);
	gb_ULight1->SetPosition(MatXf(Mat3f::ID,Vect3f(0,0,0)));

	gb_ULight1->SetColor(&sColor4f(0,0,0,1),&sColor4f(1,1,1,1));

	gb_ULight1->SetDirection(Vect3f(-1,0,-1));
	//gb_ULight1->SetDirection(&Vect3f(0,-1,-1));
	return 0;
}

void DoneRenderDevice()
{
	RELEASE(gb_ULight1);
	RELEASE(gb_Camera);
	RELEASE(terScene);
	// закрытие окна вывода
	if(gb_IVisGeneric) gb_IVisGeneric->ClearData();
	RELEASE(gb_IVisGeneric);
	RELEASE(terRenderDevice);
}

//Функция обработки сообщений Windows
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY://выйти из приложения, если нажали на крестик
            PostQuitMessage( 0 );
            return 0;
		case WM_ACTIVATEAPP:
			g_bActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
			return 0;
		case WM_MOUSEMOVE:
			if(g_runtime)
				g_runtime->SetMousePos(LOWORD(lParam),HIWORD(lParam)); 
			break;
		case WM_GETMINMAXINFO:
			{
				MINMAXINFO *pMinMax;
				pMinMax = (MINMAXINFO *)lParam;
				POINT& p=pMinMax->ptMaxTrackSize;
				p.x=GetSystemMetrics(SM_CXSCREEN);
				p.y=GetSystemMetrics(SM_CYSCREEN);
				p.x += GetSystemMetrics(SM_CXSIZEFRAME)*2;
				p.y += GetSystemMetrics(SM_CYSIZEFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);
				return 0;
			}
		case WM_KEYDOWN:
			g_runtime->KeyDown(wParam);
			break;
		case WM_SETCURSOR:
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			break;
		case WM_LBUTTONDOWN:
			g_runtime->OnLButtonDown();
			break;
		case WM_LBUTTONUP:
			g_runtime->OnLButtonUp();
			break;
		case WM_RBUTTONDOWN:
			g_runtime->OnRButtonDown();
			break;
		case WM_RBUTTONUP:
			g_runtime->OnRButtonUp();
			break;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	initclock();
	LPCSTR classname="3d demo";
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC|CS_HREDRAW |CS_VREDRAW, MsgProc, 0L, 0L,
                      hInst, NULL, NULL, NULL, NULL,
                      classname, NULL };

	int xPos=0,yPos=0;
	int xIn=GetSystemMetrics(SM_CXSCREEN),yIn=GetSystemMetrics(SM_CYSCREEN);
//	int xIn=640,yIn=512;
	int xScr=xIn,yScr=yIn;
	xPos-=GetSystemMetrics(SM_CXSIZEFRAME);
	yPos-=GetSystemMetrics(SM_CYSIZEFRAME)+GetSystemMetrics(SM_CYCAPTION);
	xScr += GetSystemMetrics(SM_CXSIZEFRAME)*2;
	yScr += GetSystemMetrics(SM_CYSIZEFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);
    RegisterClassEx( &wc );
    HWND hWnd = CreateWindow( classname, classname,
                              WS_OVERLAPPEDWINDOW, xPos, yPos, xScr, yScr,
                              GetDesktopWindow(), NULL, hInst, NULL );
	g_hWnd=hWnd;

	ShowWindow( hWnd, SW_SHOWNORMAL );
	UpdateWindow( hWnd );

	if(InitRenderDevice(xIn,yIn))
	{
		MessageBox(hWnd,"Cannot initialize 3d graphics","EasyMap",MB_OK);
		return 0;
	}

	g_runtime=CreateRuntime3D();
	g_runtime->Init();

	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );
	while( msg.message!=WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE ) )
		{
            if(!GetMessage(&msg, NULL, 0, 0))
				break;
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}else
		if(g_bActive)
			g_runtime->Quant();
		else
			WaitMessage();
	}

	if(g_runtime)
		delete g_runtime;

	DoneRenderDevice();

    UnregisterClass( classname, hInst );
    return 0;
}

////////////////////////////Runtime3D///////////////////////

FPS fps;

void Runtime3D::Quant()
{
	fps.quant();
}

void Runtime3D::DrawFps(int x,int y)
{
	char str[200];
	float cur_fps=fps.GetFPS();
	float fpsmin,fpsmax;
	fps.GetFPSminmax(fpsmin,fpsmax);

	if(cur_fps>100.0f)
		sprintf(str,"fps=%i",round(cur_fps));
	else 
	if(cur_fps>10.0f)
		sprintf(str,"fps=%.1f",round(cur_fps*10)*0.1f);
	else
		sprintf(str,"fps=%.2f",round(cur_fps*100)*0.01f);

	sprintf(str+strlen(str)," min=%.1f, max=%.1f",round(fpsmin*10)*0.1f,round(fpsmax*10)*0.1f);

	terRenderDevice->OutText(x,y,str,sColor4f(1,1,0,1));
}