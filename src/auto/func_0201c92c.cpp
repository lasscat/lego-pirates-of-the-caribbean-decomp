#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    u16 f10;
} S1c92c;

extern "C" void func_0201c92c(S1c92c *s) {
    s->f10 &= ~0x100;
}
