#include "m2c.h"

extern u32 data_021658b8[];
extern u8 data_0214b55c[];
extern u32 data_0214b544[];

void func_02137cbc(u8 *, u32);
void func_02137dd4(u8 *, u8 *);
s32 func_0213c108(u8 *, s32);

s32 func_020e63b4(s32 a) {
    u8 b[64];
    u16 i;
    for (i = 1; i < data_021658b8[7]; i++) {
        func_02137cbc(b, ((u32 *) data_0214b544[1])[i]);
        func_02137dd4(b, data_0214b55c);
        if (func_0213c108(b, a) == 0) {
            return i;
        }
    }
    return 0;
}
