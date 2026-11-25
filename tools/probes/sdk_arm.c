// Probe: TWL-SDK C functions (ARM, -O4,p, -lang=c)
// probe: func_0212c1e8 0x0212c1e8
// probe: func_0212c874 0x0212c874
// probe: func_02133ca4 0x02133ca4
// probe: func_021346e0 0x021346e0
// probe: func_02137ca0 0x02137ca0
// probe: func_02137f4c 0x02137f4c

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef volatile u16 vu16;

typedef struct {
    u8 pad[0x20];
    u32 f20;
    u32 f24;
    u32 f28;
    u32 f2c;
    u32 f30;
    u32 f34;
    u32 f38;
} S2c1e8;

int func_0212c1e8(S2c1e8 *s) {
    u32 a  = s->f30;
    u32 b  = s->f34;
    u32 c  = s->f38;
    s->f24 = a;
    s->f2c = a;
    s->f28 = b;
    s->f20 = c;
    return 0;
}

typedef struct {
    u32 f00;
    u32 f04;
    u32 f08;
} Sub2c874;

typedef struct {
    u32 f00;
    Sub2c874 *sub;
} Obj2c874;

int func_0212c874(int a, Obj2c874 *o, u32 *out) {
    Sub2c874 *s = o->sub;
    *out        = s->f08 - s->f04;
    return 0;
}

typedef struct {
    int a;
    int b;
} S33ca4;

void func_02133ca4(S33ca4 *s) {
    vu16 *reg = (vu16 *) 0x04000204;
    s->a      = (*reg & 0xc) >> 2;
    s->b      = (*reg & 0x10) >> 4;
    *reg      = (u16) ((*reg & ~0xc) | 0xc);
    *reg &= ~0x10;
}

void *func_021346e0(void *dst, const void *src, u32 n) {
    u8 *d       = (u8 *) dst;
    const u8 *s = (const u8 *) src;
    while (n != 0) {
        *d++ = *s++;
        n--;
    }
    return dst;
}

u32 func_02137ca0(const u8 *s) {
    u32 n = -1;
    do {
        n++;
    } while (*s++);
    return n;
}

const char *func_02137f4c(const char *s, int ch) {
    u8 c = (u8) ch;
    u8 cur;
    while ((cur = (u8) *s++) != 0) {
        if (cur == c) {
            return s - 1;
        }
    }
    if (c != 0) {
        return 0;
    }
    return s - 1;
}
