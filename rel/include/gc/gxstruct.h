#pragma once

#include <gc/gxenum.h>
#include <gc/mathtypes.h>

namespace gc
{

struct GXColor
{ /* A generic color structure used by various GX API functions. */
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

struct GXColorS10
{
    s16 r;
    s16 g;
    s16 b;
    s16 a;
};

struct GXFogAdjTable
{
    u16 dummy[10];
};

struct GXLightObj
{
    u32 dummy[16];
};

struct GXRenderModeObj
{
    enum VITVMode viTVmode;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    enum VIXFBMode xFBmode;
    u8 field_rendering;
    u8 aa;
    u8 sample_pattern[12][2];
    u8 vfilter[7];
};

struct GXTexObj
{
    u32 dummy[8];
};

struct GXTexRegion
{
    u32 dummy[4];
};

struct GXTlutObj
{
    u32 dummy[3];
};

struct GXTlutRegion
{
    u32 dummy[4];
};

struct GXVtxAttrFmtList
{
    enum GXAttr attr;
    enum GXCompCnt cnt;
    enum GXCompType type;
    u8 frac;
};

struct GXVtxDescList
{
    enum GXAttr attr;
    enum GXAttrType type;
};

struct GXFifoObj
{
    u8 pad[128];
};

}
