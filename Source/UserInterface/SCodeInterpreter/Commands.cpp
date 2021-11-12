
#include "StdAfx.h"
#include "Commands.hpp"
#include "../GameShellSq.h"


const std::string CreateZeroWorld::NAME = "ZEROWORLD";
const std::string CreateZeroLink::NAME = "ZEROLINK";
const std::string KillAlphaPortal::NAME = "KILLALPHA";
const std::string KillOmegaPortal::NAME = "KILLOMEGA";
const std::string KillWorld::NAME = "DISSOLVE";
const std::string RenameWorld::NAME = "SELFNAME";
const std::string JumpToWorld::NAME = "JUMP";
const std::string BuildFrame::NAME = "BUILDFRAME";
const std::string CreateAlphaPortal::NAME = "MOUNTALPHA";
const std::string CreateOmegaPortal::NAME = "MOUNTOMEGA";
const std::string CreateLink::NAME = "ESTABLISHLINK";
const std::string TransferFrame::NAME = "TRANSFER";
const std::string ReachWorld::NAME = "REACH";
const std::string DestroyFrame::NAME = "DESTROY";
const std::string SendTestament::NAME = "TESTAMENT";
const std::string StopFrame::NAME = "STOP";
const std::string ExecuteMission::NAME = "MISSION";
const std::string ExecuteFakeMission::NAME = "FAKEMISSION";
const std::string ShowText::NAME = "SHOWTEXT";
const std::string HideText::NAME = "HIDETEXT";
const std::string Pause::NAME = "PAUSE";
const std::string SetSpeed::NAME = "SETDURATION";
const std::string StartAudio::NAME = "STARTAUDIO";
const std::string StartVideo::NAME = "STARTVIDEO";
const std::string StartInterVideo::NAME = "STARTINTERVIDEO";
const std::string SetMusic::NAME = "SETMUSIC";
const std::string AddCameraPosition::NAME = "ADDCAMERAPOINT";
const std::string ClearCameraPath::NAME = "CLEARCAMERA";
const std::string CameraFollowNomad::NAME = "CAMERAFOLLOWNOMAD";
const std::string WaitForCamera::NAME = "WAITCAMERABEGIN";
const std::string WaitForCameraPoint::NAME = "WAITCAMERAPOINT";
const std::string UntilEndOfAudio::NAME = "WAITAUDIO";
const std::string UntilEndOfVideo::NAME = "WAITVIDEO";
const std::string UntilEndOfCamera::NAME = "WAITCAMERAEND";
const std::string SetDelay::NAME = "SETDELAY";
const std::string RollBack::NAME = "ROLLBACK";
const std::string BreakSkipping::NAME = "BREAKSKIPPING";
const std::string ChangeFrameRace::NAME = "CHANGE";
const std::string FinishHistory::NAME = "FINISHHISTORY";


//---Zero-Commands---

//--CreateZeroWorld--

	CreateZeroWorld::CreateZeroWorld(const std::string& params, int duration) : Command(duration) {
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

	std::string CreateZeroWorld::toString() const {
		return
			"ZEROWORLD " + origin->getName() + " WORLD " + origin->getSystemName();
	}


//--CreateZeroLink--

	CreateZeroLink::CreateZeroLink(const std::string& params, int duration) : Command(duration) {
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

	std::string CreateZeroLink::toString() const {
		return
			"ZEROLINK WORLD1 " + world1 + " WORLD2 " + world2;
	}

//---Usual-Commands---



//--KillAlphaPortal--

	KillAlphaPortal::KillAlphaPortal(const std::string& params, int duration) : Command(duration) {
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
		return xm::ceil(worlds->getWorldBySysName(worldName)->getTimeFactor() * float(duration));
	}

	std::string KillAlphaPortal::toString() const {
		return
			"KILLALPHA WORLD " + worldName;
	}


//--KillOmegaPortal--

	KillOmegaPortal::KillOmegaPortal(const std::string& params, int duration) : Command(duration) {
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
		return xm::ceil(worlds->getWorldBySysName(worldName)->getTimeFactor() * float(duration));
	}

	std::string KillOmegaPortal::toString() const {
		return
			"KILLOMEGA WORLD " + worldName;
	}


//--KillWorld--

	KillWorld::KillWorld(const std::string& params, int duration) : Command(duration) {
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
		return xm::ceil(worlds->getWorldBySysName(worldName)->getTimeFactor() * float(duration));
	}

	std::string KillWorld::toString() const {
		return
			"DISSOLVE WORLD " + worldName;
	}


//--RenameWorld--

	RenameWorld::RenameWorld(const std::string& params, int duration) : Command(duration) {
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

	std::string RenameWorld::toString() const {
		return
			"SELFNAME WORLD " + worldSysName + " NEWNAME " + worldName;
	}


//---Frame-Commands---


//--JumpToWorld--

	JumpToWorld::JumpToWorld(const std::string& params, int duration) : FrameCommand(duration) {
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
		return xm::ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	std::string JumpToWorld::toString() const {
		return
			"JUMP FRAME " + frameName + " WORLD " + origin->getSystemName();
	}


//--BuildFrame--

	BuildFrame::BuildFrame(const std::string& params, int duration) : FrameCommand(duration) {
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
		return xm::ceil(worlds->getWorldBySysName(worldSysName)->getTimeFactor() * float(duration));
	}

	std::string BuildFrame::toString() const {
		return
			"BUILDFRAME " + frameName + " WORLD " + worldSysName;
	}

//--ChangeFrameRace--

	ChangeFrameRace::ChangeFrameRace(const std::string& params, int duration) : FrameCommand(duration) {
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

	std::string ChangeFrameRace::toString() const {
		return
			"CHANGE FRAME" + frameName;
	}

//--CreateAlphaPortal--

	CreateAlphaPortal::CreateAlphaPortal(const std::string& params, int duration) : FrameCommand(duration) {
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

	std::string CreateAlphaPortal::toString() const {
		return
			"MOUNTALPHA FRAME " + frameName;
	}

	int CreateAlphaPortal::calculateLength(Worlds* worlds, Frames* frames) const {
		return xm::ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}


//--CreateOmegaPortal--

	CreateOmegaPortal::CreateOmegaPortal(const std::string& params, int duration) : FrameCommand(duration) {
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
		return xm::ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	std::string CreateOmegaPortal::toString() const {
		return
			"MOUNTOMEGA FRAME " + frameName;
	}


//--CreateLink--

	CreateLink::CreateLink(const std::string& params, int duration) : FrameCommand(duration) {
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
		return xm::ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	std::string CreateLink::toString() const {
		return
			"ESTABLISHLINK FRAME " + frameName + " WORLD " + worldSysName;
	}


//--TransferFrame--

	TransferFrame::TransferFrame(const std::string& params, int duration) : FrameCommand(duration) {
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
		return xm::ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	World* TransferFrame::getWorldToTransfer(Worlds* worlds) {
		return worlds->getWorldBySysName(worldSysName);
	}

	std::string TransferFrame::toString() const {
		return
			"TRANSFER FRAME " + frameName + " WORLD " + worldSysName;
	}


//--ReachWorld--

	ReachWorld::ReachWorld(const std::string& params, int duration) : FrameCommand(duration) {
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
		return xm::ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	World* ReachWorld::getWorldToReach(Worlds* worlds) {
		return worlds->getWorldBySysName(worldSysName);
	}

	std::string ReachWorld::toString() const {
		return
			"REACH FRAME " + frameName + " WORLD " + worldSysName;
	}

//--DestroyFrame--

	DestroyFrame::DestroyFrame(const std::string& params, int duration) : FrameCommand(duration) {
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
		return xm::ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	std::string DestroyFrame::toString() const {
		return
			"DESTROY FRAME " + frameName;
	}

//--SendTestament--

	SendTestament::SendTestament(const std::string& params, int duration) : FrameCommand(duration) {
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
		return xm::ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	std::string SendTestament::toString() const {
		return
			"TESTAMENT " + frameName;
	}


//--StopFrame--

	StopFrame::StopFrame(const std::string& params, int duration) : FrameCommand(duration) {
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
		return xm::ceil(getFrame(frames)->getCurrentWorld()->getTimeFactor() * float(duration));
	}

	std::string StopFrame::toString() const {
		return
			"STOP FRAME " + frameName;
	}


//---Control-Commands---


//--ExecuteMission--

	ExecuteMission::ExecuteMission(const std::string& params, int duration) : Command(duration) {
		std::string onlyParams = params.substr(8, params.size() - 9);
		size_t loc = onlyParams.find("spg") + 3;
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

	std::string ExecuteMission::toString() const {
		return "";
//		return
//			"MISSION " + missionName;
	}

//--ExecuteFakeMission--

	ExecuteFakeMission::ExecuteFakeMission(const std::string& params, int duration) : Command(duration) {
	}

	void ExecuteFakeMission::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->executeFakeMission();
	}

	int ExecuteFakeMission::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string ExecuteFakeMission::toString() const {
		return "";
//		return
//			"FAKEMISSION";
	}

//--ShowText--

	ShowText::ShowText(const std::string& params, int duration) : Command(duration) {
		if (strstr(params.c_str(), "ICON") == NULL) {
			text = params.substr(10, params.length() - 12);
			icon = "";
		} else {
			size_t index = params.find("ICON") + 5;
			icon = "RESOURCE\\ICONS\\Portraits\\" + params.substr(index, params.length() - index - 1) + ".tga";
			index = params.find('\"');
			size_t backIndex = params.rfind('\"');
			text = params.substr(index + 1, backIndex - index - 1);
		}

	}

	void ShowText::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->showText(text, icon);
	}

	int ShowText::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string ShowText::toString() const {
		return "";
//		return
//			"SHOWTEXT";
	}

//--HideText--

	HideText::HideText(const std::string& params, int duration) : Command(duration) {
	}

	void HideText::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->hideText();
	}

	int HideText::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string HideText::toString() const {
		return "";
//		return
//			"HIDETEXT";
	}

//--Pause--

	Pause::Pause(const std::string& params, int duration) : Command(duration) {
	}

	void Pause::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::CONTROL_SUBMIT_EVENT);
		commander->waitFor(Controller::CONTROL_SUBMIT_EVENT);
		commander->showPaused();
	}

	int Pause::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string Pause::toString() const {
		return "";
//		return
//			"PAUSE";
	}

//--SetSpeed--

	SetSpeed::SetSpeed(const std::string& params, int duration) : Command(duration) {
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

	std::string SetSpeed::toString() const {
		return "";
//		char str[31];
//		sprintf(str, "SETDURATION %f", speed);
//		return string(str);
	}

//--SetDelay--

	SetDelay::SetDelay(const std::string& params, int duration) : Command(duration) {
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

	std::string SetDelay::toString() const {
		return "";
//		char str[31];
//		sprintf(str, "SETDELAY %f", delay);
//		return string(str);
	}

//--StartAudio--

	StartAudio::StartAudio(const std::string& params, int duration) : Command(duration) {
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

	std::string StartAudio::toString() const {
		return "";
//		return 
//			"STARTAUDIO";
	}

//--StartVideo--

	StartVideo::StartVideo(const std::string& params, int duration) : Command(duration) {
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

	std::string StartVideo::toString() const {
		return "";
//		return 
//			"STARTVIDEO";
	}

//--StartInterVideo--

	StartInterVideo::StartInterVideo(const std::string& params, int duration) : Command(duration) {
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

	std::string StartInterVideo::toString() const {
		return "";
//		return 
//			"STARTINTERVIDEO";
	}

//--SetMusic--

	SetMusic::SetMusic(const std::string& params, int duration) : Command(duration) {
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

	std::string SetMusic::toString() const {
		return "";
//		return 
//			"SETMUSIC";
	}

//--AddCameraPosition--

	AddCameraPosition::AddCameraPosition(const std::string& params, int duration) : Command(duration) {
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
		theta = -theta - XM_PI / 2.0f;
		psi += XM_PI;
		pivotPosition.z = -pivotPosition.z;
	}

	void AddCameraPosition::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->addCameraPosition(pivotPosition, psi, theta, distanceToPivot);
	}

	int AddCameraPosition::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string AddCameraPosition::toString() const {
		return "";
//		return 
//			"ADDCAMERAPOINT";
	}

//--ClearCameraPath--

	ClearCameraPath::ClearCameraPath(const std::string& params, int duration) : Command(duration) {
	}

	void ClearCameraPath::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->clearCameraPath();
//		controller->waitFor(Controller::END_OF_CAMERA_EVENT);
	}

	int ClearCameraPath::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string ClearCameraPath::toString() const {
		return "";
//		return 
//			"CLEARCAMERA";
	}

//--CameraFollowNomad--

	CameraFollowNomad::CameraFollowNomad(const std::string& params, int duration) : Command(duration) {
		sscanf(
			params.c_str(),
			"CAMERAFOLLOWNOMAD %f %f %f",
			&deltaPsi,
			&radius,
			&theta );

		radius *= HISTORY_SCENE_SCALE;
		theta = -theta - XM_PI / 2.0f;
	}

	void CameraFollowNomad::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		commander->cameraFollowNomad(deltaPsi, radius, theta);
	}

	int CameraFollowNomad::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string CameraFollowNomad::toString() const {
		return "";
//		return 
//			"CAMERAFOLLOWNOMAD";
	}

//--WaitForCamera--

	WaitForCamera::WaitForCamera(const std::string& params, int duration) : Command(duration) {
	}

	void WaitForCamera::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::BEGIN_OF_CAMERA_EVENT);
		commander->waitFor(Controller::BEGIN_OF_CAMERA_EVENT);
	}

	int WaitForCamera::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string WaitForCamera::toString() const {
		return "";
//		return 
//			"WAITFORCAMERA";
	}

//--WaitForCameraPoint--

	WaitForCameraPoint::WaitForCameraPoint(const std::string& params, int duration) : Command(duration) {
	}

	void WaitForCameraPoint::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::POINT_OF_CAMERA_EVENT);
		commander->waitFor(Controller::POINT_OF_CAMERA_EVENT);
	}

	int WaitForCameraPoint::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string WaitForCameraPoint::toString() const {
		return "";
//		return 
//			"WAITFORCAMERAPOINT";
	}

//--UntilEndOfAudio--

	UntilEndOfAudio::UntilEndOfAudio(const std::string& params, int duration) : Command(duration) {
	}

	void UntilEndOfAudio::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::END_OF_AUDIO_EVENT);
		commander->waitFor(Controller::END_OF_AUDIO_EVENT);
	}

	int UntilEndOfAudio::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string UntilEndOfAudio::toString() const {
		return "";
//		return 
//			"UNTILENDOFAUDIO";
	}

//--UntilEndOfVideo--

	UntilEndOfVideo::UntilEndOfVideo(const std::string& params, int duration) : Command(duration) {
	}

	void UntilEndOfVideo::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::END_OF_VIDEO_EVENT);
		commander->waitFor(Controller::END_OF_VIDEO_EVENT);
	}

	int UntilEndOfVideo::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string UntilEndOfVideo::toString() const {
		return "";
//		return 
//			"UNTILENDOFVIDEO";
	}

//--UntilEndOfCamera--

	UntilEndOfCamera::UntilEndOfCamera(const std::string& params, int duration) : Command(duration) {
	}

	void UntilEndOfCamera::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::END_OF_CAMERA_EVENT);
		commander->waitFor(Controller::END_OF_CAMERA_EVENT);
	}

	int UntilEndOfCamera::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string UntilEndOfCamera::toString() const {
		return "";
//		return 
//			"WAITFORCAMERAEND";
	}

//--RollBack--

	RollBack::RollBack(const std::string& params, int duration) : Command(duration) {
	}

	void RollBack::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->rollback();
	}

	int RollBack::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string RollBack::toString() const {
		return 
			"ROLLBACK";
	}

//--BreakSkipping--

	BreakSkipping::BreakSkipping(const std::string& params, int duration) : Command(duration) {
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

	std::string BreakSkipping::toString() const {
		return "";
//		return 
//			"BREAKSKIPPING";
	}

//--FinishHistory--

	FinishHistory::FinishHistory(const std::string& params, int duration) : Command(duration) {
	}

	void FinishHistory::execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) {
		controller->waitFor(Controller::MISSION_VICTORY_EVENT);
		commander->finishHistory();
	}

	int FinishHistory::calculateLength(Worlds* worlds, Frames* frames) const {
		return getDuration();
	}

	std::string FinishHistory::toString() const {
		return 
			"FINISHHISTORY";
	}

//---Factory---

	Command* CommandFactory::createCommand(const std::string& command) {
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
