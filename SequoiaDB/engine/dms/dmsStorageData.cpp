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

   Source File Name = dmsStorageData.cpp

   Descriptive Name = Data Management Service Storage Unit Header

   When/how to use: this program may be used on binary and text-formatted
   versions of data management component. This file contains structure for
   DMS storage unit and its methods.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          14/08/2013  XJH Initial Draft

   Last Changed =

*******************************************************************************/

#include "dmsStorageData.hpp"
#include "dmsStorageIndex.hpp"
#include "dmsStorageLob.hpp"
#include "pmd.hpp"
#include "dpsTransCB.hpp"
#include "dpsOp2Record.hpp"
#include "mthModifier.hpp"
#include "dmsCompress.hpp"
#include "ixm.hpp"
#include "pdTrace.hpp"
#include "dmsTrace.hpp"
#include "utilCompressorFactory.hpp"
#include "dmsCB.hpp"
#include "pd.hpp"

using namespace bson ;

namespace engine
{

   #define DMS_MB_FLAG_FREE_STR                       "Free"
   #define DMS_MB_FLAG_USED_STR                       "Used"
   #define DMS_MB_FLAG_DROPED_STR                     "Dropped"
   #define DMS_MB_FLAG_OFFLINE_REORG_STR              "Offline Reorg"
   #define DMS_MB_FLAG_ONLINE_REORG_STR               "Online Reorg"
   #define DMS_MB_FLAG_LOAD_STR                       "Load"
   #define DMS_MB_FLAG_OFFLINE_REORG_SHADOW_COPY_STR  "Shadow Copy"
   #define DMS_MB_FLAG_OFFLINE_REORG_TRUNCATE_STR     "Truncate"
   #define DMS_MB_FLAG_OFFLINE_REORG_COPY_BACK_STR    "Copy Back"
   #define DMS_MB_FLAG_OFFLINE_REORG_REBUILD_STR      "Rebuild"
   #define DMS_MB_FLAG_LOAD_LOAD_STR                  "Load"
   #define DMS_MB_FLAG_LOAD_BUILD_STR                 "Build"
   #define DMS_MB_FLAG_UNKNOWN                        "Unknown"

   #define DMS_STATUS_SEPARATOR                       " | "

   static void appendFlagString( CHAR * pBuffer, INT32 bufSize,
                                 const CHAR *flagStr )
   {
      if ( 0 != *pBuffer )
      {
         ossStrncat( pBuffer, DMS_STATUS_SEPARATOR,
                     bufSize - ossStrlen( pBuffer ) ) ;
      }
      ossStrncat( pBuffer, flagStr, bufSize - ossStrlen( pBuffer ) ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__MBFLAG2STRING, "mbFlag2String" )
   void mbFlag2String( UINT16 flag, CHAR * pBuffer, INT32 bufSize )
   {
      PD_TRACE_ENTRY ( SDB__MBFLAG2STRING ) ;
      SDB_ASSERT ( pBuffer, "pBuffer can't be NULL" ) ;
      ossMemset ( pBuffer, 0, bufSize ) ;
      // Free
      if ( DMS_IS_MB_FREE ( flag ) )
      {
         appendFlagString( pBuffer, bufSize, DMS_MB_FLAG_FREE_STR ) ;
         goto done ;
      }

      // Used
      if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_USED ) )
      {
         appendFlagString( pBuffer, bufSize, DMS_MB_FLAG_USED_STR ) ;
         OSS_BIT_CLEAR( flag, DMS_MB_FLAG_USED ) ;
      }
      // Dropped
      if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_DROPED ) )
      {
         appendFlagString( pBuffer, bufSize, DMS_MB_FLAG_DROPED_STR ) ;
         OSS_BIT_CLEAR( flag, DMS_MB_FLAG_DROPED ) ;
      }

      // Offline Reorg
      if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_OFFLINE_REORG ) )
      {
         appendFlagString( pBuffer, bufSize, DMS_MB_FLAG_OFFLINE_REORG_STR ) ;
         OSS_BIT_CLEAR( flag, DMS_MB_FLAG_OFFLINE_REORG ) ;

         // Shadow Copy
         if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_OFFLINE_REORG_SHADOW_COPY ) )
         {
            appendFlagString( pBuffer, bufSize,
                              DMS_MB_FLAG_OFFLINE_REORG_SHADOW_COPY_STR ) ;
            OSS_BIT_CLEAR( flag, DMS_MB_FLAG_OFFLINE_REORG_SHADOW_COPY ) ;
         }
         // Truncate
         if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_OFFLINE_REORG_TRUNCATE ) )
         {
            appendFlagString( pBuffer, bufSize,
                              DMS_MB_FLAG_OFFLINE_REORG_TRUNCATE_STR ) ;
            OSS_BIT_CLEAR( flag, DMS_MB_FLAG_OFFLINE_REORG_TRUNCATE ) ;
         }
         // Copy Back
         if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_OFFLINE_REORG_COPY_BACK ) )
         {
            appendFlagString( pBuffer, bufSize,
                              DMS_MB_FLAG_OFFLINE_REORG_COPY_BACK_STR ) ;
            OSS_BIT_CLEAR( flag, DMS_MB_FLAG_OFFLINE_REORG_COPY_BACK ) ;
         }
         // Rebuild
         if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_OFFLINE_REORG_REBUILD ) )
         {
            appendFlagString( pBuffer, bufSize,
                              DMS_MB_FLAG_OFFLINE_REORG_REBUILD_STR ) ;
            OSS_BIT_CLEAR( flag, DMS_MB_FLAG_OFFLINE_REORG_REBUILD ) ;
         }
      }
      // Online Reorg
      if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_ONLINE_REORG ) )
      {
         appendFlagString( pBuffer, bufSize, DMS_MB_FLAG_ONLINE_REORG_STR ) ;
         OSS_BIT_CLEAR( flag, DMS_MB_FLAG_ONLINE_REORG ) ;
      }
      // load
      if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_LOAD ) )
      {
         appendFlagString( pBuffer, bufSize, DMS_MB_FLAG_LOAD_LOAD_STR ) ;
         OSS_BIT_CLEAR( flag, DMS_MB_FLAG_LOAD ) ;

         // load
         if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_LOAD_LOAD ) )
         {
            appendFlagString( pBuffer, bufSize, DMS_MB_FLAG_LOAD_LOAD_STR ) ;
            OSS_BIT_CLEAR( flag, DMS_MB_FLAG_LOAD_LOAD ) ;
         }
         // load build
         if ( OSS_BIT_TEST ( flag, DMS_MB_FLAG_LOAD_BUILD ) )
         {
            appendFlagString( pBuffer, bufSize, DMS_MB_FLAG_LOAD_BUILD_STR ) ;
            OSS_BIT_CLEAR( flag, DMS_MB_FLAG_LOAD_BUILD ) ;
         }
      }

      // Test other bits
      if ( flag )
      {
         appendFlagString( pBuffer, bufSize, DMS_MB_FLAG_UNKNOWN ) ;
      }
   done :
      PD_TRACE2 ( SDB__MBFLAG2STRING,
                  PD_PACK_USHORT ( flag ),
                  PD_PACK_STRING ( pBuffer ) ) ;
      PD_TRACE_EXIT ( SDB__MBFLAG2STRING ) ;
   }

   #define DMS_MB_ATTR_COMPRESSED_STR                        "Compressed"
   #define DMS_MB_ATTR_NOIDINDEX_STR                         "NoIDIndex"
   // PD_TRACE_DECLARE_FUNCTION ( SDB__MBATTR2STRING, "mbAttr2String" )
   void mbAttr2String( UINT32 attributes, CHAR * pBuffer, INT32 bufSize )
   {
      PD_TRACE_ENTRY ( SDB__MBATTR2STRING ) ;
      SDB_ASSERT ( pBuffer, "pBuffer can't be NULL" ) ;
      ossMemset ( pBuffer, 0, bufSize ) ;

      if ( OSS_BIT_TEST ( attributes, DMS_MB_ATTR_COMPRESSED ) )
      {
         appendFlagString( pBuffer, bufSize, DMS_MB_ATTR_COMPRESSED_STR ) ;
         OSS_BIT_CLEAR( attributes, DMS_MB_ATTR_COMPRESSED ) ;
      }
      if ( OSS_BIT_TEST ( attributes, DMS_MB_ATTR_NOIDINDEX ) )
      {
         appendFlagString( pBuffer, bufSize, DMS_MB_ATTR_NOIDINDEX_STR ) ;
         OSS_BIT_CLEAR( attributes, DMS_MB_ATTR_NOIDINDEX ) ;
      }

      // Test other bits
      if ( attributes )
      {
         appendFlagString( pBuffer, bufSize, DMS_MB_FLAG_UNKNOWN ) ;
      }
      PD_TRACE2 ( SDB__MBATTR2STRING,
                  PD_PACK_UINT ( attributes ),
                  PD_PACK_STRING ( pBuffer ) ) ;
      PD_TRACE_EXIT ( SDB__MBATTR2STRING ) ;
   }

   /*
      _dmsMBContext implement
   */
   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSMBCONTEXT, "_dmsMBContext::_dmsMBContext" )
   _dmsMBContext::_dmsMBContext ()
   {
      PD_TRACE_ENTRY ( SDB__DMSMBCONTEXT ) ;
      _reset () ;
      PD_TRACE_EXIT ( SDB__DMSMBCONTEXT ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSMBCONTEXT_DESC, "_dmsMBContext::~_dmsMBContext" )
   _dmsMBContext::~_dmsMBContext ()
   {
      PD_TRACE_ENTRY ( SDB__DMSMBCONTEXT_DESC ) ;
      _reset () ;
      PD_TRACE_EXIT ( SDB__DMSMBCONTEXT_DESC ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSMBCONTEXT__RESET, "_dmsMBContext::_reset" )
   void _dmsMBContext::_reset ()
   {
      PD_TRACE_ENTRY ( SDB__DMSMBCONTEXT__RESET ) ;
      _mb            = NULL ;
      _mbStat        = NULL ;
      _latch         = NULL ;
      _clLID         = DMS_INVALID_CLID ;
      _mbID          = DMS_INVALID_MBID ;
      _mbLockType    = -1 ;
      _resumeType    = -1 ;
      PD_TRACE_EXIT ( SDB__DMSMBCONTEXT__RESET ) ;
   }

   string _dmsMBContext::toString() const
   {
      stringstream ss ;
      ss << "dms-mb-context[" ;
      if ( _mb )
      {
         ss << "Name: " ;
         ss << _mb->_collectionName ;
         ss << ", " ;
      }
      ss << "ID: " << _mbID ;
      ss << ", LID: " << _clLID ;
      ss << ", LockType: " << _mbLockType ;
      ss << ", ResumeType: " << _resumeType ;

      ss << " ]" ;

      return ss.str() ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSMBCONTEXT_PAUSE, "_dmsMBContext::pause" )
   INT32 _dmsMBContext::pause()
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DMSMBCONTEXT_PAUSE ) ;
      if ( SHARED == _mbLockType || EXCLUSIVE == _mbLockType )
      {
         _resumeType = _mbLockType ;
         rc = mbUnlock() ;
      }
      PD_TRACE_EXITRC ( SDB__DMSMBCONTEXT_PAUSE, rc ) ;
      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSMBCONTEXT_RESUME, "_dmsMBContext::resume" )
   INT32 _dmsMBContext::resume()
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DMSMBCONTEXT_RESUME ) ;
      if ( SHARED == _resumeType || EXCLUSIVE == _resumeType )
      {
         INT32 lockType = _resumeType ;
         _resumeType = -1 ;
         rc = mbLock( lockType ) ;
      }
      PD_TRACE_EXITRC ( SDB__DMSMBCONTEXT_RESUME, rc ) ;
      return rc ;
   }

   void _dmsCompressorEntry::setCompressor( _utilCompressor *compressor )
   {
      SDB_ASSERT( compressor, "Invalid argument" ) ;
      _compressor = compressor ;
   }

   void _dmsCompressorEntry::reset()
   {
      if ( _compressor )
      {
         SDB_OSS_DEL _compressor ;
         _compressor = NULL ;
      }
   }

   /*
      _dmsStorageData implement
   */
   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA, "_dmsStorageData::_dmsStorageData" )
   _dmsStorageData::_dmsStorageData ( const CHAR *pSuFileName,
                                      dmsStorageInfo *pInfo )
   :_dmsStorageBase( pSuFileName, pInfo )
   {
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA ) ;
      _pIdxSU           = NULL ;
      _pLobSU           = NULL ;
      _logicalCSID      = 0 ;
      _CSID             = DMS_INVALID_SUID ;
      _mmeSegID         = 0 ;
      PD_TRACE_EXIT ( SDB__DMSSTORAGEDATA ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_DESC, "_dmsStorageData::~_dmsStorageData" )
   _dmsStorageData::~_dmsStorageData ()
   {
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_DESC ) ;
      _collectionNameMapCleanup() ;

      vector<dmsMBContext*>::iterator it = _vecContext.begin() ;
      while ( it != _vecContext.end() )
      {
         SDB_OSS_DEL (*it) ;
         ++it ;
      }
      _vecContext.clear() ;

      _pIdxSU = NULL ;
      _pLobSU = NULL ;

      PD_TRACE_EXIT ( SDB__DMSSTORAGEDATA_DESC ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_SYNCMEMTOMMAP, "_dmsStorageData::syncMemToMmap" )
   void _dmsStorageData::syncMemToMmap ()
   {
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_SYNCMEMTOMMAP ) ;
      // write total count to disk
      for ( UINT32 i = 0 ; i < DMS_MME_SLOTS ; ++i )
      {
         if ( DMS_IS_MB_INUSE ( _dmsMME->_mbList[i]._flag ) )
         {
            if ( _dmsMME->_mbList[i]._totalRecords !=
                 _mbStatInfo[i]._totalRecords )
            {
               _dmsMME->_mbList[i]._totalRecords =
                  _mbStatInfo[i]._totalRecords ;
            }
            if ( _dmsMME->_mbList[i]._totalDataPages !=
                 _mbStatInfo[i]._totalDataPages )
            {
               _dmsMME->_mbList[i]._totalDataPages =
                  _mbStatInfo[i]._totalDataPages ;
            }
            if ( _dmsMME->_mbList[i]._totalIndexPages !=
                 _mbStatInfo[i]._totalIndexPages )
            {
               _dmsMME->_mbList[i]._totalIndexPages =
                  _mbStatInfo[i]._totalIndexPages ;
            }
            if ( _dmsMME->_mbList[i]._totalDataFreeSpace !=
                 _mbStatInfo[i]._totalDataFreeSpace )
            {
               _dmsMME->_mbList[i]._totalDataFreeSpace =
                  _mbStatInfo[i]._totalDataFreeSpace ;
            }
            if ( _dmsMME->_mbList[i]._totalIndexFreeSpace !=
                 _mbStatInfo[i]._totalIndexFreeSpace )
            {
               _dmsMME->_mbList[i]._totalIndexFreeSpace =
                  _mbStatInfo[i]._totalIndexFreeSpace ;
            }
            if ( _dmsMME->_mbList[i]._totalLobPages !=
                 _mbStatInfo[i]._totalLobPages )
            {
               _dmsMME->_mbList[i]._totalLobPages =
                  _mbStatInfo[i]._totalLobPages ;
            }
            if ( _dmsMME->_mbList[i]._totalLobs !=
                 _mbStatInfo[i]._totalLobs )
            {
               _dmsMME->_mbList[i]._totalLobs =
                  _mbStatInfo[i]._totalLobs ;
            }
         }
      }
      PD_TRACE_EXIT ( SDB__DMSSTORAGEDATA_SYNCMEMTOMMAP ) ;
   }

   INT32 _dmsStorageData::flushMME( BOOLEAN sync )
   {
      syncMemToMmap() ;
      return flushSegment( _mmeSegID, sync ) ;
   }

   void _dmsStorageData::_attach( _dmsStorageIndex * pIndexSu )
   {
      SDB_ASSERT( pIndexSu, "Index su can't be NULL" ) ;
      _pIdxSU = pIndexSu ;
   }

   void _dmsStorageData::_detach ()
   {
      _pIdxSU = NULL ;
   }

   void _dmsStorageData::_attachLob( _dmsStorageLob * pLobSu )
   {
      SDB_ASSERT( pLobSu, "Lob su can't be NULL" ) ;
      _pLobSU = pLobSu ;
   }

   void _dmsStorageData::_detachLob()
   {
      _pLobSU = NULL ;
   }

   UINT64 _dmsStorageData::_dataOffset ()
   {
      return ( DMS_MME_OFFSET + DMS_MME_SZ ) ;
   }

   const CHAR* _dmsStorageData::_getEyeCatcher () const
   {
      return DMS_DATASU_EYECATCHER ;
   }

   UINT32 _dmsStorageData::_curVersion () const
   {
      return DMS_DATASU_CUR_VERSION ;
   }

   INT32 _dmsStorageData::_checkVersion( dmsStorageUnitHeader * pHeader )
   {
      INT32 rc = SDB_OK ;

      if ( pHeader->_version > _curVersion() )
      {
         PD_LOG( PDERROR, "Incompatible version: %u", pHeader->_version ) ;
         rc = SDB_DMS_INCOMPATIBLE_VERSION ;
      }
      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__ONCREATE, "_dmsStorageData::_onCreate" )
   INT32 _dmsStorageData::_onCreate( OSSFILE * file, UINT64 curOffSet )
   {
      INT32 rc          = SDB_OK ;
      _dmsMME           = NULL ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__ONCREATE ) ;
      SDB_ASSERT( DMS_MME_OFFSET == curOffSet, "Offset is not MME offset" ) ;

      _dmsMME = SDB_OSS_NEW dmsMetadataManagementExtent ;
      if ( !_dmsMME )
      {
         PD_LOG ( PDSEVERE, "Failed to allocate memory to for dmsMME" ) ;
         PD_LOG ( PDSEVERE, "Requested memory: %d bytes", DMS_MME_SZ ) ;
         rc = SDB_OOM ;
         goto error ;
      }
      _initializeMME () ;

      rc = _writeFile ( file, (CHAR *)_dmsMME, DMS_MME_SZ ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to write to file duirng SU init, rc: %d",
                  rc ) ;
         goto error ;
      }
      SDB_OSS_DEL _dmsMME ;
      _dmsMME = NULL ;

   done:
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__ONCREATE, rc ) ;
      return rc ;
   error:
      if ( _dmsMME )
      {
         SDB_OSS_DEL _dmsMME ;
         _dmsMME = NULL ;
      }
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__LOADCLDICTTOCACHE, "_dmsStorageData::_loadClDictToCache" )
   INT32 _dmsStorageData::_loadClDictToCache( const dmsMBContext *mbContext )
   {
      INT32 rc = SDB_OK ;
      dmsExtentID dictExtID = DMS_INVALID_EXTENT ;
      dmsDictExtent *dictExtent = NULL ;
      UINT16 mbID = mbContext->mbID() ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__LOADCLDICTTOCACHE ) ;

      dictExtID = _mbStatInfo[mbID]._dictExtID ;
      SDB_ASSERT( ( DMS_INVALID_EXTENT != dictExtID ),
                  "Dictionary extent id in mb is invalid" ) ;

      /* Load the dictionary from extent to dictionary context in cache. */
      dictExtent = (dmsDictExtent *)extentAddr( dictExtID ) ;
      if ( !dictExtent->validate( mbID ) )
      {
         PD_LOG( PDERROR, "Dictionary extent is invalid. Extent id: %d",
                 dictExtID ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      rc = prepareCompressor( mbContext,
                              (CHAR *)dictExtent + sizeof( dmsDictExtent ),
                              dictExtent->_dictLen ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to prepare compressor, rc: %d", rc ) ;

   done:
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__LOADCLDICTTOCACHE, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__ONMAPMETA, "_dmsStorageData::_onMapMeta" )
   INT32 _dmsStorageData::_onMapMeta( UINT64 curOffSet )
   {
      INT32 rc = SDB_OK ;
      BOOLEAN upgradeDictInfo = _dmsHeader->_version < DMS_COMPRESSION_ENABLE_VER ?
                                TRUE : FALSE ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__ONMAPMETA ) ;
      // MME, 4MB
      _mmeSegID = ossMmapFile::segmentSize() ;
      rc = map ( DMS_MME_OFFSET, DMS_MME_SZ, (void**)&_dmsMME ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to map MME: %s", getSuFileName() ) ;
         goto error ;
      }

      // load collection names in the SU
      for ( UINT16 i = 0 ; i < DMS_MME_SLOTS ; i++ )
      {
         if ( DMS_IS_MB_INUSE ( _dmsMME->_mbList[i]._flag ) )
         {
            _collectionNameInsert ( _dmsMME->_mbList[i]._collectionName, i ) ;

            _mbStatInfo[i]._totalRecords = _dmsMME->_mbList[i]._totalRecords ;
            _mbStatInfo[i]._totalDataPages =
               _dmsMME->_mbList[i]._totalDataPages ;
            _mbStatInfo[i]._totalIndexPages =
               _dmsMME->_mbList[i]._totalIndexPages ;
            _mbStatInfo[i]._totalDataFreeSpace =
               _dmsMME->_mbList[i]._totalDataFreeSpace ;
            _mbStatInfo[i]._totalIndexFreeSpace =
               _dmsMME->_mbList[i]._totalIndexFreeSpace ;
            _mbStatInfo[i]._totalLobPages =
               _dmsMME->_mbList[i]._totalLobPages ;
            _mbStatInfo[i]._totalLobs =
               _dmsMME->_mbList[i]._totalLobs ;
            /*
             * The following branch is for using newer program(SequoiaDB 2.0 or
             * later) with data of elder versions(Before 2.0). As dictionary
             * compression is enabled in 2.0, the data needs to upgrade,
             * because dictionary information such as dictionary extent id is
             * added in MB.
             * As for now, we do not change the version number in dms header, so
             * the second part of the following if statement condition is
             * needed, to avoid changing the dictionary related ids every time.
             * That would be done only the first time after upgrading.
             */
            if ( upgradeDictInfo && ( 0 == _dmsMME->_mbList[i]._dictExtentID ) )
            {
               _dmsMME->_mbList[i]._dictExtentID = DMS_INVALID_EXTENT ;
               _dmsMME->_mbList[i]._newDictExtentID = DMS_INVALID_EXTENT ;
            }

            _mbStatInfo[i]._dictExtID = _dmsMME->_mbList[i]._dictExtentID ;

            /*
             * In version before 2.0, the byte _compressorType is taking now was
             * set to 0. But in the new version, 0 means using snappy to
             * compress. So during the upgrading, set the _comrpessorType to -1
             * ( UTIL_COMPRESSOR_INVALID).
             */
            if ( upgradeDictInfo
                 && !OSS_BIT_TEST( _dmsMME->_mbList[i]._attributes,
                                   DMS_MB_ATTR_COMPRESSED )
                 && ( -1 != _dmsMME->_mbList[i]._compressorType ) )
            {
               _dmsMME->_mbList[i]._compressorType = -1 ;
            }

            _mbStatInfo[i]._compressorType =
               _dmsMME->_mbList[i]._compressorType ;
         }
      }

   done:
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__ONMAPMETA, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__ONOPENED, "_dmsStorageData::_onOpened" )
   INT32 _dmsStorageData::_onOpened()
   {
      INT32 rc = SDB_OK ;
      UINT32 clLID = DMS_INVALID_CLID ;
      dmsMBContext *context = NULL ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__ONOPENED ) ;
      /*
       * Check if the dictionary extent ID is valid. If yes, it means the
       * dictionary has been created. Then load it from file into the dictionary
       * cache.
       */
      for ( UINT16 i = 0; i < DMS_MME_SLOTS; ++i )
      {
         if ( UTIL_COMPRESSOR_LZW == _mbStatInfo[i]._compressorType )
         {
            /*
             * If _dictExtID is not invalid, the dictionary has been created.
             * So just load it from file to cache. Otherwise, pass it to the
             * dictionary creating job.
             */
            if ( DMS_INVALID_EXTENT != _mbStatInfo[i]._dictExtID )
            {
               rc = getMBContext( &context, i, clLID ) ;
               SDB_ASSERT( SDB_OK == rc, "mb status is invalid" ) ;
               rc = _loadClDictToCache( context ) ;
               releaseMBContext( context ) ;
               PD_RC_CHECK( rc, PDERROR, "Failed to load dictionary from file "
                         "to cache for collection. Collection name: %s",
                         _dmsMME->_mbList[i]._collectionName ) ;
            }
         }
      }

   done:
      PD_TRACE_EXITRC( SDB__DMSSTORAGEDATA__ONOPENED, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__ONCLOSED, "_dmsStorageData::_onClosed" )
   void _dmsStorageData::_onClosed ()
   {
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__ONCLOSED ) ;
      /// ensure static info will be flushed to file.
      /// do it here first.
      syncMemToMmap() ;

      _dmsMME     = NULL ;
      PD_TRACE_EXIT ( SDB__DMSSTORAGEDATA__ONCLOSED ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__INITMME, "_dmsStorageData::_initializeMME" )
   void _dmsStorageData::_initializeMME ()
   {
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__INITMME ) ;
      SDB_ASSERT ( _dmsMME, "MME is NULL" ) ;

      for ( INT32 i = 0; i < DMS_MME_SLOTS ; i++ )
      {
         _dmsMME->_mbList[i].reset () ;
      }
      PD_TRACE_EXIT ( SDB__DMSSTORAGEDATA__INITMME ) ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__LOGDPS, "_dmsStorageData::_logDPS" )
   INT32 _dmsStorageData::_logDPS( SDB_DPSCB * dpsCB,
                                   dpsMergeInfo & info,
                                   pmdEDUCB * cb,
                                   ossSLatch * pLatch,
                                   OSS_LATCH_MODE mode,
                                   BOOLEAN & locked,
                                   UINT32 clLID,
                                   dmsExtentID extLID )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__LOGDPS ) ;
      info.setInfoEx( _logicalCSID, clLID, extLID, cb ) ;
      rc = dpsCB->prepare( info ) ;
      if ( rc )
      {
         goto error ;
      }
      // release lock
      if ( pLatch && locked )
      {
         if ( SHARED == mode )
         {
            pLatch->release_shared() ;
         }
         else
         {
            pLatch->release() ;
         }
         locked = FALSE ;
      }
      // write dps
      dpsCB->writeData( info ) ;
   done :
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__LOGDPS, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__LOGDPS1, "_dmsStorageData::_logDPS" )
   INT32 _dmsStorageData::_logDPS( SDB_DPSCB *dpsCB, dpsMergeInfo &info,
                                   pmdEDUCB *cb, dmsMBContext *context,
                                   dmsExtentID extLID,
                                   BOOLEAN needUnLock,
                                   UINT32 *clLID )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__LOGDPS1 ) ;
      info.setInfoEx( logicalID(),
                      NULL == clLID ?
                      context->clLID() : *clLID,
                      extLID, cb ) ;
      rc = dpsCB->prepare( info ) ;
      if ( rc )
      {
         goto error ;
      }

      // release lock
      if ( needUnLock )
      {
         context->mbUnlock() ;
      }

      // write dps
      dpsCB->writeData( info ) ;
   done :
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__LOGDPS1, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__ALLOCATEEXTENT, "_dmsStorageData::_allocateExtent" )
   INT32 _dmsStorageData::_allocateExtent( dmsMBContext * context,
                                           UINT16 numPages,
                                           BOOLEAN map2DelList,
                                           BOOLEAN add2LoadList,
                                           dmsExtentID *allocExtID )
   {
      SDB_ASSERT( context, "dms mb context can't be NULL" ) ;
      INT32 rc                 = SDB_OK ;
      SINT32 firstFreeExtentID = DMS_INVALID_EXTENT ;
      dmsExtent *extAddr       = NULL ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__ALLOCATEEXTENT ) ;
      PD_TRACE3 ( SDB__DMSSTORAGEDATA__ALLOCATEEXTENT,
                  PD_PACK_USHORT ( numPages ),
                  PD_PACK_UINT ( map2DelList ),
                  PD_PACK_UINT ( add2LoadList ) ) ;
      if ( numPages > segmentPages() || numPages < 1 )
      {
         PD_LOG ( PDERROR, "Invalid number of pages: %d", numPages ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      rc = context->mbLock( EXCLUSIVE ) ;
      PD_RC_CHECK( rc, PDERROR, "dms mb lock failed, rc: %d", rc ) ;

      rc = _findFreeSpace ( numPages, firstFreeExtentID, context ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Error find free space for %d pages, rc = %d",
                  numPages, rc ) ;
         goto error ;
      }

      extAddr = (dmsExtent*)extentAddr( firstFreeExtentID ) ;
      extAddr->init( numPages, context->mbID(),
                     (UINT32)numPages << pageSizeSquareRoot() ) ;

      // and add the new extent into MB's extent chain
      // now let's change the extent pointer into MB's extent list
      // new extent->preextent always assign to _mbList.lastExtentID
      if ( TRUE == add2LoadList )
      {
         extAddr->_prevExtent = context->mb()->_loadLastExtentID ;
         extAddr->_nextExtent = DMS_INVALID_EXTENT ;
         // if this is the load first extent in this MB, we assign
         // firstExtentID to it
         if ( DMS_INVALID_EXTENT == context->mb()->_loadFirstExtentID )
         {
            context->mb()->_loadFirstExtentID = firstFreeExtentID ;
         }

         if ( DMS_INVALID_EXTENT != extAddr->_prevExtent )
         {
            dmsExtent *prevExt = (dmsExtent*)extentAddr(extAddr->_prevExtent) ;
            prevExt->_nextExtent = firstFreeExtentID ;
         }

         // MB's last extent always assigned to the new extent
         context->mb()->_loadLastExtentID = firstFreeExtentID ;
      }
      else
      {
         rc = addExtent2Meta( firstFreeExtentID, extAddr, context ) ;
         PD_RC_CHECK( rc, PDERROR, "Add extent to meta failed, rc: %d", rc ) ;

         /*
         extAddr->_prevExtent = context->mb()->_lastExtentID ;

         // if this is the first extent in this MB, we assign firstExtentID to
         // it
         if ( DMS_INVALID_EXTENT == context->mb()->_firstExtentID )
         {
            context->mb()->_firstExtentID = firstFreeExtentID ;
         }

         // if there's previous record, we reassign the previous
         // extent->nextExtent to this new extent
         if ( DMS_INVALID_EXTENT != extAddr->_prevExtent )
         {
            dmsExtent *prevExt = (dmsExtent*)extentAddr(extAddr->_prevExtent) ;
            prevExt->_nextExtent = firstFreeExtentID ;
            extAddr->_logicID = prevExt->_logicID + 1 ;
         }
         else
         {
            extAddr->_logicID = DMS_INVALID_EXTENT + 1 ;
         }

         // MB's last extent always assigned to the new extent
         context->mb()->_lastExtentID = firstFreeExtentID ;
         */
      }

      // Once we put the extent into linked list, next we want to break such
      // extent into multiple deleted records, and insert into delete list
      if ( map2DelList )
      {
         _mapExtent2DelList( context->mb(), extAddr, firstFreeExtentID ) ;
      }

      if ( allocExtID )
      {
         *allocExtID = firstFreeExtentID ;
         PD_TRACE1 ( SDB__DMSSTORAGEDATA__ALLOCATEEXTENT,
                     PD_PACK_INT ( firstFreeExtentID ) ) ;
      }

   done :
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__ALLOCATEEXTENT, rc ) ;
      return rc ;
   error :
      if ( DMS_INVALID_EXTENT != firstFreeExtentID )
      {
         _freeExtent( firstFreeExtentID ) ;
      }
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__FREEEXTENT, "_dmsStorageData::_freeExtent" )
   INT32 _dmsStorageData::_freeExtent( dmsExtentID extentID )
   {
      INT32 rc = SDB_OK ;
      dmsExtent *extAddr = NULL ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__FREEEXTENT ) ;
      if ( DMS_INVALID_EXTENT == extentID )
      {
         PD_LOG( PDERROR, "Invalid extent id for free" ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      PD_TRACE1 ( SDB__DMSSTORAGEDATA__FREEEXTENT,
                  PD_PACK_INT ( extentID ) ) ;
      extAddr = (dmsExtent*)extentAddr( extentID ) ;

      if ( !extAddr->validate() )
      {
         PD_LOG ( PDERROR, "Invalid eye catcher or flag for extent %d",
                  extentID ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      extAddr->_flag = DMS_EXTENT_FLAG_FREED ;
      // change logical id
      extAddr->_logicID = DMS_INVALID_EXTENT ;

      rc = _releaseSpace( extentID, extAddr->_blockSize ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to release page, rc = %d", rc ) ;
         goto error ;
      }

   done :
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__FREEEXTENT, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__RESERVEFROMDELETELIST, "_dmsStorageData::_reserveFromDeleteList" )
   INT32 _dmsStorageData::_reserveFromDeleteList( dmsMBContext *context,
                                                  UINT32 requiredSize,
                                                  dmsRecordID &resultID,
                                                  pmdEDUCB * cb )
   {
      INT32 rc                      = SDB_OK ;
      UINT32 dmsRecordSizeTemp      = 0 ;
      UINT8  deleteRecordSlot       = 0 ;
      const static INT32 s_maxSearch = 3 ;

      INT32  j                      = 0 ;
      INT32  i                      = 0 ;
      dmsRecordID prevDeletedID ;
      dmsRecordID foundDeletedID  ;
      ossValuePtr prevExtentPtr     = 0 ;
      ossValuePtr extentPtr         = 0 ;
      ossValuePtr delRecordPtr      = 0 ;
      dpsTransCB *pTransCB          = pmdGetKRCB()->getTransCB() ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__RESERVEFROMDELETELIST ) ;
      PD_TRACE1 ( SDB__DMSSTORAGEDATA__RESERVEFROMDELETELIST,
                  PD_PACK_UINT ( requiredSize ) ) ;
      rc = context->mbLock( EXCLUSIVE ) ;
      PD_RC_CHECK( rc, PDERROR, "dms mb context lock failed, rc: %d", rc ) ;

   retry:
      // let's count which delete slots it fits
      // divide by 32 first since our first slot is for <32 bytes
      dmsRecordSizeTemp = ( requiredSize-1 ) >> 5 ;
      deleteRecordSlot  = 0 ;
      while ( dmsRecordSizeTemp != 0 )
      {
         deleteRecordSlot ++ ;
         dmsRecordSizeTemp = dmsRecordSizeTemp >> 1 ;
      }
      SDB_ASSERT( deleteRecordSlot < dmsMB::_max, "Invalid record size" ) ;

      if ( deleteRecordSlot >= dmsMB::_max )
      {
         PD_LOG( PDERROR, "Invalid record size: %u", requiredSize ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      rc = SDB_DMS_NOSPC ;
      for ( j = deleteRecordSlot ; j < dmsMB::_max ; ++j )
      {
         prevDeletedID.reset() ;
         prevExtentPtr =  0 ;
         // get the first delete record from delete list
         foundDeletedID = _dmsMME->_mbList[context->mbID()]._deleteList[j] ;
         for ( i = 0 ; i < s_maxSearch ; ++i )
         {
            // if we don't get a valid record id, we break to get next slot
            if ( foundDeletedID.isNull() )
            {
               break ;
            }
            // otherwise we compare if the required slot is big enough for us
            extentPtr = extentAddr( foundDeletedID._extent ) ;
            if ( 0 == extentPtr )
            {
               PD_LOG ( PDERROR, "Deleted record is incorrect: %d.%d",
                        foundDeletedID._extent, foundDeletedID._offset ) ;
               rc = SDB_SYS ;
               goto error ;
            }

            delRecordPtr = extentPtr + foundDeletedID._offset ;
            // once the extent is valid, let's check the record is deleted
            // and got sufficient size for us
            if( DMS_RECORD_FLAG_DELETED ==
                DMS_DELETEDRECORD_GETFLAG( delRecordPtr ) &&
                DMS_DELETEDRECORD_GETSIZE( delRecordPtr ) >= requiredSize )
            {
               if ( SDB_OK == pTransCB->transLockTestX( cb, _logicalCSID,
                                                        context->mbID(),
                                                        &foundDeletedID ) )
               {
                  if ( 0 == prevExtentPtr )
                  {
                     // it's just the first one from delete list, let's get it
                     context->mb()->_deleteList[j] =
                              DMS_DELETEDRECORD_GETNEXTRID( delRecordPtr ) ;
                  }
                  else
                  {
                     // we need to link the previous delete record to the next
                     DMS_DELETEDRECORD_SETNEXTRID (
                              prevExtentPtr+prevDeletedID._offset,
                              DMS_DELETEDRECORD_GETNEXTRID ( delRecordPtr ) ) ;
                  }
                  resultID = foundDeletedID ;
                  // change extent free space
                  ((dmsExtent*)extentPtr)->_freeSpace -=
                        DMS_DELETEDRECORD_GETSIZE( delRecordPtr ) ;
                  context->mbStat()->_totalDataFreeSpace -=
                        DMS_DELETEDRECORD_GETSIZE( delRecordPtr ) ;
                  rc = SDB_OK ;
                  goto done ;
               }
               else
               {
                  // can't increase i counter
                  --i ;
               }
            }

            //for some reason this slot can't be reused, let's get to the next
            prevDeletedID  = foundDeletedID ;
            prevExtentPtr  = extentPtr ;
            foundDeletedID = DMS_DELETEDRECORD_GETNEXTRID( delRecordPtr ) ;
         }
      }

      // no space, need to allocate extent
      {
         UINT32 reqPages = ( ( ( requiredSize + DMS_EXTENT_METADATA_SZ ) <<
                             DMS_RECORDS_PER_EXTENT_SQUARE ) + pageSize() -
                             1 ) >> pageSizeSquareRoot() ;
         if ( reqPages > segmentPages() )
         {
            reqPages = segmentPages() ;
         }
         if ( reqPages < 1 )
         {
            reqPages = 1 ;
         }

         rc = _allocateExtent( context, reqPages, TRUE, FALSE, NULL ) ;
         PD_RC_CHECK( rc, PDERROR, "Unable to allocate %d pages extent to the "
                      "collection, rc: %d", reqPages, rc ) ;
         goto retry ;
      }

   done:
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__RESERVEFROMDELETELIST, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__TRUNCATECOLLECTION, "_dmsStorageData::_truncateCollection" )
   INT32 _dmsStorageData::_truncateCollection( dmsMBContext *context,
                                               BOOLEAN needChangeCLID )
   {
      INT32 rc                     = SDB_OK ;
      dmsExtentID lastExt          = DMS_INVALID_EXTENT ;
      dmsExtentID prevExt          = DMS_INVALID_EXTENT ;
      dmsMetaExtent *metaExt       = NULL ;
      dmsDictExtent *dictExt       = NULL ;

      SDB_ASSERT( context, "dms mb context can't be NULL" ) ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__TRUNCATECOLLECTION ) ;
      rc = context->mbLock( EXCLUSIVE ) ;
      PD_RC_CHECK( rc, PDERROR, "dms mb context lock failed, rc: %d", rc ) ;

      lastExt = context->mb()->_lastExtentID ;
      // reset delete list
      for ( UINT32 i = 0 ; i < dmsMB::_max ; i++ )
      {
         context->mb()->_deleteList[i].reset() ;
      }
      // free all extent
      while ( DMS_INVALID_EXTENT != lastExt )
      {
         // get the previous extent
         prevExt = ((dmsExtent*)extentAddr(lastExt))->_prevExtent ;
         // free the extent
         rc = _freeExtent ( lastExt ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to free extent[%u], rc: %d", lastExt,
                     rc ) ;
            SDB_ASSERT( SDB_OK == rc, "Free extent can't be failure" ) ;
         }

         // set last to previous
         lastExt = prevExt ;
         // update MB
         context->mb()->_lastExtentID = lastExt ;
      }
      context->mb()->_firstExtentID = DMS_INVALID_EXTENT ;

      // free all load extent
      lastExt = context->mb()->_loadLastExtentID ;
      while ( DMS_INVALID_EXTENT != lastExt )
      {
         prevExt = ((dmsExtent*)extentAddr(lastExt))->_prevExtent ;
         rc = _freeExtent( lastExt ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Failed to free load extent[%u], rc: %d",
                    lastExt, rc ) ;
            SDB_ASSERT( SDB_OK == rc, "Free extent can't be failure" ) ;
         }
         lastExt = prevExt ;
         context->mb()->_loadLastExtentID = lastExt ;
      }
      context->mb()->_loadFirstExtentID = DMS_INVALID_EXTENT ;
      metaExt = ( dmsMetaExtent* )extentAddr( context->mb()->_mbExExtentID ) ;
      if ( metaExt )
      {
         metaExt->reset() ;
      }

      /*
       * Incase of drop/truncate collection, destroy the compressor and release
       * the dictionary both in memory and on disk.
       */
      if ( DMS_INVALID_EXTENT != context->mb()->_dictExtentID
           && needChangeCLID )
      {
         /*
          * First remove the compressor, this will invalid the compressor entry.
          */
         rmCompressor( context ) ;
         dictExt
            = ( dmsDictExtent * )extentAddr( context->mb()->_dictExtentID ) ;
         _releaseSpace( context->mb()->_dictExtentID, dictExt->_blockSize ) ;
         dictExt->_flag = DMS_EXTENT_FLAG_FREED ;
         context->mb()->_dictExtentID = DMS_INVALID_EXTENT ;
         context->mbStat()->_dictExtID = DMS_INVALID_EXTENT ;
      }

      context->mbStat()->_totalDataFreeSpace = 0 ;
      context->mbStat()->_totalDataPages = 0 ;
      context->mbStat()->_totalRecords = 0 ;

   done :
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__TRUNCATECOLLECTION, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__TRUNCATECOLLECITONLOADS, "_dmsStorageData::_truncateCollectionLoads" )
   INT32 _dmsStorageData::_truncateCollectionLoads( dmsMBContext * context )
   {
      INT32 rc                     = SDB_OK ;
      dmsExtentID lastExt          = DMS_INVALID_EXTENT ;
      dmsExtentID prevExt          = DMS_INVALID_EXTENT ;

      SDB_ASSERT( context, "dms mb context can't be NULL" ) ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__TRUNCATECOLLECITONLOADS ) ;
      rc = context->mbLock( EXCLUSIVE ) ;
      PD_RC_CHECK( rc, PDERROR, "dms mb context lock failed, rc: %d", rc ) ;

      // free all load extent
      lastExt = context->mb()->_loadLastExtentID ;
      while ( DMS_INVALID_EXTENT != lastExt )
      {
         rc = context->mbLock( EXCLUSIVE ) ;
         PD_RC_CHECK( rc, PDERROR, "dms mb context lock failed, rc: %d", rc ) ;

         prevExt = ((dmsExtent*)extentAddr(lastExt))->_prevExtent ;
         rc = _freeExtent( lastExt ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Failed to free load extent[%u], rc: %d",
                    lastExt, rc ) ;
            SDB_ASSERT( SDB_OK == rc, "Free extent can't be failure" ) ;
         }
         lastExt = prevExt ;
         context->mb()->_loadLastExtentID = lastExt ;

         if ( DMS_INVALID_EXTENT != lastExt )
         {
            context->mbUnlock() ;
         }
      }
      context->mb()->_loadFirstExtentID = DMS_INVALID_EXTENT ;

   done :
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__TRUNCATECOLLECITONLOADS, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__SAVEDELETEDRECORD, "_dmsStorageData::_saveDeletedRecord" )
   INT32 _dmsStorageData::_saveDeletedRecord( dmsMB *mb, dmsExtent * extAddr,
                                              dmsOffset offset,
                                              INT32 recordSize,
                                              INT32 extentID )
   {
      UINT8 deleteRecordSlot = 0 ;
      ossValuePtr recordPtr = ( (ossValuePtr)extAddr + offset ) ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__SAVEDELETEDRECORD ) ;
      PD_TRACE6 ( SDB__DMSSTORAGEDATA__SAVEDELETEDRECORD,
                  PD_PACK_STRING ( "offset" ),
                  PD_PACK_INT ( offset ),
                  PD_PACK_STRING ( "recordSize" ),
                  PD_PACK_INT ( recordSize ),
                  PD_PACK_STRING ( "extentID" ),
                  PD_PACK_INT ( extentID ) ) ;
      // assign flags to the memory
      DMS_DELETEDRECORD_SETFLAG( recordPtr, DMS_RECORD_FLAG_DELETED ) ;
      DMS_DELETEDRECORD_SETSIZE( recordPtr, recordSize ) ;
      DMS_DELETEDRECORD_SETMYOFFSET( recordPtr, offset ) ;

      // change free space
      extAddr->_freeSpace += recordSize ;
      _mbStatInfo[mb->_blockID]._totalDataFreeSpace += recordSize ;

      // let's count which delete slots it fits
      // divide by 32 first since our first slot is for <32 bytes
      recordSize = ( recordSize - 1 ) >> 5 ;
      // while loop, divde by 2 everytime, find the closest delete slot
      // for example, for a given size 3000, we should go _4k (which is
      // _deleteList[7], using 3000>>5=93
      // then in a loop, first round we have 46, type=1
      // then 23, type=2
      // then 11, type=3
      // then 5, type=4
      // then 2, type=5
      // then 1, type=6
      // finally 0, type=7

      while ( (recordSize) != 0 )
      {
         deleteRecordSlot ++ ;
         recordSize = ( recordSize >> 1 ) ;
      }

      // make sure we don't mis calculated it
      SDB_ASSERT ( deleteRecordSlot < dmsMB::_max, "Invalid record size" ) ;

      // set the first matching delete slot to the
      // next rid for the deleted record
      DMS_DELETEDRECORD_SETNEXTRID ( recordPtr,
                                     mb->_deleteList [ deleteRecordSlot ] ) ;
      // Then assign MB delete slot to the extent and offset
      mb->_deleteList[ deleteRecordSlot ]._extent = extentID ;
      mb->_deleteList[ deleteRecordSlot ]._offset = offset ;
      PD_TRACE_EXIT ( SDB__DMSSTORAGEDATA__SAVEDELETEDRECORD ) ;
      return SDB_OK ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__SAVEDELETEDRECORD1, "_dmsStorageData::_saveDeletedRecord" )
   INT32 _dmsStorageData::_saveDeletedRecord( dmsMB * mb,
                                              const dmsRecordID &recordID,
                                              INT32 recordSize )
   {
      INT32 rc = SDB_OK ;
      ossValuePtr extentPtr = 0 ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__SAVEDELETEDRECORD1 ) ;
      PD_TRACE2 ( SDB__DMSSTORAGEDATA__SAVEDELETEDRECORD1,
                  PD_PACK_INT ( recordID._extent ),
                  PD_PACK_INT ( recordID._offset ) ) ;
      if ( recordID.isNull() )
      {
         rc = SDB_INVALIDARG ;
         goto done ;
      }
      extentPtr = extentAddr( recordID._extent ) ;

      if ( 0 == recordSize )
      {
         ossValuePtr recordPtr = extentPtr + recordID._offset ;
         recordSize = DMS_RECORD_GETSIZE(recordPtr) ;
      }

      rc = _saveDeletedRecord ( mb, (dmsExtent*)extentPtr, recordID._offset,
                                recordSize, recordID._extent ) ;
   done :
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__SAVEDELETEDRECORD1, rc ) ;
      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__MAPEXTENT2DELLIST, "_dmsStorageData::_mapExtent2DelList" )
   void _dmsStorageData::_mapExtent2DelList( dmsMB * mb, dmsExtent * extAddr,
                                             SINT32 extentID )
   {
      INT32 extentSize         = 0 ;
      INT32 extentUseableSpace = 0 ;
      INT32 deleteRecordSize   = 0 ;
      dmsOffset recordOffset   = 0 ;
      INT32 curUseableSpace    = 0 ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__MAPEXTENT2DELLIST ) ;
      if ( (UINT32)extAddr->_freeSpace < DMS_MIN_RECORD_SZ )
      {
         if ( extAddr->_freeSpace != 0 )
         {
            PD_LOG( PDINFO, "Collection[%s, mbID: %d]'s extent[%d] free "
                    "space[%d] is less than min record size[%d]",
                    mb->_collectionName, mb->_blockID, extentID,
                    extAddr->_freeSpace, DMS_MIN_RECORD_SZ ) ;
         }
         goto done ;
      }

      // calculate the delete record size we need to use
      extentSize          = extAddr->_blockSize << pageSizeSquareRoot() ;
      extentUseableSpace  = extAddr->_freeSpace ;
      extAddr->_freeSpace = 0 ;

      // make sure the delete record is not greater 16MB
      deleteRecordSize    = OSS_MIN ( extentUseableSpace,
                                      DMS_RECORD_MAX_SZ ) ;
      // place first record offset
      recordOffset        = extentSize - extentUseableSpace ;
      curUseableSpace     = extentUseableSpace ;

      // if extentUseableSpace > 16MB
      while ( curUseableSpace - deleteRecordSize >=
              (INT32)DMS_MIN_DELETEDRECORD_SZ )
      {
         _saveDeletedRecord( mb, extAddr, recordOffset, deleteRecordSize,
                             extentID ) ;
         curUseableSpace -= deleteRecordSize ;
         recordOffset += deleteRecordSize ;
      }
      if ( curUseableSpace > deleteRecordSize )
      {
         _saveDeletedRecord( mb, extAddr, recordOffset, DMS_PAGE_SIZE4K,
                             extentID ) ;
         curUseableSpace -= DMS_PAGE_SIZE4K ;
         recordOffset += DMS_PAGE_SIZE4K ;
      }

      if ( curUseableSpace > 0 )
      {
         _saveDeletedRecord( mb, extAddr, recordOffset, curUseableSpace,
                             extentID ) ;
      }

      // correct check
      SDB_ASSERT( extentUseableSpace == extAddr->_freeSpace,
                  "Extent[%d] free space invalid" ) ;
   done :
      PD_TRACE_EXIT ( SDB__DMSSTORAGEDATA__MAPEXTENT2DELLIST ) ;
      return ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_ADDEXTENT2META, "_dmsStorageData::addExtent2Meta" )
   INT32 _dmsStorageData::addExtent2Meta( dmsExtentID extID,
                                          dmsExtent *extent,
                                          dmsMBContext *context )
   {
      INT32 rc = SDB_OK ;
      dmsMBEx *mbEx = NULL ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_ADDEXTENT2META ) ;
      UINT32 segID = extent2Segment( extID ) - dataStartSegID() ;
      dmsExtentID lastExtID = DMS_INVALID_EXTENT ;
      dmsExtent *prevExt = NULL ;
      dmsExtent *nextExt = NULL ;

      if ( !context->isMBLock( EXCLUSIVE ) )
      {
         rc = SDB_SYS ;
         PD_LOG( PDERROR, "Caller must hold mb exclusive lock[%s]",
                 context->toString().c_str() ) ;
         goto error ;
      }

      // SYSTEMP SU
      if ( isTempSU() )
      {
         extent->_prevExtent = context->mb()->_lastExtentID ;

         // if this is the first extent in this MB, we assign firstExtentID to
         // it
         if ( DMS_INVALID_EXTENT == context->mb()->_firstExtentID )
         {
            context->mb()->_firstExtentID = extID ;
         }

         // if there's previous record, we reassign the previous
         // extent->nextExtent to this new extent
         if ( DMS_INVALID_EXTENT != extent->_prevExtent )
         {
            dmsExtent *prevExt = (dmsExtent*)extentAddr(extent->_prevExtent) ;
            prevExt->_nextExtent = extID ;
            extent->_logicID = prevExt->_logicID + 1 ;
         }
         else
         {
            extent->_logicID = DMS_INVALID_EXTENT + 1 ;
         }

         // MB's last extent always assigned to the new extent
         context->mb()->_lastExtentID = extID ;
      }
      else
      {
         mbEx = ( dmsMBEx* )extentAddr( context->mb()->_mbExExtentID ) ;
         if ( NULL == mbEx )
         {
            rc = SDB_SYS ;
            PD_LOG( PDERROR, "dms mb expand extent is invalid: %d",
                    context->mb()->_mbExExtentID ) ;
            goto error ;
         }

         if ( segID >= mbEx->_header._segNum )
         {
            rc = SDB_SYS ;
            PD_LOG( PDERROR, "Invalid segID: %d, max segNum: %d", segID,
                    mbEx->_header._segNum ) ;
            goto error ;
         }

         mbEx->getLastExtentID( segID, lastExtID ) ;

         if ( DMS_INVALID_EXTENT == lastExtID )
         {
            extent->_logicID = ( segID << _getFactor() ) ;
            mbEx->setFirstExtentID( segID, extID ) ;
            mbEx->setLastExtentID( segID, extID ) ;
            ++(mbEx->_header._usedSegNum) ;

            // find prevExt
            INT32 tmpSegID = segID ;
            dmsExtentID tmpExtID = DMS_INVALID_EXTENT ;
            while ( DMS_INVALID_EXTENT != context->mb()->_firstExtentID &&
                    --tmpSegID >= 0 )
            {
               mbEx->getLastExtentID( tmpSegID, tmpExtID ) ;
               if ( DMS_INVALID_EXTENT != tmpExtID )
               {
                  extent->_prevExtent = tmpExtID ;
                  prevExt = ( dmsExtent* )extentAddr( tmpExtID ) ;
                  break ;
               }
            }
         }
         else
         {
            mbEx->setLastExtentID( segID, extID ) ;
            extent->_prevExtent = lastExtID ;
            prevExt = ( dmsExtent* )extentAddr( lastExtID ) ;
            extent->_logicID = prevExt->_logicID + 1 ;
         }

         if ( prevExt )
         {
            if ( DMS_INVALID_EXTENT != prevExt->_nextExtent )
            {
               extent->_nextExtent = prevExt->_nextExtent ;
               nextExt = ( dmsExtent* )extentAddr( extent->_nextExtent ) ;
               nextExt->_prevExtent = extID ;
            }
            else
            {
               context->mb()->_lastExtentID = extID ;
            }
            prevExt->_nextExtent = extID ;
         }
         else
         {
            if ( DMS_INVALID_EXTENT != context->mb()->_firstExtentID )
            {
               extent->_nextExtent = context->mb()->_firstExtentID ;
               nextExt = ( dmsExtent* )extentAddr( extent->_nextExtent ) ;
               nextExt->_prevExtent = extID ;
            }
            context->mb()->_firstExtentID = extID ;

            if ( DMS_INVALID_EXTENT == context->mb()->_lastExtentID )
            {
               context->mb()->_lastExtentID = extID ;
            }
         }
      }

      context->mbStat()->_totalDataPages += extent->_blockSize ;

   done:
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_ADDEXTENT2META, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_ADDCOLLECTION, "_dmsStorageData::addCollection" )
   INT32 _dmsStorageData::addCollection( const CHAR * pName,
                                         UINT16 * collectionID,
                                         UINT32 attributes,
                                         pmdEDUCB * cb,
                                         SDB_DPSCB * dpscb,
                                         UINT16 initPages,
                                         BOOLEAN sysCollection,
                                         BOOLEAN noIDIndex,
                                         UTIL_COMPRESSOR_TYPE compressorType )
   {
      INT32 rc                = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_ADDCOLLECTION ) ;
      static BSONObj s_idKeyObj = BSON( IXM_FIELD_NAME_KEY <<
                                        BSON( DMS_ID_KEY_NAME << 1 ) <<
                                        IXM_FIELD_NAME_NAME << IXM_ID_KEY_NAME
                                        << IXM_FIELD_NAME_UNIQUE <<
                                        true << IXM_FIELD_NAME_V << 0 <<
                                        IXM_FIELD_NAME_ENFORCED << true ) ;
      dpsMergeInfo info ;
      dpsLogRecord &record    = info.getMergeBlock().record() ;
      UINT32 logRecSize       = 0;
      dpsTransCB *pTransCB    = pmdGetKRCB()->getTransCB() ;
      CHAR fullName[DMS_COLLECTION_FULL_NAME_SZ + 1] = {0} ;
      UINT16 newCollectionID  = DMS_INVALID_MBID ;
      UINT32 logicalID        = DMS_INVALID_CLID ;
      BOOLEAN metalocked      = FALSE ;
      dmsMB *mb               = NULL ;
      SDB_DPSCB *dropDps      = NULL ;
      dmsMBContext *context   = NULL ;

      UINT32 segNum           = DMS_MAX_PG >> segmentPagesSquareRoot() ;
      UINT32 mbExSize         = (( segNum << 3 ) >> pageSizeSquareRoot()) + 1 ;
      dmsExtentID mbExExtent  = DMS_INVALID_EXTENT ;
      dmsMetaExtent *mbExtent = NULL ;
      pmdKRCB *krCB = pmdGetKRCB() ;
      SDB_DMSCB *dmsCB = krCB->getDMSCB() ;

      SINT8 compType = (SINT8)compressorType ;

      SDB_ASSERT( pName, "Collection name cat't be NULL" ) ;

      rc = dmsCheckCLName ( pName, sysCollection ) ;
      PD_RC_CHECK( rc, PDERROR, "Invalid collection name %s, rc: %d",
                   pName, rc ) ;

      PD_CHECK( initPages <= segmentPages(), SDB_INVALIDARG, error, PDERROR,
                "Invalid pages: %u", initPages ) ;

      _registerNewWriting() ;
      // calc the reserve dps size
      if ( dpscb )
      {
         rc = dpsCLCrt2Record( _clFullName(pName, fullName, sizeof(fullName)),
                               attributes, compType, record ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build record, rc: %d", rc ) ;

         rc = dpscb->checkSyncControl( record.alignedLen(), cb ) ;
         PD_RC_CHECK( rc, PDERROR, "Check sync control failed, rc: %d", rc ) ;

         logRecSize = record.alignedLen() ;
         rc = pTransCB->reservedLogSpace( logRecSize, cb ) ;
         if( rc )
         {
            PD_LOG( PDERROR, "Failed to reserved log space(length=%u)",
                    logRecSize ) ;
            logRecSize = 0 ;
            goto error ;
         }
      }

      if ( !isTempSU () )
      {
         // allocate mbEx extent
         rc = _findFreeSpace( mbExSize, mbExExtent, NULL ) ;
         PD_RC_CHECK( rc, PDERROR, "Allocate metablock expand extent failed, "
                      "pageNum: %d, rc: %d", mbExSize, rc ) ;
      }

      // first exclusive latch metadata, this shouldn't be replaced by SHARED to
      // prevent racing with dropCollection
      ossLatch( &_metadataLatch, EXCLUSIVE ) ;
      metalocked = TRUE ;

      // then let's make sure the collection name does not exist
      if ( DMS_INVALID_MBID != _collectionNameLookup ( pName ) )
      {
         rc = SDB_DMS_EXIST ;
         goto error ;
      }

      if ( DMS_MME_SLOTS <= _dmsHeader->_numMB )
      {
         PD_LOG ( PDERROR, "There is no free slot for extra collection" ) ;
         rc = SDB_DMS_NOSPC ;
         goto error ;
      }

      // find a slot
      for ( UINT32 i = 0 ; i < DMS_MME_SLOTS ; ++i )
      {
         if ( DMS_IS_MB_FREE ( _dmsMME->_mbList[i]._flag ) ||
              DMS_IS_MB_DROPPED ( _dmsMME->_mbList[i]._flag ) )
         {
            newCollectionID = i ;
            break ;
         }
      }
      // make sure we find free collection id
      if ( DMS_INVALID_MBID == newCollectionID )
      {
         PD_LOG ( PDERROR, "Unable to find free collection id" ) ;
         rc = SDB_SYS ;
         goto error ;
      }

      // set mb meta data and header data
      logicalID = _dmsHeader->_MBHWM++ ;
      mb = &_dmsMME->_mbList[newCollectionID] ;
      // attribuites should contain the compress type.
      mb->reset( pName, newCollectionID, logicalID, attributes ) ;
      if ( !isTempSU() && ( UTIL_COMPRESSOR_INVALID != compressorType ) )
      {
         mb->_compressorType = compType ;
      }

      _mbStatInfo[ newCollectionID ].reset() ;
      _dmsHeader->_numMB++ ;
      _collectionNameInsert( pName, newCollectionID ) ;

      if ( !isTempSU () )
      {
         mbExtent = ( dmsMetaExtent* )extentAddr( mbExExtent ) ;
         PD_CHECK( mbExtent, SDB_SYS, error, PDERROR, "Invalid meta extent[%d]",
                   mbExExtent ) ;
         mbExtent->init( mbExSize, newCollectionID, segNum ) ;
         mb->_mbExExtentID = mbExExtent ;
         mbExExtent = DMS_INVALID_EXTENT ;

         /* Only new supported compressors such as lzw need a dictionary. */
         if ( UTIL_COMPRESSOR_LZW == compressorType )
         {
            dmsCB->pushToDictCreateCLList( CSID(), newCollectionID ) ;
         }
      }

      // write dps log
      if ( dpscb )
      {
         rc = _logDPS( dpscb, info, cb, &_metadataLatch, EXCLUSIVE,
                       metalocked, logicalID, DMS_INVALID_EXTENT ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to insert CLcrt record to log, "
                      "rc = %d", rc ) ;
      }

      // release meta lock
      if ( metalocked )
      {
         ossUnlatch( &_metadataLatch, EXCLUSIVE ) ;
         metalocked = FALSE ;
      }

      if ( collectionID )
      {
         *collectionID = newCollectionID ;
      }
      dropDps = dpscb ;

      // create dms cb context
      rc = getMBContext( &context, newCollectionID, logicalID, EXCLUSIVE ) ;
      if ( rc )
      {
         PD_LOG( PDERROR, "Failed to get mb[%u] context, rc: %d",
                 newCollectionID, rc ) ;
         if ( SDB_DMS_NOTEXIST == rc )
         {
            newCollectionID = DMS_INVALID_MBID ;
         }
         goto error ;
      }

      // allocate new extent
      if ( 0 != initPages )
      {
         rc = _allocateExtent( context, initPages, TRUE, FALSE, NULL ) ;
         PD_RC_CHECK( rc, PDERROR, "Allocate new %u pages of collection[%s] "
                      "failed, rc: %d", initPages, pName, rc ) ;
      }

      // create $id index[s_idKeyObj]
      if ( !noIDIndex &&
           !OSS_BIT_TEST( attributes, DMS_MB_ATTR_NOIDINDEX ) )
      {
         rc = _pIdxSU->createIndex( context, s_idKeyObj, cb, NULL, TRUE ) ;
         PD_RC_CHECK( rc, PDERROR, "Create $id index failed in collection[%s], "
                      "rc: %d", pName, rc ) ;
      }

   done:
      if ( context )
      {
         releaseMBContext( context ) ;
      }
      if ( 0 != logRecSize )
      {
         pTransCB->releaseLogSpace( logRecSize, cb );
      }
      if ( SDB_OK == rc && NULL != cb )
      {
         _updateLastLSN( cb->getEndLsn() ) ;
      }
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_ADDCOLLECTION, rc ) ;
      return rc ;
   error:
      if ( metalocked )
      {
         ossUnlatch( &_metadataLatch, EXCLUSIVE ) ;
      }
      if ( DMS_INVALID_EXTENT != mbExExtent )
      {
         _releaseSpace( mbExExtent, mbExSize ) ;
      }
      if ( DMS_INVALID_MBID != newCollectionID )
      {
         // drop collection
         INT32 rc1 = dropCollection( pName, cb, dropDps, sysCollection,
                                     context ) ;
         if ( rc1 )
         {
            PD_LOG( PDSEVERE, "Failed to clean up bad collection creation[%s], "
                    "rc: %d", pName, rc ) ;
         }
      }
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_DROPCOLLECTION, "_dmsStorageData::dropCollection" )
   INT32 _dmsStorageData::dropCollection( const CHAR * pName, pmdEDUCB * cb,
                                          SDB_DPSCB * dpscb,
                                          BOOLEAN sysCollection,
                                          dmsMBContext * context )
   {
      INT32 rc                = 0 ;
      CHAR fullName[DMS_COLLECTION_FULL_NAME_SZ + 1] = {0} ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_DROPCOLLECTION ) ;
      dpsMergeInfo info ;
      dpsLogRecord &record    = info.getMergeBlock().record() ;
      UINT32 logRecSize       = 0;
      dpsTransCB *pTransCB    = pmdGetKRCB()->getTransCB() ;
      BOOLEAN isTransLocked   = FALSE ;
      BOOLEAN getContext      = FALSE ;
      BOOLEAN metalocked      = FALSE ;
      dmsMetaExtent *metaExt  = NULL ;

      SDB_ASSERT( pName, "Collection name cat't be NULL" ) ;

      rc = dmsCheckCLName ( pName, sysCollection ) ;
      PD_RC_CHECK( rc, PDERROR, "Invalid collection name %s, rc: %d",
                   pName, rc ) ;

      // calc the reserve dps size
      if ( dpscb )
      {
         rc = dpsCLDel2Record( _clFullName(pName, fullName, sizeof(fullName)),
                               record ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build record, rc: %d", rc ) ;

         rc = dpscb->checkSyncControl( record.alignedLen(), cb ) ;
         PD_RC_CHECK( rc, PDERROR, "Check sync control failed, rc: %d", rc ) ;

         logRecSize = record.alignedLen() ;
         rc = pTransCB->reservedLogSpace( logRecSize, cb ) ;
         if( rc )
         {
            PD_LOG( PDERROR, "Failed to reserved log space(length=%u)",
                    logRecSize ) ;
            logRecSize = 0 ;
            goto error ;
         }
      }

      _registerNewWriting() ;
      // lock collection mb exclusive lock
      if ( NULL == context )
      {
         rc = getMBContext( &context, pName, EXCLUSIVE ) ;
         PD_RC_CHECK( rc, PDWARNING, "Failed to get mb[%s] context, rc: %d",
                      pName, rc ) ;
         getContext = TRUE ;
      }
      else
      {
         rc = context->mbLock( EXCLUSIVE ) ;
         PD_RC_CHECK( rc, PDWARNING, "Collection[%s] mblock failed, rc: %d",
                      pName, rc ) ;
      }

      if ( !dmsAccessAndFlagCompatiblity ( context->mb()->_flag,
                                           DMS_ACCESS_TYPE_TRUNCATE ) )
      {
         PD_LOG ( PDERROR, "Incompatible collection mode: %d",
                  context->mb()->_flag ) ;
         rc = SDB_DMS_INCOMPATIBLE_MODE ;
         goto error ;
      }

      // it is not need to lock that drop temp collection while startup
      // which cb is NULL
      if ( cb && dpscb )
      {
         rc = pTransCB->transLockTryX( cb, _logicalCSID, context->mbID() ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to lock the collection, rc: %d",
                      rc ) ;
         isTransLocked = TRUE ;
      }

      // drop all index
      rc = _pIdxSU->dropAllIndexes( context, cb, NULL ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to drop index for collection[%s], "
                   "rc: %d", pName, rc ) ;

      // truncate the collection
      rc = _truncateCollection( context ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to truncate the collection[%s], rc: %d",
                   pName, rc ) ;

      // truncate lob
      if ( _pLobSU->isOpened() )
      {
         rc = _pLobSU->truncate( context, cb, NULL ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to truncate the collection[%s] lob,"
                      "rc: %d", pName, rc ) ;
      }

      // change mb meta data
      DMS_SET_MB_FREE( context->mb()->_flag ) ;
      context->mb()->_logicalID-- ;

      /*
       * The space of dictionary has been release in _truncateCollection. In
       * drop case, the compression type should be set to invalid.
       */
      if ( -1 != context->mb()->_compressorType)
      {
         context->mb()->_compressorType = -1 ;
         context->mbStat()->_compressorType = -1 ;
      }

      // free meta extent
      metaExt = ( dmsMetaExtent* )extentAddr( context->mb()->_mbExExtentID ) ;
      if ( metaExt )
      {
         _releaseSpace( context->mb()->_mbExExtentID, metaExt->_blockSize ) ;
         metaExt->_flag = DMS_EXTENT_FLAG_FREED ;
      }

      // release mb lock
      context->mbUnlock() ;

      // change metadata
      ossLatch( &_metadataLatch, EXCLUSIVE ) ;
      metalocked = TRUE ;
      _collectionNameRemove( pName ) ;
      _dmsHeader->_numMB-- ;

      // write dps log
      if ( dpscb )
      {
         rc = _logDPS( dpscb, info, cb, &_metadataLatch, EXCLUSIVE, metalocked,
                       context->clLID(), DMS_INVALID_EXTENT ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to insert CLDel record to log, rc: "
                      "%d", rc ) ;
      }

   done:
      if ( metalocked )
      {
         ossUnlatch( &_metadataLatch, EXCLUSIVE ) ;
         metalocked = FALSE ;
      }
      if ( isTransLocked )
      {
         pTransCB->transLockRelease( cb, _logicalCSID, context->mbID() );
      }
      if ( context && getContext )
      {
         releaseMBContext( context ) ;
      }
      if ( 0 != logRecSize )
      {
         pTransCB->releaseLogSpace( logRecSize, cb );
      }

      if ( SDB_OK == rc && NULL != cb )
      {
         _updateLastLSN( cb->getEndLsn() ) ;
      }
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_DROPCOLLECTION, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_TRUNCATECOLLECTION, "_dmsStorageData::truncateCollection" )
   INT32 _dmsStorageData::truncateCollection( const CHAR *pName,
                                              pmdEDUCB *cb,
                                              SDB_DPSCB *dpscb,
                                              BOOLEAN sysCollection,
                                              dmsMBContext *context,
                                              BOOLEAN needChangeCLID,
                                              BOOLEAN truncateLob )
   {
      INT32 rc           = SDB_OK ;
      BOOLEAN getContext = FALSE ;
      UINT32 newCLID     = DMS_INVALID_CLID ;
      UINT32 oldCLID     = DMS_INVALID_CLID ;
      UINT64 oldRecords  = 0 ;
      UINT64 oldLobs     = 0 ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_TRUNCATECOLLECTION ) ;
      CHAR fullName[DMS_COLLECTION_FULL_NAME_SZ + 1] = {0} ;
      dpsMergeInfo info ;
      dpsLogRecord &record    = info.getMergeBlock().record() ;
      UINT32 logRecSize       = 0;
      dpsTransCB *pTransCB    = pmdGetKRCB()->getTransCB() ;
      BOOLEAN isTransLocked   = FALSE ;
      SDB_DMSCB *dmsCB = pmdGetKRCB()->getDMSCB() ;

      SDB_ASSERT( pName, "Collection name cat't be NULL" ) ;

      rc = dmsCheckCLName ( pName, sysCollection ) ;
      PD_RC_CHECK( rc, PDERROR, "Invalid collection name %s, rc: %d",
                   pName, rc ) ;

        _registerNewWriting() ;
      // calc the reserve dps size
      if ( dpscb )
      {
         rc = dpsCLTrunc2Record( _clFullName(pName, fullName, sizeof(fullName)),
                                 record ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build record, rc: %d", rc ) ;

         rc = dpscb->checkSyncControl( record.alignedLen(), cb ) ;
         PD_RC_CHECK( rc, PDERROR, "Check sync control failed, rc: %d", rc ) ;

         logRecSize = record.alignedLen() ;
         rc = pTransCB->reservedLogSpace( logRecSize, cb ) ;
         if( rc )
         {
            PD_LOG( PDERROR, "Failed to reserved log space(length=%u)",
                    logRecSize ) ;
            logRecSize = 0 ;
            goto error ;
         }
      }

      // lock collection mb exclusive lock
      if ( NULL == context )
      {
         rc = getMBContext( &context, pName, EXCLUSIVE ) ;
         PD_RC_CHECK( rc, PDWARNING, "Failed to get mb[%s] context, rc: %d",
                      pName, rc ) ;
         getContext = TRUE ;
      }
      else
      {
         rc = context->mbLock( EXCLUSIVE ) ;
         PD_RC_CHECK( rc, PDWARNING, "Collection[%s] mblock failed, rc: %d",
                      pName, rc ) ;
      }

      if ( !dmsAccessAndFlagCompatiblity ( context->mb()->_flag,
                                           DMS_ACCESS_TYPE_TRUNCATE ) )
      {
         PD_LOG ( PDERROR, "Incompatible collection mode: %d",
                  context->mb()->_flag ) ;
         rc = SDB_DMS_INCOMPATIBLE_MODE ;
         goto error ;
      }

      // trans lock
      if ( cb && dpscb )
      {
         rc = pTransCB->transLockTryX( cb, _logicalCSID, context->mbID() ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to lock the collection, rc: %d",
                      rc ) ;
         isTransLocked = TRUE ;
      }

      // pause mb lock and change metadata
      context->pause() ;
      ossLatch( &_metadataLatch, EXCLUSIVE ) ;
      if ( needChangeCLID )
      {
         newCLID = _dmsHeader->_MBHWM++ ;
      }
      ossUnlatch( &_metadataLatch, EXCLUSIVE ) ;

      // resume context lock
      rc = context->resume() ;
      PD_RC_CHECK( rc, PDERROR, "dms mb context resume falied, rc: %d", rc ) ;

      oldRecords = context->mbStat()->_totalRecords ;
      oldLobs = context->mbStat()->_totalLobs ;

      rc = _pIdxSU->truncateIndexes( context ) ;
      PD_RC_CHECK( rc, PDERROR, "Truncate collection[%s] indexes failed, "
                   "rc: %d", pName, rc ) ;

      rc = _truncateCollection( context, needChangeCLID ) ;
      PD_RC_CHECK( rc, PDERROR, "Truncate collection[%s] data failed, rc: %d",
                   pName, rc ) ;

      if ( truncateLob && _pLobSU->isOpened() )
      {
         rc = _pLobSU->truncate( context, cb, NULL ) ;
         PD_RC_CHECK( rc, PDERROR, "Truncate collection[%s] lob failed, rc: %d",
                      pName, rc ) ;
      }

      // change mb metadata
      if ( needChangeCLID )
      {
         oldCLID = context->_clLID ;
         context->mb()->_logicalID = newCLID ;
         context->_clLID           = newCLID ;
      }

      // write dps log
      if ( dpscb )
      {
         PD_AUDIT_OP_WITHNAME( AUDIT_DML, "TRUNCATE", AUDIT_OBJ_CL,
                               fullName, rc, "RecordNum:%llu, LobNum:%llu",
                               oldRecords, oldLobs ) ;
         rc = _logDPS( dpscb, info, cb, context, DMS_INVALID_EXTENT,
                       TRUE, &oldCLID ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to insert CLTrunc record to log, "
                      "rc: %d", rc ) ;
      }

      if ( -1 != context->mb()->_compressorType && needChangeCLID )
      {
         /*
          * The original dictionary and compressor will be removed during
          * truncation. So it should be pushed to the dictionary creating list
          * again after truncation.
          */
         dmsCB->pushToDictCreateCLList( CSID() , context->mbID() ) ;
      }

   done:
      if ( isTransLocked )
      {
         pTransCB->transLockRelease( cb, _logicalCSID, context->mbID() ) ;
      }
      if ( context && getContext )
      {
         releaseMBContext( context ) ;
      }
      if ( 0 != logRecSize )
      {
         pTransCB->releaseLogSpace( logRecSize, cb ) ;
      }
      if ( SDB_OK == rc && NULL != cb )
      {
         _updateLastLSN( cb->getEndLsn() ) ;
      }
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_TRUNCATECOLLECTION, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_TRUNCATECOLLECTIONLOADS, "_dmsStorageData::truncateCollectionLoads" )
   INT32 _dmsStorageData::truncateCollectionLoads( const CHAR *pName,
                                                   dmsMBContext * context )
   {
      INT32 rc           = SDB_OK ;
      BOOLEAN getContext = FALSE ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_TRUNCATECOLLECTIONLOADS ) ;
      if ( NULL == context )
      {
         SDB_ASSERT( pName, "Collection name cat't be NULL" ) ;

         rc = getMBContext( &context, pName, -1 ) ;
         PD_RC_CHECK( rc, PDWARNING, "Failed to get mb[%s] context, rc: %d",
                      pName, rc ) ;
         getContext = TRUE ;
      }

      rc = _truncateCollectionLoads( context ) ;
      PD_RC_CHECK( rc, PDERROR, "Truncate collection[%s] loads failed, rc: %d",
                   pName, rc ) ;

   done:
      if ( context && getContext )
      {
         releaseMBContext( context ) ;
      }
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_TRUNCATECOLLECTIONLOADS, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_RENAMECOLLECTION, "_dmsStorageData::renameCollecion" )
   INT32 _dmsStorageData::renameCollection( const CHAR * oldName,
                                            const CHAR * newName,
                                            pmdEDUCB * cb,
                                            SDB_DPSCB * dpscb,
                                            BOOLEAN sysCollection )
   {
      INT32 rc                = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_RENAMECOLLECTION ) ;
      dpsTransCB *pTransCB    = pmdGetKRCB()->getTransCB() ;
      UINT32 logRecSize       = 0 ;
      dpsMergeInfo info ;
      dpsLogRecord &record    = info.getMergeBlock().record() ;
      BOOLEAN metalocked      = FALSE ;
      UINT16  mbID            = DMS_INVALID_MBID ;
      UINT32  clLID           = DMS_INVALID_CLID ;

      PD_TRACE2 ( SDB__DMSSTORAGEDATA_RENAMECOLLECTION,
                  PD_PACK_STRING ( oldName ),
                  PD_PACK_STRING ( newName ) ) ;
      rc = dmsCheckCLName ( oldName, sysCollection ) ;
      PD_RC_CHECK( rc, PDERROR, "Invalid old collection name %s, rc: %d",
                   oldName, rc ) ;
      rc = dmsCheckCLName ( newName, sysCollection ) ;
      PD_RC_CHECK( rc, PDERROR, "Invalid new collection name %s, rc: %d",
                   newName, rc ) ;

      // reserved log-size
      if ( dpscb )
      {
         rc = dpsCLRename2Record( getSuName(), oldName, newName, record ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build log record, rc: %d", rc ) ;

         rc = dpscb->checkSyncControl( record.alignedLen(), cb ) ;
         PD_RC_CHECK( rc, PDERROR, "Check sync control failed, rc: %d", rc ) ;

         logRecSize = record.alignedLen() ;
         rc = pTransCB->reservedLogSpace( logRecSize, cb );
         if( rc )
         {
            PD_LOG( PDERROR, "Failed to reserved log space(length=%u)",
                    logRecSize ) ;
            logRecSize = 0 ;
            goto error ;
         }
      }

      // lock metadata
      ossLatch ( &_metadataLatch, EXCLUSIVE ) ;
      metalocked = TRUE ;

      mbID = _collectionNameLookup( oldName ) ;

      if ( DMS_INVALID_MBID == mbID )
      {
         rc = SDB_DMS_NOTEXIST ;
         goto error ;
      }
      if ( DMS_INVALID_MBID != _collectionNameLookup ( newName ) )
      {
         rc = SDB_DMS_EXIST ;
         goto error ;
      }

      _collectionNameRemove ( oldName ) ;
      _collectionNameInsert ( newName, mbID ) ;
      ossMemset ( _dmsMME->_mbList[mbID]._collectionName, 0,
                  DMS_COLLECTION_NAME_SZ ) ;
      ossStrncpy ( _dmsMME->_mbList[mbID]._collectionName, newName,
                   DMS_COLLECTION_NAME_SZ ) ;
      clLID = _dmsMME->_mbList[mbID]._logicalID ;

      if ( dpscb )
      {
         rc = _logDPS( dpscb, info, cb, &_metadataLatch, EXCLUSIVE, metalocked,
                       clLID, DMS_INVALID_EXTENT ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to insert clrename to log, rc = %d",
                      rc ) ;
      }

   done :
      if ( metalocked )
      {
         ossUnlatch ( &_metadataLatch, EXCLUSIVE ) ;
         metalocked = FALSE ;
      }
      if ( 0 != logRecSize )
      {
         pTransCB->releaseLogSpace( logRecSize, cb );
      }
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_RENAMECOLLECTION, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_FINDCOLLECTION, "_dmsStorageData::findCollection" )
   INT32 _dmsStorageData::findCollection( const CHAR * pName,
                                          UINT16 & collectionID )
   {
      INT32 rc            = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_FINDCOLLECTION ) ;
      PD_TRACE1 ( SDB__DMSSTORAGEDATA_FINDCOLLECTION,
                  PD_PACK_STRING ( pName ) ) ;
      ossLatch ( &_metadataLatch, SHARED ) ;
      collectionID = _collectionNameLookup ( pName ) ;
      PD_TRACE1 ( SDB__DMSSTORAGEDATA_FINDCOLLECTION,
                  PD_PACK_USHORT ( collectionID ) ) ;
      if ( DMS_INVALID_MBID == collectionID )
      {
         rc = SDB_DMS_NOTEXIST ;
         goto error ;
      }

   done :
      ossUnlatch ( &_metadataLatch, SHARED ) ;
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_FINDCOLLECTION, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_INSERTRECORD, "_dmsStorageData::insertRecord" )
   INT32 _dmsStorageData::insertRecord( dmsMBContext *context,
                                        const BSONObj &record, pmdEDUCB *cb,
                                        SDB_DPSCB *dpscb, BOOLEAN mustOID,
                                        BOOLEAN canUnLock )
   {
      INT32 rc                      = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_INSERTRECORD ) ;
      IDToInsert oid ;
      idToInsertEle oidEle((CHAR*)(&oid)) ;
      BOOLEAN addOID                = FALSE ;
      UINT32 oidLen                 = 0 ;
      UINT32 dmsRecordSize          = 0 ;
      CHAR fullName[DMS_COLLECTION_FULL_NAME_SZ + 1] = {0} ;
      dpsTransCB *pTransCB          = pmdGetKRCB()->getTransCB() ;
      UINT32 logRecSize             = 0 ;
      monAppCB * pMonAppCB          = cb ? cb->getMonAppCB() : NULL ;
      dpsMergeInfo info ;
      dpsLogRecord &logRecord       = info.getMergeBlock().record() ;
      SDB_DPSCB *dropDps            = NULL ;
      // compression related
      BOOLEAN isCompressed          = FALSE ;
      const CHAR *compressedData    = NULL ;
      INT32 compressedDataSize      = 0 ;
      // trans related
      DPS_TRANS_ID transID          = cb->getTransID() ;
      DPS_LSN_OFFSET preTransLsn    = cb->getCurTransLsn() ;
      DPS_LSN_OFFSET relatedLsn     = cb->getRelatedTransLSN() ;
      BOOLEAN  isTransLocked        = FALSE ;
      // delete record related
      dmsRecordID foundDeletedID ;
      ossValuePtr extentPtr         = 0 ;
      ossValuePtr deletedRecordPtr  = 0 ;
      ossValuePtr insertedDataPtr   = 0 ;
      BSONObj insertObj ;
      utilCompressor *compressor    = NULL ;
      BOOLEAN compressorReady       = FALSE ;
      BOOLEAN dataModified          = FALSE ;
      utilCompressorContext compressorContext = UTIL_INVALID_COMP_CTX ;
      BOOLEAN dictCompress          = FALSE ;

      /* For concurrency protection with drop CL and set compresor. */
      dmsCompressorGuard compGuard( _compressorEntry[context->mbID()], SHARED ) ;

      // verify whether the record got "_id" inside
      BSONElement ele = record.getField ( DMS_ID_KEY_NAME ) ;
      if ( ele.type() == Array )
      {
         PD_LOG ( PDERROR, "record id can't be array: %s",
                  record.toString().c_str() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      // judge must oid
      if ( mustOID && ele.eoo() )
      {
         oid._oid.init() ;
         oidLen = oidEle.size() ;
         addOID = TRUE ;
         dataModified = TRUE ;
      }
      // check
      if ( record.objsize() + DMS_RECORD_METADATA_SZ + oidLen >
           DMS_RECORD_USER_MAX_SZ )
      {
         rc = SDB_DMS_RECORD_TOO_BIG ;
         goto error ;
      }

      /*
       * Check if the compressor is ready.
       */
      if ( UTIL_COMPRESSOR_LZW ==
                     (UTIL_COMPRESSOR_TYPE)context->mb()->_compressorType )
      {
         /*
          * _dictExtentID invalid means the dictionary has not been created,
          * so the compressor is not ready.
          */
         compressorReady
            = ( DMS_INVALID_EXTENT != context->mb()->_dictExtentID ) ?
              TRUE : FALSE ;
         if ( compressorReady )
         {
            compressor = _compressorEntry[context->mbID()].getCompressor() ;
            if ( compressor )
            {
               rc = compressor->prepare( compressorContext ) ;
               PD_RC_CHECK( rc, PDERROR,
                            "Failed to prepare compressor, rc: %d", rc ) ;
               dictCompress = TRUE ;
            }
         }
      }
      else if ( UTIL_COMPRESSOR_SNAPPY ==
             (UTIL_COMPRESSOR_TYPE)context->mb()->_compressorType )
      {
         compressorReady = TRUE ;
      }
      else
      {
         compressorReady = FALSE ;
      }

      if ( compressorReady )
      {
         rc = dmsCompress( cb, compressor, compressorContext,
                           record, ((CHAR*)(&oid)), oidLen,
                           &compressedData, &compressedDataSize ) ;
         if ( rc )
         {
            // If compression failed, store the record in its original format.
            dmsRecordSize = record.objsize() ;
         }
         else
         {
            // 4 bytes len + compressed record
            dmsRecordSize = compressedDataSize + sizeof(INT32) ;
            PD_TRACE2 ( SDB__DMSSTORAGEDATA_INSERTRECORD,
                        PD_PACK_STRING ( "size after compress" ),
                        PD_PACK_UINT ( dmsRecordSize ) ) ;

            // if we find the record size is greater than non-compression, let's
            // save non-compressed version
            if ( dmsRecordSize > (UINT32)(record.objsize() + oidLen) )
            {
               dmsRecordSize = record.objsize() ;
            }
            else
            {
               // oid is already added into compression buffer, so let's unset
               // addOID stuff
               addOID = FALSE ;
               oidLen = 0 ;
               // set compression flag
               isCompressed = TRUE ;
            }
         }
      }
      else
      {
         // if not compressed, let's use object size
         dmsRecordSize = record.objsize() ;
      }

      /*
       * Release the compressor context and guard to avoid deadlock with
       * truncate/drop collection.
       */
      if ( UTIL_INVALID_COMP_CTX != compressorContext )
      {
         compressor->done( compressorContext ) ;
         compressorContext = UTIL_INVALID_COMP_CTX ;
      }
      compGuard.release() ;

      // add record metadata and oid
      dmsRecordSize += ( DMS_RECORD_METADATA_SZ + oidLen ) ;
      // get the recordsize that we have to allocate
      _overflowSize( dmsRecordSize ) ;
      // record is ALWAYS 4 bytes aligned
      dmsRecordSize = OSS_MIN( DMS_RECORD_MAX_SZ,
                               ossAlignX ( dmsRecordSize, 4 ) ) ;
      PD_TRACE2 ( SDB__DMSSTORAGEDATA_INSERTRECORD,
                  PD_PACK_STRING ( "size after align" ),
                  PD_PACK_UINT ( dmsRecordSize ) ) ;

      // calc log reserve
      if ( dpscb )
      {
         _clFullName( context->mb()->_collectionName, fullName,
                      sizeof(fullName) ) ;
         rc = dpsInsert2Record( fullName, record, transID,
                                preTransLsn, relatedLsn, logRecord ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build record, rc: %d", rc ) ;

         logRecSize = ossAlign4( logRecord.alignedLen() + oidLen ) ;

         rc = dpscb->checkSyncControl( logRecSize, cb ) ;
         if ( SDB_OK != rc )
         {
            logRecSize = 0 ;
            PD_LOG( PDERROR, "Check sync control failed, rc: %d", rc ) ;
            goto error ;
         }

         rc = pTransCB->reservedLogSpace( logRecSize, cb ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Failed to reserved log space(length=%u)",
                    logRecSize ) ;
            logRecSize = 0 ;
            goto error ;
         }
      }

      _registerNewWriting() ;
      // lock mb
      rc = context->mbLock( EXCLUSIVE ) ;
      PD_RC_CHECK( rc, PDERROR, "dms mb context lock failed, rc: %d", rc ) ;

      // then make sure the collection compatiblity
      if ( !dmsAccessAndFlagCompatiblity ( context->mb()->_flag,
                                           DMS_ACCESS_TYPE_INSERT ) )
      {
         PD_LOG ( PDERROR, "Incompatible collection mode: %d",
                  context->mb()->_flag ) ;
         rc = SDB_DMS_INCOMPATIBLE_MODE ;
         goto error ;
      }

      // find delete record
      rc = _reserveFromDeleteList( context, dmsRecordSize,
                                   foundDeletedID, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Reserve delete record failed, rc: %d", rc ) ;

      extentPtr = extentAddr( foundDeletedID._extent ) ;
      if ( !extentPtr || !((dmsExtent*)extentPtr)->validate(context->mbID()) )
      {
         rc = SDB_SYS ;
         goto error ;
      }
      deletedRecordPtr = extentPtr + foundDeletedID._offset ;

      if ( dpscb )
      {
         rc = pTransCB->transLockTryX( cb, _logicalCSID, context->mbID(),
                                       &foundDeletedID ) ;
         SDB_ASSERT( SDB_OK == rc, "Failed to get record-X-LOCK" );
         PD_RC_CHECK( rc, PDERROR, "Failed to insert the record, get "
                     "transaction-X-lock of record failed, rc: %d", rc );
         isTransLocked = TRUE ;
      }
      // insert to extent
      rc = _extentInsertRecord( context, deletedRecordPtr, dmsRecordSize,
                                isCompressed ? (ossValuePtr)compressedData :
                                               (ossValuePtr)record.objdata(),
                                isCompressed ?  compressedDataSize :
                                                record.objsize(),
                                foundDeletedID._extent,
                                addOID ? &oidEle : NULL, cb, isCompressed,
                                TRUE ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to append record, rc: %d", rc ) ;

      // update totalInsert monitor counter
      DMS_MON_OP_COUNT_INC( pMonAppCB, MON_INSERT, 1 ) ;

      // mark the segment dirty
      _markDirty ( foundDeletedID._extent ) ;

      // we have to create new object from deletedRecordPtr instead of using
      // record object, because we may have to add OID into the object
      {
         if ( dataModified )
         {
            if ( dictCompress )
            {
               rc = compressor->prepare( compressorContext ) ;
               PD_RC_CHECK( rc, PDERROR,
                            "Failed to prepare compressor, rc: %d", rc ) ;
            }

            DMS_RECORD_EXTRACTDATA( compressor, compressorContext,
                                    deletedRecordPtr, insertedDataPtr ) ;
            if ( compressorContext )
            {
               compressor->done( compressorContext ) ;
               compressorContext = UTIL_INVALID_COMP_CTX ;
            }
            insertObj = BSONObj( ( const CHAR* )insertedDataPtr ) ;
            DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_READ, 1 ) ;
            DMS_MON_OP_COUNT_INC( pMonAppCB, MON_READ, 1 ) ;
         }
         else
         {
            insertedDataPtr = (ossValuePtr)record.objdata() ;
            insertObj = record ;
         }
         rc = _pIdxSU->indexesInsert( context,
                                      ((dmsExtent*)extentPtr)->_logicID,
                                      insertObj, foundDeletedID, cb ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to insert to index, rc: %d", rc ) ;
      }

      if ( dpscb )
      {
         PD_AUDIT_OP_WITHNAME( AUDIT_INSERT, "INSERT", AUDIT_OBJ_CL,
                               fullName, rc, "%s",
                               insertObj.toString().c_str() ) ;

         dmsExtentID extLID = ((dmsExtent*)extentPtr)->_logicID ;
         info.clear() ;

         rc = dpsInsert2Record( fullName, insertObj, transID,
                                preTransLsn, relatedLsn, logRecord ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build insert record, rc: %d",
                      rc ) ;

         rc = _logDPS( dpscb, info, cb, context, extLID, canUnLock ) ;
         PD_RC_CHECK ( rc, PDERROR, "Failed to insert record into log, rc: %d",
                       rc ) ;
         dropDps = dpscb ;
         // it is transaction operations
         if ( cb && transID != DPS_INVALID_TRANS_ID )
         {
            cb->setCurTransLsn( info.getMergeBlock().record().head()._lsn ) ;
            if ( pmdGetKRCB()->getTransCB()->isFirstOp( transID ))
            {
               pmdGetKRCB()->getTransCB()->clearFirstOpTag( transID );
               cb->setTransID( transID ) ;
            }
         }
      }

   done:
      if ( UTIL_INVALID_COMP_CTX != compressorContext )
      {
         compressor->done( compressorContext ) ;
      }

      // release the lock immediately if it is not transaction-operation,
      // the transaction-operation's lock will release in rollback or commit
      if ( isTransLocked && ( transID == DPS_INVALID_TRANS_ID || rc ) )
      {
         pTransCB->transLockRelease( cb, _logicalCSID, context->mbID(),
                                     &foundDeletedID ) ;
      }
      if ( 0 != logRecSize )
      {
         pTransCB->releaseLogSpace( logRecSize, cb ) ;
      }
      if ( SDB_OK == rc && NULL != cb )
      {
         _updateLastLSN( cb->getEndLsn() ) ;
      }

      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_INSERTRECORD, rc ) ;
      return rc ;
   error:
      if ( 0 != insertedDataPtr )
      {
         INT32 rc1 = deleteRecord( context, foundDeletedID, insertedDataPtr,
                                   cb, dropDps ) ;
         if ( rc1 )
         {
            PD_LOG( PDERROR, "Failed to rollback, rc: %d", rc1 ) ;
         }
      }
      else if ( foundDeletedID.isValid() )
      {
         _saveDeletedRecord( context->mb(), foundDeletedID, 0 ) ;
      }
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__EXTENTINSERTRECORD, "_dmsStorageData::_extentInsertRecord" )
   INT32 _dmsStorageData::_extentInsertRecord( dmsMBContext *context,
                                               ossValuePtr deletedRecordPtr,
                                               UINT32 dmsRecordSize,
                                               ossValuePtr ptr,
                                               INT32 len, INT32 extentID,
                                               BSONElement *extraOID,
                                               pmdEDUCB *cb,
                                               BOOLEAN compressed,
                                               BOOLEAN addIntoList )
   {
      INT32 rc                         = SDB_OK ;
      dmsOffset deletedRecordOffset    = DMS_INVALID_OFFSET ;
      dmsExtent *extent                = NULL ;
      monAppCB * pMonAppCB             = cb ? cb->getMonAppCB() : NULL ;

      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__EXTENTINSERTRECORD ) ;
      rc = context->mbLock( EXCLUSIVE ) ;
      PD_RC_CHECK( rc, PDERROR, "dms mb context lock failed, rc: %d", rc ) ;

      // first we need to check if the delete record is large enough
      if ( DMS_DELETEDRECORD_GETSIZE ( deletedRecordPtr ) < dmsRecordSize )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      else if ( len < 5 )
      {
         PD_LOG( PDERROR, "Bson obj size[%d] is invalid", len ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      deletedRecordOffset = DMS_DELETEDRECORD_GETMYOFFSET( deletedRecordPtr ) ;
      extent = (dmsExtent*)( deletedRecordPtr - deletedRecordOffset ) ;

      // set to normal status
      DMS_RECORD_SETSTATE ( deletedRecordPtr, DMS_RECORD_FLAG_NORMAL ) ;
      DMS_RECORD_RESETATTR ( deletedRecordPtr ) ;
      // if the record is compressed, let's set the flag
      if ( compressed )
      {
         DMS_RECORD_SETATTR ( deletedRecordPtr, DMS_RECORD_FLAG_COMPRESSED ) ;
      }

      // and then need to check if we need to split deleted record
      if ( (DMS_DELETEDRECORD_GETSIZE( deletedRecordPtr ) - dmsRecordSize) >
           DMS_MIN_RECORD_SZ )
      {
         // original offset+new size = new delete offset
         dmsOffset newOffset = DMS_DELETEDRECORD_GETMYOFFSET(deletedRecordPtr) +
                               dmsRecordSize ;
         // original size - new size = new delete size
         INT32 newSize = DMS_DELETEDRECORD_GETSIZE(deletedRecordPtr) -
                         dmsRecordSize ;
         rc = _saveDeletedRecord ( context->mb(), extent, newOffset, newSize,
                                   extentID ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to save deleted record, rc: %d", rc ) ;
            goto error ;
         }
         // set the original place with new dmsrecordSize
         DMS_RECORD_SETSIZE ( deletedRecordPtr, dmsRecordSize ) ;
      }
      // if the leftover space is not good enough for a min_record, then we
      // don't change the deletedRecordPtr size

      // then for the original location we set new record header and copy data
      if ( NULL == extraOID )
      {
         DMS_RECORD_SETDATA ( deletedRecordPtr, ptr, len ) ;
      }
      else
      {
         DMS_RECORD_SETDATA_OID ( deletedRecordPtr, ptr, len, (*extraOID) ) ;
      }
      DMS_RECORD_SETNEXTOFFSET ( deletedRecordPtr, DMS_INVALID_OFFSET ) ;
      DMS_RECORD_SETPREVOFFSET ( deletedRecordPtr, DMS_INVALID_OFFSET ) ;
      // increase write counter
      DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_WRITE, 1 ) ;

      // no need to change offset
      if ( addIntoList )
      {
         dmsOffset   offset      = extent->_lastRecordOffset ;
         // finally add the record into list
         extent->_recCount++ ;
         ++( _mbStatInfo[ context->mbID() ]._totalRecords ) ;
         // if there is last record in the extent
         if ( DMS_INVALID_OFFSET != offset )
         {
            // if there is already record in the extent
            ossValuePtr preRecord = ((ossValuePtr)extent + offset) ;
            // set the next of previous point to the new record
            DMS_RECORD_SETNEXTOFFSET ( preRecord, deletedRecordOffset ) ;
            // set the previous of current points to the original last
            DMS_RECORD_SETPREVOFFSET ( deletedRecordPtr, offset ) ;
         }
         extent->_lastRecordOffset = deletedRecordOffset ;
         // then check for first record in extent
         if ( DMS_INVALID_OFFSET == extent->_firstRecordOffset )
         {
            // we only change it when it points to nothing
            extent->_firstRecordOffset = deletedRecordOffset ;
         }
      }

   done :
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__EXTENTINSERTRECORD, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_DELETERECORD, "_dmsStorageData::deleteRecord" )
   INT32 _dmsStorageData::deleteRecord( dmsMBContext *context,
                                        const dmsRecordID &recordID,
                                        ossValuePtr deletedDataPtr,
                                        pmdEDUCB *cb,
                                        SDB_DPSCB * dpscb )
   {
      INT32 rc                      = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_DELETERECORD ) ;
      ossValuePtr extentPtr         = extentAddr ( recordID._extent ) ;
      ossValuePtr recordPtr         = extentPtr + recordID._offset ;
      ossValuePtr realPtr           = recordPtr ;
      dpsTransCB *pTransCB          = pmdGetKRCB()->getTransCB() ;
      monAppCB * pMonAppCB          = cb ? cb->getMonAppCB() : NULL ;
      BOOLEAN isDeleting            = FALSE ;
      BOOLEAN isNeedToSetDeleting   = FALSE ;

      dmsRecordID ovfRID ;
      BSONObj delObject ;
      UINT32 logRecSize             = 0 ;
      dpsMergeInfo info ;
      dpsLogRecord &record          = info.getMergeBlock().record() ;
      CHAR fullName[DMS_COLLECTION_FULL_NAME_SZ + 1] = {0} ;
      DPS_TRANS_ID transID          = cb->getTransID() ;
      DPS_LSN_OFFSET preLsn         = cb->getCurTransLsn() ;
      DPS_LSN_OFFSET relatedLSN     = cb->getRelatedTransLSN() ;
      CHAR recordState              = 0 ;
      utilCompressor *compressor    = NULL ;
      utilCompressorContext compContext = UTIL_INVALID_COMP_CTX ;

      if ( !extentPtr )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      _registerNewWriting() ;
      if ( !context->isMBLock( EXCLUSIVE ) )
      {
         PD_LOG( PDERROR, "Caller must hold mb exlusive lock[%s]",
                 context->toString().c_str() ) ;
         rc = SDB_SYS ;
         goto error ;
      }

#ifdef _DEBUG
      if ( !dmsAccessAndFlagCompatiblity ( context->mb()->_flag,
                                           DMS_ACCESS_TYPE_DELETE ) )
      {
         PD_LOG ( PDERROR, "Incompatible collection mode: %d",
                  context->mb()->_flag ) ;
         rc = SDB_DMS_INCOMPATIBLE_MODE ;
         goto error ;
      }
#endif //_DEBUG

      recordState = DMS_RECORD_GETSTATE(recordPtr) ;
      if ( DMS_RECORD_FLAG_OVERFLOWF == recordState )
      {
         ovfRID = DMS_RECORD_GETOVF(recordPtr) ;
         DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_READ, 1 ) ;
         realPtr = extentAddr(ovfRID._extent) + ovfRID._offset ;
         SDB_ASSERT( DMS_RECORD_FLAG_OVERFLOWT == DMS_RECORD_GETSTATE(realPtr),
                     "ovf record must be over flow to" ) ;
      }
      else if ( DMS_RECORD_FLAG_OVERFLOWT == recordState )
      {
         _extentRemoveRecord( context->mb(), recordID, 0, cb ) ;
         goto done ;
      }
      else if ( DMS_RECORD_FLAG_DELETED == recordState )
      {
         rc = SDB_DMS_RECORD_NOTEXIST ;
         goto error ;
      }
      else if ( DMS_RECORD_FLAG_NORMAL != recordState )
      {
         rc = SDB_SYS ;
         goto error ;
      }

      // don't delete the record, someone are waitting for the record-X-Lock,
      // mark the record's attr to DMS_RECORD_FLAG_DELETING and write the log
      // the last one who get the record-X-Lock will delete the record.
      if ( pTransCB->hasWait( _logicalCSID, context->mbID(), &recordID ) )
      {
         if ( OSS_BIT_TEST ( DMS_RECORD_FLAG_DELETING,
                             DMS_RECORD_GETATTR(recordPtr)) )
         {
            rc = SDB_OK ;
            goto done ;
         }
         isNeedToSetDeleting = TRUE ;
      }
      else if ( OSS_BIT_TEST ( DMS_RECORD_FLAG_DELETING,
                               DMS_RECORD_GETATTR(recordPtr)) )
      {
         isDeleting = TRUE ;
      }

      if ( FALSE == isDeleting ) // first delete the record
      {
         if ( !deletedDataPtr )
         {
            if ( DMS_INVALID_EXTENT != context->mb()->_dictExtentID )
            {
               compressor =  _compressorEntry[context->mbID()].getCompressor() ;
               if ( compressor )
               {
                  rc = compressor->prepare( compContext ) ;
                  PD_RC_CHECK( rc, PDERROR,
                               "Failed to prepare compressor, rc: %d", rc ) ;
               }
            }
            DMS_RECORD_EXTRACTDATA ( compressor, compContext,
                                     realPtr, deletedDataPtr ) ;
            if ( UTIL_INVALID_COMP_CTX != compContext )
            {
               compressor->done( compContext ) ;
               compContext = UTIL_INVALID_COMP_CTX ;
            }

            DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_READ, 1 ) ;
            DMS_MON_OP_COUNT_INC( pMonAppCB, MON_READ, 1 ) ;
         }
         // delete index keys
         try
         {
            delObject = BSONObj( (CHAR*)deletedDataPtr ) ;
            // first to reserve dps
            if ( NULL != dpscb )
            {
               _clFullName( context->mb()->_collectionName, fullName,
                            sizeof(fullName) ) ;
               // reserved log-size
               rc = dpsDelete2Record( fullName, delObject, transID,
                                      preLsn, relatedLSN, record ) ;
               if ( SDB_OK != rc )
               {
                  PD_LOG( PDERROR, "Failed to build record: %d",rc ) ;
                  goto error ;
               }

               rc = dpscb->checkSyncControl( record.alignedLen(), cb ) ;
               PD_RC_CHECK( rc, PDERROR, "Check sync control failed, rc: %d",
                            rc ) ;

               logRecSize = record.alignedLen() ;
               rc = pTransCB->reservedLogSpace( logRecSize, cb ) ;
               if ( rc )
               {
                  PD_LOG( PDERROR, "Failed to reserved log space(length=%u)",
                          logRecSize ) ;
                  logRecSize = 0 ;
                  goto error ;
               }
            }
            // then delete indexes
            rc = _pIdxSU->indexesDelete( context, recordID._extent,
                                         delObject, recordID, cb ) ;
            if ( rc )
            {
               // if index delete fail, let's continue remove the record
               PD_LOG ( PDERROR, "Failed to delete indexes, rc: %d",rc ) ;
            }
         }
         catch ( std::exception &e )
         {
            PD_LOG ( PDERROR, "Corrupted record: %d:%d: %s",
                     recordID._extent, recordID._offset, e.what() ) ;
            rc = SDB_CORRUPTED_RECORD ;
            goto error ;
         }
      }

      // no wait for X lock, delete really
      if ( FALSE == isNeedToSetDeleting )
      {
         rc = _extentRemoveRecord( context->mb(), recordID, 0, cb,
                                   !isDeleting ) ;
         PD_RC_CHECK( rc, PDERROR, "Extent remove record failed, rc: %d", rc ) ;
         if ( ovfRID.isValid() )
         {
            _extentRemoveRecord( context->mb(), ovfRID, 0, cb ) ;
         }
      }
      // set deleting attr
      else
      {
         DMS_RECORD_SETATTR( recordPtr, DMS_RECORD_FLAG_DELETING ) ;
         // need to dec count
         --((dmsExtent*)extentPtr)->_recCount ;
         --( _mbStatInfo[ context->mbID() ]._totalRecords ) ;
      }

      if ( FALSE == isDeleting )
      {
         // increase conter
         DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DELETE, 1 ) ;
      }

      // if we are asked to log
      if ( dpscb && FALSE == isDeleting )
      {
         try
         {
            PD_AUDIT_OP_WITHNAME( AUDIT_DELETE, "DELETE", AUDIT_OBJ_CL,
                                  fullName, rc, "%s",
                                  BSONObj( (CHAR*)deletedDataPtr ).toString().c_str() ) ;
         }
         catch( std::exception &e )
         {
            PD_LOG( PDERROR, "Failed to audit delete record: %s", e.what() ) ;
            /// ignore the error
         }

         dmsExtentID extLID = ((dmsExtent*)extentPtr)->_logicID ;
         rc = _logDPS( dpscb, info, cb, context, extLID, FALSE ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to insert record into log, rc: %d",
                      rc ) ;

         // it is transaction operations
         if ( cb && transID != DPS_INVALID_TRANS_ID )
         {
            cb->setCurTransLsn( info.getMergeBlock().record().head()._lsn );
            if ( pTransCB->isFirstOp( transID ))
            {
               pTransCB->clearFirstOpTag( transID ) ;
               cb->setTransID( transID ) ;
            }
         }
      }

   done :
      if ( UTIL_INVALID_COMP_CTX != compContext )
      {
         compressor->done( compContext ) ;
      }

      if ( 0 != logRecSize )
      {
         pTransCB->releaseLogSpace( logRecSize, cb ) ;
      }
      if ( SDB_OK == rc && NULL != cb )
      {
         _updateLastLSN( cb->getEndLsn() ) ;
      }
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_DELETERECORD, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__EXTENTREMOVERECORD, "_dmsStorageData::_extentRemoveRecord" )
   INT32 _dmsStorageData::_extentRemoveRecord( dmsMB *mb,
                                               const dmsRecordID &recordID,
                                               INT32 recordSize,
                                               pmdEDUCB * cb,
                                               BOOLEAN decCount )
   {
      INT32 rc              = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__EXTENTREMOVERECORD ) ;
      monAppCB * pMonAppCB  = cb ? cb->getMonAppCB() : NULL ;
      ossValuePtr extentPtr = extentAddr( recordID._extent ) ;
      ossValuePtr recordPtr = extentPtr + recordID._offset ;

      // not over-flow to record
      if ( DMS_RECORD_FLAG_OVERFLOWT != DMS_RECORD_GETSTATE(recordPtr) )
      {
         dmsExtent *extent = (dmsExtent*)extentPtr ;
         dmsOffset prevRecordOffset = DMS_RECORD_GETPREVOFFSET(recordPtr) ;
         dmsOffset nextRecordOffset = DMS_RECORD_GETNEXTOFFSET(recordPtr) ;

         if ( DMS_INVALID_OFFSET != prevRecordOffset )
         {
            DMS_RECORD_SETNEXTOFFSET ( extentPtr+prevRecordOffset,
                                       nextRecordOffset ) ;
         }
         if ( DMS_INVALID_OFFSET != nextRecordOffset )
         {
            DMS_RECORD_SETPREVOFFSET ( extentPtr+nextRecordOffset,
                                       prevRecordOffset ) ;
         }
         if ( extent->_firstRecordOffset == DMS_RECORD_GETMYOFFSET(recordPtr) )
         {
            extent->_firstRecordOffset = nextRecordOffset ;
         }
         if ( extent->_lastRecordOffset == DMS_RECORD_GETMYOFFSET(recordPtr) )
         {
            extent->_lastRecordOffset = prevRecordOffset ;
         }

         if ( decCount )
         {
            --(extent->_recCount) ;
            --( _mbStatInfo[ mb->_blockID ]._totalRecords ) ;
         }
      }
      //increase data write counter
      DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_WRITE, 1 ) ;

      // mark as dirty
      _markDirty ( recordID._extent ) ;

      rc = _saveDeletedRecord( mb, recordID, recordSize ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to save deleted record, rc = %d", rc ) ;
         goto error ;
      }
   done :
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__EXTENTREMOVERECORD, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_UPDATERECORD, "_dmsStorageData::updateRecord" )
   INT32 _dmsStorageData::updateRecord( dmsMBContext *context,
                                        const dmsRecordID &recordID,
                                        ossValuePtr updatedDataPtr,
                                        pmdEDUCB *cb, SDB_DPSCB *dpscb,
                                        mthModifier &modifier,
                                        BSONObj* newRecord )
   {
      INT32 rc                      = SDB_OK ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_UPDATERECORD ) ;
      ossValuePtr extentPtr         = 0 ;
      dmsExtent *extent             = NULL ;
      monAppCB * pMonAppCB          = cb ? cb->getMonAppCB() : NULL ;
      BSONObj oldMatch, oldChg ;
      BSONObj newMatch, newChg ;
      UINT32 logRecSize             = 0 ;
      dpsMergeInfo info ;
      dpsLogRecord &record = info.getMergeBlock().record() ;
      dpsTransCB *pTransCB = pmdGetKRCB()->getTransCB() ;
      CHAR fullName[DMS_COLLECTION_FULL_NAME_SZ + 1] = {0} ;
      DPS_TRANS_ID transID = cb->getTransID() ;
      DPS_LSN_OFFSET preTransLsn = cb->getCurTransLsn() ;
      DPS_LSN_OFFSET relatedLSN = cb->getRelatedTransLSN() ;
      utilCompressorContext compContext = UTIL_INVALID_COMP_CTX ;
      utilCompressor *compressor = NULL ;

      _registerNewWriting() ;
      // first we need to locate the mem addr of the extent
      extentPtr = extentAddr ( recordID._extent ) ;
      if ( !extentPtr )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      extent = ( dmsExtent* )extentPtr ;

      if ( !context->isMBLock( EXCLUSIVE ) )
      {
         PD_LOG( PDERROR, "Caller must hold mb exclusive lock[%s]",
                 context->toString().c_str() ) ;
         rc = SDB_SYS ;
         goto error ;
      }

#ifdef _DEBUG
      if ( !dmsAccessAndFlagCompatiblity ( context->mb()->_flag,
                                           DMS_ACCESS_TYPE_UPDATE ) )
      {
         PD_LOG ( PDERROR, "Incompatible collection mode: %d",
                  context->mb()->_flag ) ;
         rc = SDB_DMS_INCOMPATIBLE_MODE ;
         goto error ;
      }
      // validate the extent is valid
      if ( !extent->validate( context->mbID() ) )
      {
         rc = SDB_SYS ;
         goto error ;
      }
#endif //_DEBUG

      // get data
      if ( 0 == updatedDataPtr )
      {
         ossValuePtr recordPtr = extentPtr+recordID._offset ;
         ossValuePtr recordRealPtr = recordPtr ;
         if ( DMS_RECORD_FLAG_OVERFLOWF == DMS_RECORD_GETSTATE(recordPtr) )
         {
            dmsRecordID ovfRID = DMS_RECORD_GETOVF(recordPtr) ;
            DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_READ, 1 ) ;
            ossValuePtr ovfExtentPtr = extentAddr ( ovfRID._extent ) ;
            recordRealPtr = ovfExtentPtr + ovfRID._offset ;
         }

         if ( DMS_INVALID_EXTENT != context->mb()->_dictExtentID )
         {
            compressor = _compressorEntry[context->mbID()].getCompressor() ;
            if ( compressor )
            {
               rc = compressor->prepare( compContext ) ;
               PD_RC_CHECK( rc, PDERROR,
                            "Failed to prepare compressor, rc: %d", rc ) ;
            }
         }
         DMS_RECORD_EXTRACTDATA( compressor, compContext,
                                 recordRealPtr, updatedDataPtr ) ;
         if ( UTIL_INVALID_COMP_CTX != compContext )
         {
            compressor->done( compContext ) ;
            compContext = UTIL_INVALID_COMP_CTX ;
         }

         DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_READ, 1 ) ;
         DMS_MON_OP_COUNT_INC( pMonAppCB, MON_READ, 1 ) ;
      }

      try
      {
         BSONObj obj ( (const CHAR*)updatedDataPtr ) ;
         // create a new object for updated record
         BSONObj newobj ;
         try
         {
            if ( dpscb )
            {
               rc = modifier.modify ( obj, newobj, &oldMatch, &oldChg,
                                      &newMatch, &newChg ) ;
               if ( SDB_OK == rc && newChg.isEmpty() )
               {
                  SDB_ASSERT( oldChg.isEmpty(), "Old change must be empty" ) ;
                  goto done ;
               }
            }
            else
            {
               rc = modifier.modify ( obj, newobj ) ;
            }

            if ( rc )
            {
               PD_LOG ( PDERROR, "Failed to create modified record, rc: %d",
                        rc ) ;
               goto error ;
            }
         }
         catch ( std::exception &e )
         {
            PD_LOG ( PDERROR, "Exception happened while trying to update "
                     "record: %s", e.what() ) ;
            rc = SDB_SYS ;
            goto error ;
         }

         if ( NULL != dpscb )
         {
            _clFullName( context->mb()->_collectionName, fullName,
                         sizeof(fullName) ) ;
            // reserved log-size
            rc = dpsUpdate2Record( fullName, oldMatch, oldChg, newMatch,
                                   newChg, transID, preTransLsn,
                                   relatedLSN, record ) ;
            if ( SDB_OK != rc )
            {
               PD_LOG( PDERROR, "failed to build record:%d", rc ) ;
               goto error ;
            }

            rc = dpscb->checkSyncControl( record.alignedLen(), cb ) ;
            PD_RC_CHECK( rc, PDERROR, "Check sync control failed, rc: %d",
                         rc ) ;

            logRecSize = record.alignedLen() ;
            rc = pTransCB->reservedLogSpace( logRecSize, cb );
            if ( rc )
            {
               PD_LOG( PDERROR, "Failed to reserved log space(len:%u), rc: %d",
                       logRecSize, rc ) ;
               logRecSize = 0 ;
               goto error ;
            }
         }

         rc = _extentUpdatedRecord( context, recordID, updatedDataPtr,
                                    extent->_logicID,
                                    (ossValuePtr)newobj.objdata(),
                                    newobj.objsize(), cb) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to update record, rc: %d", rc ) ;
            goto error ;
         }

         if ( NULL != newRecord )
         {
            *newRecord = newobj.getOwned() ;
         }
      }
      catch ( std::exception &e )
      {
         PD_LOG ( PDERROR, "Failed to create BSON object: %s", e.what() ) ;
         rc = SDB_CORRUPTED_RECORD ;
         goto error ;
      }

      // increase update counter
      DMS_MON_OP_COUNT_INC( pMonAppCB, MON_UPDATE, 1 ) ;

      // log update information
      if ( dpscb )
      {
         PD_LOG ( PDDEBUG, "oldChange: %s,%s\nnewChange: %s,%s",
                  oldMatch.toString().c_str(),
                  oldChg.toString().c_str(),
                  newMatch.toString().c_str(),
                  newChg.toString().c_str() ) ;

         PD_AUDIT_OP_WITHNAME( AUDIT_UPDATE, "UPDATE", AUDIT_OBJ_CL,
                               fullName, rc, "OldMatch:%s, OldChange:%s, "
                               "NewMatch:%s, NewChange:%s",
                               oldMatch.toString().c_str(),
                               oldChg.toString().c_str(),
                               newMatch.toString().c_str(),
                               newChg.toString().c_str() ) ;

         rc = _logDPS( dpscb, info, cb, context, extent->_logicID, FALSE ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to insert update record into log, "
                      "rc: %d", rc ) ;

         // it is transaction operations
         if ( cb && transID != DPS_INVALID_TRANS_ID )
         {
            cb->setCurTransLsn( info.getMergeBlock().record().head()._lsn ) ;
            if ( pTransCB->isFirstOp( transID ))
            {
               pTransCB->clearFirstOpTag( transID ) ;
               cb->setTransID( transID ) ;
            }
         }
      }

   done :
      if ( UTIL_INVALID_COMP_CTX != compContext )
      {
         compressor->done( compContext ) ;
      }

      if ( 0 != logRecSize )
      {
         pTransCB->releaseLogSpace( logRecSize, cb );
      }
      if ( SDB_OK == rc && NULL != cb )
      {
         _updateLastLSN( cb->getEndLsn() ) ;
      }
      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_UPDATERECORD, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA__EXTENTUPDATERECORD, "_dmsStorageData::_extentUpdatedRecord" )
   INT32 _dmsStorageData::_extentUpdatedRecord( dmsMBContext *context,
                                                const dmsRecordID &recordID,
                                                ossValuePtr recordDataPtr,
                                                dmsExtentID extLID,
                                                ossValuePtr ptr,
                                                INT32 len,
                                                pmdEDUCB *cb )
   {
      INT32 rc                     = SDB_OK ;
      ossValuePtr recordPtr        = 0 ;
      ossValuePtr realRecordPtr    = 0 ;
      UINT32 dmsRecordSize         = 0 ;
      INT32 compressedDataSize     = 0 ;
      const CHAR *compressedData   = NULL ;
      BOOLEAN isCompressed         = FALSE ;
      dmsRecordID ovfRID ;
      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA__EXTENTUPDATERECORD ) ;
      monAppCB * pMonAppCB         = cb ? cb->getMonAppCB() : NULL ;
      utilCompressorContext compContext = UTIL_INVALID_COMP_CTX ;
      dmsCompressorEntry *compEntry = &_compressorEntry[context->mbID()] ;
      utilCompressor *compressor   = NULL ;
      BOOLEAN needCompress         = FALSE ;

      SDB_ASSERT ( 0 != recordDataPtr, "recordDataPtr can't be NULL" ) ;

      if ( len + DMS_RECORD_METADATA_SZ > DMS_RECORD_USER_MAX_SZ )
      {
         PD_LOG ( PDERROR, "record is too big: %d", len ) ;
         rc = SDB_DMS_RECORD_TOO_BIG ;
         goto error ;
      }
      recordPtr = extentAddr(recordID._extent) + recordID._offset ;
      realRecordPtr = recordPtr ;

      if ( !context->isMBLock( EXCLUSIVE ) )
      {
         rc = SDB_SYS ;
         PD_LOG( PDERROR, "Caller must hold exclusive lock[%s]",
                 context->toString().c_str() ) ;
         goto error ;
      }

      // compress data
      if ( OSS_BIT_TEST(context->mb()->_attributes, DMS_MB_ATTR_COMPRESSED ) )
      {
         if ( DMS_INVALID_EXTENT != context->mb()->_dictExtentID )
         {
            compressor = compEntry->getCompressor() ;
            if ( compressor )
            {
               rc = compressor->prepare( compContext ) ;
               PD_RC_CHECK( rc, PDERROR,
                            "Failed to prepare compressor, rc: %d", rc ) ;
               needCompress = TRUE ;
            }
         }
         else if ( UTIL_COMPRESSOR_SNAPPY ==
              (UTIL_COMPRESSOR_TYPE)context->mb()->_compressorType )
         {
            needCompress = TRUE ;
         }
         else
         {
            needCompress = FALSE ;
         }

         if ( needCompress )
         {
            rc = dmsCompress( cb, compressor, compContext, (const CHAR*)ptr,
                              len, &compressedData, &compressedDataSize ) ;
            if ( UTIL_INVALID_COMP_CTX != compContext )
            {
               compressor->done( compContext ) ;
               compContext = UTIL_INVALID_COMP_CTX ;
            }

            if ( rc )
            {
                dmsRecordSize = (UINT32)len ;
            }
            else
            {
               dmsRecordSize = compressedDataSize + sizeof(INT32) ;
               // if we find the record size is greater than non-compression,
               // let's save non-compressed version
               if ( dmsRecordSize > (UINT32)len )
               {
                  dmsRecordSize = (UINT32)len ;
               }
               else
               {
                  isCompressed = TRUE ;
               }
            }
         }
         else
         {
            dmsRecordSize = (UINT32)len ;
         }
      }
      else
      {
         // if not compressed, let's use original size
         dmsRecordSize = len ;
      }

      // add metadata to size
      dmsRecordSize += DMS_RECORD_METADATA_SZ ;
      {
         // before moving on, let's first make sure the new object doesn't
         // violate any index unique rule
         BSONObj oriObj( (CHAR*)recordDataPtr ) ;
         BSONObj newObj( (CHAR*)ptr ) ;
         rc = _pIdxSU->indexesUpdate( context, extLID, oriObj, newObj,
                                      recordID, cb, FALSE ) ;
         if ( rc )
         {
            PD_LOG ( PDWARNING, "Failed to update index, rc: %d", rc ) ;
            goto error_rollbackindex ;
         }
      }

      if ( DMS_RECORD_FLAG_OVERFLOWF == DMS_RECORD_GETSTATE(recordPtr) )
      {
         ovfRID = DMS_RECORD_GETOVF( recordPtr ) ;
         DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_READ, 1 ) ;
         realRecordPtr = extentAddr(ovfRID._extent) + ovfRID._offset ;
      }

      // if the current space is big enough for the whole record, let's put it
      // here and return rightaway
      if ( dmsRecordSize <= DMS_RECORD_GETSIZE(realRecordPtr) )
      {
         // unset compression flag if we decided not to compress the new record
         if ( isCompressed )
         {
            DMS_RECORD_SETATTR ( realRecordPtr, DMS_RECORD_FLAG_COMPRESSED ) ;
            DMS_RECORD_SETDATA ( realRecordPtr, compressedData,
                                 compressedDataSize ) ;
         }
         else
         {
            DMS_RECORD_UNSETATTR ( realRecordPtr, DMS_RECORD_FLAG_COMPRESSED ) ;
            DMS_RECORD_SETDATA ( realRecordPtr, ptr, len ) ;
         }
         DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_WRITE, 1 ) ;
         // mark as dirty
         _markDirty ( recordID._extent ) ;
         goto done ;
      }
      // over-flow recrod
      else
      {
         dmsRecordID foundDeletedID ;
         ossValuePtr extentPtr        = 0 ;
         ossValuePtr deletedRecordPtr = 0 ;

         // get the recordsize that we have to allocate
         _overflowSize( dmsRecordSize ) ;
         // record is ALWAYS 4 bytes aligned
         dmsRecordSize = OSS_MIN( DMS_RECORD_MAX_SZ,
                                  ossAlignX ( dmsRecordSize, 4 ) ) ;

         // find a free spot from delete list
         rc = _reserveFromDeleteList ( context, dmsRecordSize,
                                       foundDeletedID, cb ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Failed to reserve delete record, rc: %d", rc ) ;
            goto error_rollbackindex ;
         }
         extentPtr = extentAddr(foundDeletedID._extent) ;
         if ( 0 == extentPtr )
         {
            PD_LOG( PDERROR, "Found non-exist extent[%d:%d]",
                    foundDeletedID._extent, foundDeletedID._offset ) ;
            rc = SDB_SYS ;
            goto error_rollbackindex ;
         }
         if ( !((dmsExtent*)extentPtr)->validate(context->mbID()))
         {
            PD_LOG ( PDERROR, "Invalid extent[%d] is detected",
                     foundDeletedID._extent ) ;
            rc = SDB_SYS ;
            goto error_rollbackindex ;
         }
         deletedRecordPtr = extentPtr+foundDeletedID._offset ;
         // pass FALSE to addIntoList so that we don't add the record into
         // target extent's list
         rc = _extentInsertRecord ( context, deletedRecordPtr, dmsRecordSize,
                                    isCompressed?(ossValuePtr)compressedData:
                                                 (ossValuePtr)ptr,
                                    isCompressed?compressedDataSize:len,
                                    foundDeletedID._extent, NULL, cb,
                                    isCompressed, FALSE ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to append record due to %d", rc ) ;
            goto error_rollbackindex ;
         }
         // set remote record as overflowed to
         DMS_RECORD_SETSTATE ( deletedRecordPtr, DMS_RECORD_FLAG_OVERFLOWT ) ;
         DMS_RECORD_SETSTATE ( recordPtr, DMS_RECORD_FLAG_OVERFLOWF ) ;
         DMS_RECORD_SETOVF ( recordPtr, foundDeletedID ) ;
         if ( ovfRID.isValid() )
         {
            // overflowed record removal is done here, and it will mark the
            // segment dirty in the function
            _extentRemoveRecord( context->mb(), ovfRID, 0, cb ) ;
         }
         // mark the segment as dirty
         _markDirty ( foundDeletedID._extent ) ;
      }

   done :
      if ( UTIL_INVALID_COMP_CTX != compContext )
      {
         compressor->done( compContext ) ;
      }

      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA__EXTENTUPDATERECORD, rc ) ;
      return rc ;
   error :
      goto done ;
   error_rollbackindex :
      BSONObj oriObj( (CHAR*)recordDataPtr ) ;
      BSONObj newObj( (CHAR*)ptr ) ;
      // rollback the change on index by switching obj and oriObj
      INT32 rc1 = _pIdxSU->indexesUpdate( context, extLID, newObj, oriObj,
                                          recordID, cb, TRUE ) ;
      if ( rc1 )
      {
         PD_LOG ( PDERROR, "Failed to rollback update due to rc %d", rc1 ) ;
      }
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_FETCH, "_dmsStorageData::fetch" )
   INT32 _dmsStorageData::fetch( dmsMBContext *context,
                                 const dmsRecordID &recordID,
                                 BSONObj &dataRecord,
                                 pmdEDUCB * cb,
                                 BOOLEAN dataOwned )
   {
      INT32 rc                     = SDB_OK ;
      ossValuePtr extentPtr        = 0 ;
      dmsExtent *extent            = NULL ;
      ossValuePtr recordPtr        = 0 ;
      CHAR flag                    = 0 ;
      monAppCB * pMonAppCB         = cb ? cb->getMonAppCB() : NULL ;
      utilCompressor *compressor   = NULL ;
      utilCompressorContext compContext = UTIL_INVALID_COMP_CTX ;


      PD_TRACE_ENTRY ( SDB__DMSSTORAGEDATA_FETCH ) ;

      extentPtr = extentAddr ( recordID._extent ) ;
      if ( ! extentPtr )
      {
         rc = SDB_INVALIDARG ;
         goto error ;
      }
      extent = (dmsExtent*)extentPtr ;

      if ( !context->isMBLock() )
      {
         rc = SDB_SYS ;
         PD_LOG( PDERROR, "Caller must hold mb lock[%s]",
                 context->toString().c_str() ) ;
         goto error ;
      }

#ifdef _DEBUG
      if ( !dmsAccessAndFlagCompatiblity ( context->mb()->_flag,
                                           DMS_ACCESS_TYPE_FETCH ) )
      {
         PD_LOG ( PDERROR, "Incompatible collection mode: %d",
                  context->mb()->_flag ) ;
         rc = SDB_DMS_INCOMPATIBLE_MODE ;
         goto error ;
      }

      // validate extent
      if ( !extent->validate( context->mbID()) )
      {
         PD_LOG ( PDERROR, "Invalid extent[%d]", recordID._extent ) ;
         rc = SDB_SYS ;
         goto error ;
      }
#endif // _DEBUG

      recordPtr = extentPtr + recordID._offset ;

      // make sure the record is not deleted
      // since we get the RID outside the latch scope, so there could be
      // inconsistency happen, we need to make sure the flag is expected
      flag = DMS_RECORD_GETSTATE( recordPtr ) ;
      if ( DMS_RECORD_FLAG_DELETED == flag )
      {
         rc = SDB_DMS_NOTEXIST ;
         goto error ;
      }
      else if ( OSS_BIT_TEST( DMS_RECORD_FLAG_DELETING,
                              DMS_RECORD_GETATTR(recordPtr) ) )
      {
         rc = SDB_DMS_DELETING ;
         goto error ;
      }

#ifdef _DEBUG
      // but the status shouldn't be in other flag
      if (  DMS_RECORD_FLAG_NORMAL != flag &&
            DMS_RECORD_FLAG_OVERFLOWF != flag )
      {
         PD_LOG ( PDERROR, "Record[%d:%d] flag[%d] error", recordID._extent,
                  recordID._offset, flag ) ;
         rc = SDB_SYS ;
         goto error ;
      }
#endif //_DEBUG

      // if this record is overflow from
      if ( DMS_RECORD_FLAG_OVERFLOWF == flag )
      {
         dmsRecordID ovfRID = DMS_RECORD_GETOVF ( recordPtr ) ;
         DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_READ, 1 ) ;
         recordPtr = extentAddr( ovfRID._extent ) + ovfRID._offset ;
      }

      try
      {
         ossValuePtr fetchedRecord = 0 ;
         if ( DMS_INVALID_EXTENT != context->mb()->_dictExtentID )
         {
            compressor = _compressorEntry[context->mbID()].getCompressor() ;
            if ( compressor )
            {
               rc = compressor->prepare( compContext ) ;
               PD_RC_CHECK( rc, PDERROR,
                            "Failed to prepare compressor, rc: %d", rc ) ;
            }
         }
         DMS_RECORD_EXTRACTDATA( compressor, compContext,
                                 recordPtr, fetchedRecord ) ;
         if ( UTIL_INVALID_COMP_CTX != compContext )
         {
            compressor->done( compContext ) ;
            compContext = UTIL_INVALID_COMP_CTX ;
         }

         BSONObj obj( (CHAR*)fetchedRecord ) ;
         DMS_MON_OP_COUNT_INC( pMonAppCB, MON_DATA_READ, 1 ) ;
         DMS_MON_OP_COUNT_INC( pMonAppCB, MON_READ, 1 ) ;
         if ( dataOwned )
         {
            dataRecord = obj.getOwned() ;
         }
         else
         {
            dataRecord = obj ;
         }
      }
      catch ( std::exception &e )
      {
         PD_LOG ( PDERROR, "Failed to create BSON object: %s",
                  e.what() ) ;
         rc = SDB_SYS ;
         goto error ;
      }

   done :
      if ( UTIL_INVALID_COMP_CTX != compContext )
      {
         compressor->done( compContext ) ;
      }

      PD_TRACE_EXITRC ( SDB__DMSSTORAGEDATA_FETCH, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_TRYTOFLUSH, "_dmsStorageData::tryToFlush" )
   INT32 _dmsStorageData::tryToFlush( BOOLEAN ignoreTick, BOOLEAN &failed )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__DMSSTORAGEDATA_TRYTOFLUSH ) ;
      BOOLEAN locked = FALSE ;

      if ( !ignoreTick && !_noWriteForAWhile() )
      {
         failed = TRUE ;
         goto done ;
      }

      _validFlag = 1 ;
      syncMemToMmap() ;
      rc = flushAll( TRUE ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "failed to flush dirty segments:%d", rc ) ;
         goto error ;
      }

      if ( 0 ==_validFlag )
      {
         failed = TRUE ;
      }
      else
      {
         ossLatch ( &_pagecleanerLatch ) ;
         locked = TRUE ;

         if ( 0 ==_validFlag )
         {
            failed = TRUE ;
            goto done ;
         }

         _markHeaderValid() ;
         failed = FALSE ;
      }
   done:
      if ( locked )
      {
         ossUnlatch( &_pagecleanerLatch ) ;
      }
      PD_TRACE_EXITRC( SDB__DMSSTORAGEDATA_TRYTOFLUSH, rc ) ;
      return rc ;
   error:
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_PREPARECOMPRESSOR, "_dmsStorageData::prepareCompressor" )
   INT32 _dmsStorageData::prepareCompressor( const _dmsMBContext *context,
                                             const CHAR *dict, UINT32 dictLen )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__DMSSTORAGEDATA_PREPARECOMPRESSOR ) ;
      utilCompressor *compressor = NULL ;
      UTIL_COMPRESSOR_TYPE type  = (UTIL_COMPRESSOR_TYPE)
                             ((dmsMBContext *)context)->mb()->_compressorType ;
      dmsCompressorGuard compGuard( _compressorEntry[context->mbID()],
                                    EXCLUSIVE ) ;

      rc = _compressorFactory.createCompressor( type, compressor ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create compressor, rc: %d", rc ) ;

      rc = compressor->setDictionary( dict, dictLen ) ;
      PD_RC_CHECK( rc, PDERROR,
                   "Failed to set dictionary for compressor, rc: %d", rc ) ;

      _compressorEntry[context->mbID()].setCompressor( compressor ) ;

   done:
      PD_TRACE_EXITRC( SDB__DMSSTORAGEDATA_PREPARECOMPRESSOR, rc ) ;
      return rc ;
   error:
      _compressorFactory.destroyCompressor( compressor ) ;
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_RMCOMPRESSOR, "_dmsStorageData::rmCompressor" )
   void _dmsStorageData::rmCompressor( _dmsMBContext *context )
   {
      PD_TRACE_ENTRY( SDB__DMSSTORAGEDATA_RMCOMPRESSOR ) ;
      dmsCompressorGuard compGuard( _compressorEntry[context->mbID()],
                                    EXCLUSIVE ) ;
      _compressorEntry[context->mbID()].reset() ;
      PD_TRACE_EXIT( SDB__DMSSTORAGEDATA_RMCOMPRESSOR ) ;
   }

    // PD_TRACE_DECLARE_FUNCTION ( SDB__DMSSTORAGEDATA_DICTPERSIST, "_dmsStorageData::dictPersist" )
   INT32 _dmsStorageData::dictPersist( UINT16 mbID, UINT32 clLID,
                                       const CHAR *dict, UINT32 dictLen )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY( SDB__DMSSTORAGEDATA_DICTPERSIST ) ;
      dmsExtentID dictExtID = DMS_INVALID_EXTENT ;
      dmsDictExtent *dictExtent = NULL ;
      dmsMBContext *context = NULL ;
      UINT32 currClLID = DMS_INVALID_CLID ;

      /* Number of pages to store the dictionary, including the extent header.*/
      UINT32 pageNum =
      ( sizeof( dmsDictExtent ) + dictLen + ( pageSize() - 1 ) ) / pageSize() ;

      rc = getMBContext( &context, mbID, currClLID, EXCLUSIVE ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to get dms mb context, rc: %d", rc ) ;

      if ( clLID != context->clLID() )
      {
         /*
          * If the collection logical ID changed, the original collection has
          * been dropped.
          */
         rc = SDB_DMS_NOTEXIST ;
         goto error ;
      }

      rc = _findFreeSpace( pageNum, dictExtID, NULL ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to allocate space for dictionary "
                   "extent" ) ;
      dictExtent = ( dmsDictExtent *)extentAddr( dictExtID ) ;
      dictExtent->init( pageNum, context->mbID() ) ;
      dictExtent->setDict( dict, dictLen ) ;
      for ( INT32 i = 0; i < 3; i++ )
      {
         rc = flushPages( dictExtID, pageNum, TRUE ) ;
         if ( SDB_OK == rc )
         {
            break ;
         }
      }
      PD_RC_CHECK( rc, PDERROR, "Failed to flush dictionary. It will be "
                   "created and flushed again next time" ) ;

      /*
       * Set the dictionary extent id in mb only after the dictionary has been
       * successfully flushed to disk.
       */
      context->mb()->_dictExtentID = dictExtID ;

      /// Make sure the dict persist
      flushMME( TRUE ) ;

      PD_LOG( PDEVENT, "Compression dictionary created succesfully for "
              "collection[%s]", context->mb()->_collectionName ) ;
   done:
      if ( context )
      {
         releaseMBContext( context ) ;
      }

      PD_TRACE_EXITRC( SDB__DMSSTORAGEDATA_DICTPERSIST, rc ) ;
      return rc ;
   error:
      if ( DMS_INVALID_EXTENT != dictExtID )
      {
         _freeExtent( dictExtID ) ;
      }
      goto done ;
   }
}

