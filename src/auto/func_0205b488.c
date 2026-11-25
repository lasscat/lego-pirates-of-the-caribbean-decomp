#include "m2c.h"

typedef struct {
    u8 f00[0x38];
    void *f38;
    u8 f3c[0xb4];
} Object5b488;

extern s32 data_0214500c[];
extern s32 data_0215f704[];

s32 func_0201279c(Object5b488 *self);
void func_020cfd20(void *p);
void func_020dd004(void *p);
void func_020de580(Object5b488 *self, s32 a, s32 b);
void *fnObject_Create(s32 *a, s32 b, s32 size);
void func_020d04b8(void *p, s32 a);

void func_0205b488(Object5b488 *self) {
    if (func_0201279c(self) != 0) {
        if (self->f38 != NULL) {
            func_020cfd20(self->f38);
            func_020dd004(&self->f3c);
        }
        func_020de580(self, 0, 0);
        func_020d04b8(self->f38, 0);
    } else {
        if (self == NULL) {
            return;
        }
        if (self->f38 != NULL) {
            func_020cfd20(self->f38);
        }
        self->f38 = fnObject_Create(data_0214500c, data_0215f704[0], 0xb4);
        func_020d04b8(self->f38, 0);
    }
}
