#include <SDL.h>
#include "StdAfxRD.h"
#include "xutil.h"
#include "VertexFormat.h"
#include "files/files.h"
#include "Font.h"
#include "IRenderDevice.h"
#include "EmptyRenderDevice.h"
#include "DrawBuffer.h"
#include "RenderTracker.h"

//Per backend includes
#ifdef PERIMETER_D3D9
#include "D3DRender.h"
#endif
#ifdef PERIMETER_SOKOL
#include "sokol/SokolRender.h"
#endif

static uint32_t ColorConvertABGR(const sColor4c& c) { return CONVERT_COLOR_TO_ABGR(c.v); };

const size_t PERIMETER_RENDER_VERTEXBUF_LEN = 40960;
const size_t PERIMETER_RENDER_INDEXBUF_LEN = PERIMETER_RENDER_VERTEXBUF_LEN * 2;

void MemoryResource::AllocData(size_t _data_len) {
    if (data) {
        FreeData();
    }
    data_len = _data_len;
    if (data_len) {
        data = malloc(data_len);
    } else {
        data = nullptr;
    }
}

void MemoryResource::FreeData() {
    if (data) {
        free(data);
        data = nullptr;
    }
    burned = false;
    data_len = 0;
}

VertexBuffer::VertexBuffer() = default;

void VertexBuffer::Destroy() {
    if (gb_RenderDevice) {
        gb_RenderDevice->DeleteVertexBuffer(*this);
    }
}

IndexBuffer::IndexBuffer() = default;

void IndexBuffer::Destroy() {
    if (gb_RenderDevice) {
        gb_RenderDevice->DeleteIndexBuffer(*this);
    }
}

cInterfaceRenderDevice::cInterfaceRenderDevice() : cUnknownClass(KIND_UI_RENDERDEVICE) {
    ConvertColor = ColorConvertABGR;
    RenderSubmitEvent(RenderEvent::INIT, "Intf construct");
}

cInterfaceRenderDevice::~cInterfaceRenderDevice() {
    RenderSubmitEvent(RenderEvent::DONE, "Intf destruct");
    //Subclasses must call Done before reaching this destructor or we will have problems due to deleted virtuals
    if (gb_RenderDevice) {
        xassert_s(gb_RenderDevice == nullptr, "RenderDevice not properly cleared");
        gb_RenderDevice = nullptr;
        Done();
    }
}

int cInterfaceRenderDevice::Init(int xScr, int yScr, int mode, SDL_Window* wnd, int RefreshRateInHz) {
    sdl_window = wnd;
    gb_RenderDevice = this;
    ScreenSize.x = xScr;
    ScreenSize.y = yScr;
    ScreenHZ = RefreshRateInHz;
    RenderMode = mode;
    if (!TexLibrary) {
        TexLibrary = new cTexLibrary();
    }
    drawBuffers.resize(0xFF);

    //Get the biggest resolution we might need
    MaxScreenSize.set(0, 0);
    int displays = SDL_GetNumVideoDisplays();
    if (displays < 0) {
        SDL_PRINT_ERROR("SDL_GetNumVideoDisplays");
    } else {
        SDL_DisplayMode mode;
        for (int i = 0; i < displays; ++i) {
            mode.w = 0;
            mode.h = 0;
            if (SDL_GetDesktopDisplayMode(i, &mode) != 0) {
                SDL_PRINT_ERROR("SDL_GetDesktopDisplayMode");
            } else {
                MaxScreenSize.x = max(MaxScreenSize.x, mode.w);
                MaxScreenSize.y = max(MaxScreenSize.y, mode.h);
            }
        }
    }
    
    RenderSubmitEvent(RenderEvent::INIT, "Intf done");
    return 0;
}

int cInterfaceRenderDevice::Done() {
    VISASSERT(CurrentFont == nullptr || CurrentFont == DefaultFont);
    for (auto db : drawBuffers) {
        delete db;
    }
    drawBuffers.clear();
    if (TexLibrary) {
        TexLibrary->Free();
        delete TexLibrary;
        TexLibrary = nullptr;
    }
    if (gb_RenderDevice) {
        gb_RenderDevice = nullptr;
    }
    if (sdl_window) {
        sdl_window = nullptr;
    }
    RenderSubmitEvent(RenderEvent::DONE, "Intf done");
    return 0;
}

int cInterfaceRenderDevice::BeginScene() {
    SetNoMaterial(ALPHA_TEST);
    SetRenderState(RS_ZWRITEENABLE, 1);
    SetRenderState(RS_ZENABLE, 1);
    SetRenderState(RS_WIREFRAME, WireframeMode);
    SetRenderState(RS_ZFUNC, CMP_LESSEQUAL);
    SetRenderState(RS_ALPHA_TEST_MODE, ALPHATEST_NONE);
    SetRenderState(RS_CULLMODE, CameraCullMode=CULL_CW);
    SetRenderState(RS_WIREFRAME, WireframeMode);
    return 0;
}

int cInterfaceRenderDevice::EndScene() {
    return 0;
}

void cInterfaceRenderDevice::CreateVertexBuffer(VertexBuffer& vb, uint32_t NumberVertex, vertex_fmt_t fmt, bool dynamic) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    std::string label = "Len: " + std::to_string(NumberVertex)
                      + " Fmt: " + std::to_string(fmt)
                      + " Dyn: " + std::to_string(dynamic);
    RenderSubmitEvent(RenderEvent::CREATE_VERTEXBUF, label.c_str(), &vb);
#endif
    xassert(NumberVertex <= std::numeric_limits<indices_t>().max());

    if (vb.data) {
        return;
    }

    vb.ptr = nullptr;
    vb.dynamic = dynamic;
    vb.fmt = fmt;
    vb.VertexSize = GetSizeFromFormat(vb.fmt);
    vb.NumberVertex = NumberVertex;
    
    vb.dirty = true;
    vb.locked = false;
    vb.burned = false;
    
    vb.AllocData(vb.NumberVertex * vb.VertexSize);
}

void cInterfaceRenderDevice::CreateIndexBuffer(IndexBuffer& ib, uint32_t NumberIndices, bool dynamic) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    std::string label = "Len: " + std::to_string(NumberIndices)
                      + " Dyn: " + std::to_string(dynamic);
    RenderSubmitEvent(RenderEvent::CREATE_INDEXBUF, label.c_str(), &ib);
#endif
    if (ib.data) {
        return;
    }

    ib.ptr = nullptr;
    ib.dynamic = dynamic;
    ib.NumberIndices = NumberIndices;

    ib.dirty = true;
    ib.locked = false;
    ib.burned = false;
    
    ib.AllocData(ib.NumberIndices * sizeof(indices_t));
}

void cInterfaceRenderDevice::DeleteVertexBuffer(VertexBuffer &vb) {
    vb.FreeData();
}

void cInterfaceRenderDevice::DeleteIndexBuffer(IndexBuffer &ib) {
    ib.FreeData();
}

void* cInterfaceRenderDevice::LockVertexBuffer(VertexBuffer &vb) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::LOCK_VERTEXBUF, "", &vb);
#endif
    if (!vb.data) {
        xassert(0);
        return nullptr;
    }
    xassert(!vb.burned);
    xassert(!vb.locked);
    vb.dirty = true;
    vb.locked = true;
    return vb.data;
}

void* cInterfaceRenderDevice::LockVertexBufferRange(VertexBuffer &vb, uint32_t Start, uint32_t Amount) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    std::string label = "Idx: " + std::to_string(Start) + " Len: " + std::to_string(Amount);
    RenderSubmitEvent(RenderEvent::LOCK_VERTEXBUF, label.c_str(), &vb);
#endif
    xassert(Start + Amount <= vb.NumberVertex);
    return &static_cast<uint8_t*>(LockVertexBuffer(vb))[vb.VertexSize * Start];
}

void cInterfaceRenderDevice::UnlockVertexBuffer(VertexBuffer &vb) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::UNLOCK_VERTEXBUF, "", &vb);
#endif
    xassert(!vb.burned);
    xassert(vb.locked);
    vb.locked = false;
}


indices_t* cInterfaceRenderDevice::LockIndexBuffer(IndexBuffer &ib) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::LOCK_INDEXBUF, "", &ib);
#endif
    if (!ib.data) {
        xassert(0);
        return nullptr;
    }
    xassert(!ib.burned);
    xassert(!ib.locked);
    ib.dirty = true;
    ib.locked = true;
    return static_cast<indices_t*>(ib.data);
}

indices_t* cInterfaceRenderDevice::LockIndexBufferRange(IndexBuffer &ib, uint32_t Start, uint32_t Amount) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    std::string label = "Idx: " + std::to_string(Start) + " Len: " + std::to_string(Amount);
    RenderSubmitEvent(RenderEvent::LOCK_INDEXBUF, label.c_str(), &ib);
#endif
    xassert(Start + Amount <= ib.NumberIndices);
    return &LockIndexBuffer(ib)[Start];
}

void cInterfaceRenderDevice::UnlockIndexBuffer(IndexBuffer &ib) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::UNLOCK_INDEXBUF, "", &ib);
#endif
    xassert(!ib.burned);
    xassert(ib.locked);
    ib.locked = false;
}

void cInterfaceRenderDevice::SetTexture(uint32_t slot, cTexture* texture, float phase) {
    if (texture == nullptr || texture->GetNumberFrame() == 0) {
        SetTextureImage(slot, nullptr);
    } else {
        float nAllFrame = static_cast<float>(texture->GetNumberFrame());
        int nFrame = 1 < nAllFrame ? static_cast<int>(0.999f * min(1.0f, phase) * nAllFrame) : 0;
        VISASSERT(0<=nFrame&&nFrame<texture->GetNumberFrame());
        TextureImage* tex = texture->GetFrameImage(nFrame);
        SetTextureImage(slot, tex);
    }
}

void cInterfaceRenderDevice::SetWorldMatXf(const MatXf& matrix) {
#ifdef PERIMETER_RENDER_TRACKER_MATRIX
    RenderSubmitEvent(RenderEvent::SET_WORLD_MATRIX, "MatXf");
#endif
    Mat4f mat;
    Mat4fSetTransposedMatXf(mat, matrix);
    SetWorldMat4f(&mat);
}

DrawBuffer* cInterfaceRenderDevice::GetDrawBuffer(vertex_fmt_t fmt, ePrimitiveType primitive) {
    uint16_t key = (fmt & VERTEX_FMT_MAX) | ((primitive & 0x3) << VERTEX_FMT_BITS);
    DrawBuffer* db = nullptr;
    if (key < drawBuffers.size()) {
        db = drawBuffers[key];
    } else {
        drawBuffers.resize(key + 1);
    }
    if (!db) {
        db = new DrawBuffer();
        db->Create(PERIMETER_RENDER_VERTEXBUF_LEN, true, PERIMETER_RENDER_INDEXBUF_LEN, true, fmt, primitive);
        drawBuffers[key] = db;
    }
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
    std::string label = "fmt: " + std::to_string(fmt) + " primitive " + std::to_string(primitive);
    RenderSubmitEvent(RenderEvent::GET_DRAW_BUFFER, label.c_str(), db);
#endif
    return db;
}

void cInterfaceRenderDevice::SetActiveDrawBuffer(DrawBuffer* db) {
#ifdef PERIMETER_RENDER_TRACKER_DRAW_BUFFER_STATE
    RenderSubmitEvent(RenderEvent::SET_ACTIVE_DRAW_BUFFER, nullptr, db);
#endif
    activeDrawBuffer = db;
}

cTexture* cInterfaceRenderDevice::GetTexture(int n) {
    return TexLibrary->GetTexture(n);
};

void cInterfaceRenderDevice::SetFont(cFont *pFont)
{
	CurrentFont=pFont;
	if(!CurrentFont)
		CurrentFont=DefaultFont;
}

void cInterfaceRenderDevice::SetDefaultFont(cFont *pFont)
{
	DefaultFont=pFont;
	if(!CurrentFont)
		CurrentFont=DefaultFont;
}

inline int FromHex(char a)
{
    if(a>='0' && a<='9')
        return a-'0';
    if(a>='A' && a<='F')
        return a-'A'+10;
    if(a>='a' && a<='f')
        return a-'a'+10;
    return -1;
}

/*
	Синтаксис строки 
	string &FEAB89 string && fdsfsdgs
	&FEAB89 - меняет цвет символа
	&& - преобразуется к &
*/
void cInterfaceRenderDevice::ChangeTextColor(const char* &str,sColor4c& diffuse)
{
    while(*str=='&')
    {
        if(str[1]=='&')
        {
            str++;
            return;
        }

        uint32_t s=0;
        int i;
        for(i=1;i<=6;i++)
        {
            int a=FromHex(str[i]);
            if(a<0)return;
            s=(s<<4)+a;
        }

        diffuse.ARGB((diffuse.a<<24)|s);
        str+=i;
    }
}

void cInterfaceRenderDevice::OutTextRect(int x, int y, const char *string, int align, Vect2f& bmin, Vect2f& bmax) {
    bmin.set(x,y);
    bmax.set(x,y);
    if (!CurrentFont) {
        VISASSERT(0 && "Font not set");
        return;
    }

    cFontInternal* cf=CurrentFont->GetInternal();
    cTexture* tex = cf->GetTexture();
    if (!tex) return;
    sColor4c diffuse(0,0,0,0);
    
    float xOfs;
    float yOfs = static_cast<float>(y);
    float xSize = CurrentFont->GetScale().x*static_cast<float>(tex->GetWidth());
    float ySize = CurrentFont->GetScale().y*cf->FontHeight*static_cast<float>(tex->GetHeight());

    for (const char* str=string; 0 != *str; str++, yOfs += ySize) {
        xOfs = static_cast<float>(x);
        if (0 <= align) {
            float StringWidth = GetFontLength(str);
            xOfs -= static_cast<float>(xm::round(StringWidth * (align == 0 ? 0.5f : 1)));
        }
        for (; *str!=10; str++) {
            ChangeTextColor(str, diffuse);
            if (*str==10) break;

            uint8_t c = *str;
            if (!c) goto out;
            if (c<32) continue;
            Vect3f& size=cf->Font[c];

            bmin.x=min(xOfs,bmin.x);
            bmin.y=min(yOfs,bmin.y);
            xOfs+=xSize*size.z-1;
            bmax.x=max(xOfs,bmax.x);
            bmax.y=max(yOfs+ySize,bmax.y);
        }
    }
    
out:
    return;
}

float cInterfaceRenderDevice::GetFontLength(const char *string, size_t* count) {
    cFontInternal* cf=CurrentFont->GetInternal();
    float xOfs = 0;
    float xSize = CurrentFont->GetScale().x*static_cast<float>(CurrentFont->GetTexture()?CurrentFont->GetTexture()->GetWidth():1);
    sColor4c diffuse(0,0,0,0);
    if (count) *count = 0;

    for(const char* str=string;*str;str++) {
        ChangeTextColor(str, diffuse);
        uint8_t c = static_cast<uint8_t>(*str); 
        if (!c || c==10) break;
        if (c < 32) continue;
        xOfs += xSize * cf->Font[c].z - 1;
        if (count) *count += 1;
    }

    return xOfs;
}

float cInterfaceRenderDevice::GetCharLength(const char c)
{
    char str[2];
    str[0]=c;
    str[1]=0;
    return GetFontLength(str);
}

size_t cInterfaceRenderDevice::GetSizeFromFormat(vertex_fmt_t fmt) const {
    size_t size = sizeof(float) * 3;
    if (fmt&VERTEX_FMT_NORMAL) size += sizeof(float) * 3;
    if (fmt&VERTEX_FMT_DIFFUSE) size += sizeof(uint32_t);
    if (fmt&VERTEX_FMT_TEX1) size += sizeof(float) * 2;
    if (fmt&VERTEX_FMT_TEX2) size += sizeof(float) * 2;
#ifdef PERIMETER_D3D9
    if (fmt&VERTEX_FMT_DOT3) size += sizeof(float) * 3 * 3;
#endif

    return size;
}

uint32_t ColorByNormal(Vect3f n)
{
    uint32_t x=static_cast<uint32_t>((n.x+1)*127.5f) & 0xFF;
    uint32_t y=static_cast<uint32_t>((n.y+1)*127.5f) & 0xFF;
    uint32_t z=static_cast<uint32_t>((n.z+1)*127.5f) & 0xFF;

    return z+(x<<8)+(y<<16);
}

Vect3f NormalByColor(uint32_t d)
{
    Vect3f v;
    v.y = static_cast<float>((d>> 16) & 0xFF);
    v.x = static_cast<float>((d>> 8) & 0xFF);
    v.z = static_cast<float>((d) & 0xFF);
    v*= (1/127.5f);
    v-=Vect3f(1,1,1);
    return v;
}

void BuildMipMap(int x,int y,int bpp,int bplSrc,void *pSrc,int bplDst,void *pDst,
				 int rc,int gc,int bc,int ac,int rs,int gs,int bs,int as,int Attr)
{
	char *Src=(char*)pSrc,*Dst=(char*)pDst;
	int ofsDst=bplDst-x*bpp, ofsSrc=bplSrc-2*x*bpp;
	int rm=(1<<rc)-1,gm=(1<<gc)-1,bm=(1<<bc)-1,am=(1<<ac)-1,xm=x-1,ym=y-1;

	if(Attr&TEXTURE_BUMP)
	{
		for(int j=0;j<y;j++,Dst+=ofsDst,Src+=ofsSrc+bplSrc)
		for(int i=0,color;i<x;i++,Dst+=bpp,Src+=2*bpp)
		{
			unsigned int p00=*((unsigned int*)&Src[0]),p01=*((unsigned int*)&Src[bpp]),
				p10=*((unsigned int*)&Src[bplSrc]),p11=*((unsigned int*)&Src[bplSrc+bpp]);
			Vect3f v=NormalByColor(p00)+NormalByColor(p01)+
					 NormalByColor(p10)+NormalByColor(p11);
			v.normalize();
			color=ColorByNormal(v);
			memcpy(Dst,&color,bpp);
		}
	}else
	if((Attr&TEXTURE_MIPMAPBLUR)==0)
		for(int j=0;j<y;j++,Dst+=ofsDst,Src+=ofsSrc+bplSrc)
			for(int i=0,color;i<x;i++,Dst+=bpp,Src+=2*bpp)
			{
				if( (Attr&TEXTURE_MIPMAP_POINT)==0 )
				{ // bilinear/trilinear mipmap
					unsigned int p00=*((unsigned int*)&Src[0]),p01=*((unsigned int*)&Src[bpp]),
						p10=*((unsigned int*)&Src[bplSrc]),p11=*((unsigned int*)&Src[bplSrc+bpp]);
					int r=(((p00>>rs)&rm)+((p01>>rs)&rm)+((p10>>rs)&rm)+((p11>>rs)&rm))>>2;
					int g=(((p00>>gs)&gm)+((p01>>gs)&gm)+((p10>>gs)&gm)+((p11>>gs)&gm))>>2;
					int b=(((p00>>bs)&bm)+((p01>>bs)&bm)+((p10>>bs)&bm)+((p11>>bs)&bm))>>2;
					int a=(((p00>>as)&am)+((p01>>as)&am)+((p10>>as)&am)+((p11>>as)&am))>>2;
					color=(r<<rs)|(g<<gs)|(b<<bs)|(a<<as);
				}
				else // nearest mipmap
					color=*((unsigned int*)&Src[0]);
				memcpy(Dst,&color,bpp);
			}
	else 
	{
		unsigned char *rgba=new unsigned char [4*x*y];
		unsigned char *buf=rgba;
		int j;
		for(j=0;j<y;j++,Src+=ofsSrc+bplSrc)
			for(int i=0;i<x;i++,buf+=4,Src+=2*bpp)
			{
				unsigned int p00=*((unsigned int*)&Src[0]),p01=*((unsigned int*)&Src[bpp]),
					p10=*((unsigned int*)&Src[bplSrc]),p11=*((unsigned int*)&Src[bplSrc+bpp]);
				buf[0]=(((p00>>rs)&rm)+((p01>>rs)&rm)+((p10>>rs)&rm)+((p11>>rs)&rm))>>2;
				buf[1]=(((p00>>gs)&gm)+((p01>>gs)&gm)+((p10>>gs)&gm)+((p11>>gs)&gm))>>2;
				buf[2]=(((p00>>bs)&bm)+((p01>>bs)&bm)+((p10>>bs)&bm)+((p11>>bs)&bm))>>2;
				buf[3]=(((p00>>as)&am)+((p01>>as)&am)+((p10>>as)&am)+((p11>>as)&am))>>2;
			}
		const int	k00=4,  k01=8,	k02=8,  k03=8,  k04=4;
		const int	k10=8,  k11=16,	k12=16, k13=16, k14=8;
		const int	k20=8,  k21=16,	k22=16, k23=16, k24=8;
		const int	k30=8,  k31=16,	k32=16, k33=16, k34=8;
		const int	k40=4,  k41=8,	k42=8,  k43=8,  k44=4;
		if((Attr&TEXTURE_BLURWHITE)==0) 
			for(j=0;j<y;j++,Dst+=ofsDst)
				for(int i=0;i<x;i++,Dst+=bpp)
				{
					int iMin2=4*((i-2)&xm),iMin1=4*((i-1)&xm),iNull=4*i,iMax1=4*((i+1)&xm),iMax2=4*((i+2)&xm),
						jMin2=4*x*((j+2)&ym),jMin1=4*x*((j+1)&ym),jNull=4*x*j,jMax1=4*x*((j+1)&ym),jMax2=4*x*((j+2)&ym);
					unsigned char *p00=&rgba[iMin2+jMin2],*p01=&rgba[iMin1+jMin2],*p02=&rgba[iNull+jMin2],*p03=&rgba[iMax1+jMin2],*p04=&rgba[iMax2+jMin2],
						*p10=&rgba[iMin2+jMin1],*p11=&rgba[iMin1+jMin1],*p12=&rgba[iNull+jMin1],*p13=&rgba[iMax1+jMin1],*p14=&rgba[iMax2+jMin1],
						*p20=&rgba[iMin2+jNull],*p21=&rgba[iMin1+jNull],*p22=&rgba[iNull+jNull],*p23=&rgba[iMax1+jNull],*p24=&rgba[iMax2+jNull],
						*p30=&rgba[iMin2+jMax1],*p31=&rgba[iMin1+jMax1],*p32=&rgba[iNull+jMax1],*p33=&rgba[iMax1+jMax1],*p34=&rgba[iMax2+jMax1],
						*p40=&rgba[iMin2+jMax2],*p41=&rgba[iMin1+jMax2],*p42=&rgba[iNull+jMax2],*p43=&rgba[iMax1+jMax2],*p44=&rgba[iMax2+jMax2];
					int r =(p00[0]*k00+p01[0]*k01+p02[0]*k02+p03[0]*k03+p04[0]*k04+
							p10[0]*k10+p11[0]*k11+p12[0]*k12+p13[0]*k13+p14[0]*k14+
							p20[0]*k20+p21[0]*k21+p22[0]*k22+p23[0]*k23+p24[0]*k24+
							p30[0]*k30+p31[0]*k31+p32[0]*k32+p33[0]*k33+p34[0]*k34+
							p40[0]*k40+p41[0]*k41+p42[0]*k42+p43[0]*k43+p44[0]*k44)>>8;
					int g =(p00[1]*k00+p01[1]*k01+p02[1]*k02+p03[1]*k03+p04[1]*k04+
							p10[1]*k10+p11[1]*k11+p12[1]*k12+p13[1]*k13+p14[1]*k14+
							p20[1]*k20+p21[1]*k21+p22[1]*k22+p23[1]*k23+p24[1]*k24+
							p30[1]*k30+p31[1]*k31+p32[1]*k32+p33[1]*k33+p34[1]*k34+
							p40[1]*k40+p41[1]*k41+p42[1]*k42+p43[1]*k43+p44[1]*k44)>>8;
					int b =(p00[2]*k00+p01[2]*k01+p02[2]*k02+p03[2]*k03+p04[2]*k04+
							p10[2]*k10+p11[2]*k11+p12[2]*k12+p13[2]*k13+p14[2]*k14+
							p20[2]*k20+p21[2]*k21+p22[2]*k22+p23[2]*k23+p24[2]*k24+
							p30[2]*k30+p31[2]*k31+p32[2]*k32+p33[2]*k33+p34[2]*k34+
							p40[2]*k40+p41[2]*k41+p42[2]*k42+p43[2]*k43+p44[2]*k44)>>8;
					int a =(p00[3]*k00+p01[3]*k01+p02[3]*k02+p03[3]*k03+p04[3]*k04+
							p10[3]*k10+p11[3]*k11+p12[3]*k12+p13[3]*k13+p14[3]*k14+
							p20[3]*k20+p21[3]*k21+p22[3]*k22+p23[3]*k23+p24[3]*k24+
							p30[3]*k30+p31[3]*k31+p32[3]*k32+p33[3]*k33+p34[3]*k34+
							p40[3]*k40+p41[3]*k41+p42[3]*k42+p43[3]*k43+p44[3]*k44)>>8;
					int color=(r<<rs)|(g<<gs)|(b<<bs)|(a<<as);
					memcpy(Dst,&color,bpp);
				}
		else
			for(j=0;j<y;j++,Dst+=ofsDst)
				for(int i=0;i<x;i++,Dst+=bpp)
				{
					const int AddWhite=2*256+255;
					int iMin2=4*((i-2)&xm),iMin1=4*((i-1)&xm),iNull=4*i,iMax1=4*((i+1)&xm),iMax2=4*((i+2)&xm),
						jMin2=4*x*((j+2)&ym),jMin1=4*x*((j+1)&ym),jNull=4*x*j,jMax1=4*x*((j+1)&ym),jMax2=4*x*((j+2)&ym);
					unsigned char *p00=&rgba[iMin2+jMin2],*p01=&rgba[iMin1+jMin2],*p02=&rgba[iNull+jMin2],*p03=&rgba[iMax1+jMin2],*p04=&rgba[iMax2+jMin2],
						*p10=&rgba[iMin2+jMin1],*p11=&rgba[iMin1+jMin1],*p12=&rgba[iNull+jMin1],*p13=&rgba[iMax1+jMin1],*p14=&rgba[iMax2+jMin1],
						*p20=&rgba[iMin2+jNull],*p21=&rgba[iMin1+jNull],*p22=&rgba[iNull+jNull],*p23=&rgba[iMax1+jNull],*p24=&rgba[iMax2+jNull],
						*p30=&rgba[iMin2+jMax1],*p31=&rgba[iMin1+jMax1],*p32=&rgba[iNull+jMax1],*p33=&rgba[iMax1+jMax1],*p34=&rgba[iMax2+jMax1],
						*p40=&rgba[iMin2+jMax2],*p41=&rgba[iMin1+jMax2],*p42=&rgba[iNull+jMax2],*p43=&rgba[iMax1+jMax2],*p44=&rgba[iMax2+jMax2];
					int r =(p00[0]*k00+p01[0]*k01+p02[0]*k02+p03[0]*k03+p04[0]*k04+
							p10[0]*k10+p11[0]*k11+p12[0]*k12+p13[0]*k13+p14[0]*k14+
							p20[0]*k20+p21[0]*k21+p22[0]*k22+p23[0]*k23+p24[0]*k24+
							p30[0]*k30+p31[0]*k31+p32[0]*k32+p33[0]*k33+p34[0]*k34+
							p40[0]*k40+p41[0]*k41+p42[0]*k42+p43[0]*k43+p44[0]*k44+AddWhite)>>8;
					int g =(p00[1]*k00+p01[1]*k01+p02[1]*k02+p03[1]*k03+p04[1]*k04+
							p10[1]*k10+p11[1]*k11+p12[1]*k12+p13[1]*k13+p14[1]*k14+
							p20[1]*k20+p21[1]*k21+p22[1]*k22+p23[1]*k23+p24[1]*k24+
							p30[1]*k30+p31[1]*k31+p32[1]*k32+p33[1]*k33+p34[1]*k34+
							p40[1]*k40+p41[1]*k41+p42[1]*k42+p43[1]*k43+p44[1]*k44+AddWhite)>>8;
					int b =(p00[2]*k00+p01[2]*k01+p02[2]*k02+p03[2]*k03+p04[2]*k04+
							p10[2]*k10+p11[2]*k11+p12[2]*k12+p13[2]*k13+p14[2]*k14+
							p20[2]*k20+p21[2]*k21+p22[2]*k22+p23[2]*k23+p24[2]*k24+
							p30[2]*k30+p31[2]*k31+p32[2]*k32+p33[2]*k33+p34[2]*k34+
							p40[2]*k40+p41[2]*k41+p42[2]*k42+p43[2]*k43+p44[2]*k44+AddWhite)>>8;
					int a =(p00[3]*k00+p01[3]*k01+p02[3]*k02+p03[3]*k03+p04[3]*k04+
							p10[3]*k10+p11[3]*k11+p12[3]*k12+p13[3]*k13+p14[3]*k14+
							p20[3]*k20+p21[3]*k21+p22[3]*k22+p23[3]*k23+p24[3]*k24+
							p30[3]*k30+p31[3]*k31+p32[3]*k32+p33[3]*k33+p34[3]*k34+
							p40[3]*k40+p41[3]*k41+p42[3]*k42+p43[3]*k43+p44[3]*k44)>>8;
					if(r>rm)r=rm; if(g>gm)g=gm;	if(b>bm)b=bm;
					int color=(r<<rs)|(g<<gs)|(b<<bs)|(a<<as);
					memcpy(Dst,&color,bpp);
				}
		delete[] rgba;
	}
}

// Render device selection

cInterfaceRenderDevice *gb_RenderDevice = nullptr;

cInterfaceRenderDevice* CreateIRenderDevice(eRenderDeviceSelection selection) {
    cInterfaceRenderDevice* device = nullptr;
    //TODO make this runtime selectable
    switch (selection) {
        case DEVICE_D3D9:
#ifdef PERIMETER_D3D9
            device = new cD3DRender();
#endif
            printf("Selected render: D3D9\n");
            break;
        case DEVICE_SOKOL:
#ifdef PERIMETER_SOKOL
            device = new cSokolRender();
#endif
            printf("Selected render: Sokol\n");
            break;
        case DEVICE_HEADLESS:
            device = new cEmptyRender();
            printf("Selected render: none\n");
            break;
        default:
            break;
    }
    if (!device) {
        ErrH.Abort("Unknown render device selected", XERR_USER, selection);
    }
    return device;
}
