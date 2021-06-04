#ifndef __PLACE_OPERATORS_H__
#define __PLACE_OPERATORS_H__

#include "ScanPoly.h"

////////////////////////////////////////////
//	Placement Scan Ops
////////////////////////////////////////////
class PlaceScanOp
{
public:
	PlaceScanOp(terUnitAttributeID attributeID, AIPlayer& aiPlayer, const PlaceOpPrm& prm, int buildDuration)
		: attributeID_(attributeID), aiPlayer_(aiPlayer),
		PlayerID(aiPlayer_.playerID()),
		invalidPosition_(true), foundPosition_(false),
		prm_(prm), position_(Vect2f::ZERO),
		buildDuration_(buildDuration)
	{
		attribute_ = aiPlayer_.unitAttribute(attributeID_);
		bound_min = attribute_->BasementMin - Vect2f(prm_.boundExtraSize, prm_.boundExtraSize);
		bound_max = attribute_->BasementMax + Vect2f(prm_.boundExtraSize, prm_.boundExtraSize);
		scan_radius = prm_.scanRadius ? prm_.scanRadius : max(bound_min.norm(), bound_max.norm());
		best_position = Vect2f::ZERO;

		if(!attribute_->ConnectionRadius){
			zeroLayerConnection_ = true;
			vector<Vect2i> points(attribute_->BasementPoints.size());
			for(int i = 0; i < points.size(); i++)
				points[i] = attribute_->BasementPoints[i];
			scanPolyByLineOp(&points[0], points.size(), shapeOp_);
		}
		else
			zeroLayerConnection_ = false;
	}

	terUnitAttributeID attributeID() const { return attributeID_; }
	const Vect2f& boundMin() const { return bound_min; }
	const Vect2f& boundMax() const { return bound_max; }

	void checkPosition(const Vect2f& pos)
	{
		if(zeroLayerConnection_){
			shapeOp_.shape().move(pos - position_);
			position_ = pos;
			if(!aiPlayer_.energyColumn().intersected(shapeOp_.shape()))
				return;
			connected_ = true;
		}
		else{
			position_ = pos;
			connected_ = false;
		}

		good_factor = 0; 
		invalidPosition_ = false;

		universe()->UnitGrid.Scan(position_.x, position_.y, scan_radius, *this);
		
		if(!invalidPosition_ && connected_){
			int work = aiPlayer_.calcWork(position_ + bound_min, structureSize());
			
			if(work >= 0){
				good_factor += work*prm_.workFactor;
				
				if(prm_.frameDistanceFactor && aiPlayer_.frame())
					good_factor += aiPlayer_.frame()->position2D().distance(position_)*prm_.frameDistanceFactor;
				
				if(prm_.enemyDistanceFactor)
					good_factor += calcNearestEnemyDistance(position_)*prm_.enemyDistanceFactor;
				
				if(prm_.filthDistanceFactor){
					terUnitBase* unit = universe()->worldPlayer()->findUnit(UNIT_ATTRIBUTE_FILTH_SPOT, position_);
					if(unit)
						good_factor += unit->position2D().distance(position_)*prm_.filthDistanceFactor;
				}
				
				if(prm_.worldBuildingDistanceFactor){
					terUnitBase* unit = universe()->worldPlayer()->findUnitByUnitClass(UNIT_CLASS_STRUCTURE | UNIT_CLASS_STRUCTURE_GUN, position_);
					if(unit)
						good_factor += unit->position2D().distance(position_)*prm_.worldBuildingDistanceFactor;
					else
						return;
				}

				if(prm_.corridorDistanceFactor){
					terUnitBase* unit = universe()->worldPlayer()->findUnitByUnitClass(UNIT_CLASS_CORRIDOR, position_);
					if(!unit)
						unit = aiPlayer_.enemyPlayer()->findUnitByUnitClass(UNIT_CLASS_CORRIDOR, position_);
					if(unit)
						good_factor += unit->position2D().distance(position_)*prm_.corridorDistanceFactor;
				}


				if(best_factor < good_factor || !foundPosition_){
					best_factor = good_factor;
					best_position = position_;
					best_connecting_building = good_connecting_building;
					foundPosition_ = true;
				}
			}
		}
	}

	bool lastCheck()
	{
		foundPosition_ = false;
		checkPosition(best_position);
		return foundPosition_;
	}

	void operator()(terUnitBase* unit2)
	{
		// Учитыаем только здания и Фрейм
		if(!unit2->isBuilding() && unit2->attr().ID != UNIT_ATTRIBUTE_FRAME)
			return;

		// Касание здания
		Vect2f pos2 = unit2->position2D();
		Vect2f p = pos2 - position_;
		Vect2f p1 = p + unit2->attr().BasementMin;
		Vect2f p2 = p + unit2->attr().BasementMax;
		if(!(p2.x < bound_min.x || p1.x > bound_max.x || p2.y < bound_min.y || p1.y > bound_max.y)){
			invalidPosition_ = true;
			return;
		}

		// Учитываем только свои юниты
		if(PlayerID != unit2->playerID())
				return;

		float distance = position_.distance(pos2);

		// Проверяем подключение Core to Frame, Core, CoreGenerator, Transmitter
		if(!zeroLayerConnection_ && unit2->attr().ConnectionRadius && (unit2->attr().ID == UNIT_ATTRIBUTE_FRAME || safe_cast<terBuilding*>(unit2)->isConnected())
		  && unit2->attr().ConnectionRadius - ai_connection_radius_tolerance - distance > 0){
			connected_ = true;
			good_connecting_building = pos2;
		}

		if(attributeID() == unit2->attr().ID)
			good_factor += prm_.sameTypeDistanceFactor*prm_.scanRadius/(distance + 1);
		
		if(prm_.connectBuildingFactor && !unit2->attr().MakeEnergy){
			if(distance < attribute_->ZeroLayerRadius + unit2->attr().BasementInscribedRadius) // позволяем свободно перемещать ядро, лишь бы оно покрывало здание
				good_factor += prm_.connectBuildingFactor;
		}

		if(prm_.escapeMakingZeroLayerFactor && unit2->attr().MakeEnergy){
		  //(unit2->attr().ID != UNIT_ATTRIBUTE_FRAME || !safe_cast<terFrame*>(unit2)->attached())){
		  //unit2->attr().ID != UNIT_ATTRIBUTE_FRAME){
			float overlap = (attribute_->ZeroLayerRadius + unit2->attr().ZeroLayerRadius)*prm_.corePlacementCompactness - distance;
			if(overlap > 0)
				good_factor += prm_.escapeMakingZeroLayerFactor*overlap;
		}
	}

	bool found(){ return foundPosition_; }
	Vect2f bestPosition(){ return best_position; }
	Vect2f bestLeftTop(){ return best_position + bound_min; }
	Vect2f structureSize(){ return bound_max - bound_min; }

	void operate(RegionDispatcher* region_disp)
	{
		//Vect2f off(ai_extra_dig_size, ai_extra_dig_size);
		//region_disp->operateByRectangle(bestLeftTop() - off, structureSize() + off*2, 1);
		aiPlayer_.checkLevelingAreaQuant();

		Vect2f center = bestLeftTop() + structureSize()/2;
		float radius = structureSize().norm()/2 + ai_extra_dig_size;
		if(radius < attribute_->ZeroLayerRadius && !prm_.minimizeDiggingRadius)
			radius = attribute_->ZeroLayerRadius;
		region_disp->operateByCircle(center, radius, 1);

		if(!prm_.minimizeDiggingRadius && !zeroLayerConnection_){
			float dist = best_position.distance(best_connecting_building);
			int n = round(dist/ai_additional_toolzer_radius);
			for(int i = 1; i < n; i++){
				float t = i*ai_additional_toolzer_radius/dist;
				region_disp->operateByCircle(best_position*t + best_connecting_building*(1.f - t), ai_additional_toolzer_radius, 1);
			}
		}
	}

	void show(const sColor4c& color)
	{
		Vect2f v1 = bestLeftTop();
		Vect2f v2 = v1 + structureSize();
		show_vector(To3D(v1),To3D(Vect2f(v1.x,v2.y)),To3D(v2),To3D(Vect2f(v2.x,v1.y)),color);
		XBuffer buf;
		buf.SetDigits(6);
		buf < getEnumNameAlt(prm_.placementStrategy) < ": " <= best_factor;
		show_text(To3D(best_position), buf, color);
	}

	float calcNearestEnemyDistance(const Vect2f& pos)
	{
		float dist, distBest = FLT_INF;
		PlayerVect::iterator pi;
		FOR_EACH(universe()->Players, pi){
			terPlayer* p = *pi;
			if(p == &aiPlayer_ || p->isWorld())
				continue;
			terUnitBase* unit = p->findUnitByUnitClass(UNIT_CLASS_STRUCTURE | UNIT_CLASS_STRUCTURE_GUN | UNIT_CLASS_FRAME, pos);
			if(unit && distBest > (dist = unit->position2D().distance2(pos)))
				distBest = dist;
		}
		return sqrt(distBest);
	}

	int buildDuration() const { return buildDuration_; }

	bool generateUnableToPlaceEvent() { return prm_.placementStrategy != PLACEMENT_STRATEGY_CORE_CATCHING; }

protected:
	AIPlayer& aiPlayer_;
	terUnitAttributeID attributeID_;
	const AttributeBase* attribute_;
	const PlaceOpPrm& prm_;
	int scan_radius;
	int PlayerID;
	Vect2f bound_min;
	Vect2f bound_max;

	float good_factor;
	Vect2f position_;
	bool invalidPosition_;
	bool connected_;
	Vect2f good_connecting_building;

	bool foundPosition_;
	float best_factor;
	Vect2f best_position;
	Vect2f best_connecting_building;

	bool zeroLayerConnection_;
	GenShapeLineOp shapeOp_;

	int buildDuration_;
};

// Размещение ядер - максимально удалять на некоторое расстояние
#endif //__PLACE_OPERATORS_H__