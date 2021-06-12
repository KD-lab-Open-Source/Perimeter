#include "StdAfx.h"
#include "Runtime.h"
#include "Controls.h"
#include "HotKey.h"

HotKey::HotKey() {
	actionNumber = -1;
}

HotKey::HotKey(int actionNumber, float threshold, int key1, int key2)
	 : actionNumber(actionNumber), threshold(threshold), key1(key1), key2(key2) {
}

CtrlAction::CtrlAction( int controlID, const ControlAction& action )
	: controlID(controlID), action(action) {
}

const char* CtrlAction::getName() {
	return action.name;
}

//void CtrlAction::actionPerformed() {
//	CShellWindow* wnd = _shellIconManager.GetWnd(controlID);
//	wnd->actionPerformed( code,	param);
//}


HotKeyManager::HotKeyManager() {
	fillActions();
	loadHotKeys();
	lastKey = 0;
}

HotKeyManager::~HotKeyManager() {
	for (int i = 0, s = actions.size(); i < s; i++) {
		delete actions[i];
	}
}

//void HotKeyManager::keyPressed(int key) {
//	float currTime = frame_time();
//	for (int i = 0, s = hotKeys.size(); i < s; i++) {
//		if ( checkHotKey(hotKeys[i], key, currTime) ) {
//			actions[ hotKeys[i].actionNumber ]->actionPerformed();
//			gameShell->updatePosition();
//		}
//	}
//	lastKey = key;
//	lastKeyTime = currTime;
//}

string HotKeyManager::getKeyNameForControlAction(const ControlAction& action) {
	for (int i = 0, s = actions.size(); i < s; i++) {
		CtrlAction* ctrlAction = dynamic_cast<CtrlAction*>(actions[i]);
		if (ctrlAction && !strcmp(ctrlAction->getName(), action.name)) {
			for (int j = 0, sj = hotKeys.size(); j < sj; j++) {
				if (hotKeys[j].actionNumber == i) {
					string res = g_controls_converter.name(hotKeys[j].key1);
					if (hotKeys[j].key2) {
						res += " + " + g_controls_converter.name(hotKeys[j].key2);
					}
					return res;
				}
			}
		}
	}
	return "";
}

bool HotKeyManager::checkHotKey(const HotKey& hotKey, int key, float currTime) {
	if (hotKey.key2) {
		return 
				(hotKey.key1 == lastKey)
			&&	(hotKey.key2 == key)
			&&	( (currTime - lastKeyTime) <= hotKey.threshold );
	} else {
		return (hotKey.key1 == key);
	}
}

void HotKeyManager::fillActions() {
	for (int i = 0; i < interfaceAttr().controls.size(); i++) {
		for (int j = 0; j < interfaceAttr().controls[i].controls.size(); j++) {
			for (int k = 0; k < interfaceAttr().controls[i].controls[j].actions.size(); k++) {
				if (strlen(interfaceAttr().controls[i].controls[j].actions[k].name)) {
					actions.push_back( new CtrlAction(
						interfaceAttr().controls[i].controls[j].id,
						interfaceAttr().controls[i].controls[j].actions[k]) );
				}
			}
			
		}
	}
	for (int i = 0; i < interfaceAttr().tabSheets.size(); i++) {
		for (int k = 0; k < interfaceAttr().tabSheets[i].actions.size(); k++) {
			if (strlen(interfaceAttr().tabSheets[i].actions[k].name)) {
				actions.push_back( new CtrlAction(
					interfaceAttr().tabSheets[i].id,
					interfaceAttr().tabSheets[i].actions[k]) );
			}
		}
	}
}

void HotKeyManager::loadHotKeys() {
	IniManager controls("RESOURCE\\controls.ini", false);
	vector<HotKey> simpleHotKeys;
	for (int i = 0, s = actions.size(); i < s; i++) {
		float threshold = controls.getFloat(actions[i]->getName(), "threshold");
		if (threshold == 0.0f) {
			threshold = 700.0f;
		}
		int key1 = controls.getInt(actions[i]->getName(), "key1");
		if (key1) {
			int key2 = controls.getInt(actions[i]->getName(), "key2");
			if (key2) {
				hotKeys.push_back( HotKey(i, threshold, key1, key2) ) ;
			} else {
				simpleHotKeys.push_back( HotKey(i, threshold, key1, key2) ) ;
			}
		}
	}
	while (simpleHotKeys.size() > 0) {
		hotKeys.push_back(simpleHotKeys.back());
		simpleHotKeys.pop_back();
	}
}

