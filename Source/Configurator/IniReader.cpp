// IniReader.cpp: implementation of the IniReader class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Configurator.h"
#include "IniReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IniReader::IniReader()
{

}

IniReader::~IniReader()
{

}

bool IniReader::readString(LPCTSTR sectionName, LPCTSTR valueName, LPCTSTR def, 
							CString& value, LPCTSTR iniFileName)
{
	CString buffer;
	GetPrivateProfileString(sectionName, valueName, 
										def, 
										buffer.GetBufferSetLength(INI_READ_BUFFER), 
										INI_READ_BUFFER,
										iniFileName);
	buffer.ReleaseBuffer();
	value = buffer;
	return true;
}

int IniReader::readInt(LPCTSTR sectionName, LPCTSTR valueName, 
					   int def, LPCTSTR iniFileName)
{
	return GetPrivateProfileInt(sectionName, valueName, def, iniFileName);
}

int IniReader::readInt(UINT sectionId, UINT valueId, int def, LPCTSTR iniFileName)
{
	CString section;
	section.LoadString(sectionId);
	CString key;
	key.LoadString(valueId);
	return readInt(section, key, def, iniFileName);
}

bool IniReader::writeInt(LPCTSTR sectionName, LPCTSTR valueName, int value, 
				LPCTSTR iniFileName)
{
	CString strValue;
	strValue.Format(_T("%d"), value);
	return writeString(sectionName, valueName, strValue, iniFileName);
}

bool IniReader::writeInt(UINT sectionId, UINT valueId, int value, LPCTSTR iniFileName)
{
	CString section;
	section.LoadString(sectionId);
	CString key;
	key.LoadString(valueId);
	return writeInt(section, key, value, iniFileName);
}

bool IniReader::writeString(LPCTSTR sectionName, LPCTSTR valueName, 
		LPCTSTR value, LPCTSTR iniFileName)
{
	return(WritePrivateProfileString(sectionName,valueName, value, iniFileName) == TRUE);
}
