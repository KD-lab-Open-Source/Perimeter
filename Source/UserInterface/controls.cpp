#include "StdAfx.h"
#include "Controls.h"

ControlsConverter g_controls_converter;

void ControlsConverter::SaveControls( int ctrl, int key )
{
	char strCtrl[256],strKey[256];
	sprintf( strCtrl, "%d", ctrl );
	sprintf( strKey, "%d", key );
	IniManager("RESOURCE\\controls.ini").put("main", strCtrl, strKey);
}

void ControlsConverter::LoadKeyNameTable()
{
	for(int i = 0; i < 256; i++ )
	{
		char str[256];
		sprintf( str, "%d", i );
		KeyToName[i] = IniManager("RESOURCE\\controls.ini", false).get("key", str);
	}
	std::string strCtrl = KeyToName[VK_CONTROL] + " + ";
	std::string strShift = KeyToName[VK_SHIFT] + " + ";
	std::string strAlt = KeyToName[VK_ALT] + " + ";
	for(int i=256; i<2048; i++ )
		if(!KeyToName[i & 255].empty())
		{
			std::string& str = KeyToName[i];
			str = "";
			if( i & KBD_SHIFT && (i & 255) != VK_SHIFT) 
				str += strShift;
			if( i & KBD_CTRL && (i & 255) != VK_CONTROL) 
				str += strCtrl;
			if(i & KBD_ALT && (i & 255) != VK_ALT) 
				str += strAlt;
			str += KeyToName[i & 255];
		}
	LoadCtrlTable( "main" );
}

void ControlsConverter::LoadCtrlTable(const char *strMain)
{
	memset(KeyToCtrl, 0xff , 2048);
	for( int i=0; i<CTRL_MAX; i++ )
	{
		char str[256];
		sprintf( str, "%d", i );
		CtrlToDeclaration[i] = IniManager("RESOURCE\\controls.ini").get("declaration", str );
		int nkey = IniManager("RESOURCE\\controls.ini").getInt(strMain, str);
		if( nkey<0 || nkey>=2048 ) continue;
		KeyToCtrl[ nkey ] = i;
		CtrlToKey[i] = nkey;
	}
}

