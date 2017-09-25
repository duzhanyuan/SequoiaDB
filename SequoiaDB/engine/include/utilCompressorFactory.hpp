/* This header file is for compressors.
   It provides the interface for the supported compressors.
 */

#ifndef UTIL_COMPRESSOR_FACTORY__
#define UTIL_COMPRESSOR_FACTORY__

#include "utilCompressor.hpp"

namespace engine
{
   class _utilCompressorFactory : public SDBObject
   {
   public:
      _utilCompressorFactory() {};
      ~_utilCompressorFactory() {};
   public:
      INT32 createCompressor( UTIL_COMPRESSOR_TYPE type,
                              utilCompressor *&compressor ) ;
      void destroyCompressor( utilCompressor* compressor ) ;
   };
   typedef _utilCompressorFactory utilCompressorFactory;
}

#endif /* UTIL_COMPRESSOR_FACTORY__ */

