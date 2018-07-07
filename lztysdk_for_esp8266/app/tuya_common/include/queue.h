/***********************************************************
*  File: queue.h // ͨ�ö���ʵ��
*  Author: nzy
*  Date: 121126
***********************************************************/
#ifndef _QUEUE_H
    #define _QUEUE_H

    #include "sys_adapter.h"
    #include "system/uni_mutex.h"
    
#ifdef __cplusplus
	extern "C" {
#endif

#define QUEUE_SAFE_FUNC // �Ƿ���Ҫ��ȫ�汾

#ifdef  __QUEUE_GLOBALS
    #define __QUEUE_EXT
#else
    #define __QUEUE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef struct{
    unsigned int queUnitSize; // ���ж�Ա�ߴ�
    unsigned int queTolNum; // ���п����ɵ��ܵĶ�Ա
    unsigned int queCurNum; // ��ǰ�����ж�Ա��
    
    unsigned char *queData; // ���л���
    unsigned char *queIn; // ���λ��
    unsigned char *queOut; // ����λ��

    #ifdef QUEUE_SAFE_FUNC
    MUTEX_HANDLE mutex;
    #endif
}QUEUE_CLASS,*P_QUEUE_CLASS;


/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
/***********************************************************
*  Function: CreateQueueObj��ʼ��
*  Input: queTolNum:���п����ɵĶ�Ա��
*         queUnitSize:��Ա�ߴ�
*  Output: none
*  Return: NULL:ʧ��
*  Date: 121127
***********************************************************/
__QUEUE_EXT P_QUEUE_CLASS CreateQueueObj(const unsigned int queTolNum,\
                                         const unsigned int queUnitSize);


/***********************************************************
*  Function:RegisterQueueObj �������ݻ����Լ����й�����󻺳������ⲿע��
*  Input: pQueObj
*         queData
*         queTolNum:���п����ɵĶ�Ա��
*         queUnitSize:��Ա�ߴ�
*  Output: pQueObj
*  Return: 0:ʧ��
*  Date: 121127
***********************************************************/
__QUEUE_EXT \
unsigned char RegisterQueueObj(P_QUEUE_CLASS pQueObj,\
                               const unsigned char *pQueData,\
                               const unsigned int queTolNum,\
                               const unsigned int queUnitSize);

/***********************************************************
*  Function: InQueue ���
*  Input: pQueObj:���ж���
*         pQueUnit:��Ա���� queNum:��Ա��
*  Output: none
*  Return: 0:ʧ��
*  Date: 121127
***********************************************************/
__QUEUE_EXT unsigned char InQueue(P_QUEUE_CLASS pQueObj, const unsigned char *pQueUnit,\
                                  const unsigned int queNum);

/***********************************************************
*  Function:OutQueue ����
*  Input: pQueObj:���ж���
*         queNum:����Ա��
*  Output: pQueUnit:��Ա����
*  Return: 0:ʧ��
*  Date: 121127
***********************************************************/
__QUEUE_EXT unsigned char OutQueue(P_QUEUE_CLASS pQueObj,unsigned char *pQueUnit,\
                                   const unsigned int queNum);

/***********************************************************
*  Function:GetCurFreeQueNum
*  Input: none
*  Output: none
*  Return: ��ǰ���п������
*  Date: 121127
***********************************************************/
__QUEUE_EXT unsigned int GetCurFreeQueNum(P_QUEUE_CLASS pQueObj);

/***********************************************************
*  Function:GetCurQueNum
*  Input: pQueObj
*  Output: none
*  Return: ��ǰ��ӳ�Ա��
*  Date: 121127
***********************************************************/
__QUEUE_EXT unsigned int GetCurQueNum(P_QUEUE_CLASS pQueObj);

/***********************************************************
*  Function:GetQueueMember ��Ա���ӣ�����ɾ����Ա
*  Input: pQueObj:���ж��� 
*         start:�ӵڼ�����Ա��ʼ��ȡ ��һ��ʼ(������ȡ����)
*         pQueUnit:���ӳ�Ա����
*         queNum:����Ա��
*  Output: pQueUnit:��Ա����
*  Return: 0:ʧ��
*  Date: 121127
***********************************************************/
__QUEUE_EXT unsigned char GetQueueMember(P_QUEUE_CLASS pQueObj,\
                                         const unsigned int start,\
                                         unsigned char *pQueUnit,\
                                         const unsigned int queNum);

/***********************************************************
*  Function:ClearQueue ��ն���
*  Input: pQueObj:���ж���
*         queNum:ɾ����Ա��Ŀ
*  Output: none
*  Return: 0:ʧ��
*  Date: 121127
***********************************************************/
__QUEUE_EXT unsigned char ClearQueue(P_QUEUE_CLASS pQueObj);

/***********************************************************
*  Function:DelQueueMember ɾ�����г�Ա
*  Input: pQueObj:���ж���
*         queNum:ɾ����Ա��Ŀ
*  Output: none
*  Return: 0:ʧ��
*  Date: 121127
***********************************************************/
__QUEUE_EXT unsigned char DelQueueMember(P_QUEUE_CLASS pQueObj,const unsigned int queNum);

/***********************************************************
*  Function: ReleaseQueueObj �ͷ�
*  Input: pQueObj
*         
*  Output: none
*  Return: none
*  Date: 121127
***********************************************************/
__QUEUE_EXT void ReleaseQueueObj(P_QUEUE_CLASS pQueObj);


#ifdef __cplusplus
}
#endif
#endif

