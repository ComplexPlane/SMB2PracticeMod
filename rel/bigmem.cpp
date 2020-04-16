#include "bigmem.h"

#include <gc/gc.h>
#include <mkb/mkb.h>
#include <cstring>

namespace bigmem {

// The additional memory Dolphin emulator is allocating to the game, above the game's actual arena
// Starts after the IPL code, and ends... less than 104 MiB later. Exactly 104 MiB later causes
// weird loading errors, potentially overwriting internal dolphin buffers?
// Perhaps I don't understand how much additional memory is actually meant to be allocated to the game...
static constexpr uint32_t EXTRAMEM_START = 0x81800000;
static constexpr uint32_t EXTRAMEM_END = 0x87fedb80;
// TODO I think this range can be increased a bit
//static constexpr uint32_t EXTRAMEM_START = 0xE0040000;
//static constexpr uint32_t EXTRAMEM_END = 0xE8000000;
static constexpr uint32_t EXTRAMEM_SIZE = EXTRAMEM_END - EXTRAMEM_START;

// Allocated in the normal memory arena
static constexpr uint32_t MAIN_HEAP_SIZE = 0x001E6DE0;
static constexpr uint32_t CHAR_HEAP_SIZE = 0x00480020;
static constexpr uint32_t REPLAY_HEAP_SIZE = 0x00028020;

// Original sizes
static constexpr uint32_t STAGE_HEAP_SIZE = 0x00320020;
static constexpr uint32_t BG_HEAP_SIZE = 0x00230020;
static constexpr uint32_t FIFO_SIZE = 0x100000;

// Allocated in the new memory space
//static constexpr uint32_t STAGE_HEAP_SIZE = 0x02600000; // 38 MiB
//static constexpr uint32_t BG_HEAP_SIZE = 0x02600000; // 38 MiB
//static constexpr uint32_t FIFO_SIZE = 14642560; // A little less than 14 MiB

static uint32_t bigArenaLo;
static uint32_t bigArenaHi;
static void *stageHeapStart;
static void *bgHeapStart;

static void *allocFromExtraMemLo(uint32_t size)
{
  uint32_t roundedSize = gc::OSRoundUp32B(size);
  ASSERT(bigArenaLo + roundedSize <= bigArenaHi);
  uint32_t ret = bigArenaLo;
  bigArenaLo += roundedSize;
  return reinterpret_cast<void *>(ret);
}

static void *getEndPtr(void *start, uint32_t size)
{
  return reinterpret_cast<void *>(reinterpret_cast<uint32_t>(start) + size);
}

static void reallocGXFifos(uint32_t fifoSize)
{
  static gc::GXFifoObj tmpFifoObj = {};

  void *newFifoBuf1 = allocFromExtraMemLo(fifoSize);
  void *newFifoBuf2 = allocFromExtraMemLo(fifoSize);

  gc::OSReport("[mod] Created new FIFO bufs at: 0x%08X and 0x%08X\n",
      reinterpret_cast<uint32_t>(newFifoBuf1),
      reinterpret_cast<uint32_t>(newFifoBuf2));

  gc::GXFifoObj *gpFifoObj = gc::GXGetGPFifo();
  gc::GXFifoObj *cpuFifoObj = gc::GXGetCPUFifo();

  //
  // Reallocate the GP FIFO
  //

  // Wait until GP has finished reading all commands from currently attached FIFO
  gc::GXBool throwAway = gc::GX_FALSE, readIdle = gc::GX_FALSE;
  while (!readIdle)
  {
    gc::GXGetGPStatus(&throwAway, &throwAway, &readIdle, &throwAway, &throwAway);
  }

  // Point temporary fifo obj to gp fifo buffer
  tmpFifoObj = {};
  gc::GXInitFifoBase(&tmpFifoObj, newFifoBuf1, fifoSize);
  gc::GXInitFifoPtrs(&tmpFifoObj, newFifoBuf1, newFifoBuf1);

  // Attach temporary fifo obj to GP
  gc::GXSetGPFifo(&tmpFifoObj);

  // Reset the fifo obj the GPU was attached to, then reattach it
  *gpFifoObj = {};
  gc::GXInitFifoBase(gpFifoObj, newFifoBuf1, fifoSize);
  gc::GXInitFifoPtrs(gpFifoObj, newFifoBuf1, newFifoBuf1);
  gc::GXSetGPFifo(gpFifoObj);

  //
  // Reallocate the CPU FIFO
  //

  gc::GXSaveCPUFifo(cpuFifoObj); // Make sure there aren't any cached pending commands that need to be flushed to the CPU FIFO buffer

  tmpFifoObj = {};
  gc::GXInitFifoBase(&tmpFifoObj, newFifoBuf2, fifoSize);
  gc::GXInitFifoPtrs(&tmpFifoObj, newFifoBuf2, newFifoBuf2);
  gc::GXSetCPUFifo(&tmpFifoObj);

  *cpuFifoObj = {};
  gc::GXInitFifoBase(cpuFifoObj, newFifoBuf2, fifoSize);
  gc::GXInitFifoPtrs(cpuFifoObj, newFifoBuf2, newFifoBuf2);
  gc::GXSetCPUFifo(cpuFifoObj);
}

void init()
{
  // Now that game has booted, restore the actual available memory size
//  *reinterpret_cast<uint32_t *>(0x80000028) = 0x08000000;
//  *reinterpret_cast<uint32_t *>(0x800000f0) = 0x08000000;

  bigArenaLo = EXTRAMEM_START;
  bigArenaHi = EXTRAMEM_END;

  // Zero the extramem
  std::memset(reinterpret_cast<void *>(EXTRAMEM_START), 0, EXTRAMEM_SIZE);

//  uint32_t fifoSize = gc::OSRoundDown32B(EXTRAMEM_SIZE / 4 - 32);
//  reallocGXFifos(fifoSize);
//  reallocGXFifos(FIFO_SIZE);

  stageHeapStart = allocFromExtraMemLo(STAGE_HEAP_SIZE);
  bgHeapStart = allocFromExtraMemLo(BG_HEAP_SIZE);
}

void createGameHeapsUsingExtraMem()
{
  // Reset the arena boundaries to reflect the original available space for game heaps,
  // like the original createGameHeaps()
  // Not all of it will get used this time around though, since we'll make some heaps
  // in our own space
  gc::OSSetArenaLo(reinterpret_cast<void *>(0x808F3FC0));
  gc::OSSetArenaHi(reinterpret_cast<void *>(0x814D2E20));

  // Allocate the heaps we don't want to change the size of in the original arena
  void *mainHeapStart = gc::OSAllocFromArenaLo(MAIN_HEAP_SIZE, 32);
//  stageHeapStart = gc::OSAllocFromArenaLo(STAGE_HEAP_SIZE, 32);
//  bgHeapStart = gc::OSAllocFromArenaLo(BG_HEAP_SIZE, 32);
  void *charHeapStart = gc::OSAllocFromArenaLo(CHAR_HEAP_SIZE, 32);
  void *replayHeapStart = gc::OSAllocFromArenaLo(REPLAY_HEAP_SIZE, 32);

  void *mainHeapEnd = getEndPtr(mainHeapStart, MAIN_HEAP_SIZE);
  void *stageHeapEnd = getEndPtr(stageHeapStart, STAGE_HEAP_SIZE);
  void *bgHeapEnd = getEndPtr(bgHeapStart, BG_HEAP_SIZE);
  void *charHeapEnd = getEndPtr(charHeapStart, CHAR_HEAP_SIZE);
  void *replayHeapEnd = getEndPtr(replayHeapStart, REPLAY_HEAP_SIZE);

  mkb::mainHeap = gc::OSCreateHeap(mainHeapStart, mainHeapEnd);
  mkb::stageHeap = gc::OSCreateHeap(stageHeapStart, stageHeapEnd);
  mkb::bgHeap = gc::OSCreateHeap(bgHeapStart, bgHeapEnd);
  mkb::charHeap = gc::OSCreateHeap(charHeapStart, charHeapEnd);
  mkb::replayHeap = gc::OSCreateHeap(replayHeapStart, replayHeapEnd);

  mkb::mainHeapSize = gc::OSCheckHeap(mkb::mainHeap);
//  mkb::stageHeapSize = gc::OSCheckHeap(mkb::stageHeap);
//  mkb::bgHeapSize = gc::OSCheckHeap(mkb::bgHeap);
  mkb::stageHeapSize = STAGE_HEAP_SIZE - 32;
  mkb::bgHeapSize = BG_HEAP_SIZE - 32;
  mkb::charHeapSize = gc::OSCheckHeap(mkb::charHeap);
  mkb::replayHeapSize = gc::OSCheckHeap(mkb::replayHeap);

  // Finish like the original game heap setup
  gc::OSSetCurrentHeap(mkb::mainHeap);
  mkb::gSomethingWithGameHeaps = 0;
}

}
