#pragma once

#ifndef _COMMANDS_HPP
#define _COMMANDS_HPP

#include "Containers.hpp"
#include "Controller.hpp"
#include "Commander.hpp"

//---Abstract---
class Command : public tx3d::SharedPointer {
	public:
		Command(int duration) : duration(duration) {
		}
		virtual void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander) = 0;
		virtual int calculateLength(Worlds* worlds, Frames* frames) const = 0;
		virtual void setObject3D(Worlds* worlds, Frames* frames, Object3D* object3D) {
		}
		virtual string toString() const {
			return "UNKNOWN";
		}

		virtual bool isControlCommand() const {
			return false;
		}
		int getDuration() const {
			return duration;
		}
	protected:
		int duration;
};

class FrameCommand : public Command {
	public:
		FrameCommand(int duration) : Command(duration) {
		}
		virtual Frame* getFrame(Frames* frames) const {
			return frames->getFrameByName(frameName);
		}
		virtual bool isCommandForFrame(const Frame* frame) const {
			return (frame->getName() == frameName);
		}
	protected:
		string frameName;
};

//---Zero-Commands---
class CreateZeroWorld : public Command {
	public:
		CreateZeroWorld(const string& params, int duration);
		~CreateZeroWorld();
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
	protected:
		World* origin;
};
class CreateZeroLink : public Command {
	public:
		CreateZeroLink(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
	protected:
		string world1;
		string world2;
};

//---Usual-Commands---
class KillAlphaPortal : public Command {
	public:
		KillAlphaPortal(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
	protected:
		string worldName;
};
class KillOmegaPortal : public Command {
	public:
		KillOmegaPortal(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
	protected:
		string worldName;
};
class KillWorld : public Command {
	public:
		KillWorld(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
	protected:
		string worldName;
};
class RenameWorld : public Command {
	public:
		RenameWorld(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
	protected:
		string worldSysName;
		string worldName;
};

//---Frame-Commands---
class JumpToWorld : public FrameCommand {
	public:
		JumpToWorld(const string& params, int duration);
		~JumpToWorld();
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		const Vect3f& getWorldCoords();
		string toString() const;
		static const string NAME;
	protected:
		World* origin;
};
class BuildFrame : public FrameCommand {
	public:
		BuildFrame(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
	protected:
		string worldSysName;
		int race;
		int colorIndex;
		bool shouldCreateNomad;
};
class ChangeFrameRace : public FrameCommand {
	public:
		ChangeFrameRace(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
	protected:
		int race;
		int colorIndex;
};
class CreateAlphaPortal : public FrameCommand {
	public:
		CreateAlphaPortal(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		void setObject3D(Worlds* worlds, Frames* frames, Object3D* object3D);
		string toString() const;
		static const string NAME;
};
class CreateOmegaPortal : public FrameCommand {
	public:
		CreateOmegaPortal(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
};
class CreateLink : public FrameCommand {
	public:
		CreateLink(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		void setObject3D(Worlds* worlds, Frames* frames, Object3D* object3D);
		World* getWorld(Worlds* worlds);
		string toString() const;
		static const string NAME;
	protected:
		string worldSysName;
};
class TransferFrame : public FrameCommand {
	public:
		TransferFrame(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		World* getWorldToTransfer(Worlds* worlds);
		string toString() const;
		static const string NAME;
	protected:
		string worldSysName;
};
class ReachWorld : public FrameCommand {
	public:
		ReachWorld(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		World* getWorldToReach(Worlds* worlds);
		string toString() const;
		static const string NAME;
	protected:
		string worldSysName;
};
class DestroyFrame : public FrameCommand {
	public:
		DestroyFrame(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
};
class SendTestament : public FrameCommand {
	public:
		SendTestament(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
	protected:
		bool checked(World* world) {
			return 
//				find(pool.begin(), pool.begin() + checkedBound, world) != pool.begin() + checkedBound;
				find(pool.begin(), pool.end(), world) != pool.end();
		}
		void connectedToForCheck(World* world);

		vector <World*> pool;
		int checkedBound;
		int forCheckBound;
		
		string nomadFrameName;
		bool shouldSendNomad;
		int depth;
};
class StopFrame : public FrameCommand {
	public:
		StopFrame(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
};

//---Control-Commands---
class ExecuteMission : public Command {
	public:
		ExecuteMission(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		const string& getMissionName() const {
			return missionName;
		}
		const string& getFileName() const {
			return fileName;
		}
		string toString() const;
		static const string NAME;
	protected:
		string missionName;
		string fileName;
};
class ExecuteFakeMission : public Command {
	public:
		ExecuteFakeMission(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
};
class ShowText : public Command {
	public:
		ShowText(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		const string& getText() const {
			return text;
		}
		string toString() const;
		static const string NAME;
	protected:
		string text;
		string icon;
};
class HideText : public Command {
	public:
		HideText (const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		string toString() const;
		static const string NAME;
};
class Pause : public Command {
	public:
		Pause(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		string toString() const;
		static const string NAME;
};
class SetSpeed : public Command {
	public:
		SetSpeed(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		string toString() const;
		static const string NAME;
	protected:
		float speed;
};
class SetDelay : public Command {
	public:
		SetDelay(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		string toString() const;
		static const string NAME;
	protected:
		float delay;
};
class StartAudio : public Command {
	public:
		StartAudio(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		string toString() const;
		static const string NAME;
	protected:
		string audioName;
};
class StartVideo : public Command {
	public:
		StartVideo(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		string toString() const;
		static const string NAME;
	protected:
		string reelName;
		string sndName;
};
class StartInterVideo : public Command {
	public:
		StartInterVideo(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		string toString() const;
		static const string NAME;
	protected:
		string reelName;
};
class SetMusic : public Command {
	public:
		SetMusic(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		string toString() const;
		static const string NAME;
	protected:
		string path;
};

class AddCameraPosition : public Command {
	public:
		AddCameraPosition(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		string toString() const;
		static const string NAME;
	protected:
		Vect3f pivotPosition;
		float psi;
		float theta;
		float distanceToPivot;
};

class ClearCameraPath : public Command {
	public:
		ClearCameraPath(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		string toString() const;
		static const string NAME;
};

class CameraFollowNomad : public Command {
	public:
		CameraFollowNomad(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		string toString() const;
		static const string NAME;
	protected:
		float deltaPsi;
		float radius;
		float theta;
};

class WaitForCamera : public Command {
	public:
		WaitForCamera(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		string toString() const;
		static const string NAME;
};

class WaitForCameraPoint : public Command {
	public:
		WaitForCameraPoint(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		string toString() const;
		static const string NAME;
};

class UntilEndOfAudio: public Command {
	public:
		UntilEndOfAudio(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		string toString() const;
		static const string NAME;
};

class UntilEndOfVideo: public Command {
	public:
		UntilEndOfVideo(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		string toString() const;
		static const string NAME;
};

class UntilEndOfCamera: public Command {
	public:
		UntilEndOfCamera(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		string toString() const;
		static const string NAME;
};

class RollBack : public Command {
	public:
		RollBack(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
};

class BreakSkipping : public Command {
	public:
		BreakSkipping(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
};

class FinishHistory : public Command {
	public:
		FinishHistory(const string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		string toString() const;
		static const string NAME;
};

//---Factory---
class CommandFactory {
	public:
		CommandFactory() {
		}
		Command* createCommand(const string& command);
		static bool commandHasName(const string& command, const string& name) {
			return (command.find(name.c_str()) == 0);
		}
};

#endif //_COMMANDS_HPP

