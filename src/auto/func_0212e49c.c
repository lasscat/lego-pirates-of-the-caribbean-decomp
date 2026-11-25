#include "m2c.h"

extern s32 data_0214dfd4;

s32 func_0212dc6c(void);
void func_0212f450(s32, s32, s32, s32, s32);
void func_0212f99c(s32, s32, s32);

void func_0212e49c(s32 a, s32 b, u32 c) {
    s32 r = func_0212dc6c();
    if (data_0214dfd4 != -1 && c > 0x30) {
        func_0212f450(data_0214dfd4, a, r + b, c, 1);
    } else {
        func_0212f99c(a, r + b, c);
    }
}
