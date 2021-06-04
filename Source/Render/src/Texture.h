#pragma once
//interface IDirect3DTexture9;
struct IDirect3DTexture9;

class cTexture : public cUnknownClass, public sAttribute
{	// класс с анимацией, является динамическим указателем, то есть может удалzться через Release()
	string		name;				// имя файла из которого загружена текстура
	short		_x,_y;				// битовый размер текстуры
	int			TimePerFrame;		// фремя проигрывания
	int			number_mipmap;
public:
	sColor4c	skin_color;
	vector<IDirect3DTexture9*>	BitMap;

	cTexture(const char *TexName=0);
	~cTexture();
	inline const char* GetName()const{return name.c_str();};
	int GetNumberMipMap();
	void SetNumberMipMap(int number);

	inline int GetTimePerFrame();
	inline void SetTimePerFrame(int tpf);
	inline int GetNumberFrame() {return BitMap.size(); };
	inline int GetX() const{return _x;};
	inline int GetY() const{return _y;};
	inline int GetWidth()const{return 1<<GetX();};
	inline int GetHeight()const{return 1<<GetY();};

	void SetWidth(int xTex);
	void SetHeight(int yTex);

	inline bool IsAlpha();
	inline bool IsAlphaTest();

	IDirect3DTexture9*& GetDDSurface(int n);
	inline void New(int number);
	inline eSurfaceFormat GetFmt();

	BYTE* LockTexture(int& Pitch);
	BYTE* LockTexture(int& Pitch,Vect2i lock_min,Vect2i lock_size);
	void UnlockTexture();
	virtual bool IsScaleTexture(){return false;}
	virtual bool IsAviScaleTexture(){return false;}
protected:
	void SetName(const char *Name);
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
	void Init(UINT n_count, int dx, int dy, UINT tx, UINT ty)
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
		UINT i = round(phase*count -0.5);
		xassert(i<pos.size());
		return pos[i];
	}
	virtual bool IsAviScaleTexture(){return true;}
}; 
class cTextureScale:public cTexture
{
	Vect2f scale;
	Vect2f uvscale;
	Vect2f inv_size;
public:
	cTextureScale(const char *TexName=0):cTexture(TexName)
	{
		scale.set(1,1);
		uvscale.set(1,1);
		inv_size.set(1,1);
	}

	inline Vect2f GetCreateScale(){return scale;};
	inline const Vect2f& GetUVScale(){return uvscale;}
	virtual bool IsScaleTexture(){return true;}
	virtual bool IsAviScaleTexture(){return false;}

	void SetScale(Vect2f scale_,Vect2f uvscale_)
	{
		scale=scale_;
		uvscale=uvscale_;
		inv_size.x=1.0f/GetWidth();
		inv_size.y=1.0f/GetHeight();
	}

	void ConvertUV(float& u,float& v)
	{
		u*=uvscale.x;
		v*=uvscale.y;
	}

	void DXY2DUV(int dx,int dy,float& du,float& dv)
	{
		du=dx*inv_size.x;
		dv=dy*inv_size.y;
	}
};
