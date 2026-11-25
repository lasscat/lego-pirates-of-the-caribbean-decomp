#include "m2c.h"

typedef struct {
    u8 pad00[0x38];
    void *f38;
} Frame206bea0;

typedef struct {
    u8 pad00[0x20];
    Frame206bea0 *frames[5];
    s32 direction;
    s32 frame;
    u8 changed;
    u8 pad3d[3];
    u8 data[0x0c];
    s32 repeat;
} Sub206bea0;

typedef struct {
    u8 pad00[0x68];
    Sub206bea0 *sub;
} Obj206bea0;

u8 *func_020d01c0(void *value);
void func_020b1a88(void *dst, void *src);

void func_0206bea0(void *obj) {
    Sub206bea0 *sub = ((Obj206bea0 *) obj)->sub;
    s32 last        = 4;
    s32 i;
    s32 frame = sub->frame;

    if (frame < 0) {
        frame = 0;
    }

    i = 0;
    do {
        Frame206bea0 *entry = sub->frames[frame];
        if (entry != NULL) {
            func_020b1a88(&sub->data, func_020d01c0(entry->f38) + 0x30);
            sub->changed = 1;
            break;
        }

        frame += sub->direction;
        if (frame < 0) {
            frame = last;
        }
        if (frame == 5) {
            frame = 0;
        }
    } while (i++ < 5);

    frame += sub->direction;
    if (frame == 5) {
        if (sub->repeat != 0) {
            sub->direction = -sub->direction;
            frame += sub->direction * 2;
        } else {
            frame = 0;
        }
    } else if (frame < 0) {
        if (sub->repeat != 0) {
            sub->direction = -sub->direction;
            frame += sub->direction * 2;
        } else {
            frame = last;
        }
    }

    sub->frame = frame;
}
