/********************************************************************
	created:	2003/07/29
	created:	29:7:2003   12:20
	filename: 	d:\Projects\Quest\QuestEditor\MakeLinkActive.h
	file path:	d:\Projects\Quest\QuestEditor
	file base:	MakeLinkActive
	file ext:	h
	author:		Илюха
	
	purpose:	делаем указанный линк активным
*********************************************************************/
#pragma once

class TEBaseWorkMode;

class MakeLinkActive
{
public:
	MakeLinkActive(TEBaseWorkMode* pmode, 
					int linkIndex, 
					int linkOwnerIndex);
	~MakeLinkActive();
	bool operator()();
	static bool run(TEBaseWorkMode* pmode, 
					int linkIndex, 
					int linkOwnerIndex);
private:
	int linkIndex_;
	int linkOwnerIndex_;
	TEBaseWorkMode* workMode_;
};
