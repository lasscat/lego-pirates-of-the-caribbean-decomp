#include "m2c.h"

extern void func_020c84d0(void *);
extern void func_020c84b0(void *, void *, void *);

typedef struct {
    u8 pad[0xc];
    u8 f0c;
    u8 pad2[3];
    u8 f10[0x20];
    u32 f30;
    u32 f34;
} Sub594c;

typedef struct {
    u8 pad[8];
    Sub594c *sub;
} Obj594c;

typedef struct {
    u8 pad[0x320];
    Obj594c *obj;
} Glob594c;

extern Glob594c data_021507c8;
extern u8 data_02150adc[];

void *func_0200594c(u32 a0, u32 a1, u8 a2, void *a3) {
    Obj594c *o = data_021507c8.obj;
    Sub594c *s;
    if (o == 0) {
        return 0;
    }
    s = o->sub;
    func_020c84d0(o);
    func_020c84b0(&data_02150adc, o, s);
    s->f30 = a0;
    s->f34 = a1;
    s->f0c = a2;
    func_021346e0(&s->f10, a3, 0x20);
    return s;
}
