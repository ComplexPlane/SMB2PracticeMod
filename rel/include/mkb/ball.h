#pragma once

#include <mkb/mathtypes.h>
#include <mkb/tickable.h>

namespace mkb
{

struct Ball
{
    uint8_t status; /* Actually called just "STAT" in the debug menu */
    uint8_t unk_0x1[2];
    uint8_t gPhysicsStateFlags; /* Something to do with physics state (Crashes the game if set wrong) (8 bit bitmask) */
    Vec3f pos;
    Vec3f prevPos;
    Vec3f vel; /* Velocity/speed */
    uint16_t gSomeRot;
    uint16_t gSomeRot2;
    uint16_t gSomeRot3;
    char gNotPadding; /* Used to be called padding, but I saw it written to */
    uint8_t field_0x2f;
    float gSomeRot4;
    float gSomeRot5;
    float xAngle;
    float xPosCopy;
    float gSomeRot6;
    float gSomeRot7;
    float yAngle;
    float yPosCopy;
    float gSomeRot8;
    float gSomeRot9;
    float zAngle;
    float zPosCopy;
    uint16_t gSomeRot10;
    uint16_t gSomeRot11;
    uint16_t gSomeRot12;
    uint16_t padding2;
    float ballSize;
    float acceleration;
    float restitution;
    float visualScale;
    uint32_t bananaCount;
    uint32_t score;
    uint32_t levelStopwatch;
    uint8_t unk_0x84[24];
    /* Some more flags related to ball state?
     * The lowest-order bit may represent "is ball touching the ground" and I believe if affects the physics */
    uint32_t someBitfield;
    uint8_t unk_0xa0[100];
    struct Ape *ape;
    uint8_t unk_0x108[46];
    int16_t gSomethingTimer; /* Created by retype action */
    uint8_t unk_0x138[120];
} __attribute__((__packed__));

static_assert(sizeof(Ball) == 0x1b0);

struct Ape
{
    // More fields are known, I'm just being lazy with defining them for now
    uint8_t unk_0x0[0x86];
    uint8_t charaAnimType;
    uint8_t unk_0x69[513];
    Quat charaRotation;
    uint8_t unk_0x298[88];
} __attribute__((__packed__));

static_assert(sizeof(Ape) == 0x2f0);

extern "C" {

extern Ball balls[8];

}

}
