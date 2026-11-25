#include "m2c.h"

typedef struct S2 S2;

typedef struct {
    u8 pad[0x10];
    u16 f10;
    u8 pad2[0x38 - 0x12];
    void *f38;
    u8 pad3[0x68 - 0x3c];
    S2 *f68;
} S1;

struct S2 {
    u8 pad[0x130];
    S1 *f130;
    u8 pad2[0x191 - 0x134];
    u8 f191;
    u8 pad3[0x250 - 0x192];
    u32 f250;
    u32 f254;
    u8 pad4[0x280 - 0x258];
    u32 f280;
    u8 pad5[0x2b4 - 0x284];
    u8 f2b4;
};

extern S1 *data_0216e964;
extern S1 *data_0216e960;
extern S1 *data_0216e95c;
extern u32 data_0216f7a0;

extern s32 func_020257c8(S1 *);
extern u8 *func_020d01c0(void *);

void func_0205f908(S1 *a, S1 *b) {
    S2 *s   = a->f68;
    s->f250 = 0;
    s->f254 = 0;
    if (func_020257c8(b) == 0) {
        return;
    }
    S2 *t   = b->f68;
    t->f130 = a;
    t->f191 |= 0x10;
    a->f10 |= 4;
    if (b == data_0216e964) {
        s->f254       = (u32) data_0216e964;
        data_0216e964 = a;
    } else if (b == data_0216e960) {
        s->f254       = (u32) data_0216e960;
        data_0216e960 = a;
    }
    if (b == data_0216e95c) {
        s->f250       = (u32) data_0216e95c;
        data_0216e95c = a;
    }
    s->f2b4       = (s->f2b4 & ~1) | 1;
    data_0216f7a0 = (u32) (func_020d01c0(data_0216e95c->f38) + 0x30);
    s->f280       = 0;
}
