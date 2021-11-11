#include "StdAfx.h"

#include "Universe.h"
#include "Player.h"
#include "TerrainMaster.h"
#include "IronDigger.h"

#include "IronExplosion.h"
#include "Config.h"

//--------------------------------------------------------------

const int PERIMETER_GARBAGE_MIN_HEIGH = 3;
const int PERIMETER_GARBAGE_MAX_HEIGH = 252;

//--------------------------------------------------------------

struct terMapGridReplaceOperator
{
	int PositionX,PositionY;
	terPlayer* Player;

	int MinDist;
	terTerraformGeneral* MapPoint;

	terMapGridReplaceOperator(int x, int y, terPlayer* player)
	{
		PositionX = x;
		PositionY = y;
		Player = player;

		MinDist = 0;
		MapPoint = NULL;
	}

	void operator()(terTerraformGeneral* p)
	{
		float dx,dy,d;
		
		if(p->Player == Player && p->Type > TERRAFORM_TYPE_OTHER && !(p->UnitPoint)){
			dx = p->PositionX - PositionX;
			dy = p->PositionY - PositionY;
			d = dx * dx + dy * dy;
			if(!MapPoint || MinDist > d){
				MinDist = d;
				MapPoint = p;
			}
		}
	}
};

struct terMapGridAccelerateOperator
{
	int PositionX,PositionY;
	terPlayer* Player;

	int MinDist;
	terTerraformGeneral* MapPoint;

	terMapGridAccelerateOperator(int x, int y, terPlayer* player)
	{
		PositionX = x;
		PositionY = y;
		Player = player;

		MinDist = 0;
		MapPoint = NULL;
	}

	void operator()(terTerraformGeneral* p)
	{
		float dx,dy,d;
		
		if(p->Player == Player && p->Type > TERRAFORM_TYPE_OTHER){
			dx = p->PositionX - PositionX;
			dy = p->PositionY - PositionY;
			d = dx * dx + dy * dy;
			if(!MapPoint || MinDist > d){
				MinDist = d;
				MapPoint = p;
			}
		}
	}
};

terUnitTruck::terUnitTruck(const UnitTemplate& data) : terUnitReal(data)
{
	TruckMode = TRUCK_MODE_GET;
	TruckVolume = 0;
	TrustMasterPoint = NULL;
	Terraform = NULL;

	GetTailToolPoint = new digToolzer4K;
	PutTailToolPoint = new putToolzer4K;
	GetGarbageToolPoint = new digToolzerGarbage;
	PutGarbageToolPoint = new putToolzerGarbage;

	GetGarbageToolPoint->minWHRestriction(PERIMETER_GARBAGE_MIN_HEIGH);
	PutGarbageToolPoint->maxWHRestriction(PERIMETER_GARBAGE_MAX_HEIGH);

	FireFactor = 0;
}

void terUnitTruck::Kill()
{
	terUnitReal::Kill();
	if(Terraform && Terraform->Alive()){
		Terraform->UnitPoint = 0;
		if(Terraform->Type == TERRAFORM_TYPE_GARBAGE)
			Player->TrustMap->DeleteElement(Terraform);
		Terraform = 0;
	}
}

terUnitTruck::~terUnitTruck()
{
	if(Terraform)
	{
		xassert(!Terraform->inserted());
	}

	delete GetTailToolPoint;
	delete PutTailToolPoint;
	delete GetGarbageToolPoint;
	delete PutGarbageToolPoint;
}

void terUnitTruck::setRealModel(int modelIndex, float scale)
{
	terUnitReal::setRealModel(modelIndex, scale);

	TruckPoint = avatar()->AddPhaseController("group zad");
	TruckPoint->SetChain("mount");

	FireSoundPoint = realAvatar()->findSound(SOUND_SHOT);
}

void terUnitTruck::WayPointStart()
{
	terUnitReal::WayPointStart();
	AnimationMode = 0;
}

void terUnitTruck::DestroyLink()
{
	terUnitReal::DestroyLink();

	if(TrustMasterPoint && !(TrustMasterPoint->alive()))
		TrustMasterPoint = NULL;

	if(Terraform && !Terraform->Alive())
	{
		Terraform=0;
	}
}

void terUnitTruck::AvatarQuant()
{
	terUnitReal::AvatarQuant();

	if(SightFactor < 0.001f)
		avatar()->Hide();
	else{
		avatar()->Show();
		avatar()->setPose(BodyPoint->pose());

		if(AnimationMode){
			avatar()->SetChain("work");
			realAvatar()->setPhase(ObjectAnimation.phase());
		}else{
			avatar()->SetChain("main");
			realAvatar()->setPhase(0);
		}

		TruckPoint->setPhase((float)(TruckVolume) / (float)(IronDiggerVolume));
		if(FireSoundPoint && FireFactor > 0.0001f){
			FireSoundPoint->setVolume(FireFactor);
			FireSoundPoint->setFrequency(FireFactor);
            FireSoundPoint->play();
		}
	}
}

void terUnitTruck::Quant()
{
	terUnitReal::Quant();

	average(FireFactor, (float)AnimationMode, 0.2f);
}

int terFillGarbageTest(int x,int y,class RegionMetaDispatcher* region, terPlayer* player);
int terDigGarbageTest(int x,int y,class RegionMetaDispatcher* region, terPlayer* player);

void terUnitTruck::WayPointController()
{
	int cx,cy;
	float dx,dy;

	terUnitReal::WayPointController();

	BodyPoint->setVelocityFactor(1);
	BodyPoint->way_points.clear();

	if(Terraform){
		if(!Terraform->Alive()){
			terMapGridReplaceOperator op(xm::round(position().x), xm::round(position().y), Player);
			Player->TrustMap->TrustGrid.Scan(op.PositionX, op.PositionY, 0, op);
			Terraform->UnitPoint = NULL;
			if(op.MapPoint){
				Terraform = op.MapPoint;
				Terraform->UnitPoint = NULL;
			}else
				Terraform = NULL;
		}

		if(Terraform){
			if(Terraform->Type == TERRAFORM_TYPE_GARBAGE){
				if(TruckMode == TRUCK_MODE_GET){
					if(!terDigGarbageTest(Terraform->PositionX,Terraform->PositionY,Player->RegionPoint,Player)){
						Player->TrustMap->DeleteElement(Terraform);
						Terraform = 0;
					}
				}else{
					if(!terFillGarbageTest(Terraform->PositionX,Terraform->PositionY,Player->RegionPoint,Player)){
						Player->TrustMap->DeleteElement(Terraform);
						Terraform = 0;
					}
				}
			}else{
				if(TruckMode == TRUCK_MODE_GET){
					if(!(Terraform->Status & TERRAFORM_STATUS_DIG)){
						Terraform->UnitPoint = 0;
						Terraform = 0;
					}
				}else{
					if(!(Terraform->Status & TERRAFORM_STATUS_FILL)){
						Terraform->UnitPoint = 0;
						Terraform = 0;
					}
				}
			}
		}
	}

	int action = 0;
	if(Terraform){
		Vect3f v(Terraform->PositionX,Terraform->PositionY,0);
		int stop = 0;
		if(TruckMode == TRUCK_MODE_GET){
			if(Terraform->Type == TERRAFORM_TYPE_GARBAGE){
				if(TruckVolume < IronDiggerVolume){
					dx = position().x - Terraform->PositionX;
					dy = position().y - Terraform->PositionY;
					if((dx * dx + dy * dy) < sqr(radius())){
						BodyPoint->stopMovement();
						GetGarbageToolPoint->quant(Terraform->PositionX,Terraform->PositionY);
						TruckVolume++;
						action = 1;
						stop = 1;
					}
				}else{
					Player->TrustMap->DeleteElement(Terraform);
					Terraform = 0;
				}
			}else{
				MetaRegionLock lock(Player->RegionPoint);

				if(TruckVolume < IronDiggerVolume && Terraform->GetDigPosition(cx,cy)){
					dx = position().x - cx;
					dy = position().y - cy;
					v = Vect3f(cx,cy,0);
					if((dx * dx + dy * dy) < sqr(radius())){
						BodyPoint->stopMovement();
						GetTailToolPoint->setHAppoximation(Terraform->GetHeight());
						GetTailToolPoint->quant(cx,cy);
						TruckVolume++;
						action = 1;
					}
				}else{
					Terraform->UnitPoint = 0;
					Terraform = 0;
				}
			}
		}else{
			if(Terraform->Type == TERRAFORM_TYPE_GARBAGE){
				if(TruckVolume > 0){
					dx = position().x - Terraform->PositionX;
					dy = position().y - Terraform->PositionY;
					if((dx * dx + dy * dy) < sqr(radius())){
						BodyPoint->stopMovement();
						PutGarbageToolPoint->quant(Terraform->PositionX,Terraform->PositionY);
						TruckVolume--;
						action = 1;
						stop = 1;
					}
				}else{
					Player->TrustMap->DeleteElement(Terraform);
					Terraform = 0;
				}
			}else{
				if(TruckVolume > 0 && Terraform->GetFillPosition(cx,cy)){
					dx = position().x - cx;
					dy = position().y - cy;
					v = Vect3f(cx,cy,0);
					if((dx * dx + dy * dy) < sqr(radius())){
						BodyPoint->stopMovement();
						PutTailToolPoint->setHAppoximation(Terraform->GetHeight());
						PutTailToolPoint->quant(cx,cy);
						TruckVolume--;
						action = 1;
					}
				}else{
					Terraform->UnitPoint = 0;
					Terraform = 0;
				}
			}
		}
		if(!stop)
			BodyPoint->way_points.push_back(v);
	}else{
		if(TruckMode == TRUCK_MODE_BACK){
			if(TruckVolume){
				if(!FindFillTarget() && !FindFillGarbage()){
					if(TrustMasterPoint)
						BodyPoint->way_points.push_back(TrustMasterPoint->position());
					PutGarbageToolPoint->quant(xm::round(position().x), xm::round(position().y));
					TruckVolume--;
				}
			}else{
				if(TrustMasterPoint){
					if(TrustMasterPoint->position2D().distance2(position2D()) < sqr(radius() + TrustMasterPoint->radius())){
						TrustMasterPoint->returnTruck();
						Kill();
					}
					else
						BodyPoint->way_points.push_back(TrustMasterPoint->position());
				}else
					Kill();
			}
		}else{
			if(TruckMode == TRUCK_MODE_PUT){
				if(TruckVolume > 0){
					if(!FindFillTarget() && !FindFillGarbage())
						TruckMode = TRUCK_MODE_BACK;
				}else{
					if(FindDigTarget())
						TruckMode = TRUCK_MODE_GET;
					else{
						if(TrustMasterPoint){
							MetaRegionLock lock(Player->RegionPoint);
							Region* region = Player->ZeroRegionPoint->locateEdit(TrustMasterPoint->position2D());
							if(region && region->positive()){
								if(Player->TrustMap->FindNearFiller(TrustMasterPoint->position2D().xi(),TrustMasterPoint->position2D().yi(),region->ID())){
									FindDigGarbage();
									TruckMode = TRUCK_MODE_GET;
								}else
									TruckMode = TRUCK_MODE_BACK;
							}else{
								if(Player->TrustMap->FindNearFiller(TrustMasterPoint->position2D().xi(),TrustMasterPoint->position2D().yi())){
									FindDigGarbage();
									TruckMode = TRUCK_MODE_GET;
								}else
									TruckMode = TRUCK_MODE_BACK;
							}
						}else
							TruckMode = TRUCK_MODE_BACK;
					}
				}
			}else{
				if(TruckVolume < IronDiggerVolume){
					if(!FindDigTarget()){
						if(TruckVolume > 0){
							if(FindFillTarget())
								TruckMode = TRUCK_MODE_PUT;
							else
								TruckMode = TRUCK_MODE_BACK;
						}else{
							if(!FindDigGarbage())
								TruckMode = TRUCK_MODE_BACK;
						}
					}
				}else{
					if(!FindFillTarget() && !FindFillGarbage())
						TruckMode = TRUCK_MODE_BACK;
					TruckMode = TRUCK_MODE_PUT;
				}
			}
		}
	}

	if(!action){
		terMapGridAccelerateOperator acc_op(xm::round(position().x), xm::round(position().y), Player);
		Player->TrustMap->TrustGrid.Scan(acc_op.PositionX, acc_op.PositionY, 0, acc_op);
		if(acc_op.MapPoint){
			if(TruckVolume < IronDiggerVolume && acc_op.MapPoint->CheckDigPosition(acc_op.PositionX,acc_op.PositionY)){
				GetTailToolPoint->setHAppoximation(acc_op.MapPoint->GetHeight());
				GetTailToolPoint->quant(acc_op.PositionX,acc_op.PositionY);

				TruckVolume++;
				action = 1;
			}else{
				if(TruckVolume > 0 && acc_op.MapPoint->CheckFillPosition(acc_op.PositionX,acc_op.PositionY)){
					PutTailToolPoint->setHAppoximation(acc_op.MapPoint->GetHeight());
					PutTailToolPoint->quant(acc_op.PositionX,acc_op.PositionY);

					TruckVolume--;
					action = 1;
				}
			}
		}
	}

	if(action){
		if(!AnimationMode){
			ObjectAnimation.startPhase(0,0.1f,1);
			AnimationMode = 1;
		}else
			ObjectAnimation.Quant();
	}else{
		if(AnimationMode){
			if(ObjectAnimation.Quant())
				AnimationMode = 0;
		}
	}
}

int terDigScanGarbage(int cx,int cy,int sx,int sy,int& x,int& y,class RegionMetaDispatcher* region, terPlayer* player);
int terFillScanGarbage(int cx,int cy,int sx,int sy,int& x,int& y,class RegionMetaDispatcher* region, terPlayer* player);

int terUnitTruck::FindDigGarbage()
{
	MetaRegionLock lock(Player->RegionPoint);
	int x = xm::round(position().x);
	int y = xm::round(position().y);
	for(int i = 0;i < vMap.H_SIZE / 2;i += TRUST_MAP_GARBAGE_SIZE){
		int cx,cy;
		if(terDigScanGarbage(x,y,i,i,cx,cy,Player->RegionPoint,Player)){
			Terraform = Player->TrustMap->AddElement(TERRAFORM_TYPE_GARBAGE, cx, cy, 0);
			Terraform->UnitPoint = this;
			return 1;
		}
	}
	return 0;
}

int terUnitTruck::FindFillGarbage()
{
	MetaRegionLock lock(Player->RegionPoint);

	int x = xm::round(position().x);
	int y = xm::round(position().y);
	for(int i = 0;i < vMap.H_SIZE / 2;i += TRUST_MAP_GARBAGE_SIZE){
		int cx,cy;
		if(terFillScanGarbage(x,y,i,i,cx,cy,Player->RegionPoint,Player)){
			Terraform = Player->TrustMap->AddElement(TERRAFORM_TYPE_GARBAGE, cx, cy, 0);
			Terraform->UnitPoint = this;

			return 1;
		}
	}

	return 0;
}

int terUnitTruck::FindFillTarget()
{
	MetaRegionLock lock(Player->RegionPoint);
	if(TrustMasterPoint){
		Region* region = Player->ZeroRegionPoint->locateEdit(TrustMasterPoint->position2D());
		if(region && region->positive()){
			Terraform = Player->TrustMap->FindNearFiller(TrustMasterPoint->position().xi(),TrustMasterPoint->position().yi(),region->ID());
			if(!Terraform)
				Terraform = Player->TrustMap->FindNearFiller(TrustMasterPoint->position2D().xi(),TrustMasterPoint->position2D().yi());
		}
		else
			Terraform = Player->TrustMap->FindNearFiller(TrustMasterPoint->position2D().xi(),TrustMasterPoint->position2D().yi());

		if(Terraform){
			Terraform->UnitPoint = this;
			return 1;
		}
	}
	return 0;
}

int terUnitTruck::FindDigTarget()
{
	MetaRegionLock lock(Player->RegionPoint);
	if(TrustMasterPoint){
		Region* region = Player->ZeroRegionPoint->locateEdit(TrustMasterPoint->position2D());
		if(region && region->positive()){
			Terraform = Player->TrustMap->FindNearDigger(TrustMasterPoint->position().xi(),TrustMasterPoint->position().yi(),region->ID());
			if(!Terraform)
				Terraform = Player->TrustMap->FindNearDigger(TrustMasterPoint->position2D().xi(),TrustMasterPoint->position2D().yi());
		}
		else 
			Terraform = Player->TrustMap->FindNearDigger(TrustMasterPoint->position2D().xi(),TrustMasterPoint->position2D().yi());

		if(Terraform){
			Terraform->UnitPoint = this;
			return 1;
		}
	}
	return 0;
}

void terUnitTruck::SoundExplosion()
{
}

//-------------------------------------------------------------

