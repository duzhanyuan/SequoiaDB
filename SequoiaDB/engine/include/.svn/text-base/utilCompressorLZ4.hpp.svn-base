#ifndef UTIL_COMPRESSOR_LZ4__
#define UTIL_COMPRESSOR_LZ4__

#include "utilCompressor.hpp"
#include "lz4.h"

namespace engine
{
   class _utilCompressorLZ4 : public utilCompressor
   {
   public:
      _utilCompressorLZ4() ;
      ~_utilCompressorLZ4() ;
   public:
      INT32 init() ;
      INT32 setDictionary( const CHAR* dict, UINT32 dict_size,
                           BOOLEAN copy = FALSE ) ;
      size_t compressBound( size_t srcLen ) ;
      INT32 compress( const CHAR* source, UINT32 sourceSize,
                      CHAR* dest, UINT32 &destSize ) ;
      INT32 decompress( const CHAR* source, UINT32 sourceSize,
                        CHAR* dest, UINT32 &destSize ) ;
      UINT32 saveDict( CHAR *dictBuf, UINT32 bufSize ) ;
      INT32 prepare( UTIL_COMPRESSOR_WORK workType ) {}
   private:
      LZ4_stream_t* _stream ;
      LZ4_streamDecode_t* _streamDecode ;
   };
   typedef _utilCompressorLZ4 utilCompressorLZ4 ;
}

#endif /* UTIL_COMPRESSOR_LZ4__ */

