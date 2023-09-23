////////////////////////////////////////////////////////////////////
//		Global Parameters 
////////////////////////////////////////////////////////////////////



#include "StdAfx.h"
#include "PrmEdit.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript definition
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
static D u_a_1 = { 1 
  };
static U UU__a_0 = { 0 };
#ifndef __XScript_struct_UU__
#define __XScript_struct_UU__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct UU {
	U u[2];
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct_UU__
int aa = 1;
UU u = { { { &UU__a_0 
  }, { &u_a_1 } } };

#ifdef _PRM_EDIT_
struct XRealPrm_ParameterSection : ParameterSection
{
	XRealPrm_ParameterSection() : ParameterSection("XRealPrm")
{
	description = 864357161;
	add_dependency("sss\\aaa\\xreal.prm");
	reserve(2);
	add(&u_a_1, "u_a_1");
	add(&UU__a_0, "UU__a_0");
	add(&aa, "aa");
	add(&u, "u");
	add_parameter_section(this);
}
};
static XRealPrm_ParameterSection XRealPrm_ParameterSectionObject;
#endif  //  _PRM_EDIT_
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript end: XRealPrm
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript definition
//	Section: SquadShellUIPrm
//	Number of variables: 122
//	This is computer generated code, don't try to change it
//////////////////////////////////////////////////////////////////////////////////////////////
int naaa = 0;
char const* const* aaaa = 0;
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
int _sqsh_sheet_count = 2;
sqshTabSheet _sqsh_sheets[] = { { 59 
 , 675, 590, 5, { { "resource\\icons\\intf\\squad01.tga", 512, 256, 0.f, 0.f, 0.678f 
 , 0.711f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\squad02.tga", 512, 256 
 , 0.f, 0.f, 0.678f, 0.711f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\squad03.tga" 
 , 512, 256, 0.f, 0.f, 0.678f, 0.711f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\squad04.tga" 
 , 512, 256, 0.f, 0.f, 0.678f, 0.711f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\squad05.tga" 
 , 512, 256, 0.f, 0.f, 0.678f, 0.711f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1 } } }, 5, { { "resource\\icons\\intf\\squad01_sel.tga" 
 , 512, 256, 0.f, 0.f, 0.678f, 0.711f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\squad02_sel.tga" 
 , 512, 256, 0.f, 0.f, 0.678f, 0.711f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\squad03_sel.tga" 
 , 512, 256, 0.f, 0.f, 0.678f, 0.711f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\squad04_sel.tga" 
 , 512, 256, 0.f, 0.f, 0.678f, 0.711f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\squad05_sel.tga" 
 , 512, 256, 0.f, 0.f, 0.678f, 0.711f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1 } } }, 5, { { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 0.165f, 8.3e-002f, 0.233f, 0.107f, 1, 4, 0, 0, 21, { -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 0.165f, 8.3e-002f, 0.233f, 0.107f, 66, 4, 0, 0, 21, { -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 0.165f, 8.3e-002f, 0.233f, 0.107f, 131, 4, 0, 0, 21, { -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 0.165f, 8.3e-002f, 0.233f, 0.107f, 196, 4, 0, 0, 21, { -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 0.165f, 8.3e-002f, 0.233f, 0.107f, 261, 4, 0, 0, 21, { -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256 
 , 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } } }, 5, { { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 7.e-002f, 3.e-003f, 0.14f, 2.7e-002f, 1, 3, 0, 0, 21, { -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 7.e-002f, 3.e-003f, 0.14f, 2.7e-002f, 65, 3, 0, 0, 21, { -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 7.e-002f, 3.e-003f, 0.14f, 2.7e-002f, 130, 3, 0, 0, 21, { -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 7.e-002f, 3.e-003f, 0.14f, 2.7e-002f, 196, 3, 0, 0, 21, { -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 7.e-002f, 3.e-003f, 0.14f, 2.7e-002f, 261, 3, 0, 0, 21, { -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "" 
 , 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } } }, 5, { { "", 1024, 1024, 9.2e-002f 
 , 8.3e-002f, 0.16f, 0.107f, 2, 4, -2, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 1024, 1024, 9.2e-002f 
 , 8.3e-002f, 0.16f, 0.107f, 67, 4, -2, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 1024, 1024, 9.2e-002f 
 , 8.3e-002f, 0.16f, 0.107f, 132, 4, -2, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 1024, 1024, 9.2e-002f 
 , 8.3e-002f, 0.16f, 0.107f, 196, 4, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 1024, 1024, 9.2e-002f 
 , 8.3e-002f, 0.16f, 0.107f, 261, 4, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1 } } }, 5, { { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 5, 7, 0, 0 
 , 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 70, 7, 0, 0, 21, { -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256 
 , 256, 0.f, 0.f, 1.f, 1.f, 135, 7, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f 
 , 1.f, 196, 7, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 261, 7, 0, 0 
 , 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } } }, 7, { { 0 
 , 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } }, { 58, 145, 591 
 , 3, { { "resource\\icons\\intf\\frame.tga", 512, 256, 4.e-003f, 1.e-002f, 0.623f 
 , 0.704f, 0, 0, 0, 0, 5, { 28, 29, 30, 31, 32, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\building.tga", 512, 256 
 , 4.e-003f, 1.e-002f, 0.623f, 0.702f, 0, 0, 0, 0, 12, { 11, 12, 13, 14, 15, 16, 17 
 , 18, 19, 20, 21, 22, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\weapon.tga" 
 , 512, 256, 4.e-003f, 1.e-002f, 0.623f, 0.702f, 0, 0, 0, 0, 5, { 23, 24, 25, 26, 27 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } } } 
 , 0, { { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "" 
 , 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1 } } }, 3, { { "resource\\icons\\intf\\all.tga", 1024, 1024 
 , 0.f, 0.116f, 6.8e-002f, 0.139f, 0, 4, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 0.f, 0.f, 6.8e-002f, 2.3e-002f, 67, 4, 0, 0, 21, { -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 0.f, 0.142f, 6.8e-002f, 0.165f, 134, 4, 0, 0, 21, { -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } } } 
 , 3, { { "resource\\icons\\intf\\all.tga", 1024, 1024, 4.6e-002f, 5.e-002f, 0.114f 
 , 7.3e-002f, 0, 4, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga", 1024, 1024 
 , 4.6e-002f, 5.e-002f, 0.114f, 7.3e-002f, 67, 4, 0, 0, 21, { -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "resource\\icons\\intf\\all.tga" 
 , 1024, 1024, 4.6e-002f, 5.e-002f, 0.114f, 7.3e-002f, 134, 4, 0, 0, 21, { -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "" 
 , 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } 
  } }, 6, { { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "" 
 , 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1 } } }, 6, { { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
 , 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256 
 , 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1 } }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } 
  }, { "", 256, 256, 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0, 21, { -1, -1, -1, -1, -1, -1, -1 
 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } } }, 7, { { 0, 0 }, { 0 
 , 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } } };
int squad_icon_count = 20;
sqshImage _squad_icons_table[] = { { "resource\\icons\\intf\\all.tga" 
 , 0.28f, 5.2e-002f, 0.341f, 7.7e-002f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.28f, 0.131f, 0.341f, 0.156f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.28f, 0.21f, 0.341f, 0.235f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.342f 
 , 0.131f, 0.402f, 0.156f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.342f 
 , 0.21f, 0.402f, 0.235f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.28f, 7.8e-002f 
 , 0.341f, 0.103f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.28f, 0.157f 
 , 0.341f, 0.182f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.28f, 0.236f 
 , 0.341f, 0.262f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.342f, 0.157f 
 , 0.402f, 0.182f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.342f, 0.236f 
 , 0.402f, 0.262f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.28f, 0.104f 
 , 0.341f, 0.13f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.28f, 0.184f, 0.341f 
 , 0.209f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.342f, 5.2e-002f, 0.402f 
 , 7.7e-002f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.342f, 0.184f, 0.402f 
 , 0.209f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.403f, 5.2e-002f, 0.464f 
 , 7.7e-002f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.342f, 7.8e-002f, 0.402f 
 , 0.103f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.342f, 0.104f, 0.402f 
 , 0.13f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.403f, 7.8e-002f, 0.464f 
 , 0.103f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.e-003f, 0.968f, 5.2e-002f 
 , 0.993f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.128f, 5.3e-002f, 0.18f 
 , 7.2e-002f, 0, 0, 0, 0 } };
_V2i squad_icon_size = { 62, 24 };
int _sqsh_control_count = 22;
sqshControlContainer _sqsh_controls[] = { { SQSH_GENERAL_TYPE 
 , 1, 65, 1, 0, 0, 1024, 768, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, 94, { { SQSH_BACKGROUND_TYPE, SQSH_BACKGRND_ID, 3, 0, 576 
 , 1025, 192, 0.f, 0.f, { "resource\\icons\\intf\\background.tga", 0.f, 0.f, 1.f, 0.751f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\background2.tga", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "resource\\icons\\intf\\background3.tga", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1 
 , 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_MENU_BUTTON_ID 
 , 11, 0, 0, 60, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.214f, 0.f, 0.273f 
 , 2.e-002f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.214f, 2.e-002f, 0.273f 
 , 4.1e-002f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_TASK_BUTTON_ID, 11, 60, 0, 60, 21, 0.f 
 , 0.f, { "resource\\icons\\intf\\all.tga", 0.273f, 0.f, 0.332f, 2.e-002f, 0, 0, 0 
 , 0 }, { "resource\\icons\\intf\\all.tga", 0.272f, 2.e-002f, 0.332f, 4.1e-002f, -1 
 , 0, 1, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SELPANEL_MOVE_ID, 10, 608, 630, 63, 21 
 , 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 6.1e-002f, 0.202f, 0.123f, 0.222f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 3.e-003f, 0.463f, 6.5e-002f, 0.484f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.e-003f, 0.49f, 6.4e-002f, 0.51f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Move", "" 
 , -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_STOP_ID, 10, 608, 652, 63, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.299f, 6.2e-002f, 0.32f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 3.e-003f, 0.463f, 6.5e-002f, 0.484f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 2.e-003f, 0.49f, 6.4e-002f, 0.51f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "Stop", "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SELPANEL_SQ_ATTACK_ID, 10, 608, 674, 63 
 , 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 6.1e-002f, 0.241f, 0.123f, 0.261f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 3.e-003f, 0.463f, 6.5e-002f, 0.484f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.e-003f, 0.49f, 6.4e-002f, 0.51f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Attack" 
 , "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_SQ_BACK_ID, 10, 608, 696, 63, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 6.1e-002f, 0.221f, 0.123f, 0.242f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 3.e-003f, 0.463f, 6.5e-002f, 0.484f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 2.e-003f, 0.49f, 6.4e-002f, 0.51f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "Back", "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SELPANEL_STOP2_ID, 10, 608, 696, 63, 21 
 , 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.f, 0.299f, 6.2e-002f, 0.32f, 0, 0 
 , 0, 0 }, { "resource\\icons\\intf\\all.tga", 3.e-003f, 0.463f, 6.5e-002f, 0.484f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.e-003f, 0.49f, 6.4e-002f, 0.51f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Stop", "" 
 , -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_SQ_OFDEF_ID, 10, 608, 718, 63, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.28f, 6.2e-002f, 0.3f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 3.e-003f 
 , 0.463f, 6.5e-002f, 0.484f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.e-003f 
 , 0.49f, 6.4e-002f, 0.51f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "Offensive/defensive", "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } 
 , { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SELPANEL_SQ_PATROL_ID, 10, 608, 740, 63, 21 
 , 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 6.1e-002f, 0.279f, 0.123f, 0.3f, 0 
 , 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 3.e-003f, 0.463f, 6.5e-002f, 0.484f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.e-003f, 0.489f, 6.4e-002f, 0.51f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Patrol" 
 , "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_ONOFF_ID, 42, 608, 630, 63, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 6.1e-002f, 0.181f, 0.123f, 0.203f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 3.e-003f, 0.463f, 6.5e-002f, 0.484f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.181f, 6.2e-002f, 0.203f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "on/off", "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SELPANEL_SELL_ID, 10, 608, 652, 63, 21 
 , 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.f, 0.241f, 6.2e-002f, 0.261f, 0 
 , 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 3.e-003f, 0.463f, 6.5e-002f, 0.484f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, "button_press", "Sell", "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SELPANEL_UPGRADE_ID, 10, 608, 674, 63 
 , 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.f, 0.221f, 6.2e-002f, 0.242f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 3.e-003f, 0.463f, 6.5e-002f, 0.484f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, "button_press", "Upgrade", "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SELPANEL_UPGRADE_CORE_ID, 10, 608, 674 
 , 63, 41, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.f, 0.341f, 6.2e-002f, 0.381f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.2e-002f, 0.522f, 8.4e-002f, 0.562f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, "button_press", "<upgrade_core_btn>", "", -1, 2, 0, 0, 0, 0, "", 0 
 , 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SELPANEL_UPGRADE_LASER1_ID 
 , 10, 608, 674, 63, 41, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 6.1e-002f, 0.38f 
 , 0.123f, 0.42f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.2e-002f, 0.522f 
 , 8.4e-002f, 0.562f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "<upgrade_laser1_btn>", "", -1 
 , 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_UPGRADE_LASER2_ID, 10, 608, 674, 63, 41, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.38f, 6.2e-002f, 0.42f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.2e-002f 
 , 0.522f, 8.4e-002f, 0.562f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "<upgrade_laser2_btn>" 
 , "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_UPGRADE_BOMB1_ID, 10, 608, 674, 63, 41, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.419f, 6.2e-002f, 0.459f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 2.2e-002f, 0.522f, 8.4e-002f, 0.562f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "<upgrade_bomb1_btn>" 
 , "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_UPGRADE_BOMB2_ID, 10, 608, 674, 63, 41, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 6.1e-002f, 0.419f, 0.123f, 0.459f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 2.2e-002f, 0.522f, 8.4e-002f, 0.562f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "<upgrade_bomb2_btn>" 
 , "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_UPGRADE_ROCKET1_ID, 10, 608, 674, 63, 41, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.129f, 0.4585f, 0.191f, 0.499f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 2.2e-002f, 0.522f, 8.4e-002f, 0.562f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "<upgrade_rocket1_btn>" 
 , "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_UPGRADE_ROCKET2_ID, 10, 608, 674, 63, 41, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 6.9e-002f, 0.4585f, 0.13f, 0.499f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 2.2e-002f, 0.522f, 8.4e-002f, 0.562f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "<upgrade_rocket2_btn>" 
 , "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_UPGRADE_FLY_ID, 10, 608, 674, 63, 41, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.122f, 0.419f, 0.183f, 0.459f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 2.2e-002f, 0.522f, 8.4e-002f, 0.562f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "<upgrade_fly_btn>" 
 , "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_FIELD_ID, 42, 608, 674, 63, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 6.1e-002f, 0.26f, 0.123f, 0.281f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 3.e-003f, 0.463f, 6.5e-002f, 0.484f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 6.1e-002f, 0.299f, 0.123f, 0.32f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "Field", "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SELPANEL_FRAME_INSTALL_ID, 42, 608, 696 
 , 63, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.f, 0.241f, 6.2e-002f, 0.261f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 3.e-003f, 0.463f, 6.5e-002f, 0.484f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.f, 0.26f, 6.2e-002f, 0.281f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Install/uninstall" 
 , "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_FRAME_ALARM_ID, 10, 608, 674, 63, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 6.1e-002f, 0.221f, 0.123f, 0.242f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 3.e-003f, 0.463f, 6.5e-002f, 0.484f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Alarm", "" 
 , -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_BRIG_CHANGE_ID, 10, 608, 674, 63, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 6.1e-002f, 0.322f, 0.123f, 0.342f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 3.e-003f, 0.463f, 6.5e-002f, 0.484f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 2.e-003f, 0.49f, 6.4e-002f, 0.51f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "Build/Terrain Exchange", "", -1, 2, 0, 0, 0, 0, "", 0, 0 
 , SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SELPANEL_BRIG_BACK_ID 
 , 10, 608, 696, 63, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 6.1e-002f, 0.221f 
 , 0.123f, 0.242f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 3.e-003f, 0.463f 
 , 6.5e-002f, 0.484f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.e-003f, 0.49f 
 , 6.4e-002f, 0.51f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "Back to frame", "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SELPANEL_BRIG_BUILD_ID, 10, 608, 718, 63, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.322f, 6.2e-002f, 0.342f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 3.e-003f, 0.463f, 6.5e-002f, 0.484f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 2.e-003f, 0.49f, 6.4e-002f, 0.51f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "Build", "", -1, 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_PROGRESS_UNIT_CHARGE_TYPE, SQSH_SELPANEL_UNIT_CHARGE_ID, 10, 608, 718 
 , 63, 10, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.f, 0.322f, 6.2e-002f, 0.342f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 3.e-003f, 0.463f, 6.5e-002f, 0.484f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 2.e-003f, 0.49f, 6.4e-002f, 0.51f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1 
 , 2, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_MAP_WINDOW_TYPE, SQSH_MAP_WINDOW_ID 
 , 3, 8, 610, 146, 155, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "map", -1, -1, 0, 0, 0, 0, "", 0 
 , 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_WORKAREA2_ID, 7 
 , 80, 591, 62, 37, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.346f, 0.458f, 0.407f 
 , 0.496f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.415f, 0.457f, 0.479f 
 , 0.497f, -2, -2, 4, 4 }, { "resource\\icons\\intf\\all.tga", 0.418f, 0.409f, 0.479f 
 , 0.446f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Abyss Zone" 
 , "workarea2", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_WORKAREA3_ID, 7, 12, 591, 62, 37, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.269f, 0.458f, 0.33f, 0.496f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.415f, 0.457f, 0.479f, 0.497f, -2, -2, 4, 4 }, { "resource\\icons\\intf\\all.tga" 
 , 0.418f, 0.409f, 0.479f, 0.446f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "Zero Zone", "workarea3", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_FIELD_ON_ID, 3, 462, 592, 106, 25, 0.f 
 , 0.f, { "resource\\icons\\intf\\all.tga", 0.115f, 0.586f, 0.218f, 0.611f, 0, 0, 0 
 , 0 }, { "resource\\icons\\intf\\all.tga", 0.113f, 0.555f, 0.217f, 0.58f, 0, 0, 0 
 , 0 }, { "resource\\icons\\intf\\all.tga", 0.115f, 0.524f, 0.218f, 0.549f, 0, 0, 0 
 , 0 }, { "resource\\icons\\intf\\all.tga", 9.e-003f, 0.586f, 0.113f, 0.611f, 0, 0 
 , 0, 0 }, "button_press", "Switch the Energy Shield ON", "field", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_FIELD_OFF_ID 
 , 3, 569, 592, 106, 25, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.115f, 0.612f 
 , 0.218f, 0.636f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.113f, 0.555f 
 , 0.217f, 0.58f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.115f, 0.524f 
 , 0.218f, 0.549f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 9.e-003f, 0.612f 
 , 0.113f, 0.636f, 0, 0, 0, 0 }, "button_press", "Switch the Energy Shield OFF", "field" 
 , -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SPEED_PAUSE, 3, 120, 0, 35, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.331f, 0.f, 0.366f, 2.e-002f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.331f, 2.e-002f, 0.366f, 4.1e-002f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Pause", "" 
 , -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_SPEED_50, 3, 155, 0, 25, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.366f 
 , 0.f, 0.39f, 2.e-002f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.365f, 2.e-002f 
 , 0.39f, 4.1e-002f, -1, 0, 1, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Speed 50%", "", -1, -1, 0, 0 
 , 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SPEED_100 
 , 3, 180, 0, 25, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.39f, 0.f, 0.415f 
 , 2.e-002f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.389f, 2.e-002f, 0.415f 
 , 4.1e-002f, -1, 0, 1, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Speed 100%", "", -1, -1, 0, 0, 0, 0, "" 
 , 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE, SQSH_SPEED_150, 3 
 , 205, 0, 35, 21, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.415f, 0.f, 0.449f 
 , 2.e-002f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.414f, 2.e-002f, 0.449f 
 , 4.1e-002f, -1, 0, 1, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "Speed 200%", "", -1, -1, 0, 0, 0, 0, "" 
 , 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_TERRAINBUILDBUTTON_TYPE, SQSH_FRAME_TERRAIN_BUILD1_ID 
 , 11, 156, 631, 144, 22, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.311f, 0.597f, 0.453f, 0.62f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.311f, 0.569f, 0.453f, 0.59f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "xx", "" 
 , -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_TERRAINBUILDBUTTON_TYPE 
 , SQSH_FRAME_TERRAIN_BUILD2_ID, 11, 156, 658, 144, 22, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.311f, 0.597f 
 , 0.453f, 0.62f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.311f, 0.569f 
 , 0.453f, 0.59f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "xx", "", -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_TERRAINBUILDBUTTON_TYPE 
 , SQSH_FRAME_TERRAIN_BUILD3_ID, 11, 156, 685, 144, 22, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.311f, 0.597f 
 , 0.453f, 0.62f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.311f, 0.569f 
 , 0.453f, 0.59f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "xx", "", -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_TERRAINBUILDBUTTON_TYPE 
 , SQSH_FRAME_TERRAIN_BUILD4_ID, 11, 156, 712, 144, 22, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.311f, 0.597f 
 , 0.453f, 0.62f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.311f, 0.569f 
 , 0.453f, 0.59f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "xx", "", -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_TERRAINBUILDBUTTON_TYPE 
 , SQSH_FRAME_TERRAIN_BUILD5_ID, 11, 156, 739, 144, 22, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.311f, 0.597f 
 , 0.453f, 0.62f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.311f, 0.569f 
 , 0.453f, 0.59f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "xx", "", -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE 
 , SQSH_SOLDIER_ID, 3, 686, 626, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 1.e-003f, 0.868f, 5.3e-002f, 0.899f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -4, -3, 5, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "resource\\icons\\intf\\all.tga", 1.e-003f, 0.735f, 5.3e-002f, 0.766f 
 , 0, 0, 0, 0 }, "button_press", "<unit_base>", "soldier", -1, 59, -3, 2, -1, 1, "" 
 , 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_OFFICER_ID, 3, 686 
 , 659, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 1.e-003f, 0.901f, 5.3e-002f 
 , 0.932f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.f, 0.693f, 5.8e-002f 
 , 0.732f, -4, -3, 5, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 1.e-003f, 0.768f, 5.3e-002f, 0.8f, 0, 0, 0, 0 }, "button_press", "<unit_base>", "officer" 
 , -1, 59, -3, 2, 0, 1, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE 
 , SQSH_TECHNIC_ID, 3, 686, 694, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 1.e-003f, 0.934f, 5.3e-002f, 0.966f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -4, -3, 5, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "resource\\icons\\intf\\all.tga", 1.e-003f, 0.801f, 5.3e-002f, 0.833f 
 , 0, 0, 0, 0 }, "button_press", "<unit_base>", "technic", -1, 59, -3, 2, -1, 1, "" 
 , 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_YADRO_ID, 10, 150, 623 
 , 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.642f, 0.866f, 0.701f, 0.909f 
 , 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 6.9e-002f, 0.679f, 0.134f, 0.73f 
 , -2, -2, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.322f, 0.733f, 0.38f, 0.777f, 0, 0, 0, 0 }, "button_press", "<Yadro>", "core", -1 
 , 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_RELAY_ID 
 , 10, 210, 624, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.702f, 0.866f 
 , 0.76f, 0.909f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 6.9e-002f, 0.679f 
 , 0.134f, 0.73f, -2, -3, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.381f, 0.733f, 0.44f, 0.777f, 0, 0, 0, 0 }, "button_press", "<Transmitter>", "" 
 , -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_YADRO_EX_ID 
 , 10, 150, 669, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.642f, 0.911f 
 , 0.701f, 0.954f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 6.9e-002f, 0.679f 
 , 0.134f, 0.73f, -2, -2, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.322f, 0.778f, 0.38f, 0.822f, 0, 0, 0, 0 }, "button_press", "<Yadro_ex>", "advanced_core" 
 , -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_SOLDIER_PLANT_ID 
 , 10, 273, 623, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.761f, 0.866f 
 , 0.82f, 0.909f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 6.9e-002f, 0.679f 
 , 0.134f, 0.73f, -3, -2, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.441f, 0.733f, 0.499f, 0.777f, 0, 0, 0, 0 }, "button_press", "<Plant_soldier>" 
 , "soldier_factory", -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE 
 , SQSH_OFFICER_PLANT_ID, 10, 334, 623, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.821f, 0.866f, 0.879f, 0.909f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 6.9e-002f, 0.679f, 0.134f, 0.73f, -3, -2, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.5f, 0.733f, 0.559f, 0.777f, 0, 0, 0 
 , 0 }, "button_press", "<Plant_officer>", "officer_factory", -1, 58, 0, 0, 0, 0, "" 
 , 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_TECHNIC_PLANT_ID, 10 
 , 395, 623, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.88f, 0.866f, 0.939f 
 , 0.909f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 6.9e-002f, 0.679f, 0.134f 
 , 0.73f, -3, -2, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.56f, 0.733f, 0.619f, 0.777f, 0, 0, 0, 0 }, "button_press", "<Plant_technic>", "technic_factory" 
 , -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_COMMANDER_ID 
 , 10, 211, 716, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.701f, 0.956f 
 , 0.76f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 6.9e-002f, 0.679f 
 , 0.134f, 0.73f, -2, -3, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.381f, 0.823f, 0.44f, 0.867f, 0, 0, 0, 0 }, "button_press", "<CommandCenter>", "" 
 , -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_STATION1_ID 
 , 10, 273, 669, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.761f, 0.911f 
 , 0.82f, 0.954f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 6.9e-002f, 0.679f 
 , 0.134f, 0.73f, -3, -2, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.441f, 0.778f, 0.499f, 0.822f, 0, 0, 0, 0 }, "button_press", "<station_rocket>" 
 , "", -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE 
 , SQSH_STATION2_ID, 10, 334, 669, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.821f, 0.911f, 0.879f, 0.954f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 6.9e-002f, 0.679f, 0.134f, 0.73f, -3, -2, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.5f, 0.778f, 0.559f, 0.822f, 0, 0, 0 
 , 0 }, "button_press", "<station_laser>", "", -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_STATION3_ID, 10, 395, 669, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.88f, 0.911f, 0.939f, 0.954f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 6.9e-002f, 0.679f, 0.134f, 0.73f, -3, -2, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.56f, 0.778f, 0.619f, 0.822f, 0, 0 
 , 0, 0 }, "button_press", "<station_bomb>", "", -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_STATION4_ID, 10, 334, 716, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.761f, 0.956f, 0.82f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 6.9e-002f 
 , 0.679f, 0.134f, 0.73f, -3, -3, 6, 6 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.441f, 0.822f, 0.499f, 0.867f, 0, 0, 0, 0 }, "button_press", "<station_secret>" 
 , "", -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE 
 , SQSH_STATION5_ID, 10, 395, 716, 60, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.821f, 0.956f, 0.879f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 7.1e-002f 
 , 0.681f, 0.132f, 0.728f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.5f, 0.822f, 0.559f, 0.867f, 0, 0, 0, 0 }, "button_press", "<station_fly>", "" 
 , -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_GUN_LASER_ID 
 , 10, 151, 624, 78, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.774f, 0.731f 
 , 0.85f, 0.775f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.14f, 0.683f, 0.218f 
 , 0.729f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.62f, 0.731f, 0.696f, 0.775f, 0, 0, 0, 0 }, "button_press", "<gun_laser>", "", -1 
 , 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_GUN_ROCKET_ID 
 , 10, 230, 624, 78, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.851f, 0.776f 
 , 0.927f, 0.82f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.14f, 0.683f, 0.218f 
 , 0.729f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.697f, 0.776f, 0.773f, 0.82f, 0, 0, 0, 0 }, "button_press", "<gun_rocket>", "" 
 , -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_GUN_HOWITZER_ID 
 , 10, 152, 669, 78, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.774f, 0.776f 
 , 0.85f, 0.82f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.14f, 0.683f, 0.218f 
 , 0.729f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.62f, 0.776f, 0.696f, 0.82f, 0, 0, 0, 0 }, "button_press", "<gun_howitzer>", "" 
 , -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_GUN_GIMLET_ID 
 , 10, 230, 669, 78, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.774f, 0.821f 
 , 0.851f, 0.865f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.14f, 0.683f 
 , 0.218f, 0.729f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.62f, 0.821f, 0.696f, 0.865f, 0, 0, 0, 0 }, "button_press", "<gun_gimlet>", "" 
 , -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE, SQSH_GUN_BALLISTIC_ID 
 , 10, 150, 716, 78, 45, 0.f, 0.f, { "resource\\icons\\intf\\all.tga", 0.851f, 0.821f 
 , 0.927f, 0.865f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga", 0.14f, 0.683f 
 , 0.218f, 0.729f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.697f, 0.821f, 0.773f, 0.865f, 0, 0, 0, 0 }, "button_press", "<gun_ballistic>" 
 , "", -1, 58, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_ATOM_BUTTON_TYPE 
 , SQSH_SQUAD_DISINTEGRATE_ID, 3, 743, 626, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 1.e-003f, 0.967f, 5.3e-002f, 0.999f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -4, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -2, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 1.e-003f, 0.834f, 5.3e-002f, 0.866f, 0, 0, 0, 0 }, "button_press", "Transform to Basic Units" 
 , "", -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT2, 7, 796, 626, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.111f, 0.868f, 0.162f, 0.899f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -4, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -1, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 0.111f, 0.735f, 0.162f, 0.766f, 0, 0, 0, 0 }, "button_press", "<unit_derived>", "" 
 , -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT5, 5, 849, 626, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.269f, 0.868f, 0.32f, 0.899f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -3, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.269f, 0.735f, 0.32f, 0.766f, 0, 0, 0, 0 }, "button_press", "<unit_derived_inaccess>" 
 , "", -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT3, 7, 904, 626, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.163f, 0.868f, 0.214f, 0.899f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -2, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, 0, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 0.164f, 0.735f, 0.214f, 0.766f, 0, 0, 0, 0 }, "button_press", "<unit_derived>", "" 
 , -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT1, 7, 958, 626, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 5.8e-002f, 0.868f, 0.109f, 0.899f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -3, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -2, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 5.8e-002f, 0.735f, 0.109f, 0.766f, 0, 0, 0, 0 }, "button_press", "<unit_derived>" 
 , "", -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT4, 7, 742, 660, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.216f, 0.868f, 0.267f, 0.899f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -3, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -1, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 0.216f, 0.735f, 0.267f, 0.766f, 0, 0, 0, 0 }, "button_press", "<unit_derived>", "" 
 , -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT7, 7, 796, 660, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.111f, 0.901f, 0.162f, 0.933f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -4, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -3, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 0.111f, 0.768f, 0.162f, 0.8f, 0, 0, 0, 0 }, "button_press", "<unit_derived>", "" 
 , -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT10, 7, 850, 660, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.269f, 0.901f, 0.32f, 0.933f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -2, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.269f, 0.768f, 0.32f, 0.8f, 0, 0, 0, 0 }, "button_press", "<unit_derived_inaccess>" 
 , "", -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT8, 5, 904, 660, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.163f, 0.901f, 0.214f, 0.933f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -2, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -1, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 0.164f, 0.768f, 0.214f, 0.8f, 0, 0, 0, 0 }, "button_press", "<unit_derived_inaccess>" 
 , "", -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT6, 7, 958, 660, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 5.8e-002f, 0.902f, 0.109f, 0.933f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -4, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -2, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 5.8e-002f, 0.768f, 0.109f, 0.8f, 0, 0, 0, 0 }, "button_press", "<unit_derived>" 
 , "", -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT14, 5, 743, 694, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.216f, 0.934f, 0.267f, 0.966f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -3, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.216f, 0.801f, 0.267f, 0.833f, 0, 0, 0, 0 }, "button_press", "<unit_derived_inaccess>" 
 , "", -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT12, 7, 796, 694, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.111f, 0.934f, 0.162f, 0.966f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -4, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -3, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 0.111f, 0.801f, 0.162f, 0.833f, 0, 0, 0, 0 }, "button_press", "<unit_derived>", "" 
 , -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT18, 7, 850, 694, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.269f, 0.967f, 0.32f, 0.999f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -3, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.269f, 0.834f, 0.32f, 0.866f, 0, 0, 0, 0 }, "button_press", "<unit_derived>", "" 
 , -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT13, 7, 904, 694, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.163f, 0.934f, 0.214f, 0.966f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -2, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -1, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 0.164f, 0.801f, 0.214f, 0.833f, 0, 0, 0, 0 }, "button_press", "<unit_derived>", "" 
 , -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT11, 7, 958, 694, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 5.8e-002f, 0.934f, 0.109f, 0.966f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -4, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -2, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 5.8e-002f, 0.801f, 0.109f, 0.833f, 0, 0, 0, 0 }, "button_press", "<unit_derived>" 
 , "", -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT9, 5, 743, 728, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.216f, 0.901f, 0.267f, 0.933f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -3, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.216f, 0.768f, 0.267f, 0.8f, 0, 0, 0, 0 }, "button_press", "<unit_derived_inaccess>" 
 , "", -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT16, 7, 796, 728, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.111f, 0.967f, 0.162f, 0.999f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -4, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -3, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 0.111f, 0.834f, 0.162f, 0.866f, 0, 0, 0, 0 }, "button_press", "<unit_derived>", "" 
 , -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT15, 7, 849, 727, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.269f, 0.934f, 0.32f, 0.966f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -3, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -1, -2, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 0.269f, 0.801f, 0.32f, 0.833f, 0, 0, 0, 0 }, "button_press", "<unit_derived>", "" 
 , -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_LEGION_BUTTON_TYPE 
 , SQSH_SQUAD_UNIT17, 7, 904, 727, 53, 33, 0.f, 0.f, { "resource\\icons\\intf\\all.tga" 
 , 0.163f, 0.967f, 0.214f, 0.999f, 0, 0, 0, 0 }, { "resource\\icons\\intf\\all.tga" 
 , 0.f, 0.693f, 5.8e-002f, 0.732f, -2, -3, 5, 6 }, { "resource\\icons\\intf\\all.tga" 
 , 0.228f, 0.691f, 0.282f, 0.726f, -1, -1, 2, 3 }, { "resource\\icons\\intf\\all.tga" 
 , 0.164f, 0.834f, 0.214f, 0.866f, 0, 0, 0, 0 }, "button_press", "<unit_derived>", "" 
 , -1, 59, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_NOHITTEST_TYPE, SQSH_RAMKA_ID 
 , 3, 0, 570, 1024, 195, 0.f, 0.f, { "resource\\icons\\intf\\up_ram.tga", 0.f, 0.f 
 , 1.f, 0.762f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PROGRESS_ENERGY_TYPE 
 , SQSH_STATIC_ID, 3, 146, 573, 314, 8, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0 
 , 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PROGRESS_SHIELD_TYPE, SQSH_STATIC_ID 
 , 0, 470, 573, 200, 8, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0 
 , SHELL_ALIGN_CENTER, 1 }, { SQSH_PROGRESS_TERRAIN_TYPE, SQSH_STATIC_ID, 0, 10, 573 
 , 200, 8, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_PROGRESS_MUTATION_TYPE, SQSH_BAR_SQUAD1_ID, 3, 679, 573, 60, 8, 0.f 
 , 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PROGRESS_MUTATION_TYPE, SQSH_BAR_SQUAD2_ID, 3, 744, 573, 60, 8, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PROGRESS_MUTATION_TYPE 
 , SQSH_BAR_SQUAD3_ID, 3, 809, 573, 60, 8, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1 
 , 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PROGRESS_MUTATION_TYPE, SQSH_BAR_SQUAD4_ID 
 , 3, 874, 573, 60, 8, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0 
 , SHELL_ALIGN_CENTER, 1 }, { SQSH_PROGRESS_MUTATION_TYPE, SQSH_BAR_SQUAD5_ID, 3, 939 
 , 573, 60, 8, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_INFOWND_TYPE, SQSH_INFOWND_ID, 2, 750, 448, 300, 120, 0.f, 0.f, { "resource\\icons\\intf\\back_info.tga" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_TEXT_WINDOW_TYPE 
 , SQSH_TUT_TEXTWINDOW_ID, 8, 50, 0, 725, 229, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "lesson_text" 
 , -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_TUT_NEXT_LESSON_ID, 8, 665, 195, 70, 35, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "next_lesson" 
 , -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_COMPLEX_PUSH_BUTTON_TYPE 
 , SQSH_TUT_RESTART_LESSON_ID, 8, 92, 195, 70, 35, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "prev_lesson" 
 , -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID 
 , 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 } } }, { SQSH_MULTITEX_WINDOW, 96, 0, 2, 0, 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, 10, { { SQSH_MOVE_BUTTON_TYPE, SQSH_MM_RAMKA, 2, 313, 0 
 , 398, 768, 313.f, -768.f, { "resource\\icons\\MainMenu\\scr_ramka.tga", 0.f, 0.f 
 , 0.77929688f, 0.75097656f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_MOVE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 2, 354, 90, 316, 64, 354.f, -678.f, { "resource\\icons\\MainMenu\\top.avi" 
 , 0.f, 0.f, 0.61914063f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } 
 , { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 2, 373, 266, 284, 67, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_up.tga" 
 , 0.f, 0.f, 0.55664063f, 0.53125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_SINGLE, 2, 384, 268, 256, 64, 0.f, 0.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "SINGLE", 128, 5, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 2, 373, 334, 284, 99, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_MULTI, 1, 384, 370, 256, 64, 0.f, 0.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "MULTI", 128, 5, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 59, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_EXIT, 2, 2, 663, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "EXIT", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 830, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_OPTIONS, 2, 772, 663, 256, 64, 0.f, 0.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "OPTIONS", 134, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_MULTITEX_WINDOW, 97, 0, 2, 0, 0 
 , 1024, 768, { "resource\\icons\\MainMenu\\t.avi", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 12, { { SQSH_MOVE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 2, 313, 0, 398, 768, 313.f, -768.f, { "resource\\icons\\MainMenu\\scr_ramka.tga" 
 , 0.f, 0.f, 0.77929688f, 0.75097656f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_MOVE_BUTTON_TYPE, SQSH_MM_RAMKA, 2, 354, 90, 316, 64, 354.f, -678.f 
 , { "resource\\icons\\MainMenu\\top.avi", 0.f, 0.f, 0.61914063f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 2 
 , 373, 168, 284, 67, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_up.tga", 0.f 
 , 0.f, 0.55664063f, 0.53125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } 
 , { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_NEW, 2, 384, 170, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "NEW", 128, 5, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 2, 373, 236, 284, 99, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_TUTORIAL, 1, 384, 270, 256, 64, 0.f, 0.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "TUTORIAL", 128, 5, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 2, 373, 335, 284, 99, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_SKIRMISH, 1, 384, 370, 256, 64, 0.f, 0.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "SKIRMISH", 128, 5, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 2, 373, 434, 284, 99, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_LOAD, 1, 384, 468, 256, 64, 0.f, 67.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "LOAD", 128, 5, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 59, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_BACK, 2, 2, 663, 256, 64, 0.f, 61.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "BACK", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 } } }, { SQSH_MULTITEX_WINDOW, 98, 0, 2, 0, 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, 5, { { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_DOLOAD, 2, 100, 256 
 , 128, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "LOAD", 64, 5 
 , SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_DELETE_LOAD, 2, 100, 512 
 , 128, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "DELETE", 64, 5 
 , SHELL_ALIGN_CENTER, 1 }, { SQSH_LIST_BOX_TYPE, SQSH_MM_LOADLIST, 2, 512, 256, 256 
 , 256, 0.f, 0.f, { "", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 59, 663, 144, 61, 0.f, 0.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_BACK, 2, 2, 663, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "BACK", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_MULTITEX_WINDOW 
 , 99, 0, 2, 0, 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, 6, { { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_DOSAVE, 2, 256, 267, 102, 0, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "SAVE", 51, 5, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_DELETE_SAVE, 1, 256, 346, 102, 0, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "DELETE", 51, 5, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_LIST_BOX_TYPE, SQSH_MM_SAVELIST, 2, 512, 256, 256, 256, 0.f, 0.f, { "" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_EDIT_BOX_TYPE 
 , SQSH_MM_SAVENAME, 2, 460, 192, 308, 23, 0.f, 0.f, { "", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1 
 , 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA 
 , 3, 59, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_BACK, 2, 2, 663, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "BACK", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 } } }, { SQSH_MULTITEX_WINDOW, 101, 0, 3, 0, 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, 12, { { SQSH_MOVE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 313, 0 
 , 398, 768, 313.f, -768.f, { "resource\\icons\\MainMenu\\scr_ramka.tga", 0.f, 0.f 
 , 0.77929688f, 0.75097656f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_MOVE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 2, 354, 90, 316, 64, 354.f, -678.f, { "resource\\icons\\MainMenu\\top.avi" 
 , 0.f, 0.f, 0.61914063f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } 
 , { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 2, 373, 168, 284, 67, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_up.tga" 
 , 0.f, 0.f, 0.55664063f, 0.53125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_OPTIONS_GAME, 2, 384, 170, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "GAME", 128, 5, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 2, 373, 236, 284, 99, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_OPTIONS_GRAPHICS, 2, 384, 270, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "GRAPHICS", 128, 5, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 2, 373, 335, 284, 99, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_OPTIONS_SOUND, 3, 384, 370, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "SOUND", 128, 5, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 2, 373, 434, 284, 99, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_OPTIONS_KEYBOARD, 3, 384, 468, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "CONTROLS", 128, 5, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 59, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_BACK, 2, 2, 663, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "BACK", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 } } }, { SQSH_MULTITEX_WINDOW, 103, 0, 3, 0, 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, 14, { { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 206 
 , 682, 57, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_up.tga", 0.f, 0.f, 0.66796875f 
 , 0.90625f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 171, 263, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 336, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 409, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_GAME_SPEED, 3, 171, 200, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "GAME SPEED", 10, 20, SHELL_ALIGN_LEFT 
 , 2 }, { SQSH_SLIDER_TYPE, SQSH_MM_GAME_SPEED_SLIDER, 3, 663, 220, 177, 32, 0.f, 0.f 
 , { "resource\\icons\\MainMenu\\slider.tga", 0.f, 0.f, 0.5859375f, 0.5f, 0, 0, 0, 0 
  }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", 141, -1, 0, 0 
 , 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_GAME_SCROLLRATE 
 , 2, 171, 273, 256, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "SCROLL RATE" 
 , 10, 20, SHELL_ALIGN_LEFT, 2 }, { SQSH_SLIDER_TYPE, SQSH_MM_GAME_SCROLLRATE_SLIDER 
 , 3, 663, 293, 177, 32, 0.f, 0.f, { "resource\\icons\\MainMenu\\slider.tga", 0.f, 0.f 
 , 0.5859375f, 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", 143, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE 
 , SQSH_MM_GAME_MOUSESPEED, 2, 171, 346, 256, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "" 
 , -1, -1, 0, 0, 0, 0, "MOUSE SPEED", 10, 20, SHELL_ALIGN_LEFT, 2 }, { SQSH_SLIDER_TYPE 
 , SQSH_MM_GAME_MOUSE_SLIDER, 3, 663, 366, 177, 32, 0.f, 0.f, { "resource\\icons\\MainMenu\\slider.tga" 
 , 0.f, 0.f, 0.5859375f, 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, "button_press", "", "", 145, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } 
 , { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_GAME_TOOLTIPS, 2, 171, 420, 256, 64, 0.f, 0.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", 148, -1, 0, 0, 0, 0, "TOOL TIPS", 10, 20, SHELL_ALIGN_LEFT, 2 }, { SQSH_COMBO_TYPE 
 , SQSH_MM_GAME_TOOLTIPS_COMBO, 3, 663, 440, 177, 32, 0.f, 0.f, { "", 0.f, 0.f, 0.5f 
 , 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "" 
 , 147, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 59, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_BACK, 2, 2, 663, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "BACK", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_MULTITEX_WINDOW 
 , 104, 0, 3, 0, 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, 26, { { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 60, 682, 57, 0.f 
 , 57.f, { "resource\\icons\\MainMenu\\frame_opt_up.tga", 0.f, 0.f, 0.66796875f, 0.90625f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1 
 , -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA 
 , 3, 171, 117, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 190, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 263, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 336, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 409, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 482, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 555, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_GRAPHICS_RESOLUTION, 2, 171, 55, 256, 64, 0.f 
 , 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "mainmenu_button", "", "", 150, -1, 0, 0, 0, 0, "RESOLUTION", 10, 20, SHELL_ALIGN_LEFT 
 , 2 }, { SQSH_COMBO_TYPE, SQSH_MM_GRAPHICS_RESOLUTION_COMBO, 3, 663, 75, 177, 32, 0.f 
 , 0.f, { "", 0.f, 0.f, 0.5f, 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", 149, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_GRAPHICS_COLORDEPTH, 2, 171, 130, 256, 64, 0.f 
 , 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "mainmenu_button", "", "", 152, -1, 0, 0, 0, 0, "COLOR DEPTH", 10, 20, SHELL_ALIGN_LEFT 
 , 2 }, { SQSH_COMBO_TYPE, SQSH_MM_GRAPHICS_COLORDEPTH_COMBO, 3, 663, 150, 177, 32 
 , 0.f, 0.f, { "", 0.f, 0.f, 0.5f, 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, "button_press", "", "", 151, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_GRAPHICS_MODE, 2, 171, 200, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", 154, -1, 0, 0, 0, 0, "MODE", 10, 20, SHELL_ALIGN_LEFT 
 , 2 }, { SQSH_COMBO_TYPE, SQSH_MM_GRAPHICS_MODE_COMBO, 3, 663, 220, 177, 32, 0.f, 0.f 
 , { "", 0.f, 0.f, 0.5f, 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, "button_press", "", "", 153, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } 
 , { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_GRAPHICS_GAMMA, 2, 171, 273, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "GAMMA ADJUST", 10, 20, SHELL_ALIGN_LEFT 
 , 2 }, { SQSH_SLIDER_TYPE, SQSH_MM_GRAPHICS_GAMMA_SLIDER, 3, 663, 293, 177, 32, 0.f 
 , 0.f, { "resource\\icons\\MainMenu\\slider.tga", 0.f, 0.f, 0.5859375f, 0.5f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", 155, -1 
 , 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_GRAPHICS_LOD 
 , 2, 171, 346, 256, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "DETAILS LEVEL" 
 , 10, 20, SHELL_ALIGN_LEFT, 2 }, { SQSH_SLIDER_TYPE, SQSH_MM_GRAPHICS_LOD_SLIDER, 3 
 , 663, 366, 177, 32, 0.f, 0.f, { "resource\\icons\\MainMenu\\slider.tga", 0.f, 0.f 
 , 0.5859375f, 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", 157, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE 
 , SQSH_MM_GRAPHICS_SHADOWS, 2, 171, 420, 256, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "" 
 , 160, -1, 0, 0, 0, 0, "SHADOW QUALITY", 10, 20, SHELL_ALIGN_LEFT, 2 }, { SQSH_COMBO_TYPE 
 , SQSH_MM_GRAPHICS_SHADOWS_COMBO, 3, 663, 440, 177, 32, 0.f, 0.f, { "", 0.f, 0.f, 0.5f 
 , 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "" 
 , 159, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE 
 , SQSH_MM_GRAPHICS_FURROWS, 2, 171, 495, 256, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "" 
 , 162, -1, 0, 0, 0, 0, "SHADOW TYPE", 10, 20, SHELL_ALIGN_LEFT, 2 }, { SQSH_COMBO_TYPE 
 , SQSH_MM_GRAPHICS_FURROWS_COMBO, 3, 663, 515, 177, 32, 0.f, 0.f, { "", 0.f, 0.f, 0.5f 
 , 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "" 
 , 161, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE 
 , SQSH_MM_GRAPHICS_REFLECTION, 2, 171, 568, 256, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "" 
 , "", 164, -1, 0, 0, 0, 0, "REFLECTIONS", 10, 20, SHELL_ALIGN_LEFT, 2 }, { SQSH_COMBO_TYPE 
 , SQSH_MM_GRAPHICS_REFLECTION_COMBO, 3, 663, 588, 177, 32, 0.f, 0.f, { "", 0.f, 0.f 
 , 0.5f, 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", 163, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 59, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_BACK, 2, 2, 663, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "BACK", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_MULTITEX_WINDOW 
 , 105, 0, 3, 0, 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, 14, { { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 206, 682, 57, 0.f 
 , 57.f, { "resource\\icons\\MainMenu\\frame_opt_up.tga", 0.f, 0.f, 0.66796875f, 0.90625f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1 
 , -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA 
 , 3, 171, 263, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 336, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 171, 409, 682, 73, 0.f, 57.f, { "resource\\icons\\MainMenu\\frame_opt_down.tga" 
 , 0.f, 0.f, 0.66796875f, 0.578125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_SOUND_SOUNDEFFECTS, 2, 171, 200, 256, 64, 0.f 
 , 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "mainmenu_button", "", "", 166, -1, 0, 0, 0, 0, "SOUND EFFECTS", 10, 20, SHELL_ALIGN_LEFT 
 , 2 }, { SQSH_COMBO_TYPE, SQSH_MM_SOUND_SOUNDEFFECTS_COMBO, 3, 663, 220, 177, 32, 0.f 
 , 0.f, { "", 0.f, 0.f, 0.5f, 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", 165, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_SOUND_SOUNDVOLUME, 2, 171, 273, 256, 64, 0.f 
 , 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "SOUND VOLUME", 10, 20, SHELL_ALIGN_LEFT 
 , 2 }, { SQSH_SLIDER_TYPE, SQSH_MM_SOUND_SOUNDVOLUME_SLIDER, 3, 663, 293, 177, 32 
 , 0.f, 0.f, { "resource\\icons\\MainMenu\\slider.tga", 0.f, 0.f, 0.5859375f, 0.5f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", 167 
 , -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_SOUND_MUSIC 
 , 2, 171, 346, 256, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", 170, -1, 0, 0, 0, 0, "MUSIC" 
 , 10, 20, SHELL_ALIGN_LEFT, 2 }, { SQSH_COMBO_TYPE, SQSH_MM_SOUND_MUSIC_COMBO, 3, 663 
 , 366, 177, 32, 0.f, 0.f, { "", 0.f, 0.f, 0.5f, 0.5f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", 169, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_SOUND_MUSICVOLUME, 2, 171, 420, 256, 64, 0.f 
 , 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "MUSIC VOLUME", 10, 20, SHELL_ALIGN_LEFT 
 , 2 }, { SQSH_SLIDER_TYPE, SQSH_MM_SOUND_MUSICVOLUME_SLIDER, 3, 663, 440, 177, 32 
 , 0.f, 0.f, { "resource\\icons\\MainMenu\\slider.tga", 0.f, 0.f, 0.5859375f, 0.5f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", 171 
 , -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA 
 , 3, 59, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_BACK, 2, 2, 663, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "BACK", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_WORLD_WINDOW_TYPE 
 , 106, 0, 2, 0, 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, 10, { { SQSH_MOVE_BUTTON_TYPE, SQSH_MM_RAMKA, 2, 313, 0, 398, 768, 313.f 
 , -768.f, { "resource\\icons\\MainMenu\\scr_ramka.tga", 0.f, 0.f, 0.77929688f, 0.75097656f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1 
 , -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_MOVE_BUTTON_TYPE, SQSH_MM_RAMKA 
 , 2, 354, 90, 316, 64, 354.f, -678.f, { "resource\\icons\\MainMenu\\top.avi", 0.f 
 , 0.f, 0.61914063f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 2, 373, 228, 284, 67, 0.f, 64.f, { "resource\\icons\\MainMenu\\scr_but_up.tga" 
 , 0.f, 0.f, 0.55664063f, 0.53125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_NEWGAME_EASY, 2, 384, 230, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "EASY", 128, 5, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 2, 373, 296, 284, 99, 0.f, 64.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_NEWGAME_NORMAL, 2, 384, 330, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "NORMAL", 128, 5, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 2, 373, 395, 284, 99, 0.f, 64.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_NEWGAME_HARD, 3, 384, 430, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "HARD", 128, 5, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 59, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_BACK, 2, 2, 663, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "BACK", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_WORLD_WINDOW_TYPE, 110, 2, 2, 0 
 , 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 3 
 , { { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA_WORLD, 3, 609, 622, 356, 102, 0.f, 102.f 
 , { "resource\\icons\\MainMenu\\world_select.tga", 0.f, 0.f, 0.69726563f, 0.796875f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1 
 , -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA 
 , 3, 59, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_BACK, 2, 2, 663, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "BACK", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 } } }, { SQSH_MULTITEX_WINDOW, 109, 0, 3, 0, 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, 7, { { SQSH_LIST_BOX_TYPE, SQSH_MM_CONTROL_LIST, 3, 56, 50 
 , 920, 550, 0.f, 0.f, { "", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 830, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_CONTROL_DEFAULTS, 3, 772, 663, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "DEFAULTS", 128, 15, SHELL_ALIGN_CENTER 
 , 2 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 444, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_CONTROL_CHANGE, 3, 386, 663, 256, 64, 0.f, 0.f 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "CHANGE", 128, 15, SHELL_ALIGN_CENTER 
 , 2 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA, 3, 59, 663, 144, 61, 0.f, 61.f, { "resource\\icons\\MainMenu\\scr_but_back.tga" 
 , 0.f, 0.f, 0.56640625f, 0.96875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_BACK, 2, 2, 663, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "BACK", 128, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_GENERAL_TYPE, 111, 0, 3, 407, 252 
 , 209, 104, { "resource\\icons\\MainMenu\\dialog_waitingkey.tga", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 0, { { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 } } }, { SQSH_GENERAL_TYPE, 112, 0, 2, 0, 0, 1024, 768, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 7, { { SQSH_GENERAL_TYPE 
 , SQSH_MM_RAMKA, 3, 188, 492, 647, 175, 0.f, 0.f, { "resource\\icons\\MainMenu\\world_ramka.tga" 
 , 0.f, 0.f, 0.63378906f, 0.6875f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_MAPWINDOW, SQSH_MM_RAMKA, 3, 683, 508, 128, 128, 0.f, 0.f, { "", 0.f, 0.f 
 , 1.0078125f, 1.0078125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_TEXT_WINDOW_TYPE 
 , SQSH_MM_TEXTWORLD, 3, 208, 499, 442, 128, 0.f, 0.f, { "", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1 
 , 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_LEFT, 1 }, { SQSH_GENERAL_TYPE, SQSH_MM_RAMKA 
 , 3, 211, 665, 68, 33, 0.f, 0.f, { "resource\\icons\\MainMenu\\start.tga", 0.f, 0.f 
 , 0.5390625f, 0.53125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_MM_RAMKA, 3, 578, 665, 68, 33, 0.f, 0.f, { "resource\\icons\\MainMenu\\start.tga" 
 , 0.f, 0.f, 0.5390625f, 0.53125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_BUTTON_TYPE, SQSH_MM_DIALOG_YES, 3, 211, 665, 68, 33, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "START", 38, 9, SHELL_ALIGN_CENTER, 3 }, { SQSH_BUTTON_TYPE 
 , SQSH_MM_DIALOG_NO, 3, 578, 665, 68, 33, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1 
 , 0, 0, 0, 0, "CANCEL", 38, 9, SHELL_ALIGN_CENTER, 3 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID 
 , 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 } } }, { SQSH_MULTITEX_WINDOW, 102, 0, 3, 0, 0, 1024, 768, { "resource\\icons\\MainMenu\\t.avi" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, 13, { { SQSH_GENERAL_TYPE, SQSH_MM_RAMKA, 3, 373, 69, 192 
 , 404, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga", 0.813f, 0.272f, 1.f, 0.666f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1 
 , 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA 
 , 3, 380, 75, 163, 56, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga", 0.653f, 0.271f 
 , 0.813f, 0.327f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE 
 , SQSH_MM_SAVE, 1, 380, 75, 163, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1 
 , 0, 0, 0, 0, "SAVE", 81, 12, SHELL_ALIGN_CENTER, 2 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA 
 , 3, 380, 140, 163, 56, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga", 0.653f, 0.271f 
 , 0.813f, 0.327f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE 
 , SQSH_MM_LOAD, 1, 380, 140, 256, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1 
 , 0, 0, 0, 0, "LOAD", 81, 12, SHELL_ALIGN_CENTER, 2 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA 
 , 3, 380, 205, 163, 56, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga", 0.653f, 0.271f 
 , 0.813f, 0.327f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE 
 , SQSH_MM_ABORT, 3, 380, 205, 256, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1 
 , 0, 0, 0, 0, "ABORT", 81, 12, SHELL_ALIGN_CENTER, 2 }, { SQSH_SCALE_BUTTON_TYPE, SQSH_MM_RAMKA 
 , 3, 380, 270, 163, 56, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga", 0.653f, 0.271f 
 , 0.813f, 0.327f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_PUSH_BUTTON_TYPE 
 , SQSH_MM_RESTART, 3, 380, 270, 256, 64, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1 
 , 0, 0, 0, 0, "RESTART", 81, 12, SHELL_ALIGN_CENTER, 2 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 380, 335, 163, 56, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga" 
 , 0.653f, 0.271f, 0.813f, 0.327f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_OPTIONS, 3, 380, 335, 256, 64, 0.f, 0.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "OPTIONS", 81, 12, SHELL_ALIGN_CENTER, 2 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 380, 400, 163, 56, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga" 
 , 0.653f, 0.271f, 0.813f, 0.327f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_RESUME, 3, 380, 400, 256, 64, 0.f, 0.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "RESUME", 81, 12, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_MULTITEX_WINDOW, 107, 0, 1, 0, 0 
 , 1024, 768, { "resource\\icons\\MainMenu\\t.avi", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 5, { { SQSH_DLG_TYPE 
 , SQSH_MM_DIALOG_RESTART, 3, 287, 256, 445, 172, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga" 
 , 0.566f, 6.4e-002f, 1.f, 0.231f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "Are you sure you\nwant to restart?" 
 , 225, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE, SQSH_MM_RAMKA, 3, 306, 370 
 , 93, 41, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga", 0.909f, 0.232f, 1.001f, 0.272f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1 
 , -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_BUTTON_TYPE, SQSH_MM_DIALOG_YES 
 , 3, 306, 370, 93, 41, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "YES" 
 , 46, 13, SHELL_ALIGN_CENTER, 3 }, { SQSH_GENERAL_TYPE, SQSH_MM_RAMKA, 3, 620, 370 
 , 93, 41, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga", 0.909f, 0.232f, 1.001f, 0.272f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1 
 , -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_BUTTON_TYPE, SQSH_MM_DIALOG_NO 
 , 3, 620, 370, 93, 41, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "NO" 
 , 46, 13, SHELL_ALIGN_CENTER, 3 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0 
 , 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_MULTITEX_WINDOW, 108, 0, 1, 0, 0 
 , 1024, 768, { "resource\\icons\\MainMenu\\t.avi", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 5, { { SQSH_DLG_TYPE 
 , SQSH_MM_DIALOG_ABORT, 3, 287, 256, 445, 172, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga" 
 , 0.566f, 6.4e-002f, 1.f, 0.231f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "Are you sure you\nwant to quit?" 
 , 225, 15, SHELL_ALIGN_CENTER, 2 }, { SQSH_GENERAL_TYPE, SQSH_MM_RAMKA, 3, 306, 370 
 , 93, 41, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga", 0.909f, 0.232f, 1.001f, 0.272f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1 
 , -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_BUTTON_TYPE, SQSH_MM_DIALOG_YES 
 , 3, 306, 370, 93, 41, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "YES" 
 , 46, 13, SHELL_ALIGN_CENTER, 3 }, { SQSH_GENERAL_TYPE, SQSH_MM_RAMKA, 3, 620, 370 
 , 93, 41, 0.f, 0.f, { "RESOURCE\\Icons\\intf\\all.tga", 0.909f, 0.232f, 1.001f, 0.272f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1 
 , -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_BUTTON_TYPE, SQSH_MM_DIALOG_NO 
 , 3, 620, 370, 93, 41, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button", "", "", -1, -1, 0, 0, 0, 0, "NO" 
 , 46, 13, SHELL_ALIGN_CENTER, 3 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0 
 , 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_MULTITEX_WINDOW, 181, 0, 1, 0, 0 
 , 1024, 768, { "resource\\icons\\MainMenu\\t.avi", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 
  }, { "resource\\icons\\MainMenu\\il.tga", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 8, { { SQSH_MOVE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 2, 312, 0, 402, 768, 313.f, -768.f, { "resource\\icons\\MainMenu\\stat_ramka.tga" 
 , 0.f, 0.f, 0.78710938f, 0.75097656f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0 
 , 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_TEXT_WINDOW_TYPE, SQSH_MM_STATTEXT, 3, 356, 89, 316, 253, 0.f, 0.f, { "" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 372, 376, 284, 67, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_up.tga" 
 , 0.f, 0.f, 0.55664063f, 0.53125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_SAVE, 1, 383, 378, 256, 64, 0.f, 0.f, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f 
 , 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "SAVE", 128, 5, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 372, 442, 284, 99, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_LOAD, 1, 383, 477, 256, 64, 0.f, 67.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "LOAD", 128, 5, SHELL_ALIGN_CENTER, 1 }, { SQSH_SCALE_BUTTON_TYPE 
 , SQSH_MM_RAMKA, 3, 372, 541, 284, 99, 0.f, 67.f, { "resource\\icons\\MainMenu\\scr_but_2.tga" 
 , 0.f, 0.f, 0.55664063f, 0.78125f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0 
 , 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 
  }, { SQSH_PUSH_BUTTON_TYPE, SQSH_MM_RESUME, 3, 383, 576, 256, 64, 0.f, 0.f, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "mainmenu_button" 
 , "", "", -1, -1, 0, 0, 0, 0, "RESUME", 128, 5, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_SPLASHSCREEN 
 , 182, 2, 2, 0, 0, 1024, 1024, { "resource\\icons\\1c.tga", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 0, { { SQSH_GENERAL_TYPE, SQSH_STATIC_ID 
 , 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_SPLASHSCREEN 
 , 183, 2, 2, 0, 0, 1024, 1024, { "resource\\icons\\KD.tga", 0.f, 0.f, 1.f, 1.f, 0 
 , 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 0, { { SQSH_GENERAL_TYPE, SQSH_STATIC_ID 
 , 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 } } }, { SQSH_SPLASHSCREEN 
 , 184, 2, 2, 0, 0, 1024, 1024, { "resource\\icons\\perimeter.tga", 0.f, 0.f, 1.f, 1.f 
 , 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 0, { { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 } } }, { SQSH_SPLASHSCREEN, 185, 2, 2, 0, 0, 1024, 1024, { "resource\\icons\\out.tga" 
 , 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, 0, { { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f 
 , 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press" 
 , "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE 
 , SQSH_STATIC_ID, 3, 0, 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 } 
 , { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0 
 , 0, "", 0, 0, SHELL_ALIGN_CENTER, 1 }, { SQSH_GENERAL_TYPE, SQSH_STATIC_ID, 3, 0 
 , 0, 0, 0, 0.f, 0.f, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f, 1.f, 0, 0, 0, 0 }, { "1", 0.f, 0.f, 1.f 
 , 1.f, 0, 0, 0, 0 }, "button_press", "", "", -1, -1, 0, 0, 0, 0, "", 0, 0, SHELL_ALIGN_CENTER 
 , 1 } } } };
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
int _sqsh_cursor_count = 26;
sqshCursor _sqsh_cursors[] = { { 1 
 , 0, "resource\\cursors\\arrow.cur", 32, 32 }, { 0, 1, "resource\\cursors\\ocrug_center_in.avi" 
 , 64, 64 }, { 0, 1, "resource\\cursors\\ocrug_center_out.avi", 64, 64 }, { 1, 0, "resource\\cursors\\instalation.ani" 
 , 32, 32 }, { 1, 0, "resource\\cursors\\uninstalation.ani", 32, 32 }, { 1, 0, "resource\\cursors\\no instal.ani" 
 , 32, 32 }, { 1, 0, "resource\\cursors\\dollar.ani", 32, 32 }, { 1, 0, "resource\\cursors\\no sale.ani" 
 , 32, 32 }, { 1, 0, "resource\\cursors\\select.ani", 32, 32 }, { 1, 0, "resource\\cursors\\attack.ani" 
 , 32, 32 }, { 1, 0, "resource\\cursors\\wait.ani", 32, 32 }, { 1, 0, "resource\\cursors\\up.cur" 
 , 32, 32 }, { 1, 0, "resource\\cursors\\down.cur", 32, 32 }, { 1, 0, "resource\\cursors\\left.cur" 
 , 32, 32 }, { 1, 0, "resource\\cursors\\right.cur", 32, 32 }, { 1, 0, "resource\\cursors\\up left.cur" 
 , 32, 32 }, { 1, 0, "resource\\cursors\\up right.cur", 32, 32 }, { 1, 0, "resource\\cursors\\down left.cur" 
 , 32, 32 }, { 1, 0, "resource\\cursors\\down right.cur", 32, 32 }, { 0, 0, "resource\\cursors\\move.tga" 
 , 64, 64 }, { 0, 0, "resource\\cursors\\rotation.tga", 64, 64 }, { 0, 0, "resource\\cursors\\rotation.tga" 
 , 64, 64 }, { 1, 0, "resource\\cursors\\in frame.ani", 32, 32 }, { 0, 1, "resource\\cursors\\rov.avi" 
 , 64, 64 }, { 1, 0, "resource\\cursors\\no atack.ani", 32, 32 }, { 1, 0, "resource\\cursors\\move3.ani" 
 , 32, 32 } };
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
int shell_galogram_table_size = 8;
ShellGalogramType shell_galogram_table[] = { { "RESOURCE\\Models\\Interface\\frm.m3d" 
 , 30.f, 2.f, 6.f, 70.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 147, 595, 60, 30, 200.f 
  }, { "RESOURCE\\Models\\Interface\\build.m3d", 25.f, 3.f, 13.f, 90.f, 0.f, 0.f, 1.f 
 , 0.f, 0.f, 0.f, 1.f, 0.f, 214, 590, 60, 32, 200.f }, { "RESOURCE\\Models\\Interface\\weapon.m3d" 
 , 30.f, 2.f, 6.f, 70.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 281, 593, 60, 36, 200.f 
  }, { "RESOURCE\\Models\\Interface\\squad.m3d", 30.f, -1.f, 3.f, 90.f, 0.f, 0.f, 1.f 
 , 0.f, 0.f, 0.f, 1.f, 0.f, 680, 582, 64, 50, 200.f }, { "RESOURCE\\Models\\Interface\\squad.m3d" 
 , 30.f, -1.f, 3.f, 90.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 745, 582, 64, 50 
 , 200.f }, { "RESOURCE\\Models\\Interface\\squad.m3d", 30.f, -1.f, 3.f, 90.f, 0.f 
 , 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 810, 582, 64, 50, 200.f }, { "RESOURCE\\Models\\Interface\\squad.m3d" 
 , 30.f, -1.f, 3.f, 90.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 875, 582, 64, 50 
 , 200.f }, { "RESOURCE\\Models\\Interface\\squad.m3d", 30.f, -1.f, 3.f, 90.f, 0.f 
 , 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 940, 582, 64, 50, 200.f } };
char const* shell_galogram_texture = "RESOURCE\\EFFECT\\lightmap.tga";
float sqshProrgessBarHeight = 1.e-002f;
float sqshVScrollSX = 32.f;
float sqshVScrollThmbSY = 32.f;
float sqshCursorWorkAreaSize = 10.f;
float sqshCursorWorkAreaDx = -3.e-002f;
float sqshCursorWorkAreaDy = -1.e-002f;
float sqshDisabledR = 0.5f;
float sqshDisabledG = 0.5f;
float sqshDisabledB = 0.5f;
float sqshDisabledA = 1.f;
char const* sqshShellMainFont = "resource\\Fonts\\Aero.font";
char const* sqshShellMainFont1 = "resource\\Fonts\\Arial.font";
char const* sqshShellMainFont2 = "resource\\Fonts\\Arial.font";
char const* sqshFontPopup = "resource\\Fonts\\Aero.font";
char const* shell_main_menu_font = "resource\\Fonts\\Aero.font";
int shell_main_menu_font_size1 = 50;
int shell_main_menu_font_size2 = 30;
int shell_main_menu_font_size3 = 15;
int nPopupTextPosX = 750;
int nPopupTextPosY = 448;
int nPopupSX = 300;
int nPopupSY = 120;
char const* sPopupTexture = "RESOURCE\\Icons\\intf\\back_info.tga";
float shellCursorAddRegionSize = 64.f;
int _buttonLabelX = 40;
int _buttonLabelY = 3;
float _squad_buttons_dx = 50.f;
int _button_atom1_x = 3;
int _button_atom1_y = 0;
int _button_atom2_x = 14;
int _button_atom2_y = 0;
int _button_atom3_x = 27;
int _button_atom3_y = 0;
int _button_atom4_x = 3;
int _button_atom4_y = 22;
int _button_atom5_x = 40;
int _button_atom5_y = 22;
int _button_pict_size = 64;
int nNrjOutputX = 880;
int nNrjOutputY = 308;
int nNrjUsedX = 970;
int nNrjUsedY = 308;
float _map_window_dx = 5.f;
float _map_window_dy = 28.f;
float _map_window_sx = 128.f;
float _map_window_sy = 128.f;
int horde_draw_radius = 10;
#ifndef __XScript_struct__ProgressBar__
#define __XScript_struct__ProgressBar__
#pragma pack( push, __XScript_struct_pack__, 4 )
struct _ProgressBar {
	_V2i points[2];
};
#pragma pack( pop, __XScript_struct_pack__)
#endif  // __XScript_struct__ProgressBar__
float fButtonEffectSizeSpeed = 3.f;
float fButtonEffectAlphaSpeed = 1.5f;
int tDeselectInterval = 300;
float fWorkAreaCenterX = 0.5f;
float fWorkAreaCenterY = 0.68f;
float empty_x1 = 0.311f;
float empty_x2 = 0.453f;
float empty_y1 = 0.681f;
float empty_y2 = 0.703f;
float brig_x1 = 0.311f;
float brig_x2 = 0.453f;
float brig_y1 = 0.655f;
float brig_y2 = 0.676f;
float build_x1 = 0.311f;
float build_x2 = 0.453f;
float build_y1 = 0.627f;
float build_y2 = 0.649f;
float hover_x1 = 0.457f;
float hover_x2 = 0.531f;
float hover_y1 = 0.596f;
float hover_y2 = 0.622f;
int hover_sx = 74;
int hover_sy = 26;
_ProgressBar _bar_squad = { { { 2 
 , -23 }, { 62, -16 } } };
int sound_shield_on_delay = 1000;
char const* sMessagesPromptFile = "SCRIPTS\\messages_prompt.txt";
int nPromptPosX = 720;
int nPromptPosY = 450;
int nShowNewClusterMsgDelay = 5000;
int nPeriodMsg = 20000;
int nDefaultMsgDelayTime = 1000;
int nMsgRectX1 = 700;
int nMsgRectY1 = 450;
int nMsgRectX2 = 1010;
int nMsgRectY2 = 570;
char const* tutMessageFile = "SCRIPTS\\messages.txt";
char const* tutSoundPath = "resource\\sounds";
int tutTextWindowDx = 10;
int tutTextWindowDy = 40;
int tutMarkWindowPeriod = 1000;
int nAtomPerSqadMax = 150;
_ProgressBar _bar_load = { { { 210 
 , 630 }, { 436, 12 } } };
int progress_tex_sz = 2;
char const* progress_texture = "resource\\icons\\intf\\progress.tga";
int splash_delay_time = 30000;
int alpha_delta = 5;
int _bDebugDisplayAllIcons = 0;
char const* sRegionTextureMain = "Resource\\Effect\\Region Main.tga";
char const* sRegionTextureWall = "Resource\\Effect\\Region Down.tga";
char const* sRegionTextureAbyss = "Resource\\Effect\\Region Down.tga";
char const* sRegionTextureCircle = "Resource\\Effect\\Region.tga";
char const* sRegionTextureEnergy = "Resource\\Effect\\fx.tga";
char const* sRegionTextureSelection = "Resource\\Effect\\Region.tga";
float fBackgrTexturePeriod = 32000.f;
int nBackgrTextureR = 255;
int nBackgrTextureG = 255;
int nBackgrTextureB = 255;
int nBackgrTextureA = 255;
int nBackgrMode = 3;

#ifdef _PRM_EDIT_
struct SquadShellUIPrm_ParameterSection : ParameterSection
{
	SquadShellUIPrm_ParameterSection() : ParameterSection("SquadShellUIPrm")
{
	description = 1049805481;
	add_dependency("gameshellsq.prm", 3202852656, 29556151);
	add_dependency("SquadShellEnums.inc", 2228309520, 29537467);
	add_dependency("UnitAttribute.inc", 2661556144, 29551737);
	reserve(122);
	add(&naaa, "naaa");
	add(&aaaa, "aaaa");
	add(&_sqsh_sheet_count, "_sqsh_sheet_count");
	add(&_sqsh_sheets, "_sqsh_sheets");
	add(&squad_icon_count, "squad_icon_count");
	add(&_squad_icons_table, "_squad_icons_table");
	add(&squad_icon_size, "squad_icon_size");
	add(&_sqsh_control_count, "_sqsh_control_count");
	add(&_sqsh_controls, "_sqsh_controls");
	add(&_sqsh_cursor_count, "_sqsh_cursor_count");
	add(&_sqsh_cursors, "_sqsh_cursors");
	add(&shell_galogram_table_size, "shell_galogram_table_size");
	add(&shell_galogram_table, "shell_galogram_table");
	add(&shell_galogram_texture, "shell_galogram_texture");
	add(&sqshProrgessBarHeight, "sqshProrgessBarHeight");
	add(&sqshVScrollSX, "sqshVScrollSX");
	add(&sqshVScrollThmbSY, "sqshVScrollThmbSY");
	add(&sqshCursorWorkAreaSize, "sqshCursorWorkAreaSize");
	add(&sqshCursorWorkAreaDx, "sqshCursorWorkAreaDx");
	add(&sqshCursorWorkAreaDy, "sqshCursorWorkAreaDy");
	add(&sqshDisabledR, "sqshDisabledR");
	add(&sqshDisabledG, "sqshDisabledG");
	add(&sqshDisabledB, "sqshDisabledB");
	add(&sqshDisabledA, "sqshDisabledA");
	add(&sqshShellMainFont, "sqshShellMainFont");
	add(&sqshShellMainFont1, "sqshShellMainFont1");
	add(&sqshShellMainFont2, "sqshShellMainFont2");
	add(&sqshFontPopup, "sqshFontPopup");
	add(&shell_main_menu_font, "shell_main_menu_font");
	add(&shell_main_menu_font_size1, "shell_main_menu_font_size1");
	add(&shell_main_menu_font_size2, "shell_main_menu_font_size2");
	add(&shell_main_menu_font_size3, "shell_main_menu_font_size3");
	add(&nPopupTextPosX, "nPopupTextPosX");
	add(&nPopupTextPosY, "nPopupTextPosY");
	add(&nPopupSX, "nPopupSX");
	add(&nPopupSY, "nPopupSY");
	add(&sPopupTexture, "sPopupTexture");
	add(&shellCursorAddRegionSize, "shellCursorAddRegionSize");
	add(&_buttonLabelX, "_buttonLabelX");
	add(&_buttonLabelY, "_buttonLabelY");
	add(&_squad_buttons_dx, "_squad_buttons_dx");
	add(&_button_atom1_x, "_button_atom1_x");
	add(&_button_atom1_y, "_button_atom1_y");
	add(&_button_atom2_x, "_button_atom2_x");
	add(&_button_atom2_y, "_button_atom2_y");
	add(&_button_atom3_x, "_button_atom3_x");
	add(&_button_atom3_y, "_button_atom3_y");
	add(&_button_atom4_x, "_button_atom4_x");
	add(&_button_atom4_y, "_button_atom4_y");
	add(&_button_atom5_x, "_button_atom5_x");
	add(&_button_atom5_y, "_button_atom5_y");
	add(&_button_pict_size, "_button_pict_size");
	add(&nNrjOutputX, "nNrjOutputX");
	add(&nNrjOutputY, "nNrjOutputY");
	add(&nNrjUsedX, "nNrjUsedX");
	add(&nNrjUsedY, "nNrjUsedY");
	add(&_map_window_dx, "_map_window_dx");
	add(&_map_window_dy, "_map_window_dy");
	add(&_map_window_sx, "_map_window_sx");
	add(&_map_window_sy, "_map_window_sy");
	add(&horde_draw_radius, "horde_draw_radius");
	add(&fButtonEffectSizeSpeed, "fButtonEffectSizeSpeed");
	add(&fButtonEffectAlphaSpeed, "fButtonEffectAlphaSpeed");
	add(&tDeselectInterval, "tDeselectInterval");
	add(&fWorkAreaCenterX, "fWorkAreaCenterX");
	add(&fWorkAreaCenterY, "fWorkAreaCenterY");
	add(&empty_x1, "empty_x1");
	add(&empty_x2, "empty_x2");
	add(&empty_y1, "empty_y1");
	add(&empty_y2, "empty_y2");
	add(&brig_x1, "brig_x1");
	add(&brig_x2, "brig_x2");
	add(&brig_y1, "brig_y1");
	add(&brig_y2, "brig_y2");
	add(&build_x1, "build_x1");
	add(&build_x2, "build_x2");
	add(&build_y1, "build_y1");
	add(&build_y2, "build_y2");
	add(&hover_x1, "hover_x1");
	add(&hover_x2, "hover_x2");
	add(&hover_y1, "hover_y1");
	add(&hover_y2, "hover_y2");
	add(&hover_sx, "hover_sx");
	add(&hover_sy, "hover_sy");
	add(&_bar_squad, "_bar_squad");
	add(&sound_shield_on_delay, "sound_shield_on_delay");
	add(&sMessagesPromptFile, "sMessagesPromptFile");
	add(&nPromptPosX, "nPromptPosX");
	add(&nPromptPosY, "nPromptPosY");
	add(&nShowNewClusterMsgDelay, "nShowNewClusterMsgDelay");
	add(&nPeriodMsg, "nPeriodMsg");
	add(&nDefaultMsgDelayTime, "nDefaultMsgDelayTime");
	add(&nMsgRectX1, "nMsgRectX1");
	add(&nMsgRectY1, "nMsgRectY1");
	add(&nMsgRectX2, "nMsgRectX2");
	add(&nMsgRectY2, "nMsgRectY2");
	add(&tutMessageFile, "tutMessageFile");
	add(&tutSoundPath, "tutSoundPath");
	add(&tutTextWindowDx, "tutTextWindowDx");
	add(&tutTextWindowDy, "tutTextWindowDy");
	add(&tutMarkWindowPeriod, "tutMarkWindowPeriod");
	add(&nAtomPerSqadMax, "nAtomPerSqadMax");
	add(&_bar_load, "_bar_load");
	add(&progress_tex_sz, "progress_tex_sz");
	add(&progress_texture, "progress_texture");
	add(&splash_delay_time, "splash_delay_time");
	add(&alpha_delta, "alpha_delta");
	add(&_bDebugDisplayAllIcons, "_bDebugDisplayAllIcons");
	add(&nBaseUnitsDisplayX, "nBaseUnitsDisplayX");
	add(&nBaseUnitsDisplayY, "nBaseUnitsDisplayY");
	add(&sRegionTextureMain, "sRegionTextureMain");
	add(&sRegionTextureWall, "sRegionTextureWall");
	add(&sRegionTextureAbyss, "sRegionTextureAbyss");
	add(&sRegionTextureCircle, "sRegionTextureCircle");
	add(&sRegionTextureEnergy, "sRegionTextureEnergy");
	add(&sRegionTextureSelection, "sRegionTextureSelection");
	add(&fBackgrTexturePeriod, "fBackgrTexturePeriod");
	add(&nBackgrTextureR, "nBackgrTextureR");
	add(&nBackgrTextureG, "nBackgrTextureG");
	add(&nBackgrTextureB, "nBackgrTextureB");
	add(&nBackgrTextureA, "nBackgrTextureA");
	add(&nBackgrMode, "nBackgrMode");
	add_parameter_section(this);
}
};
static SquadShellUIPrm_ParameterSection SquadShellUIPrm_ParameterSectionObject;
#endif  //  _PRM_EDIT_
//////////////////////////////////////////////////////////////////////////////////////////////
//	XScript end: SquadShellUIPrm
//////////////////////////////////////////////////////////////////////////////////////////////
