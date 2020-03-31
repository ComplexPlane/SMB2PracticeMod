#pragma once

#include <gc/ppc.h>
#include <gc/gxenum.h>
#include <gc/gxstruct.h>

#include <cstdint>

namespace gc {

typedef uint8_t GXBool;

constexpr GXBool GX_TRUE = 1;
constexpr GXBool GX_FALSE = 0;

extern "C" {

extern volatile PPCWGPipe GXWGFifo;

void GXSetTevColor(GXTevRegID id, GXColor color);
void GXBegin(GXPrimitive type, GXVtxFmt vtxfmt, uint16_t nverts);
void GXCopyDisp(void *dest, GXBool clear);

void GXInitFifoBase(GXFifoObj *fifo, void *base, uint32_t size);
void GXInitFifoPtrs(GXFifoObj *fifo, void *read_ptr, void *write_ptr);
void GXSetCPUFifo(GXFifoObj *fifo);
GXFifoObj *GXGetCPUFifo();
void GXSetGPFifo(GXFifoObj *fifo);
GXFifoObj *GXGetGPFifo();
void GXSaveCPUFifo(GXFifoObj *fifo);
void GXGetGPStatus(GXBool *overhi, GXBool *underlow, GXBool *readIdle, GXBool *cmdIdle, GXBool *brkpt);

}

inline void GXPosition3f32(float x, float y, float z) {
  GXWGFifo.f32 = x;
  GXWGFifo.f32 = y;
  GXWGFifo.f32 = z;
}

inline void GXTexCoord2f32(float s, float t) {
  GXWGFifo.f32 = s;
  GXWGFifo.f32 = t;
}

}