#include "stdafx.h"
#include "BGScene.h"
#include "GameShellSq.h"

BGScene::BGScene() {
	scene = 0;
	camera = 0;
	light = 0;
	bgObj = 0;
	timer = 0;
	liveTimer = 0;
	enabled = true;
}

BGScene::~BGScene() {
	done();
}

void BGScene::init(cVisGeneric* visGeneric) {
	if (ready()) {
		done();
	}
	scene = visGeneric->CreateScene();

	camera = scene->CreateCamera();
	camera->SetAttr(ATTRCAMERA_PERSPECTIVE);
	camera->SetAttr(ATTRCAMERA_CLEARZBUFFER);

	camera->SetFrustum
		(                     
			&Vect2f(0.5f, 0.5f),						// центр камеры
			&sRectangle4f(-0.5f, -0.5f, 0.5f, 0.5f),	// видимая область камеры
			&Vect2f(1.0f, 1.0f),						// фокус камеры
			&Vect2f(10.0f, 10000.0f)					// ближайший и дальний z-плоскости отсечения
		);


	Vect3f pos;
	pos.setSpherical(bgCameraPsi, bgCameraTheta, bgCameraDist);

	pos += Vect3f(bgCameraX, bgCameraY, bgCameraZ);

	MatXf matrix = MatXf::ID;
	matrix.rot() = Mat3f(bgCameraTheta, X_AXIS) * Mat3f(M_PI / 2 - bgCameraPsi, Z_AXIS);
	matrix *= MatXf(Mat3f::ID, -pos);	

	MatXf ml = MatXf::ID;
	ml.rot()[2][2] = -1;
	ml.rot()[1][1] = -1;
	MatXf cameraMatrix;
	cameraMatrix = ml * matrix;

	camera->SetPosition(cameraMatrix);

	
	light = scene->CreateLight(ATTRLIGHT_DIRECTION);
	light->SetPosition( MatXf(Mat3f::ID, Vect3f(0, 0, 0)) );
	light->SetColor( &sColor4f(1, 1, 1, 1), &sColor4f(1, 1, 1, 1) );
	light->SetDirection( Vect3f(bgLightX, bgLightY, bgLightZ) );

	bgObj = scene->CreateObject("RESOURCE\\Models\\Menu\\interface.M3D", NULL);
	setSkinColor();
}

void BGScene::onResolutionChanged() {
	if (camera) {
		camera->Update();
	}
}

void BGScene::done() {
	reset();
	RELEASE(light);
	RELEASE(camera);
	RELEASE(bgObj);
	RELEASE(scene);
}

bool BGScene::ready() const {
	return (enabled && inited());
}

void BGScene::quant(float dt) {
	if (timer != 0) {
		timer -=dt;
		if (timer <= 0) {
			timer = 0;
		}
		for (int i = 0, s = subObjects.size(); i < s; i++) {
			if (!subObjects[i].stopped) {
				subObjects[i].node->SetPhase( subObjects[i].forwardDirection ? (1.0f - timer / bgEffectTime) : (timer / bgEffectTime), false );
			}
		}
		if (timer == 0) {
			vector<SubObject>::iterator it = subObjects.begin();
			while (it != subObjects.end()) {
				if ((*it).forwardDirection) {
					(*it).stopped = true;
					it++;
				} else {
					if (!(*it).stopped) {
						it = subObjects.erase(it);
					} else {
						it++;
					}
				}
			}
		}
	}

	if (liveTimer != 0) {
		liveTimer -=dt;
		if (liveTimer< 0) {
			liveTimer= 0;
		}
		for (int i = 0, s = subObjects.size(); i < s; i++) {
			if (subObjects[i].liveGroupNode) {
				subObjects[i].liveGroupNode->SetPhase( 1.0f - liveTimer / bgEffectTime, false );
			}
		}
	}
	if (liveTimer == 0) {
		liveTimer = bgEffectTime;
	}

	scene->dSetTime(dt);
}

void BGScene::markAllToPlay(bool forward) {
	for (int i = 0, s = subObjects.size(); i < s; i++) {
		subObjects[i].stopped = false;
		subObjects[i].forwardDirection = forward;
	}
}

void BGScene::unmarkToPlay(const char* objName, const char* chainName, bool forward) {
	int i;
	int s;
	for (i = 0, s = subObjects.size(); i < s; i++) {
		if ( 
				!strcmp(objName, subObjects[i].node->GetName())
			&&	subObjects[i].chainName == chainName) {

			break;
		}
	}
	if (i != s) {
		subObjects[i].stopped = true;
		subObjects[i].forwardDirection = forward;
	}
}

void BGScene::markToPlay(const char* objName, const char* chainName, bool forward) {
	int i;
	int s;
	for (i = 0, s = subObjects.size(); i < s; i++) {
		if ( 
				!strcmp(objName, subObjects[i].node->GetName())
			&&	subObjects[i].chainName == chainName) {

			break;
		}
	}
	if (i == s) {
		subObjects.push_back(SubObject());
		subObjects.back().stopped = false;
		subObjects.back().forwardDirection = forward;
		subObjects.back().node = bgObj->FindObject(objName);
		subObjects.back().liveGroupNode = subObjects.back().node->FindObject("group live");
		if (subObjects.back().liveGroupNode) {
			subObjects.back().liveGroupNode->SetChannel("live", false);
		}
		subObjects.back().chainName = chainName;
		subObjects.back().node->SetChannel(chainName, false);
	} else if (forward != subObjects[i].forwardDirection) {
		subObjects[i].stopped = false;
		subObjects[i].forwardDirection = forward;
	}
}

bool BGScene::isPlaying() const {
	return (timer != 0);
}

void BGScene::play() {
	for (int i = 0, s = subObjects.size(); i < s; i++) {
		if (!subObjects[i].stopped) {
			timer = bgEffectTime;
			return;
		}
	}
}

void BGScene::reset() {
	subObjects.clear();
	timer = 0;
}

void BGScene::preDraw() {
	scene->PreDraw(camera);
}

void BGScene::draw() {
	scene->Draw(camera);
}

void BGScene::postDraw() {
	scene->PostDraw(camera);
}

void BGScene::setProgress(float progress) {
	cObjectNode* node = bgObj->FindObject("group progress");
	node->SetChannel("progress", false);
	node->SetPhase( progress, false );
}
