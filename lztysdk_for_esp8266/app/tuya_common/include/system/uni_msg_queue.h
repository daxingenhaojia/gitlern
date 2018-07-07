/***********************************************************
*  File: uni_msg_queue.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_MSG_QUEUE_H
#define _UNI_MSG_QUEUE_H

    #include "sys_adapter.h"
    #include "uni_pointer.h"
    
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_MSG_QUEUE_GLOBAL
    #define _UNI_MSG_QUEUE_EXT 
#else
    #define _UNI_MSG_QUEUE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef PVOID MSG_QUE_HANDLE; // ��Ϣ���в������

typedef UINT MSG_ID;            // ��ϢID
typedef PVOID P_MSG_DATA;       // ��Ϣ����
typedef UINT MSG_DATA_LEN;      // ��Ϣ���ݳ���

typedef UINT MSG_TYPE; // ��Ϣ����
#define INSTANCY_MESSAGE 0  // ������Ϣ(����ϢΪ������ִ��)
#define NORMAL_MESSAGE 1    // ��ͨ��Ϣ(����Ϣ���Ƚ��ȳ���ʽִ��)

// ��Ϣ
typedef struct
{
    MSG_ID msgID;
    P_MSG_DATA pMsgData;
    MSG_DATA_LEN msgDataLen;
}MESSAGE,*P_MESSAGE;

// ��Ϣ��
typedef struct
{
    LIST_HEAD listHead;     // ����ڵ�
    MESSAGE msg;
}MSG_LIST,*P_MSG_LIST;

#define USE_SEM_COUNTING 1
#if !(USE_SEM_COUNTING)
    #include "uni_system.h"
    #define PROC_MSG_DELAY 100
#endif
/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
_UNI_MSG_QUEUE_EXT \
OPERATE_RET CreateMsgQueAndInit(OUT MSG_QUE_HANDLE *pMsgQueHandle);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET AddMsgNodeToQueue(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                              IN CONST MSG_ID msgID,IN CONST P_MSG_DATA pMsgData,\
                              IN CONST MSG_DATA_LEN msgDataLen,\
                              IN CONST MSG_TYPE msgType);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET GetMsgNodeFromQueue(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                                IN CONST MSG_ID msgID,OUT P_MSG_LIST *ppMsgListNode);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET GetFirstMsgFromQueue(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                                 OUT P_MSG_LIST *ppMsgListNode);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET GetMsgNodeNum(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                          OUT PINT pMsgNodeNum);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET DelAndFreeMsgNodeFromQueue(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                                       IN CONST P_MSG_LIST pMsgListNode);

_UNI_MSG_QUEUE_EXT \
OPERATE_RET ReleaseMsgQue(IN CONST MSG_QUE_HANDLE msgQueHandle);

/***********************************************************
*  Function: PostMessage ����һ����Ϣ��ģ��(��Ϣ�Ƚ���ִ��)
*  Input: msgQueHandle->��Ϣ������
*         msgID->��ϢID
*         pMsgData->��Ϣ����
*         msgDataLen->��Ϣ���ݳ���
*  Output: none
*  Return: OPERATE_RET
*  Date: 20140624
***********************************************************/
_UNI_MSG_QUEUE_EXT \
OPERATE_RET PostMessage(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                        IN CONST MSG_ID msgID,\
                        IN CONST P_MSG_DATA pMsgData,\
                        IN CONST MSG_DATA_LEN msgDataLen);

/***********************************************************
*  Function: PostInstancyMsg ����һ��������Ϣ��ģ��
*  Input: msgQueHandle->��Ϣ������
*         msgID->��ϢID
*         pMsgData->��Ϣ����
*         msgDataLen->��Ϣ���ݳ���
*  Output: none
*  Return: OPERATE_RET
*  Date: 20140624
***********************************************************/
_UNI_MSG_QUEUE_EXT \
OPERATE_RET PostInstancyMsg(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                            IN CONST MSG_ID msgID,\
                            IN CONST P_MSG_DATA pMsgData,\
                            IN CONST MSG_DATA_LEN msgDataLen);

/***********************************************************
*  Function: WaitMessage �ȴ���Ϣ 
*            WaitMessage�ɹ�����ã���Ϣ������������
*                       DelAndFreeMsgNodeFromQueue�ͷ���Ϣ
*  Input: msgQueHandle->��Ϣ������
*  Output: ppMsgListNode
*  Return: OPERATE_RET
*  Date: 20140624
***********************************************************/
_UNI_MSG_QUEUE_EXT \
OPERATE_RET WaitMessage(IN CONST MSG_QUE_HANDLE msgQueHandle,\
                        OUT P_MSG_LIST *ppMsgListNode);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
