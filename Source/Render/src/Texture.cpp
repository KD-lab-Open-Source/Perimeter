#include "StdAfxRD.h"
#include "files/files.h"
#include "../../Terra/crc.h"

cTexture::cTexture(const char *TexName) : cUnknownClass(KIND_TEXTURE)
{ 
	SetName(TexName);
	_x=_y=_w=_h=TimePerFrame=0; 
	number_mipmap=1;
	skin_color.set(255,255,255,0);
}
cTexture::~cTexture()														
{ 
	if(gb_RenderDevice)
		gb_RenderDevice->DeleteTexture(this);
	else
		VISASSERT(0 && "Текстура удалена слишком поздно");
}

int cTexture::GetNumberMipMap()
{
	return number_mipmap;
}
void cTexture::SetNumberMipMap(int number)
{
	number_mipmap=number;
}


uint8_t* cTexture::LockTexture(int& Pitch)
{
	return static_cast<uint8_t*>(gb_VisGeneric->GetRenderDevice()->LockTexture(this, Pitch));
}

void cTexture::UnlockTexture()
{
	gb_VisGeneric->GetRenderDevice()->UnlockTexture(this);
}

void cTexture::SetName(const char *Name)
{
	if(Name)name=Name;
	else name.clear();
}

void cTexture::SetWidth(int xTex)
{
	_x=ReturnBit(xTex);
    _w = 1 << _x;
	if (_w != xTex) {
        VisError << "Bad width in texture " << name << VERR_END;
    }
}

void cTexture::SetHeight(int yTex)
{
	_y = ReturnBit(yTex);
    _h = 1 << _y;
	if (_h != yTex) {
        VisError << "Bad height in texture " << name << VERR_END;
    }
}

inline uint8_t ByteInterpolate(uint8_t a, uint8_t b, uint8_t factor)
{
//	float f=factor/255.0f;
//	return xm::round(a+(int(b-a))*f);
    return a+(int(b-a))*int(factor)/255;
}

void ApplySkinColor(uint8_t* buffer,int dx,int dy,sColor4c skin_color)
{
    uint8_t* cur=buffer;
    for (int y=0; y<dy; y++) {
        for (int x=0; x<dx; x++, cur += 4) {
            sColor4c& c = *reinterpret_cast<sColor4c*>(cur);
            c.r=ByteInterpolate(c.r,skin_color.r,c.a);
            c.g=ByteInterpolate(c.g,skin_color.g,c.a);
            c.b=ByteInterpolate(c.b,skin_color.b,c.a);
            c.a=255;
        }
    }
}

void ConvertDot3(uint8_t* buf,int dx,int dy,float h_mul)
{
    const int byte_per_pixel=4;
#define GET(x,y) buf[(clamp(x,0,dx-1)+dx*clamp(y,0,dy-1))*byte_per_pixel]
    uint32_t* out=new uint32_t[dx*dy];

    for(int y=0;y<dy;y++) {
        for (int x = 0; x < dx; x++) {
            Vect3f dl, dn, dsum(0, 0, 0);
            int h00 = GET(x, y);
            int h01 = GET(x + 1, y);
            int h0m = GET(x - 1, y);
            int h10 = GET(x, y + 1);
            int hm0 = GET(x, y - 1);

            dl.set(0, 1, (h01 - h00) * h_mul);
            dn.set(0, -dl.z, dl.y);
            dsum += dn.normalize();

            dl.set(0, 1, (h00 - h0m) * h_mul);
            dn.set(0, -dl.z, dl.y);
            dsum += dn.normalize();

            dl.set(1, 0, (h10 - h00) * h_mul);
            dn.set(-dl.z, 0, dl.x);
            dsum += dn.normalize();

            dl.set(1, 0, (h00 - hm0) * h_mul);
            dn.set(-dl.z, 0, dl.x);
            dsum += dn.normalize();

            out[x + y * dx] = ColorByNormal(dsum.normalize());
        }
    }

    memcpy(buf,out,dx*dy*byte_per_pixel);
    delete[] out;
#undef GET
}
void cTexture::ConvertBumpToNormal(uint8_t* buffer) {
    //Get key of cache for this texture file and the file to store/retrieve the cache
    std::string key;
    filesystem_entry* entry = get_content_entry(GetName());
    if (entry) key = entry->key;
    if (key.empty()) key = convert_path_native(GetName());
    key = string_to_lower(key.c_str());
    unsigned int hash = crc32(reinterpret_cast<const unsigned char*>(key.c_str()), key.size(), startCRC32);
    key = std::string("cache") + PATH_SEP + "bump" + PATH_SEP + std::to_string(hash) +  ".bin";
    std::string path = convert_path_content(key, true);
    xassert(!path.empty());

    //Attempt to use cache since it takes some time to convert big files
    int64_t len = static_cast<int64_t>(sizeof(uint32_t)) * GetWidth() * GetHeight();
    XStream ff(0);
    bool usable = !path.empty() && ff.open(path, XS_IN) && 0 < ff.size();
    if (usable) {
        std::string sourcefile = convert_path_content(GetName());
        //If empty then the texture was loaded from .pak so assume is older than cache
        if (!sourcefile.empty()) {
            auto cachetime = std::filesystem::last_write_time(std::filesystem::u8path(path));
            auto sourcetime = std::filesystem::last_write_time(std::filesystem::u8path(sourcefile));
            //Mark as not usable if source file is modified after cache creation
            usable = sourcetime <= cachetime;
        }
    }
    if (usable) {
        //Load file and uncompress it
        XBuffer buf(ff.size(), true);
        XBuffer dest(0, true);
        ff.read(buf.address(), ff.size());
        if (buf.uncompress(dest) == 0 && dest.tell() == len) {
            memcpy(buffer, dest.address(), len);
        } else {
            usable = false;
        }
    }

    if (!usable) {
        //Convert grayscale bump map into normal map and cache it
        ConvertDot3(buffer, GetWidth(), GetHeight(), 1.0e-2f * bump_scale);
        if (ff.open(path, XS_OUT)) {
            XBuffer buf(buffer, len);
            buf.set(len);
            XBuffer compressed(len, true);
            if (buf.compress(compressed) == 0) {
                ff.write(compressed, compressed.tell());
            }
        }
    }
    ff.close();
}

