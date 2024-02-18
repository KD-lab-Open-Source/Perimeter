#pragma once
class StandartTerraInterface:public TerraInterface
{
	float inv_vx_fraction = 0;
public:
	StandartTerraInterface() 
	{
		inv_vx_fraction=1.0f/float(1<<VX_FRACTION);
	}

	int SizeX() override { return vMap.H_SIZE; }
	int SizeY() override { return vMap.V_SIZE; }
	int GetZ(int x,int y) override {
		int ofs=x+vMap.H_SIZE*y;
		return ((vMap.VxDBuf[ofs] != 0) ? vMap.VxDBuf[ofs] : vMap.VxGBuf[ofs]);
	}

	float GetZf(int x,int y) override {
		int ofs=x+vMap.H_SIZE*y;
		return  vMap.SGetAlt(ofs)*inv_vx_fraction;
	}

	int GetHZeroPlast() override { return vMap.hZeroPlast; }

	int GetReductionZ(int x,int y) override { return vMap.getVoxel(x,y); }
	int GetReductionShift() override { return kmGrid; }

	class Column* GetColumn(int player) override {return NULL;}
	void GetBorder(int player,borderCall call,void* data) override {}
    void GetTileColor(uint8_t *tile, uint32_t line_size, int xstart, int ystart, int xend, int yend, int step) override {}
};
