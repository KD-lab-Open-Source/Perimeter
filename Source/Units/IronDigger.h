
#include "TrustMap.h"

class terUnitTerrainMaster;

struct terUnitTruck : terUnitReal
{
	enum terTruckMode
	{
		TRUCK_MODE_BACK = 0,
		TRUCK_MODE_GET,
		TRUCK_MODE_PUT
	};

	terTruckMode TruckMode;
	int TruckVolume;
	terUnitTerrainMaster* TrustMasterPoint;
	digToolzer4K* GetTailToolPoint;
	putToolzer4K* PutTailToolPoint;
	digToolzerGarbage* GetGarbageToolPoint;
	putToolzerGarbage* PutGarbageToolPoint;

	ShareHandle<terTerraformGeneral> Terraform;

	int AnimationMode;
	terRealPhaseControllerType* TruckPoint;
	terAnimationPhaseIteratorType ObjectAnimation;

	terSoundController* FireSoundPoint;

	float FireFactor;

	terUnitTruck(const UnitTemplate& data);
	virtual ~terUnitTruck();

	void Kill();

	void Quant();

	void setRealModel(int modelIndex, float scale);

	void WayPointController();
	void WayPointStart(); 

	void DestroyLink();
	void AvatarQuant();

	void SetTrustMaster(terUnitTerrainMaster* owner){ TrustMasterPoint = owner; };

	int FindDigGarbage();
	int FindFillGarbage();
	int FindFillTarget();
	int FindDigTarget();

	void SoundExplosion();

	bool selectAble() const { return false; }
};

