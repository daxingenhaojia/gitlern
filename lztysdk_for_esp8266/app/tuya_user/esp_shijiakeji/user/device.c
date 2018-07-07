/***********************************************************
*  File: device.c 
*  Author: Wangwei
*  项目：拾家科技
*  Date: 20171011
***********************************************************/
#define __DEVICE_GLOBALS
#include "device.h"
#include "mem_pool.h"
#include "smart_wf_frame.h"
#include "key.h"
#include "led_indicator.h"
#include "system/sys_timer.h"
#include "system/uni_thread.h"
#include "smart_link.h"
#include "uni_pointer.h"
#include "queue.h"
#include "uart.h"
#include "wf_sdk_adpt.h"
#include "system/uni_semaphore.h"

//#pragma pack(1)
//#pragma pack()

#define DEVICE_MOD       "device_mode" 
#define DEVICE_PART      "device_part_mode"
#define PROD_TEST_KEY    "prod_test"

#define FR_HEAD             AIRCONT_DESICCANT    //AIRCONT_DESICCANT
#define NOT_PROTEST      0
#define NOW_PROTEST      1
#define POWER_CLOSE      2
#define POWER_OPEN       1

#define STOP_COUNT      (-1)

#define WF_DP_LED         GPIO_ID_PIN(5)
#define WF_DP_KEY         GPIO_ID_PIN(0)


typedef struct
{
  UCHAR R_CMD;
  void (*Function)(REV_DATA_S*);

}TypeRecvFunction;

typedef struct {
    
    THRD_HANDLE hand;
    P_QUEUE_CLASS snd_que;
    TIMER_ID snd_tmout;             // cmd timer out
    TIMER_ID snd_que_tm;
	TIMER_ID prot_test_tm;
    BYTE timeout_cnt;
    BYTE rv_buf[DATA_BUF_SZ];
    BYTE in;                    // data in pos
    MUTEX_HANDLE mutex;
	UCHAR     mach_type;
	UCHAR     pack_number;

}UART_PROC_S;


typedef struct {

  UCHAR      POWER; 
  UCHAR      PRO_TEST_STA;
  BOOL       dp_game_str_val;
  BOOL       dp_grap_val;
}GLO_STR;

CONST CHAR *DP_CMD[11] = { "101", "102", "103","104","105","106","111","112","113","115","114"};

STATIC GLO_STR      glo_data = {0};
STATIC UART_PROC_S  uart_proc = {0};


LED_HANDLE wf_light = NULL;

STATIC DWORD CHANCE_NUM = 0;

/***********************************************************
*************************function define********************
***********************************************************/
STATIC VOID wfl_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC VOID snd_tmout_cb(UINT timerID,PVOID pTimerArg);
STATIC VOID snd_que_tm_cb(UINT timerID,PVOID pTimerArg);
STATIC OPERATE_RET init_prod_info(BYTE mch_tp);
STATIC OPERATE_RET ur_send_data(BYTE *data,BYTE len);
STATIC OPERATE_RET stat_data_proc(REV_DATA_S *prot_da);
STATIC OPERATE_RET set_prod_flag_proc(INT state);
STATIC OPERATE_RET get_prod_flag_proc(INT *state);
STATIC void  Recv_cjsondata(cJSON *inroot);
STATIC void Dp_data_handle(cJSON *indta ,UCHAR DP_count);
STATIC WORD hex_to_Decima(WORD addr );
STATIC VOID wifi_rssi_scan(OPERATE_RET *ret,CHAR *rssi);
STATIC OPERATE_RET ur_send_set_in_que(BYTE *buff, BYTE d_cmd);
STATIC VOID del_queue_buf_cmd(CMT_T cmd);
STATIC VOID del_queue_buf_len(VOID );

STATIC OPERATE_RET ur_send_quer_in_que(BYTE *buff);
STATIC OPERATE_RET ur_send_GETquer_in_que(VOID);

STATIC OPERATE_RET device_differ_init(VOID);
STATIC VOID key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt);
STATIC VOID del_queue_buf(VOID);
STATIC OPERATE_RET dp_upload_proc(CHAR *jstr);
STATIC VOID Pro_test_timer_cb(UINT timerID,PVOID pTimerArg);
STATIC void  Host_game_status(REV_DATA_S *data);


STATIC void  Host_game_over(REV_DATA_S *data)
{
   
     if(NULL == data) {
        PR_ERR("Host_Recv_status data err");
        return ;   
     }
     
     if(STAT_WORK != get_gw_status()) {  
         PR_ERR("get_gw_status err");
         return ;
     }
    
    cJSON *obj_json = NULL;
    obj_json = cJSON_CreateObject();
    if(NULL == obj_json) {
        PR_ERR("Recv_repot_data_proc cJSON_CreateObject err");
        return ;
    }
    cJSON_AddBoolToObject(obj_json,DP_CMD[7],true);  
    
    BYTE *tmp_data_buf = cJSON_PrintUnformatted(obj_json);
    cJSON_Delete(obj_json);
    if(NULL == tmp_data_buf) {
        PR_ERR("cJSON_PrintUnformatted err");
        return ;
    }
    OPERATE_RET op_ret = sf_obj_dp_trans_report(get_single_wf_dev()->dev_if.id,tmp_data_buf);
    if(OPRT_OK != op_ret) {
     PR_ERR("sf_obj_dp_report err:%s",tmp_data_buf);       
     Free(tmp_data_buf);
     return;
    }
    Free(tmp_data_buf);
    return ;

}



STATIC void  Host_grap_reply(REV_DATA_S *data)
{
   
     if(NULL == data) {
        PR_ERR("Host_Recv_status data err");
        return ;   
     }
     del_queue_buf_cmd(0x07);
     if(STAT_WORK != get_gw_status()) {  
         PR_ERR("get_gw_status err");
         return ;
     }
    
    cJSON *obj_json = NULL;
    obj_json = cJSON_CreateObject();
    if(NULL == obj_json) {
        PR_ERR("Recv_repot_data_proc cJSON_CreateObject err");
        return ;
    }
    cJSON_AddBoolToObject(obj_json,DP_CMD[5],glo_data.dp_grap_val);  
    
    BYTE *tmp_data_buf = cJSON_PrintUnformatted(obj_json);
    cJSON_Delete(obj_json);
    if(NULL == tmp_data_buf) {
        PR_ERR("cJSON_PrintUnformatted err");
        return ;
    }
    OPERATE_RET op_ret = sf_obj_dp_report(get_single_wf_dev()->dev_if.id,tmp_data_buf);
    if(OPRT_OK != op_ret) {
     PR_ERR("sf_obj_dp_report err:%s",tmp_data_buf);       
     Free(tmp_data_buf);
     return ;
    }
    Free(tmp_data_buf);
    return ;

}

STATIC void  Host_grap_success(REV_DATA_S *data)
{
   
     if(NULL == data) {
        PR_ERR("Host_Recv_status data err");
        return ;   
     }
     
     if(STAT_WORK != get_gw_status()) {  
         PR_ERR("get_gw_status err");
         return ;
     }
    
     
    cJSON *obj_json = NULL;
    obj_json = cJSON_CreateObject();
    if(NULL == obj_json) {
        PR_ERR("Recv_repot_data_proc cJSON_CreateObject err");
        return ;
    }
    cJSON_AddBoolToObject(obj_json,DP_CMD[6],true);  
    
    BYTE *tmp_data_buf = cJSON_PrintUnformatted(obj_json);
    cJSON_Delete(obj_json);
    if(NULL == tmp_data_buf) {
        PR_ERR("cJSON_PrintUnformatted err");
        return ;
    }
    OPERATE_RET op_ret = sf_obj_dp_trans_report(get_single_wf_dev()->dev_if.id,tmp_data_buf);
    if(OPRT_OK != op_ret) {
     PR_ERR("sf_obj_dp_report err:%s",tmp_data_buf);       
     Free(tmp_data_buf);
     return ;
    }
    Free(tmp_data_buf);
    return ;

}



const TypeRecvFunction RecvFunctionArray[]=
{

    { CMD_R_GAME_START,   Host_game_status     },
    { CMD_R_ROOT_GRAB,    Host_grap_reply      },
    { CMD_R_GRAB_SUCC,    Host_grap_success    },
    { CMD_R_GAME_OVER,    Host_game_over       },
};


STATIC void  Host_game_status(REV_DATA_S *data)
{

     if(NULL == data) {
        PR_ERR("Host_Recv_status data err");
        return ;   
     }
     del_queue_buf_cmd(0x01);
     if(STAT_WORK != get_gw_status()) {  
         PR_ERR("get_gw_status err");
         return ;
     }
     cJSON *obj_json = NULL;
    
    obj_json = cJSON_CreateObject();
    if(NULL == obj_json) {
        PR_ERR("Recv_repot_data_proc cJSON_CreateObject err");
        return ;
    }
    cJSON_AddBoolToObject(obj_json,DP_CMD[0],glo_data.dp_game_str_val);  
    
    BYTE *tmp_data_buf = cJSON_PrintUnformatted(obj_json);
    cJSON_Delete(obj_json);
    if(NULL == tmp_data_buf) {
        PR_ERR("cJSON_PrintUnformatted err");
        return ;
    }
    OPERATE_RET op_ret = sf_obj_dp_report(get_single_wf_dev()->dev_if.id,tmp_data_buf);
    if(OPRT_OK != op_ret) {
     PR_ERR("sf_obj_dp_report err:%s",tmp_data_buf);       
     Free(tmp_data_buf);
     return ;
    }
    Free(tmp_data_buf);
    return ;

}

/***********************************************************
*  Function: gpio_func_test
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
BOOL gpio_func_test(VOID)
{
    return TRUE;
}

VOID app_init(VOID)
{
   app_cfg_set(WCM_OLD,NULL);       //SDK2.0接口  
}

VOID set_firmware_tp(IN OUT CHAR *firm_name, IN OUT CHAR *firm_ver)
{
	strcpy(firm_name,APP_BIN_NAME);
	strcpy(firm_ver,USER_SW_VER);
	return;
}


STATIC VOID prod_timer_cb(UINT timerID,PVOID pTimerArg)
{

    PR_DEBUG(" Protest cb is reset is over \r\n"); 
    single_dev_reset_factory();  
      
}


STATIC VOID Pro_test_timer_cb(UINT timerID,PVOID pTimerArg)
{

     if(ws_exist_auzkey()==FALSE){
        PR_ERR("Pro_test ws_exist_auzkey is false\r\n");
     }else{
       start_wifi_function_test(PRODECT_TEST_NAME,wifi_rssi_scan);
     }
      
}





STATIC OPERATE_RET dp_upload_proc(CHAR *jstr)
{
    PR_DEBUG("dp_upload_proc: %s", jstr);
    OPERATE_RET op_ret;
    BYTE gw_status=0;
    
    gw_status=get_gw_status();
    if(gw_status != STAT_WORK) {
        PR_ERR("dp_trans_upload_proc gw_status:%d",gw_status);
        return op_ret;
    }
    op_ret = sf_obj_dp_report(get_single_wf_dev()->dev_if.id,jstr);
    if(OPRT_OK != op_ret) {
        PR_ERR("sf_obj_dp_report op_ret:%d",op_ret);
        return op_ret;
    }else {
        return OPRT_OK;
    }
}



STATIC OPERATE_RET set_prod_flag_proc(INT state)
{
    OPERATE_RET   op_ret;
    cJSON *root = NULL;
    UCHAR *buf = NULL;

    root = cJSON_CreateObject();
    if(NULL == root) {
        PR_ERR("cJSON_CreateObject error");
        return OPRT_CJSON_GET_ERR;
    }
    
    cJSON_AddNumberToObject(root, "prod_flag", state);
    buf = cJSON_PrintUnformatted(root);
    if(NULL == buf) {
        PR_ERR("buf is NULL");
        cJSON_Delete(root);
        return OPRT_COM_ERROR;
    }    
    cJSON_Delete(root);
    
    op_ret = msf_set_single(DEVICE_MOD,PROD_TEST_KEY,buf);
	Free(buf);
    if(OPRT_OK != op_ret) {
        PR_DEBUG("msf_set_single err:%02x",op_ret);
        return op_ret;
    }

    return OPRT_OK;    
}

STATIC OPERATE_RET get_prod_flag_proc(INT *state)
{
    OPERATE_RET op_ret;
    cJSON *root = NULL, *json = NULL;
    UCHAR *buf;

    buf = (UCHAR *)Malloc(256);
    if(NULL == buf) {
        PR_ERR("malloc error");
        return OPRT_MALLOC_FAILED;
    }

    op_ret = msf_get_single(DEVICE_MOD,PROD_TEST_KEY,buf,256);
    if(OPRT_OK != op_ret) {
        PR_DEBUG("msf_get_single err:%0d",op_ret);
        Free(buf);
        return op_ret;
    }

    root = cJSON_Parse(buf);
    if(NULL == root) {
        PR_ERR("cjson parse");
        goto JSON_PARSE_ERR;
    }

    json = cJSON_GetObjectItem(root,"prod_flag");
    if(NULL == json) {
        PR_ERR("cjson get ");
        goto JSON_PARSE_ERR;
    }

    *state = json->valueint;
    cJSON_Delete(root);
    Free(buf);
    return OPRT_OK;

JSON_PARSE_ERR:
    cJSON_Delete(root);
    Free(buf);
    return OPRT_COM_ERROR;
}

/***********************************************************
*  Function: sf_get_dp_cntl
*  Input: id or dev_cntl use to get dev_cntl
*  Output: 
*  Return: none
*  Note: none
***********************************************************/
STATIC DP_CNTL_S *__sf_get_dp_cntl(IN CONST CHAR *id,\
                                   IN CONST DEV_CNTL_N_S *dev_cntl,\
                                   IN CONST BYTE dpid)
{
    if(NULL == id && NULL == dev_cntl) {
        return NULL;
    }

    DEV_CNTL_N_S *tmp_dev_cntl = (DEV_CNTL_N_S *)dev_cntl;
    if(NULL == tmp_dev_cntl) {
        tmp_dev_cntl = get_dev_cntl(id);
    }
    
    if(NULL == tmp_dev_cntl) {
        return NULL;
    }

    INT i;
    for(i = 0;i < tmp_dev_cntl->dp_num;i++) {
        if(tmp_dev_cntl->dp[i].dp_desc.dp_id == dpid) {
            return &(tmp_dev_cntl->dp[i]);
        }
    }

    return NULL;
}

STATIC VOID del_queue_buf(VOID)
{

    UR_SEND_S *send = NULL;
    GetQueueMember(uart_proc.snd_que,1,(unsigned char *)(&send),1);
    if(NULL != send) {
        DelQueueMember(uart_proc.snd_que,1);        
        Free(send);
        PR_DEBUG("del_queue is OK \r\n");
    }else {
        PR_ERR("null queue");
    }
}


STATIC VOID del_queue_buf_cmd(CMT_T cmd )
{

      UR_SEND_S *send = NULL; 
      sys_stop_timer(uart_proc.snd_tmout);
      GetQueueMember(uart_proc.snd_que,1,(unsigned char *)(&send),1);
      if(NULL != send) {
        if(send->data[4]!= cmd){
            PR_DEBUG("del_queue_buf_by_cmd is err\r\n") ;  
          }else{
             DelQueueMember(uart_proc.snd_que,1);
             Free(send);              //这里释放内存
             uart_proc.timeout_cnt = 0;
          }

       }
        
}

STATIC VOID del_queue_buf_len(VOID )
{
	UR_SEND_S *send = NULL; 
      sys_stop_timer(uart_proc.snd_tmout);
      GetQueueMember(uart_proc.snd_que,1,(unsigned char *)(&send),1);
      if(NULL != send) {
        if(send->len!= 34){
			DelQueueMember(uart_proc.snd_que,1);
             Free(send);              //这里释放内存
             uart_proc.timeout_cnt = 0;
            PR_DEBUG("del_queue_buf_by_cmd is err\r\n") ;  
          }else{
             DelQueueMember(uart_proc.snd_que,1);
             Free(send);              //这里释放内存
             uart_proc.timeout_cnt = 0;
          }

       }
}


STATIC VOID snd_tmout_cb(UINT timerID,PVOID pTimerArg)
{
    uart_proc.timeout_cnt++;
    if(uart_proc.timeout_cnt < TIMEOUT_CNT_LMT) {
        return;
    }else {
       PR_DEBUG("timeout,delete queue");
       uart_proc.timeout_cnt = 0;
       del_queue_buf();
    }
}



STATIC VOID snd_que_tm_cb(UINT timerID,PVOID pTimerArg)
{

    if(0 == GetCurQueNum(uart_proc.snd_que)) {
        return;
    }
    if(TRUE == IsThisSysTimerRun(uart_proc.snd_tmout)) {
        return;
    }
    
    UR_SEND_S *send = NULL;
    GetQueueMember(uart_proc.snd_que,1,(unsigned char *)(&send),1);
    if(OPRT_OK != ur_send_data(send->data,send->len)) {
        PR_DEBUG("ur_send_data if Err\r\n");
        Free(send);
        return;
    }
 
    sys_start_timer(uart_proc.snd_tmout,CMD_TIMEOUT_MS,TIMER_ONCE);
    


} 


STATIC WORD hex_to_Decima(WORD addr )
{
    WORD  Decima=0;
    Decima=((addr>>4)*10)+(addr&0x0F);
    return Decima;
}


STATIC BYTE GET = 0;

STATIC void Dp_data_handle(cJSON *indta ,UCHAR DP_count)
{

    OPERATE_RET op_ret;
    BYTE Set_buff[P_DATA_LEN]={0};
	BYTE chance_buff[1]={0};
    CONST UCHAR Cmd_data[2] ={0x77,0x77};
    BYTE Vaile=0;
	BYTE CHANCE = 0;
	GET = 0;
    
   
    if(NULL==indta){
      PR_ERR("Dp_data_handle *indta is null");
      return;
    }
    if(indta->type==cJSON_False){
       Vaile=false;
       PR_DEBUG(" cJSON_False Vaile=:%d", Vaile);
    }
    else if(indta->type==cJSON_True){
       Vaile=true;
       PR_DEBUG("cJSON_True Vaile=:%d", Vaile);
    }else{
       PR_ERR("Dp_data_handle  indta->type is err");
       //return;
    }
    memcpy(Set_buff,Cmd_data,2);
    switch(DP_count)
    {
          case DP_GAME_START:  
          {  
                  
                glo_data.dp_game_str_val=Vaile;
                Set_buff[P_DATA_LEN-1]=CMD_GAME_START;
                break;
          } 
          case DP_ROOT_FRONT_STOP:     
          {   
                 if(Vaile){
                   Set_buff[P_DATA_LEN-1]=CMD_ROOT_FRONT;
                 }else{
                   Set_buff[P_DATA_LEN-1]=CMD_ROOT_FSTOP;
                 } break;       
          }     
          case DP_ROOT_BACK_STOP:      
          {  
                 if(Vaile){
                   Set_buff[P_DATA_LEN-1]=CMD_ROOT_BACK;
                 }else{
                   Set_buff[P_DATA_LEN-1]=CMD_ROOT_BSTOP;
                 } break;    
          }    
          case DP_ROOT_LEFT_STOP:      
          { 
                 if(Vaile) {
                    Set_buff[P_DATA_LEN-1]=CMD_ROOT_LEFT;
                 }else{
                    Set_buff[P_DATA_LEN-1]=CMD_ROOT_LSTOP;
                 }break; 
          }  
          case DP_ROOT_RIGHT_STOP:     
          { 
                   if(Vaile) {
                      Set_buff[P_DATA_LEN-1]=CMD_ROOT_RIGHT;
                   }else{
                      Set_buff[P_DATA_LEN-1]=CMD_ROOT_RSTOP;
                   }break; 
          }  
          case DP_ROOT_GRAB: 
          { 
               glo_data.dp_grap_val=Vaile;
               Set_buff[P_DATA_LEN-1]=CMD_ROOT_GRAB;
               break; 
          }
		  case DP_CHANGE_CHANCE:
		  {
		  		CHANCE = 1;
				PR_DEBUG("Vaile = %d",indta->valueint);
		  		chance_buff[0]=indta->valueint;
				break;
		  }
		  case DP_GET_CHANCE:
		  	{
		  		if(Vaile == 0) GET=1;
				break;
		  	}
          default:
          return;

     }
     if(CHANCE == 1)
     {op_ret = ur_send_quer_in_que(chance_buff);}
	 else if(GET == 1){op_ret=ur_send_GETquer_in_que();}
	 else op_ret= ur_send_set_in_que(Set_buff,P_DATA_LEN);
     if(op_ret!=OPRT_OK){	 	
        PR_ERR("set_in_que ERR:%d", op_ret);
     }
     

}


STATIC void  Recv_cjsondata(cJSON *inroot)
{

   cJSON *nxtr = inroot->child; 
   UCHAR  dpid=0;
   
   if(NULL==nxtr){
      PR_ERR("Recv_cjsondata nxtr is NULL \r\n");
      return;
   }
   while(nxtr){

        dpid = atoi(nxtr->string);
        
        PR_DEBUG("dpid:%d", dpid);
   
        Dp_data_handle(nxtr, dpid); 

        nxtr = nxtr->next; 
   }

   cJSON_Delete(nxtr);
}


VOID device_cb(SMART_CMD_E cmd,cJSON *root)
{

     CHAR *out = cJSON_PrintUnformatted(root);

     if(out==NULL){
       PR_ERR("device_cb out is NULL \r\n");
       return;
     }
     PR_DEBUG("device_cb out:%s \r\n",out);
     Free(out);    
     
     if(GetCurQueNum(uart_proc.snd_que)>(QUE_NUM_SZ)) {
        PR_DEBUG( " device_cb recv data snd_que is max:%d\r\n",GetCurQueNum(uart_proc.snd_que));
        return;
     }
     
     Recv_cjsondata(root);
     
}



STATIC OPERATE_RET ur_send_quer_in_que(BYTE *buff)
{

    BYTE  ret = 0;
    
    UR_SEND_S *send = (UR_SEND_S *)Malloc(SIZEOF(UR_SEND_S)+ SIZEOF(glo_quer_str) + 1); 
    if(NULL == send) {
        PR_ERR("ur_send_quer_in_que *send is NULL \r\n");
        return OPRT_MALLOC_FAILED;
    }
    glo_quer_str *quer_p = (glo_quer_str *)((BYTE *)send+SIZEOF(UR_SEND_S));
    if(NULL == quer_p) {
          PR_ERR("ur_send_quer_in_que *quer_p is NULL \r\n");
          return OPRT_MALLOC_FAILED;
     }
     
    quer_p->P_head[0]=0x24;
    quer_p->P_head[1]=0x40;
	quer_p->P_head[2]=0x64;
	quer_p->P_head[3]=0x64;
	quer_p->P_head[4]=0x1a;
	quer_p->P_head[5]=0x03;
	quer_p->P_head[6]=0x02;
    quer_p->P_cmd[0] =*buff;             //概率位
    quer_p->P_len3[0]=0x01;
    memset(quer_p->P_len0,0,SIZEOF(quer_p->P_len0));
	quer_p->P_len1[0]=0x01;
	quer_p->P_len1[1]=0x02;
	quer_p->P_len1[2]=0x03;
	quer_p->P_len1[3]=0x04;
	quer_p->P_len1[4]=0x05;
	quer_p->P_len1[5]=0x06;
	quer_p->P_sum[0]=0x1d^(*buff);	//异或校验
    quer_p->P_end[0]=0x0d;
	quer_p->P_end[1]=0x0a;
    
    send->len = SIZEOF(glo_quer_str);
    ret = InQueue(uart_proc.snd_que, (const unsigned char *)(&send),1);
    if(0 == ret) {
         Free(send);
         return OPRT_COM_ERROR;
    }
    return OPRT_OK;

}

STATIC OPERATE_RET ur_send_GETquer_in_que(VOID)
{
	
	BYTE  ret = 0;
    
    UR_SEND_S *send = (UR_SEND_S *)Malloc(SIZEOF(UR_SEND_S)+ SIZEOF(GET_quer_str) + 1); 
    if(NULL == send) {
        PR_ERR("ur_send_quer_in_que *send is NULL \r\n");
        return OPRT_MALLOC_FAILED;
    }
    GET_quer_str *quer_p = (GET_quer_str *)((BYTE *)send+SIZEOF(UR_SEND_S));
    if(NULL == quer_p) {
          PR_ERR("ur_send_quer_in_que *quer_p is NULL \r\n");
          return OPRT_MALLOC_FAILED;
     }
     
   	quer_p->GET_DATA[0]=0x24;
	quer_p->GET_DATA[1]=0x40;
    quer_p->GET_DATA[2]=0x65;
	quer_p->GET_DATA[3]=0x65;
	quer_p->GET_DATA[4]=0x07;
	quer_p->GET_DATA[5]=0x03;
	quer_p->GET_DATA[6]=0x10;
	quer_p->GET_DATA[7]=0x02;
	quer_p->GET_DATA[8]=0x03;
	quer_p->GET_DATA[9]=0x04;
	quer_p->GET_DATA[10]=0x05;
	quer_p->GET_DATA[11]=0x06;
	quer_p->GET_DATA[12]=0x12;
	quer_p->GET_DATA[13]=0x0D;
	quer_p->GET_DATA[14]=0x0A;
	
    send->len = SIZEOF(GET_quer_str);
    ret = InQueue(uart_proc.snd_que, (const unsigned char *)(&send),1);
	PR_DEBUG("get chance is ok \n\r");
    if(0 == ret) {
         Free(send);
         return OPRT_COM_ERROR;
    }
    return OPRT_OK;
}



STATIC OPERATE_RET ur_send_set_in_que(BYTE *buff, BYTE lenth)
{
     BYTE  ret = 0;
     UR_SEND_S *send = (UR_SEND_S *)Malloc(SIZEOF(UR_SEND_S)+ SIZEOF(TY_WIFI_SET) + 1); 
    if(NULL == send) {
         return OPRT_MALLOC_FAILED;
     }
     TY_WIFI_SET *P_stru = (TY_WIFI_SET *)((BYTE *)send+SIZEOF(UR_SEND_S));
     if(NULL == P_stru) {
     
          PR_ERR("ur_send_quer_in_que *quer_p is NULL \r\n");
          return OPRT_MALLOC_FAILED;
     }
     P_stru->pack_head[P_HEAD_LEN-2]  =   0x24;    
     P_stru->pack_head[P_HEAD_LEN-1]  =   0x40;
     memcpy(&(P_stru->pack_data[0]),buff,P_DATA_LEN);
     P_stru->pack_end [P_END_LEN-2]  =    0x0d;
     P_stru->pack_end [P_END_LEN-1]  =    0x0a;

     send->len = SIZEOF(TY_WIFI_SET);
     ret = InQueue(uart_proc.snd_que, (const unsigned char *)(&send),1);
     if(0 == ret) {
          Free(send);
          return OPRT_COM_ERROR;
     }
     return OPRT_OK;
}



STATIC OPERATE_RET ur_send_data(BYTE *data,BYTE len)
{
    if(((NULL == data) && len != 0) || \
       ((data) && len == 0)) {
        return OPRT_INVALID_PARM;
    }

#if UART_TX_DEBUG
{
    PR_DEBUG_RAW("TX:\n");
    int i;
    for(i = 0;i < len;i++) {
        PR_DEBUG_RAW("%02X ",data[i]);
        if(i && ((i % 20) == 0)) {
            PR_DEBUG_RAW("\n");
        }
    }
    PR_DEBUG_RAW("\n");
}
#endif

    MutexLock(uart_proc.mutex);
    user_uart_write_data((uint8 *)data,len);
    MutexUnLock(uart_proc.mutex);
    
    return OPRT_OK;
 
    
}

STATIC VOID Single_reply_processing(CONST UCHAR cmd)
{
      cJSON *obj_json = NULL;
    
     if(STAT_WORK != get_gw_status()) {  
       PR_ERR("get_gw_status err");
       return ;
     }
     obj_json = cJSON_CreateObject();
     if(NULL == obj_json) {
        PR_ERR("Single_reply_processing cJSON_CreateObject err");
        return ;
     }
    switch(cmd){

         case CMD_R_ROOT_FRONT  :{
             del_queue_buf_cmd(CMD_ROOT_FRONT);
             cJSON_AddBoolToObject(obj_json, DP_CMD[1],true);  
             break;
         }
         case CMD_R_ROOT_BACK  :{
             del_queue_buf_cmd(CMD_ROOT_BACK);
             cJSON_AddBoolToObject(obj_json, DP_CMD[2],true);  
             break;
 
         }
         case CMD_R_ROOT_LEFT  :{
             del_queue_buf_cmd(CMD_ROOT_LEFT);
             cJSON_AddBoolToObject(obj_json, DP_CMD[3],true);  
             break;
         }
         case CMD_R_ROOT_RIGHT  :{
            del_queue_buf_cmd(CMD_ROOT_RIGHT);
            cJSON_AddBoolToObject(obj_json, DP_CMD[4],true);  
            break;
         }
         case CMD_R_ROOT_FSTOP  :{
            del_queue_buf_cmd(CMD_ROOT_FSTOP);
            cJSON_AddBoolToObject(obj_json, DP_CMD[1],false); 
            break;
         }
         case CMD_R_ROOT_BSTOP  :{
            del_queue_buf_cmd(CMD_ROOT_BSTOP);
            cJSON_AddBoolToObject(obj_json, DP_CMD[2],false); 
            break;
         }
        case CMD_R_ROOT_LSTOP  :{
            del_queue_buf_cmd(CMD_ROOT_LSTOP);
            cJSON_AddBoolToObject(obj_json, DP_CMD[3],false); 
            break;
         
        }
        case CMD_R_ROOT_RSTOP  :{
            del_queue_buf_cmd(CMD_ROOT_RSTOP);
            cJSON_AddBoolToObject(obj_json, DP_CMD[4],false); 
            break;
        }
		case CMD_R_MACHINE_CHANCE :{
			del_queue_buf_len();
			PR_DEBUG("CMD_R_MACH  is ok \r\n ");
			cJSON_AddNumberToObject(obj_json, DP_CMD[9],CHANCE_NUM); 
            break;
		}
		case CMD_R_MACHINE_GETCHANCE :{
			del_queue_buf_len();
			PR_DEBUG("CMD_R_MACH  is ok \r\n ");
			cJSON_AddNumberToObject(obj_json, DP_CMD[10],CHANCE_NUM); 
            break;
		}
		case CMD_R_MACHINE_STOP :{
			PR_DEBUG("CMD_R_MACH  is ok \r\n ");
			cJSON_AddBoolToObject(obj_json, DP_CMD[8],true); 
            break;
		}
        default:
        break;
    }

    BYTE *tmp_data_buf = cJSON_PrintUnformatted(obj_json);
    cJSON_Delete(obj_json);
    
    if(NULL == tmp_data_buf) {
        PR_ERR("cJSON_PrintUnformatted err");
        return ;
    }
   
    OPERATE_RET op_ret = sf_obj_dp_report(get_single_wf_dev()->dev_if.id,tmp_data_buf);
    if(OPRT_OK != op_ret) {
     PR_ERR("sf_obj_dp_report err:%s",tmp_data_buf);       
     Free(tmp_data_buf);
     return ;
    }
    Free(tmp_data_buf);
    return ;

}
VOID uart_recv_process(PVOID pArg)
{
        BYTE  offset = 0;
        BYTE  rest_cnt=0;
		BYTE  RESIZE = 0;
        INT   error = 0;
    	INT   frm_len = 0;
        UCHAR check_sum=0;
        BYTE  ret = 0,i=0,j=0;
        REV_DATA_S *prot=NULL;
        CONST UCHAR CMD[11]={0Xa1,0xa2,0xa3,0xa4,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7};

    while(1) {

		RESIZE = user_uart_read_size();
        BYTE count = user_uart_read_data(uart_proc.rv_buf+uart_proc.in,\
        DATA_BUF_SZ-uart_proc.in);   
        
        uart_proc.in += count;

        if( (0 == count) || (uart_proc.in<MIN_RECV_lENTH) ) {
               SystemSleep(100);
               continue;
         }

		 if (RESIZE == 15)
		 	{
		 		PR_DEBUG("RESIZE = %d \r\n",RESIZE);
		 		Single_reply_processing(CMD[10]);
		 	}

         if(uart_proc.in<7){
            for(i=0;i<uart_proc.in;i++){
                for(j=0;j<SIZEOF(CMD)/SIZEOF(CMD[0]);j++){
                     if(uart_proc.rv_buf[i]==CMD[j]){
                        PR_DEBUG("alone word is ok\r\n" );
                        Single_reply_processing(CMD[j]);
                        memset(uart_proc.rv_buf,0,(DATA_BUF_SZ-1));
                        uart_proc.in=0;
                        offset = 0;
                        rest_cnt=0;
                        continue;
                     }
                }
               
             }
         }

	    if(uart_proc.in == 34){
			CHANCE_NUM = uart_proc.rv_buf[7];
			//CHANCE_NUM = hex_to_Decima(uart_proc.rv_buf[7]);
			PR_DEBUG("CHANCE_NUM = %d\r\n",CHANCE_NUM);
			if(GET == 1)Single_reply_processing(CMD[9]);
            Single_reply_processing(CMD[8]);
            memset(uart_proc.rv_buf,0,(DATA_BUF_SZ-1));
            uart_proc.in=0;
            offset = 0;
            rest_cnt=0;
            continue; //break;
			}
		
         if(uart_proc.in==0){
              continue;
          }
			
         
 REST_RUN:
 
         prot = (REV_DATA_S*)(uart_proc.rv_buf+offset);
         if(prot==NULL){
          PR_ERR("(REV_DATA_S*).prot  is NULL" );
          continue;
         }
         if( (prot->rev_head[0]!=0x24)||(prot->rev_head[1]!=0x40) )
         {
            PR_DEBUG("Rev_head. is err Rev_head[0]=%d,Rev_head[1]=%d",prot->rev_head[0],prot->rev_head[1] );
            goto RV_BUF_FMT;
         }
         
        if(uart_proc.in< 7) { 
           PR_DEBUG("uart_proc.in is err :%d", uart_proc.in );
            error++;
            if(error < 3) {
                SystemSleep(50);
                continue;
            }else {
                PR_ERR("prot->frm_len is err :%d", frm_len );
                error = 0;
                goto RV_BUF_FMT;
            }
        }
        error = 0;
        
#if UART_RX_DEBUG
            PR_DEBUG_RAW("RX:\n");
            for(i = 0;i < uart_proc.in;i++) {
                PR_DEBUG_RAW("%02X ",uart_proc.rv_buf[i]);
                if(i && ((i % 20) == 0)) {
                    PR_DEBUG_RAW("\n");
                }
            }
            PR_DEBUG_RAW("\n");
#endif
        for(i=0; i<sizeof(RecvFunctionArray)/sizeof(RecvFunctionArray[0]); i++)
	    {
		   if(prot->rev_cmd == RecvFunctionArray[i].R_CMD){
    			RecvFunctionArray[i].Function(prot); 
		   }
	    }
	    if(i>sizeof(RecvFunctionArray)/sizeof(RecvFunctionArray[0])){
	        PR_ERR("RecvFunctionArray cmd is err: %02x ",prot->rev_cmd);
            offset++;
            goto RV_BUF_FMT;
	    }
        offset = 0;
        memset(uart_proc.rv_buf,0,(DATA_BUF_SZ-1));
        uart_proc.in=0;
        rest_cnt=0;

  RV_BUF_FMT:
        if(uart_proc.in > offset) {
            BYTE rm_da_len = (uart_proc.in-offset);
            while(rm_da_len--) {
                 if( (prot->rev_head[0]!=0x24)||(prot->rev_head[1]!=0x40) ){
                    offset++;
                }else {
                    rest_cnt++;
                    break;
                }
                memcpy(uart_proc.rv_buf,uart_proc.rv_buf+offset,uart_proc.in-offset);  
            }
            uart_proc.in -= offset;
            
            if((rm_da_len>0)&&(rest_cnt<2)){
                  offset=0;
                  goto REST_RUN;
            }else{
                  rest_cnt=0;
                  memset(uart_proc.rv_buf,0,(DATA_BUF_SZ-1));
                  uart_proc.in=0;
                  offset = 0;
             }
        }else{
             memset(uart_proc.rv_buf,0,(DATA_BUF_SZ-1));
             uart_proc.in=0;
             offset = 0;
             rest_cnt=0;
        	}
	

    }

}



OPERATE_RET uart_proc_init(VOID) 
{
    OPERATE_RET op_ret;
    INT i = 0;
    
    typedef struct {
        TIMER_ID *tid;
        P_TIMER_FUNC func;  
        
    }TIMER_INIT_S;
  
    user_uart_init(BIT_RATE_9600);	
 
    uart_proc.snd_que = CreateQueueObj(QUE_NUM_SZ,SIZEOF(UR_SEND_S *));  //创建一个队列
    if(NULL == uart_proc.snd_que) {
        return OPRT_MALLOC_FAILED;
    }

    TIMER_INIT_S time_init_tbl[] = {
        
        {&uart_proc.snd_tmout,          snd_tmout_cb     },    //串口相关定时器  snd_tmout_cb 队列超时在snd_que_tm_cb中开启
        {&uart_proc.snd_que_tm,         snd_que_tm_cb    },                  
        {&uart_proc.prot_test_tm,       Pro_test_timer_cb},  
    };
        
    for(i = 0;i < CNTSOF(time_init_tbl);i++) {
        op_ret = sys_add_timer(time_init_tbl[i].func,NULL,time_init_tbl[i].tid); //创建串口相关定时器
        if(op_ret != OPRT_OK) {
            PR_ERR("sys_add_timer is err timer i is:%d: ",i);
            return op_ret;
        }
    } 
    
    sys_start_timer(uart_proc.snd_que_tm,   100,   TIMER_CYCLE);       // 150ms  发送队列?
    op_ret = CreateMutexAndInit(&uart_proc.mutex);
    if(OPRT_OK != op_ret) {
        return op_ret;
    }  
    
    THRD_HANDLE hand = NULL;
    op_ret = CreateAndStart(&uart_proc.hand,uart_recv_process,\
                            NULL,1024+256,TRD_PRIO_2,"uart_recv_thrd");//创建串口接收任务控制块
    if(OPRT_OK != op_ret) {
        ReleaseQueueObj(uart_proc.snd_que);
        system_timer_release();  //    ?
        PR_ERR("CreateAndStart uart_recv_process is err %d: ",op_ret);
        system_restart();
        return op_ret;
    }

    return OPRT_OK;
}




/***********************************************************
*  Function: device_init
*  Input: 
*  Output: 
*  Return: 
***********************************************************/
//应用层函数入口
OPERATE_RET device_init(VOID)
{

    OPERATE_RET op_ret;
    glo_data.POWER = POWER_CLOSE;

    //设备相关初始化以及接收APP数据的回调函数的注册
    op_ret = smart_frame_init(device_cb,USER_SW_VER);
    if(op_ret != OPRT_OK) {
        PR_ERR("smart_frame_init err:%d",op_ret);
        return op_ret;
    }
    //创建一个LED控制的接口初始化
    op_ret = create_led_handle(WF_DP_LED, &wf_light);  
    if(OPRT_OK != op_ret) {
        return op_ret;
    }
    
    //相关串口初始化函数
    op_ret = uart_proc_init();
    if(op_ret != OPRT_OK) {
        PR_ERR("uart_proc_init err:%d",op_ret);
        return op_ret;
    } 
    //wifi状态LED显示定时器处理函数初始化以及按键函数初始化和回调函数注册
    op_ret = device_differ_init();
     if(op_ret != OPRT_OK) {
         return op_ret;
    }
    //设备初始化函数
    DEV_DESC_IF_S def_dev_if;
    strcpy(def_dev_if.product_key,PRODECT_KEY);
    strcpy(def_dev_if.sw_ver,SW_VER);
    def_dev_if.ability = DEV_SINGLE;

    op_ret = single_wf_device_init_pk(&def_dev_if);
    if(op_ret != OPRT_OK) {
        return op_ret;
    }

    return op_ret;
}



STATIC VOID wifi_rssi_scan(OPERATE_RET *ret,CHAR *rssi)
{

     CHAR result = *rssi;
     UCHAR prod_rssi = 0;
     UCHAR test_Cmd_data[P_DATA_LEN] ={0x77,0x77,0x07};
     OPERATE_RET op_ret;
    if(*ret != OPRT_OK) {
        PR_DEBUG("NO SSID");
        prod_rssi = 200;
    } else {
        PR_DEBUG("RSSI:%d",result);
        if( result <= -100 ) {            
            prod_rssi = 0;
        }else if( (result > -100) && (result <= -80) ) {
            prod_rssi = 40;
        }else if( (result > -80) && (result <= -60) ) {
            prod_rssi = 60;
        }else if( (result > -60) && (result <= -40) ) {
            prod_rssi = 80;
        }else{
            prod_rssi = 100;
        }
    }
    if(prod_rssi>0&&prod_rssi<=100){
        PR_NOTICE("Protest  is ok \r\n");
        op_ret= ur_send_set_in_que(test_Cmd_data,P_DATA_LEN);
        if(op_ret!=OPRT_OK){
        PR_ERR("ur_send_set_in_que ERR:%d", op_ret);
       }
    }
    else{
   
        PR_NOTICE("prod_rssi:%d",prod_rssi);
        PR_NOTICE("Protest  is fail \r\n");
    }
    
    glo_data.PRO_TEST_STA=PRO_TEST_OK;
     
}



STATIC VOID key_process(INT gpio_no,PUSH_KEY_TYPE_E type,INT cnt)
{
    PR_DEBUG("gpio_no: %d",gpio_no);
    PR_DEBUG("type: %d",type);
    PR_DEBUG("cnt: %d",cnt);
    CHAR  *Rssi=NULL;
    BYTE   err_=0;
    
    if(WF_DP_KEY == gpio_no) {
    
        if(LONG_KEY == type) { 
           single_dev_reset_factory(); 
        }else if(NORMAL_KEY == type) {
           PR_NOTICE("remain size:%d",system_get_free_heap_size());
           sys_start_timer(uart_proc.prot_test_tm, 1000,   TIMER_ONCE);       // 150ms  这里开机发送查询命令
        }
        
    }
}

STATIC OPERATE_RET device_differ_init(VOID)
{
    OPERATE_RET op_ret;


    set_key_detect_time(50);    //   ?
    op_ret = key_init(NULL,0,25);
    if(OPRT_OK  != op_ret) {
        return op_ret;
    }
    op_ret = reg_proc_key(WF_DP_KEY,3*1000,key_process);//配网?还是工厂测试?
     if(OPRT_OK  != op_ret) {
         
         return op_ret;
     }

    TIMER_ID timer;
    op_ret = sys_add_timer(wfl_timer_cb,NULL,&timer);
    if(OPRT_OK != op_ret) {
        return op_ret;
    }else {
        sys_start_timer(timer,500,TIMER_CYCLE);
    }

    return OPRT_OK;
}



STATIC VOID wfl_timer_cb(UINT timerID,PVOID pTimerArg)
{
    STATIC UINT last_wf_stat = 0xffffffff;
    GW_WIFI_STAT_E wf_stat = get_wf_gw_status();
  	STATIC BOOL last_mq_stat = FALSE;
	BOOL mq_stat = get_gw_mq_conn_stat();
    
    if ( (last_wf_stat != wf_stat) || (last_mq_stat != mq_stat) ) {
        PR_DEBUG("wf_stat:%d\r\n",wf_stat);
        
        switch(wf_stat) {
        
            case STAT_UNPROVISION: {
                set_led_light_type(wf_light,OL_FLASH_HIGH,250);
				PR_DEBUG("STAT_UNPROVISION");
                break;
            }
            
            case STAT_AP_STA_UNCONN:
            case STAT_AP_STA_CONN: {
                set_led_light_type(wf_light,OL_FLASH_HIGH,1500);
				PR_DEBUG("STAT_AP_STA_UNCONN\r\n");
                break;
            }
            
            case STAT_STA_UNCONN: {
                set_led_light_type(wf_light,OL_LOW,0);
				PR_DEBUG("STAT_STA_UNCONN\r\n");
                break;
            }

            case STAT_STA_CONN: {
                 set_led_light_type(wf_light,OL_HIGH,0);
				 PR_DEBUG("STAT_STA_CONN\r\n");
                 break;
            }
            
            default: {
                set_led_light_type(wf_light,OL_LOW,0);
				PR_DEBUG("default STAT_STA_CONN\r\n");
            }
            break;
        }
        last_wf_stat = wf_stat;
        last_mq_stat = mq_stat;

    }
}














