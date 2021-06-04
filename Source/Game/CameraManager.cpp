#include "StdAfx.h"
#include "CameraManager.h"
#include "Config.h"
#include "Controls.h"
#include "Terra.h"
#include "Runtime.h"
#include "SafeMath.h"
#include "Universe.h"
#include "GameShell.h"

terCameraType* terCamera = NULL;

void SetCameraPosition(cCamera *UCamera,const MatXf& Matrix)
{
	MatXf ml=MatXf::ID;
	ml.rot()[2][2]=-1;

	MatXf mr=MatXf::ID;
	mr.rot()[1][1]=-1;
	MatXf CameraMatrix;
	CameraMatrix=mr*ml*Matrix;

	UCamera->SetPosition(CameraMatrix);
}

//-------------------------------------------------------------------

CameraCoordinate::CameraCoordinate(const Vect2f& position, float psi, float theta, float distance)
:	position_(to3D(position, 0)),
	psi_(psi), 
	theta_(theta),
	distance_(distance)
{
	//check();
}

CameraCoordinate CameraCoordinate::operator*(float t) const
{
	return CameraCoordinate(position()*t, psi()*t, theta()*t, distance()*t);
}

CameraCoordinate CameraCoordinate::operator+(const CameraCoordinate& coord) const
{
	return CameraCoordinate(position() + coord.position(), psi() + coord.psi(), theta() + coord.theta(), distance() + coord.distance());
}

void CameraCoordinate::uncycle(const CameraCoordinate& coord0)
{
	psi_ = ::uncycle(psi_, coord0.psi(), 2*M_PI);
	theta_ = ::uncycle(theta_, coord0.theta(), 2*M_PI);
}

void CameraCoordinate::interpolate(const CameraCoordinate& coord0, const CameraCoordinate& coord1, float t)
{
	position_.interpolate(coord0.position(), coord1.position(), t);
	//psi_ = coord0.psi() + (coord1.psi() - coord0.psi())*t;
	psi_ = cycle(coord0.psi() + getDist(coord1.psi(), coord0.psi(), 2*M_PI)*t, 2*M_PI);
	theta_ = cycle(coord0.theta() + getDist(coord1.theta(), coord0.theta(), 2*M_PI)*t, 2*M_PI);
	distance_ = coord0.distance() + (coord1.distance() - coord0.distance())*t;
}

void CameraCoordinate::interpolateHermite(const CameraCoordinate coords[4], float u)
{
	float t2 = u*u;
	float t3 = u*t2;
	*this = coords[3]*((-t2+t3)/2.0f) + coords[0]*(-u/2.0+t2-t3/2.0) + coords[2]*(2.0*t2-3.0/2.0*t3+u/2.0) + coords[1]*(1.0-5.0/2.0*t2+3.0/2.0*t3);
}

void CameraCoordinate::check(bool restricted)
{
	float z = FieldCluster::ZeroGround;//(float)(vMap.GetAlt(vMap.XCYCL(round(position().x)),vMap.YCYCL(round(position().y))) >> VX_FRACTION);
	float zm = 100;
	
	position_.z = z;
	
	if(distance() < CAMERA_ZOOM_TERRAIN_THRESOLD1)
		position_.z = z;
	else if(distance() > CAMERA_ZOOM_TERRAIN_THRESOLD2)
		position_.z = zm;
	else{
		float t = (distance() - CAMERA_ZOOM_TERRAIN_THRESOLD1)/(CAMERA_ZOOM_TERRAIN_THRESOLD2 - CAMERA_ZOOM_TERRAIN_THRESOLD1);
		position_.z = z + t*(zm - z);
	}
	
	float scroll_border = (distance() - CAMERA_ZOOM_MIN)/(CAMERA_ZOOM_MAX - CAMERA_ZOOM_MIN)*CAMERA_WORLD_SCROLL_BORDER;
	position_.x = clamp(position().x, scroll_border, vMap.H_SIZE - scroll_border);
	position_.y = clamp(position().y, scroll_border, vMap.V_SIZE - scroll_border);
	position_.z = FieldCluster::ZeroGround;

	if(restricted){
		distance_ = clamp(distance(), CAMERA_ZOOM_MIN, CAMERA_ZOOM_MAX);
		//максимально допустимый наклон на данной высоте
		//  линейно от CAMERA_THETA_MIN на CAMERA_ZOOM_MIN
		//          до CAMERA_THETA_MAX на CAMERA_ZOOM_MAX
		float t = 1 - (distance() - CAMERA_ZOOM_MIN)/(CAMERA_ZOOM_MAX - CAMERA_ZOOM_MIN);
		float theta_max = CAMERA_THETA_MIN + t*(CAMERA_THETA_MAX - CAMERA_THETA_MIN);

		theta_ = clamp(theta(), 0, theta_max);

//		distance_ = clamp(distance(), CAMERA_ZOOM_MIN, CAMERA_ZOOM_MAX);
	}
	else
		distance_ = clamp(distance(), 100, 10000);
	
	//psi_ = cycle(psi(), 2*M_PI);
}

int CameraCoordinate::height()
{
	return vMap.IsFullLoad() ? vMap.GetAlt(vMap.XCYCL(round(position().x)),vMap.YCYCL(round(position().y))) >> VX_FRACTION : 0;
}

//-------------------------------------------------------------------

terCameraType::terCameraType(cCamera* camera)
: coordinate_(Vect2f(500, 500), 0, 0, 300)
{
	Camera = camera;
	
	cameraMouseZoom = false;

	setFocus(HardwareCameraFocus);
	
	matrix_ = MatXf::ID;
//	focus_ = 1.0f;

	restricted_ = false;

	tilting_ = 0;

	replayIndex_ = -1;
	replayIndexMax_ = 0;
	interpolationTimer_ = 0;

	unit_follow = 0;
	memset(cameraSavePoints, 0, sizeof(void*)*5);
	
	cameraPsiVelocity = cameraPsiForce = 0;
	cameraThetaVelocity = cameraThetaForce = 0;
	cameraZoomVelocity = cameraZoomForce = 0;
	cameraPositionVelocity = cameraPositionForce = Vect3f::ZERO;

	explodingDuration_ = 0;
	explodingFactor_ = 0;

	update();
}

terCameraType::~terCameraType()
{
	Camera->Release();
}


void terCameraType::setFocus(float focus)
{
	focus_ = focus;
	Camera->SetAttr(ATTRCAMERA_PERSPECTIVE);
	SetFrustumGame();
	update();
}

void terCameraType::update()
{
	Vect3f position;
	position.setSpherical(coordinate().psi(), coordinate().theta(), coordinate().distance());
	position += coordinate().position();
	if(oscillatingTimer_()){
		float t = (float)(explodingDuration_ - oscillatingTimer_())/1000;
		position.x += explodingFactor_*cameraExplodingPrm.x(t);
		position.y += explodingFactor_*cameraExplodingPrm.y(t);
		position.z += explodingFactor_*cameraExplodingPrm.z(t);
	}
	if(restricted())
		position.z = clamp(position.z, CAMERA_MIN_HEIGHT, CAMERA_MAX_HEIGHT);
	else
		position.z = clamp(position.z, coordinate().height(), 10000);

	matrix_ = MatXf::ID;
	matrix_.rot() = Mat3f(coordinate().theta(), X_AXIS)*Mat3f(M_PI/2 - coordinate().psi(), Z_AXIS);
	matrix_ *= MatXf(Mat3f::ID, -position);	

	SetCameraPosition(Camera, matrix_);
}

void terCameraType::SetFrustumGame()
{
	Camera->SetFrustum(								// устанавливается пирамида видимости
		&Vect2f(0.5f,0.5f),							// центр камеры
		&sRectangle4f(-0.5f,-0.5f,0.5f,0.28125f),		// видимая область камеры
		&Vect2f(focus_,focus_),						// фокус камеры
		&Vect2f(30.0f,10000.0f)						// ближайший и дальний z-плоскости отсечения
		);
}

void terCameraType::SetFrustumMenu()
{
	Camera->SetFrustum(								// устанавливается пирамида видимости
		&Vect2f(0.5f,0.5f),							// центр камеры
		&sRectangle4f(-0.5f,-0.5f,0.5f,0.5f),		// видимая область камеры
		&Vect2f(focus_,focus_),						// фокус камеры
		&Vect2f(30.0f,10000.0f)						// ближайший и дальний z-плоскости отсечения
		);
}

void terCameraType::SetFrustumCutScene()
{
	Camera->SetFrustum(															// устанавливается пирамида видимости
		&Vect2f(0.5f,0.5f),														// центр камеры
		&sRectangle4f(-0.5f,CUT_SCENE_TOP,0.5f,CUT_SCENE_BOTTOM),				// видимая область камеры
		&Vect2f(focus_,focus_),													// фокус камеры
		&Vect2f(30.0f,10000.0f)													// ближайший и дальний z-плоскости отсечения
		);
}

void terCameraType::calcRayIntersection(float x,float y,Vect3f& v0,Vect3f& v1)
{
	Camera->ConvertorCameraToWorld(&v1,&Vect2f(x,y));
	if(Camera->GetAttr(ATTRCAMERA_PERSPECTIVE)){
		MatXf matrix;
		Camera->GetPosition(&matrix);
		v0 = matrix.invXformVect(matrix.trans(),v0);
		v0.negate();	
	}else{
		v0.x = v1.x;
		v0.y = v1.y;
		v0.z = v1.z + 10.0f;
	}

	Vect3f dir = v1 - v0;
	float m = 9999.9f/dir.norm();
	dir *= m;
	v1 = v0 + dir;	
}


//---------------------------------------------------
void terCameraType::controlQuant()
{
	if (gameShell->isCutSceneMode()) {
		return;
	}
	if(interpolationTimer_)
		return;
	
//	cameraMouseZoom = isPressed(VK_LBUTTON) && isPressed(VK_RBUTTON);
	
	if(!unit_follow){
		if(g_controls_converter.key(CTRL_CAMERA_MOVE_DOWN).pressed())
			cameraPositionForce.y += CAMERA_SCROLL_SPEED_DELTA;
		
		if(g_controls_converter.key(CTRL_CAMERA_MOVE_UP).pressed())
			cameraPositionForce.y -= CAMERA_SCROLL_SPEED_DELTA;
		
		if(g_controls_converter.key(CTRL_CAMERA_MOVE_RIGHT).pressed())
			cameraPositionForce.x += CAMERA_SCROLL_SPEED_DELTA;
		
		if(g_controls_converter.key(CTRL_CAMERA_MOVE_LEFT).pressed())
			cameraPositionForce.x -= CAMERA_SCROLL_SPEED_DELTA;
	}
	
	if(g_controls_converter.key(CTRL_CAMERA_ROTATE_UP).pressed())
		cameraThetaForce -= CAMERA_KBD_ANGLE_SPEED_DELTA;
	
	if(g_controls_converter.key(CTRL_CAMERA_ROTATE_DOWN).pressed())
		cameraThetaForce += CAMERA_KBD_ANGLE_SPEED_DELTA;
	
	if(g_controls_converter.key(CTRL_CAMERA_ROTATE_LEFT).pressed())
		cameraPsiForce += CAMERA_KBD_ANGLE_SPEED_DELTA;
	
	if(g_controls_converter.key(CTRL_CAMERA_ROTATE_RIGHT).pressed())
		cameraPsiForce -= CAMERA_KBD_ANGLE_SPEED_DELTA;
	
	if(g_controls_converter.key(CTRL_CAMERA_ZOOM_INC).pressed())
		cameraZoomForce	 -= CAMERA_ZOOM_SPEED_DELTA;
	
	if(g_controls_converter.key(CTRL_CAMERA_ZOOM_DEC).pressed())
		cameraZoomForce	 += CAMERA_ZOOM_SPEED_DELTA;
}

void terCameraType::mouseQuant(const Vect2f& mousePos)
{
	if (gameShell->isCutSceneMode()) {
		return;
	}
	if(interpolationTimer_ || unit_follow)
		return;

	if(fabs(mousePos.x + 0.5f) < CAMERA_BORDER_SCROLL_AREA_HORZ){
		if(coordinate().position().x > 0)
			cameraPositionForce.x -= CAMERA_BORDER_SCROLL_SPEED_DELTA;
	}
	else if(fabs(mousePos.x - 0.5f) < CAMERA_BORDER_SCROLL_AREA_HORZ){
		if(coordinate().position().x < vMap.H_SIZE) 
			cameraPositionForce.x += CAMERA_BORDER_SCROLL_SPEED_DELTA;
	}
	
	if(fabs(mousePos.y + 0.5f) < CAMERA_BORDER_SCROLL_AREA_UP){
		if(coordinate().position().y > 0)
			cameraPositionForce.y -= CAMERA_BORDER_SCROLL_SPEED_DELTA;
	}
	else if(fabs(mousePos.y - 0.5f) < CAMERA_BORDER_SCROLL_AREA_DN){
		if(coordinate().position().y < vMap.V_SIZE) 
			cameraPositionForce.y += CAMERA_BORDER_SCROLL_SPEED_DELTA;
	}
}

void terCameraType::tilt(const Vect2f& mouseDelta)
{
	if (gameShell->isCutSceneMode()) {
		return;
	}
	if(fabs(mouseDelta.x) > fabs(mouseDelta.y)){
		cameraPsiForce += mouseDelta.x*CAMERA_MOUSE_ANGLE_SPEED_DELTA;
		tilting_ = 0;
	}
	else{
		cameraThetaForce -= mouseDelta.y*CAMERA_MOUSE_ANGLE_SPEED_DELTA;
		tilting_ = 1;
	}
}

bool terCameraType::cursorTrace(const Vect2f& pos2, Vect3f& v)
{
	Vect3f pos,dir;
	GetCamera()->GetWorldRay(pos2, pos, dir);
	return terScene->Trace(pos,pos+dir,&v);
}

void terCameraType::shift(const Vect2f& mouseDelta)
{
	if (gameShell->isCutSceneMode()) {
		return;
	}
	if(interpolationTimer_ || unit_follow)
		return;

	Vect2f delta = mouseDelta;
	Vect3f v1, v2;
	if(cursorTrace(Vect2f::ZERO, v1) && cursorTrace(delta, v2))
		delta = v2 - v1; 
	else
		delta = Vect2f::ZERO;
	
	coordinate().position() -= to3D(delta, 0);
}

void terCameraType::mouseWheel(int delta)
{
	if (gameShell->isCutSceneMode()) {
		return;
	}
	if(delta > 0)
		cameraZoomForce -= CAMERA_ZOOM_SPEED_DELTA;
	else if(delta < 0)
		cameraZoomForce += CAMERA_ZOOM_SPEED_DELTA;
}

void terCameraType::quant(float mouseDeltaX, float mouseDeltaY, float delta_time)
{
	if(interpolationTimer_){
		float t = (frame_time() - interpolationTimer_)/(float)interpolationDuration_;
		if(t >= 1){
			if(replayIndex_ != -1){
				if(++replayIndex_ < replayIndexMax_){
					setPath(replayIndex_);
					t = 0;
				}
				else{
					stopReplayPath();
				}
			}
			else{
				interpolationTimer_ = 0;
				t = 1;
			}
		}
		coordinate_.interpolateHermite(interpolationPoints_, t);
		coordinate().check(false);
	}
	else{
		if(unit_follow)
			QuantCameraFollow(delta_time);

		//зум вслед за мышью
		if(cameraMouseZoom)
			cameraZoomForce += mouseDeltaY*CAMERA_ZOOM_MOUSE_MULT;
		
		//zoom
		cameraZoomVelocity += cameraZoomForce*CAMERA_ZOOM_SPEED_MASS;
		coordinate().distance() += cameraZoomVelocity*delta_time;
		
		if(restricted()){
			//if(!cameraMouseTrack){
			//при зуме камера должна принимать макс. допустимый наклон
//			if(fabs(cameraZoomVelocity) > 1.0f)
			if(cameraZoomVelocity < -1.0f)
				cameraThetaForce += CAMERA_KBD_ANGLE_SPEED_DELTA;
			//}
		}
		
		//move
		cameraPositionVelocity += cameraPositionForce*CAMERA_SCROLL_SPEED_MASS;
		
		float d = coordinate().distance()/CAMERA_MOVE_ZOOM_SCALE*delta_time;
		coordinate().position() += Mat3f(-M_PI/2 + coordinate().psi(), Z_AXIS)*cameraPositionVelocity*d;
		
		//rotate
		cameraPsiVelocity   += cameraPsiForce*CAMERA_ANGLE_SPEED_MASS;
		cameraThetaVelocity += cameraThetaForce*CAMERA_ANGLE_SPEED_MASS;
		
		coordinate().psi()   += cameraPsiVelocity*delta_time;
		coordinate().theta() += cameraThetaVelocity*delta_time;
		
		if(delta_time > 1.0f){
			cameraPsiVelocity   *= CAMERA_ANGLE_SPEED_DAMP/delta_time;
			cameraThetaVelocity *= CAMERA_ANGLE_SPEED_DAMP/delta_time;
			cameraZoomVelocity  *= CAMERA_ZOOM_SPEED_DAMP/delta_time;
			
			cameraPositionVelocity *= CAMERA_SCROLL_SPEED_DAMP/delta_time;
		}
		else{
			cameraPsiVelocity   *= CAMERA_ANGLE_SPEED_DAMP;
			cameraThetaVelocity *= CAMERA_ANGLE_SPEED_DAMP;
			cameraZoomVelocity  *= CAMERA_ZOOM_SPEED_DAMP;
			
			cameraPositionVelocity *= CAMERA_SCROLL_SPEED_DAMP;
		}
		
		cameraZoomForce = cameraThetaForce = cameraPsiForce = 0;
		cameraPositionForce = Vect3f::ZERO;
		
		coordinate().check(restricted());
	}

	update();
}

void terCameraType::setTarget(const CameraCoordinate& coord, int duration) 
{ 
	interpolationPoints_[0] = interpolationPoints_[1] = coordinate_;
	interpolationPoints_[2] = interpolationPoints_[3] = coord;
	interpolationTimer_ = frame_time();
	interpolationDuration_ = duration; 
}

void terCameraType::QuantCameraFollow(float delta_time)
{
	coordinate().position() += (unit_follow->position() - coordinate().position())*CAMERA_FOLLOW_AVERAGE_TAU*unitFollowTimer_();
}

void terCameraType::SaveCamera(int n)
{
	xassert(n < 5);
	
	if(!cameraSavePoints[n])
		cameraSavePoints[n] = new CameraCoordinate;
	
	*cameraSavePoints[n] = CameraCoordinate(coordinate().position(), coordinate().psi(), coordinate().theta(), coordinate().distance());
}

void terCameraType::RestoreCamera(int n)
{
	xassert(n < 5);
	
	if(!cameraSavePoints[n])
		return;
	
	setTarget(*cameraSavePoints[n], 1000);
}

void terCameraType::SetCameraFollow(terUnitBase* unit, int transitionTime)
{
	unit_follow = unit;
	unitFollowTimer_.start(transitionTime + 1);
}

void terCameraType::destroyLink()
{
	if(unit_follow && (!unit_follow->alive() 
	  || unit_follow->attr().ID == UNIT_ATTRIBUTE_SQUAD && safe_cast<terUnitSquad*>(unit_follow)->Empty())){
		SetCameraFollow(0);
	}
}

void terCameraType::startReplayPath(int duration, int cycles)
{
	interpolationDuration_ = duration; 

	if(path_.empty())
		return;
	
	if(path_.size() == 1){
		if(!duration){
			setCoordinate(path_.front());
			return;
		}
		else{
			CameraCoordinate coord = path_.front();
			path_.push_back(coord);
		}
	}

	replayIndexMax_ = cycles*path_.size() - 1;
	setPath(replayIndex_ = 0);
}

void terCameraType::stopReplayPath()
{
	replayIndex_ = -1;
	interpolationTimer_ = 0;
}

void terCameraType::setPath(int index) 
{ 
	xassert(!path_.empty());
	index %= path_.size();
	if(replayIndexMax_ > path_.size()){ // Зацикленное повторение последовательности
		interpolationPoints_[0] = path_[(index - 1 + path_.size()) % path_.size()];
		interpolationPoints_[1] = path_[index];
		interpolationPoints_[2] = path_[(index + 1) % path_.size()];
		interpolationPoints_[3] = path_[(index + 2) % path_.size()];
	}
	else{
		interpolationPoints_[0] = path_[clamp(index - 1, 0, path_.size() - 1)];
		interpolationPoints_[1] = path_[index];
		interpolationPoints_[2] = path_[clamp(index + 1, 0, path_.size() - 1)];
		interpolationPoints_[3] = path_[clamp(index + 2, 0, path_.size() - 1)];
	}

	interpolationTimer_ = frame_time();
}

void terCameraType::erasePath() 
{ 
	stopReplayPath();
	path_.clear(); 
	coordinate().psi() = cycle(coordinate().psi(), 2*M_PI);
}

void terCameraType::savePath(SaveCameraSplineData& data)
{
	if(!path_.empty()){
		CoordList::iterator ci;
		FOR_EACH(path_, ci){
			data.path.push_back(SaveCameraData());
			ci->save(data.path.back());
		}
	}
	else{
		data.path.push_back(SaveCameraData());
		coordinate().save(data.path.back());
	}
}

void terCameraType::loadPath(const SaveCameraSplineData& data, bool addCurrentPosition)
{
//	xassert(!isPlayingBack());
	path_.clear();
	if(addCurrentPosition){
		CameraCoordinate coord = coordinate();
		CameraCoordinate coord0;
		coord0.load(data.path.front());
		coord.uncycle(coord0);
		path_.push_back(coord);
	}
	vector<SaveCameraData>::const_iterator i;
	FOR_EACH(data.path, i){
		path_.push_back(CameraCoordinate());
		path_.back().load(*i);
	}
}

void CameraCoordinate::save(SaveCameraData& data) const 
{
	data.position = position_;
	data.psi = psi_;
	data.theta = theta_;
	data.distance = distance_;
}

void CameraCoordinate::load(const SaveCameraData& data)
{
	position_ = to3D(data.position, 0);
	psi_ = data.psi;
	theta_ = data.theta;
	distance_ = data.distance;
}


//-----------------------------------
void terCameraType::startOscillation(int duration, float factor)
{
	explodingFactor_ = factor;
	oscillatingTimer_.start(explodingDuration_ = duration);
}

void terCameraType::reset()
{
	oscillatingTimer_.stop();
	stopReplayPath();
}


