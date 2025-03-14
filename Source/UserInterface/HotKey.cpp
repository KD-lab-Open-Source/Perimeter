#include "StdAfx.h"
#include "Runtime.h"
#include "Controls.h"
#include "HotKey.h"
#include "qd_textdb.h"
#include "GameContent.h"

#include <utility>

HotKey::HotKey(int actionNumber, float threshold, uint32_t key1, uint32_t key2, bool custom)
	 : actionNumber(actionNumber), threshold(threshold), key1(key1), key2(key2), custom(custom) {
}

CtrlAction::CtrlAction( int controlID, std::string controlName, ControlAction action )
	: controlID(controlID),
    controlName(std::move(controlName)),
    action(std::move(action)) {
}

const char* CtrlAction::getName() const {
	return action.name;
}

const std::string& CtrlAction::getControlName() const {
    return controlName;
}

//void CtrlAction::actionPerformed() {
//	CShellWindow* wnd = _shellIconManager.GetWnd(controlID);
//	wnd->actionPerformed( code,	param);
//}


HotKeyManager::HotKeyManager() {
	lastKey = 0;
    lastKeyTime = 0;
}

HotKeyManager::~HotKeyManager() {
	for (auto& action : actions) {
		delete action;
	}
}

bool HotKeyManager::keyPressed(uint32_t key) {
    float currTime = frame_time();
    bool performed = false;
    for (auto& hotkey : hotKeys) {
        if (checkHotKey(hotkey, key, currTime)) {
            if (actions[hotkey.actionNumber]->actionPerformed()) {
                performed = true;
                break;
            }
        }
    }
    lastKey = key;
    lastKeyTime = currTime;
    return performed;
}

std::string HotKeyManager::getKeyNameForControlAction(const ControlAction& action) {
	for (size_t i = 0, s = actions.size(); i < s; i++) {
		CtrlAction* ctrlAction = dynamic_cast<CtrlAction*>(actions[i]);
		if (ctrlAction && !strcmp(ctrlAction->getName(), action.name)) {
			for (auto & hotKey : hotKeys) {
				if (hotKey.actionNumber == i) {
					std::string res = g_controls_converter.key_name(hotKey.key1);
                    if (res.empty()) {
                        res = std::to_string(hotKey.key1);
                    }
					if (hotKey.key2 != VK_NONE) {
                        std::string key2 = g_controls_converter.key_name(hotKey.key2);
                        if (key2.empty()) {
                            key2 = std::to_string(hotKey.key1);
                        }
						res += "&DDDDDD > &00FFFF" + key2;
					}
					return "&00FFFF" + res;
				}
			}
		}
	}
	return "";
}
std::vector<CtrlAction*>& HotKeyManager::getActions() {
    return actions;
}

std::vector<HotKey>& HotKeyManager::getHotKeys() {
    return hotKeys;
}

bool HotKeyManager::checkHotKey(const HotKey& hotKey, uint32_t key, float currTime) const {
	if (hotKey.key2) {
        float threshold = 0.0f < hotKey.threshold ? hotKey.threshold : 700.0f;
		return 
				(hotKey.key1 == lastKey)
			&&	(hotKey.key2 == key)
			&&	( (currTime - lastKeyTime) <= threshold );
	} else {
		return (hotKey.key1 == key);
	}
}

std::string getIntfControlName(const char* cbTag) {
    std::string tag;
    if (*cbTag == '<') {
        tag = cbTag;
    }
    if (tag.length() < 3) {
        return "";
    }
    tag = tag.substr(1, tag.length() - 2);
    std::string text = qdTextDB::instance().getText(("Interface.Tips." + tag).c_str());
    if (text.empty()) {
        text = tag;
    }
    if (startsWith(tag, "upgrade_") && endsWith(tag, "_btn")) {
        //Trim the text to first 2 lines
        std::vector<size_t> newlines;
        std::string tmp;
        for (size_t i = 0; i < text.length(); i++) {
            char c = text[i];
            if (c == '\n') {
                tmp += ' ';
                newlines.emplace_back(i);
                continue;
            } else if (c == '\r') {
                continue;
            } else {
                tmp += c;
            }
        }

        if (newlines.size() < 2) {
            text = tmp;
        } else {
            text = tmp.substr(0, newlines[newlines.size() - 2]);
        }
    } else if (startsWith(tag, "hotkey_10_")) {
        //Trim the end
        std::string::size_type i = text.find(" = ");
        if (i != std::string::npos) {
            text = text.substr(0, i);
        }
    } else {
        //Try to trim the name of building
        std::string::size_type i = text.find("%s\n");
        if (i != std::string::npos) {
            text = text.substr(0, i);
        }
        if (startsWith(tag, "TV_")) {
            string_replace_all(text, "\n", " ");
        }
    }
    
    //Remove any color code at start to sort work
    if (7 < text.size() && text[0] == '&') {
        text = text.substr(7);
    }
    
    string_replace_all(text, "  ", " ");
    
    //If still has newlines or %s, is probably upgrade texts or who knows, discard it
    std::string::size_type nli = text.find('\n');
    std::string::size_type stri = text.find("%s");
    if (nli != std::string::npos || stri != std::string::npos) {
        fprintf(stderr, "Unknown text for hotkey '%s': '%s\n'", tag.c_str(), text.c_str());
        text = "";
    }
    trim(text);
    return text;
}

void HotKeyManager::fillActions() {
	for (auto& control_container : interfaceAttr().controls) {
		for (auto& control : control_container.controls) {
            if (!(terGameContentAvailable & GAME_CONTENT::PERIMETER_ET)) {
                switch (control.id) {
                    case SQSH_SELPANEL_UPGRADE_ELECTRO1_ID:
                    case SQSH_SELPANEL_UPGRADE_ELECTRO2_ID:
                    case SQSH_STATION_ELECTRO_LAB_ID:
                    case SQSH_GUN_ELECTRO_ID:
                        continue;
                    default:
                        break;
                }
            }
            std::string controlName;
            if (controlName.empty() && control.popup.value() != "<unit_base>") {
                controlName = getIntfControlName(control.popup);
                if (!controlName.empty()) {
                    std::string controlNameChecked = getIntfControlName(control.popupChecked);
                    if (!controlNameChecked.empty()) {
                        controlName += " / " + controlNameChecked;
                    }
                }
            }
            for (auto& action : control.actions) {
                if (!action.name.empty()) {
                    if (controlName.empty() && !control.text.empty()) {
                        controlName = qdTextDB::instance().getText(
                            ("Interface.Menu.ButtonLabels." + control.text.value()).c_str()
                        );
                        trim(controlName);
                    }
                    if (controlName.empty()) {
                        controlName = getIntfControlName(action.hotKeyPopup);
                    }
					actions.push_back(new CtrlAction(
						control.id,
                        controlName,
						action
                    ));
				}
			}
		}
	}
	for (auto & tabSheet : interfaceAttr().tabSheets) {
        for (auto& action : tabSheet.actions) {
			if (!action.name.empty()) {
				actions.push_back(new CtrlAction(
					tabSheet.id,
                    "", //TODO load somehow
					action
                ));
			}
		}
	}
}

bool HotKeyManager::loadHotKey(std::vector<HotKey>& simpleHotKeys, size_t i, IniManager& controls, bool custom) {
    const char* actionName = actions[i]->getName();
    int key1 = -1;
    controls.getInt(actionName, "key1", key1);
    if (key1 < 0 || key1 >= KEY_MAX) {
        return false;
    }
    
    float threshold = 0.0f;
    controls.getFloat(actionName, "threshold", threshold);
    int key2 = VK_NONE;
    controls.getInt(actionName, "key2", key2);
    if (key2 < 0 || key2 >= KEY_MAX) {
        key2 = VK_NONE;
    }
    if (key2 == VK_NONE) {
        simpleHotKeys.emplace_back(i, threshold, key1, key2, custom);
    } else {
        hotKeys.emplace_back(i, threshold, key1, key2, custom);
    }
    return true;
}

void HotKeyManager::loadHotKeys() {
	IniManager controls("RESOURCE\\controls.ini", false);
    IniManager custom_controls_ini = IniManager(getPrefFilePath("custom_controls.ini").c_str(), false, true);
	std::vector<HotKey> simpleHotKeys;
    hotKeys.clear();
	for (size_t i = 0, s = actions.size(); i < s; i++) {
        bool loaded = loadHotKey(simpleHotKeys, i, custom_controls_ini, true);
        if (!loaded) {
            loadHotKey(simpleHotKeys, i, controls, false);
        }
	}
	while (!simpleHotKeys.empty()) {
		hotKeys.push_back(simpleHotKeys.back());
		simpleHotKeys.pop_back();
	}
}

