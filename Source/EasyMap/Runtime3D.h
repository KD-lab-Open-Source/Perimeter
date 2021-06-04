#pragma once
class Runtime3D
{
protected:
	Vect2i mouse_pos;
public:

	Runtime3D():mouse_pos(0,0){}
	virtual ~Runtime3D(){};
	virtual void Init(){};
	virtual void Quant();
	virtual void DrawFps(int x,int y);
	virtual void KeyDown(int key){};

	virtual void OnLButtonDown(){};
	virtual void OnLButtonUp(){};
	virtual void OnRButtonDown(){};
	virtual void OnRButtonUp(){};

	void SetMousePos(int x,int y){mouse_pos.set(x,y);}
};

Runtime3D* CreateRuntime3D();

void SetCameraPosition(cCamera *UCamera,const MatXf &Matrix);
void FixCameraRotation(Mat3f& m);

extern cVisGeneric		*gb_IVisGeneric;
extern cScene			*terScene;
extern cCamera			*gb_Camera;
extern cInterfaceRenderDevice *terRenderDevice;

extern HWND g_hWnd;