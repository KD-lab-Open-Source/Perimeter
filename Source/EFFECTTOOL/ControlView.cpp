// ControlView.cpp : implementation file
//

#include "stdafx.h"
#include "effecttool.h"
#include "ControlView.h"

extern COptTree* tr;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const float _fSliderScale = 100.0f;

const int   _nSliderLeftSideShift = 20;
const int   _nSliderRightSideShift = 7;

/////////////////////////////////////////////////////////////////////////////
// CControlView

IMPLEMENT_DYNCREATE(CControlView, CFormView)

CControlView::CControlView()
	: CFormView(CControlView::IDD)
{

/*
	m_chVelocity_Mul = false;
	m_chVelocityV = true;
	m_chGrav = false;
	m_chAngVel = false;
	m_chSize = false;
	m_nVelType = 0;
	m_nPoints = 0;
	m_fTime = 0.0f;
	m_nParticles = 0;
	m_chNParticles = false;
	m_fSize = 0.0f;
	m_chSizeDelta = false;
	m_fSizeDelta = 0.0f;
	m_chLifeTimeDelta = false;
	m_fLifeTimeDelta = 0.0f;
	m_fGrav = 0.0f;
	m_fVelocity = 0.0f;
	m_fAnglVel = 0.0f;
	m_chVelDelta = false;
	m_fVelDelta = 0.0f;
	m_fVelStart = 0.0f;
	m_bPlayCycled = false;
	m_nSplineEnding = 0;
	m_bAngChaos = false;
	m_bGenerateProlonged = false;
	m_nEmmiterType = -1;
	m_Angle_by_center = false;
	m_Use_Force_Field = false;
	m_Planar = FALSE;
	m_Base_angle = 0.0f;
	m_Add_Z = 0.0f;
	m_bUseLight = false;

	m_fAlpha_min = -180;
	m_fAlpha_max = 180;
	m_fTeta_min = -90;
	m_fTeta_max = 90;

	m_Direction = false;
*/
	//{{AFX_DATA_INIT(CControlView)
	m_CurTime = 0.0f;
	//}}AFX_DATA_INIT

	m_nTimelineUpdateTimer = 0;
	m_nGraphNormalTop = 0;
}

CControlView::~CControlView()
{
}

#define pi 3.14
#define R2D(rd) ((rd/pi)*180.f)
#define D2R(dg) ((dg/180.f)*pi)

void CControlView::SetControlsData()
{
	tr->SetControlsData();
/*
	CEmitterData* pActiveEmitter = GetDocument()->m_pActiveEmitter;
	if(pActiveEmitter)
	{
		if(pActiveEmitter->Class()==EMC_LIGHT){
			m_nPoints = 1;
			m_fTime   = pActiveEmitter->data_light->emitter_life_time;
			m_nEmmiterType = TYPE_LIGHT;
		}else{

			m_nPoints = pActiveEmitter->data->num_particle.size();
			m_fTime   = pActiveEmitter->data->emitter_life_time;

			m_nParticles     = pActiveEmitter->data->num_particle[GetDocument()->m_nCurrentGenerationPoint].f;
			m_fSizeDelta     = pActiveEmitter->data->begin_size_delta[GetDocument()->m_nCurrentGenerationPoint].f;
			m_fLifeTimeDelta = pActiveEmitter->data->life_time_delta[GetDocument()->m_nCurrentGenerationPoint].f;
			m_fSize          = pActiveEmitter->data->p_size[GetDocument()->m_nCurrentParticlePoint].f;
			m_fAnglVel       = pActiveEmitter->data->p_angle_velocity[GetDocument()->m_nCurrentParticlePoint].f;

			m_bAngChaos      = pActiveEmitter->data->rotation_direction == ETRD_RANDOM;

			m_fAlpha_min = R2D(pActiveEmitter->data->particle_position.alpha_min);
			m_fAlpha_max = R2D(pActiveEmitter->data->particle_position.alpha_max);
			m_fTeta_min = R2D(pActiveEmitter->data->particle_position.teta_min);
			m_fTeta_max = R2D(pActiveEmitter->data->particle_position.teta_max);

			tr->m_chPlume = pActiveEmitter->data->chPlume;
			tr->m_TraceCount = pActiveEmitter->data->TraceCount;
			tr->m_fPlumeInterval = pActiveEmitter->data->PlumeInterval;
			tr->m_fPlumeTimeScaling = pActiveEmitter->data->PlumeTimeScaling;
			tr->m_fPlumeSizeScaling = pActiveEmitter->data->PlumeSizeScaling;

			switch(pActiveEmitter->Class())
			{
			case EMC_LIGHT:
				m_nEmmiterType = TYPE_LIGHT;
				break;
			case EMC_SPLINE:
			{
	//			m_nVelType = 8;
				m_nEmmiterType = TYPE_SPLINE;
				m_nSplineEnding = pActiveEmitter->GetSpl()->p_position.cbegin;
				m_Direction = pActiveEmitter->GetSpl()->direction;
			}
				break;
			case EMC_INTEGRAL_Z:
			{
				m_nEmmiterType = TYPE_SURFACE;
				m_fVelDelta= pActiveEmitter->GetZ()->velocity_delta[GetDocument()->m_nCurrentGenerationPoint].f;
				m_fGrav = pActiveEmitter->GetZ()->p_gravity[GetDocument()->m_nCurrentParticlePoint].f;
				m_fVelocity = pActiveEmitter->GetZ()->p_velocity[GetDocument()->m_nCurrentParticlePoint].f;
				if(pActiveEmitter->GetZ()->begin_speed.size()){
					m_nVelType = pActiveEmitter->GetZ()->begin_speed[GetDocument()->m_nCurrentParticlePoint].velocity;
					m_fVelStart= pActiveEmitter->GetZ()->begin_speed[GetDocument()->m_nCurrentParticlePoint].mul;
				}

				m_Add_Z = pActiveEmitter->GetZ()->add_z;
				m_Angle_by_center = pActiveEmitter->GetZ()->angle_by_center;
				m_Planar = pActiveEmitter->GetZ()->planar;

				m_Use_Force_Field = pActiveEmitter->GetZ()->use_force_field;

				m_Base_angle = R2D(pActiveEmitter->GetZ()->base_angle);
			}
				break;
			case EMC_INTEGRAL:
			{
				m_nEmmiterType = TYPE_INTEGRAL;
				m_fVelDelta= pActiveEmitter->GetInt()->velocity_delta[GetDocument()->m_nCurrentGenerationPoint].f;
				m_fGrav    = pActiveEmitter->GetInt()->p_gravity[GetDocument()->m_nCurrentParticlePoint].f;
				m_fVelocity = pActiveEmitter->GetInt()->p_velocity[GetDocument()->m_nCurrentParticlePoint].f;
				if(pActiveEmitter->GetInt()->begin_speed.size()){
					m_nVelType = pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint].velocity;
					m_fVelStart= pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint].mul;
				}
			}
				break;
			default:
				break;
			}
	/*
			if(pActiveEmitter->Class() == EMC_INTEGRAL)
			{
				m_fVelDelta= pActiveEmitter->GetInt()->velocity_delta[GetDocument()->m_nCurrentGenerationPoint].f;
				m_fGrav    = pActiveEmitter->GetInt()->p_gravity[GetDocument()->m_nCurrentParticlePoint].f;
				if(pActiveEmitter->GetInt()->begin_speed.size()){
					m_nVelType = pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint].velocity;
					m_fVelStart= pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint].mul;
				}
			}
			else
			{
				m_nVelType = 8;
				m_nSplineEnding = pActiveEmitter->GetSpl()->p_position.cbegin;
			}
	* /
		}
		if(pActiveEmitter->Class()==EMC_LIGHT){
			m_bPlayCycled = pActiveEmitter->data_light->cycled;
//			m_bGenerateProlonged = pActiveEmitter->data_light->generate_prolonged;
		}else{
			m_bPlayCycled = pActiveEmitter->data->cycled;
			m_bGenerateProlonged = pActiveEmitter->data->generate_prolonged;
		}

		if(pActiveEmitter->Class()==EMC_INTEGRAL)
		{
			EmitterKeyInt* data=dynamic_cast<EmitterKeyInt*>(pActiveEmitter->data);
			m_bUseLight=data->use_light;
		}
		tr->UpdateData();
		UpdateData(FALSE);
	}
*/
}
void CControlView::SaveControlsData()
{
	tr->SaveControlsData();
/*
	CEmitterData* pActiveEmitter = GetDocument()->m_pActiveEmitter;
	if (!pActiveEmitter)
		return;
//	ASSERT(pActiveEmitter);

//	if(UpdateData())
	{
		if(pActiveEmitter->Class()==EMC_LIGHT){
		}else{
			pActiveEmitter->data->num_particle[GetDocument()->m_nCurrentGenerationPoint].f = m_nParticles;
			pActiveEmitter->data->begin_size_delta[GetDocument()->m_nCurrentGenerationPoint].f = m_fSizeDelta;
			pActiveEmitter->data->life_time_delta[GetDocument()->m_nCurrentGenerationPoint].f = m_fLifeTimeDelta;
			pActiveEmitter->data->p_size[GetDocument()->m_nCurrentParticlePoint].f = m_fSize;
			pActiveEmitter->data->p_angle_velocity[GetDocument()->m_nCurrentParticlePoint].f = m_fAnglVel;

			pActiveEmitter->data->rotation_direction = m_bAngChaos ? ETRD_RANDOM : ETRD_CW;

			pActiveEmitter->data->particle_position.alpha_min = D2R(m_fAlpha_min);
			pActiveEmitter->data->particle_position.alpha_max = D2R(m_fAlpha_max);
			pActiveEmitter->data->particle_position.teta_min = D2R(m_fTeta_min);
			pActiveEmitter->data->particle_position.teta_max = D2R(m_fTeta_max);

			pActiveEmitter->data->chPlume = tr->m_chPlume;
			pActiveEmitter->data->TraceCount = tr->m_TraceCount;
			pActiveEmitter->data->PlumeInterval = tr->m_fPlumeInterval;
			pActiveEmitter->data->PlumeTimeScaling = tr->m_fPlumeTimeScaling;
			pActiveEmitter->data->PlumeSizeScaling = tr->m_fPlumeSizeScaling;
		

			switch(pActiveEmitter->Class())
			{
			case EMC_LIGHT:
				break;
			case EMC_SPLINE:
			{
				pActiveEmitter->GetSpl()->p_position.cbegin = (CKeyPosHermit::CLOSING)m_nSplineEnding;
				pActiveEmitter->GetSpl()->p_position.cend   = (CKeyPosHermit::CLOSING)m_nSplineEnding;
				pActiveEmitter->GetSpl()->direction = (EMITTER_TYPE_DIRECTION_SPL)m_Direction;
			}
				break;
			case EMC_INTEGRAL_Z:
			{
				pActiveEmitter->GetZ()->velocity_delta[GetDocument()->m_nCurrentGenerationPoint].f = m_fVelDelta;
				pActiveEmitter->GetZ()->p_gravity[GetDocument()->m_nCurrentParticlePoint].f = m_fGrav;
				pActiveEmitter->GetZ()->p_velocity[GetDocument()->m_nCurrentParticlePoint].f = m_fVelocity;
				pActiveEmitter->GetZ()->begin_speed[GetDocument()->m_nCurrentParticlePoint].velocity = (EMITTER_TYPE_VELOCITY)m_nVelType;
				pActiveEmitter->GetZ()->begin_speed[GetDocument()->m_nCurrentParticlePoint].mul = m_fVelStart;

				pActiveEmitter->GetZ()->add_z = m_Add_Z;
				pActiveEmitter->GetZ()->angle_by_center = m_Angle_by_center;
				pActiveEmitter->GetZ()->base_angle = D2R(m_Base_angle);
				pActiveEmitter->GetZ()->planar = m_Planar;
				pActiveEmitter->GetZ()->use_force_field = m_Use_Force_Field;
			}
				break;
			case EMC_INTEGRAL:
			{
				pActiveEmitter->GetInt()->velocity_delta[GetDocument()->m_nCurrentGenerationPoint].f = m_fVelDelta;
				pActiveEmitter->GetInt()->p_gravity[GetDocument()->m_nCurrentParticlePoint].f = m_fGrav;
				pActiveEmitter->GetInt()->p_velocity[GetDocument()->m_nCurrentParticlePoint].f = m_fVelocity;
				pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint].velocity = (EMITTER_TYPE_VELOCITY)m_nVelType;
				pActiveEmitter->GetInt()->begin_speed[GetDocument()->m_nCurrentParticlePoint].mul = m_fVelStart;

			}
				break;
			default:
				break;
			}
		}

		if(pActiveEmitter->Class()==EMC_LIGHT){
			pActiveEmitter->data_light->cycled = m_bPlayCycled;
//			pActiveEmitter->data_light->generate_prolonged=m_bGenerateProlonged;
		}else{
			pActiveEmitter->data->cycled = m_bPlayCycled;
			pActiveEmitter->data->generate_prolonged=m_bGenerateProlonged;
		}

		if(pActiveEmitter->Class()==EMC_INTEGRAL)
		{
			EmitterKeyInt* data=dynamic_cast<EmitterKeyInt*>(pActiveEmitter->data);
			data->use_light=m_bUseLight;
		}

		pActiveEmitter->bDirty = true;
		ShowKeys();
		m_graph.Update(false);
		theApp.scene.InitEmitters();
		InitSlider();
	}
*/
}
void CControlView::AdjustColorControls()
{
	if(!GetDocument()->m_pActiveEmitter)
	{
		m_wndColors.ShowWindow(SW_HIDE);
		m_wndAlpha.ShowWindow(SW_HIDE);
		m_wndColors.m_Valid = false;
		m_wndAlpha.m_Valid = false;
	}
	else
	{
		m_wndColors.m_Valid = true;
		m_wndAlpha.m_Valid = (GetDocument()->m_pActiveEmitter->Class() != EMC_LIGHT);

		m_wndColors.ShowWindow(SW_SHOW);
		m_wndAlpha.ShowWindow(SW_SHOW);
/*
		float fTimeMax = GetDocument()->m_pActiveEmitter->ParticleLifeTime();
		float fStart = GetDocument()->get_particle_key_time(GetDocument()->m_nCurrentGenerationPoint, 0)/fTimeMax;
		float fEnd   = GetDocument()->get_particle_key_time(GetDocument()->m_nCurrentGenerationPoint, 
			GetDocument()->m_pActiveEmitter->data->p_size.size()-1)/fTimeMax;
*/

		float fTimeMax, fStart, fEnd;

		if(GetDocument()->m_pActiveEmitter->Class() == EMC_LIGHT){
			fTimeMax = GetDocument()->m_pActiveEmitter->data_light->emitter_life_time;
			fStart = 0; //GetDocument()->get_particle_key_time(GetDocument()->m_nCurrentGenerationPoint, 0)/fTimeMax;
			fEnd   = 1; //GetDocument()->get_particle_key_time(GetDocument()->m_nCurrentGenerationPoint, 
						//GetDocument()->m_pActiveEmitter->data->p_size.size()-1)/fTimeMax;
		}else{
			fTimeMax = GetDocument()->m_pActiveEmitter->ParticleLifeTime();
			fStart = GetDocument()->get_particle_key_time(GetDocument()->m_nCurrentGenerationPoint, 0)/fTimeMax;
			fEnd   = GetDocument()->get_particle_key_time(GetDocument()->m_nCurrentGenerationPoint, 
				GetDocument()->m_pActiveEmitter->data->p_size.size()-1)/fTimeMax;
		}

		if(fTimeMax < 1.0f)
			fEnd = fTimeMax;

		CRect rcWnd, rcClrBar;
		m_wndAlpha.GetWindowRect(rcClrBar);
		ScreenToClient(rcClrBar);
		GetClientRect(rcWnd);

		int nWidth = rcWnd.Width() - _nSliderLeftSideShift - _nSliderRightSideShift;
		CRect rc(nWidth*fStart + _nSliderLeftSideShift, rcClrBar.top, 
			nWidth*fEnd  + _nSliderLeftSideShift, rcClrBar.bottom);
		m_wndAlpha.MoveWindow(rc);

		rc.OffsetRect(0, -rcClrBar.Height() - 2);
		m_wndColors.MoveWindow(rc);

		m_wndColors.Invalidate();
		m_wndAlpha.Invalidate();
	}
}
void CControlView::AdjustEditControls()
{
/*	bool bFirstPoint = GetDocument()->m_nCurrentParticlePoint == 0;

	tr->ShowOptEmiter();
/*	tr->ItemShow(EDIT_PARTICLE_COUNT,bFirstPoint);
	tr->ItemShow(EDIT_SIZE_DELTA,bFirstPoint);
	tr->ItemShow(EDIT_TIME_DISP,bFirstPoint);
	tr->ItemShow(EDIT_VEL_DISP,bFirstPoint);
/*
	GetDlgItem(IDC_EDIT_PARTICLE_COUNT)->ShowWindow(bFirstPoint);
	GetDlgItem(IDC_STATIC_PARTICLE_COUNT)->ShowWindow(bFirstPoint);
	GetDlgItem(IDC_EDIT_SIZE_DELTA)->ShowWindow(bFirstPoint);
	GetDlgItem(IDC_STATIC_SIZE_DELTA)->ShowWindow(bFirstPoint);
	GetDlgItem(IDC_EDIT_TIME_DISP)->ShowWindow(bFirstPoint);
	GetDlgItem(IDC_STATIC_TIME_DISP)->ShowWindow(bFirstPoint);
	GetDlgItem(IDC_EDIT_VEL_DISP)->ShowWindow(bFirstPoint);
	GetDlgItem(IDC_STATIC_VEL_DISP)->ShowWindow(bFirstPoint);
*/
	
//	GetDlgItem(IDC_BUTTON_FORALL)->EnableWindow(bFirstPoint && GetDocument()->m_pActiveEmitter);
}

void CControlView::ShowKeys()
{
	CEmitterData* pActiveEmitter = GetDocument()->m_pActiveEmitter;

	list<CKey*> keylist;
	bool b = false;
	bool last = false;
	list<CKeyWithParam> emit_param;
	BSKey* pBSKey = 0;
//	if(pActiveEmitter->Class() != EMC_INTEGRAL && pActiveEmitter->Class() != EMC_INTEGRAL_Z)
//		b = true;
	b = (pActiveEmitter->Class() == EMC_SPLINE);

	if(pActiveEmitter->Class() != EMC_LIGHT){
		if(tr->NeedGraphSize())
			keylist.push_back(&pActiveEmitter->data->p_size);
		
		if(tr->NeedGraphGravitation())
			keylist.push_back(&pActiveEmitter->GetInt()->p_gravity);

		if(tr->NeedGraphAngleVel())
			keylist.push_back(&pActiveEmitter->data->p_angle_velocity);

		if(tr->NeedGraphVelocity())
		{
			keylist.push_back(&pActiveEmitter->GetInt()->p_velocity);
			last = true;
		}
		if(tr->NeedGraphVelMul())
			pBSKey = &pActiveEmitter->GetInt()->begin_speed;

		if(tr->NeedGraphParticlesCount())
			emit_param.push_back(CKeyWithParam(&pActiveEmitter->data->num_particle,100));
		if(tr->NeedGraphDeltaVel())
			emit_param.push_back(CKeyWithParam(&pActiveEmitter->GetInt()->velocity_delta,1));
		if(tr->NeedGraphDeltaSize())
			emit_param.push_back(CKeyWithParam(&pActiveEmitter->data->begin_size_delta,1));
		if(tr->NeedGraphDeltaTimeLife())
			emit_param.push_back(CKeyWithParam(&pActiveEmitter->data->life_time_delta,1));
	}
/*	
	if(pActiveEmitter->Class() == EMC_INTEGRAL){
		keylist.push_back(&pActiveEmitter->GetInt()->p_velocity);
	}else{
//		keylist.push_back(&pActiveEmitter->GetSpl()->p_position);
	}
		//keylist.push_back(&pActiveEmitter->key_dummy); //????!!!?!?!
*/
	switch(pActiveEmitter->Class())
	{
	case EMC_SPLINE:
		m_graph.ShowKeys(keylist, pBSKey, b, last/*false*/,emit_param);
		m_graph.ShowSplineKey(pActiveEmitter->GetSpl()->p_position);
		break;

	case EMC_LIGHT:
		if(tr->NeedGraphSize())
			keylist.push_back(&pActiveEmitter->GetLight()->emitter_size);
		m_graph.ShowKeys(keylist, pBSKey, b,false,emit_param);
//		m_graph.ShowKeys(keylist, pBSKey, b, false);
//		m_graph.ShowLightKey(pActiveEmitter->GetLight()->emitter_position);
		break;

	case EMC_INTEGRAL:
	case EMC_INTEGRAL_Z:
//		keylist.push_back(&pActiveEmitter->GetInt()->p_velocity);
		m_graph.ShowKeys(keylist, pBSKey, b, last,emit_param);
		break;
	}

/*
	if(pActiveEmitter->Class() == EMC_SPLINE){
		m_graph.ShowKeys(keylist, pBSKey, b, false);
		m_graph.ShowSplineKey(pActiveEmitter->GetSpl()->p_position);
	}else{
		m_graph.ShowKeys(keylist, pBSKey, b);
	}
*/
}

void CControlView::EnableControls(bool bEnable)
{
//	GetDocument()->m_EnableButForAll = bEnable;
//	GetDlgItem(IDC_BUTTON_FORALL)->ShowWindow(bEnable);
//	tr->DisableAllChItem();
//	GetDlgItem(IDC_USEL_LIGHT)->ShowWindow(bEnable);
/*	
	bEnable = false;

	GetDlgItem(IDC_STATIC_EMMITER_TYPE)->ShowWindow(bEnable);
	GetDlgItem(IDC_COMBO_EMMITER_TYPE)->ShowWindow(bEnable);
//	tr->ItemShow(COMBO_EMMITER_TYPE,bEnable);
	GetDlgItem(IDC_STATIC_NPOINTS)->ShowWindow(bEnable);
	GetDlgItem(IDC_EDIT_NPOINTS)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_NPOINTS,bEnable);
	GetDlgItem(IDC_STATIC_TIME)->ShowWindow(bEnable);
	GetDlgItem(IDC_EDIT_TIME)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_TIME,bEnable);
	GetDlgItem(IDC_COMBO_VEL_TYPE)->ShowWindow(bEnable);
//	tr->ItemShow(COMBO_VEL_TYPE,bEnable);
	GetDlgItem(IDC_STATIC_PARTICLE_COUNT)->ShowWindow(bEnable);
	GetDlgItem(IDC_EDIT_PARTICLE_COUNT)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_PARTICLE_COUNT,bEnable);
	GetDlgItem(IDC_STATIC_SIZE)->ShowWindow(bEnable);
	GetDlgItem(IDC_EDIT_SIZE)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_SIZE,bEnable);
	GetDlgItem(IDC_EDIT_SIZE_DELTA)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_SIZE_DELTA,bEnable);
	GetDlgItem(IDC_STATIC_SIZE_DELTA)->ShowWindow(bEnable);
	GetDlgItem(IDC_CHECK_SIZE)->ShowWindow(bEnable);
//	tr->ItemShow(CHECK_SIZE,bEnable);
	GetDlgItem(IDC_STATIC_GRAVITATION)->ShowWindow(bEnable);
	GetDlgItem(IDC_EDIT_GRAVITATION)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_GRAVITATION,bEnable);
	GetDlgItem(IDC_CHECK_GRAV)->ShowWindow(bEnable);
//	tr->ItemShow(CHECK_GRAV,bEnable);
	GetDlgItem(IDC_STATIC_ANG_VEL)->ShowWindow(bEnable);
	GetDlgItem(IDC_EDIT_ANG_VEL)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_ANG_VEL,bEnable);
	GetDlgItem(IDC_CHECK_ANG_VEL)->ShowWindow(bEnable);
//	tr->ItemShow(CHECK_ANG_VEL,bEnable);
	GetDlgItem(IDC_STATIC_TIME_DISP)->ShowWindow(bEnable);
	GetDlgItem(IDC_EDIT_TIME_DISP)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_TIME_DISP,bEnable);
	GetDlgItem(IDC_STATIC_VEL_DISP)->ShowWindow(bEnable);
	GetDlgItem(IDC_EDIT_VEL_DISP)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_VEL_DISP,bEnable);
	GetDlgItem(IDC_EDIT_VEL_START)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_VEL_START,bEnable);
	GetDlgItem(IDC_STATIC_VEL_START)->ShowWindow(bEnable);
	GetDlgItem(IDC_CHECK_VELOCITY)->ShowWindow(bEnable);
//	tr->ItemShow(CHECK_VELOCITY,bEnable);
	GetDlgItem(IDC_CHECK_PLAY_CYCLED)->ShowWindow(bEnable);
	GetDlgItem(IDC_COMBO_SPLINE_ENDING)->ShowWindow(bEnable);
//	tr->ItemShow(COMBO_SPLINE_ENDING,bEnable);
	GetDlgItem(IDC_CHECK_ANG_CHAOS)->ShowWindow(bEnable);
	GetDlgItem(IDC_GENERATE_PROLONGED)->ShowWindow(bEnable);
//	tr->ItemShow(CHECK_PROLONGED,bEnable);

	GetDlgItem(IDC_STATIC_GENERATION)->ShowWindow(bEnable);
	GetDlgItem(IDC_STATIC_VELOCITY)->ShowWindow(bEnable);
	GetDlgItem(IDC_STATIC_VEL_TYPE)->ShowWindow(bEnable);
	GetDlgItem(IDC_STATIC_SPLINE_ENDING)->ShowWindow(bEnable);
	GetDlgItem(IDC_STATIC_PARTICLE)->ShowWindow(bEnable);
	GetDlgItem(IDC_STATIC_DESP)->ShowWindow(bEnable);
	GetDlgItem(IDC_STATIC_EMITTER)->ShowWindow(bEnable);

	GetDlgItem(IDC_STATIC_SURFACE)->ShowWindow(bEnable);
	GetDlgItem(IDC_STATIC_SUR_SIZE)->ShowWindow(bEnable);
	GetDlgItem(IDC_STATIC_SUR_ANGLE)->ShowWindow(bEnable);
	GetDlgItem(IDC_EDIT_SUR_SIZE)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_SUR_SIZE,bEnable);
	GetDlgItem(IDC_EDIT_SUR_ANGLE)->ShowWindow(bEnable);
//	tr->ItemShow(EDIT_SUR_ANGLE,bEnable);
	GetDlgItem(IDC_CHECK_SUR_CENTER)->ShowWindow(bEnable);
//	tr->ItemShow(CHECK_SUR_CENTER,bEnable);
	GetDlgItem(IDC_CHECK_SUR_PLANAR)->ShowWindow(bEnable);
//	tr->ItemShow(CHECK_SUR_PLANAR,bEnable);
	GetDlgItem(IDC_CHECK_USE_FORCE_FIELD)->ShowWindow(bEnable);
//	tr->ItemShow(CHECK_USE_FORCE_FIELD,bEnable);
*/
	if(bEnable)
		AdjustEditControls();

	CRect rc;
	m_graph.GetWindowRect(rc);
	ScreenToClient(rc);

	if(m_nGraphNormalTop == 0)
		m_nGraphNormalTop = rc.top;

/*	if(bEnable)
	{
		rc.top = m_nGraphNormalTop;
		m_graph.MoveWindow(rc);
	}
	else*/
	{
		rc.top = 24;
		m_graph.MoveWindow(rc);
	}
}
void CControlView::EnableEmitterKeyControls()
{
//	tr->ShowOptEmiter(); //temp
/*	return;
	CEmitterData* pActiveEmitter = GetDocument()->m_pActiveEmitter;
	if(!pActiveEmitter)
		return;

	bool b = false;
	bool bl = false;
	bool bz = false;

	switch(pActiveEmitter->Class())
	{
	case EMC_SPLINE:
		b = false;
		break;
	case EMC_INTEGRAL:
		b = true;
		break;
	case EMC_INTEGRAL_Z:
		b = true;
		bz = true;
		break;
	case EMC_LIGHT:
		b = false;
		bl = true;
		break;
	}

//	bool b = pActiveEmitter->Class() != EMC_SPLINE;

//	GetDlgItem(IDC_EDIT_VEL_START)->EnableWindow(b);
	tr->ItemShow(EDIT_VEL_START,b);
//	GetDlgItem(IDC_STATIC_VEL_START)->EnableWindow(b);
//	GetDlgItem(IDC_CHECK_VELOCITY)->EnableWindow(b);
	tr->ItemShow(CHECK_VELOCITY,b);
//	GetDlgItem(IDC_STATIC_GRAVITATION)->EnableWindow(b);
//	GetDlgItem(IDC_EDIT_GRAVITATION)->EnableWindow(b);
	tr->ItemShow(EDIT_GRAVITATION,b);
//	GetDlgItem(IDC_CHECK_GRAV)->EnableWindow(b);
	tr->ItemShow(CHECK_GRAV,b);
//	GetDlgItem(IDC_STATIC_VEL_DISP)->EnableWindow(b);
//	GetDlgItem(IDC_EDIT_VEL_DISP)->EnableWindow(b);
	tr->ItemShow(EDIT_VEL_DISP,b);
//	GetDlgItem(IDC_COMBO_SPLINE_ENDING)->EnableWindow(!b&&!bl);
	tr->ItemShow(COMBO_SPLINE_ENDING,!b&&!bl);
//	GetDlgItem(IDC_STATIC_SPLINE_ENDING)->EnableWindow(!b&&!bl);

	bool b1 = false; 
	if(pActiveEmitter->Class()!=EMC_LIGHT)
		b1 = pActiveEmitter->data->num_particle.size()>1;

//	GetDlgItem(IDC_GENERATE_PROLONGED)->EnableWindow(b1);
//	GetDlgItem(IDC_STATIC_TIME)->EnableWindow(b1);
//	GetDlgItem(IDC_EDIT_TIME)->EnableWindow(b1);
	tr->ItemShow(EDIT_TIME,b1);

	//////////
	GetDlgItem(IDC_BUTTON_FORALL)->EnableWindow(!bl);
//	GetDlgItem(IDC_STATIC_NPOINTS)->EnableWindow(!bl);
//	GetDlgItem(IDC_EDIT_NPOINTS)->EnableWindow(!bl);
	tr->ItemShow(EDIT_NPOINTS,!bl);
//	GetDlgItem(IDC_STATIC_TIME)->EnableWindow(!bl);
//	GetDlgItem(IDC_EDIT_TIME)->EnableWindow(!bl);
	tr->ItemShow(EDIT_TIME,!bl);
//	GetDlgItem(IDC_COMBO_VEL_TYPE)->EnableWindow(!bl);
	tr->ItemShow(COMBO_VEL_TYPE,!bl);
//	GetDlgItem(IDC_STATIC_PARTICLE_COUNT)->EnableWindow(!bl);
//	GetDlgItem(IDC_EDIT_PARTICLE_COUNT)->EnableWindow(!bl);
	tr->ItemShow(EDIT_PARTICLE_COUNT,!bl);
//	GetDlgItem(IDC_STATIC_SIZE)->EnableWindow(!bl);
//	GetDlgItem(IDC_EDIT_SIZE)->EnableWindow(!bl);
	tr->ItemShow(EDIT_SIZE,!bl);
//	GetDlgItem(IDC_EDIT_SIZE_DELTA)->EnableWindow(!bl);
	tr->ItemShow(EDIT_SIZE_DELTA,!bl);
//	GetDlgItem(IDC_STATIC_SIZE_DELTA)->EnableWindow(!bl);
//	GetDlgItem(IDC_CHECK_SIZE)->EnableWindow(!bl);
	tr->ItemShow(CHECK_SIZE,!bl);
//	GetDlgItem(IDC_STATIC_ANG_VEL)->EnableWindow(!bl);
//	GetDlgItem(IDC_EDIT_ANG_VEL)->EnableWindow(!bl);
	tr->ItemShow(EDIT_ANG_VEL,!bl);
//	GetDlgItem(IDC_CHECK_ANG_VEL)->EnableWindow(!bl);
	tr->ItemShow(CHECK_ANG_VEL,!bl);
//	GetDlgItem(IDC_STATIC_TIME_DISP)->EnableWindow(!bl);
//	GetDlgItem(IDC_EDIT_TIME_DISP)->EnableWindow(!bl);
	tr->ItemShow(EDIT_TIME_DISP,!bl);
//	GetDlgItem(IDC_CHECK_PLAY_CYCLED)->EnableWindow(true);
	tr->ItemShow(CHECK_PLAY_CYCLED,true);
//	GetDlgItem(IDC_CHECK_ANG_CHAOS)->EnableWindow(!bl);
	tr->ItemShow(CHECK_ANG_CHAOS,!bl);
//	GetDlgItem(IDC_GENERATE_PROLONGED)->EnableWindow(!bl);
	tr->ItemShow(CHECK_PROLONGED,!bl);

//	GetDlgItem(IDC_STATIC_GENERATION)->EnableWindow(!bl);
//	GetDlgItem(IDC_STATIC_VELOCITY)->EnableWindow(!bl);
//	GetDlgItem(IDC_STATIC_VEL_TYPE)->EnableWindow(!bl);
//	GetDlgItem(IDC_STATIC_PARTICLE)->EnableWindow(!bl);
//	GetDlgItem(IDC_STATIC_DESP)->EnableWindow(!bl);
//	GetDlgItem(IDC_STATIC_EMITTER)->EnableWindow(!bl);

//	GetDlgItem(IDC_STATIC_SURFACE)->EnableWindow(bz);
//	GetDlgItem(IDC_STATIC_SUR_SIZE)->EnableWindow(bz);
//	GetDlgItem(IDC_STATIC_SUR_ANGLE)->EnableWindow(bz);
//	GetDlgItem(IDC_EDIT_SUR_SIZE)->EnableWindow(bz);
	tr->ItemShow(EDIT_SUR_SIZE,bz);
//	GetDlgItem(IDC_EDIT_SUR_ANGLE)->EnableWindow(bz);
	tr->ItemShow(EDIT_SUR_ANGLE,bz);
//	GetDlgItem(IDC_CHECK_SUR_CENTER)->EnableWindow(bz);
	tr->ItemShow(CHECK_SUR_CENTER,bz);
//	GetDlgItem(IDC_CHECK_SUR_PLANAR)->EnableWindow(bz);
	tr->ItemShow(CHECK_SUR_PLANAR,bz);
//	GetDlgItem(IDC_CHECK_USE_FORCE_FIELD)->EnableWindow(bz);
	tr->ItemShow(CHECK_USE_FORCE_FIELD,bz);
	//////////

//	GetDlgItem(IDC_USEL_LIGHT)->EnableWindow(pActiveEmitter->Class()==EMC_INTEGRAL);
*/	
}

LRESULT CControlView::InitSlider(WPARAM, LPARAM)
{
	if(GetDocument()->m_pActiveEmitter)
		m_slider_time.SetRange(0, GetDocument()->m_pActiveEmitter->ParticleLifeTime()*_fSliderScale);
	else if(GetDocument()->m_pActiveEffect)
		m_slider_time.SetRange(0, m_graph.GetTimescaleMax()*_fSliderScale);

	return 0;
}
LRESULT CControlView::OnChangeActivePoint(WPARAM, LPARAM)
{
	AdjustColorControls();
	AdjustEditControls();
	SetControlsData();
	theApp.scene.InitEmitters();
	m_graph.Update(false);
	InitSlider();

//	theApp.scene.SetActiveKeyPos(GetDocument()->m_nCurrentGenerationPoint);
	tr->ShowOptEmiter();
/*	if(GetDocument()->m_pActiveEmitter->Class())
	{
		switch(GetDocument()->m_pActiveEmitter->Class())
		{
		case EMC_SPLINE:
//			GetDlgItem(IDC_COMBO_VEL_TYPE)->EnableWindow(GetDocument()->m_nCurrentParticlePoint == 0);
			tr->ItemShow(COMBO_VEL_TYPE,GetDocument()->m_nCurrentParticlePoint == 0);
			break;
		case EMC_INTEGRAL:
			/*
			if(GetDocument()->m_nCurrentParticlePoint == 0)
			{
				if(GetDlgItem(IDC_COMBO_VEL_TYPE)->SendMessage(CB_GETCOUNT) < 9)
					GetDlgItem(IDC_COMBO_VEL_TYPE)->SendMessage(CB_ADDSTRING, 0, (LPARAM)"Spline");
			}
			else
				GetDlgItem(IDC_COMBO_VEL_TYPE)->SendMessage(CB_DELETESTRING, 8);
			* /
			break;
		case EMC_INTEGRAL_Z:
			break;
		case EMC_LIGHT:
			break;
		}

		/*
		if(GetDocument()->m_pActiveEmitter->Class() == EMC_SPLINE)
			GetDlgItem(IDC_COMBO_VEL_TYPE)->EnableWindow(GetDocument()->m_nCurrentParticlePoint == 0);
		else
		{
			if(GetDocument()->m_nCurrentParticlePoint == 0)
			{
				if(GetDlgItem(IDC_COMBO_VEL_TYPE)->SendMessage(CB_GETCOUNT) < 9)
					GetDlgItem(IDC_COMBO_VEL_TYPE)->SendMessage(CB_ADDSTRING, 0, (LPARAM)"Spline");
			}
			else
				GetDlgItem(IDC_COMBO_VEL_TYPE)->SendMessage(CB_DELETESTRING, 8);
		}
		* /
	}*/
	return 0;
}

void CControlView::DoDataExchange(CDataExchange* pDX)
{

	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlView)
	DDX_Control(pDX, IDC_GRAPH, m_graph);
	DDX_Control(pDX, IDC_SLIDER_TIME, m_slider_time);
	DDX_Control(pDX, IDC_ALPHA, m_wndAlpha);
	DDX_Control(pDX, IDC_COLORS, m_wndColors);
	DDX_Text(pDX, IDC_EDIT_CUR_TIME, m_CurTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CControlView, CFormView)
	//{{AFX_MSG_MAP(CControlView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_COMMAND(ID_BUTTON_FORALL, OnButtonForall)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SLIDER_UPDATE, InitSlider)
	ON_MESSAGE(WM_ACTIVE_POINT, OnChangeActivePoint)
	ON_CBN_SELCHANGE(IDC_COMBO_EMMITER_TYPE, OnSelchangeComboEmmiterType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlView message handlers

void CControlView::OnSize(UINT nType, int cx, int cy) 
{
	RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST);

	SetScaleToFitSize(CSize(cx, cy));
	//CFormView::OnSize(nType, cx, cy);

	if(m_wndAlpha.m_hWnd)
	{
		CRect rcClrBar;
		m_wndAlpha.GetWindowRect(rcClrBar);

		CRect rc(2, cy - rcClrBar.Height() - 4, cx - 2, cy - 4);
		m_wndAlpha.MoveWindow(rc);

		rc.OffsetRect(0, -rcClrBar.Height() - 2);
		m_wndColors.MoveWindow(rc);

		rc.OffsetRect(0, -rcClrBar.Height());
		rc.InflateRect(2, 1);
		rc.left  += _nSliderLeftSideShift;
		rc.right -= _nSliderRightSideShift;
		m_slider_time.MoveWindow(rc);

		rc.left  -= _nSliderLeftSideShift;
		rc.right += _nSliderRightSideShift;

/*		CRect rcBtnAll;
		GetDlgItem(IDC_BUTTON_FORALL)->GetWindowRect(rcBtnAll);
		ScreenToClient(&rcBtnAll);
*/
		rc.OffsetRect(0, -20);
		rc.top = 4;//rcBtnAll.bottom + 4;
		rc.DeflateRect(3, 0);
		m_graph.MoveWindow(rc);

		EnableControls(GetDocument()->m_pActiveEmitter != 0);
	}
	GetDocument()->SetWorkingDir();
}

int CControlView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, 
		WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS|CBRS_FLYBY);
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR1);
	m_wndToolBar.SetButtonStyle(0, TBBS_CHECKBOX);

	GetDocument()->m_EnableButForAll =1;
	return 0;
}
void CControlView::SetPause()
{
	m_wndToolBar.GetToolBarCtrl().CheckButton(ID_TB_PAUSE);
}

void CControlView::OnPaint() 
{
	CPaintDC dc(this);
/*
	CRect rcBar;
	m_wndToolBar.GetClientRect(rcBar);

	rcBar.bottom += 2;
	rcBar.right += 10;

	dc.Draw3dRect(rcBar, GetSysColor(COLOR_3DLIGHT), GetSysColor(COLOR_3DSHADOW)); 
*/
}

void CControlView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	m_graph.Init();

	m_nTimelineUpdateTimer = SetTimer(1, 10, 0);
	theApp.scene.InitEmitters();
}

void CControlView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CEmitterData* pActiveEmitter;
	if((pActiveEmitter = GetDocument()->m_pActiveEmitter) != 0)
	{
		if(pActiveEmitter->Class()==EMC_LIGHT)
		{
			m_wndColors.SetKey(&pActiveEmitter->GetLight()->emitter_color, false);
			
			m_wndAlpha.ShowWindow(SW_HIDE);//SetKey(&pActiveEmitter->data->p_alpha, true);
			m_wndAlpha.m_Valid = false;
			GetDlgItem(IDC_ALPHA)->ShowWindow(SW_HIDE);
		}else{
			m_wndColors.SetKey(&pActiveEmitter->data->p_color, false);
			
			m_wndAlpha.m_Valid = true;
			GetDlgItem(IDC_ALPHA)->ShowWindow(SW_SHOW);
			m_wndAlpha.ShowWindow(SW_SHOW);
			m_wndAlpha.SetKey(&pActiveEmitter->data->p_alpha, true);
		}

		SetControlsData();
		ShowKeys();

//		GetDlgItem(IDC_COMBO_VEL_TYPE)->EnableWindow(TRUE);
//		tr->ItemShow(COMBO_VEL_TYPE,true);
	}

//	GetDlgItem(IDC_GENERATE_PROLONGED)->EnableWindow(pActiveEmitter!=NULL);	
//	tr->ItemShow(CHECK_PROLONGED,pActiveEmitter!=NULL);
	tr->PutButton(pActiveEmitter != 0);
	tr->ShowOptEmiter();
	EnableControls(pActiveEmitter != 0);
	
	AdjustColorControls();
	EnableEmitterKeyControls();

	InitSlider();
	m_slider_time.SetPos(0);
	m_graph.Update(true);

	GetDocument()->SetWorkingDir();
}

void CControlView::OnDestroy() 
{
	KillTimer(m_nTimelineUpdateTimer);

	CFormView::OnDestroy();
}

void CControlView::OnTimer(UINT nIDEvent) 
{
	CEmitterData* pActiveEmitter = GetDocument()->m_pActiveEmitter;
	
	if(!theApp.scene.GetEffect())
		return;

	if(nIDEvent == m_nTimelineUpdateTimer) //slider update
	{
		float fTime = theApp.scene.GetEffect()->GetTime();

//		m_CurTime = fTime;
//		UpdateData(false);

		if(pActiveEmitter)
		{
			if(pActiveEmitter->Class()==EMC_LIGHT){
//				fTime -= pActiveEmitter->data_light->emitter_create_time
				if(fTime < pActiveEmitter->data_light->emitter_create_time)
					m_slider_time.SetPos(0);
				else if(fTime - pActiveEmitter->data_light->emitter_create_time > pActiveEmitter->ParticleLifeTime())
					m_slider_time.SetPos(m_slider_time.GetRangeMax() - 1);
				else
					m_slider_time.SetPos(round((fTime - pActiveEmitter->data_light->emitter_create_time)*_fSliderScale));
			}else{
				if(fTime < pActiveEmitter->data->emitter_create_time)
					m_slider_time.SetPos(0);
				else if(fTime - pActiveEmitter->data->emitter_create_time > pActiveEmitter->ParticleLifeTime())
					m_slider_time.SetPos(m_slider_time.GetRangeMax() - 1);
				else
					m_slider_time.SetPos(round((fTime - pActiveEmitter->data->emitter_create_time)*_fSliderScale));
			}
		}
		else if(GetDocument()->m_pActiveEffect)
			m_slider_time.SetPos(fTime*_fSliderScale);

/*		float fTime = pActiveEmitter->Effect2EmitterTime();
		m_slider_time.SetPos(fTime*_fSliderScale);
*/
//		if(GetDocument()->m_pActiveEffect && fTime>m_graph.GetTimescaleMax())
//			theApp.scene.m_pScene->SetTime(0);

		m_slider_time.EnableWindow(theApp.scene.bPause);
	}

	CFormView::OnTimer(nIDEvent);
}

void CControlView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	float fTime = float(m_slider_time.GetPos())/_fSliderScale;

	if(GetDocument()->m_pActiveEmitter){
		if(GetDocument()->m_pActiveEmitter->Class()==EMC_LIGHT)
			theApp.scene.GetEffect()->MoveToTime(/*GetDocument()->m_pActiveEmitter->GlobalTime(fTime)*/
				GetDocument()->m_pActiveEmitter->data_light->emitter_create_time + fTime);
		else
			theApp.scene.GetEffect()->MoveToTime(/*GetDocument()->m_pActiveEmitter->GlobalTime(fTime)*/
				GetDocument()->m_pActiveEmitter->data->emitter_create_time + fTime);

	}else
		theApp.scene.GetEffect()->MoveToTime(fTime);
}

void CControlView::OnChangeEditTime() 
{
	ASSERT(GetDocument()->m_pActiveEmitter);

	if(UpdateData())
	{
//		GetDocument()->m_pActiveEmitter->ChangeLifetime(m_fTime);
		
		SetControlsData();
		
		InitSlider();
		m_graph.Update(true);
		theApp.scene.InitEmitters();
	}
}
void CControlView::OnChangeEditNpoints(int n) 
{
	CEmitterData* pEmitter = GetDocument()->m_pActiveEmitter;
	ASSERT(pEmitter);

//	if(UpdateData())
	if (pEmitter->data)
	{
/*		int old_n = pEmitter->data->emitter_position.size();
		for(int i=old_n;i<m_nPoints;++i)
			pEmitter->InsertGenerationPoint(0)=1.0f;
		for(int i=m_nPoints;i<old_n;++i)
			pEmitter->DeleteGenerationPoint(0);
		if (pEmitter->data->emitter_position.size()>=2)
		{
			pEmitter->data->emitter_position.begin()->time = 0;
			(pEmitter->data->emitter_position.end()-1)->time = 1.f;
		}


//*/	
//		GetDocument()->m_pActiveEmitter->SetGenerationPointCount(n/*m_nPoints*/);
//		GetDocument()->m_nCurrentGenerationPoint = 0;

		InitSlider();
		m_graph.Update(true);

		AdjustColorControls();
		theApp.scene.InitEmitters();
		////
//		bool b1 = GetDocument()->m_pActiveEmitter->data->num_particle.size()>1;
//		GetDlgItem(IDC_GENERATE_PROLONGED)->EnableWindow(b1);
//		GetDlgItem(IDC_EDIT_TIME)->EnableWindow(b1);
//		GetDlgItem(IDC_STATIC_TIME)->EnableWindow(b1);
//		tr->ItemShow(CHECK_PROLONGED,b1);
//		tr->ItemShow(EDIT_TIME,b1);
		tr->ShowOptEmiter();
		////
	}
}

void CControlView::OnSelchangeComboVelType() 
{
//	if(UpdateData())
	{
		/*
		switch(m_nVelType)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		{
			GetDocument()->ResetEmitterType(EMC_INTEGRAL);
			SaveControlsData();
		}
			break;
		case 8:
		{
			GetDocument()->ResetEmitterType(EMC_SPLINE);
			theApp.scene.InitEmitters();
		}
			break;
		case 9:
			GetDocument()->ResetEmitterType(EMC_INTEGRAL_Z);
			SaveControlsData();
			break;
		case 10:
			GetDocument()->ResetEmitterType(EMC_LIGHT);
			SaveControlsData();
			break;
		}
		*/
/*
		if(m_nVelType > 7)
		{
			GetDocument()->ResetEmitterType(EMC_SPLINE);
			theApp.scene.InitEmitters();
		}
		else
		{
			GetDocument()->ResetEmitterType(EMC_INTEGRAL);
			SaveControlsData();
		}
*/

		SaveControlsData();

		OnUpdate(0, 0, 0);
		//EnableEmitterKeyControls();
		//SetControlsData();
		//ShowKeys();
	}
}

void CControlView::OnCheckPlayCycled() 
{
	SaveControlsData();
}
void CControlView::OnGenerateProlonged() 
{
	SaveControlsData();
}

void CControlView::OnSelchangeComboSplineEnding() 
{
	SaveControlsData();
}
void CControlView::OnCheckAngChaos() 
{
	SaveControlsData();
}

void CControlView::OnCheckGrav() 
{
	ShowKeys();
}
void CControlView::OnCheckSize() 
{
	ShowKeys();
}
void CControlView::OnCheckAngVel() 
{
	ShowKeys();
}
void CControlView::OnCheckVelocity() 
{
	ShowKeys();
}

void CControlView::OnButtonForall() 
{
	ASSERT(GetDocument()->m_pActiveEmitter);

	if(GetDocument()->m_pActiveEmitter->Class()==EMC_LIGHT)
		return;

	if(UpdateData())
	{
		CEmitterData* pEmitter = GetDocument()->m_pActiveEmitter;
/*		int nKeys = pEmitter->data->p_size.size();
		for(int i=0; i<nKeys; i++)
		{
			pEmitter->data->p_size[i].f = m_fSize;
			pEmitter->data->p_angle_velocity[i].f = m_fAnglVel;

			if(pEmitter->Class() == EMC_INTEGRAL)
				pEmitter->GetInt()->p_gravity[i].f = m_fGrav;
		}
*/
		for(int i = 0; i<pEmitter->data->life_time.size(); i++)
		{
			pEmitter->data->life_time[i].f = pEmitter->data->life_time[GetDocument()->m_nCurrentGenerationPoint].f;
			pEmitter->data->num_particle[i].f = pEmitter->data->num_particle[GetDocument()->m_nCurrentGenerationPoint].f;
			pEmitter->data->life_time_delta[i].f = pEmitter->data->life_time_delta[GetDocument()->m_nCurrentGenerationPoint].f;
			pEmitter->data->begin_size_delta[i].f = pEmitter->data->begin_size_delta[GetDocument()->m_nCurrentGenerationPoint].f;

			if(pEmitter->Class() == EMC_INTEGRAL || pEmitter->Class() == EMC_INTEGRAL_Z)
				pEmitter->GetInt()->velocity_delta[i].f = pEmitter->GetInt()->velocity_delta[GetDocument()->m_nCurrentGenerationPoint].f;
		}

		m_graph.Update(false);
		theApp.scene.InitEmitters();
	}
}

BOOL CControlView::PreTranslateMessage(MSG* pMsg) 
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		switch(::GetDlgCtrlID(pMsg->hwnd))
		{

		case IDC_EDIT_TIME:
			OnChangeEditTime();
			break;

		case IDC_EDIT_NPOINTS:
			OnChangeEditNpoints(1);//never
			break;

		default:
			SaveControlsData();
		}
		
		return TRUE;
	}
	
	return CFormView::PreTranslateMessage(pMsg);
}

void CControlView::OnSelchangeComboEmmiterType() 
{
//	if(UpdateData())
	{
		switch(m_nEmmiterType)
		{
			case TYPE_INTEGRAL:// Integral
			{
				GetDocument()->ResetEmitterType(EMC_INTEGRAL);
//				SaveControlsData();
			}
				break;
			case TYPE_SURFACE:// Surface
			{
				if(!GetDocument()->m_WorldPath.IsEmpty()){
					GetDocument()->ResetEmitterType(EMC_INTEGRAL_Z);
//					SaveControlsData();
				}else{
					m_nEmmiterType = TYPE_INTEGRAL;
					UpdateData(false);
//					OnSelchangeComboEmmiterType();
				}
			}
				break;
			case TYPE_SPLINE:// Spline
			{
				GetDocument()->ResetEmitterType(EMC_SPLINE);
//				SaveControlsData();
//				theApp.scene.InitEmitters();
			}
				break;
			case TYPE_LIGHT:// Light
			{
				GetDocument()->ResetEmitterType(EMC_LIGHT);
//				SaveControlsData();
//				theApp.scene.InitEmitters();
			}
				break;
		}
	//tr->SetControlsData(false,false);
		OnUpdate(0, 0, 0);
	}
// */
}

void CControlView::OnCheckSurCenter() 
{
	SaveControlsData();
}

void CControlView::OnCheckSurPlanar() 
{
	SaveControlsData();
}

void CControlView::OnCheckUseForceField() 
{
	SaveControlsData();
}

void CControlView::OnUselLight() 
{
	SaveControlsData();
}
