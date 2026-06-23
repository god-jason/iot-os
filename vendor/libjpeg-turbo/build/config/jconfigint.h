/* libjpeg-turbo internal configuration for xmake (Windows/MinGW) */
#define BUILD  "20260623"
#define HIDDEN
#undef inline
#define INLINE  __inline__
#define THREAD_LOCAL  __declspec(thread)
#define PACKAGE_NAME  "libjpeg-turbo"
#define VERSION  "3.1.91"
#define SIZEOF_SIZE_T  8
#define HAVE_INTRIN_H
#if (SIZEOF_SIZE_T == 8)
#define HAVE_BITSCANFORWARD64
#elif (SIZEOF_SIZE_T == 4)
#define HAVE_BITSCANFORWARD
#endif

#if defined(__has_attribute)
#if __has_attribute(fallthrough)
#define FALLTHROUGH  __attribute__((fallthrough));
#else
#define FALLTHROUGH
#endif
#else
#define FALLTHROUGH
#endif

#ifndef BITS_IN_JSAMPLE
#define BITS_IN_JSAMPLE  8
#endif

#undef C_ARITH_CODING_SUPPORTED
#undef D_ARITH_CODING_SUPPORTED
#undef WITH_SIMD

#if BITS_IN_JSAMPLE == 8
#define C_ARITH_CODING_SUPPORTED 1
#define D_ARITH_CODING_SUPPORTED 1
#define SIMD_ARCHITECTURE  NONE
#endif
