#include "m2c.h"

typedef void (*Fn)(void);

typedef struct {
    Fn f00;
    Fn f04;
    Fn f08;
    Fn f0c;
    u32 f10;
    Fn f14;
    u32 f18;
    Fn f1c;
    u32 f20;
    u32 f24;
} ClassInfo;

extern void GOMarbleRope_Create(void);
extern void func_02053eac(void);
extern void func_02100b94(void);
extern void func_02054504(void);
extern void func_02054528(void);
extern void func_02053f44(void);
extern void func_020de330(int, ClassInfo *);

void func_02054570(void) {
    ClassInfo info = {0};
    info.f00       = GOMarbleRope_Create;
    info.f04       = func_02053eac;
    info.f08       = func_02100b94;
    info.f0c       = func_02054504;
    info.f14       = func_02054528;
    info.f1c       = func_02053f44;
    func_020de330(0xa5, &info);
}
