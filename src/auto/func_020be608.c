#include "m2c.h"

extern s32 func_020be44c(char *dst, const char *src, s32 flag1, s32 flag2);
extern char *func_020bd6bc(void);
extern s32 func_02137ca0(const char *s);
extern void *func_02134700(void *dst, const void *src, u32 n);
extern char *func_02137fc4(const char *s, const char *sub);
extern char *data_02149e4c[];
extern const char data_02149e74[];

void func_020be608(char *a, char *b, s32 idx) {
    char *m;
    char *s = b;
    char *p;
    char *sep;
    b += func_020be44c(b, data_02149e4c[idx], 1, 1);
    b += func_020be44c(b, func_020bd6bc(), 1, 1);
    func_020be44c(b, a, 1, 1);

    while (*s == '/') {
        func_02134700(s, s + 1, func_02137ca0(s));
    }

    sep = (char *) data_02149e74;
    m   = func_02137fc4(s, sep);
    if (m != 0) {
        while (m != 0) {
            p = m - 1;
            while (*p != '/' && p > s) {
                p--;
            }
            func_02134700(p, m + 3, func_02137ca0(m + 3) + 1);
            m = func_02137fc4(p, sep);
        }
    }
}
