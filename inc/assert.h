/* See COPYRIGHT for copyright information. */

#ifndef JOS_INC_ASSERT_H
#define JOS_INC_ASSERT_H

#include <inc/stdio.h>

#include <inc/ansiterm.h>

void _warn(const char*, int, const char*, ...);
void _panic(const char*, int, const char*, ...) __attribute__((noreturn));
void _log(const char *file, int line, const char * color, const char *fmt, ...);

#define warn(...) _warn(__FILE__, __LINE__, __VA_ARGS__)
#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)
#define logd(...) _log(__FILE__, __LINE__, AT_BRI_BLK, __VA_ARGS__)
#define logi(...) _log(__FILE__, __LINE__, AT_BRI_GRN, __VA_ARGS__)
#define logp(...) _log(__FILE__, __LINE__, AT_BRI_MAG, __VA_ARGS__)
#define logw(...) _log(__FILE__, __LINE__, AT_BRI_YLW, __VA_ARGS__)
#define loge(...) _log(__FILE__, __LINE__, AT_BRI_RED, __VA_ARGS__)

#define assert(x)		\
	do { if (!(x)) panic("assertion failed: %s", #x); } while (0)

// static_assert(x) will generate a compile-time error if 'x' is false.
#define static_assert(x)	switch (x) case 0: case (x):

#endif /* !JOS_INC_ASSERT_H */
