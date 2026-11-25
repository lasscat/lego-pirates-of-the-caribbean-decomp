#include "m2c.h"

typedef struct {
    u8 pad[0x40];
    void *f40;
    void *f44;
} Cutscene;

extern Cutscene data_0215238c;
extern void *data_02152424[6];
extern void *data_0215243c[6];

void func_020c4a2c(void *p);
void func_020b93e4(void *p);
void func_020a1a70(s32 value);
void func_02078f88(void);

void func_0207aaf8(void) {
    u32 i = 0;

    if (data_0215238c.f40 != NULL) {
        func_020c4a2c(data_0215238c.f40);
    }
    if (data_0215238c.f44 != NULL) {
        func_020c4a2c(data_0215238c.f44);
    }

    func_020a1a70(0);

    for (i = 0; i < 6; i++) {
        if (data_02152424[i] != NULL) {
            func_020b93e4(data_02152424[i]);
            data_02152424[i] = NULL;
        }
        if (data_0215243c[i] != NULL) {
            func_020b93e4(data_0215243c[i]);
            data_0215243c[i] = NULL;
        }
    }

    func_02078f88();
}
