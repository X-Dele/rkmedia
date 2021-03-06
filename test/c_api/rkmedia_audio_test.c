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
#define AAC_NB_SAMPLES 1024
#define MP2_NB_SAMPLES 1152
#define ALSA_PATH "default:CARD=rockchiprk809co" // get from "arecord -L"
static bool quit = false;
static void sigterm_handler(int sig) {
  fprintf(stderr, "signal %d\n", sig);
  quit = true;
}

FILE *fp = NULL;
static RK_U32 g_enWorkSampleRate = 16000;

static void audio_packet_cb(MEDIA_BUFFER mb) {
  printf("Get Audio Encoded packet:ptr:%p, fd:%d, size:%zu, mode:%d\n",
         RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetFD(mb), RK_MPI_MB_GetSize(mb),
         RK_MPI_MB_GetModeID(mb));
  fwrite(RK_MPI_MB_GetPtr(mb), 1, RK_MPI_MB_GetSize(mb), fp);
  RK_MPI_MB_ReleaseBuffer(mb);
}

static RK_VOID AI_AO() {
  RK_MPI_SYS_Init();
  MPP_CHN_S mpp_chn_ai, mpp_chn_ao;
  mpp_chn_ai.enModId = RK_ID_AI;
  mpp_chn_ai.s32ChnId = 0;
  mpp_chn_ao.enModId = RK_ID_AO;
  mpp_chn_ao.s32ChnId = 0;

  AI_CHN_ATTR_S ai_attr;
  ai_attr.path = ALSA_PATH;
  ai_attr.fmt = RK_SAMPLE_FMT_S16;
  ai_attr.nb_samples = 1152;
  ai_attr.sample_rate = g_enWorkSampleRate;
  ai_attr.channels = 1;

  AO_CHN_ATTR_S ao_attr;
  ao_attr.path = ALSA_PATH;
  ao_attr.fmt = RK_SAMPLE_FMT_S16;
  ao_attr.nb_samples = 1152;
  ao_attr.sample_rate = g_enWorkSampleRate;
  ao_attr.channels = 1;

  // 1. create AI
  RK_MPI_AI_SetChnAttr(mpp_chn_ai.s32ChnId, &ai_attr);
  RK_MPI_AI_EnableChn(mpp_chn_ai.s32ChnId);

  // 2. create AO
  RK_MPI_AO_SetChnAttr(mpp_chn_ao.s32ChnId, &ao_attr);
  RK_MPI_AO_EnableChn(mpp_chn_ao.s32ChnId);

  // 3. bind AI-AO
  RK_MPI_SYS_Bind(&mpp_chn_ai, &mpp_chn_ao);

  printf("%s initial finish\n", __func__);
  signal(SIGINT, sigterm_handler);
  while (!quit) {
    usleep(100);
  }

  printf("%s exit!\n", __func__);
  RK_MPI_SYS_UnBind(&mpp_chn_ai, &mpp_chn_ao);
  RK_MPI_AI_DisableChn(mpp_chn_ai.s32ChnId);
  RK_MPI_AO_DisableChn(mpp_chn_ao.s32ChnId);
}

static RK_VOID AI_AENC_FILE(char *file_path) {
  fp = fopen(file_path, "w+");
  RK_MPI_SYS_Init();
  MPP_CHN_S mpp_chn_ai, mpp_chn_aenc;
  mpp_chn_ai.enModId = RK_ID_AI;
  mpp_chn_ai.s32ChnId = 0;
  mpp_chn_aenc.enModId = RK_ID_AENC;
  mpp_chn_aenc.s32ChnId = 0;

  AI_CHN_ATTR_S ai_attr;
  ai_attr.path = ALSA_PATH;
  ai_attr.fmt = RK_SAMPLE_FMT_S16;
  ai_attr.nb_samples = MP2_NB_SAMPLES;
  ai_attr.sample_rate = g_enWorkSampleRate;
  ai_attr.channels = 2;

  AENC_CHN_ATTR_S aenc_attr;
  aenc_attr.enType = RK_CODEC_TYPE_MP2;
  aenc_attr.u32Bitrate = 64000;
  aenc_attr.u32Quality = 1;
  aenc_attr.mp2_attr.u32Channels = 2;
  aenc_attr.mp2_attr.u32SampleRate = g_enWorkSampleRate;

  // 1. create AI
  RK_MPI_AI_SetChnAttr(mpp_chn_ai.s32ChnId, &ai_attr);
  RK_MPI_AI_EnableChn(mpp_chn_ai.s32ChnId);

  // 2. create AENC
  RK_MPI_AENC_CreateChn(mpp_chn_aenc.s32ChnId, &aenc_attr);
  RK_U32 ret = RK_MPI_SYS_RegisterOutCb(&mpp_chn_aenc, audio_packet_cb);
  printf("ret = %d.\n", ret);

  // 3. bind AI-AENC
  RK_MPI_SYS_Bind(&mpp_chn_ai, &mpp_chn_aenc);

  printf("%s initial finish\n", __func__);
  signal(SIGINT, sigterm_handler);
  while (!quit) {
    usleep(100);
  }

  RK_MPI_SYS_UnBind(&mpp_chn_ai, &mpp_chn_aenc);
  RK_MPI_AI_DisableChn(mpp_chn_ai.s32ChnId);
  RK_MPI_AENC_DestroyChn(mpp_chn_aenc.s32ChnId);
  fclose(fp);
}

static RK_VOID RKMEDIA_AUDIO_Usage() {
  printf("\n\n/Usage:./rkmdia_audio <index> <sampleRate> [filePath]/\n");
  printf("\tindex and its function list below\n");
  printf("\t0:  start AI to AO loop\n");
  printf("\t1:  send audio frame to AENC channel from AI, save them\n");
  // printf("\t2:  read audio stream from file, decode and send AO\n");
  // printf("\t3:  start AI(VQE process), then send to AO\n");
  // printf("\t4:  start AI to Extern Resampler\n");
  printf("\n");
  printf("\tsampleRate list:\n");
  printf("\t0 16000 22050 24000 32000 44100 48000\n");
  printf("\n");
  printf("\tfilePath represents the path of audio file to be decoded, only for "
         "sample 2.\n");
  printf("\tdefault filePath: /userdata/out.mp2\n");
  printf("\n");
  printf("\texample: ./rkmdia_audio 0 48000\n");
}

int main(int argc, char *argv[]) {

  RK_U32 u32Index;
  RK_CHAR *pFilePath = RK_NULL;

  if (argc != 3 && argc != 4) {
    RKMEDIA_AUDIO_Usage();
    return -1;
  }

  if (strcmp(argv[1], "-h") == 0) {
    RKMEDIA_AUDIO_Usage();
    return -1;
  }

  u32Index = atoi(argv[1]);
  g_enWorkSampleRate = atoi(argv[2]);
  if (4 == argc) {
    pFilePath = argv[3];
  } else {
    pFilePath = (char *)"/userdata/out.mp2";
  }

  switch (u32Index) {
  case 0:
    AI_AO();
    break;
  case 1:
    AI_AENC_FILE(pFilePath);
    break;
  default:
    break;
  }
  return 0;
}