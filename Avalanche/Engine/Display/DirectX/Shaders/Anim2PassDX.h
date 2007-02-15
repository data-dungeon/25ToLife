#if 0
    ps_1_1
    tex t0
    tex t1
    lrp r0.xyz, t1.w, t1, t0
    mov r0.w, t0.w
    mul r0, v0, r0

// approximately 5 instruction slots used (2 texture, 3 arithmetic)
#endif

const DWORD Anim2Pass[] =
{
    0xffff0101, 0x00000042, 0xb00f0000, 0x00000042, 0xb00f0001, 0x00000012, 
    0x80070000, 0xb0ff0001, 0xb0e40001, 0xb0e40000, 0x00000001, 0x80080000, 
    0xb0ff0000, 0x00000005, 0x800f0000, 0x90e40000, 0x80e40000, 0x0000ffff
};
