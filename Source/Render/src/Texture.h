#pragma once

//Universal handle for diff backends
union TextureImage {
    void* ptr;
#ifdef PERIMETER_D3D9
    struct IDirect3DTexture9* d3d;
#endif
#ifdef PERIMETER_SOKOL
    struct sg_image* sg;
#endif
};

class cTexture : public cUnknownClass, public sAttribute
{	// класс с анимацией, является динамическим указателем, то есть может удалzться через Release()
	std::string		name;				// имя файла из которого загружена текстура
	short		_x,_y;				// битовый размер текстуры
	int			TimePerFrame;		// фремя проигрывания
	int			number_mipmap;
public:
	sColor4c	skin_color;
	std::vector<TextureImage> frames;

	cTexture(const char *TexName=0);
	~cTexture();
    void SetName(const char *Name);
	inline const char* GetName()const{return name.c_str();};
	int GetNumberMipMap();
	void SetNumberMipMap(int number);

	inline int GetTimePerFrame();
	inline void SetTimePerFrame(int tpf);
	inline int GetNumberFrame() {return frames.size();};
	inline int GetX() const{return _x;};
	inline int GetY() const{return _y;};
	inline int GetWidth()const{return 1<<GetX();};
	inline int GetHeight()const{return 1<<GetY();};

	void SetWidth(int xTex);
	void SetHeight(int yTex);

	inline bool IsAlpha();
	inline bool IsAlphaTest();

    TextureImage& GetFrameImage(int n) {
        return frames[n];
    }
    
	inline void New(int number);
	inline eSurfaceFormat GetFmt();

	uint8_t* LockTexture(int& Pitch);
	uint8_t* LockTexture(int& Pitch, const Vect2i& lock_min, const Vect2i& lock_size);
	void UnlockTexture();
	virtual bool IsAviScaleTexture(){return false;}
};

class cTextureAviScale : public cTexture
{
public:
	struct RECT
	{
		float top;
		float bottom;
		float left;
		float right;
		static const RECT ID;
		RECT(){}
		RECT( float l, float t, float r, float b)
		{
			top=t; bottom=b; left=l; right=r;
		}
	};
private:
	std::vector<RECT> pos;
	float count;
public:
	cTextureAviScale(const char *TexName=0):cTexture(TexName){};
	void Init(uint32_t n_count, int dx, int dy, uint32_t tx, uint32_t ty)
	{
		SetWidth(tx);
		SetHeight(ty);
		xassert(GetX()*GetY()!=0);
		const int x_count = tx/dx;
		const int y_count = ty/dy;
		xassert(x_count*y_count!=0);
		pos.resize(n_count);
		for(int i=0;i<n_count;++i)
		{
			
			pos[i].top =	(dy*(i/x_count)+1)/(float)ty;	 
			pos[i].bottom = (dy*(i/x_count+1)-1)/(float)ty;
			pos[i].left =	(dx*(i%x_count)+1)/(float)tx;	 
			pos[i].right =	(dx*(i%x_count+1)-1)/(float)tx;
		}
		count = n_count;
	}
	inline RECT& GetFramePos(float phase)
	{
		uint32_t i = xm::round(phase * count - 0.5);
		xassert(i<pos.size());
		return pos[i];
	}
	virtual bool IsAviScaleTexture(){return true;}
}; 
