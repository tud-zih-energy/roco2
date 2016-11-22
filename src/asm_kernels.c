#include <roco2/kernels/asm_kernels.h>

#include <stdint.h>
#include <assert.h>

uint64_t mulpd_kernel(double *buffer, uint64_t repeat)
{
    unsigned long long passes,addr;
    unsigned long long a,b,c,d;
    uint64_t ret=0;

    passes=repeat/32; // 32 128-Bit accesses in inner loop

    addr=(unsigned long long) buffer;

    if (!passes) return ret;
    /*
     * Input:  RAX: addr (pointer to the buffer)
     *         RBX: passes (number of iterations)
     *         RCX: length (total number of accesses)
     */
    __asm__ __volatile__(
        "mov %%rax,%%r9;"   // addr
        "mov %%rbx,%%r10;"  // passes

        //initialize registers
        "movaps 0(%%r9), %%xmm0;"
        "movaps 16(%%r9), %%xmm1;"
        "movaps 32(%%r9), %%xmm2;"
        "movaps 48(%%r9), %%xmm3;"
        "movaps 64(%%r9), %%xmm4;"
        "movaps 80(%%r9), %%xmm5;"
        "movaps 96(%%r9), %%xmm6;"
        "movaps 112(%%r9), %%xmm7;"
        "movapd 0(%%r9), %%xmm8;"
        "movapd 16(%%r9), %%xmm9;"
        "movapd 32(%%r9), %%xmm10;"
        "movapd 48(%%r9), %%xmm11;"
        "movapd 64(%%r9), %%xmm12;"
        "movapd 80(%%r9), %%xmm13;"
        "movapd 96(%%r9), %%xmm14;"
        "movapd 112(%%r9), %%xmm15;"

        ".align 64;"
        "_work_loop_mul_pd:"
        "mulpd %%xmm8, %%xmm0;"
        "mulpd %%xmm9, %%xmm1;"
        "mulpd %%xmm10, %%xmm2;"
        "mulpd %%xmm11, %%xmm3;"
        "mulpd %%xmm12, %%xmm4;"
        "mulpd %%xmm13, %%xmm5;"
        "mulpd %%xmm14, %%xmm6;"
        "mulpd %%xmm15, %%xmm7;"

        "mulpd %%xmm8, %%xmm0;"
        "mulpd %%xmm9, %%xmm1;"
        "mulpd %%xmm10, %%xmm2;"
        "mulpd %%xmm11, %%xmm3;"
        "mulpd %%xmm12, %%xmm4;"
        "mulpd %%xmm13, %%xmm5;"
        "mulpd %%xmm14, %%xmm6;"
        "mulpd %%xmm15, %%xmm7;"

        "mulpd %%xmm8, %%xmm0;"
        "mulpd %%xmm9, %%xmm1;"
        "mulpd %%xmm10, %%xmm2;"
        "mulpd %%xmm11, %%xmm3;"
        "mulpd %%xmm12, %%xmm4;"
        "mulpd %%xmm13, %%xmm5;"
        "mulpd %%xmm14, %%xmm6;"
        "mulpd %%xmm15, %%xmm7;"

        "mulpd %%xmm8, %%xmm0;"
        "mulpd %%xmm9, %%xmm1;"
        "mulpd %%xmm10, %%xmm2;"
        "mulpd %%xmm11, %%xmm3;"
        "mulpd %%xmm12, %%xmm4;"
        "mulpd %%xmm13, %%xmm5;"
        "mulpd %%xmm14, %%xmm6;"
        "mulpd %%xmm15, %%xmm7;"

        "sub $1,%%r10;"
        "jnz _work_loop_mul_pd;"

        : "=a" (a), "=b" (b), "=c" (c), "=d" (d)
        : "a"(addr), "b" (passes)
        : "%r9", "%r10", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"

    );
    ret=passes;

    return ret;
}

uint64_t addpd_kernel(double *buffer, uint64_t repeat)
{
    unsigned long long passes,addr;
    unsigned long long a,b,c,d;
    uint64_t ret=0;

    passes=repeat/32; // 32 128-Bit accesses in inner loop

    addr=(unsigned long long) buffer;

    if (!passes) return ret;
    /*
     * Input:  RAX: addr (pointer to the buffer)
     *         RBX: passes (number of iterations)
     *         RCX: length (total number of accesses)
     */
    __asm__ __volatile__(
        "mov %%rax,%%r9;"   // addr
        "mov %%rbx,%%r10;"  // passes

        //initialize registers
        "movaps 0(%%r9), %%xmm0;"
        "movaps 16(%%r9), %%xmm1;"
        "movaps 32(%%r9), %%xmm2;"
        "movaps 48(%%r9), %%xmm3;"
        "movaps 64(%%r9), %%xmm4;"
        "movaps 80(%%r9), %%xmm5;"
        "movaps 96(%%r9), %%xmm6;"
        "movaps 112(%%r9), %%xmm7;"
        "movapd 0(%%r9), %%xmm8;"
        "movapd 16(%%r9), %%xmm9;"
        "movapd 32(%%r9), %%xmm10;"
        "movapd 48(%%r9), %%xmm11;"
        "movapd 64(%%r9), %%xmm12;"
        "movapd 80(%%r9), %%xmm13;"
        "movapd 96(%%r9), %%xmm14;"
        "movapd 112(%%r9), %%xmm15;"

        ".align 64;"
        "_work_loop_add_pd:"
        "addpd %%xmm8, %%xmm0;"
        "addpd %%xmm9, %%xmm1;"
        "addpd %%xmm10, %%xmm2;"
        "addpd %%xmm11, %%xmm3;"
        "addpd %%xmm12, %%xmm4;"
        "addpd %%xmm13, %%xmm5;"
        "addpd %%xmm14, %%xmm6;"
        "addpd %%xmm15, %%xmm7;"

        "addpd %%xmm8, %%xmm0;"
        "addpd %%xmm9, %%xmm1;"
        "addpd %%xmm10, %%xmm2;"
        "addpd %%xmm11, %%xmm3;"
        "addpd %%xmm12, %%xmm4;"
        "addpd %%xmm13, %%xmm5;"
        "addpd %%xmm14, %%xmm6;"
        "addpd %%xmm15, %%xmm7;"

        "addpd %%xmm8, %%xmm0;"
        "addpd %%xmm9, %%xmm1;"
        "addpd %%xmm10, %%xmm2;"
        "addpd %%xmm11, %%xmm3;"
        "addpd %%xmm12, %%xmm4;"
        "addpd %%xmm13, %%xmm5;"
        "addpd %%xmm14, %%xmm6;"
        "addpd %%xmm15, %%xmm7;"

        "addpd %%xmm8, %%xmm0;"
        "addpd %%xmm9, %%xmm1;"
        "addpd %%xmm10, %%xmm2;"
        "addpd %%xmm11, %%xmm3;"
        "addpd %%xmm12, %%xmm4;"
        "addpd %%xmm13, %%xmm5;"
        "addpd %%xmm14, %%xmm6;"
        "addpd %%xmm15, %%xmm7;"

        "sub $1,%%r10;"
        "jnz _work_loop_add_pd;"

        : "=a" (a), "=b" (b), "=c" (c), "=d" (d)
        : "a"(addr), "b" (passes)
        : "%r9", "%r10", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"

    );
    ret=passes;

    return ret;
}

uint64_t sqrtss_kernel(float *buffer, uint64_t elems, uint64_t repeat)
{
    unsigned long long passes,length,addr;
    unsigned long long a,b,c,d;
    uint64_t ret=0;
    #ifdef REGONLY
    assert(elems >= 256/sizeof(*buffer));
    #endif

    passes=elems/64; // 32 128-Bit accesses in inner loop
    length=passes*32*repeat;
    addr=(unsigned long long) buffer;

    if (!passes) return ret;
    /*
     * Input:  RAX: addr (pointer to the buffer)
     *         RBX: passes (number of iterations)
     *         RCX: length (total number of accesses)
     */
    __asm__ __volatile__(
        "mfence;"
        "mov %%rax,%%r9;"   // addr
        "mov %%rbx,%%r10;"  // passes
        "mov %%rcx,%%r15;"  // length
        "mov %%r9,%%r14;"   // store addr
        "mov %%r10,%%r8;"   // store passes
        "mov %%r15,%%r13;"  // store length

        //initialize registers
        "movaps 0(%%r9), %%xmm0;"
        #ifdef REGONLY
        "movaps 0(%%r9), %%xmm8;"
        "movaps 16(%%r9), %%xmm9;"
        "movaps 32(%%r9), %%xmm10;"
        "movaps 48(%%r9), %%xmm11;"
        "movaps 64(%%r9), %%xmm12;"
        "movaps 80(%%r9), %%xmm13;"
        "movaps 96(%%r9), %%xmm14;"
        "movaps 112(%%r9), %%xmm15;"
        #endif

        ".align 64;"
        "_work_loop_sqrt_ss:"
        #ifdef REGONLY
        "sqrtss %%xmm8, %%xmm0;"
        "sqrtss %%xmm9, %%xmm0;"
        "sqrtss %%xmm10, %%xmm0;"
        "sqrtss %%xmm11, %%xmm0;"
        "sqrtss %%xmm12, %%xmm0;"
        "sqrtss %%xmm13, %%xmm0;"
        "sqrtss %%xmm14, %%xmm0;"
        "sqrtss %%xmm15, %%xmm0;"
        "sqrtss %%xmm8, %%xmm0;"
        "sqrtss %%xmm9, %%xmm0;"
        "sqrtss %%xmm10, %%xmm0;"
        "sqrtss %%xmm11, %%xmm0;"
        "sqrtss %%xmm12, %%xmm0;"
        "sqrtss %%xmm13, %%xmm0;"
        "sqrtss %%xmm14, %%xmm0;"
        "sqrtss %%xmm15, %%xmm0;"
        "sqrtss %%xmm8, %%xmm0;"
        "sqrtss %%xmm9, %%xmm0;"
        "sqrtss %%xmm10, %%xmm0;"
        "sqrtss %%xmm11, %%xmm0;"
        "sqrtss %%xmm12, %%xmm0;"
        "sqrtss %%xmm13, %%xmm0;"
        "sqrtss %%xmm14, %%xmm0;"
        "sqrtss %%xmm15, %%xmm0;"
        "sqrtss %%xmm8, %%xmm0;"
        "sqrtss %%xmm9, %%xmm0;"
        "sqrtss %%xmm10, %%xmm0;"
        "sqrtss %%xmm11, %%xmm0;"
        "sqrtss %%xmm12, %%xmm0;"
        "sqrtss %%xmm13, %%xmm0;"
        "sqrtss %%xmm14, %%xmm0;"
        "sqrtss %%xmm15, %%xmm0;"
        #else

        "sqrtss 0(%%r9), %%xmm0;"
        "sqrtss 16(%%r9), %%xmm0;"
        "sqrtss 32(%%r9), %%xmm0;"
        "sqrtss 48(%%r9), %%xmm0;"

        "sqrtss 64(%%r9), %%xmm0;"
        "sqrtss 80(%%r9), %%xmm0;"
        "sqrtss 96(%%r9), %%xmm0;"
        "sqrtss 112(%%r9), %%xmm0;"

        "sqrtss 128(%%r9), %%xmm0;"
        "sqrtss 144(%%r9), %%xmm0;"
        "sqrtss 160(%%r9), %%xmm0;"
        "sqrtss 176(%%r9), %%xmm0;"

        "sqrtss 192(%%r9), %%xmm0;"
        "sqrtss 208(%%r9), %%xmm0;"
        "sqrtss 224(%%r9), %%xmm0;"
        "sqrtss 240(%%r9), %%xmm0;"

        "sqrtss 256(%%r9), %%xmm0;"
        "sqrtss 272(%%r9), %%xmm0;"
        "sqrtss 288(%%r9), %%xmm0;"
        "sqrtss 304(%%r9), %%xmm0;"

        "sqrtss 320(%%r9), %%xmm0;"
        "sqrtss 336(%%r9), %%xmm0;"
        "sqrtss 352(%%r9), %%xmm0;"
        "sqrtss 368(%%r9), %%xmm0;"

        "sqrtss 384(%%r9), %%xmm0;"
        "sqrtss 400(%%r9), %%xmm0;"
        "sqrtss 416(%%r9), %%xmm0;"
        "sqrtss 432(%%r9), %%xmm0;"

        "sqrtss 448(%%r9), %%xmm0;"
        "sqrtss 464(%%r9), %%xmm0;"
        "sqrtss 480(%%r9), %%xmm0;"
        "sqrtss 496(%%r9), %%xmm0;"
        #endif
        "add $512,%%r9;"
        "sub $1,%%r10;"
        "jnz _skip_reset_sqrt_ss;" // reset buffer if the end is reached
        "mov %%r14,%%r9;"   //restore addr
        "mov %%r8,%%r10;"   //restore passes
        "_skip_reset_sqrt_ss:"
        "sub $32,%%r15;"
        "jnz _work_loop_sqrt_ss;"

        "mov %%r13,%%rcx;" //restore length
        : "=a" (a), "=b" (b), "=c" (c), "=d" (d)
        : "a"(addr), "b" (passes), "c" (length)
        : "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"

    );
    ret=c;

    return ret;
}

uint64_t sqrtsd_kernel(double *buffer, uint64_t elems, uint64_t repeat)
{
    unsigned long long passes,length,addr;
    unsigned long long a,b,c,d;
    uint64_t ret=0;
    #ifdef REGONLY
    assert(elems >= 256/sizeof(*buffer));
    #endif

    passes=elems/64; // 32 128-Bit accesses in inner loop
    length=passes*32*repeat;
    addr=(unsigned long long) buffer;

    if (!passes) return ret;
    /*
     * Input:  RAX: addr (pointer to the buffer)
     *         RBX: passes (number of iterations)
     *         RCX: length (total number of accesses)
     */
    __asm__ __volatile__(
        "mfence;"
        "mov %%rax,%%r9;"   // addr
        "mov %%rbx,%%r10;"  // passes
        "mov %%rcx,%%r15;"  // length
        "mov %%r9,%%r14;"   // store addr
        "mov %%r10,%%r8;"   // store passes
        "mov %%r15,%%r13;"  // store length

        //initialize registers
        "movapd 0(%%r9), %%xmm0;"
        #ifdef REGONLY
        "movapd 0(%%r9), %%xmm8;"
        "movapd 16(%%r9), %%xmm9;"
        "movapd 32(%%r9), %%xmm10;"
        "movapd 48(%%r9), %%xmm11;"
        "movapd 64(%%r9), %%xmm12;"
        "movapd 80(%%r9), %%xmm13;"
        "movapd 96(%%r9), %%xmm14;"
        "movapd 112(%%r9), %%xmm15;"
        #endif

        ".align 64;"
        "_work_loop_sqrt_sd:"
        #ifdef REGONLY
        "sqrtsd %%xmm8, %%xmm0;"
        "sqrtsd %%xmm9, %%xmm0;"
        "sqrtsd %%xmm10, %%xmm0;"
        "sqrtsd %%xmm11, %%xmm0;"
        "sqrtsd %%xmm12, %%xmm0;"
        "sqrtsd %%xmm13, %%xmm0;"
        "sqrtsd %%xmm14, %%xmm0;"
        "sqrtsd %%xmm15, %%xmm0;"
        "sqrtsd %%xmm8, %%xmm0;"
        "sqrtsd %%xmm9, %%xmm0;"
        "sqrtsd %%xmm10, %%xmm0;"
        "sqrtsd %%xmm11, %%xmm0;"
        "sqrtsd %%xmm12, %%xmm0;"
        "sqrtsd %%xmm13, %%xmm0;"
        "sqrtsd %%xmm14, %%xmm0;"
        "sqrtsd %%xmm15, %%xmm0;"
        "sqrtsd %%xmm8, %%xmm0;"
        "sqrtsd %%xmm9, %%xmm0;"
        "sqrtsd %%xmm10, %%xmm0;"
        "sqrtsd %%xmm11, %%xmm0;"
        "sqrtsd %%xmm12, %%xmm0;"
        "sqrtsd %%xmm13, %%xmm0;"
        "sqrtsd %%xmm14, %%xmm0;"
        "sqrtsd %%xmm15, %%xmm0;"
        "sqrtsd %%xmm8, %%xmm0;"
        "sqrtsd %%xmm9, %%xmm0;"
        "sqrtsd %%xmm10, %%xmm0;"
        "sqrtsd %%xmm11, %%xmm0;"
        "sqrtsd %%xmm12, %%xmm0;"
        "sqrtsd %%xmm13, %%xmm0;"
        "sqrtsd %%xmm14, %%xmm0;"
        "sqrtsd %%xmm15, %%xmm0;"
        #else

        "sqrtsd 0(%%r9), %%xmm0;"
        "sqrtsd 16(%%r9), %%xmm0;"
        "sqrtsd 32(%%r9), %%xmm0;"
        "sqrtsd 48(%%r9), %%xmm0;"

        "sqrtsd 64(%%r9), %%xmm0;"
        "sqrtsd 80(%%r9), %%xmm0;"
        "sqrtsd 96(%%r9), %%xmm0;"
        "sqrtsd 112(%%r9), %%xmm0;"

        "sqrtsd 128(%%r9), %%xmm0;"
        "sqrtsd 144(%%r9), %%xmm0;"
        "sqrtsd 160(%%r9), %%xmm0;"
        "sqrtsd 176(%%r9), %%xmm0;"

        "sqrtsd 192(%%r9), %%xmm0;"
        "sqrtsd 208(%%r9), %%xmm0;"
        "sqrtsd 224(%%r9), %%xmm0;"
        "sqrtsd 240(%%r9), %%xmm0;"

        "sqrtsd 256(%%r9), %%xmm0;"
        "sqrtsd 272(%%r9), %%xmm0;"
        "sqrtsd 288(%%r9), %%xmm0;"
        "sqrtsd 304(%%r9), %%xmm0;"

        "sqrtsd 320(%%r9), %%xmm0;"
        "sqrtsd 336(%%r9), %%xmm0;"
        "sqrtsd 352(%%r9), %%xmm0;"
        "sqrtsd 368(%%r9), %%xmm0;"

        "sqrtsd 384(%%r9), %%xmm0;"
        "sqrtsd 400(%%r9), %%xmm0;"
        "sqrtsd 416(%%r9), %%xmm0;"
        "sqrtsd 432(%%r9), %%xmm0;"

        "sqrtsd 448(%%r9), %%xmm0;"
        "sqrtsd 464(%%r9), %%xmm0;"
        "sqrtsd 480(%%r9), %%xmm0;"
        "sqrtsd 496(%%r9), %%xmm0;"
        #endif
        "add $512,%%r9;"
        "sub $1,%%r10;"
        "jnz _skip_reset_sqrt_sd;" // reset buffer if the end is reached
        "mov %%r14,%%r9;"   //restore addr
        "mov %%r8,%%r10;"   //restore passes
        "_skip_reset_sqrt_sd:"
        "sub $32,%%r15;"
        "jnz _work_loop_sqrt_sd;"

        "mov %%r13,%%rcx;" //restore length
        : "=a" (a), "=b" (b), "=c" (c), "=d" (d)
        : "a"(addr), "b" (passes), "c" (length)
        : "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"

    );
    ret=c;

    return ret;
}

uint64_t sqrtps_kernel(float *buffer, uint64_t elems, uint64_t repeat)
{
    unsigned long long passes,length,addr;
    unsigned long long a,b,c,d;
    uint64_t ret=0;
    #ifdef REGONLY
    assert(elems >= 256/sizeof(*buffer));
    #endif

    passes=elems/64; // 32 128-Bit accesses in inner loop
    length=passes*32*repeat;
    addr=(unsigned long long) buffer;

    if (!passes) return ret;
    /*
     * Input:  RAX: addr (pointer to the buffer)
     *         RBX: passes (number of iterations)
     *         RCX: length (total number of accesses)
     */
    __asm__ __volatile__(
        "mfence;"
        "mov %%rax,%%r9;"   // addr
        "mov %%rbx,%%r10;"  // passes
        "mov %%rcx,%%r15;"  // length
        "mov %%r9,%%r14;"   // store addr
        "mov %%r10,%%r8;"   // store passes
        "mov %%r15,%%r13;"  // store length

        //initialize registers
        "movaps 0(%%r9), %%xmm0;"
        #ifdef REGONLY
        "movaps 0(%%r9), %%xmm8;"
        "movaps 16(%%r9), %%xmm9;"
        "movaps 32(%%r9), %%xmm10;"
        "movaps 48(%%r9), %%xmm11;"
        "movaps 64(%%r9), %%xmm12;"
        "movaps 80(%%r9), %%xmm13;"
        "movaps 96(%%r9), %%xmm14;"
        "movaps 112(%%r9), %%xmm15;"
        #endif

        ".align 64;"
        "_work_loop_sqrt_ps:"
        #ifdef REGONLY
        "sqrtps %%xmm8, %%xmm0;"
        "sqrtps %%xmm9, %%xmm0;"
        "sqrtps %%xmm10, %%xmm0;"
        "sqrtps %%xmm11, %%xmm0;"
        "sqrtps %%xmm12, %%xmm0;"
        "sqrtps %%xmm13, %%xmm0;"
        "sqrtps %%xmm14, %%xmm0;"
        "sqrtps %%xmm15, %%xmm0;"
        "sqrtps %%xmm8, %%xmm0;"
        "sqrtps %%xmm9, %%xmm0;"
        "sqrtps %%xmm10, %%xmm0;"
        "sqrtps %%xmm11, %%xmm0;"
        "sqrtps %%xmm12, %%xmm0;"
        "sqrtps %%xmm13, %%xmm0;"
        "sqrtps %%xmm14, %%xmm0;"
        "sqrtps %%xmm15, %%xmm0;"
        "sqrtps %%xmm8, %%xmm0;"
        "sqrtps %%xmm9, %%xmm0;"
        "sqrtps %%xmm10, %%xmm0;"
        "sqrtps %%xmm11, %%xmm0;"
        "sqrtps %%xmm12, %%xmm0;"
        "sqrtps %%xmm13, %%xmm0;"
        "sqrtps %%xmm14, %%xmm0;"
        "sqrtps %%xmm15, %%xmm0;"
        "sqrtps %%xmm8, %%xmm0;"
        "sqrtps %%xmm9, %%xmm0;"
        "sqrtps %%xmm10, %%xmm0;"
        "sqrtps %%xmm11, %%xmm0;"
        "sqrtps %%xmm12, %%xmm0;"
        "sqrtps %%xmm13, %%xmm0;"
        "sqrtps %%xmm14, %%xmm0;"
        "sqrtps %%xmm15, %%xmm0;"
        #else

        "sqrtps 0(%%r9), %%xmm0;"
        "sqrtps 16(%%r9), %%xmm0;"
        "sqrtps 32(%%r9), %%xmm0;"
        "sqrtps 48(%%r9), %%xmm0;"

        "sqrtps 64(%%r9), %%xmm0;"
        "sqrtps 80(%%r9), %%xmm0;"
        "sqrtps 96(%%r9), %%xmm0;"
        "sqrtps 112(%%r9), %%xmm0;"

        "sqrtps 128(%%r9), %%xmm0;"
        "sqrtps 144(%%r9), %%xmm0;"
        "sqrtps 160(%%r9), %%xmm0;"
        "sqrtps 176(%%r9), %%xmm0;"

        "sqrtps 192(%%r9), %%xmm0;"
        "sqrtps 208(%%r9), %%xmm0;"
        "sqrtps 224(%%r9), %%xmm0;"
        "sqrtps 240(%%r9), %%xmm0;"

        "sqrtps 256(%%r9), %%xmm0;"
        "sqrtps 272(%%r9), %%xmm0;"
        "sqrtps 288(%%r9), %%xmm0;"
        "sqrtps 304(%%r9), %%xmm0;"

        "sqrtps 320(%%r9), %%xmm0;"
        "sqrtps 336(%%r9), %%xmm0;"
        "sqrtps 352(%%r9), %%xmm0;"
        "sqrtps 368(%%r9), %%xmm0;"

        "sqrtps 384(%%r9), %%xmm0;"
        "sqrtps 400(%%r9), %%xmm0;"
        "sqrtps 416(%%r9), %%xmm0;"
        "sqrtps 432(%%r9), %%xmm0;"

        "sqrtps 448(%%r9), %%xmm0;"
        "sqrtps 464(%%r9), %%xmm0;"
        "sqrtps 480(%%r9), %%xmm0;"
        "sqrtps 496(%%r9), %%xmm0;"
        #endif
        "add $512,%%r9;"
        "sub $1,%%r10;"
        "jnz _skip_reset_sqrt_ps;" // reset buffer if the end is reached
        "mov %%r14,%%r9;"   //restore addr
        "mov %%r8,%%r10;"   //restore passes
        "_skip_reset_sqrt_ps:"
        "sub $32,%%r15;"
        "jnz _work_loop_sqrt_ps;"

        "mov %%r13,%%rcx;" //restore length
        : "=a" (a), "=b" (b), "=c" (c), "=d" (d)
        : "a"(addr), "b" (passes), "c" (length)
        : "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"

    );
    ret=4*c;

    return ret;
}

uint64_t sqrtpd_kernel(double *buffer, uint64_t elems, uint64_t repeat)
{
    unsigned long long passes,length,addr;
    unsigned long long a,b,c,d;
    uint64_t ret=0;
    #ifdef REGONLY
    assert(elems >= 256/sizeof(*buffer));
    #endif

    passes=elems/64; // 32 128-Bit accesses in inner loop
    length=passes*32*repeat;
    addr=(unsigned long long) buffer;

    if (!passes) return ret;
    /*
     * Input:  RAX: addr (pointer to the buffer)
     *         RBX: passes (number of iterations)
     *         RCX: length (total number of accesses)
     */
    __asm__ __volatile__(
        "mfence;"
        "mov %%rax,%%r9;"   // addr
        "mov %%rbx,%%r10;"  // passes
        "mov %%rcx,%%r15;"  // length
        "mov %%r9,%%r14;"   // store addr
        "mov %%r10,%%r8;"   // store passes
        "mov %%r15,%%r13;"  // store length

        //initialize registers
        "movaps 0(%%r9), %%xmm0;"
        #ifdef REGONLY
        "movapd 0(%%r9), %%xmm8;"
        "movapd 16(%%r9), %%xmm9;"
        "movapd 32(%%r9), %%xmm10;"
        "movapd 48(%%r9), %%xmm11;"
        "movapd 64(%%r9), %%xmm12;"
        "movapd 80(%%r9), %%xmm13;"
        "movapd 96(%%r9), %%xmm14;"
        "movapd 112(%%r9), %%xmm15;"
        #endif

        ".align 64;"
        "_work_loop_sqrt_pd:"
        #ifdef REGONLY
        "sqrtpd %%xmm8, %%xmm0;"
        "sqrtpd %%xmm9, %%xmm0;"
        "sqrtpd %%xmm10, %%xmm0;"
        "sqrtpd %%xmm11, %%xmm0;"
        "sqrtpd %%xmm12, %%xmm0;"
        "sqrtpd %%xmm13, %%xmm0;"
        "sqrtpd %%xmm14, %%xmm0;"
        "sqrtpd %%xmm15, %%xmm0;"
        "sqrtpd %%xmm8, %%xmm0;"
        "sqrtpd %%xmm9, %%xmm0;"
        "sqrtpd %%xmm10, %%xmm0;"
        "sqrtpd %%xmm11, %%xmm0;"
        "sqrtpd %%xmm12, %%xmm0;"
        "sqrtpd %%xmm13, %%xmm0;"
        "sqrtpd %%xmm14, %%xmm0;"
        "sqrtpd %%xmm15, %%xmm0;"
        "sqrtpd %%xmm8, %%xmm0;"
        "sqrtpd %%xmm9, %%xmm0;"
        "sqrtpd %%xmm10, %%xmm0;"
        "sqrtpd %%xmm11, %%xmm0;"
        "sqrtpd %%xmm12, %%xmm0;"
        "sqrtpd %%xmm13, %%xmm0;"
        "sqrtpd %%xmm14, %%xmm0;"
        "sqrtpd %%xmm15, %%xmm0;"
        "sqrtpd %%xmm8, %%xmm0;"
        "sqrtpd %%xmm9, %%xmm0;"
        "sqrtpd %%xmm10, %%xmm0;"
        "sqrtpd %%xmm11, %%xmm0;"
        "sqrtpd %%xmm12, %%xmm0;"
        "sqrtpd %%xmm13, %%xmm0;"
        "sqrtpd %%xmm14, %%xmm0;"
        "sqrtpd %%xmm15, %%xmm0;"
        #else

        "sqrtpd 0(%%r9), %%xmm0;"
        "sqrtpd 16(%%r9), %%xmm0;"
        "sqrtpd 32(%%r9), %%xmm0;"
        "sqrtpd 48(%%r9), %%xmm0;"

        "sqrtpd 64(%%r9), %%xmm0;"
        "sqrtpd 80(%%r9), %%xmm0;"
        "sqrtpd 96(%%r9), %%xmm0;"
        "sqrtpd 112(%%r9), %%xmm0;"

        "sqrtpd 128(%%r9), %%xmm0;"
        "sqrtpd 144(%%r9), %%xmm0;"
        "sqrtpd 160(%%r9), %%xmm0;"
        "sqrtpd 176(%%r9), %%xmm0;"

        "sqrtpd 192(%%r9), %%xmm0;"
        "sqrtpd 208(%%r9), %%xmm0;"
        "sqrtpd 224(%%r9), %%xmm0;"
        "sqrtpd 240(%%r9), %%xmm0;"

        "sqrtpd 256(%%r9), %%xmm0;"
        "sqrtpd 272(%%r9), %%xmm0;"
        "sqrtpd 288(%%r9), %%xmm0;"
        "sqrtpd 304(%%r9), %%xmm0;"

        "sqrtpd 320(%%r9), %%xmm0;"
        "sqrtpd 336(%%r9), %%xmm0;"
        "sqrtpd 352(%%r9), %%xmm0;"
        "sqrtpd 368(%%r9), %%xmm0;"

        "sqrtpd 384(%%r9), %%xmm0;"
        "sqrtpd 400(%%r9), %%xmm0;"
        "sqrtpd 416(%%r9), %%xmm0;"
        "sqrtpd 432(%%r9), %%xmm0;"

        "sqrtpd 448(%%r9), %%xmm0;"
        "sqrtpd 464(%%r9), %%xmm0;"
        "sqrtpd 480(%%r9), %%xmm0;"
        "sqrtpd 496(%%r9), %%xmm0;"
        #endif
        "add $512,%%r9;"
        "sub $1,%%r10;"
        "jnz _skip_reset_sqrt_pd;" // reset buffer if the end is reached
        "mov %%r14,%%r9;"   //restore addr
        "mov %%r8,%%r10;"   //restore passes
        "_skip_reset_sqrt_pd:"
        "sub $32,%%r15;"
        "jnz _work_loop_sqrt_pd;"

        "mov %%r13,%%rcx;" //restore length
        : "=a" (a), "=b" (b), "=c" (c), "=d" (d)
        : "a"(addr), "b" (passes), "c" (length)
        : "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"

    );
    ret=2*c;

    return ret;
}
