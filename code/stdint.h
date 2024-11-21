#ifndef _STDINT_H
#define _STDINT_H

#define __NEED_int8_t
#define __NEED_int16_t
#define __NEED_int32_t
#define __NEED_int64_t

#define __NEED_uint8_t
#define __NEED_uint16_t
#define __NEED_uint32_t
#define __NEED_uint64_t

#define __NEED_intptr_t
#define __NEED_uintptr_t

#define __NEED_intmax_t
#define __NEED_uintmax_t

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#define __USE_TIME_BITS64 1

#define __BYTE_ORDER __LITTLE_ENDIAN

#define __LONG_MAX  __LONG_MAX__

#define _Addr __PTRDIFF_TYPE__
#define _Int64 __INT64_TYPE__
#define _Reg __PTRDIFF_TYPE__

#if __GNUC__ >= 3
#if defined(__NEED_va_list) && !defined(__DEFINED_va_list)
typedef __builtin_va_list va_list;
#define __DEFINED_va_list
#endif

#if defined(__NEED___isoc_va_list) && !defined(__DEFINED___isoc_va_list)
typedef __builtin_va_list __isoc_va_list;
#define __DEFINED___isoc_va_list
#endif

#else
#if defined(__NEED_va_list) && !defined(__DEFINED_va_list)
typedef struct __va_list * va_list;
#define __DEFINED_va_list
#endif

#if defined(__NEED___isoc_va_list) && !defined(__DEFINED___isoc_va_list)
typedef struct __va_list * __isoc_va_list;
#define __DEFINED___isoc_va_list
#endif

#endif

#ifndef __cplusplus
#ifdef __WCHAR_TYPE__
#if defined(__NEED_wchar_t) && !defined(__DEFINED_wchar_t)
typedef __WCHAR_TYPE__ wchar_t;
#define __DEFINED_wchar_t
#endif

#else
#if defined(__NEED_wchar_t) && !defined(__DEFINED_wchar_t)
typedef int wchar_t;
#define __DEFINED_wchar_t
#endif

#endif
#endif
#if defined(__NEED_wint_t) && !defined(__DEFINED_wint_t)
typedef __WINT_TYPE__ wint_t;
#define __DEFINED_wint_t
#endif


#if defined(__FLT_EVAL_METHOD__) && __FLT_EVAL_METHOD__ == 0
#if defined(__NEED_float_t) && !defined(__DEFINED_float_t)
typedef float float_t;
#define __DEFINED_float_t
#endif

#if defined(__NEED_double_t) && !defined(__DEFINED_double_t)
typedef double double_t;
#define __DEFINED_double_t
#endif

#else
#if defined(__NEED_float_t) && !defined(__DEFINED_float_t)
typedef long double float_t;
#define __DEFINED_float_t
#endif

#if defined(__NEED_double_t) && !defined(__DEFINED_double_t)
typedef long double double_t;
#define __DEFINED_double_t
#endif

#endif

#if defined(__NEED_time_t) && !defined(__DEFINED_time_t)
typedef _Int64 time_t;
#define __DEFINED_time_t
#endif

#if defined(__NEED_suseconds_t) && !defined(__DEFINED_suseconds_t)
typedef int suseconds_t; /* XXX EMSCRIPTEN: ensure it's always 32-bits even in wasm64 */
#define __DEFINED_suseconds_t
#endif


#if defined(__NEED_pthread_attr_t) && !defined(__DEFINED_pthread_attr_t)
typedef struct {
    union {
        int __i[10];
        volatile int __vi[10];
        unsigned long __s[10];
    } __u;
	#ifdef __EMSCRIPTEN__
    // For canvas transfer implementation in Emscripten, use an extra control field
    // to pass a pointer to a string denoting the WebGL canvases to transfer.
    const char *_a_transferredcanvases;
	#endif
} pthread_attr_t;
#define __DEFINED_pthread_attr_t
#endif

#if defined(__NEED_pthread_mutex_t) && !defined(__DEFINED_pthread_mutex_t)
typedef struct { union { int __i[6]; volatile int __vi[6]; volatile void *__p[6]; } __u; } pthread_mutex_t;
#define __DEFINED_pthread_mutex_t
#endif

#if defined(__NEED_mtx_t) && !defined(__DEFINED_mtx_t)
typedef struct { union { int __i[6]; volatile int __vi[6]; volatile void *__p[6]; } __u; } mtx_t;
#define __DEFINED_mtx_t
#endif

#if defined(__NEED_pthread_cond_t) && !defined(__DEFINED_pthread_cond_t)
typedef struct { union { int __i[12]; volatile int __vi[12]; void *__p[12]; } __u; } pthread_cond_t;
#define __DEFINED_pthread_cond_t
#endif

#if defined(__NEED_cnd_t) && !defined(__DEFINED_cnd_t)
typedef struct { union { int __i[12]; volatile int __vi[12]; void *__p[12]; } __u; } cnd_t;
#define __DEFINED_cnd_t
#endif

#if defined(__NEED_pthread_rwlock_t) && !defined(__DEFINED_pthread_rwlock_t)
typedef struct { union { int __i[sizeof(long)==8?14:8]; volatile int __vi[sizeof(long)==8?14:8]; void *__p[sizeof(long)==8?7:8]; } __u; } pthread_rwlock_t;
#define __DEFINED_pthread_rwlock_t
#endif

#if defined(__NEED_pthread_barrier_t) && !defined(__DEFINED_pthread_barrier_t)
typedef struct { union { int __i[5]; volatile int __vi[5]; void *__p[5]; } __u; } pthread_barrier_t;
#define __DEFINED_pthread_barrier_t
#endif


#if defined(__NEED_size_t) && !defined(__DEFINED_size_t)
typedef unsigned _Addr size_t;
#define __DEFINED_size_t
#endif

#if defined(__NEED_uintptr_t) && !defined(__DEFINED_uintptr_t)
typedef unsigned _Addr uintptr_t;
#define __DEFINED_uintptr_t
#endif

#if defined(__NEED_ptrdiff_t) && !defined(__DEFINED_ptrdiff_t)
typedef _Addr ptrdiff_t;
#define __DEFINED_ptrdiff_t
#endif

#if defined(__NEED_ssize_t) && !defined(__DEFINED_ssize_t)
typedef _Addr ssize_t;
#define __DEFINED_ssize_t
#endif

#if defined(__NEED_intptr_t) && !defined(__DEFINED_intptr_t)
typedef _Addr intptr_t;
#define __DEFINED_intptr_t
#endif

#if defined(__NEED_regoff_t) && !defined(__DEFINED_regoff_t)
typedef _Addr regoff_t;
#define __DEFINED_regoff_t
#endif

#if defined(__NEED_register_t) && !defined(__DEFINED_register_t)
typedef _Reg register_t;
#define __DEFINED_register_t
#endif


#if defined(__NEED_int8_t) && !defined(__DEFINED_int8_t)
typedef signed char     int8_t;
#define __DEFINED_int8_t
#endif

#if defined(__NEED_int16_t) && !defined(__DEFINED_int16_t)
typedef short           int16_t;
#define __DEFINED_int16_t
#endif

#if defined(__NEED_int32_t) && !defined(__DEFINED_int32_t)
typedef int             int32_t;
#define __DEFINED_int32_t
#endif

#if defined(__NEED_int64_t) && !defined(__DEFINED_int64_t)
typedef _Int64          int64_t;
#define __DEFINED_int64_t
#endif

#if defined(__NEED_intmax_t) && !defined(__DEFINED_intmax_t)
typedef _Int64          intmax_t;
#define __DEFINED_intmax_t
#endif

#if defined(__NEED_uint8_t) && !defined(__DEFINED_uint8_t)
typedef unsigned char   uint8_t;
#define __DEFINED_uint8_t
#endif

#if defined(__NEED_uint16_t) && !defined(__DEFINED_uint16_t)
typedef unsigned short  uint16_t;
#define __DEFINED_uint16_t
#endif

#if defined(__NEED_uint32_t) && !defined(__DEFINED_uint32_t)
typedef unsigned int    uint32_t;
#define __DEFINED_uint32_t
#endif

#if defined(__NEED_uint64_t) && !defined(__DEFINED_uint64_t)
typedef unsigned _Int64 uint64_t;
#define __DEFINED_uint64_t
#endif

#if defined(__NEED_u_int64_t) && !defined(__DEFINED_u_int64_t)
typedef unsigned _Int64 u_int64_t;
#define __DEFINED_u_int64_t
#endif

#if defined(__NEED_uintmax_t) && !defined(__DEFINED_uintmax_t)
typedef unsigned _Int64 uintmax_t;
#define __DEFINED_uintmax_t
#endif


#if defined(__NEED_mode_t) && !defined(__DEFINED_mode_t)
typedef unsigned mode_t;
#define __DEFINED_mode_t
#endif

#if defined(__NEED_nlink_t) && !defined(__DEFINED_nlink_t)
typedef unsigned _Reg nlink_t;
#define __DEFINED_nlink_t
#endif

#if defined(__NEED_off_t) && !defined(__DEFINED_off_t)
typedef _Int64 off_t;
#define __DEFINED_off_t
#endif

#if defined(__NEED_ino_t) && !defined(__DEFINED_ino_t)
typedef unsigned _Int64 ino_t;
#define __DEFINED_ino_t
#endif

#if defined(__NEED_dev_t) && !defined(__DEFINED_dev_t)
typedef unsigned int dev_t;
#define __DEFINED_dev_t
#endif

#if defined(__NEED_blksize_t) && !defined(__DEFINED_blksize_t)
typedef int blksize_t; /* XXX EMSCRIPTEN: ensure it's always 32-bits even in wasm64 */
#define __DEFINED_blksize_t
#endif

#if defined(__NEED_blkcnt_t) && !defined(__DEFINED_blkcnt_t)
typedef int blkcnt_t;
#define __DEFINED_blkcnt_t
#endif

#if defined(__NEED_fsblkcnt_t) && !defined(__DEFINED_fsblkcnt_t)
typedef unsigned int fsblkcnt_t;
#define __DEFINED_fsblkcnt_t
#endif

#if defined(__NEED_fsfilcnt_t) && !defined(__DEFINED_fsfilcnt_t)
typedef unsigned int fsfilcnt_t;
#define __DEFINED_fsfilcnt_t
#endif

#if defined(__NEED_wint_t) && !defined(__DEFINED_wint_t)
typedef unsigned wint_t;
#define __DEFINED_wint_t
#endif

#if defined(__NEED_wctype_t) && !defined(__DEFINED_wctype_t)
typedef unsigned int wctype_t; /* XXX EMSCRIPTEN: ensure it's always 32-bits even in wasm64 */
#define __DEFINED_wctype_t
#endif


#if defined(__NEED_timer_t) && !defined(__DEFINED_timer_t)
typedef void * timer_t;
#define __DEFINED_timer_t
#endif

#if defined(__NEED_clockid_t) && !defined(__DEFINED_clockid_t)
typedef int clockid_t;
#define __DEFINED_clockid_t
#endif

#if defined(__NEED_clock_t) && !defined(__DEFINED_clock_t)
typedef int clock_t;  /* XXX EMSCRIPTEN: ensure it's always 32-bits even in wasm64 */
#define __DEFINED_clock_t
#endif

#if defined(__NEED_max_align_t) && !defined(__DEFINED_max_align_t)
typedef struct { long long __ll; long double __ld; } max_align_t;
#define __DEFINED_max_align_t
#endif

#if defined(__NEED_struct_timeval) && !defined(__DEFINED_struct_timeval)
struct timeval { time_t tv_sec; suseconds_t tv_usec; };
#define __DEFINED_struct_timeval
#endif

#if defined(__NEED_struct_timespec) && !defined(__DEFINED_struct_timespec)
struct timespec { time_t tv_sec; long tv_nsec; };
#define __DEFINED_struct_timespec
#endif


#if defined(__NEED_pid_t) && !defined(__DEFINED_pid_t)
typedef int pid_t;
#define __DEFINED_pid_t
#endif

#if defined(__NEED_id_t) && !defined(__DEFINED_id_t)
typedef unsigned id_t;
#define __DEFINED_id_t
#endif

#if defined(__NEED_uid_t) && !defined(__DEFINED_uid_t)
typedef unsigned uid_t;
#define __DEFINED_uid_t
#endif

#if defined(__NEED_gid_t) && !defined(__DEFINED_gid_t)
typedef unsigned gid_t;
#define __DEFINED_gid_t
#endif

#if defined(__NEED_key_t) && !defined(__DEFINED_key_t)
typedef int key_t;
#define __DEFINED_key_t
#endif

#if defined(__NEED_useconds_t) && !defined(__DEFINED_useconds_t)
typedef unsigned useconds_t;
#define __DEFINED_useconds_t
#endif


#ifdef __cplusplus
#if defined(__NEED_pthread_t) && !defined(__DEFINED_pthread_t)
typedef unsigned long pthread_t;
#define __DEFINED_pthread_t
#endif

#else
#if defined(__NEED_pthread_t) && !defined(__DEFINED_pthread_t)
typedef struct __pthread * pthread_t;
#define __DEFINED_pthread_t
#endif

#endif
#if defined(__NEED_pthread_once_t) && !defined(__DEFINED_pthread_once_t)
typedef int pthread_once_t;
#define __DEFINED_pthread_once_t
#endif

#if defined(__NEED_pthread_key_t) && !defined(__DEFINED_pthread_key_t)
typedef unsigned pthread_key_t;
#define __DEFINED_pthread_key_t
#endif

#if defined(__NEED_pthread_spinlock_t) && !defined(__DEFINED_pthread_spinlock_t)
typedef int pthread_spinlock_t;
#define __DEFINED_pthread_spinlock_t
#endif

#if defined(__NEED_pthread_mutexattr_t) && !defined(__DEFINED_pthread_mutexattr_t)
typedef struct { unsigned __attr; } pthread_mutexattr_t;
#define __DEFINED_pthread_mutexattr_t
#endif

#if defined(__NEED_pthread_condattr_t) && !defined(__DEFINED_pthread_condattr_t)
typedef struct { unsigned __attr; } pthread_condattr_t;
#define __DEFINED_pthread_condattr_t
#endif

#if defined(__NEED_pthread_barrierattr_t) && !defined(__DEFINED_pthread_barrierattr_t)
typedef struct { unsigned __attr; } pthread_barrierattr_t;
#define __DEFINED_pthread_barrierattr_t
#endif

#if defined(__NEED_pthread_rwlockattr_t) && !defined(__DEFINED_pthread_rwlockattr_t)
typedef struct { unsigned __attr[2]; } pthread_rwlockattr_t;
#define __DEFINED_pthread_rwlockattr_t
#endif


#if defined(__NEED_FILE) && !defined(__DEFINED_FILE)
typedef struct _IO_FILE FILE;
#define __DEFINED_FILE
#endif


#if defined(__NEED_mbstate_t) && !defined(__DEFINED_mbstate_t)
typedef struct __mbstate_t { unsigned __opaque1, __opaque2; } mbstate_t;
#define __DEFINED_mbstate_t
#endif


#if defined(__NEED_statice_t) && !defined(__DEFINED_statice_t)
typedef struct __statice_struct * statice_t;
#define __DEFINED_statice_t
#endif


#if defined(__NEED_sigset_t) && !defined(__DEFINED_sigset_t)
typedef struct __sigset_t { unsigned long __bits[128/sizeof(long)]; } sigset_t;
#define __DEFINED_sigset_t
#endif


#if defined(__NEED_struct_iovec) && !defined(__DEFINED_struct_iovec)
struct iovec { void *iov_base; size_t iov_len; };
#define __DEFINED_struct_iovec
#endif


#if defined(__NEED_struct_winsize) && !defined(__DEFINED_struct_winsize)
struct winsize { unsigned short ws_row, ws_col, ws_xpixel, ws_ypixel; };
#define __DEFINED_struct_winsize
#endif


#if defined(__NEED_socklen_t) && !defined(__DEFINED_socklen_t)
typedef unsigned socklen_t;
#define __DEFINED_socklen_t
#endif

#if defined(__NEED_sa_family_t) && !defined(__DEFINED_sa_family_t)
typedef unsigned short sa_family_t;
#define __DEFINED_sa_family_t
#endif


#undef _Addr
#undef _Int64
#undef _Reg

typedef int8_t int_fast8_t;
typedef int64_t int_fast64_t;

typedef int8_t  int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

typedef uint8_t uint_fast8_t;
typedef uint64_t uint_fast64_t;

typedef uint8_t  uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

#define INT8_MIN   (-1-0x7f)
#define INT16_MIN  (-1-0x7fff)
#define INT32_MIN  (-1-0x7fffffff)
#define INT64_MIN  (-1-0x7fffffffffffffff)

#define INT8_MAX   (0x7f)
#define INT16_MAX  (0x7fff)
#define INT32_MAX  (0x7fffffff)
#define INT64_MAX  (0x7fffffffffffffff)

#define UINT8_MAX  (0xff)
#define UINT16_MAX (0xffff)
#define UINT32_MAX (0xffffffffu)
#define UINT64_MAX (0xffffffffffffffffu)

#define INT_FAST8_MIN   INT8_MIN
#define INT_FAST64_MIN  INT64_MIN

#define INT_LEAST8_MIN   INT8_MIN
#define INT_LEAST16_MIN  INT16_MIN
#define INT_LEAST32_MIN  INT32_MIN
#define INT_LEAST64_MIN  INT64_MIN

#define INT_FAST8_MAX   INT8_MAX
#define INT_FAST64_MAX  INT64_MAX

#define INT_LEAST8_MAX   INT8_MAX
#define INT_LEAST16_MAX  INT16_MAX
#define INT_LEAST32_MAX  INT32_MAX
#define INT_LEAST64_MAX  INT64_MAX

#define UINT_FAST8_MAX  UINT8_MAX
#define UINT_FAST64_MAX UINT64_MAX

#define UINT_LEAST8_MAX  UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#define INTMAX_MIN  INT64_MIN
#define INTMAX_MAX  INT64_MAX
#define UINTMAX_MAX UINT64_MAX

#define WINT_MIN 0U
#define WINT_MAX UINT32_MAX

#if L'\0'-1 > 0
#define WCHAR_MAX (0xffffffffu+L'\0')
#define WCHAR_MIN (0+L'\0')
#else
#define WCHAR_MAX (0x7fffffff+L'\0')
#define WCHAR_MIN (-1-0x7fffffff+L'\0')
#endif

#define SIG_ATOMIC_MIN  INT32_MIN
#define SIG_ATOMIC_MAX  INT32_MAX

typedef int32_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef uint32_t uint_fast16_t;
typedef uint32_t uint_fast32_t;

#define INT_FAST16_MIN  INT32_MIN
#define INT_FAST32_MIN  INT32_MIN

#define INT_FAST16_MAX  INT32_MAX
#define INT_FAST32_MAX  INT32_MAX

#define UINT_FAST16_MAX UINT32_MAX
#define UINT_FAST32_MAX UINT32_MAX

#if __LP64__

#define INTPTR_MIN      INT64_MIN
#define INTPTR_MAX      INT64_MAX
#define UINTPTR_MAX     UINT64_MAX
#define PTRDIFF_MIN     INT64_MIN
#define PTRDIFF_MAX     INT64_MAX
#define SIZE_MAX        UINT64_MAX

#else

#define INTPTR_MIN      (-1-__INTPTR_MAX__)
#define INTPTR_MAX      __INTPTR_MAX__
#define UINTPTR_MAX     __UINTPTR_MAX__
#define PTRDIFF_MIN     (-1-__PTRDIFF_MAX__)
#define PTRDIFF_MAX     __PTRDIFF_MAX__
#define SIZE_MAX        __SIZE_MAX__

#endif

#define INT8_C(c)  c
#define INT16_C(c) c
#define INT32_C(c) c

#define UINT8_C(c)  c
#define UINT16_C(c) c
#define UINT32_C(c) c ## U

#if UINTPTR_MAX == UINT64_MAX
#define INT64_C(c) c ## L
#define UINT64_C(c) c ## UL
#define INTMAX_C(c)  c ## L
#define UINTMAX_C(c) c ## UL
#else
#define INT64_C(c) c ## LL
#define UINT64_C(c) c ## ULL
#define INTMAX_C(c)  c ## LL
#define UINTMAX_C(c) c ## ULL
#endif

#endif