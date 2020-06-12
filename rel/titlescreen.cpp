#include "titlescreen.h"

#include "patch.h"
#include "assembly.h"

#include <cstring>

namespace titlescreen
{

static const char *text = "APESPHERE PRACTICE MOD";

void init()
{
    strcpy(reinterpret_cast<char *>(0x8047f4ec), text);
    patch::writeBranch(reinterpret_cast<void *>(0x8032ad0c),
                       reinterpret_cast<void *>(main::CustomTitleScreenTextColor));
}

void update() {}

}