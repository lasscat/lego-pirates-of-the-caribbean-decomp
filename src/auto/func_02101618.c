#include "m2c.h"

typedef struct {
    u8 f00;
    u8 f01;
    u8 f02;
    u8 f03;
    u8 f04;
    u8 f05;
    u8 f06;
    u8 f07;
    u8 f08;
    u8 f09;
    u8 f0a;
    u8 f0b;
    u8 value;
    u8 f0d;
    u8 f0e;
    u8 f0f;
} Shade;

typedef struct {
    void *items[8];
    u8 f20;
    u8 f21;
    u8 f22;
    u8 f23;
    u8 f24;
    u8 f25;
    u8 f26;
    u8 f27;
    u8 f28;
    u8 f29;
    u8 f2a;
    u8 f2b;
    u8 f2c : 3;
    u8 f2d : 5;
} ObjectList;

typedef struct {
    u8 f00[0x12];
    u16 f12;
    u8 f14[0x20];
    ObjectList *f34;
    void *f38;
    u8 f3c[0x2c];
    Shade *f68;
} Object;

void func_020d04d0(void *a, void *b, s32 c, s32 d);
void func_020e0ebc(void *a);

void func_02101618(Object *self, s32 a) {
    Shade *shade = self->f68;
    u8 old       = shade->value;
    s32 value;

    if ((self->f12 & 0x800) != 0) {
        value = old - 5;
        if (value < 0) {
            if (a == 0) {
                shade->value = 0;
            } else {
                func_020e0ebc(self);
            }
        } else {
            shade->value = value;
        }
    } else {
        value = old + 5;
        if (value > 255) {
            shade->value = 255;
        } else {
            shade->value = value;
        }
    }

    if (old == shade->value) {
        return;
    }

    if (self->f34 != NULL) {
        u32 i;

        func_020d04d0(self->f34->items[0], shade->value, -1, 1);
        for (i = 0; i < self->f34->f2c; i++) {
            if (self->f34->items[i + 1] != self->f34->items[0]) {
                func_020d04d0(self->f34->items[i + 1], shade->value, -1, 1);
            }
        }
        return;
    }

    func_020d04d0(self->f38, shade->value, -1, 1);
}
