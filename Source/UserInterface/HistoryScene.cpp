#include "stdafx.h"
#include "HistoryScene.h"
#include "Runtime.h"
#include "GameShell.h"
#include "GameShellSq.h"
#include "qd_textdb.h"
#include "tx3d.hpp"

extern GameShell* gameShell;
extern cInterfaceRenderDevice* terRenderDevice;
extern cVisGeneric* terVisGeneric;
extern HWND hWndVisGeneric;
extern int terSoundEnable;
extern float terSoundVolume;

extern MpegSound gb_Music;

HistoryScene::HistoryScene(const string& programFileName) {
	scene = 0;
	sceneSky = 0;
	cameraSky = 0;
	lightSky = 0;
	skySphere = 0;

	nomadMarker = 0;
	timer = 0;

	initialMat = MatXf::ID;
	Rotate( initialMat, Vect3f(0, -30, 0) );

	addBlendAlpha = false;

	for (int i = 0; i < WorldSphere::MAX_TYPE; i++) {
		worldTextures1[i] = 0;
		worldTextures2[i] = 0;
	}

	linkSelected = 0;
	linkNormal = 0;
	electric = 0;

	lastEvent = Controller::CONTROL_SUBMIT_EVENT;

	interpreter = new Interpreter(this);
	historyCamera = new HistorySceneCamera(interpreter);
	loadProgram(programFileName.c_str());

	fnt = 0;
	logFnt = 0;
	playingVoice = false;
}

HistoryScene::~HistoryScene() {
	done();
	delete historyCamera;
	delete interpreter;
}

void HistoryScene::loadProgram(const string& fileName) {
	interpreter->setPaused(true);
//	interpreter->loadProgram("RESOURCE\\scenario.hst");
	if (!interpreter->loadProgram(fileName.c_str())) {
		ErrH.Abort( (fileName + " not found!").c_str() );
	}
}
void HistoryScene::start() {
	interpreter->setPaused(false);
}
void HistoryScene::stop() {
	interpreter->setPaused(true);
}

void HistoryScene::goToMission(int missionNumber) {
	interpreter->goToMission(missionNumber);
}

void HistoryScene::goToJustAfterMissionPosition(int missionNumber) {
	interpreter->goToJustAfterMissionPosition(missionNumber);
}

void HistoryScene::resetCamera() {
	historyCamera->reset();
}


void HistoryScene::init(cVisGeneric* visGeneric, bool bw, bool addBlendAlphaMode) {
	if (ready()) {
		done();
	}

	bwMode = bw;

	fnt = terVisGeneric->CreateFont("Arial", HISTORY_SCENE_LOG_FONT_SIZE);
	logFnt = terVisGeneric->CreateFont("Arial", BRIEFING_LOG_FONT_SIZE);
	m_hPopupTexture = terVisGeneric->CreateTexture(sPopupTexture);

	terVisGeneric->EnableOcclusion(false);

	addBlendAlpha = addBlendAlphaMode;

	scene = visGeneric->CreateScene();

	cCamera* c = scene->CreateCamera();
	historyCamera->setCamera(c);
	c->Release();

	sceneSky = visGeneric->CreateScene();

	cameraSky = sceneSky->CreateCamera();
	cameraSky->SetAttr(ATTRCAMERA_PERSPECTIVE);

	cameraSky->SetFrustum
		(                     
			&Vect2f(0.5f, 0.5f),						// центр камеры
			&sRectangle4f(-0.5f, -0.5f, 0.5f, 0.5f),	// видимая область камеры
			&Vect2f(1.0f, 1.0f),						// фокус камеры
			&Vect2f(10.0f, 100000.0f)					// ближайший и дальний z-плоскости отсечения
		);

	lightSky = sceneSky->CreateLight(ATTRLIGHT_DIRECTION);
	lightSky->SetPosition( MatXf(Mat3f::ID, Vect3f(0, 0, 0)) );
	lightSky->SetColor( &sColor4f(1, 1, 1, 1), &sColor4f(1, 1, 1, 1) );
	lightSky->SetDirection( Vect3f(0, 0, -1) );

	if (bw) {
		for (int i = 0; i < WorldSphere::MAX_TYPE; i++) {
			worldTextures1[i] = terVisGeneric->CreateTexture( worldFullTexture1 );
			worldTextures2[i] = terVisGeneric->CreateTexture( worldFullTexture2 );
		}

		linkSelected = GetTexLibrary()->GetElement(
						"RESOURCE\\MODELS\\MENU\\History\\LinkTex.tga",
						"NoMipMap");
		linkNormal = GetTexLibrary()->GetElement(
						"RESOURCE\\MODELS\\MENU\\History\\LinkTex.tga",
						"NoMipMap");
	} else {
		worldTextures1[WorldSphere::UNKNOWN] = terVisGeneric->CreateTexture( worldUnknownTexture1 ); 
		worldTextures2[WorldSphere::UNKNOWN] = terVisGeneric->CreateTexture( worldUnknownTexture2 );
		worldTextures1[WorldSphere::FULL] = terVisGeneric->CreateTexture( worldFullTexture1 );
		worldTextures2[WorldSphere::FULL] = terVisGeneric->CreateTexture( worldFullTexture2 );
		worldTextures1[WorldSphere::ALPHA_EXPEDITION] = terVisGeneric->CreateTexture( worldAExpeditionTexture1 ); 
		worldTextures2[WorldSphere::ALPHA_EXPEDITION] = terVisGeneric->CreateTexture( worldAExpeditionTexture2 );
		worldTextures1[WorldSphere::DEAD_END] = terVisGeneric->CreateTexture( worldDeadEndTexture1 ); 
		worldTextures2[WorldSphere::DEAD_END] = terVisGeneric->CreateTexture( worldDeadEndTexture2 );
		worldTextures1[WorldSphere::VISITED] = terVisGeneric->CreateTexture( worldVisitedTexture1 ); 
		worldTextures2[WorldSphere::VISITED] = terVisGeneric->CreateTexture( worldVisitedTexture2 );
		worldTextures1[WorldSphere::MISSIONED] = terVisGeneric->CreateTexture( worldMissionedTexture1 ); 
		worldTextures2[WorldSphere::MISSIONED] = terVisGeneric->CreateTexture( worldMissionedTexture2 );

		linkSelected = GetTexLibrary()->GetElement(
						"RESOURCE\\MODELS\\MENU\\History\\LinkTex_select.tga",
						"NoMipMap");
		linkNormal = GetTexLibrary()->GetElement(
						"RESOURCE\\MODELS\\MENU\\History\\LinkTex.tga",
						"NoMipMap");
	}

	electric = GetTexLibrary()->GetElement(
			"RESOURCE\\MODELS\\MENU\\History\\electric.tga",
			"NoMipMap");
	createSkySphere(bw);
	createNomadMarker();
}

void HistoryScene::done() {
	stopAudio();

	map <string, World*>::iterator it;
	map <string, World*>::iterator end = interpreter->worlds->worlds.end();
	for (it = interpreter->worlds->worlds.begin(); it != end; it++) {
		World* world = it->second;
		if (world->getWorld3D()) {
			world->setWorld3D(0);
		}
	}
	for (it = interpreter->worlds->worlds.begin(); it != end; it++) {
		World* world = it->second;
		int linkCount = world->getConnectedToOmegaWorldCount();
		for (int i = 0; i < linkCount; i++) {
			if (world->getConnectedToOmegaLink3DByIndex(i)) {
				world->setConnectedToOmegaLink3D(i, 0);
			}
		}
	}
	map <string, Frame*>::iterator itFrames;
	map <string, Frame*>::iterator endFrames = interpreter->frames->frames.end();
	for (itFrames = interpreter->frames->frames.begin(); itFrames != endFrames; itFrames++) {
		Frame* frame = itFrames->second;
		if (frame->getFrame3D()) {
			frame->setFrame3D(0);
		}
	}
	interpreter->clearObjects3D();

	RELEASE(nomadMarker);

	historyCamera->setCamera(0);
	RELEASE(lightSky);
	RELEASE(skySphere);
	RELEASE(cameraSky);
	RELEASE(scene);
	RELEASE(sceneSky);

	for (int i = 0; i < WorldSphere::MAX_TYPE; i++) {
		RELEASE(worldTextures1[i]);
		RELEASE(worldTextures2[i]);
	}

	RELEASE(linkSelected);
	RELEASE(linkNormal);
	RELEASE(electric);

	RELEASE(fnt);
	RELEASE(logFnt);
	RELEASE(m_hPopupTexture);
}

void HistoryScene::quant(const Vect2f& mousePos, float dt) {
	if (!voice.IsPlay() && playingVoice) {
		playingVoice = false;
		audioStopped();
		interpreter->eventOccured(Controller::END_OF_AUDIO_EVENT);
	}

	updateObjects(dt);
//	for (int i = 0; i < 100; i++) {
//		updateObjects(10);
//	}

	historyCamera->quant(mousePos, dt);
	
	MatXf& m = historyCamera->getCamera()->GetMatrix();
	cameraSky->SetPosition(m);

	skySphere->SetPhase(float(sceneSky->GetTime() % 60000) / 60000);

	interpreter->quant(dt);

	sceneSky->dSetTime(dt);
	scene->dSetTime(dt);

}

void HistoryScene::updateObjects(float dt) {
	MatXf mat;
	historyCamera->getCamera()->GetPosition( &mat, 0 );
	Vect3f vPos = mat.rot().invXform( mat.trans(), vPos );

	Frame* nomadFrame = interpreter->getNomadFrame();

	//worlds
	map <string, World*>::iterator it;
	map <string, World*>::iterator end = interpreter->worlds->worlds.end();
	for (it = interpreter->worlds->worlds.begin(); it != end; it++) {
		World* world = it->second;
		if (!world->getWorld3D()) {
			world->setTestamentLevel(0);
			createWorld3D(world, scene);
		}
		bool stateChanged = false;
		WorldSphere* sphere = (WorldSphere*)world->getWorld3D();
		if (world->getTestamentLevel()) {
			sphere->startTestamentEffect(world->getTestamentLevel());
			world->setTestamentLevel(0);
		}
		sphere->resetFrameCount();
		if (world->hasAlphaPortal()) {
			if (world->hasOmegaPortal()) {
				stateChanged = sphere->setState(WorldSphere::FULL);
			} else {
				stateChanged = sphere->setState(WorldSphere::DEAD_END);
			}
		} else {
			stateChanged = sphere->setState(WorldSphere::ALPHA_EXPEDITION);
		}
		if (!nomadFrame || nomadFrame->getKnowledge().knowAboutWorld(world)) {
			sphere->color().a = KNOWN_WORLD_ALPHA;
//			sphere->getElasticSphere()->blendMode = ALPHA_BLEND;
		} else {
			sphere->color().a = UNKNOWN_WORLD_ALPHA;
//			sphere->getElasticSphere()->blendMode = ALPHA_ADDBLENDALPHA;
		}
		if ( stateChanged ) {
			setupWorld3D( sphere );
		}
		if (!world->isAlive()) {
			sphere->setDead();
		}
		sphere->quant(dt);
		sphere->updateColor(vPos);
	}

	//nomad path
	if (interpreter->getNomadFrame()) {
		const vector<World*>& path = interpreter->getNomadFrame()->getKnowledge().getPath();
		for (int i = 0, s = path.size(); i < s; i++) {
			path[i]->getWorld3D()->setState(WorldSphere::VISITED);
			setupWorld3D( (WorldSphere*)path[i]->getWorld3D() );
		}
	}

	//Missioned
	if (interpreter->missionWorlds.size()) {
		for (int i = 0, s = interpreter->missionWorlds.size(); i < s; i++) {
			interpreter->missionWorlds[i]->getWorld3D()->setState(WorldSphere::MISSIONED);
			setupWorld3D( (WorldSphere*)interpreter->missionWorlds[i]->getWorld3D() );
		}
	}

	//frames
	map <string, Frame*>::iterator itFrames;
	map <string, Frame*>::iterator endFrames = interpreter->frames->frames.end();
	for (itFrames = interpreter->frames->frames.begin(); itFrames != endFrames; itFrames++) {
		Frame* frame = itFrames->second;
		if (!frame->getFrame3D()) {
			createFrame3D(frame, scene);
		}
		Frame3D* frame3D = (Frame3D*)frame->getFrame3D();
		setupFrame3D(frame);

		frame3D->quant(dt);
		frame3D->updateColor(vPos);
	}

	//continuos commands
	for (int i = 0, s = interpreter->continuousCommands.size(); i < s; i++) {
		JumpToWorld* jc = dynamic_cast<JumpToWorld*>(interpreter->continuousCommands[i]->command);
		if (jc) {
			setupFrame3D(jc, jc->getWorldCoords(), interpreter->continuousCommands[i], vPos, true);
			continue;
		}
		TransferFrame* tc = dynamic_cast<TransferFrame*>(interpreter->continuousCommands[i]->command);
		if (tc) {
			WorldSphere* world3D = (WorldSphere*) tc->getWorldToTransfer(interpreter->worlds)->getWorld3D();
			MatXf mat = world3D->getElasticSphere()->GetGlobalMatrix();
			setupFrame3D(tc, mat.trans(), interpreter->continuousCommands[i], vPos, false);
			continue;
		}
		ReachWorld* rw = dynamic_cast<ReachWorld*>(interpreter->continuousCommands[i]->command);
		if (rw) {
			WorldSphere* world3D = (WorldSphere*) rw->getWorldToReach(interpreter->worlds)->getWorld3D();
			MatXf mat = world3D->getElasticSphere()->GetGlobalMatrix();
			setupFrame3D(rw, mat.trans(), interpreter->continuousCommands[i], vPos, false);
			continue;
		}
		CreateLink* cc = dynamic_cast<CreateLink*>(interpreter->continuousCommands[i]->command);
		if (cc) {
			Object3D* obj = interpreter->continuousCommands[i]->getObject3D();
			if (obj) {
				if ( obj->setState(LinkTubule::BLINKING) ) {
					setupLink3D( (LinkTubule*)obj );
				}
				obj->quant(dt);
				obj->updateColor(vPos);
			} else {
				createBuildingLink(interpreter->continuousCommands[i], cc->getWorld(interpreter->worlds), cc->getFrame(interpreter->frames)->getCurrentWorld());
			}
			continue;
		}
		CreateAlphaPortal* ca = dynamic_cast<CreateAlphaPortal*>(interpreter->continuousCommands[i]->command);
		if (ca) {
			Object3D* obj = interpreter->continuousCommands[i]->getObject3D();
			if (obj) {
				if ( obj->setState(LinkTubule::BLINKING) ) {
					setupLink3D( (LinkTubule*)obj );
				}
				obj->quant(dt);
				obj->updateColor(vPos);
			} else {
				Frame* curFrame = ca->getFrame(interpreter->frames);
				int size = curFrame->getKnowledge().getPath().size();
				createBuildingLink(interpreter->continuousCommands[i], curFrame->getKnowledge().getPath()[size - 2], curFrame->getCurrentWorld());
			}
			continue;
		}
	}

	updateNomadMarker(vPos, dt);

	//links
	for (it = interpreter->worlds->worlds.begin(); it != end; it++) {
		World* world = it->second;
		int linkCount = world->getConnectedToOmegaWorldCount();
		for (int i = 0; i < linkCount; i++) {
			if (!world->getConnectedToOmegaLink3DByIndex(i)) {
				createLink3D(world, i, scene);
			}
			Object3D* obj = world->getConnectedToOmegaLink3DByIndex(i);
			if (
					world->getWorld3D()->getState() == WorldSphere::VISITED
				&&	world->getConnectedToOmegaWorldByIndex(i)->getWorld3D()->getState() == WorldSphere::VISITED ) {

				if ( obj->setState(LinkTubule::SELECTED) ) {
					setupLink3D( (LinkTubule*)obj );
				}
			} else {
				if ( obj->setState(LinkTubule::NORMAL) ) {
					setupLink3D( (LinkTubule*)obj );
				}
			}
			obj->quant(dt);
			obj->updateColor(vPos);
		}
	}
}

void HistoryScene::createBuildingLink(ContinuousCommand* cc, World* w1, World* w2) {
	WorldSphere* sphere1 = (WorldSphere*)(w1->getWorld3D());
	WorldSphere* sphere2 = (WorldSphere*)(w2->getWorld3D());
	ElasticLink* elasticLink = new ElasticLink(
			sphere1->getElasticSphere(),
			sphere2->getElasticSphere() );
	electric->IncRef();
	elasticLink->SetTexture2( electric );
//	world->setConnectedToOmegaLink3D(linkNumber, new LinkTubule(elasticLink));
	Object3D* obj = new LinkTubule(elasticLink);
	cc->setObject3D(obj);
	elasticLink->blendMode = addBlendAlpha ? ALPHA_ADDBLENDALPHA : ALPHA_BLEND;
	elasticLink->Release();
	scene->AttachObj(elasticLink);
}

void HistoryScene::preDraw() {
	sceneSky->PreDraw(cameraSky);
	scene->PreDraw(historyCamera->getCamera());
}
void HistoryScene::drawPopup() {
	if (bwMode) {
		return;
	}
	terRenderDevice->SetFont(fnt);
	Vect2f mousePos = historyCamera->getMousePos();
	World* w = traceWorld(mousePos);

	POINT pt = { round((mousePos.x + 0.5f) * terRenderDevice->GetSizeX()), round((mousePos.y + 0.5f) * terRenderDevice->GetSizeY()) };
	ClientToScreen(hWndVisGeneric, &pt);

	if (w) {
		string frameNames;
		map <string, Frame*>::iterator it;
		map <string, Frame*>::iterator end = interpreter->frames->frames.end();
		for (it = interpreter->frames->frames.begin(); it != end; it++) {
			Frame* frame = it->second;
			if (frame->getCurrentWorld() == w) {
				if (!frameNames.empty()) {
					frameNames += ", ";
				}
				frameNames += getFrameNameFromBase(frame->getName());
			}
		}
		static char popupTxt[512];
		if (frameNames.empty()) {
			sprintf(popupTxt, qdTextDB::instance().getText("Interface.Tips.ChainWorldEmpty"), w->getName().c_str(), w->getSize(), w->getTimeFactor());
		} else {
			sprintf(popupTxt, qdTextDB::instance().getText("Interface.Tips.ChainWorld"), w->getName().c_str(), frameNames.c_str(), w->getSize(), w->getTimeFactor());
		}

		Vect2f v1;
		Vect2f v2;
		terRenderDevice->OutTextRect(0, 0, popupTxt, -1, v1, v2);

		int pos_x = pt.x - (v2.x - v1.x) - 1;
		int delta_y = pt.y - (v2.y - v1.y);
		terRenderDevice->DrawSprite(pos_x - 2, delta_y, v2.x - v1.x + 2, v2.y - v1.y + 2,
			0, 0, 1, 1, m_hPopupTexture, sColor4c(255,255,255,255));

		terRenderDevice->OutText(pos_x, delta_y, popupTxt, sColor4f(1, 1, 1, 1), -1);
		terRenderDevice->SetFont(0);


		Vect2f va(pos_x - 2, delta_y);
		Vect2f vb(va.x + v2.x - v1.x + 2, va.y + v2.y - v1.y + 2);

		terRenderDevice->DrawRectangle(va.x, va.y, vb.x - va.x, vb.y - va.y, sColor4c(255, 255, 255, 255), true);
		terRenderDevice->FlushPrimitive2D();

	}
}

void HistoryScene::drawSprites() {
/*	Frame* nomad = interpreter->getNomadFrame();
	if (nomad) {
		WorldSphere* world3D = (WorldSphere*) nomad->getCurrentWorld()->getWorld3D();
//		MatXf mat = world3D->getElasticSphere()->GetGlobalMatrix();
		Frame3D* frame3D = (Frame3D*)nomad->getFrame3D();
		MatXf mat = frame3D->getFrameObj()->GetGlobalMatrix();
		mat.rot() = Mat3f::ID;
		Rotate(mat, Vect3f(0, 180, 0));
		Translate(mat, Vect3f(0, 0, world3D->getElasticSphere()->radius));
		nomadMarker->SetPosition(mat);
		float distance = nomadMarker->GetGlobalMatrix().trans().distance(-cameraPos);
		if (distance < 0 || distance >= SPHERE_VISIBLE_DISTANCE) {
			nomadMarker->SetAttr(ATTRUNKOBJ_IGNORE);
		} else {
			nomadMarker->ClearAttr(ATTRUNKOBJ_IGNORE);
			float f = 1.0f - distance / SPHERE_VISIBLE_DISTANCE;
			nomadMarker->SetColor( 0, &sColor4f(1, 1, 1, f) );
		}
		timer += dt;
		nomadMarker->SetPhase(fmod(timer, NOMAD_MARKER_PERIOD) / NOMAD_MARKER_PERIOD, true);
	}
	terRenderDevice->DrawSprite(pos_x - 2, delta_y, v2.x - v1.x + 2, v2.y - v1.y,
		0, 0, 1, 1, m_hPopupTexture, sColor4c(255,255,255,255));
*/
}

void HistoryScene::draw() {
	sceneSky->Draw(cameraSky);
	scene->Draw(historyCamera->getCamera());

	drawSprites();

	drawPopup();

//	if (debug_show_briefing_log) {
	if (!bwMode) {
		terRenderDevice->SetFont( logFnt );

//		terRenderDevice->OutText(0,0,historyCamera->log.c_str(),sColor4f(1,1,1,1));
		terRenderDevice->OutText(
			round(BRIEFING_LOG_X * terRenderDevice->GetSizeX()),
			round(BRIEFING_LOG_Y * terRenderDevice->GetSizeY()),
			interpreter->log.c_str(),
			sColor4f(1, 1, 1, BRIEFING_LOG_ALPHA) );
		terRenderDevice->SetFont( NULL );
	}
}
void HistoryScene::postDraw() {
	sceneSky->PostDraw(cameraSky);
	scene->PostDraw(historyCamera->getCamera());
}

void HistoryScene::setupAudio() {
	if (!terSoundEnable) {
		stopAudio();
	}
	voice.SetVolume( round(255 * terSoundVolume) );
}

void HistoryScene::startAudio(const string& name) {
	if (!name.empty()) {
		stopAudio();
		interpreter->eventOccured(Controller::END_OF_AUDIO_EVENT);
		if (terSoundEnable) {
			playingVoice = true;
			int ret = voice.OpenToPlay((gameShell->getLocDataPath() + name).c_str(), 0);
			xassert(ret);
			voice.SetVolume( round(255 * terSoundVolume) );
		}
	}

}
void HistoryScene::startVideo(const string& reelName, const string& sndName) {
	gameShell->showReelModal(reelName.c_str(), sndName.c_str());
}
void HistoryScene::startInterVideo(const string& reelName) {
	gameShell->showReelModal(reelName.c_str(), 0, false, false, INTERVIDEO_ALPHA);
}
void HistoryScene::setMusic(const string& path) {
	musicNamePath = path;
	if (getController()->isNormalSpeedMode()) {
		playMusic();
	}
}
void HistoryScene::addCameraPosition(
				const Vect3f& pivotPosition,  
				float psi,
				float theta,
				float distanceToPivot) {

	historyCamera->addToPath( CameraPosition(pivotPosition, psi, theta, distanceToPivot) );
	historyCamera->startPlaying();
}

void HistoryScene::waitFor(Controller::WaitEventType event) {
	switch (event) {
		case Controller::BEGIN_OF_CAMERA_EVENT:
		case Controller::END_OF_CAMERA_EVENT:
			lastEvent = event;
			break;
	}
}

void HistoryScene::clearCameraPath() {
	if (historyCamera->isFollowNomadMode()) {
		if (!interpreter->isNormalSpeedMode()) {
			historyCamera->setPositionToNomad();
		}
		historyCamera->setFollowNomadMode(false);
	} else {
		if (!interpreter->isNormalSpeedMode()) {
			switch (lastEvent) {
				case Controller::BEGIN_OF_CAMERA_EVENT:
					historyCamera->setPositionToBegin();
					break;
				case Controller::END_OF_CAMERA_EVENT:
					historyCamera->setPositionToEnd();
					break;
			}
		}
		historyCamera->clearPath();
	}
}

void HistoryScene::cameraFollowNomad(float deltaPsi, float radius, float theta) {
	historyCamera->setFollowNomadModeSettings(deltaPsi, radius, theta);
	if (!historyCamera->isFollowNomadMode()) {
		clearCameraPath();
		historyCamera->setFollowNomadMode(true);
	}
}

void HistoryScene::createNomadMarker() {
	nomadMarker = scene->CreateObject("RESOURCE\\MODELS\\MAIN\\Point_grn.M3D", "RESOURCE\\MODELS\\MAIN\\TEXTURES\\");
	nomadMarker->SetAttr(ATTRUNKOBJ_NOLIGHT);
	nomadMarker->SetScale(Vect3f(NOMAD_MARKER_SCALE, NOMAD_MARKER_SCALE, NOMAD_MARKER_SCALE));
	nomadMarker->SetPosition( initialMat );
	nomadMarker->SetChannel("main", true);
}
void HistoryScene::createSkySphere(bool bw) {
	RELEASE(skySphere);
	
	skySphere = sceneSky->CreateObject(
				bw ? "RESOURCE\\MODELS\\MENU\\B&W SkySphere.m3d" : "RESOURCE\\MODELS\\MENU\\SkySphere.m3d",
				"RESOURCE\\MODELS\\MENU\\TEXTURES\\"
	);
	skySphere->SetScale(Vect3f(100, 100, 100));
	skySphere->SetPosition(initialMat);
	skySphere->SetAttr(ATTRUNKOBJ_NOLIGHT);
}

void HistoryScene::createWorld3D(World* world, cScene* scene) {
	ElasticSphere* elasticSphere = (ElasticSphere*) scene->CreateElasticSphere();
	elasticSphere->setRadius(HISTORY_SCENE_WORLD_SIZE_FACTOR * world->getSize(), 0.2f);
	elasticSphere->SetPosition( MatXf(Mat3f::ID, world->getCoords()) );
	elasticSphere->blendMode = addBlendAlpha ? ALPHA_ADDBLENDALPHA : ALPHA_BLEND;

	world->setWorld3D(new WorldSphere(elasticSphere, world->isAlive()));
	elasticSphere->Release();
}

void HistoryScene::createLink3D(World* world, int linkNumber, cScene* scene) {
	WorldSphere* sphere1 = dynamic_cast<WorldSphere*>(world->getWorld3D());
	WorldSphere* sphere2 = dynamic_cast<WorldSphere*>(world->getConnectedToOmegaWorldByIndex(linkNumber)->getWorld3D());
	ElasticLink* elasticLink = new ElasticLink(
			sphere1->getElasticSphere(),
			sphere2->getElasticSphere() );
	electric->IncRef();
	elasticLink->SetTexture2( electric );
	world->setConnectedToOmegaLink3D(linkNumber, new LinkTubule(elasticLink));
	elasticLink->blendMode = addBlendAlpha ? ALPHA_ADDBLENDALPHA : ALPHA_BLEND;
	elasticLink->Release();
	scene->AttachObj(elasticLink);
}

void HistoryScene::createFrame3D(Frame* frame, cScene* scene) {
	frame->setFrame3D(new Frame3D(scene, frame->getRace()));
}

void HistoryScene::setupFrame3D(Frame* frame) {
	Frame3D* frame3D = (Frame3D*) frame->getFrame3D();
	if (frame->isAlive()) {
		frame3D->setRace(scene, frame->getRace());
		WorldSphere* world3D = (WorldSphere*) frame->getCurrentWorld()->getWorld3D();
		world3D->setupFrame3D(frame3D);
	} else {
		frame3D->setState(0);
	}
	if (frame3D->hasPlumeEffect) {
		frame3D->tryFlareEffect();
	}
}
void HistoryScene::setupFrame3D(FrameCommand* fc, const Vect3f& toPoint, ContinuousCommand* cc, const Vect3f& vPos, bool shouldStartFlareEffect) {
	Frame* frame = fc->getFrame(interpreter->frames);
	Frame3D* frame3D = (Frame3D*)frame->getFrame3D();
	frame3D->setPlumeEffect(true);
	frame3D->shouldStartFlareEffect = shouldStartFlareEffect;

	WorldSphere* world3D = (WorldSphere*) frame->getCurrentWorld()->getWorld3D();
	MatXf mat = world3D->getElasticSphere()->GetGlobalMatrix();
	mat.trans() += (toPoint - mat.trans()) * cc->phase(interpreter->getYearPhase());

	frame3D->getFrameObj()->SetPosition( mat );
	frame3D->updateColor(vPos);
}

void HistoryScene::setupWorld3D(WorldSphere* worldSphere) {
	int state = worldSphere->getState();
	xassert(state >= 0);
	worldTextures1[state]->IncRef();
	worldSphere->getElasticSphere()->SetTexture( 0, worldTextures1[state] ),
	worldTextures2[state]->IncRef();
	worldSphere->getElasticSphere()->SetTexture( 1, worldTextures2[state] );
}

void HistoryScene::setupLink3D(LinkTubule* linkTubule) {
	switch(linkTubule->getState()) {
		case LinkTubule::SELECTED:
			linkSelected->IncRef();
			linkTubule->getElasticLink()->SetTexture( linkSelected );
			break;
		case LinkTubule::NORMAL:
		default:
			linkNormal->IncRef();
			linkTubule->getElasticLink()->SetTexture( linkNormal );
	}
}
void HistoryScene::updateNomadMarker(const Vect3f& cameraPos, float dt) {
	Frame* nomad = interpreter->getNomadFrame();
	if (nomad) {
		WorldSphere* world3D = (WorldSphere*) nomad->getCurrentWorld()->getWorld3D();
//		MatXf mat = world3D->getElasticSphere()->GetGlobalMatrix();
		Frame3D* frame3D = (Frame3D*)nomad->getFrame3D();
		MatXf mat = frame3D->getFrameObj()->GetGlobalMatrix();
		mat.rot() = Mat3f::ID;
		Rotate(mat, Vect3f(0, 180, 0));
		Translate(mat, Vect3f(0, 0, world3D->getElasticSphere()->radius));
		nomadMarker->SetPosition(mat);
		float distance = nomadMarker->GetGlobalMatrix().trans().distance(-cameraPos);
		if (distance < 0 || distance >= SPHERE_VISIBLE_DISTANCE) {
			nomadMarker->SetAttr(ATTRUNKOBJ_IGNORE);
		} else {
			nomadMarker->ClearAttr(ATTRUNKOBJ_IGNORE);
			float f = 1.0f - distance / SPHERE_VISIBLE_DISTANCE;
			nomadMarker->SetColor( 0, &sColor4f(1, 1, 1, f) );
		}
		timer += dt;
		nomadMarker->SetPhase(fmod(timer, NOMAD_MARKER_PERIOD) / NOMAD_MARKER_PERIOD, true);
	} else {
		nomadMarker->SetAttr(ATTRUNKOBJ_IGNORE);
	}
}

void HistoryScene::onResolutionChanged() {
	historyCamera->onResolutionChanged();
//	RELEASE(fnt);
//	fnt = terVisGeneric->CreateFont("Arial", 10);
	RELEASE(m_hPopupTexture);
	m_hPopupTexture = terVisGeneric->CreateTexture(sPopupTexture);
}

World* HistoryScene::traceWorld(const Vect2f& pos) {
	Vect3f v0;
	Vect3f v1;
	historyCamera->calcRayIntersection(pos.x, pos.y, v0, v1);
	Vect3f v01 = v1 - v0;

	float dist;
	float distMin = FLT_INF;
	World* worldMin = 0;

	map <string, World*>::iterator it;
	map <string, World*>::iterator end = interpreter->worlds->worlds.end();
	for (it = interpreter->worlds->worlds.begin(); it != end; it++) {
		World* world = it->second;
		if (world->getWorld3D()) {
			ElasticSphere* eSphere = ((WorldSphere*)world->getWorld3D())->getElasticSphere();

			Vect3f v0x = eSphere->position() - v0;
			Vect3f v_normal;
			Vect3f v_tangent;
			decomposition(v01, v0x, v_normal, v_tangent);
			if (
					v_tangent.norm2() < sqr(eSphere->radius)
				&&	distMin > (dist = v_normal.norm2()) ) {

				distMin = dist;
				worldMin = world;
			}
		}
	}
	return worldMin;
}

string HistoryScene::getFrameNameFromBase(const string& keyStr) {
	string key = "Mission Frames." + keyStr;
	const char* stringFromBase = qdTextDB::instance().getText(key.c_str());
	return (*stringFromBase) ? stringFromBase : "";
//	return (*stringFromBase) ? stringFromBase : keyStr;
}

void HistoryScene::setNormalSpeedMode(bool normal) {
	if (normal) {
		playMusic();
	}
	getController()->setNormalSpeedMode(normal);
}
void HistoryScene::playMusic() {
	PlayMusic(("RESOURCE\\MUSIC\\" + musicNamePath).c_str());
}
