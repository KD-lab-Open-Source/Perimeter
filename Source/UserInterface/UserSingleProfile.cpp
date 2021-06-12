
#include "StdAfx.h"
#include "UserSingleProfile.h"
#include "GameShellSq.h"
#include "Config.h"

UserSingleProfile::UserSingleProfile() :
		currentMissionNumber(-1),
		currentProfileIndex(-1),
		result(UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED),
		lastType(UNDEFINED) {
}
void UserSingleProfile::setDifficulty(Difficulty newDifficulty) {
	profiles[currentProfileIndex].difficulty = newDifficulty;
	string path = getProfileIniPath(currentProfileIndex);
	IniManager man( path.c_str(), true );
	man.putInt("General", "difficulty", newDifficulty);
}
void UserSingleProfile::setCurrentMissionNumber(int newMissionNumber) {
	currentMissionNumber = newMissionNumber;
	//save();
}
void UserSingleProfile::setLastMissionNumber(int newMissionNumber) {
	profiles[currentProfileIndex].lastMissionNumber = newMissionNumber;
	string path = getProfileIniPath(currentProfileIndex);
	IniManager man( path.c_str(), true );
	man.putInt("General", "lastMissionNumber", newMissionNumber);
}

void UserSingleProfile::scanProfiles() {
	profiles.clear();

	int maxIndex = -1;

	WIN32_FIND_DATA FindFileData;
	HANDLE hf = FindFirstFile( "RESOURCE\\SAVES\\Profile*", &FindFileData );
	if(hf != INVALID_HANDLE_VALUE){
		do{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				profiles.push_back( Profile(FindFileData.cFileName) );
			}
		} while(FindNextFile( hf, &FindFileData ));
		FindClose( hf );
	}
	for (int i = 0, s = profiles.size(); i < s; i++) {
		loadProfile(i);
		maxIndex = max(maxIndex, profiles[i].dirIndex);
	}
	freeInds.clear();
	if (maxIndex != -1) {
		freeInds.resize(maxIndex + 1);
		for (int i = 0, s = freeInds.size(); i < s; i++) {
			freeInds[i] = false;
		}
		for (int i = 0, s = profiles.size(); i < s; i++) {
			freeInds[profiles[i].dirIndex] = true;
		}
	}
	vector<Profile>::iterator it = profiles.begin();
	while (it != profiles.end()) {
		if ( (*it).name.empty() ) {
			it = profiles.erase(it);
		} else {
			it++;
		}
	}
}
void UserSingleProfile::addProfile(const string& name) {
	int i;
	int s;
	for (i = 0, s = freeInds.size(); i < s; i++) {
		if (!freeInds[i]) {
			break;
		}
	}
	char ind[18];
	sprintf(ind, "Profile%d", i);
	Profile newProfile(ind);
	newProfile.name = name;
	string root = "RESOURCE\\SAVES\\";
	string path = root + newProfile.dirName;
	string origin = "RESOURCE\\SAVES\\DefaultPlayerData";
	if( CreateDirectory(path.c_str(), NULL) ) {
		path += "\\data";
		if ( CopyFile(origin.c_str(), path.c_str(), FALSE) ) {
			profiles.push_back( newProfile );
			IniManager man( path.c_str(), true );
			man.put("General", "name", name.c_str());
			man.putInt("General", "lastMissionNumber", firstMissionNumber);
			if (i == freeInds.size()) {
				freeInds.push_back(true);
			} else {
				freeInds[i] = true;
			}
			loadProfile(profiles.size() - 1);
		} else {
			ErrH.Abort("Can't copy: ", XERR_USER, 0, origin.c_str());
		}
	} else {
		ErrH.Abort("Can't create directory: ", XERR_USER, 0, path.c_str());
	}
}

bool UserSingleProfile::removeDir(const string& dir) {
	WIN32_FIND_DATA findFileData;
	string mask = dir + "*.*";
	HANDLE hf = FindFirstFile( mask.c_str(), &findFileData );
	if (hf != INVALID_HANDLE_VALUE) {
		do {
			if ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
				DeleteFile((dir + findFileData.cFileName).c_str());
			}
		} while(FindNextFile( hf, &findFileData ));
		FindClose( hf );
	}
	return RemoveDirectory(dir.c_str());
}

void UserSingleProfile::removeProfile(int index) {
	if (removeDir("RESOURCE\\SAVES\\" + profiles[index].dirName + "\\")) {
		freeInds[profiles[index].dirIndex] = false;
	}	

	vector<Profile>::iterator forErase = profiles.begin();
	advance(forErase, index);
	profiles.erase(forErase);

	if ( getCurrentProfileIndex() >= index ) {
		if ( getCurrentProfileIndex() || (profiles.size() == 0)) {
			setCurrentProfileIndex(getCurrentProfileIndex() - 1);
		} else {
			setCurrentProfileIndex(0);
		}
	}
}

void UserSingleProfile::setCurrentProfileIndex(int index) {
	currentProfileIndex = index;
}

void UserSingleProfile::deleteSave(const string& name) {
	string fullName = getSavesDirectory() + name;
	DeleteFile( (fullName + ".spg").c_str() );
	DeleteFile( (fullName + ".gmp").c_str() );
	DeleteFile( (fullName + ".dat").c_str() );
	DeleteFile( (fullName + ".sph").c_str() );
}

string UserSingleProfile::getSavesDirectory() const {
//	return "RESOURCE\\SAVES\\";
	return "RESOURCE\\SAVES\\" + profiles[currentProfileIndex].dirName + "\\";
}

void UserSingleProfile::loadProfile(int index) {
	string path = getProfileIniPath(index);
	IniManager man(path.c_str(), false);
	profiles[index].name = man.get("General","name");
	profiles[index].lastMissionNumber = man.getInt("General","lastMissionNumber");
	profiles[index].difficulty = (Difficulty)man.getInt("General","difficulty");
}

string UserSingleProfile::getFileNameWithDifficulty(const string& fileName) {
	string fileNameWithoutExt = fileName;
	fileNameWithoutExt.erase(fileNameWithoutExt.size() - 4, fileNameWithoutExt.size());
	string res = MISSIONS_PATH;
	res += "\\";
	res += fileNameWithoutExt;
	res += missionDifficultyPostfix[getDifficulty()]; 

	WIN32_FIND_DATA FindFileData;
	HANDLE hf = FindFirstFile( (res + ".spg").c_str(), &FindFileData );
	if (hf == INVALID_HANDLE_VALUE) {
		res = MISSIONS_PATH;
		res += "\\";
		res += fileName;
		hf = FindFirstFile( res.c_str(), &FindFileData );
		xassert( hf != INVALID_HANDLE_VALUE );
	}
	return res;
}

void UserSingleProfile::setCurrentProfile(const string& name) {
	for (int i = 0, s = profiles.size(); i < s; i++) {
		if (profiles[i].name == name) {
			setCurrentProfileIndex(i);
			return;
		}
	}
	if (profiles.size() > 0) {
		setCurrentProfileIndex(0);
	}
}

void UserSingleProfile::setRecord(const string& name, int milis) {
	string path = getProfileIniPath(currentProfileIndex);
	IniManager man(path.c_str(), false);
	man.putInt("Records", name.c_str(), milis);
}

int UserSingleProfile::getRecord(const string& name) {
	string path = getProfileIniPath(currentProfileIndex);
	IniManager man(path.c_str(), false);
	return man.getInt("Records", name.c_str());
}
