#include "pd.hpp"
#include "pdTrace.hpp"
#include "utilTrace.hpp"
#include "utilCompressor.hpp"
#include "utilLZW.hpp"

namespace engine
{
   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILLZWDICTIONARY_INIT, "_utilLZWDictionary::init" )
   INT32 _utilLZWDictionary::init( UINT32 maxNodeNum )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__UTILLZWDICTIONARY_INIT ) ;

      _maxNodeNum = maxNodeNum ;
      _nodes = ( _utilLZWNode * )SDB_OSS_MALLOC(
                                       sizeof( _utilLZWNode ) * maxNodeNum ) ;
      PD_CHECK( _nodes, SDB_OOM, error, PDERROR,
                "Failed to allocate memory for dictionary items, "
                "requested size: %d", sizeof( _utilLZWNode ) * maxNodeNum );

      for ( UINT32 i = 0; i < maxNodeNum; ++i )
      {
         _nodes[i]._prev = DICT_INVALID_NODE ;
         _nodes[i]._first = DICT_INVALID_NODE ;
         _nodes[i]._next = DICT_INVALID_NODE ;
         /*
          * 1 byte(8 bits) can represent 256 characters. Every search will start
          * with them.
          */
         if ( i < 256 )
         {
            _nodes[i]._ch = i ;
            _nodes[i]._len = 1 ;
         }
         else
         {
            _nodes[i]._len = 0 ;
         }
      }

      _head._maxCode = 255 ;
      _head._codeSize = 8 ;

   done:
      PD_TRACE_EXITRC( SDB__UTILLZWDICTIONARY_INIT, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILLZWDICTIONARY_RESET, "_utilLZWDictionary::reset" )
   void _utilLZWDictionary::reset()
   {
      PD_TRACE_ENTRY( SDB__UTILLZWDICTIONARY_RESET ) ;
      _head._codeSize = 0 ;
      _head._maxCode = 0 ;
      if ( _nodes )
      {
         SDB_OSS_FREE( _nodes ) ;
         _nodes = NULL ;
      }

      PD_TRACE_EXIT( SDB__UTILLZWDICTIONARY_RESET ) ;
   }

   UINT32 _utilLZWDictionary::getDictSize()
   {
      return sizeof( _utilLZWDictHead ) +
             sizeof( _utilLZWNode ) * ( _head._maxCode + 1 ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILLZWDICTIONARY_DUMPTOSTREAM, "_utilLZWDictionary::dumpToStream" )
   INT32 _utilLZWDictionary::dumpToStream( CHAR *stream, UINT32 &length )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__UTILLZWDICTIONARY_DUMPTOSTREAM ) ;
      UINT32 pos = 0 ;
      UINT32 nodeTotalSize = sizeof( _utilLZWNode ) * ( _head._maxCode + 1 ) ;
      UINT32 totalSize = sizeof( _utilLZWDictHead ) + nodeTotalSize ;

      SDB_ASSERT( stream, "Buffer dor dictionary is invalid" ) ;
      PD_CHECK( length >= totalSize, SDB_INVALIDARG, error, PDERROR,
                "Length of dictionary buffer is invalid, requested size: %d, "
                "actual size: %d", totalSize, length ) ;

      *(UINT32 *)stream = _head._codeSize ;
      pos += sizeof( UINT32 ) ;
      *(UINT32 *)(stream + pos) = _head._maxCode ;
      pos += sizeof( UINT32 ) ;
      ossMemcpy( stream + pos, (CHAR *)_nodes, nodeTotalSize ) ;
      length = totalSize ;

   done:
      PD_TRACE_EXITRC( SDB__UTILLZWDICTIONARY_DUMPTOSTREAM, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILLZWDICTIONARY_LOADFROMSTREAM, "_utilLZWDictionary::loadFromStream" )
   INT32 _utilLZWDictionary::loadFromStream( const CHAR *stream, UINT32 len )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__UTILLZWDICTIONARY_LOADFROMSTREAM ) ;
      UINT32 readPos = 0 ;
      UINT32 totalNodeSize = 0 ;

      _head._codeSize = *( UINT32 * )stream ;
      readPos += sizeof( UINT32 ) ;
      _head._maxCode = *( UINT32 * )( stream + readPos ) ;
      readPos += sizeof( UINT32 ) ;

      totalNodeSize = sizeof( utilLZWNode ) * ( _head._maxCode + 1 ) ;

      SDB_ASSERT( len == ( sizeof(UINT32) * 2 + totalNodeSize ),
                  "Dictionary data is invalid" ) ;

      _nodes = ( _utilLZWNode * )SDB_OSS_MALLOC( totalNodeSize ) ;
      PD_CHECK( _nodes, SDB_OOM, error, PDERROR,
                "Failed to allocate memory for compressor dictionary, "
                "requested size: %d", totalNodeSize ) ;
      ossMemcpy( _nodes, stream + sizeof( UINT32 ) * 2, totalNodeSize ) ;
   done:
      PD_TRACE_EXITRC( SDB__UTILLZWDICTIONARY_LOADFROMSTREAM, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILLZWDICTCREATOR_PREPARE, "_utilLZWDictionary::prepare" )
   INT32 _utilLZWDictCreator::prepare( UINT32 maxSize )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__UTILLZWDICTCREATOR_PREPARE ) ;
      PD_CHECK( maxSize >= MIN_DICT_SIZE, SDB_INVALIDARG, error, PDERROR,
                "Dictionary size provided is too small: %d. The mininum requred"
                " size is: %d", maxSize, MIN_DICT_SIZE ) ;

      _dictionary = SDB_OSS_NEW utilLZWDictionary ;
      PD_CHECK( _dictionary, SDB_OOM, error, PDERROR,
                "Failed to allocate memory for creating dictionry, "
                "requested size: %d", maxSize ) ;

      _dictionary->init( ( maxSize - sizeof( _utilLZWDictHead ) )
                         / sizeof( _utilLZWNode ) ) ;
      _ctx.setDictionary( _dictionary ) ;

   done:
      PD_TRACE_EXITRC( SDB__UTILLZWDICTCREATOR_PREPARE, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILLZWDICTCREATOR_RESET, "_utilLZWDictionary::reset" )
   void _utilLZWDictCreator::reset()
   {
      PD_TRACE_ENTRY( SDB__UTILLZWDICTCREATOR_RESET ) ;
      if ( _dictionary )
      {
         SDB_OSS_DEL _dictionary ;
         _dictionary = NULL ;
      }

      _ctx.reset( FALSE ) ;
      PD_TRACE_EXIT( SDB__UTILLZWDICTCREATOR_RESET ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILLZWDICTCREATOR_BUILD, "_utilLZWDictionary::build" )
   INT32 _utilLZWDictCreator::build( const CHAR *source, UINT32 sourceLen,
                                     BOOLEAN &full )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__UTILLZWDICTCREATOR_BUILD ) ;
      UINT8 ch = 0 ;
      UINT32 pos = 0 ;
      UINT32 strLen = 0 ;
      LZW_CODE code = DICT_INVALID_NODE ;
      LZW_CODE nextCode = DICT_INVALID_NODE ;
      LZW_CODE maxCode = _ctx.getDictionary()->getMaxNodeNum() - 1 ;

      ch = source[0] ;
      code = ch ;
      pos++ ;
      strLen++ ;
      full = FALSE ;

      for ( ; pos < sourceLen; ++pos )
      {
         ch = source[pos] ;
         nextCode = _dictionary->findStr( code, ch ) ;
         if ( DICT_INVALID_NODE == nextCode )
         {
            nextCode = _dictionary->addStr( code, ch ) ;
            if ( nextCode == maxCode )
            {
               /* Dictionary is full */
               full = TRUE ;
               goto done ;
            }

            code = ch ;
            strLen = 1 ;
         }
         else
         {
            code = nextCode ;
            strLen++ ;
         }
      }

   done:
      PD_TRACE_EXITRC( SDB__UTILLZWDICTCREATOR_BUILD, rc ) ;
      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__UTILLZWDICTCREATOR_SAVE, "_utilLZWDictionary::save" )
   INT32 _utilLZWDictCreator::save( CHAR *dictBuf, UINT32 &maxDictLen )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__UTILLZWDICTCREATOR_SAVE ) ;

      rc = _dictionary->dumpToStream( dictBuf, maxDictLen ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to get LZW dictionary, rc: %d", rc ) ;
   done:
      PD_TRACE_EXITRC( SDB__UTILLZWDICTCREATOR_SAVE, rc ) ;
      return rc ;
   error:
      goto done ;
   }
}

