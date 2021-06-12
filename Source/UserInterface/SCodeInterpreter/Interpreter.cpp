
#include "StdAfx.h"
#include "Interpreter.hpp"

Interpreter::Year::~Year() {
	for (int i = 0, s = commands.size(); i < s; i++) {
		commands[i]->released();
	}
}

Interpreter::Interpreter(Commander* commander) : commander(commander) {
	commandFactory = new CommandFactory();
	frames = new Frames();
	worlds = new Worlds();
	nomadFrame = 0;
	currentYear = 0;
	rollbackDelta = 0;
	paused = false;
	waitForEvent = false;
	currentCommandNumber = 0;
	isCurrentCommandContinuous = false;
	yearTimeElapsed = 0;
	commandTimeElapsed = 0;

	yearDuration = 1000;
	delay = 0;
	ignoreControlCommands = false;
	shouldCheckYearDuration = true;
	ignoreExecuteMissionCommands = false;
}

Interpreter::~Interpreter() {
	delete commandFactory;
	clearYears();
	delete worlds;
	delete frames;
}

bool Interpreter::goToYear(int year) {
	bool res = false;
	if (year <= currentYear) {
		worlds->clear();
		frames->clear();
		nomadFrame = 0;
		for (int i = 0, s = continuousCommands.size(); i < s; i++) {
			continuousCommands[i]->released();
		}
		continuousCommands.clear();
		currentYear = 0;
		rollbackDelta = 0;
		yearDuration = 1000;
		delay = 0;
		waitForEvent = false;
		currentCommandNumber = 0;
		isCurrentCommandContinuous = false;
		yearTimeElapsed = 0;
		commandTimeElapsed = 0;
		missionWorlds.clear();
		res = true;
		commander->resetCamera();
	}

	delay = 0;
	yearTimeElapsed = 0;
	commandTimeElapsed = 0;
	waitForEvent = false;

	bool oldPaused = paused;
	paused = false;
	bool oldIgnoreMission = ignoreExecuteMissionCommands;
	ignoreExecuteMissionCommands = true;
	bool oldIngnoreControls = ignoreControlCommands;
	ignoreControlCommands = true;
	bool oldCheckYearDuration = shouldCheckYearDuration;
	shouldCheckYearDuration = false;
	while ( currentYear < year ) {
		quant(0);
	}
	ignoreControlCommands = oldIngnoreControls;
	shouldCheckYearDuration = oldCheckYearDuration;
	ignoreExecuteMissionCommands = oldIgnoreMission;
	paused = oldPaused;
	return res;
}

bool Interpreter::goToMission(int number) {
	if (number <= 0) { // ???
		goToYear(0);
		return true;
	} else {
		bool res = goToYear(missions[number].year);
		bool oldPaused = paused;
		paused = false;
		bool oldIgnoreMission = ignoreExecuteMissionCommands;
		ignoreExecuteMissionCommands = true;
		bool oldIngnoreControls = ignoreControlCommands;
		ignoreControlCommands = true;
		bool oldCheckYearDuration = shouldCheckYearDuration;
		shouldCheckYearDuration = false;
		while ( isCurrentCommandContinuous || currentCommandNumber < missions[number].commandNumber ) {
			quant(-1);
		}
		ignoreControlCommands = oldIngnoreControls;
		shouldCheckYearDuration = oldCheckYearDuration;
		ignoreExecuteMissionCommands = oldIgnoreMission;
		paused = oldPaused;
		return res;
	}
}
bool Interpreter::goToJustAfterMissionPosition(int number) {
	if (number < 0) {
		goToYear(0);
		return true;
	} else {
		bool res = goToYear(missions[number].exactYear);
		bool oldPaused = paused;
		paused = false;
		bool oldIgnoreMission = ignoreExecuteMissionCommands;
		ignoreExecuteMissionCommands = true;
		bool oldIngnoreControls = ignoreControlCommands;
		ignoreControlCommands = true;
		bool oldCheckYearDuration = shouldCheckYearDuration;
		shouldCheckYearDuration = false;
		while ( isCurrentCommandContinuous || currentCommandNumber <= missions[number].exactCommandNumber ) {
			quant(-1);
		}
		ignoreControlCommands = oldIngnoreControls;
		shouldCheckYearDuration = oldCheckYearDuration;
		ignoreExecuteMissionCommands = oldIgnoreMission;
		paused = oldPaused;
		return res;
	}
}
int Interpreter::findMission(const string& fileName) {
	for (int i = 0, s = missions.size(); i < s; i++) {
		if (missions[i].fileName == fileName) {
			return i;
		}
	}
	return -1;
}

void Interpreter::rollback() {
	worlds->clear();
	frames->clear();
	nomadFrame = 0;
	for (int i = 0, s = continuousCommands.size(); i < s; i++) {
		continuousCommands[i]->released();
	}
	continuousCommands.clear();
	rollbackDelta = currentYear + 1;
//	currentYear = 0;
	yearDuration = 1000;
	delay = 0;
	waitForEvent = false;
//	currentCommandNumber = 0;
	isCurrentCommandContinuous = false;
//	yearTimeElapsed = 0;
	commandTimeElapsed = 0;
	missionWorlds.clear();
//	commander->resetCamera();
}

void Interpreter::clearObjects3D() {
	for (int i = 0, s = continuousCommands.size(); i < s; i++) {
		continuousCommands[i]->setObject3D(0);
	}
}

void Interpreter::clearYears() {
	for (int i = 0, s = years.size(); i < s; i++) {
		if (years[i]) {
			years[i]->released();
		}
	}

	for (int i = 0, s = continuousCommands.size(); i < s; i++) {
		continuousCommands[i]->released();
	}
	continuousCommands.clear();
	missions.clear();
}

bool Interpreter::loadProgram(const string& fileName) {
	bool res = false;
	FILE* file = fopen(fileName.c_str(), "rt");
	if(file) {
		clearYears();
		res = true;
		char* buff = new char[201];

		int yearNumber = -1;
		while (fgets(buff, 200, file) != NULL) {
			if ( ferror(file) ) {
				res = false;
				break;
			}
			if ( strstr(buff, "[Year=") ) {
//				yearNumber = extractYear(buff);
				yearNumber++;
				if (yearNumber >= 0) {
					years.push_back(0);
				} else {
					res = false;
					break;
				}
			} else {
				Command* currentCommand = commandFactory->createCommand(buff);
				if (currentCommand) {
					currentCommand->referenced();
/*
					ShowText* sht = dynamic_cast<ShowText*>(currentCommand);
					if (sht) {
						string textToShow = ShowText::NAME + " ";
						int rowNumber = atoi(sht->getText().c_str());
						for (int i = 0; i < rowNumber; i++) {
							if ( fgets(buff, 200, file) == NULL || ferror(file) ) {
								res = false;
								break;
							}
//							textToShow = textToShow + "\n" + string(buff);
							textToShow = textToShow + string(buff);
						}

						currentCommand->released();
						if (!res) {
							break;
						}
						currentCommand = commandFactory->createCommand( textToShow );
						currentCommand->referenced();
					}
*/
					if (yearNumber >= 0) {
						if (!years[yearNumber]) {
							Year* year = new Year();
							year->referenced();
							years[yearNumber] = year;
						}
						years[yearNumber]->commands.push_back(currentCommand);
						ExecuteMission* em = dynamic_cast<ExecuteMission*>(currentCommand);
						if (em) {
							missions.push_back( Mission(
											em->getMissionName(),
											em->getFileName(),
											yearNumber,
											years[yearNumber]->commands.size() - 1) );
						}
						//BreakSkipping* bs = dynamic_cast<BreakSkipping*>(currentCommand);
						//if (bs && !missions.empty()) {
							//missions.back().year = yearNumber;
							//missions.back().commandNumber = years[yearNumber]->commands.size() - 1;
						//}
					} else {
						currentCommand->released();
						res = false;
						break;
					}
				}
			}
		}

		delete [] buff;
		fclose(file);
	}
	return res;
}

void Interpreter::quant(float dt) {
	if (!paused && !isHistoryFinished()) {
		if (!ignoreControlCommands && delay) {
			commandTimeElapsed += dt;
//			if (shouldCheckYearDuration) {
//				yearTimeElapsed += dt;
//			}
			if (commandTimeElapsed >= delay) {
				delay = 0;
				commandTimeElapsed = 0;
			}
			return;
		}
		if (ignoreControlCommands || !waitForEvent) {
			if (shouldCheckYearDuration) {
				yearTimeElapsed += dt;
			}

			if (dt == -1) {
				if (isCurrentCommandContinuous) {
					processContinuousCommand();
				} else {
					processYearCommand();
				}
			} else {
				while (isCurrentCommandContinuous && !waitForEvent && !delay) {
					processContinuousCommand();
				}
				if (!isCurrentCommandContinuous) {
					while (!waitForEvent && !delay && !processYearCommand()) {
					}
				}
			}
		}
	}
}

bool Interpreter::processContinuousCommand() {
	bool res = false;
	for (; currentCommandNumber < continuousCommands.size(); currentCommandNumber++) {
		if (continuousCommands[currentCommandNumber]->finished()) {
			ContinuousCommand* currentCommand = continuousCommands[currentCommandNumber];
			ExecuteMission* em = dynamic_cast<ExecuteMission*>(currentCommand->command);
			if (em && nomadFrame) {
				missionWorlds.push_back(nomadFrame->getCurrentWorld());
			}
			BreakSkipping* bs = dynamic_cast<BreakSkipping*>(currentCommand->command);
			if (
					(!ignoreControlCommands || !currentCommand->command->isControlCommand())
				&&	(!em || !ignoreExecuteMissionCommands)
				&&	(!bs || !ignoreExecuteMissionCommands)
				) {

				addLineToLog(currentCommand->command->toString());
				currentCommand->command->execute(worlds, frames, this, commander);
				if (currentCommand->getObject3D()) {
					currentCommand->command->setObject3D(worlds, frames, currentCommand->getObject3D());
				}
			}
			currentCommand->released();
			continuousCommands.erase(continuousCommands.begin() + currentCommandNumber);
			res = true;
			break;
		}
	}
	if ( currentCommandNumber >= continuousCommands.size() ) {
		currentCommandNumber = 0;
		isCurrentCommandContinuous = false;
	}
	return res;
}
bool Interpreter::processYearCommand() {
	if (years[currentYear]) {
		for (int s = years[currentYear]->commands.size(); currentCommandNumber < s; currentCommandNumber++) {
			Command* currentCommand = years[currentYear]->commands[currentCommandNumber];
			if (currentCommand->getDuration()) {
				ContinuousCommand* cCommand = new ContinuousCommand(
													currentCommand,
													currentCommand->calculateLength(worlds, frames) );
				cCommand->referenced();
				continuousCommands.push_back(cCommand);
			} else {
				ExecuteMission* em = dynamic_cast<ExecuteMission*>(currentCommand);
				if (em && nomadFrame) {
					missionWorlds.push_back(nomadFrame->getCurrentWorld());
				}
				BreakSkipping* bs = dynamic_cast<BreakSkipping*>(currentCommand);
				if (
						(!ignoreControlCommands || !currentCommand->isControlCommand())
					&&	(!em || !ignoreExecuteMissionCommands)
					&&	(!bs || !ignoreExecuteMissionCommands)
					) {

					addLineToLog(currentCommand->toString());
					currentCommand->execute(worlds, frames, this, commander);
				}
				currentCommandNumber++;
				return false;
			}	
		}
	}
	if (!shouldCheckYearDuration || yearTimeElapsed >= yearDuration) {
		currentYear++;
		char str[30];
		sprintf( str, "year=%d", currentYear - rollbackDelta);
		addLineToLog(string(str));
		yearTimeElapsed = 0;
		currentCommandNumber = 0;
		isCurrentCommandContinuous = (continuousCommands.size() > 0);
		for (int i = 0, s = continuousCommands.size(); i < s; i++) {
			continuousCommands[i]->yearPassed();
		}
	}
	return true;
}

bool Interpreter::isHistoryFinished() {
	return ( currentYear >= years.size() && continuousCommands.size() == 0 );
}

int Interpreter::extractYear(char* year) {
	int res = -1;
	sscanf(year, "[Year=%d]",&res);
	return res;
}

//---Controller Implementation---
void Interpreter::stopFrame(const Frame* frame) {
	vector<ContinuousCommand*>::iterator it = continuousCommands.begin();
	while (it != continuousCommands.end()) {
		FrameCommand* fCommand = dynamic_cast<FrameCommand*>((*it)->command);
		if ( fCommand && fCommand->isCommandForFrame(frame) ) {
			(*it)->released();
			it = continuousCommands.erase(it);
		} else {
			it++;
		}
	}
}
void Interpreter::executeFakeMission() {
	if (nomadFrame) {
		missionWorlds.push_back(nomadFrame->getCurrentWorld());
	}
}

void Interpreter::nomadToFrame(Frame* frame) {
	nomadFrame = frame;
}
void Interpreter::waitFor(WaitEventType event) {
	currentWaitEvent = event;
	waitForEvent = true;
}
void Interpreter::eventOccured(WaitEventType event) {
	if (waitForEvent && currentWaitEvent == event) {
		waitForEvent = false;
	}
}
void Interpreter::setYearDuration(float duration) {
	yearDuration = duration;
}
void Interpreter::setDelay(float newDelay) {
	delay = newDelay;
	commandTimeElapsed = 0;
}

void Interpreter::addLineToLog(const string& line) {
	if (!line.empty()) {
		if (log.empty()) {
			for (int i = 0; i < 108; i++) {
				log += "\n";
			}
			log += line + "\n";
		} else {
			int returnCh = log.find("\n");
			log.erase(0, returnCh + 1);
			log += line + "\n";
		}
	}

}

void ContinuousCommand::setObject3D(Object3D* newObject3D) {
	if (object3D) {
		object3D->released();
	}
	object3D = newObject3D;
	if (object3D) {
		object3D->referenced();
	}
}

/*
int main(int argc, char *argv[])
{

	Interpreter* interpreter = new Interpreter();
	bool res = interpreter->loadProgram(argv[1]);
	if (res) {
		for (int i = 0, s = interpreter->years.size(); i < s; i++) {
			interpreter->quant();
		}
		while ( !interpreter->isHistoryFinished() ) {
			interpreter->quant();
		}
//		map <string, Frame*>::iterator it;
//		map <string, Frame*>::iterator end = interpreter->frames->frames.end();
//
//		for (it = interpreter->frames->frames.begin(); it != end; it++) {
//			Frame* frame = it->second;
//		}

//		map <string, World*>::iterator it;
//		map <string, World*>::iterator end = worlds.end();
//		for (it = worlds.begin(); it != end; it++) {
//			it->second->released();
//		}

//------test commands-----------------
//		for (int i = 0, s = interpreter->years.size(); i < s; i++) {
//			if (i == 255) {
//				int g = 0;
//			}
//			if (i == 998) {
//				int g = 0;
//			}
//			if (interpreter->years[i]) {
//				for (int j = 0, s1 = interpreter->years[i]->commands.size(); j < s1; j++) {
//					Command* c = interpreter->years[i]->commands[j];
//				}
//			}
//		}
//		int k = 0;
//		for (int i = 0, s = interpreter->years.size(); i < s; i++) {
//			if (i == 255) {
//				int g = 0;
//			}
//			if (i == 998) {
//				int g = 0;
//			}
//			if (interpreter->years[i]) {
//				for (int j = 0, s1 = interpreter->years[i]->commands.size(); j < s1; j++) {
//					Command* c = interpreter->years[i]->commands[j];
//					k++;
//				}
//			}
//		}
//		int h = 0;

//		interpreter->worlds->getWorldBySysName("Al-1[6]")->destroy();
	}
	delete interpreter;
	return 0;
}
*/
