#ifndef OS_H
#define OS_H

#include <stdint.h>
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>

typedef HANDLE os_thread_t;
typedef HANDLE os_mutex_t;
typedef HANDLE os_sem_t;

#define os_thread_create(thread, attr, start_routine, arg) \
    (*thread = (HANDLE)_beginthreadex(NULL, 0, (unsigned (__stdcall *)(void*))start_routine, arg, 0, NULL))

#define os_thread_join(thread, retval) \
    WaitForSingleObject(thread, INFINITE)

#define os_mutex_create(mutex) \
    (*mutex = CreateMutex(NULL, FALSE, NULL))

#define os_mutex_lock(mutex) \
    WaitForSingleObject(*mutex, INFINITE)

#define os_mutex_unlock(mutex) \
    ReleaseMutex(*mutex)

#define os_mutex_delete(mutex) \
    CloseHandle(*mutex)

#define os_sem_create(sem, initial_count) \
    (*sem = CreateSemaphore(NULL, initial_count, 1000, NULL))

#define os_sem_wait(sem) \
    WaitForSingleObject(*sem, INFINITE)

#define os_sem_post(sem) \
    ReleaseSemaphore(*sem, 1, NULL)

#define os_sem_delete(sem) \
    CloseHandle(*sem)

#define os_malloc(size) malloc(size)
#define os_free(ptr) free(ptr)

#define os_memcpy(dst, src, len) memcpy(dst, src, len)
#define os_memset(ptr, value, len) memset(ptr, value, len)

#define os_time_ms() GetTickCount()

#define os_delay_ms(ms) Sleep(ms)

#else

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

typedef pthread_t os_thread_t;
typedef pthread_mutex_t os_mutex_t;
typedef sem_t os_sem_t;

#define os_thread_create(thread, attr, start_routine, arg) \
    pthread_create(thread, attr, start_routine, arg)

#define os_thread_join(thread, retval) \
    pthread_join(*thread, retval)

#define os_mutex_create(mutex) \
    pthread_mutex_init(mutex, NULL)

#define os_mutex_lock(mutex) \
    pthread_mutex_lock(mutex)

#define os_mutex_unlock(mutex) \
    pthread_mutex_unlock(mutex)

#define os_mutex_delete(mutex) \
    pthread_mutex_destroy(mutex)

#define os_sem_create(sem, initial_count) \
    sem_init(sem, 0, initial_count)

#define os_sem_wait(sem) \
    sem_wait(sem)

#define os_sem_post(sem) \
    sem_post(sem)

#define os_sem_delete(sem) \
    sem_destroy(sem)

#define os_malloc(size) malloc(size)
#define os_free(ptr) free(ptr)

#define os_memcpy(dst, src, len) memcpy(dst, src, len)
#define os_memset(ptr, value, len) memset(ptr, value, len)

static inline uint32_t os_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

#define os_delay_ms(ms) usleep(ms * 1000)

#endif

#endif /* OS_H */