#pragma once

#define VPX_CODEC_DISABLE_COMPAT

#include <vpx/vpx_encoder.h>
#include <vpx/vp8cx.h>

#include <mkvmuxer.hpp>
#include <mkvmuxerutil.hpp>
#include <mkvwriter.hpp>

#include <QString>
#include <QImage>

#define VP8_FOURCC 0x30385056

class WebMWriter {
private:
  mkvmuxer::MkvWriter *mWriter;
  mkvmuxer::Segment *mSegment;
  int64_t mLastPtsNs;

  vpx_rational mFramerate;
  int mFrameCounter;

  vpx_codec_enc_cfg_t mCfg;
  vpx_codec_ctx_t mCodec;
  vpx_image_t mImg;

  static void QImageToYV12( const QImage& src, vpx_image_t *dst );
  static QImage Rescale( const QImage& img, int width, int height );

  void WriteBlock( const vpx_codec_cx_pkt_t *pkt );

public:
  WebMWriter( int width, int height, int bitrate, int fps );
  ~WebMWriter();

  bool IsOpen() const;
  bool Open( const QString& path );
  void Close();

  void AddFrame( const QImage& img );
};
