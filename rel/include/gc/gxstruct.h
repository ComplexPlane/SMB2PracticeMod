#pragma once

#include <gc/gxenum.h>

#include <cstdint>

namespace gc
{

struct GXColor
{ /* A generic color structure used by various GX API functions. */
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct GXColorS10
{
    int16_t r;
    int16_t g;
    int16_t b;
    int16_t a;
};

struct GXFogAdjTable
{
    uint16_t dummy[10];
};

struct GXLightObj
{
    uint32_t dummy[16];
};

struct GXRenderModeObj
{
    enum VITVMode viTVmode;
    uint16_t fbWidth;
    uint16_t efbHeight;
    uint16_t xfbHeight;
    uint16_t viXOrigin;
    uint16_t viYOrigin;
    uint16_t viWidth;
    uint16_t viHeight;
    enum VIXFBMode xFBmode;
    uint8_t field_rendering;
    uint8_t aa;
    uint8_t sample_pattern[12][2];
    uint8_t vfilter[7];
};

struct GXTexObj
{
    uint32_t dummy[8];
};

struct GXTexRegion
{
    uint32_t dummy[4];
};

struct GXTlutObj
{
    uint32_t dummy[3];
};

struct GXTlutRegion
{
    uint32_t dummy[4];
};

struct GXVtxAttrFmtList
{
    enum GXAttr attr;
    enum GXCompCnt cnt;
    enum GXCompType type;
    uint8_t frac;
};

struct GXVtxDescList
{
    enum GXAttr attr;
    enum GXAttrType type;
};

struct GXFifoObj
{
    uint8_t pad[128];
};

}
