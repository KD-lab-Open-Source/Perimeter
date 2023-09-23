#include <string>
#include <vector>
#include "xmath.h"
#include "Umath.h"
#include "StdAfxRD.h"
#include "VertexFormat.h"
#include "sokol_gfx.h"
#include "IRenderDevice.h"
#include "SokolRender.h"
#include "FileImage.h"
#include "SokolResources.h"
#include "RenderTracker.h"

int cSokolRender::CreateTexture(cTexture* Texture, cFileImage* FileImage, bool enable_assert) {
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::CREATE_TEXTURE, "", Texture);
#endif
    bool is_alpha_test = false;
    bool is_alpha_blend = false;
    bool is_skin=Texture->skin_color.a==255;
    int dx = Texture->GetWidth();
    int dy = Texture->GetHeight();
    size_t tex_len = dx * dy * 4;

    for (int i=0; i<Texture->GetNumberFrame(); i++) {
        SokolTexture2D*& img = Texture->GetFrameImage(i)->sg;
        if (img) {
            delete img;
            img = nullptr;
        }
        std::string label = Texture->GetName() + std::to_string(i); 
        sg_image_desc* desc = new sg_image_desc();
        desc->label = label.c_str();
        desc->width = dx;
        desc->height = dy;
        desc->wrap_u = desc->wrap_v = SG_WRAP_REPEAT;
        desc->pixel_format = SG_PIXELFORMAT_RGBA8;
        desc->num_slices = 1;
#ifdef SOKOL_GL
        //TODO check why mipmaps isn't working
        desc->num_mipmaps = 1;
#else
        desc->num_mipmaps = std::min(static_cast<int>(SG_MAX_MIPMAPS), Texture->GetNumberMipMap());
#endif
        //Filter must be linear for small font textures to not look unreadable
        desc->min_filter = 1 < desc->num_mipmaps ? SG_FILTER_LINEAR_MIPMAP_LINEAR : SG_FILTER_LINEAR;
        desc->mag_filter = SG_FILTER_LINEAR;

        if (!FileImage) {
            desc->usage = SG_USAGE_DYNAMIC;
            img = new SokolTexture2D(desc);
        } else {
            desc->usage = SG_USAGE_IMMUTABLE;
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
            desc->data.subimage[0][0] = { buf, tex_len };

            //Load mipmaps    
            if (1 < desc->num_mipmaps) {
                for (int nMipMap = 1; nMipMap < desc->num_mipmaps; nMipMap++) {
                    int mmw = dx >> nMipMap;
                    int mmh = dy >> nMipMap;
                    size_t bufNextLen = mmw * mmh * 4;
                    uint8_t *bufNext = new uint8_t[bufNextLen];
                    buffers.push_back(bufNext);

                    BuildMipMap(mmw, mmh, 4, 8 * mmw, buf, 4 * mmw, bufNext,
                                8, 8, 8, 8,
                                0, 8, 16, 24,
                                Texture->GetAttribute(TEXTURE_MIPMAP_POINT | TEXTURE_MIPMAPBLUR | TEXTURE_BLURWHITE)
                    );

                    desc->data.subimage[0][nMipMap] = { bufNext, bufNextLen };

                    buf = bufNext;
                }
            }

            img = new SokolTexture2D(desc);
        }

        Texture->GetFrameImage(i)->sg = img;
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
#ifdef PERIMETER_RENDER_TRACKER_RESOURCES
    RenderSubmitEvent(RenderEvent::DELETE_TEXTURE, "", Texture);
#endif
    for (auto frame : Texture->frames) {
        delete frame.sg;
        frame.ptr = nullptr;
    }
    return 0;
}

void* cSokolRender::LockTexture(cTexture* Texture, int& Pitch) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::LOCK_TEXTURE);
#endif
    TextureImage* frm = Texture->GetFrameImage(0);
    xassert(frm);
    if (!frm) {
        return nullptr;
    }
    SokolTexture2D* tex = frm->sg;
    xassert(!tex->locked);
    xassert(tex->data);
    Pitch = static_cast<int>(Texture->GetWidth() * sokol_pixelformat_bytesize(tex->pixel_format));
    if (!tex->data) {
        //Immutable texture, nothing to do
        return nullptr;
    }
    tex->dirty = true;
    tex->locked = true;
    return tex->data;
}

void* cSokolRender::LockTextureRect(cTexture* Texture, int& Pitch, Vect2i pos, Vect2i size) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::LOCK_TEXTURE_RECT);
#endif
    TextureImage* frm = Texture->GetFrameImage(0);
    xassert(frm);
    if (!frm) {
        return nullptr;
    }
    SokolTexture2D* tex = frm->sg;
    xassert(!tex->locked);
    xassert(tex->data);
    size_t fmt_size = sokol_pixelformat_bytesize(tex->pixel_format);
    Pitch = static_cast<int>(Texture->GetWidth() * fmt_size);
    if (!tex->data) {
        //Immutable texture, nothing to do
        return nullptr;
    }
    tex->dirty = true;
    tex->locked = true;
    uint8_t* ptr = static_cast<uint8_t*>(tex->data);
    size_t start = (pos.y * Texture->GetWidth() + pos.x) * fmt_size;
    return ptr + start;
}

void cSokolRender::UnlockTexture(cTexture* Texture) {
#ifdef PERIMETER_RENDER_TRACKER_LOCKS
    RenderSubmitEvent(RenderEvent::UNLOCK_TEXTURE);
#endif
    TextureImage* frm = Texture->GetFrameImage(0);
    xassert(frm);
    if (!frm) {
        return;
    }
    SokolTexture2D* tex = frm->sg;
    xassert(tex->locked);
    xassert(tex->data);
    if (!tex->data) {
        return;
    }
    tex->locked = false;
}
