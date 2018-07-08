#ifndef COROUTINE_H
#define COROUTINE_H

//#include "log.h"

#if defined(__ESP8266__)
#define COR_LINE __LINE__
#else
#define COR_LINE 0
#endif
/*cann't use in switch*/
/*line save extern param*/
#if defined(__ESP8266__)
#define ecr_init(line,timer)			unsigned int *ecr_line=line; \
										ETSTimer *ecr_timer=timer;

#define ecr_begin(func,arg)					switch(*ecr_line) { case 0: \
														os_timer_disarm(ecr_timer);\
														os_timer_setfn(ecr_timer, (os_timer_func_t *)func, arg);

#define ecr_yield_noret				do {\
											*ecr_line=COR_LINE;\
											return; case COR_LINE:;\
										} while (0)

#define ecr_delay_ms(time)		os_timer_arm(ecr_timer, time, 0);ecr_yield_noret

#define ecr_finish_noret		} *ecr_line=0;return

#define ecr_finish				*ecr_line=0


/*line save extern static param*/
#define scr_init(timer,func,arg) os_timer_disarm(timer),os_timer_setfn(timer, (os_timer_func_t *)func, arg)

#define scr_begin         static unsigned int scrLine = 0; switch(scrLine) { case 0:;
#define scr_finish(z)     } return (z)
#define scr_finish_noret       } scrLine=0;return
#define scr_exit			scrLine=0;return

#define scr_yield(z)     \
        do {\
            scrLine=COR_LINE;\
            return (z); case COR_LINE:;\
        } while (0)
#define scr_yield_noret       \
        do {\
            scrLine=COR_LINE;\
            return; case COR_LINE:;\
        } while (0)
#define scr_delay_ms(timer,time)	os_timer_arm(timer, time, 0);scr_yield_noret
#else
#define ecr_init(line,timer)
#define ecr_begin(func,arg)
#define ecr_yield_noret
#define ecr_delay_ms(time)
#define ecr_finish_noret
#define ecr_finish\
/*line save extern static param*/
#define scr_init(timer,func,arg)
#define scr_begin
#define scr_finish(z)
#define scr_finish_noret
#define scr_exit

#define scr_yield(z)
#define scr_yield_noret
#define scr_delay_ms(timer,time)
#endif

#endif 
