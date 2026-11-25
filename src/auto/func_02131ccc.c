#include "m2c.h"

typedef struct {
    u32 f00;
    u32 f04;
    u32 f08;
    u32 f0c;
} Obj31ccc;

extern Obj31ccc data_02170f2c;
extern void func_02131d44(void);

void func_02131ccc(void) {
    u32 *slot = (u32 *) 0x02fffd9c;
    u32 valid = 0;
    u32 value = *slot;

    data_02170f2c.f08 = value;
    if (value >= 0x2600000 && value < 0x2800000) {
        valid = 1;
    }
    if (valid == 0) {
        value = 0;
    }
    data_02170f2c.f00 = value;
    if (value == 0) {
        *slot                               = (u32) func_02131d44;
        *((u32 *) ((u32) slot & ~0x800000)) = (u32) func_02131d44;
    }
    data_02170f2c.f0c = 0;
}
