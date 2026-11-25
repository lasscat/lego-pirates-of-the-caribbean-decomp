#include "m2c.h"

typedef struct {
    u8 pad00[0x10];
    u32 f10;
    u32 f14;
    u32 f18;
    u32 f1c;
} Action61ea8;

typedef struct {
    u8 pad00[0x30];
    Action61ea8 *f30;
    u8 pad34[0x44 - 0x34];
    void *f44;
    u8 pad48[0x62 - 0x48];
    u8 f62;
} State61ea8;

typedef struct {
    u8 pad00[0x38];
    void *f38;
    u8 pad3c[0x68 - 0x3c];
    State61ea8 *f68;
} Obj61ea8;

extern void *data_0216e95c;

void *func_020d01c0(void *);
s32 func_020d8b2c(void *, void *, void *, void *, s32, s32, s32);
s32 func_0211c938(void *, void *);

void func_02061ea8(Obj61ea8 *self) {
    State61ea8 *state = self->f68;
    if (!state->f62) {
        return;
    }
    Action61ea8 *action = state->f30;
    void *result        = func_020d01c0(self->f38);
    if (func_020d8b2c(result, &action->f10, &action->f1c, data_0216e95c, 0, 0, 0)) {
        return;
    }
    state->f62 = 0;
    if (!state->f44) {
        return;
    }
    func_0211c938(state->f44, self);
}
