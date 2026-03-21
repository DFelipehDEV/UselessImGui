#pragma once

#define GMREAL extern "C" __declspec(dllexport) double __cdecl
#define GMSTRING extern "C" __declspec(dllexport) char* __cdecl

#define GMCOLOR_R(c) ((float)(static_cast<int>(c) & 0xFF) / 255.0f)
#define GMCOLOR_G(c) ((float)((static_cast<int>(c) >> 8) & 0xFF) / 255.0f)
#define GMCOLOR_B(c) ((float)((static_cast<int>(c) >> 16) & 0xFF) / 255.0f)
#define GMCOLOR_TO_RGB(c) GMCOLOR_R(c), GMCOLOR_G(c), GMCOLOR_B(c)
#define RGB_TO_GMCOLOR(r, g, b) (static_cast<double>( \
    (static_cast<int>((r) * 255.0f) & 0xFF) | \
    ((static_cast<int>((g) * 255.0f) & 0xFF) << 8) | \
    ((static_cast<int>((b) * 255.0f) & 0xFF) << 16)))