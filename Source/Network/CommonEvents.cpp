#include "NetIncludes.h"
#include "CommonEvents.h"

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
		const auto& fop=dynamic_cast<const netCommand4G_UnitCommand&>(*this);
		const auto& sop=dynamic_cast<const netCommand4G_UnitCommand&>(secop);
		return fop==sop;
	}
	else if(EventID==NETCOM_4G_ID_REGION){
		const auto& fop=dynamic_cast<const netCommand4G_Region&>(*this);
		const auto& sop=dynamic_cast<const netCommand4G_Region&>(secop);
		return fop==sop;
	}
	else if(EventID==NETCOM_4G_ID_FORCED_DEFEAT){
		const auto& fop=dynamic_cast<const netCommand4G_ForcedDefeat&>(*this);
		const auto& sop=dynamic_cast<const netCommand4G_ForcedDefeat&>(secop);
		return fop==sop;
	}
	else return false;
}
