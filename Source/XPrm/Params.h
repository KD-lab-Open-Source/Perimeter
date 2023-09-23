//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript declaration
//	Section: SquadShellUIPrm
//	Number of variables: 122
//	This is computer generated code, don't try to change it
//////////////////////////////////////////////////////////////////////////////////////////////
extern int naaa;
extern char const* const* aaaa;
#ifndef __XScript_struct_sqshImage__
#define __XScript_struct_sqshImage__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct sqshImage {
	char const* texture;
	float _ix;
	float _iy;
	float ix;
	float iy;
	int dx;
	int dy;
	int dsx;
	int dsy;
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_sqshImage__
#ifndef __XScript_struct_sqshControl__
#define __XScript_struct_sqshControl__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct sqshControl {
	SHELL_CONTROL_TYPE type;
	SHELL_CONTROL_ID id;
	int state;
	int x;
	int y;
	int sx;
	int sy;
	float xstart;
	float ystart;
	sqshImage image;
	sqshImage image_h;
	sqshImage image_check;
	sqshImage image_disabled;
	char const* sound;
	char const* popup;
	char const* tutorial_name;
	int target;
	int parent_id;
	int diagram_delta_x;
	int diagram_delta_sx;
	int diagram_delta_y;
	int diagram_delta_sy;
	char const* text;
	int txt_dx;
	int txt_dy;
	SHELL_ALIGN txt_align;
	int font_group;
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_sqshControl__
#ifndef __XScript_struct_sqshControlContainer__
#define __XScript_struct_sqshControlContainer__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct sqshControlContainer {
	SHELL_CONTROL_TYPE type;
	int id;
	int state;
	int load_group;
	int x;
	int y;
	int sx;
	int sy;
	sqshImage image;
	sqshImage image2;
	int ControlCount;
	sqshControl controls[95];
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_sqshControlContainer__
#ifndef __XScript_struct__V2i__
#define __XScript_struct__V2i__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct _V2i {
	int x;
	int y;
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct__V2i__
#ifndef __XScript_struct_sqshTabElement__
#define __XScript_struct_sqshTabElement__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct sqshTabElement {
	char const* image;
	int size_x;
	int size_y;
	float x;
	float y;
	float ix;
	float iy;
	int scr_x;
	int scr_y;
	int dsx;
	int dsy;
	int ctrl_count;
	int ctrls[21];
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_sqshTabElement__
#ifndef __XScript_struct_sqshTabSheet__
#define __XScript_struct_sqshTabSheet__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct sqshTabSheet {
	int id;
	int scr_x;
	int scr_y;
	int page_count;
	sqshTabElement pages[6];
	int page_select_count;
	sqshTabElement pages_select[6];
	int inactive_tab_count;
	sqshTabElement inactive_tabs[6];
	int inactive_tab_h_count;
	sqshTabElement inactive_tabs_h[6];
	int inactive_tab_sel_count;
	sqshTabElement inactive_tabs_sel[6];
	int active_label_count;
	sqshTabElement active_labels[6];
	int var_label_count;
	_V2i var_label[7];
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_sqshTabSheet__
extern int _sqsh_sheet_count;
extern sqshTabSheet _sqsh_sheets[];
extern int squad_icon_count;
extern sqshImage _squad_icons_table[];
extern _V2i squad_icon_size;
extern int _sqsh_control_count;
extern sqshControlContainer _sqsh_controls[];
#ifndef __XScript_struct_sqshCursor__
#define __XScript_struct_sqshCursor__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct sqshCursor {
	int system;
	int hotspot_center;
	char const* image;
	int sx;
	int sy;
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_sqshCursor__
extern int _sqsh_cursor_count;
extern sqshCursor _sqsh_cursors[];
#ifndef __XScript_struct_ShellGalogramType__
#define __XScript_struct_ShellGalogramType__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct ShellGalogramType {
	char const* model;
	float radius;
	float pos_dx;
	float pos_dy;
	float angle_x;
	float angle_y;
	float angle_z;
	float r1;
	float g1;
	float b1;
	float r2;
	float g2;
	float b2;
	int camera_x;
	int camera_y;
	int camera_sx;
	int camera_sy;
	float model_rate;
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_ShellGalogramType__
extern int shell_galogram_table_size;
extern ShellGalogramType shell_galogram_table[];
extern char const* shell_galogram_texture;
extern float sqshProrgessBarHeight;
extern float sqshVScrollSX;
extern float sqshVScrollThmbSY;
extern float sqshCursorWorkAreaSize;
extern float sqshCursorWorkAreaDx;
extern float sqshCursorWorkAreaDy;
extern float sqshDisabledR;
extern float sqshDisabledG;
extern float sqshDisabledB;
extern float sqshDisabledA;
extern char const* sqshShellMainFont;
extern char const* sqshShellMainFont1;
extern char const* sqshShellMainFont2;
extern char const* sqshFontPopup;
extern char const* shell_main_menu_font;
extern int shell_main_menu_font_size1;
extern int shell_main_menu_font_size2;
extern int shell_main_menu_font_size3;
extern int nPopupTextPosX;
extern int nPopupTextPosY;
extern int nPopupSX;
extern int nPopupSY;
extern char const* sPopupTexture;
extern float shellCursorAddRegionSize;
extern int _buttonLabelX;
extern int _buttonLabelY;
extern float _squad_buttons_dx;
extern int _button_atom1_x;
extern int _button_atom1_y;
extern int _button_atom2_x;
extern int _button_atom2_y;
extern int _button_atom3_x;
extern int _button_atom3_y;
extern int _button_atom4_x;
extern int _button_atom4_y;
extern int _button_atom5_x;
extern int _button_atom5_y;
extern int _button_pict_size;
extern int nNrjOutputX;
extern int nNrjOutputY;
extern int nNrjUsedX;
extern int nNrjUsedY;
extern float _map_window_dx;
extern float _map_window_dy;
extern float _map_window_sx;
extern float _map_window_sy;
extern int horde_draw_radius;
#ifndef __XScript_struct__ProgressBar__
#define __XScript_struct__ProgressBar__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct _ProgressBar {
	_V2i points[2];
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct__ProgressBar__
extern float fButtonEffectSizeSpeed;
extern float fButtonEffectAlphaSpeed;
extern int tDeselectInterval;
extern float fWorkAreaCenterX;
extern float fWorkAreaCenterY;
extern float empty_x1;
extern float empty_x2;
extern float empty_y1;
extern float empty_y2;
extern float brig_x1;
extern float brig_x2;
extern float brig_y1;
extern float brig_y2;
extern float build_x1;
extern float build_x2;
extern float build_y1;
extern float build_y2;
extern float hover_x1;
extern float hover_x2;
extern float hover_y1;
extern float hover_y2;
extern int hover_sx;
extern int hover_sy;
extern _ProgressBar _bar_squad;
extern int sound_shield_on_delay;
extern char const* sMessagesPromptFile;
extern int nPromptPosX;
extern int nPromptPosY;
extern int nShowNewClusterMsgDelay;
extern int nPeriodMsg;
extern int nDefaultMsgDelayTime;
extern int nMsgRectX1;
extern int nMsgRectY1;
extern int nMsgRectX2;
extern int nMsgRectY2;
extern char const* tutMessageFile;
extern char const* tutSoundPath;
extern int tutTextWindowDx;
extern int tutTextWindowDy;
extern int tutMarkWindowPeriod;
extern int nAtomPerSqadMax;
extern _ProgressBar _bar_load;
extern int progress_tex_sz;
extern char const* progress_texture;
extern int splash_delay_time;
extern int alpha_delta;
extern int _bDebugDisplayAllIcons;
extern char const* sRegionTextureMain;
extern char const* sRegionTextureWall;
extern char const* sRegionTextureAbyss;
extern char const* sRegionTextureCircle;
extern char const* sRegionTextureEnergy;
extern char const* sRegionTextureSelection;
extern float fBackgrTexturePeriod;
extern int nBackgrTextureR;
extern int nBackgrTextureG;
extern int nBackgrTextureB;
extern int nBackgrTextureA;
extern int nBackgrMode;
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript end: SquadShellUIPrm
//////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __PARAMS_H__
#define __PARAMS_H__

//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript declaration
//	Section: CameraPrm
//	Number of variables: 9
//	This is computer generated code, don't try to change it
//////////////////////////////////////////////////////////////////////////////////////////////
extern double camera_clip_mass_inv;
extern double camera_position_mass_inv;
extern double camera_slope_mass_inv;
extern double camera_water_slope_mass_inv;
extern double camera_turn_mass_inv;
extern double camera_distance_mass_inv;
extern double camera_centering_mass_inv;
extern double camera_const_input_mass_inv;
extern double camera_damping_factor;
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript end: CameraPrm
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript declaration
//	Section: XRealPrm
//	Number of variables: 2
//	This is computer generated code, don't try to change it
//////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __XScript_struct_D__
#define __XScript_struct_D__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct D {
	int a;
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_D__
#ifndef __XScript_struct_U__
#define __XScript_struct_U__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct U {
	D const* a;
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_U__
#ifndef __XScript_struct_UU__
#define __XScript_struct_UU__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct UU {
	U u[2];
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_UU__
extern int aa;
extern UU u;
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript end: XRealPrm
//////////////////////////////////////////////////////////////////////////////////////////////

#endif // __PARAMS_H__
