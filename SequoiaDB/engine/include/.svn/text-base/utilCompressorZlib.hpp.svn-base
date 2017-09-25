#ifndef UTIL_COMPRESSOR_ZLIB__
#define UTIL_COMPRESSOR_ZLIB__

#include "utilCompressor.hpp"
#include "zlib.h"

namespace engine
{
#if 0
   class _utilCompressorZlib : public utilCompressor
   {
      public:
         _utilCompressorZlib();
         ~_utilCompressorZlib();
      public:
         INT32 init();
         INT32 setDictionary( const CHAR* dict, UINT32 dict_size,
                              BOOLEAN copy = FALSE ) ;
         size_t compressBound( size_t srcLen ) ;
         INT32 compress( const CHAR* source, UINT32 sourceSize,
                        CHAR* dest, UINT32 &destSize ) ;
         INT32 decompress( const CHAR* source, UINT32 sourceSize,
                          CHAR* dest, UINT32 &destSize ) ;
      private:
         z_stream _stream ;
         z_stream _streamDecompress ;
         BOOLEAN _deflateInit ;
         BOOLEAN _inflateInit ;
   };
   typedef _utilCompressorZlib utilCompressorZlib ;
#endif

   enum STREAM_INIT_STATE
   {
      STREAM_NOT_INIT = 0,
      STREAM_INIT_FOR_DEFLATE,
      STREAM_INIT_FOR_INFLATE
   } ;

   class _utilCompressorZlib : public utilCompressor
   {
      public:
         _utilCompressorZlib();
         ~_utilCompressorZlib();
      public:
         INT32 prepare( UTIL_COMPRESSOR_WORK workType );
         INT32 setDictionary( const CHAR* dict, UINT32 dict_size,
                              BOOLEAN copy = FALSE ) ;
         size_t compressBound( size_t srcLen ) ;
         INT32 compress( const CHAR* source, UINT32 sourceSize,
                        CHAR* dest, UINT32 &destSize ) ;
         INT32 decompress( const CHAR* source, UINT32 sourceSize,
                          CHAR* dest, UINT32 &destSize ) ;
      private:
         z_stream _stream ;   /* Stream for compress and decompress */
         STREAM_INIT_STATE _state ;
   };
   typedef _utilCompressorZlib utilCompressorZlib ;
}

#endif /* UTIL_COMPRESSOR_ZLIB__ */

