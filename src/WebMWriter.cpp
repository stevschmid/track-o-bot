#include "WebMWriter.h"

#include <QPainter>
#include <cassert>

#define VP8_FOURCC 0x30385056

WebMWriter::WebMWriter( int width, int height, int bitrate, int fps )
  : mWriter( NULL ),
    mSegment( NULL ),
    mLastPtsNs( 0 ),
    mFrameCounter( 0 )
{
  mFramerate.num = fps;
  mFramerate.den = 1;

  // mCfg
  vpx_codec_err_t err;
  err = vpx_codec_enc_config_default( &vpx_codec_vp8_cx_algo, &mCfg, 0 );
  assert( !err );

  mCfg.g_w = width;
  mCfg.g_h = height;

  mCfg.rc_target_bitrate = bitrate;
  mCfg.rc_end_usage = VPX_CBR;
  mCfg.g_pass = VPX_RC_ONE_PASS;

  mCfg.g_timebase.den = 1000;

  // mCodec
  err = vpx_codec_enc_init( &mCodec, &vpx_codec_vp8_cx_algo, &mCfg, 0 );
  assert( !err );

  // mImg
	vpx_img_alloc( &mImg, VPX_IMG_FMT_YV12, width, height, 1 );
}

WebMWriter::~WebMWriter() {
  if( IsOpen() ) {
    Close();
  }

  if( mWriter )
    delete mWriter;
  if( mSegment )
    delete mSegment;

  vpx_codec_destroy( &mCodec );
  vpx_img_free( &mImg );
}

bool WebMWriter::IsOpen() const {
  return mWriter && mSegment;
}

bool WebMWriter::Open( const QString& path ) {
  if( IsOpen() )
    return false;

  mFrameCounter = 0;

  // libvpx/webm.cc
  mWriter = new mkvmuxer::MkvWriter();
  if( !mWriter->Open( qt2cstr( path ) ) ) {
    delete mWriter;
    mWriter = NULL;
    return false;
  }

  mSegment = new mkvmuxer::Segment();
  mSegment->Init( mWriter );
  mSegment->set_mode( mkvmuxer::Segment::kFile );
  mSegment->OutputCues( true );

  mkvmuxer::SegmentInfo *const info = mSegment->GetSegmentInfo();
  const uint64_t kTimecodeScale = 1000000;
  info->set_timecode_scale( kTimecodeScale );
  std::string version = "Track-o-Bot";
  info->set_writing_app( version.c_str() );

  const uint64_t video_track_id =
      mSegment->AddVideoTrack(static_cast<int>( mCfg.g_w ),
                             static_cast<int>( mCfg.g_h ),
                             1);
  mkvmuxer::VideoTrack* const video_track =
      static_cast<mkvmuxer::VideoTrack*>(
          mSegment->GetTrackByNumber( video_track_id ));
  video_track->set_codec_id( "V_VP8" );

  return true;
}

void WebMWriter::Close() {
  if( IsOpen() ) {
    DBG( "Close WebM" );
    // libvpx/webm.cc
    mSegment->Finalize();
    mWriter->Close();
    delete mSegment;
    delete mWriter;
  }

  mWriter = NULL;
  mSegment = NULL;
}

void WebMWriter::WriteBlock( const vpx_codec_cx_pkt_t *pkt ) {
  // libvpx/webm.cc
  int64_t pts_ns = pkt->data.frame.pts * 1000000000ll *
                   mCfg.g_timebase.num / mCfg.g_timebase.den;

  if( pts_ns <= mLastPtsNs )
    pts_ns = mLastPtsNs + 1000000;
  mLastPtsNs = pts_ns;
  mSegment->AddFrame( static_cast<uint8_t*>( pkt->data.frame.buf ),
                      pkt->data.frame.sz,
                      1,
                      pts_ns,
                      pkt->data.frame.flags & VPX_FRAME_IS_KEY );
}

void WebMWriter::AddFrame( const QImage& raw ) {
  if( !IsOpen() ) {
    DBG( "[WebM] AddFrame called but writer not open" );
    return;
  }

  QImage img = Rescale( raw, mCfg.g_w, mCfg.g_h );

  DBG( "[WebM] Adding frame nr: %d size: %d %d",
      mFrameCounter, img.width(), img.height() );
  QImageToYV12( img, &mImg );

  mFrameCounter++;

  vpx_codec_pts_t frameStart = ( mCfg.g_timebase.den * ( int64_t )( mFrameCounter - 1) * mFramerate.den )
    / mCfg.g_timebase.num / mFramerate.num;
  vpx_codec_pts_t nextFrameStart = ( mCfg.g_timebase.den * ( int64_t )( mFrameCounter ) * mFramerate.den )
    / mCfg.g_timebase.num / mFramerate.num;
  unsigned long duration = ( unsigned long )( nextFrameStart - frameStart );

  vpx_codec_err_t err = vpx_codec_encode( &mCodec, &mImg,
      frameStart, duration, 0, VPX_DL_GOOD_QUALITY );
  assert( !err );

  const vpx_codec_cx_pkt_t *pkt;
  vpx_codec_iter_t iter = NULL;
  while( ( pkt = vpx_codec_get_cx_data( &mCodec, &iter ) ) ) {
    switch( pkt->kind ) {
      case VPX_CODEC_CX_FRAME_PKT:
        WriteBlock( pkt );
        break;
      default:
        break;
    }
  }
}

#define CONVERT(ptr, rx, gx, bx, offset) ( ( \
      ( rx * ( ( *ptr >> 16 ) & 0xFF )  +  \
        gx * ( ( *ptr >> 8 ) & 0xFF ) + \
        bx * ( *ptr & 0xFF ) \
      ) >> 8 ) + offset )

void WebMWriter::QImageToYV12( const QImage& src, vpx_image_t *dst )
{
  const QRgb *rgb = reinterpret_cast< const QRgb* >( src.constBits() );
  int width = src.width();
  int height = src.height();

  uint8_t *y = dst->planes[ VPX_PLANE_Y ];
  uint8_t *u = dst->planes[ VPX_PLANE_U ];
  uint8_t *v = dst->planes[ VPX_PLANE_V ];

  for( int l = 0; l < height; l++ ) {
    if( l % 2 == 0 ) {
      for( int x = 0; x < width; x += 2, rgb++ ) {
        *(y++) = CONVERT( rgb, 66, 129, 25, 16 );
        *(u++) = CONVERT( rgb, -38, -74, 112, 128 );
        *(v++) = CONVERT( rgb, 112, -94, -18, 128 );
        rgb++;
        *(y++) = CONVERT( rgb, 66, 129, 25, 16 );
      }
    } else {
      for( int x = 0; x < width; x++, rgb++ ) {
        *(y++) = CONVERT( rgb, 66, 129, 25, 16 );
      }
    }
  }
}

QImage WebMWriter::Rescale( const QImage& img, int width, int height ) {
  QImage dst( width, height, img.format() );
  dst.fill( Qt::black );

  QImage src = img.scaled( width, height,
      Qt::KeepAspectRatio, Qt::SmoothTransformation );
  QPainter painter( &dst );

  int x = ( dst.width() - src.width() ) >> 1;
  int y = ( dst.height() - src.height() ) >> 1;

  painter.drawImage( x, y, src );
  return dst;
}
