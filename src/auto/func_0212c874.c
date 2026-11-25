#include "m2c.h"

typedef struct {
    u32 f00;
    u32 f04;
    u32 f08;
} Sub2c874;

typedef struct {
    u32 f00;
    Sub2c874 *sub;
} Obj2c874;

s32 func_0212c874(s32 a, Obj2c874 *o, u32 *out) {
    Sub2c874 *s = o->sub;
    (void) a;
    *out = s->f08 - s->f04;
    return 0;
}
