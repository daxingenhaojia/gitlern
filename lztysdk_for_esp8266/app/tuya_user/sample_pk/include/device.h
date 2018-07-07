/***********************************************************
*  File: device.h 
*  Author: nzy
*  Date: 20150605
***********************************************************/
#ifndef _DEVICE_H
    #define _DEVICE_H

    #include "sys_adapter.h"
    #include "error_code.h"
    
#ifdef __cplusplus
	extern "C" {
#endif

#ifdef  __DEVICE_GLOBALS
    #define __DEVICE_EXT
#else
    #define __DEVICE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
// device information define
#define SW_VER USER_SW_VER
//#define PRODECT_KEY "XPpKfY2eyFN7rt7w" // ²ÊµÆ
//#define PRODECT_KEY "1mUEvwutNqKKaAxd" // ÏÈ·æÓÍÍ¡
#define PRODECT_KEY "PjK3wjTrPflaFX8p" // Í¿Ñ»²âÊÔ30-dp
//#define PRODECT_KEY "ZH0QJsansRET4bTW" // Í¿Ñ»Ñ¹²â²úÆ·
//#define PRODECT_KEY "htJtbmizXPsW42cz"
//#define PRODECT_KEY "O95Mw3t2MZc7a8eh" // Ë®Ê±´ú
//#define PRODECT_KEY "1KKA7FrKxaPgajO1" // ¼Ò±ÌË®Ê±´ú²âÊÔ
//#define PRODECT_KEY "2dROy1OLYJDXK1Mz" // °×µÆ
//#define PRODECT_KEY "hABOtJIkr2FnT5KO" // ¸ßÉ­²âÊÔ°æ
//#define PRODECT_KEY "4Ic6GgKI0vUG65Jd" // °®±´Ô´ÄÌ»ú
#define DEF_DEV_ABI DEV_SINGLE

/***********************************************************
*************************variable define********************
***********************************************************/


/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: device_init
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
__DEVICE_EXT \
OPERATE_RET device_init(VOID);


#ifdef __cplusplus
}
#endif
#endif

