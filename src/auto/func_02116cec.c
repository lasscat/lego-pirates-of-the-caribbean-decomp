#include "m2c.h"

typedef struct {
    u8 pad00[0x10];
    u16 f10;
    u16 f12;
    u8 pad14[0x24];
    void *f38;
    u8 pad3c[0x2c];
    void *f68;
} Obj216cec;

typedef struct {
    u8 pad00[0x26];
    u8 f26;
    u8 pad27[0x7d];
    void *fa4;
} Sub216cec;

extern u8 data_0216f77c;
extern u32 data_0214d310;
extern u32 data_0214d448;

void func_020d0194(void *a, void *b);
void func_021007d0(Obj216cec *a, s32 b, s32 c, s32 d, s32 e, u8 f);
s32 func_020e000c(Obj216cec *a, void *b, s32 c);
s32 func_0210209c(Obj216cec *a);

void func_02116cec(Obj216cec *self) {
    Sub216cec *sub = self->f68;
    func_020d0194(self->f38, &sub->fa4);
    func_021007d0(self, 1, 0, 1, 1, data_0216f77c);
    if (func_020e000c(self, &data_0214d448, 0)) {
        if (func_0210209c(self)) {
            self->f10 &= ~0x200;
        }
    }
    sub->f26 = func_020e000c(self, &data_0214d310, 1);
    self->f12 &= ~0x10;
}
