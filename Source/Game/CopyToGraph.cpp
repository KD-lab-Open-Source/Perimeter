#include "StdAfx.h"
#include "ForceField.h"
#include "Runtime.h"
#include "terra.h"
#include "Region.h"
#include "CameraManager.h"
#include "GenericControls.h"
#include "Universe.h"
#include "Config.h"

#include "TerraInterface.inl"
class PerimeterTerraInterface:public StandartTerraInterface
{
public:
	class Column* GetColumn(int player) override {
		terUniverse* tu=universe();
		xassert(player>=0 && player<tu->Players.size());
		return &tu->Players[player]->energyRegion().getEditColumn();
	}

	void GetBorder(int player,borderCall call,void* data) override {
		terUniverse* tu=universe();
		xassert(player>=0 && player<tu->Players.size());
		tu->Players[player]->energyRegion().getBorder(call,data,true);
	}

	void LockColumn() override {
		terUniverse* tu=universe();
		tu->EnergyRegionLocker()->Lock();
	}
    
	void UnlockColumn() override {
		terUniverse* tu=universe();
		tu->EnergyRegionLocker()->Unlock();
	}

    void GetTileColor(uint8_t* texture, uint32_t pitch, int xstart, int ystart, int xend, int yend, int step) override {
        sColor4c color;
        for(int y = ystart; y < yend; y += step) {
            uint32_t * tx = reinterpret_cast<uint32_t*>(texture);
            int yy=min(max(0,y),vMap.clip_mask_y);;
            for (int x = xstart; x < xend; x += step) {
                int xx=min(max(0,x),vMap.clip_mask_x);
                color.v = vMap.getColor32(xx, yy);
                color.v |= (GetZ(xx,yy)<=vMap.hZeroPlast) ? 0xFE000000 : 0xFF000000;

                *tx = gb_RenderDevice->ConvertColor(color);
                tx++;
            }
            texture += pitch;
        }

    }
};

TerraInterface* GreateTerraInterface()
{
	return new PerimeterTerraInterface;
}

void terPlayer::burnZeroplast()
{
	MTEnter enter(lock_burn_zeroplast);
	begin_time_burn_zeroplast.push_back(clockf());
}

void terPlayer::quantZeroplast()
{
	MTEnter enter(lock_burn_zeroplast);
	float multiply=1;
	double cur_time=clockf();
	double fade=zeroplastBurn.fade_time;

	std::list<double>::iterator it;
	for(it=begin_time_burn_zeroplast.begin();it!=begin_time_burn_zeroplast.end();)
	{
		float dt=cur_time-*it;
		if(dt>fade)
		{
			it=begin_time_burn_zeroplast.erase(it);
			continue;
		}
/*
		float mul=(fade-dt)/fade;
		multiply+=mul;
/*/
		float t=dt/fade;
		multiply+=zeroplastBurn.strength*(xm::cos((t - 0.5f) * 2 * XM_PI) + 1);
/**/
		it++;
	}

	sColor4f color=zeroLayerColor_;
	color*=multiply;
	color.r=min(2.0f,color.r);
	color.g=min(2.0f,color.g);
	color.b=min(2.0f,color.b);
	color.a=min(1.0f,zeroplastBurn.reflection*
		(1+zeroplastBurn.reflection_strength*(multiply-1))
		);
	terMapPoint->SetZeroplastColor(playerID(), color);
}
