
#pragma once

enum CAMERA_PLANE
{
	CAMERA_PLANE_X,
	CAMERA_PLANE_Y,
	CAMERA_PLANE_Z,
};

struct CEmitterData;

class CD3DCamera
{
	float   m_cameraPsi;
	float   m_cameraTheta;
	float   m_cameraR;
	Vect3f  m_cameraFocus;

	CRect   m_rcCamera;

public:
	cCamera* m_pCamera;

	bool    m_bVisible;
	bool    m_bLocked;
	bool    m_bMain;

	CAMERA_PLANE m_plane;

	CD3DCamera(float psi = M_PI/2, float theta = M_PI/3, float r = 512);
	~CD3DCamera();

	void SaveCameraSettings();
	void LoadCameraSettings();
	void Resize(const CRect& rcWindow, const CRect& rcCamera, bool bPerspective );
	void Init(cScene* pScene, cInterfaceRenderDevice* pRenderDevice, 
		const CRect& rcWindow, const CRect& rcCamera, CAMERA_PLANE plane, bool bPerspective = true);
	void Done();

	void ViewPos(const Vect3f& np);
	void Rotate(float dPsi, float dTheta);
	void Move(float dx, float dy, float dz);
	void Zoom(float delta);
	void Quant();

	void GetBilboardMatrix(Mat3f& mat);//матрица для отображения всегда лицом к камере

	void DrawCameraRect(cInterfaceRenderDevice* pRenderDevice, bool bActive);

	bool HitTest(const CPoint& pt);

	void Camera2World(Vect3f& v_to, float x, float y, Vect3f* pvObjDist);
	void GetCameraRay(Vect3f& vRay, Vect3f& vOrg, float x, float y);

	friend class CD3DScene;
};

typedef  TAutoReleaseContainer< vector<CD3DCamera*> >  CameraListType;

//////////////////////////////////////////////////////////////////////////////////////////////////
//
class CEffectToolDoc;

class CD3DScene  
{
public:

	float m_particle_rate;

	cObjectNodeRoot*        m_pModel;
	cObjectNodeRoot*        m_pBackModel;
	CD3DCamera*             m_pActiveCamera;
	cScene*                 m_pScene;

	bool					m_bCameraMode;

	cIUnkClass*				terMapPoint;

private:
	cInterfaceRenderDevice* m_pRenderDevice;
	cVisGeneric*            m_pVisGeneric;
	cUnkLight*              m_pLight;
	cEffect*                m_pEffectObj;
	CameraListType          m_cameras;

	KeyPos*                 m_pWayPointActive;

	CEffectToolDoc*         m_pDoc;

	void DrawGrid();
	void DrawInfo(CD3DCamera* pCamera);
public:
	void LoadWorld(LPCTSTR filename);

	enum  ToolMode
	{
		TOOL_NONE,
		TOOL_POSITION,
		TOOL_DIRECTION,
		TOOL_DIRECTION_BS,
		TOOL_SPLINE,
	};

	static ToolMode m_ToolMode;

	static bool   bPause;
	static bool   bShowEmitterBox;
	static bool   bShowGrid;
	static bool	  bShowWorld;

	CD3DScene();
	~CD3DScene();

	void DoneRenderDevice();
	bool InitRenderDevice(CWnd* pWnd);
	void InitEmitters();
	void Resize(CRect& rcWnd);
	void SetMultiviewMode(bool bMulti);
	
	void CameraDefault();
	void SetActiveCamera(const CPoint& pt);
	void CameraRotate(float dPsi, float dTheta);
	void CameraMove(float dx, float dy, float dz);
	void CameraZoom(float delta);

	CAMERA_PLANE GetCameraPlane();
	const CRect& GetActiveCameraRect();
	void Camera2World(Vect3f& v_to, float x, float y, Vect3f* pvObjDist = 0);
	void GetCameraRay(Vect3f& vRay, Vect3f& vOrg, float x, float y);

	void SetActiveKeyPos(int i);
	void SetActiveWayPoint(int nPoint);
	KeyPos* GetActiveWayPoint();
	KeyPos* HitTestWayPoint(const Vect3f& v);

	cTexture* CreateTexture(LPCSTR lpszName);
	bool LoadModelInsideEmitter(LPCTSTR lpszModelName);
	void LoadParticleModel(LPCTSTR lpszModelName, int mode = 0);

	cEffect* GetEffect(){
		return m_pEffectObj;
	}
	void SetDocument(CEffectToolDoc* pDoc){
		m_pDoc = pDoc;
	}

	void Quant();

	void LoadSceneSettings();
	void SaveSceneSettings();

	Vect3f& GetCenter();
	int GetTriangleCount();
	double GetSquareTriangle();
protected:
	FPS fps;
	void QuantFps();

};
