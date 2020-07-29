// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "rkmedia_api.h"
#include "rkmedia_venc.h"

static bool quit = false;
static void sigterm_handler(int sig) {
  fprintf(stderr, "signal %d\n", sig);
  quit = true;
}
static FILE *g_save_file;

#define TEST_ARGB32_YELLOW 0xFFFFFF00
#define TEST_ARGB32_RED 0xFFFF0033

void video_packet_cb(MEDIA_BUFFER mb) {
  printf("Get Video Encoded packet:ptr:%p, fd:%d, size:%zu, mode:%d\n",
         RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetFD(mb), RK_MPI_MB_GetSize(mb),
         RK_MPI_MB_GetModeID(mb));
  if (g_save_file)
    fwrite(RK_MPI_MB_GetPtr(mb), 1, RK_MPI_MB_GetSize(mb), g_save_file);
  RK_MPI_MB_ReleaseBuffer(mb);
}

int main() {
  int ret = 0;

  RK_MPI_SYS_Init();

  VI_CHN_ATTR_S vi_chn_attr;
  vi_chn_attr.buffer_cnt = 4;
  vi_chn_attr.width = 1920;
  vi_chn_attr.height = 1080;
  vi_chn_attr.pix_fmt = IMAGE_TYPE_NV12;
  ret = RK_MPI_VI_SetChnAttr(0, 1, &vi_chn_attr);
  ret |= RK_MPI_VI_EnableChn(0, 1);
  if (ret) {
    printf("TEST: ERROR: Create vi[0] error! code:%d\n", ret);
    return -1;
  }

  VENC_CHN_ATTR_S venc_chn_attr;
  venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H264;
  venc_chn_attr.stVencAttr.imageType = IMAGE_TYPE_NV12;
  venc_chn_attr.stVencAttr.u32PicWidth = 1920;
  venc_chn_attr.stVencAttr.u32PicHeight = 1080;
  venc_chn_attr.stVencAttr.u32VirWidth = 1920;
  venc_chn_attr.stVencAttr.u32VirHeight = 1080;
  venc_chn_attr.stVencAttr.u32Profile = 77;
  venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
  venc_chn_attr.stRcAttr.stH264Cbr.u32Gop = 30;
  venc_chn_attr.stRcAttr.stH264Cbr.u32BitRate = 2000000; // 2Mb
  venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = 1;
  venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = 30;
  venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = 1;
  venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = 30;
  ret = RK_MPI_VENC_CreateChn(0, &venc_chn_attr);
  if (ret) {
    printf("TEST: ERROR: Create venc[0] error! code:%d\n", ret);
    return -1;
  }

  g_save_file = fopen("/userdata/output.h264", "w");
  if (!g_save_file)
    printf("#VENC OSD TEST:: Open /userdata/output.h264 failed!\n");

  MPP_CHN_S stEncChn;
  stEncChn.enModId = RK_ID_VENC;
  stEncChn.s32DevId = 0;
  stEncChn.s32ChnId = 0;
  ret = RK_MPI_SYS_RegisterOutCb(&stEncChn, video_packet_cb);
  if (ret) {
    printf("TEST: ERROR: Register cb for venc[0] error! code:%d\n", ret);
    return -1;
  }

  ret = RK_MPI_VENC_RGN_Init(0);
  if (ret) {
    printf("TEST: ERROR: venc[0] rgn init error! code:%d\n", ret);
    return -1;
  }

  COVER_INFO_S CoverInfo;
  OSD_REGION_INFO_S RngInfo;
  CoverInfo.enPixelFormat = PIXEL_FORMAT_ARGB_8888;

  // Yellow, 256x256
  CoverInfo.u32Color = TEST_ARGB32_YELLOW;
  RngInfo.enRegionId = REGION_ID_0;
  RngInfo.u32PosX = 0;
  RngInfo.u32PosY = 0;
  RngInfo.u32Width = 256;
  RngInfo.u32Height = 256;
  RngInfo.u8Enable = 1;
  RngInfo.u8Inverse = 0;
  ret = RK_MPI_VENC_RGN_SetCover(0, &RngInfo, &CoverInfo);
  if (ret) {
    printf("TEST: ERROR: Set cover(Y) for venc[0] error! code:%d\n", ret);
    return -1;
  }

  // Red, 512x512
  CoverInfo.u32Color = TEST_ARGB32_RED;
  RngInfo.enRegionId = REGION_ID_1;
  RngInfo.u32PosX = 256;
  RngInfo.u32PosY = 256;
  RngInfo.u32Width = 512;
  RngInfo.u32Height = 512;
  ret = RK_MPI_VENC_RGN_SetCover(0, &RngInfo, &CoverInfo);
  if (ret) {
    printf("TEST: ERROR: Set cover(R) for venc[0] error! code:%d\n", ret);
    return -1;
  }

  // Bind VI and VENC
  MPP_CHN_S stSrcChn;
  stSrcChn.enModId = RK_ID_VI;
  stSrcChn.s32DevId = 0;
  stSrcChn.s32ChnId = 1;
  MPP_CHN_S stDestChn;
  stDestChn.enModId = RK_ID_VENC;
  stDestChn.s32DevId = 0;
  stDestChn.s32ChnId = 0;
  ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
  if (ret) {
    printf("TEST: ERROR: Bind vi[0] to venc[0] error! code:%d\n", ret);
    return -1;
  }

  printf("%s initial finish\n", __func__);
  signal(SIGINT, sigterm_handler);

  sleep(3);
  printf("Disable cover region 0....\n");
  RngInfo.enRegionId = REGION_ID_0;
  RngInfo.u8Enable = 0;
  ret = RK_MPI_VENC_RGN_SetCover(0, &RngInfo, NULL);
  if (ret) {
    printf("TEST: ERROR: Unset cover for venc[0] error! code:%d\n", ret);
    return -1;
  }

  while (!quit) {
    usleep(100);
  }

  if (g_save_file) {
    printf("#VENC OSD TEST:: Close save file!\n");
    fclose(g_save_file);
  }

  printf("%s exit!\n", __func__);
  RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
  RK_MPI_VI_DisableChn(0, 1);
  RK_MPI_VENC_DestroyChn(0);

  return 0;
}