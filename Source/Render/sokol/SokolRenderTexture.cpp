#include <vector>
#include "xmath.h"
#include "Umath.h"
#include "StdAfxRD.h"
#include "sokol_gfx.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "FileImage.h"
#include "SokolResources.h"

int cSokolRender::CreateTexture(cTexture* Texture, cFileImage* FileImage, bool enable_assert) {
    bool is_alpha_test = false;
    bool is_alpha_blend = false;
    bool is_skin=Texture->skin_color.a==255;
    int dx = Texture->GetWidth();
    int dy = Texture->GetHeight();
    size_t tex_len = dx * dy * 4;

    for (int i=0; i<Texture->GetNumberFrame(); i++) {
        SokolTexture2D*& img = Texture->GetFrameImage(i).sg;
        if (img) {
            delete img;
            img = nullptr;
        }
        std::string label = Texture->GetName() + std::to_string(i); 
        sg_image_desc desc = {};
        desc.label = label.c_str();
        desc.width = dx;
        desc.height = dy;
        desc.wrap_u = desc.wrap_v = SG_WRAP_REPEAT;
        desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        desc.num_mipmaps = Texture->GetNumberMipMap();
        desc.min_filter = desc.mag_filter = desc.num_mipmaps ? SG_FILTER_NEAREST : SG_FILTER_NEAREST_MIPMAP_LINEAR;

        if (!FileImage) {
            desc.usage = SG_USAGE_DYNAMIC;
            img = new SokolTexture2D(desc);
        } else {
            desc.usage = SG_USAGE_IMMUTABLE;
            std::vector<uint8_t*> buffers;

            uint8_t* buf = new uint8_t[tex_len];
            buffers.push_back(buf);
            memset(buf, 0xFF, tex_len);
            //Load in RGBA
            FileImage->GetTextureRGB(buf, i * Texture->GetTimePerFrame(), 4, 4 * dx,
                                     8, 8, 8, 0, 8, 16, dx, dy);

            //If it has alpha then load it
            if (Texture->IsAlpha() || Texture->IsAlphaTest()) {
                FileImage->GetTextureAlpha(buf, i * Texture->GetTimePerFrame(), 4, 4 * dx,
                                           8, 24, dx, dy);

                // We need to check alpha color bytes
                bool finished_early = false;
                for (size_t n = 3; n < tex_len; n += 4) {
                    uint8_t alpha = buf[n];
                    if (alpha != 255 && alpha != 0) {
                        finished_early = true;
                        break;
                    }
                }

                if (finished_early) {
                    is_alpha_blend = true;
                } else {
                    is_alpha_test = true;
                }
            }

            if (is_skin) {
                ApplySkinColor(buf, dx, dy, Texture->skin_color);
            }

            //We need to convert grayscale bumpmap to normalmap
            if (Texture->GetAttribute(TEXTURE_BUMP) && !Texture->GetAttribute(TEXTURE_NORMAL)) {
                Texture->ConvertBumpToNormal(buf);
            }

            //Load buffer into desc image
            desc.data.subimage[0][0] = { buf, tex_len };

            //Load mipmaps    
            if (1 < desc.num_mipmaps) {
                for (int nMipMap = 1; nMipMap < desc.num_mipmaps; nMipMap++) {
                    int mmw = dx >> nMipMap;
                    int mmh = dy >> nMipMap;
                    size_t bufNextLen = mmw * mmh * 4;
                    uint8_t *bufNext = new uint8_t[bufNextLen];
                    buffers.push_back(bufNext);

                    BuildMipMap(mmw, mmh, 4, 8 * mmw, buf, 4 * mmw, bufNext,
                                8, 8, 8, 8, 16, 8, 0, 24,
                                Texture->GetAttribute(TEXTURE_MIPMAP_POINT | TEXTURE_MIPMAPBLUR | TEXTURE_BLURWHITE)
                    );

                    desc.data.subimage[0][nMipMap] = { bufNext, bufNextLen };

                    buf = bufNext;
                }
            }

            img = new SokolTexture2D(desc);

            //Cleanup buffers
            for (uint8_t* dbuf : buffers) {
                delete[] dbuf;
            }
        }

        Texture->GetFrameImage(i).sg = img;
    }

    if (is_skin) {
        Texture->ClearAttribute(TEXTURE_ALPHA_BLEND|TEXTURE_ALPHA_TEST);
    } else if(is_alpha_test && !is_alpha_blend) {
        Texture->ClearAttribute(TEXTURE_BLURWHITE|TEXTURE_MIPMAPBLUR|TEXTURE_ALPHA_BLEND);
        Texture->SetAttribute(TEXTURE_MIPMAP_POINT|TEXTURE_ALPHA_TEST);
    }

    return 0;
}

int cSokolRender::DeleteTexture(cTexture* Texture) {
    for (auto frame : Texture->frames) {
        delete frame.sg;
        frame.ptr = nullptr;
    }
    return 0;
}

void* cSokolRender::LockTexture(cTexture* Texture, int& Pitch) {
    SokolTexture2D* tex = Texture->GetFrameImage(0).sg;
    xassert(!tex->locked);
    xassert(tex->data);
    Pitch = Texture->GetWidth() * sokol_pixelformat_bytesize(tex->pixel_format);
    if (!tex->data) {
        //Immutable texture, nothing to do
        return nullptr;
    }
    tex->dirty = true;
    tex->locked = true;
    return tex->data;
}

void cSokolRender::UnlockTexture(cTexture* Texture) {
    SokolTexture2D* tex = Texture->GetFrameImage(0).sg;
    xassert(tex->locked);
    xassert(tex->data);
    if (!tex->data) {
        return;
    }
    tex->locked = false;
}
