#ifndef IOT_MEM_LINUX_H
#define IOT_MEM_LINUX_H

#include <stdlib.h>

#define iot_malloc(size) \
    malloc((size_t)(size))

#define iot_calloc(nmemb, size) \
    calloc((size_t)(nmemb), (size_t)(size))

#define iot_realloc(ptr, size) \
    realloc((void *)(ptr), (size_t)(size))

#define iot_free(ptr) \
    free((void *)(ptr))

#endif