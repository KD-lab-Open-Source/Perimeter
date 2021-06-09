// D3DWindow.cpp : implementation file
//

#include "stdafx.h"
#include "EffectTool.h"
#include "D3DWindow.h"
#include "ControlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CControlView* ctrv;

#define IS_SHIFT() (GetAsyncKeyState(VK_SHIFT) & 0x8000)
#define IS_CTRL() (GetAsyncKeyState(VK_CONTROL) & 0x8000)
#define IS_RBUT() (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
float SqrDistRayPoint(const Vect3f& vPoint, const Vect3f& vRay, const Vect3f& vRayOrg)
{
    Vect3f kDiff = vPoint - vRayOrg;
    float fT = kDiff.dot(vRay);

    if (fT <= 0.0f)
    {
        fT = 0.0f;
    }
    else
    {
        fT /= vRay.norm2();
        kDiff -= fT*vRay;
    }

    return kDiff.norm2();
}

KeyPos* HitTestSplinePoint(CKeyPosHermit& spline, float x, float y, const MatXf& offs)
{
	const float _eps = 5.0f;

	Vect3f vRay, vOrg;
	theApp.scene.GetCameraRay(vRay, vOrg, x, y);

	CKeyPosHermit::iterator it;
	FOR_EACH(spline, it)
	{
		Vect3f abs_pos = offs*it->pos+theApp.scene.GetCenter();
		if(SqrDistRayPoint(abs_pos, vRay, vOrg) < _eps)
			return &*it;
	}

	return 0;
}
void RemoveSplinePoint(CKeyPosHermit& spline, KeyPos *p)
{
	CKeyPosHermit::iterator it;
	FOR_EACH(spline, it)
		if(&*it == p)
		{
			spline.erase(it);
			break;
		}
}
KeyPos* AddSplinePoint(CKeyPosHermit& spline)
{
	spline.push_back(KeyPos());
	spline.back().pos.set(0, 0, 0);

	return &spline.back();
}
/////////////////////////////////////////////////////////////////////////////
// CD3DWindow

IMPLEMENT_DYNCREATE(CD3DWindow, CView)

CD3DWindow::CD3DWindow()
{
	m_hCursorHand = theApp.LoadCursor(IDC_CURSOR_MOVE);
	m_ptPrevPoint = CPoint(0, 0);
	m_bLeftPressed = false;
}
CD3DWindow::~CD3DWindow()
{
}

BOOL CD3DWindow::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.lpszClass = AfxRegisterWndClass(0);
	
	return CView::PreCreateWindow(cs);
}


void CD3DWindow::SetWaypointHover(const CPoint& pt)
{
/*	if(GetDocument()->m_pActiveEmitter)
	{
		CRect rc;
		GetClientRect(rc);

		Vect3f v;
		theApp.scene.Camera2World(v, float(pt.x)/rc.Width() - 0.5f, float(pt.y)/rc.Height() - 0.5f);
		theApp.scene.WayPointHover(v);
	}
*/
}
void CD3DWindow::PositionKeysRotate(float dPsi, float dTheta)
{
	CEmitterData* pEmitter = GetDocument()->m_pActiveEmitter;
	if(!pEmitter||!pEmitter->data)
		return;
	ASSERT(GetDocument()->m_nCurrentGenerationPoint<pEmitter->data->emitter_position.size());
	
	Vect3f center(0,0,0);
	CKeyPos& pks = pEmitter->data->emitter_position;
	CKeyPos::iterator it;
	FOR_EACH(pks,it)
		center+=it->pos;
	center/=pks.size();
	Mat3f mtr;
	switch(theApp.scene.GetCameraPlane())
	{
	case CAMERA_PLANE_X:
		mtr.set(dPsi,X_AXIS);
		break;
	case CAMERA_PLANE_Y:
		mtr.set(dPsi,Y_AXIS);
		break;
	case CAMERA_PLANE_Z:
		mtr.set(dPsi,Z_AXIS);
		break;
	}
	FOR_EACH(pks,it)
		it->pos = (it->pos-center)*mtr+center;
}
void CD3DWindow::SetAllPositionKey(const CPoint& pt,const CPoint& prev_pt)
{
	CEmitterData* pEmitter = GetDocument()->m_pActiveEmitter;
	if(!pEmitter||!pEmitter->data)
		return;
	CRect rc;
	GetClientRect(rc);
	CKeyPos::iterator it;
	Vect3f v1,v2;
	theApp.scene.Camera2World(v1, float(pt.x)/rc.Width() - 0.5f, float(pt.y)/rc.Height() - 0.5f);
	theApp.scene.Camera2World(v2, float(prev_pt.x)/rc.Width() - 0.5f, float(prev_pt.y)/rc.Height() - 0.5f);
	v1-=v2;
	FOR_EACH(pEmitter->data->emitter_position,it)
		it->pos+=v1;
}
KeyPos* CD3DWindow::HitPositionKey(const CPoint& pt)
{
	CEmitterData* pEmitter = GetDocument()->m_pActiveEmitter;
	if(!pEmitter || !pEmitter->data)
		return NULL;
	Vect3f fxc = GetDocument()->m_EffectCenter;
	Vect3f fx_center=theApp.scene.GetCenter();
	CRect rc;
	GetClientRect(rc);
	Vect3f v;
	Vect2f pt_sc(float(pt.x)/rc.Width() - 0.5f , float(pt.y)/rc.Height() - 0.5f);
	CKeyPos& emp = pEmitter->data->emitter_position;
	int i;
	int cam_plane = theApp.scene.GetCameraPlane();
	for(i = emp.size()-1;i>=0;--i)
	{
		Vect3f abs_pos = fx_center+emp[i].pos;
		theApp.scene.Camera2World(v, pt_sc.x, pt_sc.y,&abs_pos/*emp[i].pos*/);
		v -= fx_center;
		switch(cam_plane)
		{
		case CAMERA_PLANE_X:
			if (abs(emp[i].pos.y - v.y)<5 && abs(emp[i].pos.z - v.z)<5)
				goto finish;			 
			break;
		case CAMERA_PLANE_Y:
			if (abs(emp[i].pos.x - v.x)<5 && abs(emp[i].pos.z - v.z)<5)
				goto finish;			 
			break;
		case CAMERA_PLANE_Z:
			if (abs(emp[i].pos.x - v.x)<5 && abs(emp[i].pos.y - v.y)<5)
				goto finish;			 
			break;
		}
	}
	return NULL;
finish:
	GetDocument()->m_nCurrentGenerationPoint = i;
	ctrv->OnChangeActivePoint(0,0);//PostMessage(WM_ACTIVE_POINT,0,0);
	return &emp[i];
}
extern void ChangePosition(CEmitterData* emi, int ix);
void CD3DWindow::SetPositionKey(const CPoint& pt, bool bNoChangePos)
{
	CEmitterData* pEmitter = GetDocument()->m_pActiveEmitter;
	if(!pEmitter || !pEmitter->data)
		return;
	theApp.scene.SetActiveCamera(pt);
//	Vect3f fxc = GetDocument()->m_EffectCenter;
	Vect3f fx_center=theApp.scene.GetCenter();
	CRect rc;
	GetClientRect(rc);
	Vect3f v;
	Vect2f pt_sc(float(pt.x)/rc.Width() - 0.5f , float(pt.y)/rc.Height() - 0.5f);
	ASSERT(GetDocument()->m_nCurrentGenerationPoint<pEmitter->data->emitter_position.size());
	KeyPos* pos_key = &pEmitter->data->emitter_position[GetDocument()->m_nCurrentGenerationPoint];
	Vect3f abs_pos = fx_center+pos_key->pos;
	theApp.scene.Camera2World(v, pt_sc.x, pt_sc.y, &abs_pos);
	v-=fx_center;

	if(pos_key&&!bNoChangePos)
	{
		switch(theApp.scene.GetCameraPlane())
		{
		case CAMERA_PLANE_X:
			pos_key->pos.y = v.y; pos_key->pos.z = v.z;
			break;
		case CAMERA_PLANE_Y:
			pos_key->pos.x = v.x; pos_key->pos.z = v.z;
			break;
		case CAMERA_PLANE_Z:
			pos_key->pos.y = v.y; pos_key->pos.x = v.x;
			break;
		}
	}
//	ChangePosition(GetDocument()->m_pActiveEmitter, GetDocument()->m_nCurrentGenerationPoint);

}


Mat3f CD3DWindow::RotateAllKeyPos(CKeyPos& key_pos, float dPsi, float dTheta)
{
	if (key_pos.empty())
		return Mat3f::ID;
	Vect3f center(0,0,0);
	CKeyPos::iterator it;
//	FOR_EACH(key_pos, it)
//		center+=it->pos;
//	center/=key_pos.size();
	center = key_pos.front().pos; 
	Mat3f mtr;
	switch(theApp.scene.GetCameraPlane())
	{
	case CAMERA_PLANE_X:
		mtr.set(dPsi,X_AXIS);
		break;
	case CAMERA_PLANE_Y:
		mtr.set(dPsi,Y_AXIS);
		break;
	case CAMERA_PLANE_Z:
		mtr.set(dPsi,Z_AXIS);
		break;
	}
	FOR_EACH(key_pos, it)
		it->pos = (it->pos-center)*mtr+center;
	return mtr;
}

Vect3f CD3DWindow::SetAllKeyPos(CKeyPos& key_pos, const CPoint& pt, const CPoint& prev_pt)
{
	CRect rc;
	GetClientRect(rc);
	Vect3f v1,v2;
	theApp.scene.Camera2World(v1, float(pt.x)/rc.Width() - 0.5f, float(pt.y)/rc.Height() - 0.5f);
	theApp.scene.Camera2World(v2, float(prev_pt.x)/rc.Width() - 0.5f, float(prev_pt.y)/rc.Height() - 0.5f);
	v1-=v2;
	CKeyPos::iterator it;
	FOR_EACH(key_pos,it)
		it->pos+=v1;
	return v1;
}

KeyPos* CD3DWindow::HitTestPoint(CKeyPos& key_pos, const CPoint& pt, const MatXf* offs /*=NULL*/)
{
	const float _eps = 20.0f;

	CRect rc;
	GetClientRect(rc);
	Vect3f vRay, vOrg;
	theApp.scene.GetCameraRay(vRay, vOrg, float(pt.x)/rc.Width() - 0.5f, float(pt.y)/rc.Height() - 0.5f);

	CKeyPosHermit::iterator it;
	FOR_EACH(key_pos, it)
	{
		Vect3f abs_pos = ((offs) ? *offs*it->pos : it->pos) + theApp.scene.GetCenter();
		if(SqrDistRayPoint(abs_pos, vRay, vOrg) < _eps)
			return &*it;
	}
	return NULL;
}

void CD3DWindow::SetKeyPos(KeyPos* key_pos, const CPoint& pt, const MatXf* offs /*=NULL*/)
{
	if(key_pos == NULL)
		return;
	CRect rc;
	GetClientRect(rc);
	Vect3f v;
	Vect3f pos = offs ? *offs*key_pos->pos : key_pos->pos;
	Vect3f abs_pos = pos + theApp.scene.GetCenter();
	theApp.scene.Camera2World(v, float(pt.x)/rc.Width() - 0.5f, float(pt.y)/rc.Height() - 0.5f, &abs_pos);
	v-=theApp.scene.GetCenter();
	switch(theApp.scene.GetCameraPlane())
	{
	case CAMERA_PLANE_X:
		pos.y = v.y; pos.z = v.z;
		break;
	case CAMERA_PLANE_Y:
		pos.x = v.x; pos.z = v.z;
		break;
	case CAMERA_PLANE_Z:
		pos.y = v.y; pos.x = v.x;
		break;
	}
	if (offs)
		offs->invXformPoint(pos);
	key_pos->pos = pos;
}
/*
void CD3DWindow::SetSplineKey(const CPoint& pt)
{
	if(m_pSplinePointTrack == 0)
		return;

	MatXf _offs;
	GetDocument()->GetSplineMatrix(_offs);


	CRect rc;
	GetClientRect(rc);

	Vect3f v;
	Vect3f pos = _offs*m_pSplinePointTrack->pos;
	Vect3f abs_pos = pos + theApp.scene.GetCenter();
	theApp.scene.Camera2World(v, float(pt.x)/rc.Width() - 0.5f, float(pt.y)/rc.Height() - 0.5f, &abs_pos);
	v-=theApp.scene.GetCenter();
	switch(theApp.scene.GetCameraPlane())
	{
	case CAMERA_PLANE_X:
		pos.y = v.y; pos.z = v.z;
		break;
	case CAMERA_PLANE_Y:
		pos.x = v.x; pos.z = v.z;
		break;
	case CAMERA_PLANE_Z:
		pos.y = v.y; pos.x = v.x;
		break;
	}
	_offs.invXformPoint(pos);
	m_pSplinePointTrack->pos = pos;
}
*/
void RotateQuaternion(QuatF& q, const CPoint& pt_delta, CAMERA_PLANE plane)
{
	QuatF qf;
	const float mul=0.01f;

	if(plane == CAMERA_PLANE_X)
	{
		qf.mult(QuatF(pt_delta.x*mul,Vect3f(1,0,0)), q);
		q = qf;
	}
	else if(plane == CAMERA_PLANE_Y)
	{
		qf.mult(QuatF(pt_delta.x*mul,Vect3f(0,1,0)), q);
		q = qf;
	}
	else
	{	
		qf.mult(QuatF(-pt_delta.y*mul,Vect3f(1,0,0)), q);
		q = qf;
		qf.mult(QuatF(pt_delta.x*mul,Vect3f(0,1,0)), q);
		q = qf;
	}
}
void CD3DWindow::SetDirectionKey(const CPoint& pt_delta)
{
	CEmitterData* pEmitter = GetDocument()->m_pActiveEmitter;
	ASSERT(pEmitter);

	if(!theApp.scene.bPause)
		return;

	float fTime = pEmitter->GenerationPointGlobalTime(GetDocument()->m_nCurrentGenerationPoint);
	KeyRotate* r_key = pEmitter->data->GetOrCreateRotateKey(fTime, 0);

	if(r_key)
		RotateQuaternion(r_key->pos, pt_delta, theApp.scene.GetCameraPlane());
}
void CD3DWindow::SetBSDirectionKey(const CPoint& pt_delta)
{
	ASSERT(GetDocument()->m_pActiveEmitter);

	if(GetDocument()->m_pActiveEmitter->Class() == EMC_INTEGRAL || GetDocument()->m_pActiveEmitter->Class() == EMC_INTEGRAL_Z)
	{
		EffectBeginSpeed& bs = GetDocument()->m_pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint];

		RotateQuaternion(bs.rotation, pt_delta, theApp.scene.GetCameraPlane());
	}
}
void CD3DWindow::MoveToPositionTime(float tm)
{
	float fTime = theApp.scene.GetEffect()->GetTime();

	if(fabs(fTime - tm) < 0.0001)
		return;

	if(tm < fTime)
		theApp.scene.InitEmitters();	

	theApp.scene.GetEffect()->MoveToTime(tm);
}


BEGIN_MESSAGE_MAP(CD3DWindow, CView)
	//{{AFX_MSG_MAP(CD3DWindow)
//	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CD3DWindow message handlers

BOOL CD3DWindow::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

const float fMouseRotateScale = 0.01f;
const float fMouseMoveScale = 1.f;
#include "OptTree.h"
extern COptTree* tr;
void CD3DWindow::OnMouseMove(UINT nFlags, CPoint point) 
{
	CPoint ptDelta = point - m_ptPrevPoint;
	if(nFlags & MK_RBUTTON)
	{
		if(nFlags & MK_SHIFT)
		{
			CAMERA_PLANE cpl = theApp.scene.GetCameraPlane();
			if((cpl == CAMERA_PLANE_X) || (cpl == CAMERA_PLANE_Y))
				theApp.scene.CameraMove(ptDelta.x*fMouseMoveScale, 0, -ptDelta.y*fMouseMoveScale);
			else
				theApp.scene.CameraMove(ptDelta.x*fMouseMoveScale, ptDelta.y*fMouseMoveScale, 0);
//			theApp.scene.CameraMove(ptDelta.x*fMouseMoveScale, 0, -ptDelta.y*fMouseMoveScale);
//			theApp.scene.CameraMove(ptDelta.x, ptDelta.y, 0);
		}else if (!(nFlags & MK_CONTROL))
		{
			theApp.scene.CameraRotate(ptDelta.x*fMouseRotateScale, -ptDelta.y*fMouseRotateScale);
		}
	}
//	if(m_bLeftPressed)
	{
		CAMERA_PLANE cpl = theApp.scene.GetCameraPlane();

/*		if(IS_CTRL())
		{
			if((cpl == CAMERA_PLANE_X) || (cpl == CAMERA_PLANE_Y))
				theApp.scene.CameraMove(ptDelta.x*fMouseMoveScale, 0, -ptDelta.y*fMouseMoveScale);
			else
				theApp.scene.CameraMove(ptDelta.x*fMouseMoveScale, ptDelta.y*fMouseMoveScale, 0);
		}
		else
*/		{
			CEmitterData* emitter = GetDocument()->m_pActiveEmitter;
			if (emitter)
			{			
				CKeyPos& emit_pos =  emitter->emitter_pos();
			switch(theApp.scene.m_ToolMode)
			{
			case CD3DScene::TOOL_SPLINE: 
				if(emitter->Class() == EMC_SPLINE)
				{
					if (nFlags & MK_CONTROL)
					{
						if (nFlags & MK_LBUTTON)
						{
							emitter->data->sdat.GetData(1).mat.trans()+=
								SetAllKeyPos(emitter->GetSpl()->p_position,point,m_ptPrevPoint);
						}
						else if (nFlags & MK_RBUTTON)
						{
							emitter->data->sdat.GetData(1).mat.rot()*=RotateAllKeyPos(emitter->GetSpl()->p_position,
								ptDelta.x*fMouseRotateScale, -ptDelta.y*fMouseRotateScale);
						}
					}
					else if ((nFlags & MK_LBUTTON) && tr->EnableEditPos())
					{
						MatXf offs;
						GetDocument()->GetSplineMatrix(offs);
						ASSERT((UINT)GetDocument()->m_nCurrentParticlePoint<emitter->GetSpl()->p_position.size());
						SetKeyPos(&emitter->GetSpl()->p_position[GetDocument()->m_nCurrentParticlePoint], point, &offs);
					}
				}
				break;
			case CD3DScene::TOOL_POSITION:
				if(nFlags & MK_CONTROL)
				{
					if (nFlags & MK_RBUTTON)
					{
						Mat3f &r = (emitter->data)?emitter->data->sdat.GetData(0).mat.rot():emitter->data_light->sdat.GetData(0).mat.rot();
						r*=RotateAllKeyPos(emit_pos, ptDelta.x*fMouseRotateScale, -ptDelta.y*fMouseRotateScale);
					}
					else if (nFlags & MK_LBUTTON)
					{
						Vect3f &t = (emitter->data)?emitter->data->sdat.GetData(0).mat.trans():emitter->data_light->sdat.GetData(0).mat.trans();
						t+=SetAllKeyPos(emit_pos, point, m_ptPrevPoint);
					}
				}				
				else 
				{
					ASSERT((UINT)GetDocument()->m_nCurrentGenerationPoint<emit_pos.size());
					if ((nFlags & MK_LBUTTON)&& tr->EnableEditPos())
						SetKeyPos(&emit_pos[GetDocument()->m_nCurrentGenerationPoint], point);
				}
				break;

			case CD3DScene::TOOL_DIRECTION:
				if (nFlags & MK_LBUTTON)
					SetDirectionKey(ptDelta);
				break;

			case CD3DScene::TOOL_DIRECTION_BS:
				if (nFlags & MK_LBUTTON)
					SetBSDirectionKey(ptDelta);
				break;
			}
			}
		}
	}

	SetWaypointHover(point);

	m_ptPrevPoint = point;
}

void CD3DWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	theApp.scene.SetActiveCamera(point);

	m_ptPrevPoint = point;

	if(nFlags&MK_SHIFT &&theApp.scene.bShowWorld)
	{
		SetEffectCenter(point);
		return;
	}
	CEmitterData* emitter = GetDocument()->m_pActiveEmitter;
	if(emitter && IS_CTRL() == 0)
	{
		CKeyPos& emit_pos =  (emitter->data) ? emitter->data->emitter_position : emitter->data_light->emitter_position;
		switch(theApp.scene.m_ToolMode)
		{
		case CD3DScene::TOOL_POSITION:
		{
			KeyPos* key  = HitTestPoint(emit_pos, point);
			if (key)
			{
				GetDocument()->m_nCurrentGenerationPoint = std::distance(emit_pos.begin(), key);
				ctrv->OnChangeActivePoint(0,0);//PostMessage(WM_ACTIVE_POINT,0,0);
//				SetKeyPos(key, point);
			}

//			HitPositionKey(point);
//			SetPositionKey(point);
			break;
		}
		case CD3DScene::TOOL_DIRECTION:
			//SetPositionKey(point, true);
			break;
		case CD3DScene::TOOL_SPLINE:
			if (emitter->Class() == EMC_SPLINE)
			{
				MatXf offs;
				GetDocument()->GetSplineMatrix(offs);
				KeyPos* t = HitTestPoint(emitter->GetSpl()->p_position,point,&offs);
				if (t)
				{
					GetDocument()->m_nCurrentParticlePoint = std::distance(emitter->GetSpl()->p_position.begin(),t);
					if (GetDocument()->m_nCurrentParticlePoint>=emitter->GetSpl()->p_position.size())
						GetDocument()->m_nCurrentParticlePoint = 0;
					ctrv->m_graph.Update(true);
				}
			}											
			
/*			if(GetDocument()->m_pActiveEmitter && (GetDocument()->m_pActiveEmitter->Class() == EMC_SPLINE))
			{
				CRect rc;
				GetClientRect(rc);

				MatXf offs;
				m_pSplinePointTrack = HitTestSplinePoint(GetDocument()->m_pActiveEmitter->GetSpl()->p_position, 
					float(point.x)/rc.Width() - 0.5f, float(point.y)/rc.Height() - 0.5f, GetDocument()->GetSplineMatrix(offs));
*//*
				if(m_pSplinePointTrack == 0)
				{
					m_pSplinePointTrack = AddSplinePoint(GetDocument()->m_pActiveEmitter->GetSpl()->p_position);
					SetSplineKey(point);
				}
*/
//			}
			break;
		}
	}

	m_bLeftPressed = true;

	SetCapture();
}
void CD3DWindow::OnLButtonUp(UINT nFlags, CPoint point) 
{
//	if (theApp.scene.m_ToolMode == CD3DScene::TOOL_SPLINE)
//	m_pSplinePointTrack = 0;

	m_bLeftPressed = false;
	ReleaseCapture();

	if(!IS_CTRL())
		theApp.scene.InitEmitters();
}

void CD3DWindow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	theApp.scene.SetActiveCamera(point);
/*	if(GetDocument()->m_pActiveEmitter)
		if(GetDocument()->m_pActiveEmitter->Class() == EMC_SPLINE)
		{
			CRect rc;
			GetClientRect(rc);

			MatXf offs;
			KeyPos* p = HitTestSplinePoint(GetDocument()->m_pActiveEmitter->GetSpl()->p_position, 
				float(point.x)/rc.Width() - 0.5f, float(point.y)/rc.Height() - 0.5f, GetDocument()->GetSplineMatrix(offs));
*/
			/*
			if(p)
				RemoveSplinePoint(GetDocument()->m_pActiveEmitter->GetSpl()->p_position, p);
			*/
//		}
}

BOOL CD3DWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	float delta = float(zDelta)/120.0f;

	theApp.scene.CameraZoom(-delta);

	return TRUE;
}

void CD3DWindow::OnSize(UINT nType, int cx, int cy) 
{
	static d=1;
	CView::OnSize(nType, cx, cy);
	if (cx<=0||cy<=0) return;
	if(d)
	{
//		theApp.scene.DoneRenderDevice();
		theApp.scene.InitRenderDevice(this);
	}
//	return;
	d=0;
	CRect rcWnd;
	GetClientRect(rcWnd);
	theApp.scene.Resize(rcWnd);
}

BOOL CD3DWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(IS_SHIFT()&&IS_RBUT())
	{
		SetCursor(m_hCursorHand);
	}
	else
		SetCursor(theApp.LoadStandardCursor(IDC_ARROW));
	
	return TRUE;
}

#define MOVE_EFFECT_STEP 10

void CD3DWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	/*
	if(nChar == VK_DELETE)
	{
		CEmitterData* pEmitter = GetDocument()->m_pActiveEmitter;
		if(pEmitter)
		{
			KeyPos* pos_key = theApp.scene.GetActiveWayPoint();
			if(pos_key)
				pEmitter->DeletePositionKey(pos_key);
		}
	}
	*/
	switch(nChar)
	{
	case VK_DELETE:
	{
		CEmitterData* pEmitter = GetDocument()->m_pActiveEmitter;
		if(pEmitter)
		{
			KeyPos* pos_key = theApp.scene.GetActiveWayPoint();
			if(pos_key)
				pEmitter->DeletePositionKey(pos_key);
		}
	}
		break;
/*
	case VK_LEFT:
//		if(GetDocument()->m_pActiveEmitter)
		{
			GetDocument()->MoveEffectCenter(-MOVE_EFFECT_STEP, 0);
		}
		break;
	case VK_RIGHT:
//		if(GetDocument()->m_pActiveEmitter)
		{
			GetDocument()->MoveEffectCenter(MOVE_EFFECT_STEP, 0);
		}
		break;
	case VK_UP:
//		if(GetDocument()->m_pActiveEmitter)
		{
			GetDocument()->MoveEffectCenter(0, -MOVE_EFFECT_STEP);
		}
		break;
	case VK_DOWN:
//		if(GetDocument()->m_pActiveEmitter)
		{
			GetDocument()->MoveEffectCenter(0, MOVE_EFFECT_STEP);
		}
*/
		break;
	}
}

void CD3DWindow::SetEffectCenter(CPoint pt)
{
	CRect rc;
	GetClientRect(rc);
	Vect3f v,pos,dir;
	theApp.scene.m_pActiveCamera->m_pCamera->GetWorldRay(Vect2f(float(pt.x)/rc.Width() - 0.5f, float(pt.y)/rc.Height() - 0.5f), pos, dir);
	if(!GetDocument()->m_WorldPath.IsEmpty()&&theApp.scene.m_pScene->Trace(pos,pos+dir,&v))
	{
		GetDocument()->m_EffectCenter = v;
	}
}

void CD3DWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnRButtonUp(nFlags, point);
}

