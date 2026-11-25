#include "m2c.h"

typedef struct {
    u8 pad[0x18];
    void *f18;
} Keyframe;

extern "C" {

Keyframe *fnFileparser_DbgLoadBinaryBlockAligned(void *file, s32 a, s32 b);

Keyframe *fnFlash_BuildKeyframeFromBinary(void *file) {
    Keyframe *kf = fnFileparser_DbgLoadBinaryBlockAligned(file, 0, 1);
    kf->f18      = (void *) 0;
    return kf;
}
}
