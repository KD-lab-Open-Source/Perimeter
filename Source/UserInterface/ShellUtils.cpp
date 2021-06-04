
#include "StdAfx.h"

#include "GameShell.h"
#include "runtime.h"
#include "CameraManager.h"
#include "terra.h"
#include "UnitAttribute.h"
#include "Universe.h"
#include "GenericControls.h"
#include "UniverseInterface.h"


///////////////////////////////////////////////////////////////////////
// облет точки на карте
/*
const int   circuitStepping = 32;
const float mapBordersForCamera = 10.0f;

void CorrectMapZ(Vect3f& v)
{
	float z = (float)(vMap.GetAlt(vMap.XCYCL(round(v.x)),vMap.YCYCL(round(v.y))) >> VX_FRACTION);
	if(v.z < CAMERA_MIN_HEIGHT + z)
		v.z = CAMERA_MIN_HEIGHT + z;
	else
	{
		if(v.z > CAMERA_MAX_HEIGHT)
			v.z = CAMERA_MAX_HEIGHT;
	};
}

CameraPathType& GetPositionCircuit(const Vect3f& pos, CameraPathType& path)
{
	Vect3f v0, v, a;

	MatXf matrix;
	terCamera->GetPosition(&matrix);
	v0 = matrix.invXformVect(matrix.trans(),v0);
	v0.negate();	

	float circuitDistance = v0.distance(pos);
	float circuitViewAngle = acosf((v0.z - pos.z)/circuitDistance);

	if(circuitViewAngle < M_PI/10)
		circuitViewAngle = M_PI/10;

	float dPsi = 2*M_PI/circuitStepping;
	float psi = M_PI/2 - atan2f((v0-pos).x, (v0-pos).y);
	float da  = 0;

	while(da < 2*M_PI)
	{
		v.setSpherical(psi, circuitViewAngle, circuitDistance);

		float angle = M_PI/2 - psi;
		//float angle = atan2f(v.x, v.y);
		if(angle > 2*M_PI)
			angle -= 2*M_PI;
		if(angle < 0)
			angle += 2*M_PI;
		if(angle < 0)
			angle += 2*M_PI;

		v += pos;
		if(v.x < mapBordersForCamera)
			v.x = mapBordersForCamera;
		if(v.y < mapBordersForCamera)
			v.y = mapBordersForCamera;
		if(v.x > vMap.H_SIZE - mapBordersForCamera)
			v.x = vMap.H_SIZE - mapBordersForCamera;
		if(v.y > vMap.V_SIZE - mapBordersForCamera)
			v.y = vMap.V_SIZE - mapBordersForCamera;

		CorrectMapZ(v);
		path.push_back(CAMERA_PATH_POINT(v, Vect3f(circuitViewAngle + M_PI/10, 0, angle)));

		psi += dPsi;
		da += dPsi;
	}

	return path;
}
*/
void GetCameraViewPolygon(Vect2f* pv, int zero_level)
{
	MatXf Matrix;
	Vect2f Center,Focus;
	Vect3f p0, p1, p2, p3, p4;
	terCamera->GetCamera()->GetPosition(&Matrix);
	terCamera->GetCamera()->GetFrustum(&Center, 0, &Focus, 0);
	Vect2f focus(Focus.x,Focus.y*terScreenSizeX/terScreenSizeY);

	Matrix.invXformPoint(Vect3f(0,0,0), p0);
	Matrix.invXformPoint(Vect3f(-0.5f/focus.x, -0.5f/focus.y,1),p1);
	Matrix.invXformPoint(Vect3f(-0.5f/focus.x, 0.5f/focus.y,1),p2);
	Matrix.invXformPoint(Vect3f(0.5f/focus.x, 0.5f/focus.y,1),p3);
	Matrix.invXformPoint(Vect3f(0.5f/focus.x, -0.5f/focus.y,1),p4);

	pv[0].x = p0.x - (p1.x - p0.x)*(p0.z - zero_level)/(p1.z - p0.z);
	pv[0].y = p0.y - (p1.y - p0.y)*(p0.z - zero_level)/(p1.z - p0.z);

	pv[1].x = p0.x - (p2.x - p0.x)*(p0.z - zero_level)/(p2.z - p0.z);
	pv[1].y = p0.y - (p2.y - p0.y)*(p0.z - zero_level)/(p2.z - p0.z);

	pv[2].x = p0.x - (p3.x - p0.x)*(p0.z - zero_level)/(p3.z - p0.z);
	pv[2].y = p0.y - (p3.y - p0.y)*(p0.z - zero_level)/(p3.z - p0.z);

	pv[3].x = p0.x - (p4.x - p0.x)*(p0.z - zero_level)/(p4.z - p0.z);
	pv[3].y = p0.y - (p4.y - p0.y)*(p0.z - zero_level)/(p4.z - p0.z);
}

///////////////////////////////////////////////////////////////////////
//2D line clipping
enum
{
	LEFT = 1,
	RIGHT = 2,
	BOTTOM = 4,
	TOP = 8
};
inline int ClipOutCode(float x, float y, float xmin, float xmax, float ymin, float ymax)
{
	int code = 0;
	if(y > ymax)
		code |= TOP;
	else if(y < ymin)
		code |= BOTTOM;
	if(x > xmax)
		code |= RIGHT;
	else if(x < xmin)
		code |= LEFT;

	return code;
}

bool LineClip2d(Vect2i& v1, Vect2i& v2,  const Vect2i& _w1, const Vect2i& _w2)
{
	int x, y;
	bool accept = false,  done = false;
	int outcodeOut;
	int outcode0 = ClipOutCode(v1.x, v1.y, _w1.x, _w2.x, _w1.y, _w2.y);
	int outcode1 = ClipOutCode(v2.x, v2.y, _w1.x, _w2.x, _w1.y, _w2.y);

	do
	{
		if(outcode0 == 0 && outcode1 == 0)
		{
			accept = true; done = true;
		}
		else if((outcode0 & outcode1) != 0)
			done = true;
		else
		{
			if(outcode0)
				outcodeOut = outcode0;
			else 
				outcodeOut = outcode1;

			if(TOP & outcodeOut)
			{
				x = v1.x + (v2.x - v1.x)*(_w2.y - v1.y)/(v2.y - v1.y);
				y = _w2.y;
			}
			else if(BOTTOM & outcodeOut)
			{
				x = v1.x + (v2.x - v1.x)*(_w1.y - v1.y)/(v2.y - v1.y);
				y = _w1.y;
			}
			if(RIGHT & outcodeOut)
			{
				y = v1.y + (v2.y - v1.y)*(_w2.x - v1.x)/(v2.x - v1.x);
				x = _w2.x;
			}
			else if(LEFT & outcodeOut)
			{
				y = v1.y + (v2.y - v1.y)*(_w1.x - v1.x)/(v2.x - v1.x);
				x = _w1.x;
			}

			if(outcodeOut = outcode0)
			{
				v1.x = x; v1.y = y; outcode0 = ClipOutCode(x,y,_w1.x, _w2.x, _w1.y, _w2.y);
			}
			else
			{
				v2.x = x; v2.y = y; outcode1 = ClipOutCode(x,y,_w1.x, _w2.x, _w1.y, _w2.y);
			}
		}
	}
	while(!done);

	return accept;
}

