#include "m2c.h"

typedef struct Obj Obj;

typedef s32 (*SubFn)(Obj *o, s32 a, s32 b);

struct Obj {
    u8 f00;
    u8 pad[3];
    SubFn sub;
};

void func_020faec8(Obj *o) {
    s32 r;
    if (o->f00 == 1) {
        r = 1;
        if (o->sub != 0) {
            r = o->sub(o, 1, 0);
        }
        if (r != 0) {
            o->f00 = 2;
        }
    } else if (o->f00 == 3) {
        r = 1;
        if (o->sub != 0) {
            r = o->sub(o, 0, 0);
        }
        if (r != 0) {
            o->f00 = 0;
        }
    }
}
