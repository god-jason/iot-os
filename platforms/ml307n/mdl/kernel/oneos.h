#ifndef __ONEOS_H__
#define __ONEOS_H__

#include "slog_print.h"
#include "os_ext.h"


/* OneOS API adaptation*/
#define os_malloc(size)       osMalloc(size)
#define os_free(ptr)          osFree(ptr)

#define os_kprintf(fmt, ...)  osPrintf(fmt,##__VA_ARGS__)
#ifndef os_malloc
#define os_malloc             osMalloc
#endif

#ifndef os_free
#define os_free               osFree
#endif

#ifndef os_snprintf
#define os_snprintf           osSnprintf
#endif


#define OS_MS_PER_SECOND 1000
#define os_get_tick_from_ms(msecond) (OS_TICK_PER_SECOND * msecond / OS_MS_PER_SECOND + (OS_TICK_PER_SECOND * (msecond % OS_MS_PER_SECOND) + (OS_MS_PER_SECOND - 1))/ OS_MS_PER_SECOND)
#define os_task_msleep(ms)     osThreadSleep(os_get_tick_from_ms(ms))


//#define LOG_OUTPUT_AP_SHELL
#ifdef LOG_OUTPUT_AP_SHELL
#define app_printf(fmt, ...)  osPrintf(fmt,##__VA_ARGS__)
#else
#define app_printf(fmt, ...)  slogPrintf(SLOG_LEVEL_INFO,SLOG_PRINT_SUBMDL_ONEOS,fmt,##__VA_ARGS__)
#endif

#ifndef OS_FALSE
#define OS_FALSE 0
#endif
#ifndef OS_TRUE
#define OS_TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define OS_EOK      0   /* There is no error. */
#define OS_ERROR    -1  /* A generic error happens. */
#define OS_ETIMEOUT -2  /* Timed out. */
#define OS_EFULL    -3  /* The resource is full. */
#define OS_EEMPTY   -4  /* The resource is empty. */
#define OS_ENOMEM   -5  /* No memory. */
#define OS_ENOSYS   -6  /* Function not implemented. */
#define OS_EBUSY    -7  /* Busy. */
#define OS_EIO      -8  /* IO error. */
#define OS_EINTR    -9  /* Interrupted system call. */
#define OS_EINVAL   -10 /* Invalid argument. */
#define OS_ENODEV   -11 /* No such device */
#define OS_EPERM    -12 /* Operation not permitted */
#define OS_EFLASHBAD -13 /* Nand flash bad block */
#define OS_EBADMSG   -14 /* Bad message */
#define OS_EECCBITF  -15 /* ECC has bitflips */



/* Maximum value of base type */
//#define OS_UINT8_MAX  0xffU                 /* Maxium value of UINT8 */
//#define OS_UINT16_MAX 0xffffU               /* Maxium value of UINT16 */
//#define OS_UINT32_MAX 0xffffffffU           /* Maxium value of UINT32 */
//#define OS_UINT64_MAX 0xffffffffffffffffULL /* Maxium value of UINT64 */

#ifdef OS_ARCH_CPU_64BIT
//#define OS_TICK_MAX OS_UINT64_MAX /* Maxium value of tick */
#else
//#define OS_TICK_MAX OS_UINT32_MAX /* Maxium value of tick */
#endif

/* Basic data type definitions */
typedef signed char        os_int8_t;   /* 8bit integer type */
typedef signed short       os_int16_t;  /* 16bit integer type */
typedef signed int         os_int32_t;  /* 32bit integer type */
typedef signed long long   os_int64_t;  /* 64bit integer type */
typedef unsigned char      os_uint8_t;  /* 8bit unsigned integer type */
typedef unsigned short     os_uint16_t; /* 16bit unsigned integer type */
typedef unsigned long int  os_uint32_t; /* 32bit unsigned integer type */
typedef unsigned long long os_uint64_t; /* 64bit unsigned integer type */
typedef signed int         os_bool_t;   /* Boolean type */

/* Represent Nbit CPU */
typedef signed long   os_base_t;  /* Nbit CPU related date type */
typedef unsigned long os_ubase_t; /* Nbit unsigned CPU related data type */

/* Else data type definitions */
typedef os_int32_t os_err_t;   /* Type for error number */
typedef os_ubase_t os_tick_t;  /* Type for tick count */
typedef os_ubase_t os_size_t;  /* Type for size */
typedef os_base_t  os_ssize_t; /* Type for ssize */
typedef os_base_t  os_off_t;   /* Type for offset */

/* Alignment size for CPU architecture data access */
#ifndef OS_ALIGN_SIZE
#define OS_ALIGN_SIZE sizeof(os_base_t)
#endif

#ifdef __cplusplus
}
#endif

#endif
