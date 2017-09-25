#include "core.hpp"
#include "oss.hpp"
#include "ossUtil.hpp"
#include "pd.hpp"
#include "utilCompressorZlib.hpp"
#include "zlib.h"

namespace engine
{
   _utilCompressorZlib::_utilCompressorZlib()
      : _utilCompressor( UTIL_COMPRESSOR_ZLIB )
   {
      ossMemset(&_stream, 0, sizeof(_stream)) ;
      _state = STREAM_NOT_INIT ;
   }

   _utilCompressorZlib::~_utilCompressorZlib()
   {
      if ( _dictCopy && _dictionary )
      {
         SDB_OSS_FREE( _dictionary ) ;
      }

      if ( STREAM_INIT_FOR_DEFLATE == _state )
      {
         deflateEnd( &_stream );
      }
      else
      {
         if ( STREAM_INIT_FOR_INFLATE == _state )
         {
            inflateEnd( &_stream ) ;
         }
      }
   }

   INT32 _utilCompressorZlib::prepare( UTIL_COMPRESSOR_WORK workType )
   {
      INT32 rc = SDB_OK ;

      if ( UTIL_COMP_COMPRESS == workType )
      {
         rc = deflateInit( &_stream, Z_BEST_COMPRESSION ) ;
         PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_INIT_FAIL, error,
                   PDERROR, "deflateInit failed when initialize zlib "
                   "compressor, rc: %d", rc ) ;
         _state = STREAM_INIT_FOR_DEFLATE ;
      }
      else
      {
         rc = inflateInit( &_stream ) ;
         PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_INIT_FAIL, error,
                   PDERROR, "inflateInit failed when initialize zlib "
                   "compressor, rc: %d", rc ) ;
         _state = STREAM_INIT_FOR_INFLATE ;
      }
   done:
      return rc ;
   error:
      goto done ;
   }

#if 0
   INT32 _utilCompressorZlib::init()
   {
      INT32 rc = SDB_OK ;

      rc = deflateInit( &_stream, Z_BEST_COMPRESSION ) ;
      PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_INIT_FAIL, error,
                PDERROR, "deflateInit failed when initialize zlib compressor, "
                "rc: %d", rc ) ;
      _deflateInit = TRUE ;

      rc = inflateInit( &_stream ) ;
      PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_INIT_FAIL, error,
                PDERROR, "inflateInit failed when initialize zlib compressor, "
                "rc: %d", rc ) ;
      _inflateInit = TRUE ;
   done:
      return rc ;
   error:
      goto done ;
   }
#endif

   INT32 _utilCompressorZlib::setDictionary( const CHAR* dict, UINT32 dictSize,
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

   size_t _utilCompressorZlib::compressBound( size_t srcLen )
   {
      return deflateBound( &_stream, srcLen ) ;
   }

#if 0
   INT32 _utilCompressorZlib::compress( const CHAR* source, UINT32 sourceSize,
                                        CHAR* dest, UINT32 &destSize )
   {
      INT32 rc = SDB_OK ;
      BOOLEAN streamInited = FALSE ;

      rc = deflateInit( &_stream, Z_BEST_COMPRESSION ) ;
      PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_INIT_FAIL, error, PDERROR,
                "Zlib compressor initialization failed when compressing" ) ;

      streamInited = TRUE ;
      if ( NULL != _dictionary )
      {
         rc = deflateSetDictionary( &_stream,
                                 ( const Bytef* )_dictionary, _dictSize ) ;
         PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_SETDICT_FAIL, error,
                   PDERROR,
                   "Failed to set dictionary for zlib when compressiong" ) ;
      }

      _stream.data_type = Z_BINARY;
      _stream.next_in = (Bytef*)source;
      _stream.avail_in = sourceSize;
      _stream.next_out = (Bytef*)dest;
      _stream.avail_out = destSize;

      rc = deflate(&_stream, Z_FINISH);
      if ( Z_STREAM_END == rc )
      {
         destSize = _stream.total_out ;
         deflateEnd( &_stream ) ;
         rc = SDB_OK ;
      }
      else if ( Z_OK == rc )
      {
         // Out put buffer not enough.
         rc = SDB_UTIL_COMP_BUF_SMALL ;
         PD_LOG( PDERROR, "Output buffer is too small to compress" ) ;
         goto error ;
      }
      else
      {
         rc = SDB_UTIL_COMP_COMPRESS_FAIL ;
         PD_LOG( PDERROR, "Compress data failed" ) ;
         goto error ;
      }

   done:
      if ( streamInited )
      {
         deflateEnd( &_stream ) ;
      }
      return rc ;
   error:
      goto done ;
   }
#endif

   INT32 _utilCompressorZlib::compress( const CHAR* source, UINT32 sourceSize,
                                        CHAR* dest, UINT32 &destSize )
   {
      INT32 rc = SDB_OK ;
      //BOOLEAN streamInited = FALSE ;

      deflateResetKeep( &_stream ) ;
      if ( NULL != _dictionary )
      {
         rc = deflateSetDictionary( &_stream,
                                    ( const Bytef* )_dictionary, _dictSize ) ;
         PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_SETDICT_FAIL, error,
                   PDERROR,
                   "Failed to set dictionary for zlib when compressiong" ) ;
      }

      _stream.data_type = Z_BINARY;
      _stream.next_in = (Bytef*)source;
      _stream.avail_in = sourceSize;
      _stream.next_out = (Bytef*)dest;
      _stream.avail_out = destSize;

      rc = deflate(&_stream, Z_FINISH);
      if ( Z_STREAM_END == rc )
      {
         destSize = _stream.total_out ;
         rc = SDB_OK ;
         goto done ;
      }
      else if ( Z_OK == rc )
      {
         // Out put buffer not enough.
         rc = SDB_UTIL_COMP_BUF_SMALL ;
         PD_LOG( PDERROR, "Output buffer is too small to compress" ) ;
         goto error ;
      }
      else
      {
         rc = SDB_UTIL_COMP_COMPRESS_FAIL ;
         PD_LOG( PDERROR, "Compress data failed" ) ;
         goto error ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

#if 0
   INT32 _utilCompressorZlib::decompress( const CHAR* source, UINT32 sourceSize,
                                          CHAR* dest, UINT32 &destSize )
   {
      INT32 rc = SDB_OK ;
      BOOLEAN streamInit = FALSE ;

      rc = inflateInit( &_stream ) ;
      PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_INIT_FAIL, error, PDERROR,
                "Zlib compressor initialization failed when decompressing" ) ;

      streamInit = TRUE ;
      _stream.next_in = (Bytef *)source ;
      _stream.avail_in = sourceSize ;
      _stream.next_out = (Bytef *)dest ;
      _stream.avail_out = destSize ;

      rc = inflate( &_stream, Z_FINISH ) ;
      if ( Z_NEED_DICT == rc )
      {
         rc = inflateSetDictionary( &_stream,
                                    ( const unsigned char* )_dictionary,
                                    _dictSize ) ;
         PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_SETDICT_FAIL, error,
                   PDERROR,
                   "Failed to set dictionary for zlib when decompressing" ) ;
      }

      while ( ( rc = inflate( &_stream, Z_FINISH ) ) == Z_OK )
         ;

      PD_CHECK( Z_STREAM_END == rc, SDB_UTIL_COMP_DECOMPRESS_FAIL,
                error, PDERROR,
                "Failed to decompress data with zlib" ) ;

      destSize = _stream.total_out;
      rc = SDB_OK ;

   done:
      if ( streamInit )
      {
         inflateEnd( &_stream ) ;
      }

      return rc ;
   error:
      goto done ;
   }
#endif

   INT32 _utilCompressorZlib::decompress( const CHAR* source, UINT32 sourceSize,
                                          CHAR* dest, UINT32 &destSize )
   {
      INT32 rc = SDB_OK ;
      BOOLEAN streamInit = FALSE ;

      rc = inflateResetKeep( &_stream ) ;
      PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_INIT_FAIL, error, PDERROR,
                "Zlib compressor initialization failed when decompressing" ) ;

      streamInit = TRUE ;
      _stream.next_in = (Bytef *)source ;
      _stream.avail_in = sourceSize ;
      _stream.next_out = (Bytef *)dest ;
      _stream.avail_out = destSize ;

      rc = inflate( &_stream, Z_FINISH ) ;
      if ( Z_NEED_DICT == rc && _dictionary )
      {
         rc = inflateSetDictionary( &_stream,
                                    ( const unsigned char* )_dictionary,
                                    _dictSize ) ;
         PD_CHECK( Z_OK == rc, SDB_UTIL_COMP_SETDICT_FAIL, error,
                   PDERROR,
                   "Failed to set dictionary for zlib when decompressing" ) ;
      }

      while ( ( rc = inflate( &_stream, Z_FINISH ) ) == Z_OK )
         ;

      PD_CHECK( Z_STREAM_END == rc, SDB_UTIL_COMP_DECOMPRESS_FAIL,
                error, PDERROR,
                "Failed to decompress data with zlib" ) ;

      destSize = _stream.total_out;
      rc = SDB_OK ;

   done:
      return rc ;
   error:
      goto done ;
   }
}

