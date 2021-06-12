#ifndef __CONTROLS_H__
#define __CONTROLS_H__

enum eGameKeysControl
{
	CTRL_TIME_NORMAL			= 0,	// shift a, shift z 
	CTRL_TIME_DEC				= 1,	// z
	CTRL_TIME_INC				= 2,	// a
	CTRL_CAMERA_MOVE_UP			= 3,	// VK_UP
	CTRL_CAMERA_MOVE_DOWN		= 4,	// VK_DOWN
	CTRL_CAMERA_MOVE_LEFT		= 5,	// VK_LEFT
	CTRL_CAMERA_MOVE_RIGHT		= 6,	// VK_RIGHT
	CTRL_CAMERA_ROTATE_UP		= 7,	// ctrl VK_UP
	CTRL_CAMERA_ROTATE_DOWN		= 8,	// ctrl VK_DOWN
	CTRL_CAMERA_ROTATE_LEFT		= 9,	// ctrl VK_LEFT
	CTRL_CAMERA_ROTATE_RIGHT	=10,	// ctrl VK_RIGHT
	CTRL_CAMERA_ZOOM_INC		=11,	// VK_ADD
	CTRL_CAMERA_ZOOM_DEC		=12,	// VK_SUBTRACT
	CTRL_CAMERA_MOUSE_LOOK		=13,	// VK_RBUTTON
	CTRL_GAME_HELP				=14,	// h
	CTRL_ESCAPE					=15,	// VK_ESCAPE

	CTRL_CAMERA_SAVE1           =16,	//пока VK_F1
	CTRL_CAMERA_SAVE2           =17,	//пока VK_F2
	CTRL_CAMERA_SAVE3           =18,	//пока VK_F3
	CTRL_CAMERA_SAVE4           =19,	//пока VK_F4
	CTRL_CAMERA_SAVE5           =20,	//пока VK_F5
	CTRL_CAMERA_RESTORE1        =21,	//пока shift-VK_F1
	CTRL_CAMERA_RESTORE2        =22,	//пока shift-VK_F2
	CTRL_CAMERA_RESTORE3        =23,	//пока shift-VK_F3
	CTRL_CAMERA_RESTORE4        =24,	//пока shift-VK_F4
	CTRL_CAMERA_RESTORE5        =25,	//пока shift-VK_F5

	CTRL_CAMERA_TO_EVENT        =26,	// VK_CONTROL

	CTRL_CAMERA_MAP_SHIFT       =27,	// VK_CONTROL

	CTRL_TOGGLE_MUSIC = 28,
	CTRL_TOGGLE_SOUND = 29,
	CTRL_LOAD = 30,
	CTRL_SAVE = 31,
	CTRL_HOLD_PRODUCTION = 32,
	CTRL_TOGGLE_LIFEBARS = 33,

	CTRL_MAX
};

class ControlsConverter 
{
	unsigned char KeyToCtrl[2048];		// таблица перехода от кнопок в команды
	sKey CtrlToKey[CTRL_MAX];		// таблица перехода от команд к кнопкам
	string KeyToName[2048];				// таблица перехода от кнопок в имена кнопок
	string CtrlToDeclaration[CTRL_MAX];	// таблица перехода от команд в описание команд

public:
	ControlsConverter(){ LoadKeyNameTable(); }
	void LoadKeyNameTable();
	void LoadCtrlTable(const char *strMain);
	void SaveControls(int ctrl, int key);

	const string& declaration(int ctrl) const { return CtrlToDeclaration[ctrl]; }
	const string& name(int key) const { return KeyToName[key]; }
	unsigned char control(int key) const { return KeyToCtrl[key]; }
	const sKey& key(int ctrl) const { return CtrlToKey[ctrl]; }
};

extern ControlsConverter g_controls_converter;

#endif //__CONTROLS_H__