#include "patch.h"
#include "pad.h"
#include "global.h"

#include <gc/gc.h>

#include <cstdint>

namespace mod::patch {

void NopToggle::init(void *dest, uint16_t combo1, uint16_t combo2) {
	m_dest = dest;
	m_instr = 0x60000000;
	m_combo1 = combo1;
	m_combo2 = combo2;
}

void NopToggle::update() {
	if (pad::buttonChordPressed(m_combo1, m_combo2)) {
		uint32_t tmp = *reinterpret_cast<uint32_t *>(m_dest);
		*reinterpret_cast<uint32_t *>(m_dest) = m_instr;
		m_instr = tmp;
		clear_DC_IC_Cache(m_dest, sizeof(uint32_t));
	}
}

void clear_DC_IC_Cache(void *ptr, uint32_t size)
{
	gc::DCFlushRange(ptr, size);
	gc::ICInvalidateRange(ptr, size);
}

//void writeStandardBranches(void *address, void functionStart(), void functionBranchBack())
//{
//	void *BranchBackAddress = reinterpret_cast<void *>(
//		reinterpret_cast<uint32_t>(address) + 0x4);
//
//	writeBranch(address, reinterpret_cast<void *>(functionStart));
//	writeBranch(reinterpret_cast<void *>(functionBranchBack), BranchBackAddress);
//}

void writeBranch(void *ptr, void *destination)
{
	uint32_t branch = 0x48000000; // b
	writeBranchMain(ptr, destination, branch);
}

void writeBranchBL(void *ptr, void *destination)
{
	uint32_t branch = 0x48000001; // bl
	writeBranchMain(ptr, destination, branch);
}

void writeBLR(void *ptr)
{
  writeWord(ptr, 0x4e800020);
}

void writeBranchMain(void *ptr, void *destination, uint32_t branch)
{
	uint32_t delta = reinterpret_cast<uint32_t>(destination) - reinterpret_cast<uint32_t>(ptr);
	
	branch |= (delta & 0x03FFFFFC);
	
	uint32_t *p = reinterpret_cast<uint32_t *>(ptr);
	*p = branch;
	
	clear_DC_IC_Cache(ptr, sizeof(uint32_t));
}

void writeWord(void *ptr, uint32_t data)
{
  *reinterpret_cast<uint32_t *>(ptr) = data;
  clear_DC_IC_Cache(ptr, sizeof(uint32_t));
}

void writeNop(void *ptr)
{
  writeWord(ptr, 0x60000000);
}

}