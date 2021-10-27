
#include "StdAfx.h"
#include "UserSingleProfile.h"
#include "GameShellSq.h"
#include "Config.h"
#include "xutil.h"

UserSingleProfile::UserSingleProfile() :
		currentMissionNumber(-1),
		currentProfileIndex(-1),
		result(UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED),
		lastType(UNDEFINED) {
}
void UserSingleProfile::setDifficulty(Difficulty newDifficulty) {
	profiles[currentProfileIndex].difficulty = newDifficulty;
	std::string path = getProfileIniPath(currentProfileIndex);
	IniManager man( path.c_str(), true );
	man.putInt("General", "difficulty", newDifficulty);
}
void UserSingleProfile::setCurrentMissionNumber(int newMissionNumber) {
	currentMissionNumber = newMissionNumber;
	//save();
}
void UserSingleProfile::setLastMissionNumber(int newMissionNumber) {
	profiles[currentProfileIndex].lastMissionNumber = newMissionNumber;
	std::string path = getProfileIniPath(currentProfileIndex);
	IniManager man( path.c_str(), true );
	man.putInt("General", "lastMissionNumber", newMissionNumber);
}

void UserSingleProfile::scanProfiles() {
	profiles.clear();

	int maxIndex = -1;

    for (const auto & entry : std::filesystem::directory_iterator(getAllSavesDirectory())) {
        if (entry.is_directory()) {
            std::string path = entry.path().filename().string();
            profiles.emplace_back(Profile(path));
        }
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
	std::vector<Profile>::iterator it = profiles.begin();
	while (it != profiles.end()) {
		if ( (*it).name.empty() ) {
			it = profiles.erase(it);
		} else {
			it++;
		}
	}
}
void UserSingleProfile::addProfile(const std::string& name) {
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
	std::string root = getAllSavesDirectory();
	std::string path = root + newProfile.dirName;
	std::string origin = root + PATH_SEP + "DefaultPlayerData";
    std::error_code error;
    std::filesystem::create_directories(path, error);
	if( error ) {
        ErrH.Abort("Can't create profile directory: ", XERR_USER, error.value(), error.message().c_str());
	} else {
        std::string path_data = path + PATH_SEP + "data";
        std::filesystem::copy_file(origin, path_data, error);
        if (error) {
            ErrH.Abort("Can't copy new profile: ", XERR_USER, error.value(), error.message().c_str());
        } else {
            scan_resource_paths(path);
            profiles.push_back( newProfile );
            IniManager man( path_data.c_str(), true );
            man.put("General", "name", name.c_str());
            man.putInt("General", "lastMissionNumber", firstMissionNumber);
            if (i == freeInds.size()) {
                freeInds.push_back(true);
            } else {
                freeInds[i] = true;
            }
            loadProfile(profiles.size() - 1);
        }
	}
}

bool UserSingleProfile::removeDir(const std::string& dir) {
    std::error_code error;
    std::string allSaves = getAllSavesDirectory();
    std::filesystem::path target_path = allSaves + PATH_SEP + dir;
    std::filesystem::remove_all(target_path, error);
    if( error ) {
        ErrH.Abort("Can't remove profile directory: ", XERR_USER, error.value(), error.message().c_str());
    }
    bool deleted = !std::filesystem::exists(target_path);
    scan_resource_paths(allSaves);
    return deleted;
}

void UserSingleProfile::removeProfile(int index) {
	if (removeDir(profiles[index].dirName + PATH_SEP)) {
		freeInds[profiles[index].dirIndex] = false;
	}

	std::vector<Profile>::iterator forErase = profiles.begin();
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

void UserSingleProfile::deleteSave(const std::string& name) {
    std::string savesDir = getSavesDirectory();
	std::string fullName = savesDir + name;
	std::remove( (fullName + ".spg").c_str() );
	std::remove( (fullName + ".gmp").c_str() );
	std::remove( (fullName + ".dat").c_str() );
	std::remove( (fullName + ".sph").c_str() );
    scan_resource_paths(savesDir);
}

std::string UserSingleProfile::getAllSavesDirectory() {
    return convert_path_resource("RESOURCE") + PATH_SEP + "Saves" + PATH_SEP;
}

std::string UserSingleProfile::getSavesDirectory() const {
	return getAllSavesDirectory() + profiles[currentProfileIndex].dirName + PATH_SEP;
}

void UserSingleProfile::loadProfile(int index) {
	std::string path = getProfileIniPath(index);
	IniManager man(path.c_str(), false);
	profiles[index].name = man.get("General","name");
	profiles[index].lastMissionNumber = man.getInt("General","lastMissionNumber");
	profiles[index].difficulty = (Difficulty)man.getInt("General","difficulty");
}

std::string UserSingleProfile::getFileNameWithDifficulty(const std::string& fileName) {
	std::string res = MISSIONS_PATH;
#if 0 //TODO apparently thisfunction is not used
    std::string fileNameWithoutExt = fileName;
	fileNameWithoutExt.erase(fileNameWithoutExt.size() - 4, fileNameWithoutExt.size());
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
#endif
	return res;
}

void UserSingleProfile::setCurrentProfile(const std::string& name) {
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

void UserSingleProfile::setRecord(const std::string& name, int milis) {
	std::string path = getProfileIniPath(currentProfileIndex);
	IniManager man(path.c_str(), false);
	man.putInt("Records", name.c_str(), milis);
}

int UserSingleProfile::getRecord(const std::string& name) {
	std::string path = getProfileIniPath(currentProfileIndex);
	IniManager man(path.c_str(), false);
	return man.getInt("Records", name.c_str());
}
