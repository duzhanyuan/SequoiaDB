#include "pd.hpp"
#include "utilCompressorLZW.hpp"
#include "pdTrace.hpp"
#include "utilTrace.hpp"

namespace engine
{
   #define MAX_DICT_CTX_NUM      32

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILCOMPRESSORLZW_CONSTRUCTOR, "_utilCompressorLZW::_utilCompressorLZW" )
   _utilCompressorLZW::_utilCompressorLZW()
      : _utilCompressor( UTIL_COMPRESSOR_LZW ),
        _dictionary( NULL )
   {
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILCOMPRESSORLZW_DESTRUCTOR, "_utilCompressorLZW::~_utilCompressorLZW" )
   _utilCompressorLZW::~_utilCompressorLZW()
   {
      PD_TRACE_ENTRY( SDB__UTILCOMPRESSORLZW_DESTRUCTOR ) ;
      utilLZWContext *context = NULL ;

      while ( _vecContext.size() > 0 )
      {
         context = _vecContext.back() ;
         SDB_OSS_DEL context ;
         _vecContext.pop_back() ;
      }

      if ( _dictionary )
      {
         SDB_OSS_DEL _dictionary ;
      }

      PD_TRACE_EXIT( SDB__UTILCOMPRESSORLZW_DESTRUCTOR ) ;
   }

   /* Get a compressor context ready. The dictionary will be set. */
   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILCOMPRESSORLZW_PREPARE, "_utilCompressorLZW::prepare" )
   INT32 _utilCompressorLZW::prepare( utilCompressorContext &ctx )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__UTILCOMPRESSORLZW_PREPARE ) ;
      _utilLZWContext *context = NULL ;

      _vecCtxLatch.get() ;
      if ( _vecContext.size() > 0 )
      {
         context = _vecContext.back() ;
         _vecContext.pop_back() ;
      }
      _vecCtxLatch.release() ;

      if ( !context )
      {
         context = SDB_OSS_NEW _utilLZWContext ;
         PD_CHECK( context, SDB_OOM, error, PDERROR,
                   "Failed to allocate context for LZW, requested size: %d",
                   sizeof( _utilLZWContext ) ) ;
         context->setDictionary( _dictionary ) ;
      }

      ctx = ( utilCompressorContext )context ;
      _prepared  = TRUE ;
   done:
      PD_TRACE_EXITRC( SDB__UTILCOMPRESSORLZW_PREPARE, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   /*
    * Reset everything in the context except the dictionary. Then it can be used
    * directly to compress or decompress.
    */
   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILCOMPRESSORLZW_REPREPARE, "_utilCompressorLZW::rePrepare" )
   INT32 _utilCompressorLZW::rePrepare( utilCompressorContext &ctx )
   {
      PD_TRACE_ENTRY( SDB__UTILCOMPRESSORLZW_REPREPARE ) ;
      _utilLZWContext *context = ( _utilLZWContext * )ctx ;

      SDB_ASSERT( context, "Compressor context is invalid" ) ;
      SDB_ASSERT( context->isReady(), "Order of interface invoking is wrong" ) ;

      context->reset( TRUE ) ;

      PD_TRACE_EXIT( SDB__UTILCOMPRESSORLZW_REPREPARE ) ;
      return SDB_OK ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILCOMPRESSORLZW_SETDICTIONARY, "_utilCompressorLZW::setDictionary" )
   INT32 _utilCompressorLZW::setDictionary( const CHAR * dict, UINT32 dictLen )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB__UTILCOMPRESSORLZW_SETDICTIONARY ) ;
      SDB_ASSERT( dict && dictLen > 0, "Dictionary information is invalid" ) ;

      _dictionary = SDB_OSS_NEW _utilLZWDictionary ;
      PD_CHECK( _dictionary, SDB_OOM, error, PDERROR,
                "Failed to allocate memory for compressor dictionary, "
                "requested size: %d", sizeof( _utilLZWDictionary ) ) ;

      rc = _dictionary->loadFromStream( dict, dictLen ) ;
      PD_RC_CHECK( rc, PDERROR,
                   "Failed to set format dictionary for LZW, rc: %d", rc ) ;
   done:
      PD_TRACE_EXITRC( SDB__UTILCOMPRESSORLZW_SETDICTIONARY, rc ) ;
      return rc ;
   error:
      if ( _dictionary )
      {
         SDB_OSS_DEL _dictionary ;
      }
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILCOMPRESSORLZW__FREEDICTIONARY, "_utilCompressorLZW::_freeDictionary" )
   void _utilCompressorLZW::_freeDictionary()
   {
      PD_TRACE_ENTRY( SDB__UTILCOMPRESSORLZW__FREEDICTIONARY ) ;
      if ( _dictionary )
      {
         SDB_OSS_DEL _dictionary ;
         _dictionary = NULL ;
      }
      PD_TRACE_EXIT( SDB__UTILCOMPRESSORLZW__FREEDICTIONARY ) ;
   }

   size_t _utilCompressorLZW::compressBound( size_t srcLen )
   {
      /*
       * At the worst scenario, no string in the source with length greater than
       * 1 can be found in the dictionary. In this case, each character in the
       * source should be represented by one dictionary code separately. If the
       * code size is greater than 8, the data will expand after encoding...
       */
      return ( _dictionary->getCodeSize() * srcLen ) / 8 + 1 ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILCOMPRESSORLZW_COMPRESS, "_utilCompressorLZW::compress" )
   INT32 _utilCompressorLZW::compress( utilCompressorContext ctx,
                                       const CHAR* source, UINT32 sourceLen,
                                       CHAR* dest, UINT32 &destLen )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB__UTILCOMPRESSORLZW_COMPRESS ) ;
      rc = _lzw.encode( ( _utilLZWContext * )ctx, source,
                          sourceLen, dest, destLen ) ;
      PD_RC_CHECK( rc, PDERROR,
                   "Failed to encode data using LZW, rc: %d", rc ) ;
   done:
      PD_TRACE_EXITRC( SDB__UTILCOMPRESSORLZW_COMPRESS, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILCOMPRESSORLZW_DECOMPRESS, "_utilCompressorLZW::decompress" )
   INT32 _utilCompressorLZW::decompress( utilCompressorContext ctx,
                                         const CHAR* source, UINT32 sourceLen,
                                         CHAR* dest, UINT32 &destLen )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__UTILCOMPRESSORLZW_DECOMPRESS ) ;

      rc = _lzw.decode( ( _utilLZWContext * )ctx, source,
                          sourceLen, dest, destLen ) ;
      PD_RC_CHECK( rc, PDERROR,
                   "Failed to decode data using LZW, rc: %d", rc ) ;
   done:
      PD_TRACE_EXITRC( SDB__UTILCOMPRESSORLZW_DECOMPRESS, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILCOMPRESSORLZW_DONE, "_utilCompressorLZW::done" )
   INT32 _utilCompressorLZW::done( utilCompressorContext &ctx )
   {
      BOOLEAN delContext = FALSE ;
      PD_TRACE_ENTRY( SDB__UTILCOMPRESSORLZW_DONE ) ;
      _utilLZWContext *context = ( _utilLZWContext * )ctx ;
      SDB_ASSERT( context, "Invalid compressor context" ) ;

      context->reset( TRUE ) ;

      _vecCtxLatch.get() ;
      if ( _vecContext.size() < MAX_DICT_CTX_NUM )
      {
         _vecContext.push_back( context ) ;
      }
      else
      {
         delContext = TRUE ;

      }
      _vecCtxLatch.release() ;

      if ( delContext )
      {
         SDB_OSS_DEL context ;
      }

      ctx = UTIL_INVALID_COMP_CTX ;

      _prepared = FALSE ;

      PD_TRACE_EXIT( SDB__UTILCOMPRESSORLZW_DONE ) ;
      return SDB_OK ;
   }
}

