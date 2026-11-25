#include "m2c.h"

typedef struct {
    u8 f00;
    u8 f01;
    u8 f02;
    u8 f03;
    u8 flag : 1;
} S_de04;

extern S_de04 data_02151efc;
extern u8 data_0216f781;
extern u16 data_0215dd98;

s32 func_02090598(void);
void func_0208d3ec(void);
void func_0208d8d0(s32);

void func_0208de04(void) {
    data_0216f781 = data_02151efc.flag != 0;
    if (func_02090598() == 0) {
        func_0208d3ec();
        return;
    }
    data_0215dd98 = 1;
    func_0208d8d0(0);
    data_0215dd98 = 0;
}
