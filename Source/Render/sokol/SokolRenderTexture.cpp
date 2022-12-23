#include <vector>
#include "xmath.h"
#include "Umath.h"
#include <SDL.h>
#include "sokol_gfx.h"
#include "IRenderDevice.h"
#include "EmptyRenderDevice.h"
#include "SokolRender.h"
#include "xerrhand.h"
#include "SokolResources.h"
#include "Src/BaseClass.h"
#include "Texture.h"

int cSokolRender::CreateTexture(cTexture* Texture, cFileImage* FileImage, bool enable_assert) {
    sg_image_desc desc = {};
    desc.width = Texture->GetWidth();
    desc.height = Texture->GetHeight();
    desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    desc.usage = FileImage ? SG_USAGE_IMMUTABLE : SG_USAGE_DYNAMIC;
    
    //TODO load from FileImage? might as well make inmutable then
    //desc.data.subimage[0][0] = {data, data_len};
    //TODO whereiputthis = new SokolTexture2D(desc);

    return 0;
}

int cSokolRender::DeleteTexture(cTexture* Texture) {
    //TODO
    return 0;
}

void* cSokolRender::LockTexture(cTexture* Texture, int& Pitch) {
    //TODO
    return nullptr;
}

void cSokolRender::UnlockTexture(cTexture* Texture) {
    //TODO
}
