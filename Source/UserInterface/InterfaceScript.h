#ifndef __INTERFACE_SCRIPT_H__
#define __INTERFACE_SCRIPT_H__

#include "..\Util\Serialization.h"
#include "..\Util\TypeLibrary.h"

#ifndef _UNIT_ATTRIBUTE_INL_
#define _UNIT_ATTRIBUTE_INL_
#include "UnitAttribute.inl"
#include "SquadShellEnums.inl"
#endif // _UNIT_ATTRIBUTE_INL_

/// интерфейсное событие
struct iEvent
{
	EnumWrapper<iEventID> ID;
	EnumWrapper<ShellControlID> controlID;
	int data;

	iEvent()
	{
		ID = iEVENT_ID_DEFAULT;
		controlID = SQSH_STATIC_ID;
		data = 0;
	}

	template<class Archive>
	void serialize(Archive& ar)
	{
		ar & TRANSLATE_OBJECT(ID, "код события");
		ar & TRANSLATE_OBJECT(controlID, "controlID");
		ar & TRANSLATE_OBJECT(data, "данные события");
	}
};

struct ControlAction {
	PrmString name;
	EnumWrapper<InterfaceEventCode> code;
	int param;
	PrmString hotKeyPopup;

	ControlAction() {
		code = EVENT_PRESSED;
		param = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "&name");
		ar & TRANSLATE_OBJECT(code, "Код");
		ar & TRANSLATE_OBJECT(param, "Параметр");
		ar & TRANSLATE_OBJECT(hotKeyPopup, "hotKeyPopup");
	}
};

struct BGObj
{
	PrmString name;
	PrmString chainName;

	BGObj() { }

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "&name");
		ar & TRANSLATE_OBJECT(chainName, "&chainName");
	}
};

struct sqshImage
{
	PrmString texture;
	float _ix;
	float _iy;
	float ix;
	float iy;
	float dx;
	float dy;
	float dsx;
	float dsy;
	int hasResolutionVersion;
	int hasBelligerentVersion;

	sqshImage() {
		_ix = 0;
		_iy = 0;
		ix = 1024;
		iy = 1024;
		dx = 0;
		dy = 0;
		dsx = 0;
		dsy = 0;
		hasResolutionVersion = 1;
		hasBelligerentVersion = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(texture, "&texture");
		ar & TRANSLATE_OBJECT(_ix, "ix_");
		ar & TRANSLATE_OBJECT(_iy, "iy_");
		ar & TRANSLATE_OBJECT(ix, "ix");
		ar & TRANSLATE_OBJECT(iy, "iy");
		ar & TRANSLATE_OBJECT(dx, "dx");
		ar & TRANSLATE_OBJECT(dy, "dy");
		ar & TRANSLATE_OBJECT(dsx, "dsx");
		ar & TRANSLATE_OBJECT(dsy, "dsy");
		ar & TRANSLATE_OBJECT(hasResolutionVersion, "hasResolutionVersion");
		ar & TRANSLATE_OBJECT(hasBelligerentVersion, "hasBelligerentVersion");
	}
};

struct sqshControl
{
	EnumWrapper<ShellControlType>  type;
	EnumWrapper<ShellControlID>    id;

	BitVector<SHELL_CONTROL_STATE> state;
	float x;
	float y;
	float sx;
	float sy;
	float xstart;	
	float ystart;	

	sqshImage image;
	sqshImage image_h;
	sqshImage image_check;
	sqshImage image_disabled;

	sqshImage image2;
	sqshImage image2_h;
	sqshImage image2_check;
	sqshImage image2_disabled;

	PrmString sound;
	PrmString popup;
	PrmString popupChecked;
	int target;
	int parent_id;

	int diagram_delta_x;
	int diagram_delta_sx;
	int diagram_delta_y;
	int diagram_delta_sy;

	BGObj bgObject;

	//текст и его смещение для кнопок меню
	PrmString text;
	float    txt_dx;
	float    txt_dy;
	EnumWrapper<SHELL_ALIGN>  txt_align;
	EnumWrapper<SHELL_ALIGN>  txt_vert_align;
	int    font_group;

	//flashing
	int flashR;
	int flashG;
	int flashB;
	int flashA;

	int flashBG_R;
	int flashBG_G;
	int flashBG_B;
	int flashBG_A;

	float tickTime;

	//diagram
	int diaR;
	int diaG;
	int diaB;
	int diaA;
	
	//actions
	vector<ControlAction> actions;

	int passive;
	int flashWithImage;
	int alnum;
	
	EnumWrapper<ControlHitTestMode> hitTestMode;

	int canHandleDisabledInput;

	sqshControl() {
		type   = SQSH_GENERAL_TYPE;
		id     = SQSH_STATIC_ID;

		state  = SQSH_VISIBLE|SQSH_ENABLED;
		x    = 0;
		y    = 0;
		sx   = 0;
		sy   = 0;
		xstart = 0;	
		ystart = 0;	

		sound = "button_press";
		target = -1;
		parent_id = -1;

		diagram_delta_x = 0;
		diagram_delta_sx = 0;
		diagram_delta_y = 0;
		diagram_delta_sy = 0;

		txt_dx = 0;
		txt_dy = 0;
		txt_align = SHELL_ALIGN_CENTER;
		txt_vert_align = SHELL_ALIGN_LEFT;
		font_group = 1;

		flashR = 255;
		flashG = 0;
		flashB = 0;
		flashA = 255;

		flashBG_R = 255;
		flashBG_G = 0;
		flashBG_B = 0;
		flashBG_A = 255;

		tickTime = 900;

		diaR = 0;
		diaG = 255;
		diaB = 0;
		diaA = 64;
		
		passive = 0;
		flashWithImage = 0;
		alnum = 1;
		
		hitTestMode = HITTEST_DEFAULT; 

		canHandleDisabledInput = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(type, "type");
		ar & TRANSLATE_OBJECT(id, "id");
		ar & TRANSLATE_OBJECT(state, "state");
		ar & TRANSLATE_OBJECT(x, "x");
		ar & TRANSLATE_OBJECT(y, "y");
		ar & TRANSLATE_OBJECT(sx, "sx");
		ar & TRANSLATE_OBJECT(sy, "sy");
		ar & TRANSLATE_OBJECT(xstart, "xstart");
		ar & TRANSLATE_OBJECT(ystart, "ystart");

		ar & TRANSLATE_OBJECT(image, "image");
		ar & TRANSLATE_OBJECT(image_h, "image_h");
		ar & TRANSLATE_OBJECT(image_check, "image_check");
		ar & TRANSLATE_OBJECT(image_disabled, "image_disabled");

		ar & TRANSLATE_OBJECT(image2, "image2");
		ar & TRANSLATE_OBJECT(image2_h, "image2_h");
		ar & TRANSLATE_OBJECT(image2_check, "image2_check");
		ar & TRANSLATE_OBJECT(image2_disabled, "image2_disabled");

		ar & TRANSLATE_OBJECT(sound, "sound");
		ar & TRANSLATE_OBJECT(popup, "popup");
		ar & TRANSLATE_OBJECT(popupChecked, "popupChecked");
		ar & TRANSLATE_OBJECT(target, "target");
		ar & TRANSLATE_OBJECT(parent_id, "parent_id");

		ar & TRANSLATE_OBJECT(diagram_delta_x, "diagram_delta_x");
		ar & TRANSLATE_OBJECT(diagram_delta_sx, "diagram_delta_sx");
		ar & TRANSLATE_OBJECT(diagram_delta_y, "diagram_delta_y");
		ar & TRANSLATE_OBJECT(diagram_delta_sy, "diagram_delta_sy");

		ar & TRANSLATE_OBJECT(bgObject, "bgObject");

		ar & TRANSLATE_OBJECT(text, "text");
		ar & TRANSLATE_OBJECT(txt_dx, "txt_dx");
		ar & TRANSLATE_OBJECT(txt_dy, "txt_dy");
		ar & TRANSLATE_OBJECT(txt_align, "txt_align");
		ar & TRANSLATE_OBJECT(txt_vert_align, "txt_vert_align");
		ar & TRANSLATE_OBJECT(font_group, "font_group");

		ar & TRANSLATE_OBJECT(flashR, "flashR");
		ar & TRANSLATE_OBJECT(flashG, "flashG");
		ar & TRANSLATE_OBJECT(flashB, "flashB");
		ar & TRANSLATE_OBJECT(flashA, "flashA");

		ar & TRANSLATE_OBJECT(flashBG_R, "flashBG_R");
		ar & TRANSLATE_OBJECT(flashBG_G, "flashBG_G");
		ar & TRANSLATE_OBJECT(flashBG_B, "flashBG_B");
		ar & TRANSLATE_OBJECT(flashBG_A, "flashBG_A");

		ar & TRANSLATE_OBJECT(tickTime, "tickTime");

		ar & TRANSLATE_OBJECT(diaR, "diaR");
		ar & TRANSLATE_OBJECT(diaG, "diaG");
		ar & TRANSLATE_OBJECT(diaB, "diaB");
		ar & TRANSLATE_OBJECT(diaA, "diaA");
		
		ar & TRANSLATE_OBJECT(actions, "actions");

		ar & TRANSLATE_OBJECT(passive, "passive");
		ar & TRANSLATE_OBJECT(flashWithImage, "flashWithImage");
		ar & TRANSLATE_OBJECT(alnum, "alnum");
		
		ar & TRANSLATE_OBJECT(hitTestMode, "hitTestMode");

		ar & TRANSLATE_OBJECT(canHandleDisabledInput, "canHandleDisabledInput");
	}
};

struct sqshControlContainer
{
	EnumWrapper<ShellControlType>	type;
	int			id;
	int         state;
	int         load_group;
	int         x;
	int         y;
	int         sx;
	int         sy;
	sqshImage   image;
	sqshImage   image2;

	vector<sqshControl> controls;;
	vector<BGObj> bgObjects;

	EnumWrapper<ShellControlID> defaultEnterBtnID;
	EnumWrapper<ShellControlID> defaultEnterBtnIDAux;
	EnumWrapper<ShellControlID> defaultEscBtnID;

	sqshControlContainer() {
		type = SQSH_GENERAL_TYPE;
		id = SQSH_STATIC_ID;
		state = 0;
		load_group = 0;
		x = 0;
		y = 0;
		sx = 0;
		sy = 0;

		defaultEnterBtnID = SQSH_STATIC_ID;
		defaultEnterBtnIDAux = SQSH_STATIC_ID;
		defaultEscBtnID = SQSH_STATIC_ID;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(type, "type");
		ar & TRANSLATE_OBJECT(id, "id");
		ar & TRANSLATE_OBJECT(state, "state");
		ar & TRANSLATE_OBJECT(load_group, "load_group");
		ar & TRANSLATE_OBJECT(x, "x");
		ar & TRANSLATE_OBJECT(y, "y");
		ar & TRANSLATE_OBJECT(sx, "sx");
		ar & TRANSLATE_OBJECT(sy, "sy");
		ar & TRANSLATE_OBJECT(image, "image");
		ar & TRANSLATE_OBJECT(image2, "image2");
		ar & TRANSLATE_OBJECT(controls, "controls");
		ar & TRANSLATE_OBJECT(bgObjects, "bgObjects");
		ar & TRANSLATE_OBJECT(defaultEnterBtnID, "defaultEnterBtnID");
		ar & TRANSLATE_OBJECT(defaultEnterBtnIDAux, "defaultEnterBtnIDAux");
		ar & TRANSLATE_OBJECT(defaultEscBtnID, "defaultEscBtnID");
	}
};

/*
struct _V2i
{
	int x;
	int y;

	_V2i(){ x = 0; y = 0; }

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(x, "x");
		ar & TRANSLATE_OBJECT(y, "y");
	}
};

struct _V2f
{
	float x;
	float y;

	_V2f(){ x = 0; y = 0; }

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(x, "x");
		ar & TRANSLATE_OBJECT(y, "y");
	}
};
*/
struct TabIcon {
	sqshImage image;
	sqshImage image_h;
	sqshImage image_check;
	sqshImage image_disabled;
	sqshImage image_flashing;

	TabIcon() { }

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(image, "image");
		ar & TRANSLATE_OBJECT(image_h, "image_h");
		ar & TRANSLATE_OBJECT(image_check, "image_check");
		ar & TRANSLATE_OBJECT(image_disabled, "image_disabled");
		ar & TRANSLATE_OBJECT(image_flashing, "image_flashing");
	}
};

struct sqshTabElement
{
	float x;
	float y;
	float sx;
	float sy;

	TabIcon icon; 

	float activeX;
	float activeY;
	float activeSX;
	float activeSY;
	sqshImage activeTabImage;

	vector<int> ctrls;

	sqshTabElement()
	{
		x    = 0;
		y    = 0;
		sx   = 0;
		sy   = 0;

		activeX    = 0;
		activeY    = 0;
		activeSX   = 0;
		activeSY   = 0;

		ctrls.resize(21,-1);
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(x, "x");
		ar & TRANSLATE_OBJECT(y, "y");
		ar & TRANSLATE_OBJECT(sx, "sx");
		ar & TRANSLATE_OBJECT(sy, "sy");

		ar & TRANSLATE_OBJECT(icon, "icon");

		ar & TRANSLATE_OBJECT(activeX, "activeX");
		ar & TRANSLATE_OBJECT(activeY, "activeY");
		ar & TRANSLATE_OBJECT(activeSX, "activeSX");
		ar & TRANSLATE_OBJECT(activeSY, "activeSY");
		ar & TRANSLATE_OBJECT(activeTabImage, "activeTabImage");

		ar & TRANSLATE_OBJECT(ctrls, "ctrls");
	}
};
struct sqshTabSheet
{
	int id;
	float x;
	float y;
	float sx;
	float sy;

	float dx;
	float dy;
	float dsx;
	float dsy;

	vector<sqshTabElement> pages;

	vector<ControlAction> actions;

	int flashR;
	int flashG;
	int flashB;
	int flashA;

	int flashBG_R;
	int flashBG_G;
	int flashBG_B;
	int flashBG_A;

	float tickTime;

	sqshTabSheet()
	{
		id = -1;
		x = 0.0f;
		y = 0.0f;
		sx = 0.0f;
		sy = 0.0f;

		dx    = 0;
		dy    = 0;
		dsx   = 0;
		dsy   = 0;

		flashR = 255;
		flashG = 0;
		flashB = 0;
		flashA = 255;

		flashBG_R = 255;
		flashBG_G = 0;
		flashBG_B = 0;
		flashBG_A = 255;

		tickTime = 900;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(id, "id");
		ar & TRANSLATE_OBJECT(x, "x");
		ar & TRANSLATE_OBJECT(y, "y");
		ar & TRANSLATE_OBJECT(sx, "sx");
		ar & TRANSLATE_OBJECT(sy, "sy");

		ar & TRANSLATE_OBJECT(dx, "dx");
		ar & TRANSLATE_OBJECT(dy, "dy");
		ar & TRANSLATE_OBJECT(dsx, "dsx");
		ar & TRANSLATE_OBJECT(dsy, "dsy");

		ar & TRANSLATE_OBJECT(pages, "pages");
		ar & TRANSLATE_OBJECT(actions, "actions");

		ar & TRANSLATE_OBJECT(flashR, "flashR");
		ar & TRANSLATE_OBJECT(flashG, "flashG");
		ar & TRANSLATE_OBJECT(flashB, "flashB");
		ar & TRANSLATE_OBJECT(flashA, "flashA");

		ar & TRANSLATE_OBJECT(flashBG_R, "flashBG_R");
		ar & TRANSLATE_OBJECT(flashBG_G, "flashBG_G");
		ar & TRANSLATE_OBJECT(flashBG_B, "flashBG_B");
		ar & TRANSLATE_OBJECT(flashBG_A, "flashBG_A");

		ar & TRANSLATE_OBJECT(tickTime, "tickTime");
	}
};
/*
struct sqshCursor
{
	int system;
	int hotspot_center;
	PrmString image;
	int sx;
	int sy;

	sqshCursor()
	{
		system = 0;
		hotspot_center = 0;
		sx = 32;
		sy = 32;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(system, "system");
		ar & TRANSLATE_OBJECT(hotspot_center, "hotspot_center");
		ar & TRANSLATE_OBJECT(image, "image");
		ar & TRANSLATE_OBJECT(sx, "sx");
		ar & TRANSLATE_OBJECT(sy, "sy");
	}
};


struct ShellGalogramType
{
	PrmString model;
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

	int	camera_x;
	int camera_y;
	int camera_sx;
	int camera_sy;
	float model_rate;

	ShellGalogramType()
	{
		radius = 50;
		pos_dx = 0;
		pos_dy = 0;
		angle_x = 90;
		angle_y = 0;
		angle_z = 0;

		r1 = 1;
		g1 = 0;
		b1 = 0;

		r2 = 0;
		g2 = 1;
		b2 = 0;

		camera_x = 145;
		camera_y = 590;
		camera_sx  = 141;
		camera_sy  = 147;
		model_rate = 200;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(model, "model");
		ar & TRANSLATE_OBJECT(radius, "radius");
		ar & TRANSLATE_OBJECT(pos_dx, "pos_dx");
		ar & TRANSLATE_OBJECT(pos_dy, "pos_dy");
		ar & TRANSLATE_OBJECT(angle_x, "angle_x");
		ar & TRANSLATE_OBJECT(angle_y, "angle_y");
		ar & TRANSLATE_OBJECT(angle_z, "angle_z");

		ar & TRANSLATE_OBJECT(r1, "r1");
		ar & TRANSLATE_OBJECT(g1, "g1");
		ar & TRANSLATE_OBJECT(b1, "b1");

		ar & TRANSLATE_OBJECT(r2, "r2");
		ar & TRANSLATE_OBJECT(g2, "g2");
		ar & TRANSLATE_OBJECT(b2, "b2");

		ar & TRANSLATE_OBJECT(camera_x, "camera_x");
		ar & TRANSLATE_OBJECT(camera_y, "camera_y");
		ar & TRANSLATE_OBJECT(camera_sx, "camera_sx");
		ar & TRANSLATE_OBJECT(camera_sy, "camera_sy");
		ar & TRANSLATE_OBJECT(model_rate, "model_rate");
	}
};

struct EventSymbol {
	PrmString image;
	float sx;
	float sy;
	float period;
	float animTime;
	PrmString snd;

	EventSymbol() 
	{
		sx = 16/1024;
		sy = 16/768;
		period = 3000;
		animTime = 3000;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(image, "image");
		ar & TRANSLATE_OBJECT(sx, "sx");
		ar & TRANSLATE_OBJECT(sy, "sy");
		ar & TRANSLATE_OBJECT(period, "period");
		ar & TRANSLATE_OBJECT(animTime, "animTime");
		ar & TRANSLATE_OBJECT(snd, "snd");
	}
};

struct _ProgressBar
{
	_V2i points[2];

	_ProgressBar(){ }

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(points, "points");
	}
};

struct unitClassDesc
{
	int unitClass;
	PrmString textID;

	unitClassDesc(){
		unitClass = 0;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(unitClass, "unitClass");
		ar & TRANSLATE_OBJECT(textID, "textID");
	}
};

struct Reel
{
	PrmString name;
	int abortEnabled;
	int localized;
	int video;
	int time;

	Reel()
	{
		abortEnabled = 1;
		localized = 0;
		video = 1;
		time = 4000;
	}

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(name, "name");
		ar & TRANSLATE_OBJECT(abortEnabled, "abortEnabled");
		ar & TRANSLATE_OBJECT(localized, "localized");
		ar & TRANSLATE_OBJECT(video, "video");
		ar & TRANSLATE_OBJECT(time, "time");
	}
};
*/

struct InterfaceAttributes
{
	vector<TabIcon> tabIcons;
	vector<sqshTabSheet> tabSheets;
	vector<sqshControlContainer> controls;

	InterfaceAttributes(){ };

	template<class Archive>
	void serialize(Archive& ar) {
		ar & TRANSLATE_OBJECT(tabIcons, "tabIcons");
		ar & TRANSLATE_OBJECT(tabSheets, "tabSheets");
		ar & TRANSLATE_OBJECT(controls, "controls");
	}
};

extern SingletonPrm<InterfaceAttributes> interfaceAttr;

#endif /* __INTERFACE_SCRIPT_H__ */
