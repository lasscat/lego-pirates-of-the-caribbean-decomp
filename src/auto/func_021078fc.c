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

void func_021065a8(void);
void func_02106870(void);
void func_02106894(void);
void func_02106bb0(void);
void func_02106c44(void);
void func_021076a0(void);
void func_021076f4(void);
void func_020de330(u32 type, void *table);

void func_021078fc(void) {
    Defaults table = {0};
    table.f00      = func_02106894;
    table.f04      = func_02106bb0;
    table.f08      = func_02106c44;
    table.f0c      = func_021076a0;
    table.f14      = func_021076f4;
    table.f1c      = func_021065a8;
    table.f18      = func_02106870;
    func_020de330(0xc, &table);
}
