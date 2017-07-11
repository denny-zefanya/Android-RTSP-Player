/*****************************************************************************
   ģ����      : ants360 type
   �ļ���      :ants360type.h
   ����ļ�    :
   �ļ�ʵ�ֹ���: 360ants�궨��
   ����        : κ�α�
   �汾        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2004/02/17  3.0         κ�α�        ����
   2012/08/21  4.0         ������        ������ض���
******************************************************************************/
#ifndef _VIDEO_DEF_H_
#define _VIDEO_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define  MAX_WIDTH                      1920
#define  MAX_HEIGHT                     1080
#define  MAX_FRAME_BUFFER_SIZE          ( MAX_WIDTH * MAX_HEIGHT * 3 / 2 )
#define  MAX_H264_FRAME_SIZE            300000
#define  MAX_H264_HEADER_SIZE           1000
#define  BLOCK_QUEUE_SIZE               30

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _VIDEO_DEF_H_ */

/* end of file videodef.h */

