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
#ifndef VP10_COMMON_DERING_H_
#define VP10_COMMON_DERING_H_

#include "av1/common/od_dering.h"
#include "av1/common/onyxc_int.h"
#include "aom/vpx_integer.h"
#include "./vpx_config.h"
#include "aom_ports/mem.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DERING_LEVEL_BITS 6
#define MAX_DERING_LEVEL (1 << DERING_LEVEL_BITS)

#define DERING_REFINEMENT 1
#define DERING_REFINEMENT_BITS 2
#define DERING_REFINEMENT_LEVELS 4

int compute_level_from_index(int global_level, int gi);
int sb_all_skip(const VP10_COMMON *const cm, int mi_row, int mi_col);
void vp10_dering_frame(YV12_BUFFER_CONFIG *frame, VP10_COMMON *cm,
                       MACROBLOCKD *xd, int global_level);

int vp10_dering_search(YV12_BUFFER_CONFIG *frame, const YV12_BUFFER_CONFIG *ref,
                      VP10_COMMON *cm,
                      MACROBLOCKD *xd);

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // VP10_COMMON_DERING_H_