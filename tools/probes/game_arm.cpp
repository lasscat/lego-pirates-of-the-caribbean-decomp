// Probe: game/engine C++ functions across .text (ARM, -O4,p, -lang=c++)
// probe: func_0200946c 0x0200946c
// probe: func_0201c92c 0x0201c92c
// probe: func_02020e94 0x02020e94
// probe: func_0202564c 0x0202564c
// probe: func_0202c670 0x0202c670
// probe: func_020306c0 0x020306c0
// probe: func_02061f9c 0x02061f9c
// probe: func_020654c0 0x020654c0
// probe: func_020c0304 0x020c0304
// probe: func_020d04b8 0x020d04b8
// probe: func_020d4ba8 0x020d4ba8
// probe: func_02126c34 0x02126c34
// probe: func_02126c64 0x02126c64
// probe: func_02126cb4 0x02126cb4
// probe: func_02126cf0 0x02126cf0

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;

class Obj946c {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3(int);
    u8 pad[0x21];
    u8 flag;
};

struct S1c92c {
    u8 pad[0x10];
    u16 f10;
    u16 f12;
};

struct Mid61f9c {
    u8 pad[0x38];
    S1c92c *sub;
};

struct Obj61f9c {
    u8 pad[0x68];
    Mid61f9c *m;
};

struct Sub2564c {
    u8 pad0[0xc];
    u16 f0c;
    u8 f0e;
    u8 f0f;
    u8 pad10[2];
    u8 f12;
};

struct Mid2564c {
    u8 pad0[8];
    Sub2564c *sub;
    u8 padc[2];
    u16 f0e;
    u8 pad10[4];
    u16 f14;
    u16 f16;
    u8 f18;
};

struct Obj2564c {
    u8 pad0[0x68];
    Mid2564c *m;
};

struct DstD4ba8 {
    u32 f00;
    u32 f04;
    u32 f08;
    u32 f0c;
    u32 f10;
};

struct SrcD4ba8 {
    u16 f00;
    u16 f02;
    u32 f04;
    u8 pad8[6];
    u16 f0e;
};

struct Ring {
    u8 *buf;
    u16 stride;
    u8 count;
    u8 cur;
};

extern "C" {

void *func_02009960(void);
int func_02113fb4(void *);
void *Mem_Alloc(u32 size, int a, int b);
void Mem_Free(void *p);

void func_0200946c(void) {
    Obj946c *o = (Obj946c *) func_02009960();
    if (o != 0 && o->flag != 0) {
        o->v3(1);
        o->flag = 0;
    }
}

void func_0201c92c(S1c92c *s) {
    s->f10 &= ~0x100;
}

void func_02020e94(void *a, void *b) {
    void **p = *(void ***) ((u8 *) b + 0x134);
    if (p != 0) {
        void **q = (void **) p[0x38 / 4];
        if (q != 0 && q[1] != *(void **) ((u8 *) a + 0x38)) {
            *(void **) ((u8 *) b + 0x134) = 0;
        }
    }
}

void func_0202564c(Obj2564c *o) {
    Mid2564c *m = o->m;
    Sub2564c *s = m->sub;
    m->f0e      = s->f0c;
    m->f14      = s->f0e;
    m->f16      = s->f0f;
    m->f18      = s->f12;
}

s8 func_0202c670(s8 *p) {
    return (s8) (0x1c - p[0x168]);
}

void func_020306c0(S1c92c *s) {
    s->f10 &= ~0x100;
    s->f12 &= ~0x100;
}

void func_02061f9c(Obj61f9c *o) {
    Mid61f9c *m = o->m;
    S1c92c *s   = m->sub;
    if (s != 0) {
        s->f10 &= ~0x4;
    }
    m->sub = 0;
}

void func_020654c0(S1c92c *s) {
    if (func_02113fb4(s) != 0) {
        s->f10 &= ~0x200;
    }
}

int func_020c0304(const char *s) {
    int n = 0;
    while (*s != 0 && *s != '.') {
        n = n * 10 + *s - '0';
        s++;
    }
    return n;
}

void func_020d04b8(u16 *p, int clr) {
    if (clr != 0) {
        *p &= ~3;
    } else {
        *p |= 3;
    }
}

void func_020d4ba8(DstD4ba8 *d, SrcD4ba8 *s) {
    d->f00 = 0;
    d->f08 = s->f04;
    d->f10 = s->f0e;
    if (s->f00 == 1) {
        d->f00 |= 1;
    }
    d->f04 = s->f02;
}

u32 func_02126c34(u32 *arr, u32 n) {
    u32 max = 0;
    u32 i;
    for (i = 0; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

Ring *func_02126c64(u16 stride, u8 count) {
    Ring *r   = (Ring *) Mem_Alloc(8, 1, 1);
    r->stride = stride;
    r->count  = count;
    r->cur    = 0;
    r->buf    = (u8 *) Mem_Alloc(stride * count, 1, 1);
    return r;
}

void *func_02126cb4(Ring *r) {
    void *p = r->buf + r->stride * r->cur;
    r->cur++;
    if (r->cur >= r->count - 1) {
        r->cur = 0;
    }
    return p;
}

void func_02126cf0(Ring *r) {
    Mem_Free(r->buf);
    Mem_Free(r);
}
}
