#ifndef _LAUNCH_DATA_
#define _LAUNCH_DATA_

enum terLaunchStatusType
{
	PERIMETER_LAUNCH_STATUS_NONE = 0,
	PERIMETER_LAUNCH_STATUS_HIT = 1,
	PERIMETER_LAUNCH_STATUS_EXPLOSION_SOURCE = 2,
	PERIMETER_LAUNCH_STATUS_EXPLOSION_TARGET = 4
};

struct terLaunchDataType
{
	int SourceAttributeID,TargetAttributeID;
	Vect3f LaunchSourcePosition,LaunchTargetPosition;

	terUnitBase* SourcePoint;
	terUnitBase* TargetPoint;

	terUnitBase* SourceSquad;
	terUnitBase* TargetSquad;

	int Status;
	int FlyTime;
	float ExplosionDamage;
	Vect3f ExplosionSourcePosition,ExplosionTargetPosition;
	Vect3f ExplosionPoint;

	terLaunchDataType()
	{
		SourceAttributeID = -1;
		TargetAttributeID = -1;

		SourcePoint = TargetPoint = NULL;
		SourceSquad = TargetSquad = NULL;

		Status = PERIMETER_LAUNCH_STATUS_NONE;
		FlyTime = 0;

		LaunchSourcePosition = Vect3f::ZERO;
		LaunchTargetPosition = Vect3f::ZERO;

		ExplosionSourcePosition = Vect3f::ZERO;
		ExplosionTargetPosition = Vect3f::ZERO;
		ExplosionPoint = Vect3f::ZERO;

		ExplosionDamage = 0;
	};
};

#endif