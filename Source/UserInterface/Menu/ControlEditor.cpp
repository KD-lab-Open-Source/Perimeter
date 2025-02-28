#include "StdAfx.h"
#include <unordered_set>
#include "GameShell.h"
#include "PerimeterShellUI.h"
#include "MessageBox.h"
#include "Controls.h"
#include "HotKey.h"
#include "MainMenu.h"

struct control_input_t {
    std::string name = {};
    eGameKeysControl control = static_cast<eGameKeysControl>(-1);
    std::unordered_set<size_t> hotkeys = {};
    std::vector<uint32_t> inputs = {};

    control_input_t() = default;

    FORCEINLINE bool operator <(const control_input_t& v) const {
        return std::tie(name)
             < std::tie(v.name);
    }
};

extern bool menuChangingDone;
std::vector<control_input_t> control_states;
std::vector<uint32_t> captured_inputs = {};
uint32_t pressed_mod_keys = 0;
bool first_capture_input = false;

std::string getInputName(uint32_t key) {
    std::string key_name = g_controls_converter.key_name(key);
    if (key_name.empty()) {
        fprintf(stderr, "%s: Unknown key name: 0x%" PRIx32 "\n", __func__, key);
        key_name = "&CC6600" + std::to_string(key);
    } else if (key == VK_NONE) {
        key_name = "&DDDDDD" + key_name;
    } else {
        key_name = "&00FFFF" + key_name;
    }
    return key_name;
}

CListBoxWindow* getControlListWindow() {
    CListBoxWindow* list;
    if (universe()) {
        list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_CONTROL_EDITOR_INGAME_LIST));
    } else {
        list = dynamic_cast<CListBoxWindow*>(_shellIconManager.GetWnd(SQSH_MM_CONTROL_EDITOR_LIST));
    }
    xassert(list);
    return list;
}

void updateControlEditorInputDialogText() {
    CListBoxWindow* list = getControlListWindow();
    int cursel = list ? list->GetCurSel() : -1;
    if (cursel < 0 || cursel >= control_states.size()) {
        return;
    }
    const control_input_t& controlInput = control_states[cursel];
    std::string text = qdTextDB::instance().getText("Interface.Menu.Messages.PressNewControlInput");
    text += "\n\n" + controlInput.name + "&FFFFFF\n\n";
    bool first_input = true;
    for (auto& input : captured_inputs) {
        if (first_input) {
            first_input = false;
        } else {
            text += "&DDDDDD > &00FFFF";
        }
        text += getInputName(input);
    }
    dynamic_cast<CTextWindow*>(_shellIconManager.GetWnd(SQSH_MM_SUBMIT_TXT))->setText(text);
}

void onMMControlEditorButton(CShellWindow* pWnd, InterfaceEventCode code, int _param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput() ) {
        if (universe()) {
            _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_CONTROL_EDITOR_INGAME_SCR);
        } else {
            _shellIconManager.SwitchMenuScreens(pWnd->m_pParent->ID, SQSH_MM_CONTROL_EDITOR_SCR);
        }
    }
}

///Refilles control list entries
void updateControlList(CListBoxWindow* list) {
    if (!list) {
        return;
    }
    control_states.clear();

    const auto& hotKeyActions = gameShell->hotKeyManager->getActions();
    const auto& hotKeys = gameShell->hotKeyManager->getHotKeys();

    //Load the eGameKeysControl mappings
    for (int i = 0; i < CTRL_MAX; i++) {
        eGameKeysControl ctrl = static_cast<eGameKeysControl>(i);
        switch (ctrl) {
            case CTRL_TOGGLE_MUSIC:
            case CTRL_TOGGLE_SOUND:
            case CTRL_GAME_HELP:
                //Unused, skip
                continue;
            default:
                break;
        }

        const sKey& skey = g_controls_converter.control_key(ctrl);
        control_input_t& ctrlinput = control_states.emplace_back();
        ctrlinput.control = ctrl;
        std::string ctrl_text = std::string("Controls.") + getEnumName<eGameKeysControl>(ctrl);
        ctrlinput.name = qdTextDB::instance().getText(ctrl_text.c_str());
        ctrlinput.inputs.emplace_back(skey.fullkey);
    }
    
    //These hotkey are a bit special since they have separate copies with same hotkey
    std::unordered_map<std::string, control_input_t> ctrlinput_mergeables;
    //So we find them and store the reference
    for (size_t i = 0; i < hotKeys.size(); i++) {
        const HotKey& hotkey = hotKeys[i];
        const CtrlAction* action = hotKeyActions[hotkey.actionNumber];
        std::string actionName = action->getName();
        if (actionName == "tvUpgrade"
        || actionName == "tvStop") {
            control_input_t ctrlinput;
            ctrlinput.name = action->getControlName();
            if (ctrlinput.name.empty()) {
                ctrlinput.name = actionName;
            }
            ctrlinput.hotkeys.emplace(i);
            ctrlinput.inputs.emplace_back(hotkey.key1);
            if (hotkey.key2 != VK_NONE) {
                ctrlinput.inputs.emplace_back(hotkey.key2);
            }
            ctrlinput_mergeables.emplace(actionName, ctrlinput);
        }
    }
    
    //Load the hotkey mappings
    for (size_t i = 0; i < hotKeys.size(); i++) {
        const HotKey& hotkey = hotKeys[i];
        const CtrlAction* action = hotKeyActions[hotkey.actionNumber];
        const std::string& controlName = action->getControlName();
        if (controlName.empty()) {
            continue;
        }

        std::vector<uint32_t> inputs;
        inputs.emplace_back(hotkey.key1);
        if (hotkey.key2 != VK_NONE) {
            inputs.emplace_back(hotkey.key2);
        }

        bool skipControlInput = false;
        for (auto& ctrlinput_pair : ctrlinput_mergeables) {
            if (startsWith(action->getName(), ctrlinput_pair.first)) {
                if (ctrlinput_pair.second.hotkeys.find(i) != ctrlinput_pair.second.hotkeys.end()) {
                    //Already added hotkey, just skip to next hotkey
                    skipControlInput = true;
                    break;
                }
                if (ctrlinput_pair.second.inputs == inputs) {
                    //Can be merged, add the action name to list if hotkey matches and continue
                    ctrlinput_pair.second.hotkeys.emplace(i);
                    skipControlInput = true;
                    break;
                }
            }
        }
        if (skipControlInput) {
            continue;
        }
        
        control_input_t& ctrlinput = control_states.emplace_back();
        ctrlinput.hotkeys.emplace(i);
        ctrlinput.name = controlName;
#if 0 && defined(PERIMETER_DEBUG)
        ctrlinput.name += std::string("&FFFFFF = ") + action->getName();
#endif
        ctrlinput.inputs = inputs;
    }
    
    //Add the mergeables
    for (auto& ctrlinput_pair : ctrlinput_mergeables) {
        if (!ctrlinput_pair.second.hotkeys.empty()) {
            control_states.emplace_back(ctrlinput_pair.second);
        }
    }
    ctrlinput_mergeables.clear();
    
    //Sort
    std::sort(control_states.begin(), control_states.end());

    //Update UI with control strings
    if (list->GetItem(0).m_data.size() != control_states.size()) {
        //We can't update in place, redo the whole list
        list->Clear();
    }
    auto& items = list->GetItem(0).m_data;
    for (size_t i = 0; i < control_states.size(); i++) {
        auto& control_input = control_states[i];
        std::string line;
        for (auto& input : control_input.inputs) {
            if (!line.empty()) {
                line += "&DDDDDD > ";
            }
            line += getInputName(input);
        }
        line = control_input.name + "&FFFFFF  -  " + line;
        if (i < items.size()) {
            items[i].text = line;
        } else {
            list->AddString(line, 0);
        }
    }
}

void setControlInputState(const control_input_t& controlInput, bool custom) {
    if (controlInput.hotkeys.empty()) {
        g_controls_converter.set_key_control(controlInput.control, controlInput.inputs[0], custom);
    } else {
        auto& hotKeys = gameShell->hotKeyManager->getHotKeys();
        for (size_t i : controlInput.hotkeys) {
            HotKey& hotkey = hotKeys[i];
            hotkey.custom = custom;
            hotkey.key1 = controlInput.inputs[0];
            if (1 < controlInput.inputs.size()) {
                hotkey.key2 = controlInput.inputs[1];
            } else {
                hotkey.key2 = VK_NONE;
            }
        }
    }
}

void saveCustomControlInputs() {
    std::string custom_controls_path = getPrefFilePath("custom_controls.ini");
    std::remove(custom_controls_path.c_str());
    IniManager custom_controls_ini = IniManager(custom_controls_path.c_str(), false, true);
    for (auto& controlInput : control_states) {
        if (controlInput.inputs.empty()) {
            continue;
        }
        if (controlInput.hotkeys.empty()) {
            if (!g_controls_converter.is_custom(controlInput.control)) {
                continue;
            }
            std::string ctrl_str = std::to_string(controlInput.control);
            const sKey& skey = g_controls_converter.control_key(controlInput.control);
            custom_controls_ini.put("main", ctrl_str.c_str(), std::to_string(skey.fullkey).c_str());
        } else {
            auto& hotKeys = gameShell->hotKeyManager->getHotKeys();
            auto& hotKeyActions = gameShell->hotKeyManager->getActions();
            for (size_t i : controlInput.hotkeys) {
                HotKey& hotkey = hotKeys[i];
                if (!hotkey.custom) {
                    continue;
                }
                const CtrlAction* action = hotKeyActions[hotkey.actionNumber];
                const char* action_name = action->getName();
                custom_controls_ini.putFloat(action_name, "threshold", hotkey.threshold);
                custom_controls_ini.put(action_name, "key1", std::to_string(hotkey.key1).c_str());
                custom_controls_ini.put(action_name, "key2", std::to_string(hotkey.key2).c_str());
            }
        }
    }
}

void applyControlInputChange() {
    CListBoxWindow* list = getControlListWindow();
    int cursel = list ? list->GetCurSel() : -1;
    if (cursel < 0 || cursel >= control_states.size() || captured_inputs.empty()) {
        return;
    }
    control_input_t& controlInput = control_states[cursel];
    controlInput.inputs = captured_inputs;
    setControlInputState(controlInput, true);
    saveCustomControlInputs();    
    updateControlList(list);
}

int controlEditorInputDone(float, float) {
    gameShell->setCaptureInputCallback(nullptr);
    applyControlInputChange();
    hideMessageBox();
    return 1;
}

bool controlEditorInputCallback(uint32_t key, bool pressed) {
    //Don't capture input when hovering dialog OK button
    switch (key & VK_MASK) {
        case VK_LBUTTON:
        case VK_RBUTTON:
        case VK_MBUTTON:
        case VK_ESCAPE:
        case VK_RETURN:
            if (_shellIconManager.m_pCtrlHover && _shellIconManager.m_pCtrlHover->ID == SQSH_MM_SUBMIT_OK_BTN) {
                return false;
            }
            break;
        default:
            break;
    }

    //Store mod key presses to allow them unless a key is already stored in between press-release
    uint32_t mod_flag = getModFlagFromKey(key & VK_MASK);
    if (pressed) {
        pressed_mod_keys |= mod_flag;
        return true;
    }

    //Get the editing control input
    CListBoxWindow* list = getControlListWindow();
    int cursel = list ? list->GetCurSel() : -1;
    if (cursel < 0 || cursel >= control_states.size() || captured_inputs.empty()) {
        return true;
    }
    const control_input_t& controlInput = control_states[cursel];

    //Handle modifier keys
    if (mod_flag != 0 && !(pressed_mod_keys & mod_flag)) {
        return true;
    }

    //Game key controls only have one key, hotkeys don't need more than two
    if (!captured_inputs.empty() && (
        first_capture_input || controlInput.hotkeys.empty() || captured_inputs.size() >= 2
    )) {
        captured_inputs.clear();
    }
    first_capture_input = false;
    pressed_mod_keys = 0; //Ignore releases of mod keys

    captured_inputs.emplace_back(key);
    updateControlEditorInputDialogText();

    return true;
}

void controlEditorDialog() {
    CListBoxWindow* list = getControlListWindow();
    int cursel = list ? list->GetCurSel() : -1;
    if (cursel < 0 || cursel >= control_states.size()) {
        return;
    }

    first_capture_input = true;
    const control_input_t& controlInput = control_states[cursel];
    captured_inputs = controlInput.inputs;
    
    gameShell->setCaptureInputCallback(controlEditorInputCallback);
    setupOkMessageBox(controlEditorInputDone, 0, "", MBOX_OK, true);
    updateControlEditorInputDialogText();
    showMessageBox();
}

void onMMControlEditorList(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if (code == EVENT_CREATEWND) {
        CListBoxWindow* list = dynamic_cast<CListBoxWindow*>(pWnd);
        list->Clear();
        updateControlList(list);
        list->SetCurSel(0);
    } else if (code == EVENT_DOUBLECLICK) {
        controlEditorDialog();
    }
}

void onMMControlEditorChange(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput()) {
        controlEditorDialog();
    }
}

void onMMControlEditorRestore(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput()) {
        CListBoxWindow* list = getControlListWindow();
        int cursel = list ? list->GetCurSel() : -1;
        if (cursel < 0 || cursel >= control_states.size()) {
            return;
        }
        const control_input_t& controlInput = control_states[cursel];
        setControlInputState(controlInput, false);
        saveCustomControlInputs();
        g_controls_converter.LoadCtrlTable();
        gameShell->hotKeyManager->loadHotKeys();
        updateControlList(list);
    }
}

void onMMControlEditorDelete(CShellWindow* pWnd, InterfaceEventCode code, int param) {
    if( code == EVENT_UNPRESSED && intfCanHandleInput()) {
        captured_inputs = { VK_NONE };
        applyControlInputChange();
        g_controls_converter.LoadCtrlTable();
        gameShell->hotKeyManager->loadHotKeys();
        CListBoxWindow* list = getControlListWindow();
        updateControlList(list);
    }
}
