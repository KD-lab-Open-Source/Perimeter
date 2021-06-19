#pragma once

#ifndef _HOTKEY_H
#define _HOTKEY_H

#include "GameShellSq.h"

struct HotKey {
	int actionNumber;
	float threshold;
	int key1;
	int key2;

	HotKey();
	HotKey(int actionNumber, float threshold, int key1, int key2);
};

class AbstractAction {
	public:
		virtual bool actionPerformed() = 0;
		virtual const char* getName() = 0;
		virtual ~AbstractAction() = default;
};

class CtrlAction : public AbstractAction {
	public:
		CtrlAction( int controlID, const ControlAction& action );
		bool actionPerformed();
		const char* getName();
	protected:
		int controlID;
		ControlAction action;

};

class HotKeyManager {
	public:
		HotKeyManager();
		~HotKeyManager();
		void keyPressed(int key);
		std::string getKeyNameForControlAction(const ControlAction& action);
	protected:
		void fillActions();
		void loadHotKeys();
		bool checkHotKey(const HotKey& hotKey, int key, float currTime);

		std::vector<AbstractAction*> actions;
		std::vector<HotKey> hotKeys;
		int lastKey;
		float lastKeyTime;
};

#endif //_HOTKEY_H
