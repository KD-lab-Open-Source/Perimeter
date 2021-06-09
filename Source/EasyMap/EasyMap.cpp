#include "StdAfx.h"
#include "Runtime3D.h"
#include "../Terra/terra.h"
#include "../UserInterface/chaos.h"
#include "../PluginMAX/ZIPStream.h"
#include "TerraInterface.inl"
#include <crtdbg.h>
#include "../Water/Water.h"
#include "../Water/WaterGarbage.h"
#include "../Wind/wind.h"
#include "../Render/client/WinVideo.h"

cTileMap		*terMapPoint;
cFont* pFont;



EffectLibrary effect_lib;

class Demo3D:public Runtime3D
{
	double prevtime;
    Vect3f	vPosition;
	Vect2f	AnglePosition;
	CChaos* pChaos;

	bool wireframe;

	EffectKey* effect_key;
	cWater* pWater;

	cObject3dx* sphere;
	cWaterGarbage* pWaterGarbage;
	cWaterBubble* pWaterBubble;
public:
	Demo3D();
	~Demo3D();
	void Init();
	void Quant();

	void Animate(float dt);
	void Draw();

	void CameraQuant(float dt);
	void DrawMousePos();
	void KeyDown(int key);

	Vect3f MousePos3D();

	void LogicQuant();
protected:
	bool show_help;

	enum
	{
		CLIMATE_HOT=0,
		CLIMATE_NORMAL=1,
	} climate;

	int enviroment_water;
	vector<Vect2i> water_source;
	bool mouse_lbutton;
	bool mouse_rbutton;
	float logic_quant;
	int logic_speed;
	float logic_time;

	cMapWind wind;

	void OnLButtonDown(){mouse_lbutton=true;};
	void OnLButtonUp(){mouse_lbutton=false;};
	void OnRButtonDown(){mouse_rbutton=true;};
	void OnRButtonUp(){mouse_rbutton=false;};

	void ShowHelp();
//	sVideoWrite video;
};

void TestSaveVRML(const char* filename);

Runtime3D* CreateRuntime3D()
{
	return new Demo3D;
}

Demo3D::Demo3D()
{
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_CHECK_ALWAYS_DF);
	prevtime=0;
	terMapPoint=NULL;
	pFont=NULL;

//	vPosition.set(1015.28f,823.464f,488.779f);
//	AnglePosition.set(0.404606f,7.91086f);
	vPosition.set(945.894f,2042.05f,2197.22f);
	AnglePosition.set(0.404606f,7.91086f);
	pChaos=NULL;

	wireframe=false;
	effect_key=NULL;
	pWater=NULL;
	sphere=NULL;
	show_help=false;
	climate=CLIMATE_NORMAL;
	mouse_lbutton=false;
	mouse_rbutton=false;
	logic_quant=0.1f;
	logic_speed=2;
	logic_time=0;
}

Demo3D::~Demo3D()
{
	RELEASE(sphere);
	RELEASE(pWater);
	delete pWaterGarbage;
	RELEASE(pWaterBubble);
	RELEASE(terMapPoint);
	RELEASE(pFont);
	delete pChaos;
}

void Demo3D::Init()
{
	gb_IVisGeneric->SetEffectLibraryPath("RESOURCE\\FX","RESOURCE\\FX\\Textures");
	gb_IVisGeneric->SetFontRootDirectory("resource\\LocData");
	gb_IVisGeneric->SetFontDirectory("resource\\LocData\\English\\Fonts");
//	terScene->DisableTileMapVisibleTest();
//	TestSaveVRML("RESOURCE\\MODELS\\MAIN\\Frame_Imperia_v_2.m3d");
/*
	gb_IVisGeneric->SetShadowType(SHADOW_MAP_SELF,0);
	vMap.ShadowControl(false);
/*/
//	gb_IVisGeneric->SetShadowType(SHADOW_MAP,3);
	vMap.ShadowControl(true);
/**/
	gb_IVisGeneric->SetEnableBump(true);
	gb_IVisGeneric->EnableOcclusion(true);
//	gb_IVisGeneric->SetShowRenderTextureDBG(TRUE);

	pFont=gb_IVisGeneric->CreateFont("Arial",20);

//*
	vMap.prepare("RESOURCE\\WORLDS\\worlds.prm");

	vMap.selectUsedWorld(6);//12);//6 34 2 19
	vMap.fullLoad(false);
	vMap.WorldRender();

	terMapPoint = terScene->CreateMap(new StandartTerraInterface,0);
/*
	effect_lib.Load("RESOURCE\\FX\\zemitter.effect","RESOURCE\\FX\\Textures");
	effect_key=effect_lib.Get("Effect");

	EmitterKeyLight* key=new EmitterKeyLight;
	key->texture_name="RESOURCE\\EFFECT\\lightmap.tga";
	effect_key->key.push_back(key);

/**/
//	terMapPoint->SetAttribute(ATTRUNKOBJ_REFLECTION);

	pWater=new cWater;
	pWater->Init(vMap.H_SIZE,vMap.V_SIZE);
	enviroment_water=34;
	pWater->SetEnvironmentWater(enviroment_water);
	sphere=terScene->CreateObject3dx("resource\\balmer\\SmallSphere.3DX");

	pWaterGarbage=new cWaterGarbage(pWater);
	pWaterBubble=new cWaterBubble(pWater);
	pWater->SetSpeedInterface(pWaterGarbage);
	terScene->AttachObj(pWater);
	terScene->AttachObj(pWaterBubble);

	wind.Init(vMap.H_SIZE,vMap.V_SIZE,4,pWater);
	
	pWater->InitExternalSpeedTexture(wind.GetGridSize().x,wind.GetGridSize().y);
	pWater->SetTechnique(1);

	//GetTexLibrary()->GetElement("resource\\Cursors\\cursor_atack.avi");
//	video.Open("aaa.avi",320,256);
}

void Demo3D::Quant()
{
	Runtime3D::Quant();

	double curtime=clockf();
	float dt=min(100.0,curtime-prevtime);
	CameraQuant(dt);
	Animate(dt);
	prevtime=curtime;

	Draw();

	if(logic_speed!=3)
		logic_time+=dt*1e-3f;
	bool unlimit=logic_speed==2;
	if(logic_time>=logic_quant || unlimit)
	{
		LogicQuant();

		logic_time-=logic_quant;
		if(unlimit)
			logic_time=0;
	}
	pWaterGarbage->Animate(dt);

	if(mouse_lbutton || mouse_rbutton)
	{
		Vect3f pos=MousePos3D();
		Vect2i posi(pos.x,pos.y);

		//rad 
		//smth=0..10
		//dh=
		int dh=mouse_lbutton?2:-2;
		sToolzerPMO p;
		p.set(posi.x,posi.y,32,2,dh<<VX_FRACTION,0,0,0);
		vMap.deltaZone(p);
		p.set(posi.x,posi.y,32,2,0,0,0,0);
		vMap.deltaZone(p);
	}
	vMap.renderQuant();
}

void Demo3D::LogicQuant()
{
	if(pWater)
	{
		//pWater->AddWaterRect(2240,2048,1.5f,1);
		for(int i=0;i<water_source.size();i++)
		{
			Vect2i pos=water_source[i];
			//pWater->AddWaterRect(pos.x,pos.y,3.0f,1);
			pWater->AddWaterRect(pos.x,pos.y,1.0f,2);
		}

		pWater->AnimateLogic();
	}
}

void Demo3D::Draw()
{
	terScene->PreDraw(gb_Camera);

	sColor4f Color(0.5f,0.5f,0,1.0f);
	terRenderDevice->Fill(Color.GetR(),Color.GetG(),Color.GetB());
	terRenderDevice->BeginScene();
//	terRenderDevice->SetGlobalFog(sColor4f(0.5f,0.5f,0.5f,1),Vect2f(1000,2000));
	terRenderDevice->SetGlobalFog(sColor4f(0.5f,0.5f,0.5f,1),Vect2f(10000,20000));


	terRenderDevice->SetRenderState(RS_FILLMODE,wireframe?FILL_WIREFRAME:FILL_SOLID);
	terRenderDevice->SetFont(pFont);
	if(pChaos)pChaos->Draw();

	terScene->Draw(gb_Camera);
	wind.Draw();

	terRenderDevice->SetRenderState(RS_FILLMODE,FILL_SOLID);
	Runtime3D::DrawFps(256,0);
	ShowHelp();

//	DrawMousePos();

/*
	if(pWater)
	{
		char str[256];
		sprintf(str,"%.2f",pWater->GetChangeStat());
		terRenderDevice->OutText(20,120,str,sColor4f(1,1,0,1));
	}
*/
	terRenderDevice->SetFont(NULL);

	terRenderDevice->EndScene();
	terRenderDevice->Flush();

//	video.WriteFrame();
}

void Demo3D::Animate(float dt)
{
	terScene->dSetTime(dt);
	wind.Animate(dt);
/*
	cTexture* pTexture=pWater->GetExternalSpeedTexture();
	int pitch;
	BYTE* data=pTexture->LockTexture(pitch);
	wind.GetSpeed(data,pitch,5);
	pTexture->UnlockTexture();
*/
}

bool GetK(int key)
{
	return GetAsyncKeyState(key);
}

void Demo3D::CameraQuant(float dt)
{
    float fElapsedTime=dt/1000.0f;

    float fSpeed        = 800.0f*fElapsedTime;

	if(GetK(VK_CONTROL))
	{
		float fAngularSpeed = 4.0f*fElapsedTime;
		if(GetK(VK_UP))     AnglePosition.x+=-fAngularSpeed;
		if(GetK(VK_DOWN))   AnglePosition.x+=+fAngularSpeed;
		if(GetK(VK_LEFT))   AnglePosition.y+=-fAngularSpeed;
		if(GetK(VK_RIGHT))  AnglePosition.y+=+fAngularSpeed;
	}else
	{

		if(GetK(VK_RIGHT))  vPosition+=gb_Camera->GetWorldI()*fSpeed;
		if(GetK(VK_LEFT))   vPosition-=gb_Camera->GetWorldI()*fSpeed;
		if(GetK(VK_UP))     vPosition+=gb_Camera->GetWorldK()*fSpeed;
		if(GetK(VK_DOWN))   vPosition-=gb_Camera->GetWorldK()*fSpeed;
		if(GetK(VK_PRIOR))	vPosition+=gb_Camera->GetWorldJ()*fSpeed;
		if(GetK(VK_NEXT))   vPosition-=gb_Camera->GetWorldJ()*fSpeed;
	}

	Vect3f Target;
	Target.x=AnglePosition.x;
	Target.y=0;
	Target.z=M_PI/2 - AnglePosition.y;

	MatXf m = MatXf::ID;
	Mat3f mx(Target.x, X_AXIS),my(Target.y, Y_AXIS),mz(Target.z, Z_AXIS);
	m.rot() = mx*my*mz;
	::Translate(m,-vPosition);
	SetCameraPosition(gb_Camera,m);
}

void Demo3D::KeyDown(int key)
{
	if(key=='W')
	{
		wireframe=!wireframe;
	}
/*
	if(key=='1')
		gb_IVisGeneric->SetShadowHint(1);
	if(key=='2')
		gb_IVisGeneric->SetShadowHint(2);
	if(key=='3')
		gb_IVisGeneric->SetShadowHint(3);
	if(key=='4')
		gb_IVisGeneric->SetShadowHint(4);
	if(key=='0')
		gb_IVisGeneric->SetShadowHint(0);
	if(key=='S')
	{
		static bool self=true;
		self=!self;
		if(self)
		{
			gb_IVisGeneric->SetShadowType(SHADOW_MAP_SELF,3);
			vMap.ShadowControl(false);
		}else
		{
			gb_IVisGeneric->SetShadowType(SHADOW_MAP,3);
			vMap.ShadowControl(true);
		}
		vMap.WorldRender();
		terScene->GetTileMap()->UpdateMap(Vect2i(0,0),Vect2i((int)vMap.H_SIZE,(int)vMap.V_SIZE));
	}

	if(key=='D')
	{
		static bool self4x4=true;
		self4x4=!self4x4;
		gb_IVisGeneric->SetShadowMapSelf4x4(self4x4);
	}

	if(key=='E')
	{
		static bool is4=false;
		is4=!is4;
		gb_IVisGeneric->SetShadowType(SHADOW_MAP_SELF,is4?4:3);
		gb_IVisGeneric->SetMapLevel(is4?10000:100);
	}
*/
	if(key==VK_F1)
		show_help=!show_help;
	if(key=='1')
	{
		pWater->SetRainConstant(-0.03f);
		climate=CLIMATE_HOT;
	}
	if(key=='2')
	{
		pWater->SetRainConstant(-0.003f);
		climate=CLIMATE_NORMAL;
	}

	if(key==VK_ADD)
	{
		enviroment_water=min(enviroment_water+1,256);
		pWater->SetEnvironmentWater(enviroment_water);
	}
	if(key==VK_SUBTRACT)
	{
		enviroment_water=max(enviroment_water-1,0);
		pWater->SetEnvironmentWater(enviroment_water);
	}

	if(key==VK_INSERT)
	{
		Vect3f pos=MousePos3D();
		water_source.push_back(Vect2i(pos.x,pos.y));
		pWaterBubble->AddCenter(pos.x,pos.y,50,60);
	}
	if(key==VK_DELETE)
	{
		Vect3f pos=MousePos3D();
		Vect2i posi(pos.x,pos.y);
		int ifound=-1;
		int dist_found=INT_MAX;
		for(int i=0;i<water_source.size();i++)
		{
			Vect2i p=water_source[i];
			int dist=sqr(p.x-posi.x)+sqr(p.y-posi.y);
			if(dist<dist_found)
			{
				ifound=i;
				dist_found=dist;
			}
		}

		if(ifound>=0)
		{
			water_source.erase(water_source.begin()+ifound);
			pWaterBubble->DeleteNearCenter(pos.x,pos.y);
		}
	}

	if(key=='Z')
	{
		Vect3f pos=MousePos3D();
		pWaterBubble->AddCenter(pos.x,pos.y,50,60);
	}

	if(key=='X')
	{
		Vect3f pos=MousePos3D();
		pWaterBubble->DeleteNearCenter(pos.x,pos.y);
	}

	if(key=='A')
	{
		Vect3f pos=MousePos3D();
		cObject3dx* object=terScene->CreateObject3dx("resource\\balmer\\SmallSphere.3DX");
		object->SetScale(5);
		pWaterGarbage->Drop(pos.x,pos.y,object,50);
	}

	if(key=='Q')
	{
		Vect3f pos=MousePos3D();
		wind.CreateQuant(Vect2f(pos.x,pos.y), Vect2f(100,100),50, 200,0,0);
	}

	if(key=='E')
	{
		Vect3f pos=MousePos3D();
		wind.CreateQuantW(Vect2f(pos.x,pos.y), Vect2f(100,100),20, 200,0,0,5);
	}

	if(key==VK_F5)
	{
		logic_quant=0.1f;
		logic_speed=0;
	}

	if(key==VK_F6)
	{
		logic_quant=0.05f;
		logic_speed=1;
	}

	if(key==VK_F7)
	{
		logic_quant=0.1f;
		logic_speed=2;
	}
	if(key==VK_F8)
	{
		logic_quant=0.1f;
		logic_speed=3;
	}

	if(key=='T')
	{
		static int t=1;
		t++;
		pWater->SetTechnique(t%3);
	}
}


void UpdateRegionMap(int x1,int y1,int x2,int y2)
{
	if(terMapPoint) terMapPoint->UpdateMap(Vect2i(x1,y1),Vect2i(x2,y2));
}

int terEnergyBorderCheck(int x,int y)
{
	return 0;
}

Vect3f Demo3D::MousePos3D()
{
	Vect2f pos_in(mouse_pos.x/(float)terRenderDevice->GetSizeX()-0.5f,
		mouse_pos.y/(float)terRenderDevice->GetSizeY()-0.5f);

	Vect3f pos,dir;
	gb_Camera->GetWorldRay(pos_in,pos,dir);
	Vect3f trace;
	bool b=terScene->Trace(pos,(pos+dir*2000),&trace);
	pos=trace;
	return pos;
}

void Demo3D::DrawMousePos()
{
	Vect2f bmin,bmax;
	terRenderDevice->OutTextRect(0,0,"A",-1,bmin,bmax);
	int dy=bmax.y+1;

	Vect3f pos=MousePos3D();
	cCamera* pCamera=gb_Camera;

	terRenderDevice->SetDrawTransform(pCamera);
	Vect3f I=pCamera->GetWorldI()*10.0f,J=pCamera->GetWorldJ()*10.0f;

	char str[256];
	sprintf(str,"%.2f,%.2f,%.2f",pos.x,pos.y,pos.z);
	int y=100;
	terRenderDevice->OutText(20,y,str,sColor4f(1,1,0,1));
	y+=dy;

		float div=1<<cWater::grid_shift;
		Vect2f vel;
		int xx=round(pos.x/div);
		int yy=round(pos.y/div);
		if(xx<0)xx=0;
		if(yy<0)yy=0;
		pWater->GetVelocity(vel,xx,yy);
		char* type="empty";
		cWater::OnePoint& cur=pWater->Get(xx,yy);
		float z=cur.SZ()/float(1<<16);
		if(cur.type==cWater::TYPE::type_filled)
			type="filled";
		if(cur.type==cWater::TYPE::type_border)
			type="border";
		sprintf(str,"pos=%i,%i,%f,vel=%.2f,%.2f %s",xx,yy,z,vel.x,vel.y,type);
		terRenderDevice->OutText(20,y,str,sColor4f(1,1,0,1));
		y+=dy;

	terRenderDevice->DrawLine(pos-I,pos+I,sColor4c(255,255,255,255));
	terRenderDevice->DrawLine(pos-J,pos+J,sColor4c(255,255,255,255));
	terRenderDevice->FlushPrimitive3D();

	if(sphere)
	{
		pos.x=xx*16;
		pos.y=yy*16;
		if(pWater)
		{
			pos.z=z;//pWater->GetZ(pos.x,pos.y);
		}

		sphere->SetScale(10);
		MatXf matr=MatXf::ID;
		matr.trans()=pos;

		sphere->SetPosition(matr);
	}
}

void TestSaveVRML(const char* filename)
{
	FILE* f=fopen("out.wrl","wt");
fprintf(f,"#VRML V2.0 utf8\n");

vector<Vect3f> point;
vector<sPolygon> polygon;
//obj->GetAllTriangle(point,polygon);
GetAllTriangle(filename,point,polygon);


fprintf(f,
"DEF Box01 Transform {\n"
"  translation 2.415 -9.662 0\n"
"  rotation -1 0 0 -1.571\n"
"  children [\n"
"    Shape {\n"
"      appearance Appearance {\n"
"        material Material {\n"
"          diffuseColor 0.5686 0.1098 0.6941\n"
"        }\n"
"      }\n"
"      geometry DEF Box01-FACES IndexedFaceSet {\n"
"        ccw FALSE\n"
"        solid TRUE\n"
"        coord DEF Box01-COORD Coordinate { point [\n"
);
	for(int i=0;i<point.size();i++)
	{
		Vect3f p=point[i];
		if(i<point.size()-1)
			fprintf(f,"%f %f %f,\n",p.x,p.y,p.z);
		else
			fprintf(f,"%f %f %f\n",p.x,p.y,p.z);
	}
//          -26.09 0 17.87, 26.09 0 17.87, -26.09 0 -17.87, 26.09 0 -17.87, 
//          -26.09 45.41 17.87, 26.09 45.41 17.87, -26.09 45.41 -17.87, 
//          26.09 45.41 -17.87
fprintf(f,
"		  ]\n"
"        }\n"
"        coordIndex [\n"
);
	for(i=0;i<polygon.size();i++)
	{
		sPolygon& p=polygon[i];
		fprintf(f,"%i,%i,%i,-1",p.p1,p.p2,p.p3);
		if(i<point.size()-1)
			fprintf(f,",\n");
		else
			fprintf(f,"\n");
	}
//          0, 2, 3, -1, 3, 1, 0, -1, 4, 5, 7, -1, 7, 6, 4, -1, 0, 1, 5, -1, 
//          5, 4, 0, -1, 1, 3, 7, -1, 7, 5, 1, -1, 3, 2, 6, -1, 
//          6, 7, 3, -1, 2, 0, 4, -1, 4, 6, 2, -1
fprintf(f,
"		  ]\n"
"        }\n"
"    }\n"
"  ]\n"
"}\n");
	fclose(f);
}

void DrawSpecialString(int x,int y,vector<string>& data,int choose)
{
	for(int i=0;i<data.size();i++)
	{
		const char* str=data[i].c_str();
		terRenderDevice->OutText(x,y,str,i==choose?sColor4f(1,1,1,1):sColor4f(1,1,0,1));
		x+=terRenderDevice->GetFontLength(str);
	}
}

void Demo3D::ShowHelp()
{
	Vect2f bmin,bmax;
	terRenderDevice->OutTextRect(0,0,"A",-1,bmin,bmax);
	int dy=bmax.y+1;
	int y=dy;

	terRenderDevice->OutText(10,y,"K-D Lab Water demo (September 2004)",sColor4f(1,1,1,1));
	y+=dy;
	if(!show_help)
	{
		terRenderDevice->OutText(10,y,"F1 - show help",sColor4f(1,1,0,1));
		return;
	}
	char str[256];

	vector<string> data;
	data.push_back("1 - hot climate, ");
	data.push_back("2 - temperate climate");
	DrawSpecialString(10,y,data,climate);
	y+=dy;
	sprintf(str,"ADD,SUBSTRACT - ocean level=%i",enviroment_water);
	terRenderDevice->OutText(10,y,str,sColor4f(1,1,0,1));
	y+=dy;
	terRenderDevice->OutText(10,y,"INSERT, DELETE - water supply source",sColor4f(1,1,0,1));
	y+=dy;
	terRenderDevice->OutText(10,y,"Left, right mouse button - up, down ground",sColor4f(1,1,0,1));
	y+=dy;
	data.clear();
	data.push_back("SPEED ");
	data.push_back("F5=1x, ");
	data.push_back("F6=2x, ");
	data.push_back("F7=unlimit, ");
	data.push_back("F8=pause");
	DrawSpecialString(10,y,data,logic_speed+1);
	y+=dy;
	terRenderDevice->OutText(10,y,"Z(ADD), X(DELETE) - swim bubble",sColor4f(1,1,0,1));
	y+=dy;
	terRenderDevice->OutText(10,y,"A - add object",sColor4f(1,1,0,1));
	y+=dy;
	terRenderDevice->OutText(10,y,"Q - wind, E - tornado",sColor4f(1,1,0,1));
	y+=dy;
}