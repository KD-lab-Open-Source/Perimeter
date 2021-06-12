#pragma once

#ifndef _INTERPRETER_HPP
#define _INTERPRETER_HPP

#include "Commander.hpp"
#include "Controller.hpp"
#include "Commands.hpp"
#include "Containers.hpp"

struct Mission {
	Mission(const Mission& origin) {
		name = origin.name;
		fileName = origin.fileName;
		year = origin.year;
		commandNumber = origin.commandNumber;
		exactYear = origin.exactYear;
		exactCommandNumber = origin.exactCommandNumber;
	}
	Mission(const string& name, const string& fileName, int year, int commandNumber)
		: name(name), fileName(fileName), year(year), commandNumber(commandNumber) {
		exactYear = year;
		exactCommandNumber = commandNumber;
	}
	string name;
	string fileName;
	int year;
	int commandNumber;
	int exactYear;
	int exactCommandNumber;
};

class ContinuousCommand : public tx3d::SharedPointer {
	public:
		ContinuousCommand(Command* command, int length)
			:	command(command),
				length(length),
				fullLength(length),
				object3D(0) {

			command->referenced();
		}
		~ContinuousCommand() {
			command->released();
			if (object3D) {
				object3D->released();
			}
		}
		void yearPassed() {
			length--;
		}
		bool finished() {
			return (length == 0);
		}
		float phase(float yearPhase) {
			return 1.0f - (length - yearPhase) / fullLength;
		}
		Object3D* getObject3D() {
			return object3D;
		}
		void setObject3D(Object3D* newObject3D);

		Command* command;
	protected:
		int length;
		int fullLength;
		Object3D* object3D;
};

class Interpreter : public Controller {
	public:
		Interpreter(Commander* commander);
		virtual ~Interpreter();
		void clearObjects3D();
		bool loadProgram(const string& fileName);
		int findMission(const string& fileName);
		bool isHistoryFinished();
		void quant(float dt);

		bool goToYear(int year);
		bool goToMission(int number);
		bool goToJustAfterMissionPosition(int number);

		void rollback();
		
		void waitFor(WaitEventType event);
		void stopFrame(const Frame* frame);
		void nomadToFrame(Frame* frame);
		void setYearDuration(float duration);
		void eventOccured(WaitEventType event);
		void setDelay(float delay);
		void executeFakeMission();

		Frame* getNomadFrame() {
			return nomadFrame;
		}

		void setNormalSpeedMode(bool normal) {
			ignoreControlCommands = !normal;
			shouldCheckYearDuration = normal;
			delay = 0;
		}

		void breakSkipping() {
			if (!isNormalSpeedMode()) {
				setNormalSpeedMode(true);
			}
		}

		bool isNormalSpeedMode() const {
			return (!ignoreControlCommands && shouldCheckYearDuration);
		}

		void setPaused(bool isPaused) {
			paused = isPaused;
		}
		bool isPaused() {
			return paused;
		}
		float getYearPhase() {
			return yearDuration ? (yearTimeElapsed / yearDuration) : 0;
		}

		int getCurrentYear() const {
			return currentYear - rollbackDelta;
		}

		int getRealCurrentYear() const {
			return currentYear;
		}

		void addLineToLog(const string& line);
		string log;

	protected:
		class Year : public tx3d::SharedPointer {
			public:
				~Year();
				vector<Command*> commands;
		};
		void clearYears();
		bool processContinuousCommand();
		bool processYearCommand();
		static int extractYear(char* year);

		Frame* nomadFrame;

		Commander* commander;

		int currentYear;
		int rollbackDelta;
		int currentCommandNumber;
		bool isCurrentCommandContinuous;
		float yearTimeElapsed;
		float commandTimeElapsed;
		float yearDuration;
		float delay;
		bool ignoreExecuteMissionCommands;
		bool ignoreControlCommands;
		bool shouldCheckYearDuration;

		CommandFactory* commandFactory;

		bool waitForEvent;
		WaitEventType currentWaitEvent;
		bool paused;

		vector<Year*> years;
	
	public:
		Frames* frames;
		Worlds* worlds;
		vector<Mission> missions;
		vector<ContinuousCommand*> continuousCommands;
		vector<World*> missionWorlds;

};


#endif //_INTERPRETER_HPP