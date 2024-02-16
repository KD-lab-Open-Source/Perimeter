
#include "StdAfx.h"
#include "UserSingleProfile.h"
#include "GameShellSq.h"
#include "Config.h"
#include "xutil.h"
#include "files/files.h"
#include "GameContent.h"

std::string getMissionNumberKey() {
    std::string missionNumberKey = "lastMissionNumber";
    GAME_CONTENT content = getGameContentCampaign();
    if (content != terGameContentBase) {
        missionNumberKey += "_" + getGameContentEnumName(terGameContentSelect);
    }
    return missionNumberKey;
}

UserSingleProfile::UserSingleProfile() :
		currentProfileIndex(-1),
        currentMissionNumber(-1),
		lastType(UNDEFINED),
        result(UNIVERSE_INTERFACE_MESSAGE_GAME_RESULT_UNDEFINED) {
}
void UserSingleProfile::setDifficulty(Difficulty newDifficulty) {
    if (!isValidProfile()) return;
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
    if (!isValidProfile()) return;
	profiles[currentProfileIndex].lastMissionNumber = newMissionNumber;
	std::string path = getProfileIniPath(currentProfileIndex);
	IniManager man( path.c_str(), true );
	man.putInt("General", getMissionNumberKey().c_str(), newMissionNumber);
}

void UserSingleProfile::scanProfiles() {
	profiles.clear();

	int maxIndex = -1;

    for (const auto & entry : get_content_entries_directory(getAllSavesDirectory())) {
        if (entry->is_directory) {
            std::string path = std::filesystem::u8path(entry->path_content).filename().u8string();
            std::string path_lwr = string_to_lower(path.c_str());
            if (startsWith(path_lwr, "profile")) {
                if (get_content_entry(entry->path_content + PATH_SEP + "data")) {
                    profiles.emplace_back(path);
                }
            }
        }
    }
	for (size_t i = 0, s = profiles.size(); i < s; i++) {
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
	std::string origin = root + "DefaultPlayerData";
    std::error_code error;
    create_directories(path, &error);
	if( error ) {
        ErrH.Abort("Can't create profile directory: ", XERR_USER, error.value(), error.message().c_str());
	} else {
        std::string path_data = path + PATH_SEP + "data";
        std::filesystem::copy_file(
                std::filesystem::u8path(origin),
                std::filesystem::u8path(path_data),
                error
        );
        if (error) {
            ErrH.Abort("Can't copy new profile: ", XERR_USER, error.value(), error.message().c_str());
        } else {
            profiles.push_back( newProfile );
            IniManager man( path_data.c_str(), true );
            man.put("General", "name", name.c_str());
            man.putInt("General", getMissionNumberKey().c_str(), firstMissionNumber);
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
    std::filesystem::path target_path = std::filesystem::u8path(allSaves + dir);
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

    if (profiles.empty()) {
        setCurrentProfileIndex(-1);
    } else {
        if (getCurrentProfileIndex() >= index ) {
            setCurrentProfileIndex(getCurrentProfileIndex() - 1);
        }
        if (!isValidProfile()) {
            setCurrentProfileIndex(0);
        }
    }
}

void UserSingleProfile::setCurrentProfileIndex(int index) {
	currentProfileIndex = index;
}

void UserSingleProfile::deleteSave(const std::string& path) {
    for (auto& ext : {"spg", "bin", "sph", "gmp", "dat"}) {
        std::string path_ext = setExtension(path, ext);
        std::remove(path_ext.c_str());
    }
    std::filesystem::path path_path = std::filesystem::u8path(path);
    scan_resource_paths(path_path.parent_path().u8string());
}

std::string UserSingleProfile::getAllSavesDirectory() {
    return convert_path_content("Resource/Saves") + PATH_SEP;
}

std::string UserSingleProfile::getSavesDirectory() const {
    std::string savesDir = getAllSavesDirectory();
    if (getLastGameType() == UserSingleProfile::MULTIPLAYER) {
        //Workaround for when inside multiplayer
        savesDir += "Multiplayer";
    } else if (isValidProfile()) {
        savesDir += profiles[currentProfileIndex].dirName;
    }
	return savesDir + PATH_SEP;
}

void UserSingleProfile::loadProfile(int index) {
	std::string path = getProfileIniPath(index);
    if (path.empty()) return;
	IniManager man(path.c_str(), false);
	profiles[index].name = man.get("General","name");
    int missionNumber = firstMissionNumber;
    man.getInt("General", getMissionNumberKey().c_str(), missionNumber);
	profiles[index].lastMissionNumber = missionNumber;
	profiles[index].difficulty = (Difficulty)man.getInt("General","difficulty");
}

void UserSingleProfile::setCurrentProfile(const std::string& name) {
	for (size_t i = 0, s = profiles.size(); i < s; i++) {
		if (profiles[i].name == name) {
			setCurrentProfileIndex(i);
			return;
		}
	}
	if (!profiles.empty()) {
		setCurrentProfileIndex(0);
	}
}

void UserSingleProfile::setRecord(const std::string& name, int milis) {
    if (!isValidProfile()) return;
	std::string path = getProfileIniPath(currentProfileIndex);
	IniManager man(path.c_str(), false);
	man.putInt("Records", name.c_str(), milis);
}

int UserSingleProfile::getRecord(const std::string& name) {
    if (!isValidProfile()) return 0;
	std::string path = getProfileIniPath(currentProfileIndex);
	IniManager man(path.c_str(), false);
	return man.getInt("Records", name.c_str());
}
