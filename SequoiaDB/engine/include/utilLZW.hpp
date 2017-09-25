#ifndef UTIL_LZW__
#define UTIL_LZW__

#include "core.hpp"
#include "oss.hpp"
#include "ossMem.hpp"
#include "ossUtil.h"
#include "pd.hpp"

namespace engine
{
   #define DICT_INVALID_NODE         4294967295

   /*
    * Internally we use a 32bit buffer to do continuous bits reading and,
    * writting, so the maximum node code should be less than 2^24(16M).
    */
   #define MAX_NODE_CODE        ( 1 << 24 - 1 )

   /*
    * 0~255 represent 256 diffrent symbols(initial state of the dictionary),
    * duplicated strings can be handle only when more are added to the
    * dictionary.
    */
   #define MIN_NODE_NUM         ( 256 + 1 )
   #define MAX_STREAM_BUFF_SIZE 256

   typedef UINT32 LZW_CODE ;

   /*
    * Used to perform byte(bits) reading and writting of compressed stream data.
    * As code of each dictionary node will be stored as part of the dictionary,
    * we use at least bits to represent a node as possible in order to save
    * space( both in memory and on disk). The number of bits may be not multiple
    * of 8, but we store the stream byte by byte. So the bits of one code may
    * be handled in more than one shot. This buffer is used to handle the
    * combination of the code bits.
    */
   struct _utilBitBuffer
   {
      UINT32 _buf ;
      UINT32 _n ;     // Number of bits remain in the buffer.
   } ;
   typedef _utilBitBuffer utilBitBuffer ;

   /*
    * Dictionary node in LZW. Each node represents a string, which linked by
    * prev of each node. This node contains the last character of the string.
    * All nodes which mark this node as 'prev' are linked by their 'next', and
    * this parent node points to the first child using 'first'.
    */
   struct _utilLZWNode
   {
      LZW_CODE  _prev;   // Code of the previous part of the string
                         // (the last character excluded)
      LZW_CODE  _first;  // first 'child'
      LZW_CODE  _next;   // first 'brother'
      UINT32    _len ;   // Length of the string.
      UINT8     _ch;     // Last character of the string this node represents.
      UINT8     _pad[3] ;
   } ;
   typedef _utilLZWNode utilLZWNode ;

   struct _utilLZWDictHead
   {
      UINT32 _codeSize;       /* Bit number to represent a code. */
      UINT32 _maxCode;        /* Maximum code in the dictionary. */
   } ;
   typedef _utilLZWDictHead utilLZWDictHead ;

   class _utilLZWDictionary : public SDBObject
   {
   public:
      _utilLZWDictionary()
      {
         _head._codeSize = 0 ;
         _head._maxCode = 0 ;
         _nodes = NULL ;
      }

      ~_utilLZWDictionary()
      {
         if ( _nodes )
         {
            SDB_OSS_FREE( _nodes ) ;
         }
      }

      INT32 init( UINT32 maxNodeNum ) ;
      void reset() ;
      UINT32 getMaxNodeNum() { return _maxNodeNum ; }
      UINT32 getCodeSize() { return _head._codeSize ; }
      UINT32 getMaxCode() { return _head._maxCode ; }
      BOOLEAN codeFull()
      {
         return ( _head._maxCode == (UINT32)( 1 << _head._codeSize ) ) ;
      }

      void codeSizeInc() { _head._codeSize++; }
      UINT32 getDictSize() ;
      INT32 dumpToStream( CHAR *stream, UINT32 &length ) ;
      INT32 loadFromStream( const CHAR *stream, UINT32 len ) ;

      OSS_INLINE LZW_CODE addStr( LZW_CODE preCode, UINT8 ch ) ;
      OSS_INLINE LZW_CODE findStr( LZW_CODE preCode, UINT8 ch ) ;
      OSS_INLINE UINT32 getStr( LZW_CODE code, UINT8 *buff, UINT32 bufSize ) ;

   private:
      _utilLZWDictHead _head ;
      UINT32 _maxNodeNum ;
      _utilLZWNode *_nodes ;
   } ;
   typedef _utilLZWDictionary utilLZWDictionary ;

   /*
    * For dictionary storage, both in memory and on disk. The above structure
    * _utilLZWDictionary as well as all the nodes will be stored together
    */
   #define MIN_DICT_SIZE \
      ( sizeof( _utilLZWDictionary ) + sizeof( _utilLZWNode) * MIN_NODE_NUM )

   class _utilLZWContext : public SDBObject
   {
      friend class _utilLZWDictCreator ;
      friend class _utilLZW ;
   public:
      _utilLZWContext()
      {
         reset( FALSE ) ;
      }

      ~_utilLZWContext() {}

      OSS_INLINE BOOLEAN isReady() { return _ready ; }

      OSS_INLINE void setDictionary( _utilLZWDictionary *dict )
      {
         SDB_ASSERT( dict, "Dictionary is invalid" ) ;
         _dictionary = dict ;
         _ready = TRUE ;
      }

      _utilLZWDictionary* getDictionary() { return _dictionary ; }

      OSS_INLINE void reset( BOOLEAN keepDict )
      {
         if ( !keepDict )
         {
            _dictionary = NULL ;
            _ready = FALSE ;
         }

         _maxDictNodeNum = 0 ;
         _stream = NULL ;
         _streamLen = 0 ;
         _streamPos = 0 ;
         _bitBuf._buf = 0 ;
         _bitBuf._n = 0 ;
      }

   private:
      BOOLEAN _ready ;
      utilLZWDictionary *_dictionary ;   /* dictionary pointer */
      UINT32 _maxDictNodeNum ;
      UINT8 *_stream ;
      UINT32 _streamLen ;
      UINT32 _streamPos ;
      utilBitBuffer _bitBuf ;   /* bit buffer used during encoding and decoding */
   } ;
   typedef _utilLZWContext utilLZWContext ;

   class _utilLZWDictCreator : public SDBObject
   {
   public:
      _utilLZWDictCreator()
         : _dictionary( NULL )
      {
         _ctx.reset( FALSE ) ;
      }

      ~_utilLZWDictCreator()
      {
         if ( _dictionary )
         {
            _dictionary->reset() ;
            SDB_OSS_FREE( _dictionary ) ;
         }
      }

      /* The following member functions are used to handle the dictionary. */
      /* Prepare for dictionary building, setting the maximum allowed size. */
      INT32 prepare( UINT32 maxSize ) ;

      void reset() ;

      /*
       * Use the source data to build the dictionary for LZW. It can be called
       * multiple times, and the dictionary will grow until reaching the maximum
       * size.
       * This function should be called after buildDictPrepare.
       */
      INT32 build( const CHAR *src, UINT32 srcLen, BOOLEAN &full ) ;

      /* Save the dictionary to the provided memory space. */
      INT32 save( CHAR *dictBuf, UINT32 &maxDictLen ) ;

      _utilLZWDictionary* getDictionary() { return _dictionary ; }

   private:
      _utilLZWDictionary *_dictionary ;
      _utilLZWContext _ctx ;
   } ;
   typedef _utilLZWDictCreator utilLZWDictCreator ;

   class _utilLZW : public SDBObject
   {
   public:
      OSS_INLINE INT32 encode( _utilLZWContext *ctx,
                               const CHAR *source, UINT32 sourceLen,
                               CHAR *destBuf, UINT32 &destLen ) ;

      OSS_INLINE INT32 decode( _utilLZWContext *ctx,
                               const CHAR *source, UINT32 sourceLen,
                               CHAR *destBuf, UINT32 &destLen );
   private:
      OSS_INLINE LZW_CODE _readCode( _utilLZWContext *ctx ) ;
      OSS_INLINE void _writeCode( _utilLZWContext *ctx, LZW_CODE code ) ;
      //OSS_INLINE UINT32 _readBits( _utilLZWContext *ctx ) ;
      OSS_INLINE void _writeBits( _utilLZWContext *ctx, UINT32 bits,
                                  UINT32 codeSize ) ;
      OSS_INLINE UINT8 _readByte( _utilLZWContext *ctx ) ;
      OSS_INLINE void _writeByte( _utilLZWContext *ctx, UINT8 ch ) ;
      //OSS_INLINE void _readBuf() ;
      //OSS_INLINE void _writeBuf() ;

      OSS_INLINE void _flushBits( _utilLZWContext *ctx ) ;
   } ;
   typedef _utilLZW utilLZW ;

   /* Add new string to the LZW dictionary. */
   OSS_INLINE LZW_CODE _utilLZWDictionary::addStr( LZW_CODE preCode, UINT8 ch )
   {
      LZW_CODE currCode ;
      LZW_CODE nextCode ;

      if ( _head._maxCode + 1 == _maxNodeNum )
      {
         /* Dictionary is full. */
         return DICT_INVALID_NODE ;
      }

      _head._maxCode++ ;
      /*
       * The current code size is not enough to represent the code, so increase
       * it by 1 bit.
       */
      if ( _head._maxCode == (UINT32)( 1 << _head._codeSize ) )
      {
         _head._codeSize++ ;
      }

      _nodes[_head._maxCode]._prev = preCode ;
      _nodes[_head._maxCode]._first = DICT_INVALID_NODE ;

      if ( DICT_INVALID_NODE == _nodes[preCode]._first )
      {
         _nodes[preCode]._first = _head._maxCode ;
      }
      else
      {
         currCode = nextCode = _nodes[preCode]._first ;
         while ( ( DICT_INVALID_NODE != nextCode )
                 && ( ch < _nodes[nextCode]._ch ) )
         {
            currCode = nextCode ;
            nextCode = _nodes[currCode]._next ;
         }

         if ( currCode == _nodes[preCode]._first )
         {
            /* currCode not moved, then add the new node to the head. */
            _nodes[_head._maxCode]._next = _nodes[preCode]._first ;
            _nodes[preCode]._first = _head._maxCode;
         }
         else
         {
            /* Add the new node at the middle or end of the chain. */
            _nodes[_head._maxCode]._next = _nodes[currCode]._next ;
            _nodes[currCode]._next = _head._maxCode ;
         }
      }

      _nodes[_head._maxCode]._ch = ch ;
      _nodes[_head._maxCode]._len = _nodes[preCode]._len + 1 ;

      return _head._maxCode ;
   }

   OSS_INLINE LZW_CODE _utilLZWDictionary::findStr( LZW_CODE preCode, UINT8 ch )
   {
      LZW_CODE nextCode ;

      for ( nextCode = _nodes[preCode]._first; nextCode != DICT_INVALID_NODE;
            nextCode = _nodes[nextCode]._next )
      {
         if ( ch < _nodes[nextCode]._ch )
         {
            continue ;
         }

         if ( preCode == _nodes[nextCode]._prev && ch == _nodes[nextCode]._ch )
         {
            return nextCode ;
         }
         else
         {
            break ;
         }
      }

      return DICT_INVALID_NODE ;
   }

   OSS_INLINE UINT32 _utilLZWDictionary::getStr( LZW_CODE code, UINT8 *buff,
                                                 UINT32 bufSize )
   {
      UINT32 strLen = _nodes[code]._len ;
      UINT32 i = strLen ;
      SDB_ASSERT( bufSize >= strLen, "Invalid argument, bufSize too small" ) ;

      while ( code != DICT_INVALID_NODE && i )
      {
         buff[--i] = _nodes[code]._ch ;
         code = _nodes[code]._prev ;
      }

      return strLen ;
   }

   OSS_INLINE LZW_CODE _utilLZW::_readCode( _utilLZWContext *ctx )
   {
      UINT32 bits = 0 ;
      UINT32 codeSize = ctx->getDictionary()->getCodeSize() ;

      while ( ctx->_bitBuf._n < codeSize )
      {
         UINT8 ch = _readByte( ctx ) ;
         ctx->_bitBuf._buf = ( ctx->_bitBuf._buf << 8 ) | ch ;
         ctx->_bitBuf._n += 8 ;
      }

      ctx->_bitBuf._n -= codeSize ;
      bits = ( ctx->_bitBuf._buf >> ctx->_bitBuf._n )
             & (( 1 << codeSize ) - 1 ) ;

      return bits ;
   }

   OSS_INLINE void _utilLZW::_writeCode( _utilLZWContext *ctx, LZW_CODE code )
   {
      _writeBits( ctx, code, ctx->getDictionary()->getCodeSize() ) ;
   }

   OSS_INLINE void _utilLZW::_writeBits( _utilLZWContext *ctx, UINT32 bits,
                                         UINT32 bitNum)
   {
      ctx->_bitBuf._buf = ( ctx->_bitBuf._buf << bitNum )
                          | ( bits & (( 1 << bitNum ) - 1 ) ) ;

      bitNum += ctx->_bitBuf._n ;

      while ( bitNum >= 8 )
      {
         UINT8 ch ;
         bitNum -= 8 ;
         ch = ctx->_bitBuf._buf >> bitNum ;
         _writeByte( ctx, ch ) ;
      }

      ctx->_bitBuf._n = bitNum ;
   }


   OSS_INLINE UINT8 _utilLZW::_readByte( _utilLZWContext *ctx )
   {
      return ctx->_stream[ctx->_streamPos++] ;
   }

   OSS_INLINE void _utilLZW::_writeByte( _utilLZWContext *ctx, UINT8 ch )
   {
      ctx->_stream[ctx->_streamPos++] = ch ;
   }

   OSS_INLINE void _utilLZW::_flushBits( _utilLZWContext *ctx )
   {
      if ( ctx->_bitBuf._n )
      {
         _writeBits( ctx, 0, 8 - ctx->_bitBuf._n ) ;
      }
   }

   OSS_INLINE INT32 _utilLZW::encode( _utilLZWContext *ctx,
                                      const CHAR *source, UINT32 sourceLen,
                                      CHAR *destBuf, UINT32 &destLen )
   {
      INT32 rc = SDB_OK ;
      UINT8 ch = 0 ;
      UINT32 pos = 0 ;
      UINT32 strLen = 0 ;
      LZW_CODE code = DICT_INVALID_NODE ;
      LZW_CODE nextCode = DICT_INVALID_NODE ;
      utilLZWDictionary *dictionary = ctx->getDictionary() ;
      SDB_ASSERT( dictionary, "Compressor context is invalid" ) ;

      ctx->_stream = (UINT8* )destBuf ;
      ctx->_streamLen = destLen ;

      ch = source[0] ;
      code = ch ;
      pos++ ;
      strLen++ ;

      for ( ; pos < sourceLen; ++pos )
      {
         ch = source[pos] ;
         nextCode = dictionary->findStr( code, ch ) ;
         /*
          * If <code> + ch can not be found in the dictionary, write <code> to
          * the output stream. Otherwise make <code> = <code> + ch.
          */
         if ( DICT_INVALID_NODE == nextCode )
         {
            SDB_ASSERT( code <= dictionary->getMaxCode(), "invalid code" ) ;
            _writeCode( ctx, code ) ;
            code = ch ;
            strLen = 1 ;
         }
         else
         {
            code = nextCode ;
            strLen++ ;
         }
      }

      /* Write the last code */
      SDB_ASSERT( code <= dictionary->getMaxCode(), "invalid code" ) ;
      _writeCode( ctx, code ) ;
      _flushBits( ctx ) ;
      destLen = ctx->_streamPos;

      return rc ;
   }

   OSS_INLINE INT32 _utilLZW::decode( _utilLZWContext *ctx,
                                      const CHAR *source, UINT32 sourceLen,
                                      CHAR *destBuf, UINT32 &destLen )
   {
      INT32 rc = SDB_OK ;
      UINT32 strLen = 0 ;
      LZW_CODE code ;
      UINT32 totalOut = 0 ;
      utilLZWDictionary *dictionary = ctx->getDictionary() ;
      ctx->_stream = (UINT8 *)source ;
      ctx->_streamLen = sourceLen ;
      ctx->_streamPos = 0 ;

      for( ; ctx->_streamPos < ctx->_streamLen; )
      {
         code = _readCode( ctx ) ;
         /*
          * Generally this should not happen. If it does, there is something
          * wrong in the data to be decompressed.
          */
         SDB_ASSERT( code <= dictionary->getMaxCode(), "invalid code in data" ) ;
         if ( code > dictionary->getMaxCode() )
         {
            PD_LOG( PDERROR, "Invalid code found: code = %d, maxCode = %d",
                    code, dictionary->getMaxCode() ) ;
            rc = SDB_CORRUPTED_RECORD ;
            goto error ;
         }
         strLen = dictionary->getStr( code, (UINT8*)(destBuf + totalOut),
                                      destLen - totalOut) ;
         totalOut += strLen ;
      }

      destLen = totalOut ;

   done:
      return rc ;
   error:
      goto done ;
   }
}

#endif /* UTIL_LZW__ */

