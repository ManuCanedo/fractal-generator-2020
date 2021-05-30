// License https://software.intel.com/

#ifndef CPU_FEATURES_H
#define CPU_FEATURES_H

#if defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 1300) /* Intel compiler (unlikely) */

#include <immintrin.h>

int check_intel_features()
{
    const int the_4th_gen_features =
        (_FEATURE_AVX2 | _FEATURE_FMA | _FEATURE_BMI | _FEATURE_LZCNT | _FEATURE_MOVBE);
    return _may_i_use_cpu_feature(the_4th_gen_features);
}

#else /* non-Intel compiler */

#include <stdint.h>
#if defined(_MSC_VER)
# include <intrin.h>
#endif

void run_cpuid(uint32_t eax, uint32_t ecx, uint32_t* abcd)
{
#if defined(_MSC_VER)
    __cpuidex((int*)(abcd), eax, ecx);
#else
    uint32_t ebx, edx;
# if defined( __i386__ ) && defined ( __PIC__ )
    __asm__("movl %%ebx, %%edi \n\t cpuid \n\t xchgl %%ebx, %%edi" : "=D" (ebx),
# else
    __asm__("cpuid" : "+b" (ebx),
# endif
        "+a" (eax), "+c" (ecx), "=d" (edx));
    abcd[0] = eax; abcd[1] = ebx; abcd[2] = ecx; abcd[3] = edx;
#endif
}

int check_xcr0_ymm()
{
    uint32_t xcr0;
#if defined(_MSC_VER)
    xcr0 = (uint32_t)_xgetbv(0);
#else
    __asm__("xgetbv" : "=a" (xcr0) : "c" (0) : "%edx");
#endif
    return ((xcr0 & 6) == 6);
}

int check_intel_features()
{
    uint32_t abcd[4];
    uint32_t fma_movbe_osxsave_mask = ((1 << 12) | (1 << 22) | (1 << 27));
    uint32_t avx2_bmi12_mask = (1 << 5) | (1 << 3) | (1 << 8);

    run_cpuid(1, 0, abcd);
    if ((abcd[2] & fma_movbe_osxsave_mask) != fma_movbe_osxsave_mask)
        return 0;

    if (!check_xcr0_ymm())
        return 0;

    run_cpuid(7, 0, abcd);
    if ((abcd[1] & avx2_bmi12_mask) != avx2_bmi12_mask)
        return 0;

    run_cpuid(0x80000001, 0, abcd);
    if ((abcd[2] & (1 << 5)) == 0)
        return 0;

    return 1;
}

#endif /* non-Intel compiler */

static int check_vec_ext_support()
{
    static int intel_4gen_features = -1;

    if (intel_4gen_features < 0)
        intel_4gen_features = check_intel_features();

    if (!intel_4gen_features)
        LOG_ERROR("CPU does not support AVX2 instructions. AVX mode disabled");
    else
        LOG_INFO("CPU supports AVX2 instructions. AVX mode enabled");

    return intel_4gen_features;
}

#endif // CPU_FEATURES_H