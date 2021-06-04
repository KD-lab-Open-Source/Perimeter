#include "SecondGun.h"

//----------------------------------------------

struct terMinotaurLegionType : terUnitLegionary
{
	enum terMinotaurChainModeType
	{
		PERIMETER_MINOTAUR_CHAIN_MODE_MAIN = 0,
		PERIMETER_MINOTAUR_CHAIN_MODE_LEFT_MINE,
		PERIMETER_MINOTAUR_CHAIN_MODE_RIGHT_MINE
	};

	int HandFlag;
	int Transaction;
	int RelaxCount;

	int ChainEnable;
	terMinotaurChainModeType ChainMode;
	terAnimationPhaseIteratorType ChainPhase;

	class cLogicTile* LeftLogicControl;
	class cLogicTile* RightLogicControl;

	terMinotaurLegionType(const UnitTemplate& data) : terUnitLegionary(data) {};

	void WayPointStart();
	void Quant();

	void StartMainChain();
	void StartLeftChain();
	void StartRightChain();

	int MainChain();
	int LeftChain();
	int RightChain();

};

