#pragma once

#include <gc/ppc.h>
#include <gc/gxenum.h>
#include <gc/gxstruct.h>
#include <gc/mathtypes.h>

namespace gc
{

typedef u8 GXBool;

constexpr GXBool GX_TRUE = 1;
constexpr GXBool GX_FALSE = 0;

extern "C" {

extern volatile PPCWGPipe GXWGFifo;

void GXSetTevColor(GXTevRegID id, GXColor color);
void GXBegin(GXPrimitive type, GXVtxFmt vtxfmt, u16 nverts);
void GXCopyDisp(void *dest, GXBool clear);

void GXInitFifoBase(GXFifoObj *fifo, void *base, u32 size);
void GXInitFifoPtrs(GXFifoObj *fifo, void *read_ptr, void *write_ptr);
void GXSetCPUFifo(GXFifoObj *fifo);
GXFifoObj *GXGetCPUFifo();
void GXSetGPFifo(GXFifoObj *fifo);
GXFifoObj *GXGetGPFifo();
void GXSaveCPUFifo(GXFifoObj *fifo);
void GXGetGPStatus(GXBool *overhi, GXBool *underlow, GXBool *readIdle, GXBool *cmdIdle, GXBool *brkpt);

}

inline void GXPosition3f32(float x, float y, float z)
{
    GXWGFifo.v_f32 = x;
    GXWGFifo.v_f32 = y;
    GXWGFifo.v_f32 = z;
}

inline void GXTexCoord2f32(float s, float t)
{
    GXWGFifo.v_f32 = s;
    GXWGFifo.v_f32 = t;
}

}