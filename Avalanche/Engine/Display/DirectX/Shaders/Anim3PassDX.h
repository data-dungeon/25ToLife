#if 0
    ps_1_1
    tex t0
    tex t1
    tex t2
    lrp r0.xyz, t1.w, t1, t0
    lrp r0.xyz, t2.w, t2, r0
    mul r0.xyz, v0, r0
    mov r0.w, t0.w

// approximately 7 instruction slots used (3 texture, 4 arithmetic)
#endif

const DWORD Anim3Pass[] =
{
    0xffff0101, 0x00000042, 0xb00f0000, 0x00000042, 0xb00f0001, 0x00000042, 
    0xb00f0002, 0x00000012, 0x80070000, 0xb0ff0001, 0xb0e40001, 0xb0e40000, 
    0x00000012, 0x80070000, 0xb0ff0002, 0xb0e40002, 0x80e40000, 0x00000005, 
    0x80070000, 0x90e40000, 0x80e40000, 0x00000001, 0x80080000, 0xb0ff0000, 
    0x0000ffff
};
