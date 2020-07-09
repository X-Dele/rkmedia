// Copyright 2019 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __RKMEDIA_COMMON_
#define __RKMEDIA_COMMON_

#define _CAPI __attribute__((visibility("default")))

typedef unsigned char RK_U8;
typedef unsigned short RK_U16;
typedef unsigned int RK_U32;

typedef signed char RK_S8;
typedef short RK_S16;
typedef int RK_S32;

typedef unsigned long RK_UL;
typedef signed long RK_SL;

typedef float RK_FLOAT;
typedef double RK_DOUBLE;

#ifndef _M_IX86
typedef unsigned long long RK_U64;
typedef long long RK_S64;
#else
typedef unsigned __int64 RK_U64;
typedef __int64 RK_S64;
#endif

typedef char RK_CHAR;
#define RK_VOID void

typedef unsigned int RK_HANDLE;

/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
typedef enum {
  RK_FALSE = 0,
  RK_TRUE = 1,
} RK_BOOL;

#ifndef NULL
#define NULL 0L
#endif

#define RK_NULL 0L
#define RK_SUCCESS 0
#define RK_FAILURE (-1)

typedef enum rk_IMAGE_TYPE_E {
  IMAGE_TYPE_UNKNOW = 0,
  IMAGE_TYPE_GRAY8,
  IMAGE_TYPE_GRAY16,
  IMAGE_TYPE_YUV420P,
  IMAGE_TYPE_NV12,
  IMAGE_TYPE_NV21,
  IMAGE_TYPE_YV12,
  IMAGE_TYPE_FBC2,
  IMAGE_TYPE_FBC0,
  IMAGE_TYPE_YUV422P,
  IMAGE_TYPE_NV16,
  IMAGE_TYPE_NV61,
  IMAGE_TYPE_YV16,
  IMAGE_TYPE_YUYV422,
  IMAGE_TYPE_UYVY422,
  IMAGE_TYPE_RGB332,
  IMAGE_TYPE_RGB565,
  IMAGE_TYPE_BGR565,
  IMAGE_TYPE_RGB888,
  IMAGE_TYPE_BGR888,
  IMAGE_TYPE_ARGB8888,
  IMAGE_TYPE_ABGR8888,
  IMAGE_TYPE_JPEG,

  IMAGE_TYPE_BUTT
} IMAGE_TYPE_E;

typedef enum rkMOD_ID_E {
  RK_ID_UNKNOW = 0,
  RK_ID_VB,
  RK_ID_SYS,
  RK_ID_VDEC,
  RK_ID_VENC,
  RK_ID_H264E,
  RK_ID_JPEGE,
  RK_ID_MPEG4E,
  RK_ID_H265E,
  RK_ID_JPEGD,
  RK_ID_VO,
  RK_ID_VI,
  RK_ID_AIO,
  RK_ID_AI,
  RK_ID_AO,
  RK_ID_AENC,
  RK_ID_ADEC,

  RK_ID_BUTT,
} MOD_ID_E;

enum {
  RK_ERR_SYS_OK = 0,
  RK_ERR_SYS_NULL_PTR,
  RK_ERR_SYS_NOTREADY,
  RK_ERR_SYS_NOT_PERM,
  RK_ERR_SYS_NOMEM,
  RK_ERR_SYS_ILLEGAL_PARAM,
  RK_ERR_SYS_BUSY,
  RK_ERR_SYS_NOT_SUPPORT,

  /* invlalid channel ID */
  RK_ERR_VI_INVALID_CHNID,
  /* system is busy*/
  RK_ERR_VI_BUSY,
  /* channel exists */
  RK_ERR_VI_EXIST,
  RK_ERR_VI_NOT_CONFIG,

  /* invlalid channel ID */
  RK_ERR_VENC_INVALID_CHNID,
  /* at lease one parameter is illagal ,eg, an illegal enumeration value  */
  RK_ERR_VENC_ILLEGAL_PARAM,
  /* channel exists */
  RK_ERR_VENC_EXIST,
  /* channel exists */
  RK_ERR_VENC_UNEXIST,
  /* using a NULL point */
  RK_ERR_VENC_NULL_PTR,
  /* try to enable or initialize system,device or channel,
     before configing attrib */
  RK_ERR_VENC_NOT_CONFIG,
  /* operation is not supported by NOW */
  RK_ERR_VENC_NOT_SUPPORT,
  /* operation is not permitted ,eg, try to change stati attribute */
  RK_ERR_VENC_NOT_PERM,
  /* failure caused by malloc memory */
  RK_ERR_VENC_NOMEM,
  /* failure caused by malloc buffer */
  RK_ERR_VENC_NOBUF,
  /* no data in buffer */
  RK_ERR_VENC_BUF_EMPTY,
  /* no buffer for new data */
  RK_ERR_VENC_BUF_FULL,
  /* system is not ready,had not initialed or loaded*/
  RK_ERR_VENC_SYS_NOTREADY,
  /* system is busy*/
  RK_ERR_VENC_BUSY,

  RK_ERR_BUIT,
};

#endif // #ifndef __RKMEDIA_COMMON_