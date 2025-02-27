#pragma once

#ifndef _HOTKEY_H
#define _HOTKEY_H

#include "GameShellSq.h"

struct HotKey {
	int actionNumber = -1;
	float threshold = 0.0f;
	uint32_t key1 = VK_NONE;
	uint32_t key2 = VK_NONE;
    bool custom = false;

	HotKey() = default;
	HotKey(int actionNumber, float threshold, uint32_t key1, uint32_t key2, bool custom);
};

class CtrlAction {
	public:
		CtrlAction( int controlID, std::string  controlName, ControlAction  action );
		bool actionPerformed();
		const char* getName() const;
        const std::string& getControlName() const;
	protected:
		int controlID;
        std::string controlName;
		ControlAction action;
};

class HotKeyManager {
	public:
		HotKeyManager();
		~HotKeyManager();
		bool keyPressed(uint32_t key);
		std::string getKeyNameForControlAction(const ControlAction& action);
        std::vector<CtrlAction*>& getActions();
        std::vector<HotKey>& getHotKeys();
        void fillActions();
        void loadHotKeys();
	protected:
        bool loadHotKey(std::vector<HotKey>& simpleHotKeys, size_t i, IniManager& ini, bool custom);
		bool checkHotKey(const HotKey& hotKey, uint32_t key, float currTime) const;

		std::vector<CtrlAction*> actions;
		std::vector<HotKey> hotKeys;
        uint32_t lastKey;
		float lastKeyTime;
};

#endif //_HOTKEY_H
