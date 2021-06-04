enum terBuildingBlockModeType
{
	BUILDING_BLOCK_MODE_FIND = 0,
	BUILDING_BLOCK_MODE_MASTER,
	BUILDING_BLOCK_MODE_WORK,
	BUILDING_BLOCK_MODE_TARGET
};

enum terBuildingBlockChainType
{
	BUILDING_BLOCK_CHAIN_MOVE = 0,
	BUILDING_BLOCK_CHAIN_WORK
};

class terUnitBuildingBlock : public terUnitReal
{
public:
	terUnitBuildingBlock(const UnitTemplate& data);

	void setDamage(const DamageData& damage,terUnitBase* p = NULL);

	void Quant();

	void Kill();

	void WayPointController();
	void WayPointStart(); 

	void DestroyLink();
	void AvatarQuant();

	void SetMasterPoint(terUnitReal* owner){ masterPoint_ = owner; };

	bool findTarget();

	int MoveChain();
	int WorkChain();

	void StartMoveChain();
	void StartWorkChain();

	terUnitBase* GetIgnoreUnit();

	void SoundExplosion();

	bool selectAble() const { return false; }

private:

	int BlockMode;

	terUnitGeneric* targetPoint_;
	terUnitReal* masterPoint_;

	terSoundController* FireSoundPoint;

	float fireFactor_;

	int ChainMode;
	terAnimationPhaseIteratorType ChainAnimation;

	float ConstructionScale;
};
