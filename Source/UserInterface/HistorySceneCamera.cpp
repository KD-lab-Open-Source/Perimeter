#include "stdafx.h"
#include "HistorySceneCamera.h"
#include "SafeMath.h"
#include "Controls.h"
#include "Config.h"
#include "GameShellSq.h"
#include "History3D.h"

#include "GameShell.h"

CameraPosition::CameraPosition(
		const Vect3f& pivotPosition,
		float psi,
		float theta,
		float distanceToPivot )

	:	pivotPosition(pivotPosition),
		psi(psi), 
		theta(theta),
		distanceToPivot(distanceToPivot) {
}

CameraPosition CameraPosition::operator*(float t) const {
	return CameraPosition(pivotPosition * t, psi * t, theta * t, distanceToPivot * t);
}

CameraPosition CameraPosition::operator+(const CameraPosition& pos) const {
	return CameraPosition(pivotPosition + pos.pivotPosition, psi + pos.psi, theta + pos.theta, distanceToPivot + pos.distanceToPivot);
}

void CameraPosition::interpolate(const CameraPosition& pos0, const CameraPosition& pos1, float t, CameraPosition* res) {
	res->pivotPosition.interpolate(pos0.pivotPosition, pos1.pivotPosition, t);
	res->psi = pos0.psi + (pos1.psi - pos0.psi) * t;
	res->theta = cycle(pos0.theta + getDist(pos1.theta, pos0.theta, 2 * M_PI) * t, 2 * M_PI);
	res->distanceToPivot = pos0.distanceToPivot + (pos1.distanceToPivot - pos0.distanceToPivot) * t;
}

void CameraPosition::interpolateHermite(const CameraPosition positions[4], float u, CameraPosition* res) {
	float t2 = u * u;
	float t3 = u * t2;
	*res =
			positions[3] * ( (-t2 + t3) / 2.0f )
		+	positions[0] * ( -u / 2.0f + t2-t3 / 2.0f )
		+	positions[2] * ( 2.0f * t2 - 3.0f / 2.0f * t3 + u / 2.0f )
		+	positions[1] * ( 1.0f - 5.0f / 2.0f * t2 + 3.0f / 2.0f * t3 );
}

//-----------------------------
extern SyncroTimer frame_time;

HistorySceneCamera::HistorySceneCamera(Controller* controller) : controller(controller) {
	camera = 0;
	reset();
}

HistorySceneCamera::~HistorySceneCamera() {
	if (camera) {
		camera->Release();
	}
}

void HistorySceneCamera::setCamera(cCamera* newCamera) {
	if (camera) {
		camera->Release();
	}
	camera = newCamera;
	if(newCamera)
	{
		camera->IncRef();
		setup();
		update();
	}
}

void HistorySceneCamera::reset() {
	flyStepDuration = HISTORY_CAMERA_FLY_TIME;
	flyStepTimeElapsed = frame_time();

	currentStep = 0;
	isPlaying = false;
	isFree = true;
	isLoopedPlaying = true;
	isFirstPointFromPath = true;
	shouldClearAtEnd = false;
	followNomadMode = false;
	tracking = false;
//	position = CameraPosition(Vect3f(0, 0, 0), 6.209f + M_PI, -1.516f - M_PI / 2.0f, 2000*HISTORY_SCENE_SCALE);
	position = CameraPosition(
			Vect3f(HISTORY_SCENE_CENTER_X * HISTORY_SCENE_SCALE, HISTORY_SCENE_CENTER_Y * HISTORY_SCENE_SCALE, -HISTORY_SCENE_CENTER_Z * HISTORY_SCENE_SCALE),
			HISTORY_SCENE_PSI + M_PI, -HISTORY_SCENE_THETA - M_PI / 2.0f, HISTORY_SCENE_DISTANCE * HISTORY_SCENE_SCALE );
	path.clear();
	waitingList.clear();
}

void HistorySceneCamera::setup() {
	camera->SetAttr(ATTRCAMERA_PERSPECTIVE);

	camera->SetFrustum
		(                     
			&Vect2f(0.5f, 0.5f),						// центр камеры
			&sRectangle4f(-0.5f, -0.5f, 0.5f, 0.5f),	// видимая область камеры
			&Vect2f(1.0f, 1.0f),						// фокус камеры
			&Vect2f(10.0f, 100000.0f)					// ближайшая и дальняя z-плоскости отсечения
		);
}

void HistorySceneCamera::update() {
	Vect3f pos;
	pos.setSpherical(position.psi, position.theta, position.distanceToPivot);

	pos += position.pivotPosition;

	//restrict radius

	MatXf matrix = MatXf::ID;
	matrix.rot() = Mat3f(position.theta, X_AXIS) * Mat3f(M_PI / 2 - position.psi, Z_AXIS);
	matrix *= MatXf(Mat3f::ID, -pos);	

	setMatrixToCamera(camera, matrix);
}

void HistorySceneCamera::setupNextFollowPos(const Vect3f& nomadPosition) {
	nextFollowPos = currentFollowPos;
	nextFollowPos.psi += dPsi;
	nextFollowPos.pivotPosition = nomadPosition;

	nextFollowPos.distanceToPivot = originFollowPos.distanceToPivot;
	nextFollowPos.theta = originFollowPos.theta;
}

void HistorySceneCamera::setFollowNomadModeSettings(float deltaPsi, float radius, float theta) {
	dPsi = deltaPsi;
	originFollowPos.distanceToPivot = radius;
	originFollowPos.theta = theta;
}

void HistorySceneCamera::quant(const Vect2f& mousePos, float dt) {
	lastMousePos = mousePos;
	if (!isFree) {
		gameShell->setSideArrowsVisible(false);
		if (followNomadMode) {
			Frame* nomad = controller->getNomadFrame();
			if (nomad && nomad->getFrame3D()) {
				cObjectNodeRoot* obj = ((Frame3D*) nomad->getFrame3D())->getFrameObj();
				const MatXf& objPos = obj->GetPosition();
				if (!tracking) {
					tracking = true;
					currentFollowPos = position;
					setupNextFollowPos(objPos.trans());
				} else {
					float t = (frame_time() - flyStepTimeElapsed) / flyStepDuration;
					if (t > 1.0f) {
						// advance
						currentFollowPos = position;
						if (t >= 2.0f) {
							t = 0.0f;
							flyStepTimeElapsed = frame_time();
						} else {
							t -= 1.0f;
						}
						flyStepTimeElapsed += flyStepDuration;
						setupNextFollowPos(objPos.trans());
					}
					CameraPosition::interpolate(currentFollowPos, nextFollowPos, t, &position);
				}
			} else {
				tracking = false;
			}
		} else {
			float t = (frame_time() - flyStepTimeElapsed) / flyStepDuration;
			if (t > 1.0f) {
				advance();
				if (t >= 2.0f) {
					t = 0.0f;
					flyStepTimeElapsed = frame_time();
				} else {
					t -= 1.0f;
				}
				flyStepTimeElapsed += flyStepDuration;
			}
			CameraPosition::interpolateHermite(interpolationPoints, t, &position);
		}
	} else {
		gameShell->setSideArrowsVisible(true);
//		if (isPressed(VK_LBUTTON) && isPressed(VK_RBUTTON)) {
//			position.distanceToPivot += mouseDeltaY * CAMERA_ZOOM_MOUSE_MULT;
//		}
		
		if (
				g_controls_converter.key(CTRL_CAMERA_MOVE_UP).pressed()
			||	fabs(mousePos.y + 0.5f) < CAMERA_BORDER_SCROLL_AREA_UP ) {

			position.theta -= HISTORY_CAMERA_ANGLE_SPEED_DELTA * dt;
		}
		
		if (
				g_controls_converter.key(CTRL_CAMERA_MOVE_DOWN).pressed()
			||	fabs(mousePos.y - 0.5f) < CAMERA_BORDER_SCROLL_AREA_DN ) {

			position.theta += HISTORY_CAMERA_ANGLE_SPEED_DELTA * dt;
		}
		
		if (
				g_controls_converter.key(CTRL_CAMERA_MOVE_LEFT).pressed()
			||	fabs(mousePos.x - 0.5f) < CAMERA_BORDER_SCROLL_AREA_HORZ ) {

			position.psi += HISTORY_CAMERA_ANGLE_SPEED_DELTA * dt;
		}
		
		if ( 
				g_controls_converter.key(CTRL_CAMERA_MOVE_RIGHT).pressed()
			||	fabs(mousePos.x + 0.5f) < CAMERA_BORDER_SCROLL_AREA_HORZ ) {

			position.psi -= HISTORY_CAMERA_ANGLE_SPEED_DELTA * dt;
		}
		
		if ( g_controls_converter.key(CTRL_CAMERA_ZOOM_INC).pressed() ) {
			position.distanceToPivot -= CAMERA_ZOOM_SPEED_DELTA * dt;
		}
		
		if ( g_controls_converter.key(CTRL_CAMERA_ZOOM_DEC).pressed() ) {
			position.distanceToPivot += CAMERA_ZOOM_SPEED_DELTA * dt;
		}
		if (position.distanceToPivot < HISTORY_SCENE_CAMERA_MIN_DISTANCE) {
			position.distanceToPivot = HISTORY_SCENE_CAMERA_MIN_DISTANCE;
		}
		if (position.distanceToPivot > HISTORY_SCENE_CAMERA_MAX_DISTANCE) {
			position.distanceToPivot = HISTORY_SCENE_CAMERA_MAX_DISTANCE;
		}
	}
	update();
}

void HistorySceneCamera::mouseWheel(int delta) {
	if (isFree) {
		if (delta > 0) {
			position.distanceToPivot -= CAMERA_ZOOM_SPEED_DELTA * 10;
		} else if (delta < 0) {
			position.distanceToPivot += CAMERA_ZOOM_SPEED_DELTA * 10;
		}
	}
}

void HistorySceneCamera::advance() {
	addLineToLog("advance:" + posToString(position));
	if (!isFirstPointFromPath) {
		addLineToLog("      :!isFirstPointFromPath");
		isFirstPointFromPath = true;
		path.pop_front();
		addLineToLog("      :BEGIN_OF_CAMERA_EVENT");
		controller->eventOccured(Controller::BEGIN_OF_CAMERA_EVENT);
	} else {
		addLineToLog("      :currentStep++");
		currentStep++;
		addLineToLog("      :POINT_OF_CAMERA_EVENT");
		controller->eventOccured(Controller::POINT_OF_CAMERA_EVENT);
	}
	setupInterpolationPoints();
}

void HistorySceneCamera::setupInterpolationPoints() {
	if (isLoopedPlaying) {
		if ( currentStep == path.size() ) {
			currentStep = 0;
		} else if (currentStep == (path.size() - 1)) {
			addLineToLog("setupInterpolationPoints:END_OF_CAMERA_EVENT");
			controller->eventOccured(Controller::END_OF_CAMERA_EVENT);
		}
		for (int i = 0, s = waitingList.size(); i < s; i++) {
			path.push_back(waitingList[0]);
			waitingList.pop_front();
		}
		interpolationPoints[1] = path[currentStep];
		interpolationPoints[2] = path[(currentStep + 1) % path.size()];
		if (path.size() > 2) {
			interpolationPoints[0] = path[(currentStep - 1 + path.size()) % path.size()];
			interpolationPoints[3] = path[(currentStep + 2) % path.size()];
		} else {
			interpolationPoints[0] = interpolationPoints[1];
			interpolationPoints[3] = interpolationPoints[2];
		}
	} else {
		if ( currentStep == path.size() ) {
			int s = waitingList.size();
			if (s) {
				currentStep--;
				for (int i = 0; i < s; i++) {
					path.push_back(waitingList[0]);
					waitingList.pop_front();
				}
			} else {
				currentStep = 0;
				setFree(true);
				controller->eventOccured(Controller::END_OF_CAMERA_EVENT);
				return;
			}
		}
		interpolationPoints[0] = path[ max(currentStep - 1, 0) ];
		interpolationPoints[1] = path[currentStep];
		interpolationPoints[2] = path[ min( currentStep + 1, path.size() - 1) ];
		interpolationPoints[3] = path[ min( currentStep + 2, path.size() - 1) ];
	}
	CameraPosition::correctToShortestPath(interpolationPoints);
}

void HistorySceneCamera::startPlaying() {
	if (isFree) {
		addLineToLog("startPlaying:");
		for (int i = 0, s = waitingList.size(); i < s; i++) {
			path.push_back(waitingList[0]);
			waitingList.pop_front();
		}
		path.push_front(position);
		currentStep = 0;
		isFirstPointFromPath = false;
		setFree(false);
		setupInterpolationPoints();
		flyStepTimeElapsed = frame_time();
	}
}

void HistorySceneCamera::stopPlaying() {
	addLineToLog("stopPlaying:");
	setFree(true);
}

void HistorySceneCamera::addToPath(const CameraPosition& pos) {
	addLineToLog("addToPath:" + posToString(pos));
	waitingList.push_back(pos);
}

void HistorySceneCamera::clearPath() {
	addLineToLog("clearPath:");
	stopPlaying();
	path.clear();
	waitingList.clear();
	currentStep = 0;
}

void HistorySceneCamera::setPositionToBegin() {
	addLineToLog("setPositionToBegin:");
	if (!path.empty()) {
		position = path[isFirstPointFromPath ? 0 : 1];
	} else if (!waitingList.empty()) {
		position = waitingList.front();
	}
}

void HistorySceneCamera::setPositionToEnd() {
	addLineToLog("setPositionToEnd:");
	if (!waitingList.empty()) {
		position = waitingList.back();
	} else if (!path.empty()) {
		position = path.back();
	}
}

void HistorySceneCamera::setPositionToNomad() {
	Frame* nomad = controller->getNomadFrame();
	if (nomad && nomad->getFrame3D()) {
		cObjectNodeRoot* obj = ((Frame3D*) nomad->getFrame3D())->getFrameObj();
		const MatXf& objPos = obj->GetPosition();
		position.pivotPosition = objPos.trans();
		position.distanceToPivot = originFollowPos.distanceToPivot;
		position.theta = originFollowPos.theta;
	}
}

void HistorySceneCamera::setFollowNomadMode(bool follow) {
	setFree(!follow);
	tracking = false;
	flyStepTimeElapsed = frame_time();
	followNomadMode = follow;
}

void HistorySceneCamera::setFree(bool free) {
	isFree = free;
	gameShell->setSideArrowsVisible(isFree);
}

void HistorySceneCamera::setMatrixToCamera(cCamera* camera, const MatXf& matrix) {
	MatXf ml = MatXf::ID;
	ml.rot()[2][2] = -1;
	ml.rot()[1][1] = -1;
	MatXf cameraMatrix;
	cameraMatrix = ml * matrix;

	camera->SetPosition(cameraMatrix);
}

void HistorySceneCamera::onResolutionChanged() {
	if (camera) {
		camera->Update();
	}
}

string HistorySceneCamera::posToString(const CameraPosition& pos) {
	char str[200];
	sprintf(
		str,
		"x=%f y=%f z=%f psi=%f theta=%f dist=%f",
		pos.distanceToPivot/HISTORY_SCENE_SCALE,
		-pos.theta - M_PI / 2.0f,
		pos.psi - M_PI,
		pos.pivotPosition.x*HISTORY_SCENE_SCALE,
		pos.pivotPosition.y*HISTORY_SCENE_SCALE,
		pos.pivotPosition.z*HISTORY_SCENE_SCALE
		);
	return string(str);
}

void HistorySceneCamera::addLineToLog(const string& line) {
/*
	if (log.empty()) {
		log += "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" + line + "\n";
	} else {
		int returnCh = log.find("\n");
		log.erase(0, returnCh + 1);
		log += line + "\n";
	}
*/
}

void HistorySceneCamera::calcRayIntersection(float x, float y, Vect3f& v0, Vect3f& v1) {
	camera->ConvertorCameraToWorld( &v1, &Vect2f(x,y) );
	if ( camera->GetAttr(ATTRCAMERA_PERSPECTIVE) ) {
		MatXf matrix;
		camera->GetPosition(&matrix);
		v0 = matrix.invXformVect(matrix.trans(),v0);
		v0.negate();	
	} else {
		v0.x = v1.x;
		v0.y = v1.y;
		v0.z = v1.z + 10.0f;
	}

	Vect3f dir = v1 - v0;
	float m = 9999.9f / dir.norm();
	dir *= m;
	v1 = v0 + dir;	
}
