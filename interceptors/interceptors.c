/* This file is in public domain */

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

/*
 * VARIOUS helpers:
 *
 * Add this library to your target via LD_PRELOAD
 */

/*
 * This function lets exit() on panic instead of aborting()
 * It's useful when you want to ignore panics() during a fuzzing 
 * process e.g.:
 *
 * RUMP_IGNORE_PANIC=1 afl-fuzz ...
 */

void
rumpuser_exit(int rv)
{
    if (getenv("RUMP_IGNORE_PANIC") || rv != -1)
        exit(rv);
    else
        abort();
}

/*
 * Interceptors
 *
 * Rump by default renames symbols to avoid collisions by adding
 * the rumpns_ prefix. ASAN is not aware that rumpns_memcpy is a
 * memcpy function, so we simply wrap them to let ASAN intercept
 * such calls.
 */

/* 
   DECLARE_REAL(int, memcmp, const void *a1, const void *a2, uptr size)
   DECLARE_REAL(void*, memcpy, void *to, const void *from, uptr size)
   DECLARE_REAL(void*, memset, void *block, int c, uptr size)
   DECLARE_REAL(char*, strchr, const char *str, int c)
   DECLARE_REAL(uptr, strlen, const char *s)
   DECLARE_REAL(char*, strncpy, char *to, const char *from, uptr size)
   DECLARE_REAL(uptr, strnlen, const char *s, uptr maxlen)
   DECLARE_REAL(char*, strstr, const char *s1, const char *s2)

   it's taken from the gcc repository, XXX: check the clang's one.
*/

int
rumpns_memcmp(const void *a1, const void *a2, size_t size)
{
    return memcmp(a1, a2, size);
}

char *
rumpns_strchr(const char *str, int c)
{
    return strchr(str, c);
}

size_t
rumpns_strlen(const char *s)
{
    return strlen(s);
}

char *
rumpns_strncpy(char *to, const char *from, size_t size)
{
    return strncpy(to, from, size);
}

size_t
rumpns_strnlen(const char *s, size_t maxlen)
{
    return strnlen(s, maxlen);
}

char* rumpns_strstr(const char *s1, const char *s2)
{
    return strstr(s1, s2);
}

void *
rumpns_memset(void *s, int c, size_t n)
{
    return memset(s, c, n);
}

void *
rumpns_memcpy(void *a, void *b, int n)
{
    return memcpy(a, b, n);
}
