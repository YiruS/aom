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

#include "third_party/googletest/src/include/gtest/gtest.h"

#include "test/codec_factory.h"
#include "test/encode_test_driver.h"
#include "test/util.h"
#include "test/y4m_video_source.h"
#include "test/yuv_video_source.h"

namespace {

const unsigned int kWidth = 160;
const unsigned int kHeight = 90;
const unsigned int kFramerate = 50;
const unsigned int kFrames = 10;
const int kBitrate = 500;
// List of psnr thresholds for speed settings 0-7 and 5 encoding modes
const double kPsnrThreshold[][5] = {
  { 36.0, 37.0, 37.0, 37.0, 37.0 }, { 35.0, 36.0, 36.0, 36.0, 36.0 },
  { 34.0, 35.0, 35.0, 35.0, 35.0 }, { 33.0, 34.0, 34.0, 34.0, 34.0 },
  { 32.0, 33.0, 33.0, 33.0, 33.0 }, { 31.0, 32.0, 32.0, 32.0, 32.0 },
  { 30.0, 31.0, 31.0, 31.0, 31.0 }, { 29.0, 30.0, 30.0, 30.0, 30.0 },
};

typedef struct {
  const char *filename;
  unsigned int input_bit_depth;
  aom_img_fmt fmt;
  aom_bit_depth_t bit_depth;
  unsigned int profile;
} TestVideoParam;

const TestVideoParam kTestVectors[] = {
  { "park_joy_90p_8_420.y4m", 8, AOM_IMG_FMT_I420, AOM_BITS_8, 0 },
  { "park_joy_90p_8_422.y4m", 8, AOM_IMG_FMT_I422, AOM_BITS_8, 1 },
  { "park_joy_90p_8_444.y4m", 8, AOM_IMG_FMT_I444, AOM_BITS_8, 1 },
  { "park_joy_90p_8_440.yuv", 8, AOM_IMG_FMT_I440, AOM_BITS_8, 1 },
#if CONFIG_AOM_HIGHBITDEPTH
  { "park_joy_90p_10_420.y4m", 10, AOM_IMG_FMT_I42016, AOM_BITS_10, 2 },
  { "park_joy_90p_10_422.y4m", 10, AOM_IMG_FMT_I42216, AOM_BITS_10, 3 },
  { "park_joy_90p_10_444.y4m", 10, AOM_IMG_FMT_I44416, AOM_BITS_10, 3 },
  { "park_joy_90p_10_440.yuv", 10, AOM_IMG_FMT_I44016, AOM_BITS_10, 3 },
  { "park_joy_90p_12_420.y4m", 12, AOM_IMG_FMT_I42016, AOM_BITS_12, 2 },
  { "park_joy_90p_12_422.y4m", 12, AOM_IMG_FMT_I42216, AOM_BITS_12, 3 },
  { "park_joy_90p_12_444.y4m", 12, AOM_IMG_FMT_I44416, AOM_BITS_12, 3 },
  { "park_joy_90p_12_440.yuv", 12, AOM_IMG_FMT_I44016, AOM_BITS_12, 3 },
#endif  // CONFIG_AOM_HIGHBITDEPTH
};

// Encoding modes tested
const libaom_test::TestMode kEncodingModeVectors[] = {
  ::libaom_test::kTwoPassGood, ::libaom_test::kOnePassGood,
  ::libaom_test::kRealTime,
};

// Speed settings tested
const int kCpuUsedVectors[] = { 1, 2, 3, 5, 6 };

int is_extension_y4m(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return 0;
  else
    return !strcmp(dot, ".y4m");
}

class EndToEndTestLarge
    : public ::libaom_test::EncoderTest,
      public ::libaom_test::CodecTestWith3Params<libaom_test::TestMode,
                                                 TestVideoParam, int> {
 protected:
  EndToEndTestLarge()
      : EncoderTest(GET_PARAM(0)), test_video_param_(GET_PARAM(2)),
        cpu_used_(GET_PARAM(3)), psnr_(0.0), nframes_(0),
        encoding_mode_(GET_PARAM(1)) {}

  virtual ~EndToEndTestLarge() {}

  virtual void SetUp() {
    InitializeConfig();
    SetMode(encoding_mode_);
    if (encoding_mode_ != ::libaom_test::kRealTime) {
      cfg_.g_lag_in_frames = 5;
      cfg_.rc_end_usage = AOM_VBR;
    } else {
      cfg_.g_lag_in_frames = 0;
      cfg_.rc_end_usage = AOM_CBR;
      cfg_.rc_buf_sz = 1000;
      cfg_.rc_buf_initial_sz = 500;
      cfg_.rc_buf_optimal_sz = 600;
    }
    dec_cfg_.threads = 4;
  }

  virtual void BeginPassHook(unsigned int) {
    psnr_ = 0.0;
    nframes_ = 0;
  }

  virtual void PSNRPktHook(const aom_codec_cx_pkt_t *pkt) {
    psnr_ += pkt->data.psnr.psnr[0];
    nframes_++;
  }

  virtual void PreEncodeFrameHook(::libaom_test::VideoSource *video,
                                  ::libaom_test::Encoder *encoder) {
    if (video->frame() == 1) {
      encoder->Control(AV1E_SET_FRAME_PARALLEL_DECODING, 1);
      encoder->Control(AV1E_SET_TILE_COLUMNS, 4);
      encoder->Control(AOME_SET_CPUUSED, cpu_used_);
      if (encoding_mode_ != ::libaom_test::kRealTime) {
        encoder->Control(AOME_SET_ENABLEAUTOALTREF, 1);
        encoder->Control(AOME_SET_ARNR_MAXFRAMES, 7);
        encoder->Control(AOME_SET_ARNR_STRENGTH, 5);
        encoder->Control(AOME_SET_ARNR_TYPE, 3);
      }
    }
  }

  double GetAveragePsnr() const {
    if (nframes_) return psnr_ / nframes_;
    return 0.0;
  }

  double GetPsnrThreshold() {
    return kPsnrThreshold[cpu_used_][encoding_mode_];
  }

  TestVideoParam test_video_param_;
  int cpu_used_;

 private:
  double psnr_;
  unsigned int nframes_;
  libaom_test::TestMode encoding_mode_;
};

TEST_P(EndToEndTestLarge, EndtoEndPSNRTest) {
  cfg_.rc_target_bitrate = kBitrate;
  cfg_.g_error_resilient = 0;
  cfg_.g_profile = test_video_param_.profile;
  cfg_.g_input_bit_depth = test_video_param_.input_bit_depth;
  cfg_.g_bit_depth = test_video_param_.bit_depth;
  init_flags_ = AOM_CODEC_USE_PSNR;
  if (cfg_.g_bit_depth > 8) init_flags_ |= AOM_CODEC_USE_HIGHBITDEPTH;

  libaom_test::VideoSource *video;
  if (is_extension_y4m(test_video_param_.filename)) {
    video =
        new libaom_test::Y4mVideoSource(test_video_param_.filename, 0, kFrames);
  } else {
    video = new libaom_test::YUVVideoSource(test_video_param_.filename,
                                            test_video_param_.fmt, kWidth,
                                            kHeight, kFramerate, 1, 0, kFrames);
  }

  ASSERT_NO_FATAL_FAILURE(RunLoop(video));
  const double psnr = GetAveragePsnr();
  EXPECT_GT(psnr, GetPsnrThreshold());
  delete (video);
}

#if CONFIG_AOM_HIGHBITDEPTH
#if CONFIG_AV1_ENCODER
// TODO(angiebird): many fail in high bitdepth mode.
INSTANTIATE_TEST_CASE_P(
    DISABLED_AV1, EndToEndTestLarge,
    ::testing::Combine(
        ::testing::Values(
            static_cast<const libaom_test::CodecFactory *>(&libaom_test::kAV1)),
        ::testing::ValuesIn(kEncodingModeVectors),
        ::testing::ValuesIn(kTestVectors),
        ::testing::ValuesIn(kCpuUsedVectors)));
#endif  // CONFIG_AV1_ENCODER
#else
AV1_INSTANTIATE_TEST_CASE(EndToEndTestLarge,
                          ::testing::ValuesIn(kEncodingModeVectors),
                          ::testing::ValuesIn(kTestVectors),
                          ::testing::ValuesIn(kCpuUsedVectors));
#endif  // CONFIG_AOM_HIGHBITDEPTH
}  // namespace
