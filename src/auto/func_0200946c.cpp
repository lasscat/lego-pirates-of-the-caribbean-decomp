#include "m2c.h"

class Obj946c {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3(s32 arg);
    u8 pad[0x21];
    u8 flag;
};

extern "C" {

Obj946c *func_02009960(void);

void func_0200946c(void) {
    Obj946c *o = func_02009960();
    if (o != (void *) 0 && o->flag != 0) {
        o->v3(1);
        o->flag = 0;
    }
}
}
