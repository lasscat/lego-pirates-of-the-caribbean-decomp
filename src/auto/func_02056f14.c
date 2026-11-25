#include "m2c.h"

extern u8 data_0216f77c;
extern const char data_02144ddc[];
extern const char data_02144df0[];

void func_021007d0(void *o, s32 a, s32 b, s32 c, s32 d, u8 e);
s32 func_020e000c(void *o, const char *s, s32 a);
s32 func_0210209c(void *o);

typedef struct {
    u8 pad[0x10];
    u16 f10;
} Obj;

void func_02056f14(Obj *o) {
    func_021007d0(o, 1, 1, 0, 1, data_0216f77c);
    if (func_020e000c(o, data_02144ddc, 0)) {
        if (func_0210209c(o)) {
            o->f10 &= ~0x200;
        }
    } else {
        if (func_020e000c(o, data_02144df0, 0)) {
            o->f10 &= ~0x200;
        }
    }
}
