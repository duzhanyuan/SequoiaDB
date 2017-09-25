#ifndef UTIL_COMPRESSOR_LZW__
#define UTIL_COMPRESSOR_LZW__

#include "ossLatch.hpp"
#include "utilCompressor.hpp"
#include "utilLZW.hpp"

namespace engine
{
   class _utilCompressorLZW : public utilCompressor
   {
      public:
         _utilCompressorLZW();
         ~_utilCompressorLZW();
      public:
         INT32 setDictionary( const CHAR* dict, UINT32 dictLen ) ;

         size_t compressBound( size_t srcLen ) ;

         INT32 prepare( utilCompressorContext &ctx ) ;

         INT32 rePrepare( utilCompressorContext &ctx ) ;

         INT32 compress( utilCompressorContext ctx,
                         const CHAR* source, UINT32 sourceLen,
                         CHAR* dest, UINT32 &destLen ) ;

         INT32 decompress( utilCompressorContext ctx,
                           const CHAR* source, UINT32 sourceLen,
                           CHAR* dest, UINT32 &destLen ) ;

         INT32 done( utilCompressorContext &ctx ) ;

      private:
         void _freeDictionary() ;

      private:
         _utilLZW _lzw ;
         _utilLZWDictionary *_dictionary ;
         std::vector<_utilLZWContext *> _vecContext ;
         ossSpinXLatch _vecCtxLatch ;
   };
   typedef _utilCompressorLZW utilCompressorLZW ;
}

#endif /* UTIL_COMPRESSOR_LZW__ */


