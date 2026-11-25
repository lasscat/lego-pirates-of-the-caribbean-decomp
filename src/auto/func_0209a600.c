#include "m2c.h"

typedef struct {
    void *f00;
} Inner9a600;

typedef struct {
    u32 f00;
    Inner9a600 *f04;
} Arg9a600;

typedef struct {
    u8 pad[0x24];
    void *f24;
    u8 pad2[0x18];
} Entry9a600;

extern void *data_0216e964;
extern Entry9a600 *data_02154b80;
extern u8 data_0215237a;
extern u8 data_0215237b;
extern u8 data_0215237c;
extern u8 data_02154da4[2];

s32 func_021107ec(void *a, Arg9a600 *b);
s32 func_02025800(void *p);
void func_0205af18(void *p);
void func_0209a4a8(void *a, Arg9a600 *b);
s32 func_020beed4(void *a, void *b);
s32 func_02090598(void);
s32 func_020905b0(s8 v);
void func_02097b78(void);
void func_02091104(s8 v);

s32 func_0209a600(void *a, Arg9a600 *b) {
    u32 i;

    func_021107ec(a, b);
    if (func_02025800(data_0216e964) != 0) {
        func_0205af18(data_0216e964);
    }
    func_0209a4a8(a, b);

    for (i = 0; i < 0x2b; i++) {
        if (func_020beed4(data_02154b80[i].f24, b->f04->f00) != 0) {
            if (!func_02090598()) {
                data_0215237a = 1;
            } else if (func_02090598() && func_020905b0((s8) i)) {
                func_02097b78();
            }
            data_0215237b = data_02154da4[0];
            data_0215237c = data_02154da4[1];
            func_02091104((s8) i);
            break;
        }
    }
    return 1;
}
