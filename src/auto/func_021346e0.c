#include "m2c.h"

void *func_021346e0(void *dst, const void *src, u32 n) {
    u8 *d       = (u8 *) dst;
    const u8 *s = (const u8 *) src;
    while (n != 0) {
        *d++ = *s++;
        n--;
    }
    return dst;
}
