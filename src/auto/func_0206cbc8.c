#include "m2c.h"

typedef void (*Handler)(void);

typedef struct {
    Handler f00;
    Handler f04;
    Handler f08;
    Handler f0c;
    Handler f10;
    Handler f14;
    Handler f18;
    Handler f1c;
    Handler f20;
    Handler f24;
} HandlerTable;

extern void func_020de330(s32 idx, HandlerTable *t);
extern void func_02102e1c(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);

extern void func_0206c478(void);
extern void func_0206c68c(void);
extern void func_0206c964(void);
extern void func_0206c704(void);
extern void func_0206c988(void);
extern void func_0206c6e0(void);
extern void func_0206ca34(void);
extern void func_0206ca48(void);
extern void func_0206cbc4(void);
extern void func_0206cad4(void);

void func_0206cbc8(void) {
    HandlerTable t = {0};
    t.f00          = func_0206c478;
    t.f04          = func_0206c68c;
    t.f1c          = func_0206c6e0;
    t.f0c          = func_0206c704;
    t.f08          = func_0206c964;
    t.f14          = func_0206c988;
    func_020de330(0x85, &t);
    func_02102e1c(0x5b, (u32) func_0206ca34, (u32) func_0206ca48, (u32) func_0206cad4, (u32) func_0206cbc4, 0);
}
