#ifndef _C_SPRINTF_H_
#define _C_SPRINTF_H_

#define	EOF	(-1)

#ifdef __BUFSIZ__
#define   BUFSIZ         __BUFSIZ__
#else
#define   BUFSIZ         1024
#endif

//extern void output_redirect(const char *str);
//#define c_puts output_redirect
#define c_puts os_printf

#if defined( LUA_NUMBER_INTEGRAL )
	#define c_sprintf os_sprintf
#else

	#if defined(__GNUC__)
	#include <stdarg.h>
	#else
	typedef char * va_list;
	#define _INTSIZEOF(n)   ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1)) 
	#define va_start(ap,v)  (ap = (va_list)&v + _INTSIZEOF(v))
	#define va_arg(ap,t)    (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
	#define va_end(ap)      (ap = (va_list)0)
	#endif

	void c_sprintf(char* s,char *fmt, ...);
#endif

// #define c_vsprintf ets_vsprintf
#define c_printf(...) do {					\
	unsigned char __print_buf[BUFSIZ];		\
	c_sprintf(__print_buf, __VA_ARGS__);	\
	c_puts(__print_buf);					\
} while(0)

#endif /* _C_STDIO_H_ */
