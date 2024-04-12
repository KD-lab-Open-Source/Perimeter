#ifndef PERIMETER_SOURCEUIRESOLUTION_H
#define PERIMETER_SOURCEUIRESOLUTION_H

#include "GameShellSq.h"
#include "Runtime.h"

//For now the main menu UI is forced to be 4:3
const float MAIN_MENU_RATIO = 4.0f / 3.0f;

enum SHELL_ANCHOR
{
    //Center UI elements in X axis of screen resolution maintaining UI aspect ratio
    SHELL_ANCHOR_CENTER = 0,
    //Place UI elements at left side of screen maintaining UI aspect ratio
    SHELL_ANCHOR_LEFT = 1,
    //Place UI elements at right side of screen maintaining UI aspect ratio
    SHELL_ANCHOR_RIGHT = 2,
    //Scale UI elements from UI resolution to screen resolution
    SHELL_ANCHOR_SCALED = 3,
    
    //Internal usage on this point downwards
    SHELL_ANCHOR_DEFAULT = 4,
    //Special anchor mode for menu, basically same as SHELL_ANCHOR_CENTER
    SHELL_ANCHOR_MENU = 5,
};

class UIResolution {
public:
    bool legacy;
    int x, y;
    //Menu ratio'd X derived from Y
    float mx;
    
    FORCEINLINE UIResolution(bool legacy_, int x_, int y_) {
        this->set(legacy_, x_, y_);
    }
    
    FORCEINLINE void set(bool legacy_, int x_, int y_) {
        this->legacy = legacy_;
        this->x = x_;
        this->y = y_;
        this->mx = xm::round(static_cast<float>(y_) * MAIN_MENU_RATIO);
    }

    FORCEINLINE void set(const UIResolution& v) {
        legacy = v.legacy;
        x = v.x;
        y = v.y;
        mx = v.mx;
    }

    FORCEINLINE int operator == (const UIResolution& v) const {
        return x == v.x && y == v.y && legacy == v.legacy;
    }
};

struct SortResolutionVectors {
    inline bool operator ()(const Vect2i& s1,const Vect2i& s2) {
        return s1.x <= s2.x && s1.y < s2.y;
    }
    
    inline bool operator ()(const UIResolution& s1,const UIResolution& s2) {
        return s1.x <= s2.x && s1.y < s2.y;
    }
};

extern char _bMenuMode;
extern UIResolution source_ui_resolution;
extern Vect2f source_ui_factor;
extern SHELL_ANCHOR shell_anchor;

inline float getRenderRatio() {
    return static_cast<float>(terRenderDevice->GetSizeX())
         / static_cast<float>(terRenderDevice->GetSizeY());
}

inline float getUIX(SHELL_ANCHOR anchor) {
    if (anchor == SHELL_ANCHOR_MENU) {
        return source_ui_resolution.mx;
    } else {
        return static_cast<float>(source_ui_resolution.x);
    }
}

inline float getUIY() {
    return static_cast<float>(source_ui_resolution.y);
}

inline float relativeX(float x, SHELL_ANCHOR anchor) {
    return x * getUIX(anchor) / SQSH_COORD_WIDTH_SCALE;
}

inline float relativeY(float y) {
    return y * getUIY() / SQSH_COORD_HEIGHT_SCALE;
}

inline Vect2f relativeUV(float x, float y, cTexture *texture, SHELL_ANCHOR anchor) {
    xassert(texture != nullptr);
    return Vect2f(
        relativeX(x, anchor) / (float)texture->GetWidth(),
        relativeY(y) / (float)texture->GetHeight()
    );
}

int absoluteUIPosX(float x, SHELL_ANCHOR anchor);

int absoluteUIPosY(float y, SHELL_ANCHOR anchor);

int absoluteUISizeX(float x, SHELL_ANCHOR anchor);

int absoluteUISizeY(float y, SHELL_ANCHOR anchor);

void initSourceUIResolution();

bool setSourceUIResolution(const Vect2i& res);

const UIResolution& getMinimumUIResolution();

const std::vector<UIResolution>& getSourceUIResolutions();

#endif //PERIMETER_SOURCEUIRESOLUTION_H
