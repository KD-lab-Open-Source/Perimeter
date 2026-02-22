#include "stdafx.h"
#include "XPrmProInternal.h"
#include "Dictionary.h"
#include "Dictionary.cxx"


//-------------------------------------------
ScriptParser& ScriptParser::operator>>(SmartName& smartName)
{
	string name;
	(*this) >> name;

	smartName = dictionary_.map(smartName.parentName(), name);
	return *this;
}

