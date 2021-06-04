#include "stdafxTr.h"

#include "procedurMap.h"


BEGIN_ENUM_DESCRIPTOR(PMOperatinID, "PMOperatinID")
REGISTER_ENUM(PMO_ID_NONE, "ничего");
REGISTER_ENUM(PMO_ID_TOOLZER, "toolzer");
REGISTER_ENUM(PMO_ID_GEO, "geo");
END_ENUM_DESCRIPTOR(PMOperatinID)


static const char* PM_CFG_FNAME="!pm.txt";
static const char* PM_CFG_SECTION="PM";

ContainerPMO::ContainerPMO(){
	if(XStream(0).open(PM_CFG_FNAME, XS_IN)){
		XPrmIArchive(PM_CFG_FNAME) >> WRAP_NAME(*this, PM_CFG_SECTION);
	}
	else{
/*		sGeoPMO g1,g2;
		sToolzerPMO	t1,t2,t3;
		g1.set(12); g2.set(15);
		t1.set(17); t2.set(2); t3.set(1);

		cntr.push_back(g1);
		cntr.push_back(g2);
		cntr.push_back(t1);
		cntr.push_back(t2);
		cntr.push_back(t3);
*/	}
}

ContainerPMO::~ContainerPMO(){
	XPrmOArchive oa(PM_CFG_FNAME);
	oa << WRAP_NAME(*this, PM_CFG_SECTION);
}


void vrtMap::playPMOperation(void)
{
	flag_record_operation=false;
	vector<ElementPMO>::iterator p;
	for(p=containerPMO.begin(); p!=containerPMO.end(); p++){
		ElementPMO o=*p;
		switch(o.basePMO.pmoID){
		case PMO_ID_NONE:
			break;
		case PMO_ID_TOOLZER:
			deltaZone(o.toolzerPMO);
		case PMO_ID_SQUARE_TOOLZER: 
			squareDeltaZone(o.squareToolzerPMO);
			break;
		case PMO_ID_GEO:
			geoGeneration(o.geoPMO);
			break;
		case PMO_ID_BLUR:
			gaussFilter(o.blurPMO.x, o.blurPMO.y, o.blurPMO.rad, o.blurPMO.intensity);
			break;
		}
	}
	flag_record_operation=true;
}
