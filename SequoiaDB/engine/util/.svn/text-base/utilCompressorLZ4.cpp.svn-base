#include "pd.hpp"
#include "ossUtil.h"
#include "utilCompressorLZ4.hpp"

namespace engine
{
    _utilCompressorLZ4::_utilCompressorLZ4()
      : _utilCompressor( UTIL_COMPRESSOR_LZ4 )
    {
      _stream = NULL;
      _streamDecode = NULL;
    }

    _utilCompressorLZ4::~_utilCompressorLZ4()
    {
      if ( !_stream )
      {
         ( void )LZ4_freeStream( _stream );
      }

      if ( !_streamDecode )
      {
         (void)LZ4_freeStreamDecode(_streamDecode);
      }

      if ( _dictCopy && _dictionary )
      {
         SDB_OSS_FREE( _dictionary ) ;
      }
    }

   INT32 _utilCompressorLZ4::init()
   {
      INT32 rc = SDB_OK ;
      _stream = LZ4_createStream();
      PD_CHECK( _stream, SDB_OOM, error, PDERROR,
                "Failed to create LZ4 encode stream" ) ;

      _streamDecode = LZ4_createStreamDecode();
      PD_CHECK( _streamDecode, SDB_OOM, error, PDERROR,
                "Failed to create LZ4 decode stream" ) ;
   done:
      return rc ;
   error:
      if ( _stream )
      {
         ( void )LZ4_freeStream( _stream ) ;
      }
      goto done ;
   }

   INT32 _utilCompressorLZ4::setDictionary( const CHAR* dict, UINT32 dictSize,
                                            BOOLEAN copy )
   {
      INT32 rc = SDB_OK ;
      SDB_ASSERT( dict && ( dictSize > 0 ), "Invalid argument value" ) ;

      if ( copy )
      {
         _dictionary = (CHAR *)SDB_OSS_MALLOC( dictSize ) ;
         PD_CHECK( _dictionary, SDB_OOM, error, PDERROR,
                   "Failed to allocate memory to store dictionary by "
                   "compressor, requested size: %d", dictSize ) ;
         ossMemcpy( _dictionary, dict, dictSize ) ;
      }
      else
      {
         _dictionary = (CHAR *)dict ;
      }
      _dictSize = dictSize ;
      _dictCopy = copy ;

   done:
      return rc ;
   error:
      goto done ;
   }

   size_t _utilCompressorLZ4::compressBound( size_t srcLen )
   {
      return LZ4_COMPRESSBOUND( srcLen ) ;
   }

   INT32 _utilCompressorLZ4::compress( const CHAR* source, UINT32 sourceSize,
                                       CHAR* dest, UINT32 &destSize )
   {
      INT32 rc = SDB_OK ;

      SDB_ASSERT( _stream, "LZ4 compressor is not initialized" ) ;
      SDB_ASSERT( source && dest, "Invalid argument value" ) ;

      LZ4_resetStream( _stream ) ;

      if ( 0 != _dictSize )
      {
         INT32 size = LZ4_loadDict(_stream, _dictionary, _dictSize);
         PD_CHECK( (size > 0), SDB_UTIL_COMP_SETDICT_FAIL, error, PDERROR,
                   "Set dictionary for LZ4 failed when compressing" ) ;
      }

      destSize = LZ4_compress_fast_continue( _stream, source, dest,
                                            sourceSize, destSize, 1 ) ;
      PD_CHECK( (destSize > 0), SDB_UTIL_COMP_COMPRESS_FAIL, error, PDERROR,
                "Compress data with LZ4 failed" ) ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _utilCompressorLZ4::decompress( const CHAR* source, UINT32 sourceSize,
                                         CHAR* dest, UINT32 &destSize )
   {
      INT32 rc = SDB_OK ;
      destSize = LZ4_decompress_safe_continue( _streamDecode, source, dest,
                                              sourceSize, destSize ) ;
      PD_CHECK( (destSize > 0), SDB_UTIL_COMP_DECOMPRESS_FAIL,
               error, PDERROR,
                "Decompress data with LZ4 failed" ) ;
   done:
      return rc ;
   error:
      goto done ;
   }

   UINT32 _utilCompressorLZ4::saveDict( CHAR *dictBuf, UINT32 bufSize )
   {
      return LZ4_saveDict( _stream, dictBuf, bufSize ) ;
   }
}

