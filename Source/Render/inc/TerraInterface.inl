#pragma once
class StandartTerraInterface:public TerraInterface
{
	float inv_vx_fraction;
public:
	StandartTerraInterface()
	{
		inv_vx_fraction=1.0f/float(1<<VX_FRACTION);
	}

	int SizeX(){return vMap.H_SIZE;}
	int SizeY(){return vMap.V_SIZE;}
	int GetZ(int x,int y)
	{
		int ofs=x+vMap.H_SIZE*y;
		return ((vMap.VxDBuf[ofs] != 0) ? vMap.VxDBuf[ofs] : vMap.VxGBuf[ofs]);
	}

	float GetZf(int x,int y)
	{
		int ofs=x+vMap.H_SIZE*y;
		return  vMap.SGetAlt(ofs)*inv_vx_fraction;
	}

	int GetHZeroPlast() {return vMap.hZeroPlast;}

	int GetReductionZ(int x,int y)
	{
		return vMap.getVoxel(x,y);
	}
	int GetReductionShift() {return kmGrid;}

	virtual class Column* GetColumn(int player){return NULL;}
	virtual void GetBorder(int player,borderCall call,void* data){}

	virtual void GetTileColor(char* Texture, uint32_t pitch, int xstart, int ystart, int xend, int yend, int step)
	{
		for(int y = ystart; y < yend; y += step)
		{
            uint32_t * tx=(uint32_t*)Texture;
			int yy=min(max(0,y),vMap.clip_mask_y);;
			for (int x = xstart; x < xend; x += step)
			{
				int xx=min(max(0,x),vMap.clip_mask_x);
				uint32_t color=vMap.getColor32(xx, yy);
				if(GetZ(xx,yy)<=vMap.hZeroPlast)
					color|=0xFE000000;
				else
					color|=0xFF000000;

				*tx = color;
				tx++;
			}
			Texture += pitch;
		}
		
	}
};
