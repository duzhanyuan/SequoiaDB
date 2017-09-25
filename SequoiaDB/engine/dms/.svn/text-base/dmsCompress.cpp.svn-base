/*******************************************************************************


   Copyright (C) 2011-2014 SequoiaDB Ltd.

   This program is free software: you can redistribute it and/or modify
   it under the term of the GNU Affero General Public License, version 3,
   as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warrenty of
   MARCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program. If not, see <http://www.gnu.org/license/>.

   Source File Name = dmsCompress.cpp

   Descriptive Name =

   When/how to use: str util

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          20/06/2014  XJH Initial Draft

   Last Changed =

******************************************************************************/

#include "dmsCompress.hpp"
#include "pmdEDU.hpp"
#include "dmsRecord.hpp"
#include <../snappy/snappy.h>

using namespace bson ;

namespace engine
{

   INT32 dmsCompress ( _pmdEDUCB *cb, utilCompressor *compressor,
                       utilCompressorContext compContext,
                       const CHAR *pInputData, INT32 inputSize,
                       const CHAR **ppData, INT32 *pDataSize )
   {
      INT32 rc = SDB_OK ;
      CHAR *pBuff = NULL ;
      size_t maxCompressedLen = 0 ;

      SDB_ASSERT ( pInputData && ppData && pDataSize,
                   "Data pointer and size pointer can't be NULL" ) ;

      /*
       * If compressor is not NULL, it's using zlib or lz4. Otherwise, compress
       * using snappy.
       */
      if ( compressor )
      {
         /*
          * From compressor zlib and lz4, 4 bytes which specifies the original
          * data length are stored before the actual compressed data, as the
          * compression algorithms don't provide any way to estimate the
          * uncompressed data length.
          */
         maxCompressedLen = compressor->compressBound( inputSize )
                            + sizeof( UINT32 ) ;
      }
      else
      {
         maxCompressedLen = snappy::MaxCompressedLength ( inputSize ) ;
      }

      pBuff = cb->getCompressBuff( maxCompressedLen ) ;
      if ( !pBuff )
      {
         PD_LOG( PDERROR, "Failed to alloc compress buff, size: %d",
                 maxCompressedLen ) ;
         rc = SDB_OOM ;
         goto error ;
      }

      if ( NULL == compressor )
      {
         // let's rock :)
         snappy::RawCompress ( pInputData, (size_t)inputSize,
                               pBuff, &maxCompressedLen ) ;
      }
      else
      {
         UINT32 actualDataBufLen = maxCompressedLen - sizeof( UINT32 ) ;
         rc = compressor->compress( compContext,  pInputData, inputSize,
                                    pBuff + sizeof( UINT32 ),
                                    actualDataBufLen ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Failed to compress record, the data will stay in "
                    "not compressed format, rc: %d", rc ) ;
            goto error ;
         }

         //If compressed successfully, write the original length in the output.
         maxCompressedLen = actualDataBufLen + sizeof( UINT32 ) ;
         *(UINT32 *)pBuff = inputSize ;
      }

      // assign the output buffer pointer
      if ( ppData )
      {
         *ppData = pBuff ;
      }
      if ( pDataSize )
      {
         *pDataSize = (INT32)maxCompressedLen ;
      }

   done :
      return rc ;
   error :
      goto done ;
   }

   INT32 dmsCompress ( _pmdEDUCB *cb, utilCompressor *compressor,
                       utilCompressorContext compContext,
                       const BSONObj &obj,
                       const CHAR* pOIDPtr, INT32 oidLen,
                       const CHAR **ppData, INT32 *pDataSize )
   {
      INT32 rc = SDB_OK ;
      CHAR *pTmpBuff = NULL ;

      // if we want to append OID, then
      if ( oidLen && pOIDPtr )
      {
         INT32 tmpBuffLen = 0 ;
         const CHAR *pObjData = NULL ;

         // get the requested size by adding object size and oid size
         INT32 requestedSize = obj.objsize() + oidLen + DMS_RECORD_METADATA_SZ ;
         rc = cb->allocBuff( requestedSize, &pTmpBuff, tmpBuffLen ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Failed to alloc tmp buffer, size: %d",
                    requestedSize ) ;
            goto error ;
         }
         pObjData = pTmpBuff + DMS_RECORD_METADATA_SZ ;

         DMS_RECORD_SETDATA_OID ( pTmpBuff, obj.objdata(), obj.objsize(),
                                  BSONElement(pOIDPtr) ) ;

         rc = dmsCompress ( cb, compressor, compContext, pObjData,
                            BSONObj(pObjData).objsize(), ppData, pDataSize ) ;
      }
      else
      {
         rc = dmsCompress( cb, compressor, compContext, obj.objdata(),
                           obj.objsize(), ppData, pDataSize ) ;
      }
      if ( rc )
      {
         goto error ;
      }

   done :
      if ( pTmpBuff )
      {
         cb->releaseBuff( pTmpBuff ) ;
      }
      return rc ;
   error :
      goto done ;
   }

   INT32 dmsUncompress ( _pmdEDUCB *cb, utilCompressor *compressor,
                         utilCompressorContext compContext,
                         const CHAR *pInputData, INT32 inputSize,
                         const CHAR **ppData, INT32 *pDataSize )
   {
      INT32 rc = SDB_OK ;
      bool  result = FALSE ;
      CHAR *pBuff = NULL ;

      SDB_ASSERT ( pInputData && ppData && pDataSize,
                   "Data pointer and size pointer can't be NULL" ) ;

      size_t maxUncompressedLen = 0 ;
      UINT32 destSize = 0 ;
      if ( compressor )
      {
         maxUncompressedLen = *(UINT32 *)pInputData ;
      }
      else
      {
         // estimate the max possible size for uncompressed data + sanity check
         result = snappy::GetUncompressedLength ( pInputData, (size_t)inputSize,
                                                  &maxUncompressedLen ) ;
         if ( !result )
         {
            PD_LOG( PDERROR, "Failed to get uncompressed length" ) ;
            rc = SDB_CORRUPTED_RECORD ;
            goto error ;
         }
      }

      pBuff = cb->getUncompressBuff( maxUncompressedLen ) ;
      if ( !pBuff )
      {
         PD_LOG( PDERROR, "Failed to allocate uncompression buff, size: %d",
                 maxUncompressedLen ) ;
         rc = SDB_OOM ;
         goto error ;
      }

      destSize = maxUncompressedLen ;
      if ( compressor )
      {
         /*
          * First 4 bytes of the input data is the original uncompressed data
          * length.
          */
         rc = compressor->decompress( compContext,
                                      pInputData + sizeof( UINT32 ),
                                      inputSize - sizeof( UINT32 ),
                                      pBuff, destSize ) ;
         result = ( SDB_OK == rc ) ? TRUE : FALSE ;
      }
      else
      {
         // let's rock :)
         result = snappy::RawUncompress ( pInputData, (size_t)inputSize,
                                          pBuff ) ;
      }
      if ( !result )
      {
         PD_LOG( PDERROR, "Failed to uncompress record" ) ;
         rc = SDB_CORRUPTED_RECORD ;
         goto error ;
      }

      // assign return value
      if ( ppData )
      {
         *ppData = pBuff ;
      }
      if ( pDataSize )
      {
         *pDataSize = destSize ;
      }

   done :
      return rc ;
   error :
      goto done ;
   }

}


