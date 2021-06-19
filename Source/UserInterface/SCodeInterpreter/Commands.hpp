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
		virtual std::string toString() const {
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
		std::string frameName;
};

//---Zero-Commands---
class CreateZeroWorld : public Command {
	public:
		CreateZeroWorld(const std::string& params, int duration);
		~CreateZeroWorld();
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
	protected:
		World* origin;
};
class CreateZeroLink : public Command {
	public:
		CreateZeroLink(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string world1;
		std::string world2;
};

//---Usual-Commands---
class KillAlphaPortal : public Command {
	public:
		KillAlphaPortal(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string worldName;
};
class KillOmegaPortal : public Command {
	public:
		KillOmegaPortal(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string worldName;
};
class KillWorld : public Command {
	public:
		KillWorld(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string worldName;
};
class RenameWorld : public Command {
	public:
		RenameWorld(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string worldSysName;
		std::string worldName;
};

//---Frame-Commands---
class JumpToWorld : public FrameCommand {
	public:
		JumpToWorld(const std::string& params, int duration);
		~JumpToWorld();
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		const Vect3f& getWorldCoords();
		std::string toString() const;
		static const std::string NAME;
	protected:
		World* origin;
};
class BuildFrame : public FrameCommand {
	public:
		BuildFrame(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string worldSysName;
		int race;
		int colorIndex;
		bool shouldCreateNomad;
};
class ChangeFrameRace : public FrameCommand {
	public:
		ChangeFrameRace(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
	protected:
		int race;
		int colorIndex;
};
class CreateAlphaPortal : public FrameCommand {
	public:
		CreateAlphaPortal(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		void setObject3D(Worlds* worlds, Frames* frames, Object3D* object3D);
		std::string toString() const;
		static const std::string NAME;
};
class CreateOmegaPortal : public FrameCommand {
	public:
		CreateOmegaPortal(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
};
class CreateLink : public FrameCommand {
	public:
		CreateLink(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		void setObject3D(Worlds* worlds, Frames* frames, Object3D* object3D);
		World* getWorld(Worlds* worlds);
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string worldSysName;
};
class TransferFrame : public FrameCommand {
	public:
		TransferFrame(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		World* getWorldToTransfer(Worlds* worlds);
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string worldSysName;
};
class ReachWorld : public FrameCommand {
	public:
		ReachWorld(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		World* getWorldToReach(Worlds* worlds);
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string worldSysName;
};
class DestroyFrame : public FrameCommand {
	public:
		DestroyFrame(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
};
class SendTestament : public FrameCommand {
	public:
		SendTestament(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
	protected:
		bool checked(World* world) {
			return 
//				find(pool.begin(), pool.begin() + checkedBound, world) != pool.begin() + checkedBound;
				std::find(pool.begin(), pool.end(), world) != pool.end();
		}
		void connectedToForCheck(World* world);

		std::vector <World*> pool;
		int checkedBound;
		int forCheckBound;
		
		std::string nomadFrameName;
		bool shouldSendNomad;
		int depth;
};
class StopFrame : public FrameCommand {
	public:
		StopFrame(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
};

//---Control-Commands---
class ExecuteMission : public Command {
	public:
		ExecuteMission(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		const std::string& getMissionName() const {
			return missionName;
		}
		const std::string& getFileName() const {
			return fileName;
		}
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string missionName;
		std::string fileName;
};
class ExecuteFakeMission : public Command {
	public:
		ExecuteFakeMission(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
};
class ShowText : public Command {
	public:
		ShowText(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		const std::string& getText() const {
			return text;
		}
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string text;
		std::string icon;
};
class HideText : public Command {
	public:
		HideText (const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		std::string toString() const;
		static const std::string NAME;
};
class Pause : public Command {
	public:
		Pause(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		std::string toString() const;
		static const std::string NAME;
};
class SetSpeed : public Command {
	public:
		SetSpeed(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		std::string toString() const;
		static const std::string NAME;
	protected:
		float speed;
};
class SetDelay : public Command {
	public:
		SetDelay(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		std::string toString() const;
		static const std::string NAME;
	protected:
		float delay;
};
class StartAudio : public Command {
	public:
		StartAudio(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string audioName;
};
class StartVideo : public Command {
	public:
		StartVideo(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string reelName;
		std::string sndName;
};
class StartInterVideo : public Command {
	public:
		StartInterVideo(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string reelName;
};
class SetMusic : public Command {
	public:
		SetMusic(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		std::string toString() const;
		static const std::string NAME;
	protected:
		std::string path;
};

class AddCameraPosition : public Command {
	public:
		AddCameraPosition(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		std::string toString() const;
		static const std::string NAME;
	protected:
		Vect3f pivotPosition;
		float psi;
		float theta;
		float distanceToPivot;
};

class ClearCameraPath : public Command {
	public:
		ClearCameraPath(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		std::string toString() const;
		static const std::string NAME;
};

class CameraFollowNomad : public Command {
	public:
		CameraFollowNomad(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return false;
		}
		std::string toString() const;
		static const std::string NAME;
	protected:
		float deltaPsi;
		float radius;
		float theta;
};

class WaitForCamera : public Command {
	public:
		WaitForCamera(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		std::string toString() const;
		static const std::string NAME;
};

class WaitForCameraPoint : public Command {
	public:
		WaitForCameraPoint(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		std::string toString() const;
		static const std::string NAME;
};

class UntilEndOfAudio: public Command {
	public:
		UntilEndOfAudio(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		std::string toString() const;
		static const std::string NAME;
};

class UntilEndOfVideo: public Command {
	public:
		UntilEndOfVideo(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		std::string toString() const;
		static const std::string NAME;
};

class UntilEndOfCamera: public Command {
	public:
		UntilEndOfCamera(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		bool isControlCommand() const {
			return true;
		}
		std::string toString() const;
		static const std::string NAME;
};

class RollBack : public Command {
	public:
		RollBack(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
};

class BreakSkipping : public Command {
	public:
		BreakSkipping(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
};

class FinishHistory : public Command {
	public:
		FinishHistory(const std::string& params, int duration);
		void execute(Worlds* worlds, Frames* frames, Controller* controller, Commander* commander);
		int calculateLength(Worlds* worlds, Frames* frames) const;
		std::string toString() const;
		static const std::string NAME;
};

//---Factory---
class CommandFactory {
	public:
		CommandFactory() {
		}
		Command* createCommand(const std::string& command);
		static bool commandHasName(const std::string& command, const std::string& name) {
			return (command.find(name.c_str()) == 0);
		}
};

#endif //_COMMANDS_HPP

