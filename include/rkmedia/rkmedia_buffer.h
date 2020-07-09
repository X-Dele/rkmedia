// Copyright 2019 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef __RK_BUFFER_
#define __RK_BUFFER_

#include <stddef.h>

#include "rkmedia_common.h"

typedef void *MEDIA_BUFFER;
typedef void (*OutCbFunc)(MEDIA_BUFFER mb);

_CAPI void *RK_MPI_MB_GetPtr(MEDIA_BUFFER mb);
_CAPI int RK_MPI_MB_GetFD(MEDIA_BUFFER mb);
_CAPI size_t RK_MPI_MB_GetSize(MEDIA_BUFFER mb);
_CAPI MOD_ID_E RK_MPI_MB_GetModeID(MEDIA_BUFFER mb);
_CAPI RK_S32 RK_MPI_MB_ReleaseBuffer(MEDIA_BUFFER buffer);

#endif // #ifndef __RK_BUFFER_
