#include "StdAfx.h"
#include "Controls.h"
#include "qd_textdb.h"
#include "Localization.h"
#include "codepages/codepages.h"
#include "xutil.h"

BEGIN_ENUM_DESCRIPTOR(eGameKeysControl, "eGameKeysControl")
REGISTER_ENUM(CTRL_TIME_NORMAL, "CTRL_TIME_NORMAL");
REGISTER_ENUM(CTRL_TIME_DEC, "CTRL_TIME_DEC");
REGISTER_ENUM(CTRL_TIME_INC, "CTRL_TIME_INC");
REGISTER_ENUM(CTRL_CAMERA_MOVE_UP, "CTRL_CAMERA_MOVE_UP");
REGISTER_ENUM(CTRL_CAMERA_MOVE_DOWN, "CTRL_CAMERA_MOVE_DOWN");
REGISTER_ENUM(CTRL_CAMERA_MOVE_LEFT, "CTRL_CAMERA_MOVE_LEFT");
REGISTER_ENUM(CTRL_CAMERA_MOVE_RIGHT, "CTRL_CAMERA_MOVE_RIGHT");
REGISTER_ENUM(CTRL_CAMERA_ROTATE_UP, "CTRL_CAMERA_ROTATE_UP");
REGISTER_ENUM(CTRL_CAMERA_ROTATE_DOWN, "CTRL_CAMERA_ROTATE_DOWN");
REGISTER_ENUM(CTRL_CAMERA_ROTATE_LEFT, "CTRL_CAMERA_ROTATE_LEFT");
REGISTER_ENUM(CTRL_CAMERA_ROTATE_RIGHT, "CTRL_CAMERA_ROTATE_RIGHT");
REGISTER_ENUM(CTRL_CAMERA_ZOOM_INC, "CTRL_CAMERA_ZOOM_INC");
REGISTER_ENUM(CTRL_CAMERA_ZOOM_DEC, "CTRL_CAMERA_ZOOM_DEC");
REGISTER_ENUM(CTRL_CAMERA_MOUSE_LOOK, "CTRL_CAMERA_MOUSE_LOOK");
REGISTER_ENUM(CTRL_GAME_HELP, "CTRL_GAME_HELP");
REGISTER_ENUM(CTRL_ESCAPE, "CTRL_ESCAPE");
REGISTER_ENUM(CTRL_CAMERA_SAVE1, "CTRL_CAMERA_SAVE1");
REGISTER_ENUM(CTRL_CAMERA_SAVE2, "CTRL_CAMERA_SAVE2");
REGISTER_ENUM(CTRL_CAMERA_SAVE3, "CTRL_CAMERA_SAVE3");
REGISTER_ENUM(CTRL_CAMERA_SAVE4, "CTRL_CAMERA_SAVE4");
REGISTER_ENUM(CTRL_CAMERA_SAVE5, "CTRL_CAMERA_SAVE5");
REGISTER_ENUM(CTRL_CAMERA_RESTORE1, "CTRL_CAMERA_RESTORE1");
REGISTER_ENUM(CTRL_CAMERA_RESTORE2, "CTRL_CAMERA_RESTORE2");
REGISTER_ENUM(CTRL_CAMERA_RESTORE3, "CTRL_CAMERA_RESTORE3");
REGISTER_ENUM(CTRL_CAMERA_RESTORE4, "CTRL_CAMERA_RESTORE4");
REGISTER_ENUM(CTRL_CAMERA_RESTORE5, "CTRL_CAMERA_RESTORE5");
REGISTER_ENUM(CTRL_CAMERA_TO_EVENT, "CTRL_CAMERA_TO_EVENT");
REGISTER_ENUM(CTRL_CAMERA_MOUSE_MOVE, "CTRL_CAMERA_MOUSE_MOVE");
REGISTER_ENUM(CTRL_TOGGLE_MUSIC, "CTRL_TOGGLE_MUSIC");
REGISTER_ENUM(CTRL_TOGGLE_SOUND, "CTRL_TOGGLE_SOUND");
REGISTER_ENUM(CTRL_LOAD, "CTRL_LOAD");
REGISTER_ENUM(CTRL_SAVE, "CTRL_SAVE");
REGISTER_ENUM(CTRL_HOLD_PRODUCTION, "CTRL_HOLD_PRODUCTION");
REGISTER_ENUM(CTRL_TOGGLE_LIFEBARS, "CTRL_TOGGLE_LIFEBARS");
REGISTER_ENUM(CTRL_RESUME_PRODUCTION, "CTRL_RESUME_PRODUCTION");
REGISTER_ENUM(CTRL_TOGGLE_FPS, "CTRL_TOGGLE_FPS");
END_ENUM_DESCRIPTOR(eGameKeysControl)

ControlsConverter g_controls_converter;

void ControlsConverter::LoadKeyNames() {
	for (uint32_t i = 0; i <= VK_MAX_CODE; i++) {
        switch (i) {
            case VK_NONE:
                KeyToName[i] = qdTextDB::instance().getText("Controls.Input.NoInputSet");
                break;
            case VK_LBUTTON:
                KeyToName[i] = qdTextDB::instance().getText("Controls.Input.MouseLeftButton");
                break;
            case VK_RBUTTON:
                KeyToName[i] = qdTextDB::instance().getText("Controls.Input.MouseRightButton");
                break;
            case VK_MBUTTON:
                KeyToName[i] = qdTextDB::instance().getText("Controls.Input.MouseMiddleButton");
                break;
            default:
                SDL_Scancode code = getSDLScanCodeFromVK(i);
                std::string name;

                SDL_Keycode keycode = SDL_GetKeyFromScancode(code);
                name = SDL_GetKeyName(keycode);
                name = convertToCodepage(name.c_str(), getLocale(), 0);
                if (name.empty()) {
                    name = SDL_GetScancodeName(code);
                    name = convertToCodepage(name.c_str(), getLocale(), 0);
                }
                KeyToName[i] = name;
                break;
        }
    }

    std::string strCtrl = KeyToName[VK_CONTROL] + " + ";
    std::string strShift = KeyToName[VK_SHIFT] + " + ";
    std::string strAlt = KeyToName[VK_ALT] + " + ";
    for (uint32_t i = VK_MAX_CODE + 1; i < KEY_MAX; i++) {
        if (!KeyToName[i & VK_MASK].empty()) {
            std::string& str = KeyToName[i];
            str = "";
            uint32_t k = (i & VK_MASK);
            if (i & KBD_SHIFT && k != VK_SHIFT)
                str += strShift;
            if (i & KBD_CTRL && k != VK_CONTROL)
                str += strCtrl;
            if (i & KBD_ALT && k != VK_ALT)
                str += strAlt;
            str += KeyToName[i & VK_MASK];
        }
    }
}

void ControlsConverter::LoadCtrlTable() {
    for (uint32_t i = 0; i < KEY_MAX; i++) {
        KeyToCtrl[i] = static_cast<eGameKeysControl>(-1);
    }
    IniManager controls_ini = IniManager("RESOURCE\\controls.ini");
    IniManager custom_controls_ini = IniManager(getPrefFilePath("custom_controls.ini").c_str(), false, true);
	for (uint32_t ctrl = 0; ctrl < CTRL_MAX; ctrl++) {
		char ctrl_str[256];
		snprintf(ctrl_str, 256, "%d", ctrl);
        int nkey = -1;
        
        //Fetch any custom key first
        bool custom = true;
        custom_controls_ini.getInt("main", ctrl_str, nkey);
        //If none then check from game content 
        if (nkey < 0 || nkey >= KEY_MAX) {
            nkey = -1;
            controls_ini.getInt("main", ctrl_str, nkey);
            custom = false;
        }
        if (nkey < 0 || nkey >= KEY_MAX) {
            nkey = VK_NONE;
        }
        CtrlToKey[ctrl].fullkey = VK_NONE;
        set_key_control(ctrl, nkey, custom);
	}
}

void ControlsConverter::set_key_control(uint32_t ctrl, uint32_t key, bool custom) {
    xassert(ctrl < CTRL_MAX && key < KEY_MAX);
    
    //Invalidate old key -> ctrl mapping
    uint32_t old_key = CtrlToKey[ctrl].fullkey;
    if (old_key != VK_NONE && old_key < KEY_MAX && KeyToCtrl[old_key] == ctrl) {
        KeyToCtrl[old_key] = static_cast<eGameKeysControl>(-1);
    }
    
    //Set the new key -> ctrl map
    if (key != VK_NONE && key < KEY_MAX) {
        KeyToCtrl[key] = static_cast<eGameKeysControl>(ctrl);
    }
    
    CtrlToKey[ctrl] = sKey(key, false);
    CtrlIsCustom[ctrl] = custom;
}

const std::string& ControlsConverter::key_name(uint32_t key) const {
    xassert(key < KEY_MAX);
    return KeyToName[key];
}

eGameKeysControl ControlsConverter::key_control(uint32_t key) const {
    xassert(key < KEY_MAX);
    return KeyToCtrl[key];
}

const sKey& ControlsConverter::control_key(uint32_t ctrl) const {
    xassert(ctrl < CTRL_MAX);
    return CtrlToKey[ctrl];
}

bool ControlsConverter::is_custom(uint32_t ctrl) const {
    xassert(ctrl < CTRL_MAX);
    return CtrlIsCustom[ctrl];
}

#ifndef GPX
bool ControlsConverter::pressed(uint32_t ctrl) const {
    if (ctrl < CTRL_MAX) {
        return CtrlToKey[ctrl].pressed();
    } else {
        xassert(0);
        return false;
    }
}
#endif
