#ifndef __ESP_COMMON_H__
#define __ESP_COMMON_H__
#include "user_config.h"

#include <ra_types.h>
#if defined(__ESP8266__)
#include "esp_miss_header.h"
#include <osapi.h>
#include <c_types.h>
#include <ets_sys.h>
#include <os_type.h>
#include <mem.h>
#include <user_interface.h>
#include <spi_flash.h>
#include <espconn.h>
#include "uart.h"
#include <gpio.h>
#include <gpio16.h>
#include <smartconfig.h>
#else
#include <libesp8266emu.h>
#include "uart.h"
#endif

#include <RCLutils.h>

/*PLATFORM DEFINE */
#if defined(__ESP8266__)
#define ALIGNED(N)		__attribute__((aligned(N)))
#define FUN_ATTRIBUTE		__attribute__((section(".irom0.text")))
#define RODATA_ATTRIBUTE	static const __attribute__((section(".irom.text")))
#else
#define FUN_ATTRIBUTE
#define ALIGNED(N)
#define RODATA_ATTRIBUTE static
#define inline
#endif
/*END*/



#if defined(__ESP8266__)
/*lib define*/
#define memcpy		ets_memcpy
#define memset		ets_memset
#define memcmp		ets_memcmp
#define memchr		lib_memchr
#define strcmp		ets_strcmp
#define strlen		ets_strlen
#define strnlen		lib_strnlen
#define strncmp		ets_strncmp
#define sprintf		ets_sprintf
#define printf		ets_printf
#define strcpy		ets_strcpy
#define strncpy		ets_strncpy
#define strchr		ets_strchr
#define strstr		ets_strstr
#define strcat		ets_strcat
#define vsprintf	ets_vsprintf
#define vsnprintf	ets_vsnprintf
#define snprintf	ets_snprintf
#else
#include <stdlib.h>
#include <stdio.h>
#endif

#ifndef __WINDOWS32__
/*define memory leak check*/
void *malloc_detector(size_t size, const char *file, size_t line);
void *zalloc_detector(size_t size, const char *file, size_t line);
void *realloc_detector(void *p, size_t size, const char *file, size_t line);
void free_detector(void *addr);

#if defined(__MEM_LEAK_CHECK_ENABLE__)
#define malloc(size)	malloc_detector(size,__FILE__,__LINE__)
#define zalloc(size)	zalloc_detector(size,__FILE__,__LINE__)
#define realloc(p,size)	realloc_detector(p,size,__FILE__,__LINE__)
#define free(p)			free_detector(p)
#else
#define malloc	os_malloc
#define realloc	os_realloc
#define zalloc	os_zalloc
#define free	os_free
#endif
/*end define */

#endif // !__WINDOWS32__

/*N Must be a power of 2*/
#define NUM_ALIGNED(NUM,N) (NUM + ((-((sint64_t )NUM)) & ((N) - 1)))

#endif
