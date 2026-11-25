#include "m2c.h"

typedef struct {
    void (*f00)(void);
    void (*f04)(void);
    void (*f08)(void);
    void (*f0c)(void);
    void (*f10)(void);
    void (*f14)(void);
    void (*f18)(void);
    void (*f1c)(void);
    void (*f20)(void);
    void (*f24)(void);
} Defaults;

void func_0201c584(void);
void func_0201c748(void);
void func_0201c7a0(void);
void func_0201c7f4(void);
void func_0201ccec(void);
void func_0201cbec(void);
void func_0201ccdc(void);
void func_021036f8(void);
void func_0201cbdc(void);
void func_0201c9c0(void);
void func_0201c92c(void);
void func_0201cd04(void);
void func_0201c8b8(void);
void func_0201c9b0(void);
void func_0201c93c(void);
void func_020de330(u32 type, void *table);
void func_02102e1c(u32 a, void *b, void *c, void *d, void *e, u32 f);

void func_0201d0d0(void) {
    Defaults table = {0};
    table.f00      = func_0201c584;
    table.f08      = func_0201c7a0;
    table.f0c      = func_0201c7f4;
    table.f14      = 0;
    table.f10      = 0;
    table.f04      = func_0201c748;
    func_020de330(0x8f, &table);

    func_02102e1c(0x9e, func_0201cbec, func_0201ccdc, func_021036f8, func_0201ccec, 0);
    func_02102e1c(0x9f, 0, func_0201c9c0, 0, func_0201cbdc, 0);
    func_02102e1c(0xa0, func_0201cd04, func_0201c8b8, 0, func_0201c92c, 0);
    func_02102e1c(0xa1, func_0201cd04, func_0201c93c, 0, func_0201c9b0, 0);
}
