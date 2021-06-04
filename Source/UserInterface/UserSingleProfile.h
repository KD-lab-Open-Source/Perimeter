#pragma once

#ifndef _USERSINGLEPROFILE_H
#define _USERSINGLEPROFILE_H

#include "UniverseInterface.h"

struct Profile {
	Profile(const string& dirName) : dirName(dirName), lastMissionNumber(-1), name(""), difficulty(DIFFICULTY_HARD) {
		dirIndex = atof( (dirName.substr(7)).c_str() );
	}
	int dirIndex;
	string name;
	int lastMissionNumber;
	string dirName;
	Difficulty difficulty;
};

class UserSingleProfile {
	public:
		enum GameType {
			SCENARIO,
			BATTLE,
			SURVIVAL,
			LAN,
			REPLAY,
			UNDEFINED
		};
		UserSingleProfile();

		void setDifficulty(Difficulty newDifficulty);
		void setCurrentMissionNumber(int newMissionNumber);
		void setLastMissionNumber(int newMissionNumber);

		void setLastWin(bool newLastWin) {
			lastWin = newLastWin;
		}
		void setLastGameType(GameType type) {
			lastType = type;
		}
		Difficulty getDifficulty() const {
			xassert(currentProfileIndex >= 0 && currentProfileIndex < profiles.size());
			return profiles[currentProfileIndex].difficulty;
		}
		int getCurrentMissionNumber() const {
			return currentMissionNumber;
		}
		int getLastMissionNumber() const {
			xassert(currentProfileIndex >= 0 && currentProfileIndex < profiles.size());
			return profiles[currentProfileIndex].lastMissionNumber;
		}
		bool isLastWin() const {
			return lastWin;
		}
		GameType getLastGameType() const {
			return lastType;
		}

		void scanProfiles();

		const Profile& getCurrentProfile() const {
			return profiles[currentProfileIndex];
		}
		const vector<Profile>& getProfilesVector() const {
			return profiles;
		}
		void addProfile(const string& name);
		void removeProfile(int index);

		int getCurrentProfileIndex() const {
			return currentProfileIndex;
		}
		void setCurrentProfileIndex(int index);

		string getFileNameWithDifficulty(const string& fileName);

		void deleteSave(const string& name);
		string getSavesDirectory() const;

		void setCurrentProfile(const string& name);
		void setGameResult(terUniverseInterfaceMessage gameResult) {
			result = gameResult;
		}
		terUniverseInterfaceMessage getGameResult() const {
			return result;
		}
		void setRecord(const string& name, int milis);
		int getRecord(const string& name);

	protected:
		bool removeDir(const string& dir);
		void loadProfile(int index);

		int currentProfileIndex;

		int currentMissionNumber;
		bool lastWin;
		GameType lastType;

		vector<bool> freeInds;
		vector<Profile> profiles;

		string getProfileIniPath(int index) const {
			return "RESOURCE\\SAVES\\" + profiles[index].dirName + "\\data";
		}

		terUniverseInterfaceMessage result;
};

#ifdef _PERIMETER_ADDON_
const int firstMissionNumber = 0;
#else
const int firstMissionNumber = 1;
#endif

#endif //_USERSINGLEPROFILE_H