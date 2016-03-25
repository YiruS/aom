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

#ifndef VPX_DSP_BITREADER_BUFFER_H_
#define VPX_DSP_BITREADER_BUFFER_H_

#include <limits.h>

#include "aom/vpx_integer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*vpx_rb_error_handler)(void *data);

struct vpx_read_bit_buffer {
  const uint8_t *bit_buffer;
  const uint8_t *bit_buffer_end;
  size_t bit_offset;

  void *error_handler_data;
  vpx_rb_error_handler error_handler;
};

size_t vpx_rb_bytes_read(struct vpx_read_bit_buffer *rb);

int vpx_rb_read_bit(struct vpx_read_bit_buffer *rb);

int vpx_rb_read_literal(struct vpx_read_bit_buffer *rb, int bits);

int vpx_rb_read_signed_literal(struct vpx_read_bit_buffer *rb, int bits);

int vpx_rb_read_inv_signed_literal(struct vpx_read_bit_buffer *rb, int bits);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // VPX_DSP_BITREADER_BUFFER_H_