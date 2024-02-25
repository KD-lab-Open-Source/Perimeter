#ifndef PERIMETER_RENDERUTILS_H
#define PERIMETER_RENDERUTILS_H

#ifdef RENDERUTILS_HWND_FROM_SDL_WINDOW
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#ifdef _WIN32
//Needed for extracting HWND from SDL_Window, in Linux it gives conflict due to XErrorHandler
#include <SDL_syswm.h>
//For hWndVisGeneric
#include "SystemUtil.h"
#endif
#endif //RENDERUTILS_HWND_FROM_SDL_WINDOW

static void copy_indices_list(indices_t* dst, const indices_t* src, size_t len, size_t written_vertices) {
    if (written_vertices == 0) {
        //No indices shifting required, copy as is
        memcpy(dst, src, len * sizeof(indices_t));
    } else {
        size_t i = 0;
        //8 unrolled
        while (i + 7 < len) {
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
        }
        //3 unrolled
        while (i + 2 < len) {
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
            dst[i] = src[i] + written_vertices; i++;
        }
        //Leftovers
        while (i < len) {
            dst[i] = src[i] + written_vertices; i++;
        }
    }
}

#ifdef RENDERUTILS_HWND_FROM_SDL_WINDOW
static HWND get_hwnd_from_sdl_window(SDL_Window* sdl_window) {
    HWND hWnd;
#ifdef _WIN32
    //Get HWND from SDL window
    SDL_SysWMinfo wm_info;
    SDL_VERSION(&wm_info.version);
    SDL_GetWindowWMInfo(sdl_window, &wm_info);
    
    hWnd = wm_info.info.win.window;
    hWndVisGeneric = hWnd;
#else
    //dxvk-native uses HWND as SDL2 window handle, so this is allowed
    hWnd = static_cast<HWND>(sdl_window);
#endif

    return hWnd;
}
#endif

#endif //PERIMETER_RENDERUTILS_H
