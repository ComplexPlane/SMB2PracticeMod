#pragma once

#include <mkb/mathtypes.h>

#include <cstdint>

namespace mkb {

typedef uint16_t *StagedefCollisionGridTriIndices;
typedef StagedefCollisionGridTriIndices StagedefCollisionGridTilesTriIndices;

struct StagedefAnimHeader
{
    uint32_t rotXKeyframeCount;
    struct StagedefAnimKeyframe *rotXKeyframeList;
    uint32_t rotYKeyframeCount;
    struct StagedefAnimKeyframe *rotYKeyframeList;
    uint32_t rotZKeyframeCount;
    struct StagedefAnimKeyframe *rotZKeyframeList;
    uint32_t posXKeyframeCount;
    struct StagedefAnimKeyframe *posXKeyframeList;
    uint32_t posYKeyframeCount;
    struct StagedefAnimKeyframe *posYKeyframeList;
    uint32_t posZKeyframeCount;
    struct StagedefAnimKeyframe *posZKeyframeList;
    uint8_t unk_0x30[16];
} __attribute__((__packed__));

static_assert(sizeof(StagedefAnimHeader) == 0x40);

struct StagedefAnimKeyframe
{
    uint32_t easing;
    float time;
    float value;
    uint8_t unk_0xc[8];
} __attribute__((__packed__));

static_assert(sizeof(StagedefAnimKeyframe) == 0x14);

struct StagedefBackgroundAnim2Header
{
    uint8_t unk_0x0[4];
    float loopPointSeconds;
    uint32_t unk1KeyframeCount;
    struct StagedefAnimKeyframe *unk1KeyframeList;
    uint32_t unk2KeyframeCount;
    struct StagedefAnimKeyframe *Unk2KeyframeList;
    uint32_t rotXKeyframeCount;
    struct StagedefAnimKeyframe *rotXKeyframeList;
    uint32_t rotYKeyframeCount;
    struct StagedefAnimKeyframe *rotYKeyframeList;
    uint32_t rotZKeyframeCount;
    struct StagedefAnimKeyframe *rotZKeyframeList;
    uint32_t posXKeyframeCount;
    struct StagedefAnimKeyframe *posXKeyframeList;
    uint32_t posYKeyframeCount;
    struct StagedefAnimKeyframe *posYKeyframeList;
    uint32_t posZKeyframeCount;
    struct StagedefAnimKeyframe *posZKeyframeList;
    uint32_t unk9KeyframeCount;
    struct StagedefAnimKeyframe *unk9KeyframeList;
    uint32_t unk10KeyframeCount;
    struct StagedefAnimKeyframe *unk10KeyframeList;
    uint32_t unk11KeyframeCount;
    struct StagedefAnimKeyframe *unk11KeyframeList;
} __attribute__((__packed__));

static_assert(sizeof(StagedefBackgroundAnim2Header) == 0x60);

struct StagedefBackgroundAnimHeader
{
    uint8_t unk_0x0[4];
    float loop_point_seconds;
    uint8_t unk_0x8[8];
    uint32_t rotXKeyframeCount;
    struct StagedefAnimKeyframe *rotXKeyframeList;
    uint32_t rotYKeyframeCount;
    struct StagedefAnimKeyframe *rotYKeyframeList;
    uint32_t rotZKeyframeCount;
    struct StagedefAnimKeyframe *rotZKeyframeList;
    uint32_t posXKeyframeCount;
    struct StagedefAnimKeyframe *posXKeyframeList;
    uint32_t posYKeyframeCount;
    struct StagedefAnimKeyframe *posYKeyframeList;
    uint32_t posZKeyframeCount;
    struct StagedefAnimKeyframe *posZKeyframeList;
    uint8_t unk_0x40[16];
} __attribute__((__packed__));

static_assert(sizeof(StagedefBackgroundAnimHeader) == 0x50);

struct StagedefEffectHeader
{
    uint32_t fx1KeyframeCount;
    struct StagedefEffect1 *fx1KeyframeList;
    uint32_t fx2KeyframeCount;
    struct StagedefEffect2 *fx2KeyframeList;
    struct StagedefTextureScroll *textureScroll;
    uint8_t unk_0x14[28];
} __attribute__((__packed__));

static_assert(sizeof(StagedefEffectHeader) == 0x30);

struct StagedefEffect1
{ /* Likely used for animated textures, such as the silhouettes in Night */
    uint32_t unk_0x0;
    uint32_t unk_0x4;
    uint32_t unk_0x8;
    uint16_t unk_0x12;
    uint16_t unk_0x14;
    uint16_t unk_0x16;
    uint8_t unk_0x18[2];
} __attribute__((__packed__));

static_assert(sizeof(StagedefEffect1) == 0x14);

struct StagedefEffect2
{ /* May be used for the flames in Storm? */
    float unk_0x0;
    float unk_0x4;
    float unk_0x8;
    uint8_t unk_0xc[4];
} __attribute__((__packed__));

static_assert(sizeof(StagedefEffect2) == 0x10);

struct StagedefTextureScroll
{
    struct Vec2f speed;
} __attribute__((__packed__));

static_assert(sizeof(StagedefTextureScroll) == 0x8);

struct StagedefBackgroundModel
{
    uint8_t unk_0x0[4];
    char *modelName;
    uint8_t unk_0x8[4];
    struct Vec3f position;
    struct Vec3s rotation;
    uint8_t padding[2];
    struct Vec3f scale;
    struct StagedefBackgroundAnimHeader *backgroundAnimHeader;
    struct StagedefBackgroundAnim2Header *backgroundAnim2Header;
    struct StagedefEffectHeader *effectHeader;
} __attribute__((__packed__));

static_assert(sizeof(StagedefBackgroundModel) == 0x38);

enum BananaType
{
    BANANA_BUNCH = 1,
    BANANA_SINGLE = 0
};

struct StagedefBanana
{
    struct Vec3f position;
    enum BananaType type;
} __attribute__((__packed__));

static_assert(sizeof(StagedefBanana) == 0x10);

enum PlaybackState
{
    PLAYBACK_BACKWARD = 2,
    PLAYBACK_FAST_BACKWARD = 4,
    PLAYBACK_FAST_FORWARD = 3,
    PLAYBACK_FORWARD = 0,
    PLAYBACK_PAUSE = 1
};

struct StagedefStageModelInstance
{
    struct StagedefStageModelPtrA *stageModelA;
    struct Vec3f position;
    struct Vec3s rotation;
    uint8_t padding[2];
    struct Vec3f scale;
} __attribute__((__packed__));

static_assert(sizeof(StagedefStageModelInstance) == 0x24);

struct StagedefFogAnimHeader
{
    uint32_t startDistanceKeyframeCount;
    struct StagedefAnimKeyframe *startDistanceKeyframeList;
    uint32_t endDistanceKeyframeCount;
    struct StagedefAnimKeyframe *endDistanceKeyframeList;
    uint32_t redKeyframeCount;
    struct StagedefAnimKeyframe *redKeyframeList;
    uint32_t greenKeyframeCount;
    struct StagedefAnimKeyframe *greenKeyframeList;
    uint32_t blueKeyframeCount;
    struct StagedefAnimKeyframe *blueKeyframeList;
    uint32_t unkKeyframeCount;
    struct StagedefAnimKeyframe *unkKeyframeList;
} __attribute__((__packed__));

static_assert(sizeof(StagedefFogAnimHeader) == 0x30);

struct StagedefConeCollision
{
    struct Vec3f position;
    struct Vec3s rotation;
    uint8_t padding[2];
    struct Vec3f scale;
} __attribute__((__packed__));

static_assert(sizeof(StagedefConeCollision) == 0x20);

struct StagedefBumper
{
    struct Vec3f position;
    struct Vec3s rotation;
    uint8_t padding[2];
    struct Vec3f scale;
} __attribute__((__packed__));

static_assert(sizeof(StagedefBumper) == 0x20);

struct StagedefReflectiveStageModel
{
    char *modelName;
    uint8_t unk_0x8[8];
} __attribute__((__packed__));

static_assert(sizeof(StagedefReflectiveStageModel) == 0xc);

struct StagedefFallout
{
    float y;
} __attribute__((__packed__));

static_assert(sizeof(StagedefFallout) == 0x4);

struct StagedefStageModel
{
    uint8_t unk_0x0[4];
    char *modelName;
    uint8_t unk_0x8[8];
} __attribute__((__packed__));

static_assert(sizeof(StagedefStageModel) == 0x10);

struct StagedefCollisionHeader
{
    struct Vec3f centerOfRotation;
    struct Vec3s initialRotation;
    int16_t animLoopTypeAndSeesaw;
    struct StagedefAnimHeader *animationHeader;
    struct Vec3f conveyorSpeed;
    struct StagedefCollisionTri *collisionTriangleList;
    StagedefCollisionGridTilesTriIndices collisionTriangleListPtr;
    struct Vec2f collisionGridStart;
    struct Vec2f collisionGridStep;
    struct Vec2i collisionGridStepCount;
    uint32_t goalCount;
    struct StagedefGoal *goalList;
    uint32_t bumperCount;
    struct StagedefBumper *bumperList;
    uint32_t jamabarCount;
    struct StagedefJamabar *jamabarList;
    uint32_t bananaCount;
    struct StagedefBanana *bananaList;
    uint32_t coneCollisionObjectCount;
    struct StagedefConeCollision *coneCollisionObjectList;
    uint32_t sphereCollisionObjectCount;
    struct StagedefSphereCollision *sphereCollisionObjectList;
    uint32_t cylinderCollisionObjectCount;
    struct StagedefCylinderCollision *cylinderObjectCollisionList;
    uint32_t falloutVolumeCount;
    struct StagedefFalloutVolume *falloutVolumeList;
    uint32_t reflectiveStageModelCount;
    struct StagedefReflectiveStageModel *reflectiveStageModelList;
    uint32_t stageModelInstanceCount;
    struct StagedefStageModelInstance *stageModelInstanceList;
    uint32_t stageModelBCount;
    struct StagedefStageModelPtrB *stageModelBList;
    uint8_t unk_0x9c[8];
    uint16_t animGroupId; /* For use with switches - Must be non-zero if you want to assign a group ID */
    uint8_t padding[2];
    uint32_t switchCount;
    struct StagedefSwitch *switchList;
    uint8_t unk_0xb0[4];
    struct StagedefMystery5 *mystery5;
    float seesawSensitivity; /* Higher is more sensitive, negative makes the seesaw reversed */
    float seesawFriction; /* Lower is looser */
    float seesawSpring; /* 0 prevents the seesaw from resetting */
    uint32_t wormholeCount;
    struct StagedefWormhole *wormholeList;
    uint32_t initialPlaybackState; /* Should this be split into 2x padding bytes + PlaybackState enum? */
    uint8_t unk_0xd0[4];
    float animLoopPointSeconds;
    struct StagedefTextureScroll *textureScroll;
    uint8_t unk_0xdc[960];
} __attribute__((__packed__));

static_assert(sizeof(StagedefCollisionHeader) == 0x49c);

struct StagedefStageModelPtrA
{
    uint8_t unk_0x0[8];
    struct StagedefStageModel *stageModel;
} __attribute__((__packed__));

static_assert(sizeof(StagedefStageModelPtrA) == 0xc);

struct StagedefForegroundModel
{
    uint8_t unk_0x0[4]; /* Unknown - typically 0x0000001F, sometimes 0x00000007 or 0x0000000F */
    char *modelName;
    uint8_t unk_0x8[4];
    Vec3f position;
    Vec3s rotation;
    uint8_t padding[2];
    Vec3f scale;
    uint8_t unk_0x2c[4];
    struct BackgroundAnim2Header *backgroundAnim2Header;
    void *unk_0x34;
} __attribute__((__packed__));

static_assert(sizeof(StagedefForegroundModel) == 0x38);

struct StagedefStageModelPtrB
{
    struct StagedefStageModelPtrA *stageModelA;
} __attribute__((__packed__));

static_assert(sizeof(StagedefStageModelPtrB) == 0x4);

struct StagedefMystery5
{
    uint8_t unk_0x0[4];
    float unk_0x4;
    float unk_0x8;
    float unk_0xc;
    float unk_0x10;
} __attribute__((__packed__));

static_assert(sizeof(StagedefMystery5) == 0x14);

struct StagedefSphereCollision
{
    struct Vec3f position;
    float radius;
    uint8_t unk_0x10[4]; /* Nullable */
} __attribute__((__packed__));

static_assert(sizeof(StagedefSphereCollision) == 0x14);

struct StagedefFileHeader
{
    uint32_t magicNumberA;
    uint32_t magicNumberB;
    uint32_t collisionHeaderCount;
    struct StagedefCollisionHeader *collisionHeaderList;
    struct StagedefStart *start;
    struct StagedefFallout *fallout;
    uint32_t goalCount;
    struct StagedefGoal *goalList;
    uint32_t bumperCount;
    struct StagedefBumper *bumperList;
    uint32_t jamabarCount;
    struct StagedefJamabar *jamabarList;
    uint32_t bananaCount;
    struct StagedefBanana *bananaList;
    uint32_t coneCollisionObjectCount;
    struct StagedefConeCollision *coneCollisionObjectList;
    uint32_t sphereCollisionObjectCount;
    struct StagedefSphereCollision *sphereCollisionObjectList;
    uint32_t cylinderCollisionObjectCount;
    struct StagedefCylinderCollision *cylinderCollisionObjectList;
    uint32_t falloutVolumeCount;
    struct StagedefFalloutVolume *falloutVolumeList;
    uint32_t backgroundModelCount;
    struct StagedefBackgroundModel *backgroundModelList;
    uint32_t foregroundModelCount;
    struct StagedefForegroundModel *foregroundModelList;
    uint8_t unk_0x68[8];
    uint32_t reflectiveStageModelCount;
    struct StagedefReflectiveStageModel *reflectiveStageModelList;
    uint8_t unk_0x78[12];
    uint32_t stageModelInstanceCount;
    struct StagedefStageModelInstance *stageModelInstanceList;
    uint32_t stageModelACount;
    struct StagedefStageModelPtrA *stageModelAList;
    uint32_t stageModelBCount;
    struct StagedefStageModelPtrB *stageModelBList;
    uint8_t unk_0x9c[12];
    uint32_t switchCount;
    struct StagedefSwitch *switchList;
    struct StagedefFogAnimHeader *fogAnimationHeader; /* Nullable */
    uint32_t wormholeCount;
    struct StagedefWormhole *wormholeList;
    struct StagedefFog *fog; /* Nullable */
    uint8_t unk_0xc0[20];
    struct StagedefMystery3 *mystery3;
    uint8_t unk_0xd8[1988];
} __attribute__((__packed__));

static_assert(sizeof(StagedefFileHeader) == 0x89c);

struct StagedefJamabar
{
    struct Vec3f position;
    struct Vec3s rotation;
    uint8_t padding[2];
    struct Vec3f scale;
} __attribute__((__packed__));

static_assert(sizeof(StagedefJamabar) == 0x20);

struct StagedefMystery3
{
    float unk_0x0;
    float unk_0x4;
    float unk_0x8;
    uint8_t unk_0xc[24];
} __attribute__((__packed__));

static_assert(sizeof(StagedefMystery3) == 0x24);

struct StagedefFalloutVolume
{
    struct Vec3f position;
    struct Vec3f size;
    struct Vec3s rotation;
    uint8_t padding[2];
} __attribute__((__packed__));

static_assert(sizeof(StagedefFalloutVolume) == 0x20);

struct StagedefCollisionTri
{
    struct Vec3f point1Position;
    struct Vec3f normal;
    struct Vec3s rotationFromXy;
    uint8_t padding[2];
    struct Vec2f point2DeltaPosFromPoint1; /* Before rotation is applied */
    struct Vec2f point3DeltaPosFromPoint1;
    struct Vec2f tangent;
    struct Vec2f bitangent;
} __attribute__((__packed__));

static_assert(sizeof(StagedefCollisionTri) == 0x40);

struct StagedefGoal
{
    struct Vec3f position;
    struct Vec3s rotation;
    int16_t goalFlags;
} __attribute__((__packed__));

static_assert(sizeof(StagedefGoal) == 0x14);

struct StagedefWormhole
{
    uint8_t field_0x0;
    uint8_t field_0x1;
    uint8_t field_0x2;
    uint8_t field_0x3;
    struct Vec3f positon;
    struct Vec3s rotation;
    uint8_t padding[2];
    struct StagedefWormhole *destination;
} __attribute__((__packed__));

static_assert(sizeof(StagedefWormhole) == 0x1c);

struct StagedefCylinderCollision
{
    struct Vec3f position;
    float radius;
    float height;
    struct Vec3s rotation;
    uint8_t padding[2];
} __attribute__((__packed__));

static_assert(sizeof(StagedefCylinderCollision) == 0x1c);

struct StagedefStart
{
    struct Vec3f position;
    struct Vec3s rotation;
    uint8_t padding[2];
} __attribute__((__packed__));

static_assert(sizeof(StagedefStart) == 0x14);

struct StagedefSwitch
{
    struct Vec3f position;
    struct Vec3s rotation;
    uint16_t playbackState;
    uint16_t animGroupId;
    uint8_t padding[2];
} __attribute__((__packed__));

static_assert(sizeof(StagedefSwitch) == 0x18);

struct StagedefFog
{
    uint8_t type; /* Same as GXFogType */
    uint8_t padding[3];
    float fogStartDistance;
    float fogEndDistance;
    struct Vec3f color;
    uint8_t unk_0x18[12];
} __attribute__((__packed__));

static_assert(sizeof(StagedefFog) == 0x24);

}
