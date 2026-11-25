#include "m2c.h"

typedef struct {
    u8 f00[0x68];
    void *f68;
} Root3c858;

typedef struct {
    u8 f00[0x1fc];
    void *f1fc;
} Manager3c858;

typedef struct {
    u8 f00[0x1a9];
    u8 f1a9;
} State3c858;

extern void *data_0216e95c;
void func_02096188(s32);
void func_020962cc(s32);
s32 func_02101edc(void);

s32 func_0203c858(s32 a, s32 action) {
    Root3c858 *root;
    Manager3c858 *manager;
    State3c858 *state;

    (void) a;
    if (action != 6) {
        if (action == 22) {
            return func_02101edc();
        }
        goto done;
    }
    root    = data_0216e95c;
    manager = root->f68;
    state   = manager->f1fc;
    if (((u32) state->f1a9 << 24) >> 31) {
        func_02096188(0);
        func_020962cc(0);
        state->f1a9 &= ~0x80;
    }
done:
    return 0;
}
