
#include "StdAfx.h"
#include "Commands.hpp"
#include "../GameShellSq.h"


const string CreateZeroWorld::NAME = "ZEROWORLD";
const string CreateZeroLink::NAME = "ZEROLINK";
const string KillAlphaPortal::NAME = "KILLALPHA";
const string KillOmegaPortal::NAME = "KILLOMEGA";
const string KillWorld::NAME = "DISSOLVE";
const string RenameWorld::NAME = "SELFNAME";
const string JumpToWorld::NAME = "JUMP";
const string BuildFrame::NAME = "BUILDFRAME";
const string CreateAlphaPortal::NAME = "MOUNTALPHA";
const string CreateOmegaPortal::NAME = "MOUNTOMEGA";
const string CreateLink::NAME = "ESTABLISHLINK";
const string TransferFrame::NAME = "TRANSFER";
const string ReachWorld::NAME = "REACH";
const string DestroyFrame::NAME = "DESTROY";
const string SendTestament::NAME = "TESTAMENT";
const string StopFrame::NAME = "STOP";
const string ExecuteMission::NAME = "MISSION";
const string ExecuteFakeMission::NAME = "FAKEMISSION";
const string ShowText::NAME = "SHOWTEXT";
const string HideText::NAME = "HIDETEXT";
const string Pause::NAME = "PAUSE";
const string SetSpeed::NAME = "SETDURATION";
const string StartAudio::NAME = "STARTAUDIO";
const string StartVideo::NAME = "STARTVIDEO";
const string StartInterVideo::NAME = "STARTINTERVIDEO";
const string SetMusic::NAME = "SETMUSIC";
const string AddCameraPosition::NAME = "ADDCAMERAPOINT";
const string ClearCameraPath::NAME = "CLEARCAMERA";
const string CameraFollowNomad::NAME = "CAMERAFOLLOWNOMAD";
const string WaitForCamera::NAME = "WAITCAMERABEGIN";
const string WaitForCameraPoint::NAME = "WAITCAMERAPOINT";
const string UntilEndOfAudio::NAME = "WAITAUDIO";
const string UntilEndOfVideo::NAME = "WAITVIDEO";
const string UntilEndOfCamera::NAME = "WAITCAMERAEND";
const string SetDelay::NAME = "SETDELAY";
const string RollBack::NAME = "ROLLBACK";
const string BreakSkipping::NAME = "BREAKSKIPPING";
const string ChangeFrameRace::NAME = "CHANGE";
const string FinishHistory::NAME = "FINISHHISTORY";


//---Zero-Commands---

//--CreateZeroWorld--

	CreateZeroWorld::CreateZeroWorld(const string& params, int duration) : Command(duration) {
		float x;
		float y;
		float z;
		int size;
		float timeFactor;
		int filthLevel;
		char* sysName = new char[100];
		char* name = new char[100];
		sscanf(
			params.c_str(),
			"ZEROWORLD %s X %f Y %f Z %f SIZE %d TIMERATIO %f FILTHLEVEL %d WORLD %s",
			name,
			&x,
			&y,
			&z,
			&size,
			&timeFactor,
			&filthLevel,
			sysName );
		origin = new World(sysName, Vect3f(x, y, -z) * HISTORY_SCENE_SCALE, size, filthLevel, timeFactor);
		origin->referenced();
		origin->createAlphaPortal(0);
		origin->createOmegaPortal();
		delete [] sysName;
		delete [] name;
	}

	CreateZeroWorld::~CreateZeroWorld() {
		if (origin) {
			origin->released();
		}
	}

	void CreateZeroWorld::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		worlds->addNewWorld(origin->clone());
	}

	int CreateZeroWorld::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string CreateZeroWorld::toString() const {
		return
			"ZEROWORLD " + origin->getName() + " WORLD " + origin->getSystemName();
	}


//--CreateZeroLink--

	CreateZeroLink::CreateZeroLink(const string& params, int duration) : Command(duration) {
		char* w1 = new char[100];
		char* w2 = new char[100];
		sscanf(
			params.c_str(),
			"ZEROLINK WORLD1 %s WORLD2 %s",
			w1,
			w2 );
		world1 = w1;
		world2 = w2;
		delete [] w1;
		delete [] w2;
	}

	void CreateZeroLink::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		World* w1 = worlds->getWorldBySysName(world1);
		World* w2 = worlds->getWorldBySysName(world2);
		World::linkWorlds(w1, w2);
	}

	int CreateZeroLink::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string CreateZeroLink::toString() const {
		return
			"ZEROLINK WORLD1 " + world1 + " WORLD2 " + world2;
	}

//---Usual-Commands---



//--KillAlphaPortal--

	KillAlphaPortal::KillAlphaPortal(const string& params, int duration) : Command(duration) {
		char* wName = new char[100];
		sscanf(
			params.c_str(),
			"KILLALPHA WORLD %s",
			wName );
		worldName = wName;
		delete [] wName;
	}

	void KillAlphaPortal::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		worlds->getWorldBySysName(worldName)->destroyAlphaPortal();
	}

	int KillAlphaPortal::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(worlds->getWorldBySysName(worldName)->getTimeFactor() * float(duration));
	}

	string KillAlphaPortal::toString() const {
		return
			"KILLALPHA WORLD " + worldName;
	}


//--KillOmegaPortal--

	KillOmegaPortal::KillOmegaPortal(const string& params, int duration) : Command(duration) {
		char* wName = new char[100];
		sscanf(
			params.c_str(),
			"KILLOMEGA WORLD %s",
			wName );
		worldName = wName;
		delete [] wName;
	}

	void KillOmegaPortal::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		worlds->getWorldBySysName(worldName)->destroyOmegaPortal();
	}

	int KillOmegaPortal::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(worlds->getWorldBySysName(worldName)->getTimeFactor() * float(duration));
	}

	string KillOmegaPortal::toString() const {
		return
			"KILLOMEGA WORLD " + worldName;
	}


//--KillWorld--

	KillWorld::KillWorld(const string& params, int duration) : Command(duration) {
		char* wName = new char[100];
		sscanf(
			params.c_str(),
			"DISSOLVE WORLD %s",
			wName );
		worldName = wName;
		delete [] wName;
	}

	void KillWorld::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		World* w = worlds->getWorldBySysName(worldName);
		if (w) {
			w->destroy();
		}
	}

	int KillWorld::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(worlds->getWorldBySysName(worldName)->getTimeFactor() * float(duration));
	}

	string KillWorld::toString() const {
		return
			"DISSOLVE WORLD " + worldName;
	}


//--RenameWorld--

	RenameWorld::RenameWorld(const string& params, int duration) : Command(duration) {
		char* sysName = new char[100];
		char* wName = new char[100];
		sscanf(
			params.c_str(),
			"SELFNAME WORLD %s NEWNAME %s",
			sysName,
			wName );
		worldSysName = sysName;
		worldName = wName;
		delete [] sysName;
		delete [] wName;
	}

	void RenameWorld::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		worlds->getWorldBySysName(worldSysName)->setNewName(worldName);
	}

	int RenameWorld::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string RenameWorld::toString() const {
		return
			"SELFNAME WORLD " + worldSysName + " NEWNAME " + worldName;
	}


//---Frame-Commands---


//--JumpToWorld--

	JumpToWorld::JumpToWorld(const string& params, int duration) : FrameCommand(duration) {
		float x;
		float y;
		float z;
		int size;
		float timeFactor;
		int filthLevel;
		char* sysName = new char[100];
		char* fName = new char[100];
		sscanf(
			params.c_str(),
			"JUMP X %f Y %f Z %f SIZE %d TIMERATIO %f FILTHLEVEL %d FRAME %s WORLD %s",
			&x,
			&y,
			&z,
			&size,
			&timeFactor,
			&filthLevel,
			fName,
			sysName );
		frameName = fName;
		origin = new World(sysName, Vect3f(x, y, -z) * HISTORY_SCENE_SCALE, size, filthLevel, timeFactor);
		origin->referenced();
		delete [] sysName;
		delete [] fName;
	}

	JumpToWorld::~JumpToWorld() {
		if (origin) {
			origin->released();
		}
	}

	void JumpToWorld::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		World* w = origin->clone();
		worlds->addNewWorld(w);
		getFrame(frames)->goToWorld(w);
		
	}

	const Vect3f& JumpToWorld::getWorldCoords() {
		return origin->getCoords();
	}

	int JumpToWorld::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	string JumpToWorld::toString() const {
		return
			"JUMP FRAME " + frameName + " WORLD " + origin->getSystemName();
	}


//--BuildFrame--

	BuildFrame::BuildFrame(const string& params, int duration) : FrameCommand(duration) {
		char* fName = new char[100];
		char* wName = new char[100];

		sscanf(
			params.c_str(),
			"BUILDFRAME %s WORLD %s RACE %d COLOR %d",
			fName,
			wName,
			&race,
			&colorIndex );

		frameName = fName;
		worldSysName = wName;

		shouldCreateNomad = (strstr(params.c_str(), "NOMAD") != NULL);
		delete [] fName;
		delete [] wName;
	}

	void BuildFrame::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		Frame* newFrame = new Frame(frameName, worlds->getWorldBySysName(worldSysName), race, colorIndex);
		frames->addNewFrame( newFrame );
		if (shouldCreateNomad) {
			controller->nomadToFrame(newFrame);
		}
	}

	int BuildFrame::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(worlds->getWorldBySysName(worldSysName)->getTimeFactor() * float(duration));
	}

	string BuildFrame::toString() const {
		return
			"BUILDFRAME " + frameName + " WORLD " + worldSysName;
	}

//--ChangeFrameRace--

	ChangeFrameRace::ChangeFrameRace(const string& params, int duration) : FrameCommand(duration) {
		char* fName = new char[100];
		sscanf(
			params.c_str(),
			"CHANGE FRAME %s RACE %d COLOR %d",
			fName,
			&race,
			&colorIndex);
		frameName = fName;
		delete [] fName;
	}

	void ChangeFrameRace::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		frames->getFrameByName(frameName)->setRace(race);
		frames->getFrameByName(frameName)->setColorIndex(colorIndex);
	}

	int ChangeFrameRace::calculateLength(Worlds* worlds, Frames* frames) const {
		return float(duration);
	}

	string ChangeFrameRace::toString() const {
		return
			"CHANGE FRAME" + frameName;
	}

//--CreateAlphaPortal--

	CreateAlphaPortal::CreateAlphaPortal(const string& params, int duration) : FrameCommand(duration) {
		char* fName = new char[100];
		sscanf(
			params.c_str(),
			"MOUNTALPHA FRAME %s",
			fName );
		frameName = fName;
		delete [] fName;
	}

	void CreateAlphaPortal::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		Frame* frame = frames->getFrameByName(frameName);
		int size = frame->getKnowledge().getPath().size();
		World* world1 = frame->getKnowledge().getPath()[size - 1];
		World* world2 = frame->getKnowledge().getPath()[size - 2];
		world1->createAlphaPortal(world2);
	}

	void CreateAlphaPortal::setObject3D(Worlds* worlds, Frames* frames, Object3D* object3D) {
		Frame* frame = frames->getFrameByName(frameName);
		int size = frame->getKnowledge().getPath().size();
		World* world2 = frame->getKnowledge().getPath()[size - 2];
		if ( world2->hasOmegaPortal() ) {
			world2->setLastConnectedToOmegaLink3D(object3D);
		}
	}

	string CreateAlphaPortal::toString() const {
		return
			"MOUNTALPHA FRAME " + frameName;
	}

	int CreateAlphaPortal::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}


//--CreateOmegaPortal--

	CreateOmegaPortal::CreateOmegaPortal(const string& params, int duration) : FrameCommand(duration) {
		char* fName = new char[100];
		sscanf(
			params.c_str(),
			"MOUNTOMEGA FRAME %s",
			fName );
		frameName = fName;
		delete [] fName;
	}

	void CreateOmegaPortal::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		frames->getFrameByName(frameName)->getCurrentWorld()->createOmegaPortal();
	}

	int CreateOmegaPortal::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	string CreateOmegaPortal::toString() const {
		return
			"MOUNTOMEGA FRAME " + frameName;
	}


//--CreateLink--

	CreateLink::CreateLink(const string& params, int duration) : FrameCommand(duration) {
		char* fName = new char[100];
		char* wName = new char[100];
		sscanf(
			params.c_str(),
			"ESTABLISHLINK FRAME %s WORLD %s",
			fName,
			wName );
		frameName = fName;
		worldSysName = wName;
		delete [] fName;
		delete [] wName;
	}

	void CreateLink::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		World::linkWorlds(
			worlds->getWorldBySysName(worldSysName),
			frames->getFrameByName(frameName)->getCurrentWorld() );
	}

	void CreateLink::setObject3D(Worlds* worlds, Frames* frames, Object3D* object3D) {
		World* world = frames->getFrameByName(frameName)->getCurrentWorld();
		if ( !(world->hasOmegaPortal()) ) {
			world = worlds->getWorldBySysName(worldSysName);
		}
		world->setLastConnectedToOmegaLink3D(object3D);
	}

	World* CreateLink::getWorld(Worlds* worlds) {
		return worlds->getWorldBySysName(worldSysName);
	}

	int CreateLink::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	string CreateLink::toString() const {
		return
			"ESTABLISHLINK FRAME " + frameName + " WORLD " + worldSysName;
	}


//--TransferFrame--

	TransferFrame::TransferFrame(const string& params, int duration) : FrameCommand(duration) {
		char* fName = new char[100];
		char* wName = new char[100];
		sscanf(
			params.c_str(),
			"TRANSFER FRAME %s WORLD %s",
			fName,
			wName );
		frameName = fName;
		worldSysName = wName;
		delete [] fName;
		delete [] wName;
	}

	void TransferFrame::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		frames->getFrameByName(frameName)->goToWorld( worlds->getWorldBySysName(worldSysName) );
	}

	int TransferFrame::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	World* TransferFrame::getWorldToTransfer(Worlds* worlds) {
		return worlds->getWorldBySysName(worldSysName);
	}

	string TransferFrame::toString() const {
		return
			"TRANSFER FRAME " + frameName + " WORLD " + worldSysName;
	}


//--ReachWorld--

	ReachWorld::ReachWorld(const string& params, int duration) : FrameCommand(duration) {
		char* fName = new char[100];
		char* wName = new char[100];
		sscanf(
			params.c_str(),
			"REACH FRAME %s WORLD %s",
			fName,
			wName );
		frameName = fName;
		worldSysName = wName;
		delete [] fName;
		delete [] wName;
	}

	void ReachWorld::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		frames->getFrameByName(frameName)->goToWorld( worlds->getWorldBySysName(worldSysName) );
	}

	int ReachWorld::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	World* ReachWorld::getWorldToReach(Worlds* worlds) {
		return worlds->getWorldBySysName(worldSysName);
	}

	string ReachWorld::toString() const {
		return
			"REACH FRAME " + frameName + " WORLD " + worldSysName;
	}

//--DestroyFrame--

	DestroyFrame::DestroyFrame(const string& params, int duration) : FrameCommand(duration) {
		char* fName = new char[100];
		sscanf(
			params.c_str(),
			"DESTROY FRAME %s",
			fName );
		frameName = fName;
		delete [] fName;
	}

	void DestroyFrame::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		frames->getFrameByName(frameName)->destroy();	
	}

	int DestroyFrame::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	string DestroyFrame::toString() const {
		return
			"DESTROY FRAME " + frameName;
	}

//--SendTestament--

	SendTestament::SendTestament(const string& params, int duration) : FrameCommand(duration) {
		char* fName = new char[100];
		if (strstr(params.c_str(), "NOMAD") == NULL) {
			sscanf(
				params.c_str(),
				"TESTAMENT %s NUM %d",
				fName,
				&depth );
			shouldSendNomad = false;
		} else {
			char* nomadFName = new char[100];
			sscanf(
				params.c_str(),
				"TESTAMENT %s NUM %d NOMAD FRAME %s",
				fName,
				&depth,
				nomadFName );
			shouldSendNomad = true;
			nomadFrameName = nomadFName;
			delete [] nomadFName;
		}
		frameName = fName;
		delete [] fName;
	}

	void SendTestament::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		Frame* frame = frames->getFrameByName(frameName);
		const Knowledge& knowledge = frame->getKnowledge();
		World* w = frame->getCurrentWorld();
		w->setTestamentLevel(1);
		pool.clear();
		pool.push_back(w);
		checkedBound = pool.size();
		connectedToForCheck(w);
		forCheckBound = pool.size();
		for (int i = 1; i < depth; i++) {
			for (int j = checkedBound; j < forCheckBound; j++) {
				pool[j]->setTestamentLevel(i + 1);
				frames->knowledgeArrivedToWorld(knowledge, pool[j]);
				connectedToForCheck(pool[j]);
				checkedBound++;
			}
			forCheckBound = pool.size();
		}
		if (shouldSendNomad) {
			controller->nomadToFrame(frames->getFrameByName(nomadFrameName));
		}
	}

	void SendTestament::connectedToForCheck(World* world) {
		int s = world->getConnectedToAlphaWorldCount();
		for (int i = 0; i < s; i++) {
			World* w = world->getConnectedToAlphaWorldByIndex(i);
			if (!checked(w)) {
				pool.push_back(w);
			}
		}
		s = world->getConnectedToOmegaWorldCount();
		for (int i = 0; i < s; i++) {
			World* w = world->getConnectedToOmegaWorldByIndex(i);
			if (!checked(w)) {
				pool.push_back(w);
			}
		}
	}

	int SendTestament::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	string SendTestament::toString() const {
		return
			"TESTAMENT " + frameName;
	}


//--StopFrame--

	StopFrame::StopFrame(const string& params, int duration) : FrameCommand(duration) {
		char* fName = new char[100];
		sscanf(
			params.c_str(),
			"STOP FRAME %s",
			fName );
		frameName = fName;
		delete [] fName;
	}

	void StopFrame::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->stopFrame(frames->getFrameByName(frameName));
	}

	int StopFrame::calculateLength(Worlds* worlds, Frames* frames) const {
		return ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	string StopFrame::toString() const {
		return
			"STOP FRAME " + frameName;
	}


//---Control-Commands---


//--ExecuteMission--

	ExecuteMission::ExecuteMission(const string& params, int duration) : Command(duration) {
		string onlyParams = params.substr(8, params.size() - 9);
		int loc = onlyParams.find("spg") + 3;
		fileName = onlyParams.substr(0, loc);
		missionName = onlyParams.substr(loc + 1);
	}

	void ExecuteMission::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::MISSION_VICTORY_EVENT);
		commander->waitFor(Controller::MISSION_VICTORY_EVENT);
		commander->executeMission(fileName);
	}

	int ExecuteMission::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string ExecuteMission::toString() const {
		return "";
//		return
//			"MISSION " + missionName;
	}

//--ExecuteFakeMission--

	ExecuteFakeMission::ExecuteFakeMission(const string& params, int duration) : Command(duration) {
	}

	void ExecuteFakeMission::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->executeFakeMission();
	}

	int ExecuteFakeMission::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string ExecuteFakeMission::toString() const {
		return "";
//		return
//			"FAKEMISSION";
	}

//--ShowText--

	ShowText::ShowText(const string& params, int duration) : Command(duration) {
		if (strstr(params.c_str(), "ICON") == NULL) {
			text = params.substr(10, params.length() - 12);
			icon = "";
		} else {
			int index = params.find("ICON") + 5;
			icon = "RESOURCE\\ICONS\\Portraits\\" + params.substr(index, params.length() - index - 1) + ".tga";
			index = params.find("\"");
			int backIndex = params.rfind("\"");
			text = params.substr(index + 1, backIndex - index - 1);
		}

	}

	void ShowText::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->showText(text, icon);
	}

	int ShowText::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string ShowText::toString() const {
		return "";
//		return
//			"SHOWTEXT";
	}

//--HideText--

	HideText::HideText(const string& params, int duration) : Command(duration) {
	}

	void HideText::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->hideText();
	}

	int HideText::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string HideText::toString() const {
		return "";
//		return
//			"HIDETEXT";
	}

//--Pause--

	Pause::Pause(const string& params, int duration) : Command(duration) {
	}

	void Pause::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::CONTROL_SUBMIT_EVENT);
		commander->waitFor(Controller::CONTROL_SUBMIT_EVENT);
		commander->showPaused();
	}

	int Pause::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string Pause::toString() const {
		return "";
//		return
//			"PAUSE";
	}

//--SetSpeed--

	SetSpeed::SetSpeed(const string& params, int duration) : Command(duration) {
		sscanf(
			params.c_str(),
			"SETDURATION %f",
			&speed );
	}

	void SetSpeed::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->setYearDuration(speed);
	}

	int SetSpeed::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string SetSpeed::toString() const {
		return "";
//		char str[31];
//		sprintf(str, "SETDURATION %f", speed);
//		return string(str);
	}

//--SetDelay--

	SetDelay::SetDelay(const string& params, int duration) : Command(duration) {
		sscanf(
			params.c_str(),
			"SETDELAY %f",
			&delay );
	}

	void SetDelay::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->setDelay(delay);
	}

	int SetDelay::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string SetDelay::toString() const {
		return "";
//		char str[31];
//		sprintf(str, "SETDELAY %f", delay);
//		return string(str);
	}

//--StartAudio--

	StartAudio::StartAudio(const string& params, int duration) : Command(duration) {
		char* name = new char[100];
		sscanf(
			params.c_str(),
			"STARTAUDIO %s",
			name );
		audioName = name;
		delete [] name;
	}

	void StartAudio::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->startAudio(audioName);
	}

	int StartAudio::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string StartAudio::toString() const {
		return "";
//		return 
//			"STARTAUDIO";
	}

//--StartVideo--

	StartVideo::StartVideo(const string& params, int duration) : Command(duration) {
		char* rName = new char[100];
		char* sName = new char[100];
		sscanf(
			params.c_str(),
			"STARTVIDEO REEL %s SND %s",
			rName,
			sName );
		reelName = rName;
		sndName = sName;
		delete [] rName;
		delete [] sName;
	}

	void StartVideo::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->startVideo(reelName, sndName);
	}

	int StartVideo::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string StartVideo::toString() const {
		return "";
//		return 
//			"STARTVIDEO";
	}

//--StartInterVideo--

	StartInterVideo::StartInterVideo(const string& params, int duration) : Command(duration) {
		char* rName = new char[100];
		sscanf(
			params.c_str(),
			"STARTINTERVIDEO REEL %s",
			rName);
		reelName = rName;
		delete [] rName;
	}

	void StartInterVideo::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->startInterVideo(reelName);
	}

	int StartInterVideo::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string StartInterVideo::toString() const {
		return "";
//		return 
//			"STARTINTERVIDEO";
	}

//--SetMusic--

	SetMusic::SetMusic(const string& params, int duration) : Command(duration) {
		char* namePath = new char[256];
		sscanf(
			params.c_str(),
			"SETMUSIC %s",
			namePath);
		path = namePath;
		delete [] namePath;
	}

	void SetMusic::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->setMusic(path);
	}

	int SetMusic::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string SetMusic::toString() const {
		return "";
//		return 
//			"SETMUSIC";
	}

//--AddCameraPosition--

	AddCameraPosition::AddCameraPosition(const string& params, int duration) : Command(duration) {
		sscanf(
			params.c_str(),
			"ADDCAMERAPOINT %f %f %f %f %f %f",
			&distanceToPivot,
			&theta,
			&psi,
			&pivotPosition.x,
			&pivotPosition.z,
			&pivotPosition.y );
		distanceToPivot *= HISTORY_SCENE_SCALE;
		pivotPosition *= HISTORY_SCENE_SCALE; 
		theta = -theta - M_PI / 2.0f;
		psi += M_PI;
		pivotPosition.z = -pivotPosition.z;
	}

	void AddCameraPosition::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->addCameraPosition(pivotPosition, psi, theta, distanceToPivot);
	}

	int AddCameraPosition::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string AddCameraPosition::toString() const {
		return "";
//		return 
//			"ADDCAMERAPOINT";
	}

//--ClearCameraPath--

	ClearCameraPath::ClearCameraPath(const string& params, int duration) : Command(duration) {
	}

	void ClearCameraPath::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->clearCameraPath();
//		controller->waitFor(Controller::END_OF_CAMERA_EVENT);
	}

	int ClearCameraPath::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string ClearCameraPath::toString() const {
		return "";
//		return 
//			"CLEARCAMERA";
	}

//--CameraFollowNomad--

	CameraFollowNomad::CameraFollowNomad(const string& params, int duration) : Command(duration) {
		sscanf(
			params.c_str(),
			"CAMERAFOLLOWNOMAD %f %f %f",
			&deltaPsi,
			&radius,
			&theta );

		radius *= HISTORY_SCENE_SCALE;
		theta = -theta - M_PI / 2.0f;
	}

	void CameraFollowNomad::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->cameraFollowNomad(deltaPsi, radius, theta);
	}

	int CameraFollowNomad::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string CameraFollowNomad::toString() const {
		return "";
//		return 
//			"CAMERAFOLLOWNOMAD";
	}

//--WaitForCamera--

	WaitForCamera::WaitForCamera(const string& params, int duration) : Command(duration) {
	}

	void WaitForCamera::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::BEGIN_OF_CAMERA_EVENT);
		commander->waitFor(Controller::BEGIN_OF_CAMERA_EVENT);
	}

	int WaitForCamera::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string WaitForCamera::toString() const {
		return "";
//		return 
//			"WAITFORCAMERA";
	}

//--WaitForCameraPoint--

	WaitForCameraPoint::WaitForCameraPoint(const string& params, int duration) : Command(duration) {
	}

	void WaitForCameraPoint::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::POINT_OF_CAMERA_EVENT);
		commander->waitFor(Controller::POINT_OF_CAMERA_EVENT);
	}

	int WaitForCameraPoint::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string WaitForCameraPoint::toString() const {
		return "";
//		return 
//			"WAITFORCAMERAPOINT";
	}

//--UntilEndOfAudio--

	UntilEndOfAudio::UntilEndOfAudio(const string& params, int duration) : Command(duration) {
	}

	void UntilEndOfAudio::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::END_OF_AUDIO_EVENT);
		commander->waitFor(Controller::END_OF_AUDIO_EVENT);
	}

	int UntilEndOfAudio::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string UntilEndOfAudio::toString() const {
		return "";
//		return 
//			"UNTILENDOFAUDIO";
	}

//--UntilEndOfVideo--

	UntilEndOfVideo::UntilEndOfVideo(const string& params, int duration) : Command(duration) {
	}

	void UntilEndOfVideo::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::END_OF_VIDEO_EVENT);
		commander->waitFor(Controller::END_OF_VIDEO_EVENT);
	}

	int UntilEndOfVideo::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string UntilEndOfVideo::toString() const {
		return "";
//		return 
//			"UNTILENDOFVIDEO";
	}

//--UntilEndOfCamera--

	UntilEndOfCamera::UntilEndOfCamera(const string& params, int duration) : Command(duration) {
	}

	void UntilEndOfCamera::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::END_OF_CAMERA_EVENT);
		commander->waitFor(Controller::END_OF_CAMERA_EVENT);
	}

	int UntilEndOfCamera::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string UntilEndOfCamera::toString() const {
		return "";
//		return 
//			"WAITFORCAMERAEND";
	}

//--RollBack--

	RollBack::RollBack(const string& params, int duration) : Command(duration) {
	}

	void RollBack::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->rollback();
	}

	int RollBack::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string RollBack::toString() const {
		return 
			"ROLLBACK";
	}

//--BreakSkipping--

	BreakSkipping::BreakSkipping(const string& params, int duration) : Command(duration) {
	}

	void BreakSkipping::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
//		controller->breakSkipping();
//		controller->waitFor(Controller::CONTROL_SUBMIT_EVENT);
//		commander->waitFor(Controller::CONTROL_SUBMIT_EVENT);
//		commander->showPaused();
	}

	int BreakSkipping::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string BreakSkipping::toString() const {
		return "";
//		return 
//			"BREAKSKIPPING";
	}

//--FinishHistory--

	FinishHistory::FinishHistory(const string& params, int duration) : Command(duration) {
	}

	void FinishHistory::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::MISSION_VICTORY_EVENT);
		commander->finishHistory();
	}

	int FinishHistory::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	string FinishHistory::toString() const {
		return 
			"FINISHHISTORY";
	}

//---Factory---

	Command* CommandFactory::createCommand(const string& command) {
		if (commandHasName(command, CreateZeroWorld::NAME)) {
			return new CreateZeroWorld(command, 0);
		} else if (commandHasName(command, CreateZeroLink::NAME)) {
			return new CreateZeroLink(command, 0);
		} else if (commandHasName(command, JumpToWorld::NAME)) {
			return new JumpToWorld(command, 3);
		} else if (commandHasName(command, BuildFrame::NAME)) {
			return new BuildFrame(command, 2);
		} else if (commandHasName(command, ChangeFrameRace::NAME)) {
			return new ChangeFrameRace(command, 0);
		} else if (commandHasName(command, CreateAlphaPortal::NAME)) {
			return new CreateAlphaPortal(command, 3);
		} else if (commandHasName(command, CreateOmegaPortal::NAME)) {
			return new CreateOmegaPortal(command, 3);
		} else if (commandHasName(command, CreateLink::NAME)) {
			return new CreateLink(command, 1);
		} else if (commandHasName(command, TransferFrame::NAME)) {
			return new TransferFrame(command, 1);
		} else if (commandHasName(command, ReachWorld::NAME)) {
			return new ReachWorld(command, 3);
		} else if (commandHasName(command, DestroyFrame::NAME)) {
			return new DestroyFrame(command, 0);
		} else if (commandHasName(command, SendTestament::NAME)) {
			return new SendTestament(command, 0);
		} else if (commandHasName(command, StopFrame::NAME)) {
			return new StopFrame(command, 0);
		} else if (commandHasName(command, KillAlphaPortal::NAME)) {
			return new KillAlphaPortal(command, 0);
		} else if (commandHasName(command, KillOmegaPortal::NAME)) {
			return new KillOmegaPortal(command, 0);
		} else if (commandHasName(command, KillWorld::NAME)) {
			return new KillWorld(command, 0);
		} else if (commandHasName(command, ExecuteMission::NAME)) {
			return new ExecuteMission(command, 0);
		} else if (commandHasName(command, RenameWorld::NAME)) {
			return new RenameWorld(command, 0);
		} else if (commandHasName(command, ShowText::NAME)) {
			return new ShowText(command, 0);
		} else if (commandHasName(command, HideText::NAME)) {
			return new HideText(command, 0);
		} else if (commandHasName(command, Pause::NAME)) {
			return new Pause(command, 0);
		} else if (commandHasName(command, SetSpeed::NAME)) {
			return new SetSpeed(command, 0);
		} else if (commandHasName(command, StartAudio::NAME)) {
			return new StartAudio(command, 0);
		} else if (commandHasName(command, StartVideo::NAME)) {
			return new StartVideo(command, 0);
		} else if (commandHasName(command, StartInterVideo::NAME)) {
			return new StartInterVideo(command, 0);
		} else if (commandHasName(command, SetMusic::NAME)) {
			return new SetMusic(command, 0);
		} else if (commandHasName(command, AddCameraPosition::NAME)) {
			return new AddCameraPosition(command, 0);
		} else if (commandHasName(command, ClearCameraPath::NAME)) {
			return new ClearCameraPath(command, 0);
		} else if (commandHasName(command, CameraFollowNomad::NAME)) {
			return new CameraFollowNomad(command, 0);
		} else if (commandHasName(command, WaitForCamera::NAME)) {
			return new WaitForCamera(command, 0);
		} else if (commandHasName(command, WaitForCameraPoint::NAME)) {
			return new WaitForCameraPoint(command, 0);
		} else if (commandHasName(command, UntilEndOfAudio::NAME)) {
			return new UntilEndOfAudio(command, 0);
		} else if (commandHasName(command, UntilEndOfVideo::NAME)) {
			return new UntilEndOfVideo(command, 0);
		} else if (commandHasName(command, UntilEndOfCamera::NAME)) {
			return new UntilEndOfCamera(command, 0);
		} else if (commandHasName(command, SetDelay::NAME)) {
			return new SetDelay(command, 0);
		} else if (commandHasName(command, RollBack::NAME)) {
			return new RollBack(command, 0);
		} else if (commandHasName(command, ExecuteFakeMission::NAME)) {
			return new ExecuteFakeMission(command, 0);
		} else if (commandHasName(command, BreakSkipping::NAME)) {
			return new BreakSkipping(command, 0);
		} else if (commandHasName(command, FinishHistory::NAME)) {
			return new FinishHistory(command, 0);
		}
		return 0;
	}
