#pragma once

#ifndef _USERSINGLEPROFILE_H
#define _USERSINGLEPROFILE_H

#include "UniverseInterface.h"

struct Profile {
	Profile(const std::string& dirName) : dirName(dirName), lastMissionNumber(-1), name(""), difficulty(DIFFICULTY_HARD) {
		dirIndex = atof( (dirName.substr(7)).c_str() );
	}
	int dirIndex;
	std::string name;
	int lastMissionNumber;
	std::string dirName;
	Difficulty difficulty;
};

class UserSingleProfile {
	public:
		enum GameType {
			SCENARIO,
			BATTLE,
			SURVIVAL,
			MULTIPLAYER,
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
		const std::vector<Profile>& getProfilesVector() const {
			return profiles;
		}
		void addProfile(const std::string& name);
		void removeProfile(int index);

		int getCurrentProfileIndex() const {
			return currentProfileIndex;
		}
		void setCurrentProfileIndex(int index);

		std::string getFileNameWithDifficulty(const std::string& fileName);

		void deleteSave(const std::string& name);
        static std::string getAllSavesDirectory();
		std::string getSavesDirectory() const;

		void setCurrentProfile(const std::string& name);
		void setGameResult(terUniverseInterfaceMessage gameResult) {
			result = gameResult;
		}
		terUniverseInterfaceMessage getGameResult() const {
			return result;
		}
		void setRecord(const std::string& name, int milis);
		int getRecord(const std::string& name);

	protected:
		bool removeDir(const std::string& dir);
		void loadProfile(int index);

		int currentProfileIndex;

		int currentMissionNumber;
		bool lastWin;
		GameType lastType;

		std::vector<bool> freeInds;
		std::vector<Profile> profiles;

		std::string getProfileIniPath(int index) const {
			return getAllSavesDirectory() + profiles[index].dirName + PATH_SEP + "data";
		}

		terUniverseInterfaceMessage result;
};

extern int firstMissionNumber;

#endif //_USERSINGLEPROFILE_H