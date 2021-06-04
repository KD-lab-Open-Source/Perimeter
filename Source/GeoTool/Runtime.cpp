#include <stdafx.h>
#include <Clear3D.hpp>
#include <Texture3DFactory.hpp>

#include "Runtime.h"
#include "ColorMapManager.h"
#include "VMapOperator.h"
#include "TerraInterface.inl"

// terMapPoint is static because of Render and VisGeneric dependencies from Game
cIUnkClass* terMapPoint = NULL;

Runtime::Runtime(InputHandler* inputHandler) : inputHandler(inputHandler) {
	colorMapManager = new ColorMapManager();
	geoToolTerraInterface = new GeoToolTerraInterface(colorMapManager);
	vMapOperator = new VMapOperator(this);
	log = "Test";
	window = 0;
	mapLoaded = false;

	deltaMouseWheelPosition = 0;

	inputEnabled = true;

	gb_IVisGeneric = 0;
	terScene = 0;
	gb_Camera = 0;
	gb_IRenderDevice = 0;
	gb_ULight1 = 0;
	terMapPoint = NULL;
	pFont=NULL;

	prevtime=0;

//	vPosition.set(1994.11f,852.744f,390.963f);
//	AnglePosition.set(0.225019f,7.9876f);
//	distance = 1024.0f;

	vPosition.set(1024.0f, 1024.0f, 0.0f);
	AnglePosition.set(0.001f, 0.001f);
	distance = 1024.0f;
}

Runtime::~Runtime() {
	geoToolTerraInterface->Release();
	delete colorMapManager;
	delete vMapOperator;
	doneRenderDevice();
}

cScene* Runtime::getScene() {
	return terScene;
}

bool Runtime::isInputEnabled() {
	return inputEnabled;
}

void Runtime::setInputEnabled(bool enabled) {
	inputEnabled = enabled;
}

void Runtime::updateRect(const Rect4i& rect) {
	colorMapManager->updateRect(rect);
	updateTileMapRect(rect.xMin(), rect.yMin(), rect.xMax() - 1, rect.yMax() - 1);
}

void Runtime::updateRect(int xMin, int yMin, int xMax, int yMax) {
	colorMapManager->updateRect(xMin, yMin, xMax, yMax);
	updateTileMapRect(xMin, yMin, xMax - 1, yMax - 1);
}

VMapOperator* Runtime::getVMapOperator() {
	return mapLoaded ? vMapOperator : 0;
}

ColorMapManager* Runtime::getColorMapManager() {
	return mapLoaded ? colorMapManager : 0;
}

void Runtime::test() {
}

void Runtime::test(Vect3f* v) {
	Rect4i rect(v->x - 16, v->y - 16, v->x + 16, v->y + 16);
	rect.intersect(Rect4i(0, 0, vMap.H_SIZE - 1, vMap.V_SIZE - 1));
	if (rect.getWidth() > 0 && rect.getHeight() > 0) {
		for (int x = rect.xMin(); x < rect.xMax(); x++) {
			for (int y = rect.yMin(); y < rect.yMax(); y++) {
				int offset = y * vMap.H_SIZE + x;
//				vMap.VxGBuf[offset] -= 10;
				vMap.VxDBuf[offset] = 0;
			}
		}
		vMap.regRender(rect.xMin(), rect.yMin(), rect.xMax(), rect.yMax());
		updateRect(rect);
	}
}

void Runtime::shiftExactColorMap(Vect3f* v) {
	Rect4i exactRect = colorMapManager->getExactGeoColorMapRect();
	colorMapManager->shiftExactGeoColorMapRect(
									v->x - exactRect.xMin() - exactRect.getWidth() / 2,
									v->y - exactRect.yMin() - exactRect.getHeight() / 2	);
	updateTileMapRect(exactRect);
	updateTileMapRect( colorMapManager->getExactGeoColorMapRect() );
}

void Runtime::mouseWheelPositionChanged(short zDelta) {
	deltaMouseWheelPosition += zDelta;
}

void Runtime::handleControls(float dt) {
	if (inputEnabled) {

		if (mapLoaded) {
			if ( GetAsyncKeyState(VK_SPACE) < 0 ) {
				geoToolTerraInterface->showDamLayer = !geoToolTerraInterface->showDamLayer;
				updateTileMapRect( Rect4i(0, 0, vMap.H_SIZE - 1, vMap.V_SIZE - 1) );
			}

			inputHandler->handleInput(this, dt);
		}


		float fElapsedTime = dt / 1000.0f;
		float fSpeed = 1000.0f * fElapsedTime;

		if (deltaMouseWheelPosition != 0.0f) {
			distance += deltaMouseWheelPosition;
			deltaMouseWheelPosition = 0.0f;
		}

		Vect2f currMousePosition;
		if ( getViewportPos(&currMousePosition) ) {
			if ( GetAsyncKeyState(VK_MBUTTON) < 0 ) {
				AnglePosition.y += 
					(lastMousePosition.y - currMousePosition.y) * 2.5f;
				AnglePosition.x +=
					(currMousePosition.x - lastMousePosition.x) * 2.5f;
			}
			lastMousePosition = currMousePosition;
/*
			if (currMousePosition.x < -0.45f) {
				vPosition -= gb_Camera->GetWorldI() * fSpeed;
			}
			if (currMousePosition.x > 0.45f) {
				vPosition += gb_Camera->GetWorldI() * fSpeed;
			}
			if (currMousePosition.y < -0.45f) {
				Vect3f kz = gb_Camera->GetWorldK();
				kz.z = 0;
				kz.normalize();
				vPosition += kz * fSpeed;
			}
			if (currMousePosition.y > 0.45f) {
				Vect3f kz = gb_Camera->GetWorldK();
				kz.z = 0;
				kz.normalize();
				vPosition -= kz * fSpeed;
			}
*/
		}

		if ( GetAsyncKeyState(VK_MENU) < 0 ) {

			float fAngularSpeed = 1.0f * fElapsedTime;

			if( GetAsyncKeyState(VK_UP) < 0 ) {
				AnglePosition.y -= fAngularSpeed;
			}
			if( GetAsyncKeyState(VK_DOWN) < 0 ) {
				AnglePosition.y += fAngularSpeed;
			}
			if( GetAsyncKeyState(VK_LEFT) < 0 ) {
				AnglePosition.x -= fAngularSpeed;
			}
			if( GetAsyncKeyState(VK_RIGHT) < 0 ) {
				AnglePosition.x += fAngularSpeed;
			}

		} else {

			if( GetAsyncKeyState(VK_RIGHT) < 0 ) {
				vPosition += gb_Camera->GetWorldI() * fSpeed;
			}
			if( GetAsyncKeyState(VK_LEFT) < 0 ) {
				vPosition -= gb_Camera->GetWorldI() * fSpeed;
			}
			if( GetAsyncKeyState(VK_UP) < 0 ) {
				Vect3f kz = gb_Camera->GetWorldK();
				kz.z = 0;
				kz.normalize();
				vPosition += kz * fSpeed;
			}
			if( GetAsyncKeyState(VK_DOWN) < 0 ) {
				Vect3f kz = gb_Camera->GetWorldK();
				kz.z = 0;
				kz.normalize();
				vPosition -= kz * fSpeed;
			}
			if( GetAsyncKeyState(VK_SUBTRACT) < 0 )	{
				distance += fSpeed;
			}
			if( GetAsyncKeyState(VK_ADD) < 0 ) {
				distance -= fSpeed;
			}
		}

	} else {
		inputHandler->inputDisabled(this);
	}
}

void Runtime::loadMap(int number) {

	if (!vMap.wTable) {
		VMapOperator::prepareVMap();
	}

	VMapOperator::loadMap(number);
	colorMapManager->vMapLoaded();
	colorMapManager->setTexture3D(tx3d::Texture3DFactory::createTexture3D("<texture type='Clear'/>"));
	if (!mapLoaded) {
		mapLoaded = true;
	}
	createTileMap();
}

void Runtime::setTexture(tx3d::Texture3D* tx) {
	if (tx) {
		if (mapLoaded) {
			double curtime = clockf();

			colorMapManager->setTexture3D(tx);

			curtime = clockf() - curtime;
			char intBuffer[30 + 1];
			sprintf(intBuffer, "%f", curtime);
			log = string(intBuffer);
			updateTileMapRect(Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
//			updateRect(Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
			return;
		} else {
			tx->referenced();
			tx->released();
		}
	} else {
		if (mapLoaded) {
			colorMapManager->setTexture3D(tx3d::Texture3DFactory::createTexture3D("<texture type='Clear'/>"));
			updateTileMapRect(Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
//			updateRect(Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
		}
	}
}

void Runtime::setIndexingParams(tx3d::Vector3D* pal, unsigned char* indexLattice) {
	colorMapManager->setIndexingParams(pal, indexLattice);
}

bool Runtime::cursorTrace(Vect3f* vMapPoint) {

	Vect2f pos;
	if ( !getViewportPos(&pos) ) {
		return false;
	}

	Vect3f p0 = gb_Camera->GetPos();
	Vect3f p1;
	gb_Camera->ConvertorCameraToWorld( &p1, &pos );

	Vect3f dir = p1 - p0;
	float m = 100000.0f / dir.norm();
	dir *= m;
	p1 = p0 + dir;	

	return terScene->Trace(p0, p1, vMapPoint);
}

Vect3f Runtime::getDirection(Vect3f& cameraPos, Vect2f& cursor) {
	Vect3f res;
	gb_Camera->ConvertorCameraToWorld( &res, &cursor );
	res -= cameraPos;
	float m = 100000.0f / res.norm();
	res *= m;
	return res;
}

bool Runtime::getViewportPos(Vect2f* pos) {
	POINT Point;
	GetCursorPos(&Point);
	window->ScreenToClient(&Point);
	if (Point.x < 0 || Point.y < 0) {
		return false;
	}
	pos->x = ( (float) Point.x ) / ( (float) windowRect.Width() ) - 0.5f;
	pos->y = ( (float) Point.y ) / ( (float) windowRect.Height() ) - 0.5f;
	return
		   pos->x >= -0.5f
		&& pos->y >= -0.5f
		&& pos->x <= 0.5f
		&& pos->y <= 0.5f;
}

bool Runtime::cursorTrace(Vect3f* p, Vect3f* dir) {
	Vect2f pos;
	if ( !getViewportPos(&pos) ) {
		return false;
	}
	*p = gb_Camera->GetPos();
	*dir = getDirection(*p, pos);
	return true;
}

bool Runtime::cursorTrace(Vect3f* p, Vect3f* dir, Vect3f* dir2) {
	Vect2f pos;
	if ( !getViewportPos(&pos) ) {
		return false;
	}
	*p = gb_Camera->GetPos();
	*dir = getDirection(*p, pos);
	pos.x = 1.0f;
	*dir2 = getDirection(*p, pos);
	return true;
}

void Runtime::updateTileMapRect(int x1, int y1, int x2, int y2) {
	if (terMapPoint && mapLoaded) {
		((cTileMap*)terMapPoint)->UpdateMap(Vect2i(x1, y1), Vect2i(x2, y2));
	}
}

void Runtime::updateTileMapRect(Rect4i& rect) {
	if (terMapPoint && mapLoaded) {
		((cTileMap*)terMapPoint)->UpdateMap(rect.min, rect.max);
	}
}

void Runtime::quant() {

	double curtime = clockf();
	float dt = min(100.0, curtime-prevtime);
	handleControls(dt);
	prevtime = curtime;

	cameraQuant(dt);
	vMap.renderQuant();
	draw();
}

void Runtime::draw() {
	terScene->PreDraw(gb_Camera);

	sColor4f Color(0.5f,0.5f,0,1.0f);
	gb_IRenderDevice->Fill(Color.GetR(),Color.GetG(),Color.GetB());
	gb_IRenderDevice->BeginScene();
//	gb_IRenderDevice->SetRenderState(RS_FILLMODE,FILL_WIREFRAME);

	gb_IRenderDevice->SetFont(pFont);

	terScene->Draw(gb_Camera);

	drawOutput(8,8);

	gb_IRenderDevice->SetFont(NULL);

	gb_IRenderDevice->EndScene();
	gb_IRenderDevice->Flush();
}

void Runtime::drawOutput(int x, int y) {
	gb_IRenderDevice->OutText(x, y, log.c_str(), sColor4f(1,1,0,1));
}

void Runtime::cameraQuant(float dt) {
	Vect3f position;
	position.setSpherical(AnglePosition.x, AnglePosition.y, distance);
	position += vPosition;
	MatXf matrix_ = MatXf::ID;
	matrix_.rot() = Mat3f(AnglePosition.y, X_AXIS) * Mat3f(M_PI/2 - AnglePosition.x, Z_AXIS);
	matrix_ *= MatXf(Mat3f::ID, -position);	
	setCameraPosition(gb_Camera, matrix_);
}

void Runtime::setup() {
	gb_IVisGeneric->SetMapLevel(100);
//	gb_IVisGeneric->SetFarDistanceLOD(43);
//	gb_IVisGeneric->SetNearDistanceLOD(0);
//	gb_IVisGeneric->SetMipMapBlur(0);
//	gb_IVisGeneric->SetMipMapLevel(4);

//	gb_IVisGeneric->SetDrawMeshShadow(0);
//	gb_IVisGeneric->SetShadowType(SHADOW_MAP_SELF);
	gb_IVisGeneric->SetShadowType(SHADOW_NONE, 0);
}

void Runtime::createTileMap() {
	if (mapLoaded) {
		RELEASE(terMapPoint);
//		terMapPoint = terScene->CreateMap(Vect2i((int)vMap.H_SIZE, (int)vMap.V_SIZE), 0.7f, 1);
		geoToolTerraInterface->IncRef();
		terMapPoint = terScene->CreateMap(geoToolTerraInterface, 0);
	}
}

int Runtime::windowResized(CWnd *wnd) {
	if(gb_IRenderDevice) {
		wnd->GetClientRect(windowRect);
		gb_IRenderDevice->ChangeSize(
			windowRect.Width(),
			windowRect.Height(),
			RENDERDEVICE_MODE_WINDOW);
		gb_Camera->Update();
		return 0;
	} else {
		return initRenderDevice(wnd);
	}
}

int Runtime::initRenderDevice(CWnd *wnd) {
	window = wnd;
	wnd->GetClientRect(windowRect);

	gb_IVisGeneric = CreateIVisGeneric();
	gb_IRenderDevice = CreateIRenderDevice();
	if ( gb_IRenderDevice->Init(
							windowRect.Width(),
							windowRect.Height(),
							RENDERDEVICE_MODE_WINDOW | RENDERDEVICE_MODE_Z24,
							wnd->m_hWnd) ) {
		return 1;
	}
	gb_IVisGeneric->SetData(gb_IRenderDevice);
	terScene = gb_IVisGeneric->CreateScene(); 
	terScene->DisableTileMapVisibleTest();

	gb_Camera = terScene->CreateCamera();
	gb_Camera->SetAttr(ATTRCAMERA_PERSPECTIVE);
	MatXf cameraMatrix;
	Identity(cameraMatrix);
	Vect3f cameraPos(0, 0, -512);
	SetPosition(cameraMatrix, cameraPos, Vect3f(0, 0, 0));
	setCameraPosition(gb_Camera, cameraMatrix);

	gb_Camera->SetFrustum(							// устанавливается пирамида видимости
		&Vect2f(0.5f, 0.5f),						// центр камеры
		&sRectangle4f(-0.5f, -0.5f, 0.5f, 0.5f),	// видимая область камеры
		&Vect2f(1.f, 1.f),							// фокус камеры
		&Vect2f(10.0f, 9000.0f)
		);

	// создается источник света, иначе кромешная тьма и объектов не видно
	gb_ULight1=terScene->CreateLight(ATTRLIGHT_DIRECTION);
	gb_ULight1->SetPosition( MatXf(Mat3f::ID, Vect3f(0, 0, 0)) );

	gb_ULight1->SetColor( &sColor4f(0, 0, 0, 1), &sColor4f(1, 1, 1, 1) );

	gb_ULight1->SetDirection( Vect3f(-1,0,-1) );

	gb_IVisGeneric->SetFontRootDirectory("resource\\LocData");
	string dir = "resource\\LocData\\English\\Fonts";
	gb_IVisGeneric->SetFontDirectory(dir.c_str());

	pFont = gb_IVisGeneric->CreateFont("Arial", 20);

	setup();
	createTileMap();
	return 0;
}

void Runtime::doneRenderDevice() {
	RELEASE(pFont);
	window = 0;
	RELEASE(terMapPoint);
	RELEASE(gb_Camera);
	RELEASE(gb_ULight1);
	RELEASE(terScene);
	if (gb_IVisGeneric) {
		gb_IVisGeneric->ClearData();
	}
	RELEASE(gb_IVisGeneric);
	RELEASE(gb_IRenderDevice);
}

int Runtime::reInitRenderDevice(CWnd *wnd) {
	doneRenderDevice();
	return initRenderDevice(wnd);
}

void Runtime::setCameraPosition(cCamera *uCamera,const MatXf &matrix) {
	MatXf ml= MatXf::ID;
	ml.rot()[2][2] = -1;

	MatXf mr = MatXf::ID;
	mr.rot()[1][1] = -1;
	MatXf cameraMatrix;
	cameraMatrix = mr * ml * matrix;

	uCamera->SetPosition(cameraMatrix);
}

// method copies are here because of Render and VisGeneric dependencies from Game
void UpdateRegionMap(int x1, int y1, int x2, int y2) {
	if (terMapPoint) {
		((cTileMap*)terMapPoint)->UpdateMap(Vect2i(x1, y1), Vect2i(x2, y2));
//		terMapPoint->UpdateMap(Vect2i(x1, y1), Vect2i(x2, y2));
	}
}

void terExternalShowCall(int type, void *pointer, int size) {
}

int terEnergyBorderCheck(int x, int y) {
	return 0;
}
// end of method copies

/*
	POINT mouse_pos;
	GetCursorPos(&mouse_pos);
	window->ScreenToClient(&mouse_pos);
	Vect2f pos_in(mouse_pos.x/(float)gb_IRenderDevice->GetSizeX()-0.5f,
		mouse_pos.y/(float)gb_IRenderDevice->GetSizeY()-0.5f);

	cCamera* pCamera=gb_Camera;
	gb_IRenderDevice->SetDrawTransform(pCamera);
	Vect3f pos,dir;
	pCamera->GetWorldRay(pos_in,pos,dir);
	//pos+=dir*512;

	Vect3f trace;
	cIUnkClass* b=terScene->Trace(&pos,&(pos+dir*2000),&trace);

	pos=trace;

	Vect3f I=pCamera->GetWorldI()*10.0f,J=pCamera->GetWorldJ()*10.0f;

	char str[256];
	sprintf(str,"%f,%f,%f",pos.x,pos.y,pos.z);
	gb_IRenderDevice->OutText(20,100,str,&sColor4f(1,1,0,1));



	gb_IRenderDevice->DrawLine(pos-I,pos+I,255,255,255,255);
	gb_IRenderDevice->DrawLine(pos-J,pos+J,255,255,255,255);
	gb_IRenderDevice->FlushLine3D();

*/

/*
	char str[200];
	sprintf(str, "v.x = %.6f, v.y = %.6f, v.z = %.6f", v->x, v->y, v->z);
	log += string(str);
*/

//	vMap.SetTer(offset, vMap.GetGeoType(offset,0));		

/*
	vMap.prepare("RESOURCE\\WORLDS\\worlds.prm",NULL,NULL,0,0,0);
	vMap.selectUsedWorld(0);

	ifstream ifsTx(GetTargetName("geoTx.xml"));
	char szBuffer[1024 * 10];
	ifsTx.get(szBuffer, 1024 * 10, '\0');

	tx3d::Texture3D* pTx = tx3d::Texture3DFactory::createTexture3D(string(szBuffer));
	if (!pTx) {
		pTx = tx3d::Texture3DFactory::createTexture3D("<texture type='Clear'/>");
	}
	SlotManager slotManager;
	pTx->referenced();
	if (slotManager.parseTexture(pTx)) {
		log = "OK";
		if ( slotManager.assembleTexture() ) {
			log += "OK";
				ofstream ofs;

				ofs.open(GetTargetName("geoTxNew.xml"));
				string strLUT = tx3d::XMLUtils::serializableToTag("texture", *pTx);
				tx3d::XMLUtils::indentXML(strLUT);
				ofs << strLUT.c_str();
				ofs.close();
		}
	}
	pTx->released();
*/
