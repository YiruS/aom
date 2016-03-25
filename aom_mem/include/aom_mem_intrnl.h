/*
 * Copyright (c) 2016, Alliance for Open Media. All rights reserved
 *
 * This source code is subject to the terms of the BSD 2 Clause License and
 * the Alliance for Open Media Patent License 1.0. If the BSD 2 Clause License
 * was not distributed with this source code in the LICENSE file, you can
 * obtain it at www.aomedia.org/license/software. If the Alliance for Open
 * Media Patent License 1.0 was not distributed with this source code in the
 * PATENTS file, you can obtain it at www.aomedia.org/license/patent.
 */

#ifndef VPX_MEM_INCLUDE_VPX_MEM_INTRNL_H_
#define VPX_MEM_INCLUDE_VPX_MEM_INTRNL_H_
#include "./aom_config.h"

#define ADDRESS_STORAGE_SIZE sizeof(size_t)

#ifndef DEFAULT_ALIGNMENT
#if defined(VXWORKS)
/*default addr alignment to use in calls to aom_* functions other than
  aom_memalign*/
#define DEFAULT_ALIGNMENT 32
#else
#define DEFAULT_ALIGNMENT (2 * sizeof(void *)) /* NOLINT */
#endif
#endif

/*returns an addr aligned to the byte boundary specified by align*/
#define align_addr(addr, align) \
  (void *)(((size_t)(addr) + ((align)-1)) & (size_t) - (align))

#endif  // VPX_MEM_INCLUDE_VPX_MEM_INTRNL_H_