#include "StdAfx.h"
#include "CommonEvents.h"


terEventVersion::terEventVersion(XBuffer& in)
: netCommandGeneral(EVENT_ID_VERSION)
{
	in > Version;
}

void terEventVersion::Write(XBuffer& out) const
{
	out < Version;
}

//----------------------------------------------------------
 
terEventError::terEventError(XBuffer& in)
: netCommandGeneral(EVENT_ID_ERROR)
{
	in > ErrorCode;
}

void terEventError::Write(XBuffer& out) const
{
	out < ErrorCode;
}

//----------------------------------

terEventControlServerTime::terEventControlServerTime(XBuffer& in)
: netCommandGeneral(EVENT_ID_SERVER_TIME_CONTROL)
{
	in > scale;
}
void terEventControlServerTime::Write(XBuffer& out) const
{
	out < scale;
}



/////////////////////////////////////////////////////////////////////

bool netCommandGame::operator == (const netCommandGame &secop) const 
{
	if(EventID!=secop.EventID) return false;

	if(EventID==NETCOM_4G_ID_UNIT_COMMAND){
		const netCommand4G_UnitCommand& fop=static_cast<const netCommand4G_UnitCommand&>(*this);
		const netCommand4G_UnitCommand& sop=static_cast<const netCommand4G_UnitCommand&>(secop);
		return fop==sop;
	}
	else if(EventID==NETCOM_4G_ID_REGION){
		const netCommand4G_Region& fop=static_cast<const netCommand4G_Region&>(*this);
		const netCommand4G_Region& sop=static_cast<const netCommand4G_Region&>(secop);
		return fop==sop;
	}
	else if(EventID==NETCOM_4G_ID_FORCED_DEFEAT){
		const netCommand4G_ForcedDefeat& fop=static_cast<const netCommand4G_ForcedDefeat&>(*this);
		const netCommand4G_ForcedDefeat& sop=static_cast<const netCommand4G_ForcedDefeat&>(secop);
		return fop==sop;
	}
	else return false;
}
