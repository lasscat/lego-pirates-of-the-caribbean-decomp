// Probe: TWL-SDK Thumb syscall stubs (0x0200007c..0x020007a8, -thumb)
// probe: WaitByLoop 0x0200007c thumb
// probe: VBlankIntrWait 0x020002e4 thumb
// probe: Mod 0x02000728 thumb

asm void WaitByLoop(int count) {
    swi 3 bx lr
}

asm void VBlankIntrWait(void) {
    mov r2, #0 swi 5 bx lr
}

asm int Mod(int numer, int denom) {
    swi 9 add r0, r1, #0 bx lr
}
