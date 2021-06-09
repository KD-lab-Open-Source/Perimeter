#ifndef __PROCEDURMAP_H__
#define __PROCEDURMAP_H__

#include "../Util/Serialization.h"
#include "../Util/SerializationImpl.h"
#include "../Util/XPrmArchive.h"


enum PMOperatinID {
	PMO_ID_NONE,
	PMO_ID_TOOLZER,
	PMO_ID_SQUARE_TOOLZER,
	PMO_ID_GEO,
	PMO_ID_BLUR
};

template<class base4Wrap>
class WrapPMO : public base4Wrap {
public:
	WrapPMO() { this->set(); }
	WrapPMO(int r) { this->set(r); }
	WrapPMO(int x,int y,int rad,int smth,int dh,int smode,int eql, int _filterHeigh) { this->set(x,y,rad,smth,dh,smode,eql, _filterHeigh); }
	WrapPMO(int _x, int _y, int _rad, float _intensity){ this->set(_x,_y,_rad,_intensity); }
	WrapPMO(int _x, int _y, int _sx, int _sy, int _MAX_GG_ALT, int _kPowCellSize, int _kPowShiftCS4RG, int _GeonetMESH, int _NOISELEVEL_GG, int _BorderForm, bool _inverse){
        this->set(_x, _y, _sx, _sy, _MAX_GG_ALT, _kPowCellSize, _kPowShiftCS4RG, _GeonetMESH, _NOISELEVEL_GG, _BorderForm, _inverse);
	}
};

struct sBasePMOperation {
	PMOperatinID pmoID;
	unsigned short x,y;
	//sBasePMOperation(){pmoID=PMO_ID_NONE;}
	void set(){
		pmoID=PMO_ID_NONE;
	}
};

struct sToolzerPMO : public sBasePMOperation {
	short x, y, rad;
	char smth;
	short dh;
	char smode;
	short eql;
	short filterHeigh;

	void set(int _x,int _y,int _rad,int _smth,int _dh,int _smode,int _eql, int _filterHeigh){
		pmoID=PMO_ID_TOOLZER;
		x=_x; y=_y; rad=_rad;
		smth=_smth;
		dh=_dh;
		smode=_smode;
		eql=_eql;
		filterHeigh=_filterHeigh;
	}
	template<class Archive>
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(x);
		ar & WRAP_OBJECT(y);
		ar & WRAP_OBJECT(rad);
		ar & WRAP_OBJECT(smth);
		ar & WRAP_OBJECT(dh);
		ar & WRAP_OBJECT(smode);
		ar & WRAP_OBJECT(eql);
	}
};

struct sSquareToolzerPMO : public sToolzerPMO {
	void set(int _x,int _y,int _rad,int _smth,int _dh,int _smode,int _eql, int _filterHeigh){
		sToolzerPMO::set(_x,_y,_rad,_smth,_dh,_smode,_eql,_filterHeigh);
		pmoID=PMO_ID_SQUARE_TOOLZER;
	}
};

struct sGeoPMO : public sBasePMOperation {
	short x, y, sx, sy;
    short maxGGAlt;
	char powerCellSize, powerShift;
	short geoNetMesh;
	short noiseLevel;
	char borderForm;
	char inverse;
	void set(int _x, int _y, int _sx, int _sy, int _MAX_GG_ALT, int _kPowCellSize, int _kPowShiftCS4RG, int _GeonetMESH, int _NOISELEVEL_GG, int _BorderForm, bool _inverse){
		pmoID=PMO_ID_GEO;
		x=_x; y=_y; sx=_sx; sy=_sy;
		maxGGAlt=_MAX_GG_ALT;
		powerCellSize=_kPowCellSize;
		powerShift=_kPowShiftCS4RG;
		geoNetMesh=_GeonetMESH;
		noiseLevel=_NOISELEVEL_GG;
		borderForm=_BorderForm;
		inverse=_inverse;
	}
	template<class Archive>
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(x);
		ar & WRAP_OBJECT(y);
		ar & WRAP_OBJECT(sx);
		ar & WRAP_OBJECT(sy);
		ar & WRAP_OBJECT(maxGGAlt);
		ar & WRAP_OBJECT(powerCellSize);
		ar & WRAP_OBJECT(powerShift);
		ar & WRAP_OBJECT(geoNetMesh);
		ar & WRAP_OBJECT(noiseLevel);
		ar & WRAP_OBJECT(borderForm);
		ar & WRAP_OBJECT(inverse);
	}
};

struct sBlurPMO : public sBasePMOperation {
	short x, y, rad;
	double intensity;

	void set(int _x, int _y, int _rad, float _intensity){
		pmoID=PMO_ID_BLUR;
	}
	template<class Archive>
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(x);
		ar & WRAP_OBJECT(y);
		ar & WRAP_OBJECT(rad);
		ar & WRAP_OBJECT(intensity);
	}
};

class ElementPMO {
public:
	union{
		sBasePMOperation	basePMO;
		sGeoPMO				geoPMO;
		sToolzerPMO			toolzerPMO;
		sSquareToolzerPMO	squareToolzerPMO;
		sBlurPMO			blurPMO;
	};

	ElementPMO(){ basePMO.set(); }
	ElementPMO(sBasePMOperation& d){ basePMO=d; }
	ElementPMO(sToolzerPMO& d){ toolzerPMO=d; }
	ElementPMO(sSquareToolzerPMO& d){ squareToolzerPMO=d; }
	ElementPMO(sGeoPMO& d){ geoPMO=d; }
	ElementPMO(sBlurPMO& d){ blurPMO=d; }


	template<class Archive>
	void serialize(Archive& ar) {
		int tID;
		tID=basePMO.pmoID;
		ar & WRAP_OBJECT(tID);
		basePMO.pmoID=(PMOperatinID)tID;
		switch(basePMO.pmoID){
		case PMO_ID_NONE:
			break;
		case PMO_ID_TOOLZER:
			ar & WRAP_OBJECT(toolzerPMO);
			break;
		case PMO_ID_SQUARE_TOOLZER:
			ar & WRAP_OBJECT(squareToolzerPMO);
			break;
		case PMO_ID_GEO:
			ar & WRAP_OBJECT(geoPMO);
			break;
		case PMO_ID_BLUR:
			ar & WRAP_OBJECT(blurPMO);
			break;
		}
	}
};

struct ContainerPMO {
	vector<ElementPMO> container;

	ContainerPMO();
	~ContainerPMO();

	template<class Archive>
	void serialize(Archive& ar) {
		ar & WRAP_OBJECT(container);
	}
};

#endif //__PROCEDURMAP_H__
