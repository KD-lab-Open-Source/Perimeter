#pragma once

enum eAttributeTexture
{
	TEXTURE_MIPMAP_POINT	=	1<<18,		// текстурные мипмапы получены ближайшими точками
	TEXTURE_MIPMAPBLUR		=	1<<19,		// использывать блюр при мипмапинге
	TEXTURE_BLURWHITE		=	1<<20,		// использовать блюр с насильным округлением в большую сторону
	TEXTURE_RENDER16		=	1<<21,		// в текстуру происходит рендер
	TEXTURE_RENDER32		=	1<<22,
	TEXTURE_32				=	1<<24,		//Только 32 битный формат
	TEXTURE_RENDER_SHADOW_9700 =1<<25,
    TEXTURE_POOL_DEFAULT    =	1<<26,
	TEXTURE_GRAY			=   1<<27,
	TEXTURE_UVBUMP			=	1<<28,
	TEXTURE_U16V16			=	1<<29,

	TEXTURE_ALPHA_BLEND		=	MAT_ALPHA_BLEND,	//  текстура содержит альфу
	TEXTURE_ALPHA_TEST		=	MAT_ALPHA_TEST,	// текстура содержит маску в альфе
    TEXTURE_BUMP			=   MAT_BUMP,
    TEXTURE_NORMAL			=   MAT_NORMAL,
};

inline bool cTexture::IsAlpha()
{
	return GetAttribute(TEXTURE_ALPHA_BLEND);
}

inline bool cTexture::IsAlphaTest()
{
	return GetAttribute(TEXTURE_ALPHA_TEST);
}

inline int cTexture::GetTimePerFrame()
{
	return TimePerFrame;
}

inline void cTexture::SetTimePerFrame(int tpf)
{
	TimePerFrame=tpf;
}

inline void cTexture::New(int number)						
{ 
	frames.resize(number); 
	for(unsigned i=0;i<frames.size();i++) {
        frames[i].ptr = nullptr;
    }
}
inline eSurfaceFormat cTexture::GetFmt()					
{ 
	if(GetAttribute(TEXTURE_GRAY) && GetAttribute(TEXTURE_ALPHA_BLEND))
		return SURFMT_GRAYALPHA;
	if(GetAttribute(TEXTURE_RENDER_SHADOW_9700))
		return SURFMT_RENDERMAP_FLOAT;
	if(GetAttribute(TEXTURE_RENDER16))
		return SURFMT_RENDERMAP16;
	if(GetAttribute(TEXTURE_RENDER32))
		return SURFMT_RENDERMAP32;
	if(GetAttribute(TEXTURE_32))
		return GetAttribute(TEXTURE_ALPHA_BLEND|TEXTURE_ALPHA_TEST) ? SURFMT_COLORALPHA32 : SURFMT_COLOR32;
	if(GetAttribute(TEXTURE_BUMP|TEXTURE_NORMAL))
		return SURFMT_BUMP;
	if(GetAttribute(TEXTURE_UVBUMP))
		return SURFMT_UV;
	if(GetAttribute(TEXTURE_U16V16))
		return SURFMT_U16V16;

	return GetAttribute(TEXTURE_ALPHA_BLEND|TEXTURE_ALPHA_TEST) ? SURFMT_COLORALPHA : SURFMT_COLOR;
}
