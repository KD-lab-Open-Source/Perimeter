// IniReader.h: interface for the IniReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIREADER_H__9C47D37B_A7AA_4D1C_8CD0_A3845399BB76__INCLUDED_)
#define AFX_INIREADER_H__9C47D37B_A7AA_4D1C_8CD0_A3845399BB76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IniReader  
{
	IniReader();
	virtual ~IniReader();
public:

	static bool readString(LPCTSTR sectionName, LPCTSTR valueName, LPCTSTR def, 
		CString& value, LPCTSTR iniFileName);
	static int readInt(LPCTSTR sectionName, LPCTSTR valueName, int def, 
					LPCTSTR iniFileName);
	static int readInt(UINT sectionId, UINT valueId, int def, LPCTSTR iniFileName);

	static bool writeString(LPCTSTR sectionName, LPCTSTR valueName, 
		LPCTSTR value, LPCTSTR iniFileName);

	static bool writeInt(LPCTSTR sectionName, LPCTSTR valueName, int value, 
					LPCTSTR iniFileName);
	static bool writeInt(UINT sectionId, UINT valueId, int value, LPCTSTR iniFileName);
};

#endif // !defined(AFX_INIREADER_H__9C47D37B_A7AA_4D1C_8CD0_A3845399BB76__INCLUDED_)
