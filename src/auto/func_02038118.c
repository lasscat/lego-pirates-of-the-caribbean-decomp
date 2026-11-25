#include "m2c.h"

typedef struct {
    u16 f00;
    u16 f02;
} Obj38118;

extern void *data_0216e964;
extern s32 func_02025800(void *obj);

u16 func_02038118(void *self, Obj38118 *obj, s32 flag) {
    u32 flags = 0;
    s32 match = obj->f02 == 0x76 || obj->f02 == 0x77;

    if (match || obj->f02 == 0xa7) {
        flags |= 0x10;
    }
    if (flag == 0 && (func_02025800(self) != 0 || self == data_0216e964)) {
        flags |= 0x20;
    }
    if (self == data_0216e964) {
        flags |= 0x82;
    } else {
        flags |= 1;
    }
    return (u16) flags;
}
