#ifndef _RUNTIME_H
#define _RUNTIME_H

#include "InputHandler.h"
#include "Operator.h"
#include "Updater.h"
#include "VMapOperator.h"
#include "GeoToolTerraInterface.h"

class Runtime : public Updater, Operator {

	public :

		Runtime(InputHandler* inputHandler);
		~Runtime();

		cScene* getScene();
		bool isInputEnabled();
		void setInputEnabled(bool enabled);

		void updateRect(const Rect4i& rect);
		void updateRect(int xMin, int yMin, int xMax, int yMax);

		VMapOperator* getVMapOperator();
		ColorMapManager* getColorMapManager();

		void quant();
		void loadMap(int number);
		int windowResized(CWnd *wnd);
		int initRenderDevice(CWnd *wnd);
		void doneRenderDevice();
		int reInitRenderDevice(CWnd *wnd);

		void setTexture(tx3d::Texture3D* tx);
		void setIndexingParams(tx3d::Vector3D* pal, unsigned char* indexLattice);
		void toggleShowIndexed() {
			colorMapManager->toggleShowIndexed();
			updateTileMapRect(Rect4i(0, 0, vMap.H_SIZE, vMap.V_SIZE));
		}

		bool cursorTrace(Vect3f* p, Vect3f* dir);
		bool cursorTrace(Vect3f* vMapPoint);
		bool cursorTrace(Vect3f* p, Vect3f* dir, Vect3f* dir2);
		bool getViewportPos(Vect2f* pos);

		void shiftExactColorMap(Vect3f* v);

		void mouseWheelPositionChanged(short zDelta);
		
		string log;

	protected:

		void createTileMap();
		void draw();
		void handleControls(float dt);
		void cameraQuant(float dt);
		void setup();
		void test();
		void test(Vect3f* v);
		void updateTileMapRect(Rect4i& rect);
		void updateTileMapRect(int x1, int y1, int x2, int y2);

		Vect3f getDirection(Vect3f& cameraPos, Vect2f& cursor);
		void drawOutput(int x, int y);

		InputHandler* inputHandler;
		VMapOperator* vMapOperator;
		CWnd *window;
		ColorMapManager* colorMapManager;
		GeoToolTerraInterface* geoToolTerraInterface;
		CRect windowRect;

		cVisGeneric				*gb_IVisGeneric;
		cScene					*terScene;
		cCamera					*gb_Camera;
		cInterfaceRenderDevice	*gb_IRenderDevice;
		cUnkLight				*gb_ULight1;
		cFont					*pFont;

		double prevtime;
		Vect3f vPosition;
		Vect2f AnglePosition;
		float distance;

		Vect2f lastMousePosition;
		short deltaMouseWheelPosition;

		bool mapLoaded;

		bool inputEnabled;

		static void setCameraPosition(cCamera *uCamera,const MatXf &matrix);
};

#endif // _RUNTIME_H