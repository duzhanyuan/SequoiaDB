// This header file is to define the compressor type and interfaces.

#ifndef UTIL_COMPRESSOR__
#define UTIL_COMPRESSOR__

#include "core.hpp"
#include "oss.hpp"
#include "ossUtil.hpp"

namespace engine
{
   typedef void * utilCompressorContext ;
   #define UTIL_INVALID_COMP_CTX    NULL

    // compressor type definition. Currently only support for LZW is provided.
   enum UTIL_COMPRESSOR_TYPE
   {
      UTIL_COMPRESSOR_INVALID = -1,
      UTIL_COMPRESSOR_SNAPPY = 0,
      UTIL_COMPRESSOR_LZW = 1,
      UTIL_COMPRESSOR_LZ4 = 2,
      UTIL_COMPRESSOR_ZLIB = 3,
   } ;

   const CHAR *utilCompressType2String( UINT8 type ) ;

   /* This class provides compressor interfaces. */
   class _utilCompressor : public SDBObject
   {
      public:
         _utilCompressor(UTIL_COMPRESSOR_TYPE type)
            : _type(type), _dictCopy( FALSE ),
              _dictionary( NULL ), _dictSize( 0 ),
              _prepared( FALSE )
         {
         }
         virtual ~_utilCompressor() {};
      public:
         /*
          * Set the dictionary used by the compressor and decompressor.
          * They should use exactly the same dictionary.
          * If the compressor is expected to maintain the dictionary by itself,
          * pass 'copy' as TRUE.
          */
         virtual INT32 setDictionary( const CHAR *dict, UINT32 dictLen ) = 0 ;

         /*
         * Get the possible compressed size in the worst case. srcLen is the
         * source data size.
         */
         virtual size_t compressBound( size_t srcLen ) = 0 ;

         virtual INT32 prepare( utilCompressorContext &context ) = 0 ;
         virtual BOOLEAN canRePrepare() { return _prepared ; }

         /*
          * Only called after prepare and before done. It will reuse the context
          * to start another work.
          */
         virtual INT32 rePrepare( utilCompressorContext &context ) = 0 ;

         /*
         * Compress the source data, and write the compressed data into dest.
         * On success, destSize will be the actual dest data size.
         * RETURN:
         *     SDB_OK
         *        Compress successfully. dest and destSize contain the
         *        compressed data and size seperately.
         *     SDB_UTIL_BUF_FULL
         *        Compression failed due to too small dest buf.
         *     SDB_UTIL_PREPARE_COMPRESSOR_FAIL
         *        Compression failed due to some error during preparing
         *        compression.
         *     SDB_UTIL_COMPRESS_FAIL
         *        Compression failed due to some error during the actual
         *        compressing phase.
         */
         virtual INT32 compress( utilCompressorContext ctx,
                                 const CHAR* source, UINT32 sourceLen,
                                 CHAR* dest, UINT32 &destLen ) = 0 ;
         virtual INT32 decompress( utilCompressorContext ctx,
                                   const CHAR* source, UINT32 sourceLen,
                                   CHAR* dest, UINT32 &destLen ) = 0 ;
         virtual INT32 done( utilCompressorContext &ctx ) = 0;
         UTIL_COMPRESSOR_TYPE getType(void) { return _type; }

      protected:
         UTIL_COMPRESSOR_TYPE _type ;
         BOOLEAN _dictCopy ;
         CHAR *_dictionary ;
         UINT32 _dictSize ;
         BOOLEAN _prepared ;
   };
   typedef _utilCompressor utilCompressor;
}

#endif /* UTIL_COMPRESSOR__ */

