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

   Source File Name = omagentTask.hpp

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          06/30/2014  TZB Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef OMAGENTTASK_HPP_
#define OMAGENTTASK_HPP_

#include "core.hpp"
#include "oss.hpp"
#include "ossUtil.hpp"
#include "ossLatch.hpp"
#include "ossEvent.hpp"
#include "omagentTaskBase.hpp"
#include "../bson/bson.h"
#include "omagent.hpp"
#include "ossIO.hpp"
#include <map>
#include <set>
#include <vector>
#include <string>

using namespace std ;
using namespace bson ;

#define OMA_TASK_NAME_ADD_HOST                "add host task"
#define OMA_TASK_NAME_REMOVE_HOST             "remove host task"
#define OMA_TASK_NAME_INSTALL_DB_BUSINESS     "install db business task"
#define OMA_TASK_NAME_REMOVE_DB_BUSINESS      "remove db business task"
#define OMA_TASK_NAME_INSTALL_ZN_BUSINESS     "install zn business task"
#define OMA_TASK_NAME_REMOVE_ZN_BUSINESS      "remove zn business task"
#define OMA_TASK_NAME_SSQL_EXEC               "ssql exec task"


namespace engine
{
   /*
      add host task
   */
   class _omaAddHostTask : public _omaTask
   {
      public:
         _omaAddHostTask ( INT64 taskID ) ;
         virtual ~_omaAddHostTask () ;

      public:
         INT32 init( const BSONObj &info, void *ptr = NULL ) ;
         INT32 doit() ;

      public:
         AddHostInfo* getAddHostItem() ;
         INT32 updateProgressToTask( INT32 serialNum,
                                     AddHostResultInfo &resultInfo ) ;
         void notifyUpdateProgress() ;
         void setErrInfo( INT32 errNum, const CHAR *pDetail ) ;
         
      private:
         INT32 _initAddHostInfo( BSONObj &info ) ;
         void _initAddHostResult() ;
         INT32 _checkHostInfo() ;
         INT32 _addHosts() ;
         INT32 _waitAndUpdateProgress() ;
         void _buildUpdateTaskObj( BSONObj &retObj ) ; 
         INT32 _updateProgressToOM() ;
         BOOLEAN _isTaskFinish() ;
         void _setRetErr( INT32 errNum ) ;

      private:
         // add host raw info
         BSONObj                           _addHostRawInfo ;
         // add host info
         vector<AddHostInfo>               _addHostInfo ;
         // result
         map< INT32, AddHostResultInfo >   _addHostResult ;

         ossSpinSLatch                     _taskLatch ;
         ossEvent                          _taskEvent ;
         UINT64                            _eventID ; 

         INT32                             _progress ;
         INT32                             _errno ;
         CHAR                              _detail[OMA_BUFF_SIZE + 1] ;
   } ;
   typedef _omaAddHostTask omaAddHostTask ;


   /*
      remove host task
   */
   class _omaRemoveHostTask : public _omaTask
   {
      public:
         _omaRemoveHostTask ( INT64 taskID ) ;
         virtual ~_omaRemoveHostTask () ;

      public:
         INT32 init( const BSONObj &info, void *ptr = NULL ) ;
         INT32 doit() ;

      public:
         INT32 updateProgressToTask( INT32 serialNum,
                                     RemoveHostResultInfo &resultInfo,
                                     BOOLEAN needToNotify = FALSE ) ;
         
      private:
         INT32 _initRemoveHostInfo( BSONObj &info ) ;
         void _initRemoveHostResult() ;
         INT32 _removeHosts() ;
         void _buildUpdateTaskObj( BSONObj &retObj ) ; 
         INT32 _updateProgressToOM() ;
         void _setRetErr( INT32 errNum ) ;

      private:
         // remove host raw info
         BSONObj                            _removeHostRawInfo ;
         // remove host info
         vector<RemoveHostInfo>             _removeHostInfo ;
         // result
         map< INT32, RemoveHostResultInfo > _removeHostResult ;

         ossSpinSLatch                      _taskLatch ;

         INT32                              _progress ;
         INT32                              _errno ;
         CHAR                               _detail[OMA_BUFF_SIZE + 1] ;
   } ;
   typedef _omaRemoveHostTask omaRemoveHostTask ;

   /*
      install db business task
   */
   class _omaInstDBBusTask: public _omaTask
   {
      public:
         _omaInstDBBusTask( INT64 taskID ) ;
         virtual ~_omaInstDBBusTask () ;

      public:
         INT32 init( const BSONObj &info, void *ptr = NULL ) ;
         INT32 doit() ;

      public:
         void setIsTaskFail() ;
         BOOLEAN getIsTaskFail() ;

      public:
         INT32 updateProgressToTask( INT32 serialNum, InstDBResult &instResult,
                                     BOOLEAN needToNotify = FALSE ) ;
         INT32 updateProgressToTask( INT32 errNum, const CHAR *pDetail,
                                     const CHAR *pRole, OMA_TASK_STATUS status ) ;
         string getTmpCoordSvcName() ;
         void notifyUpdateProgress() ;
         void setErrInfo( INT32 errNum, const CHAR *pDetail ) ;
         string getDataRGToInst() ;
         InstDBBusInfo* getDataNodeInfo( string &groupName ) ;

      private:
         INT32 _initInstInfo( BSONObj &info ) ;
         INT32 _initInstAndResultInfo( BSONObj &hostInfo,
                                       InstDBBusInfo &info ) ;
         INT32 _initResultOrder( BSONObj &info ) ;
         INT32 _waitAndUpdateProgress() ;
         void _buildResultInfo( BOOLEAN isStandalone,
                                pair<string, string> &p,
                                BSONArrayBuilder &bab ) ;
         void  _buildUpdateTaskObj( BSONObj &retObj ) ;
         INT32 _calculateProgress() ;
         INT32 _updateProgressToOM() ;
         BOOLEAN _isTaskFinish() ;
         BOOLEAN _needToRollback() ;
         void _setRetErr( INT32 errNum ) ;
         void _setResultToFail() ;

      private:
         INT32 _saveTmpCoordInfo( BSONObj &info ) ;
         INT32 _installTmpCoord() ;
         INT32 _removeTmpCoord() ;
         INT32 _installStandalone() ;
         INT32 _rollback() ;
         INT32 _rollbackStandalone() ;
         INT32 _rollbackCatalog() ;
         INT32 _rollbackCoord() ;
         INT32 _rollbackDataRG() ;
         INT32 _installCatalog() ;
         INT32 _installCoord() ;
         INT32 _installDataRG() ;

      private:
         BSONObj                                _instDBBusRawInfo ;
         vector< pair<string, string> >         _resultOrder ;
         // install and result info
         vector<InstDBBusInfo>                  _standalone ;
         vector<InstDBBusInfo>                  _catalog ;
         vector<InstDBBusInfo>                  _coord ;
         map< string, vector<InstDBBusInfo> >   _mapGroups ;                        
         
         // temporary coord info
         string                                 _tmpCoordSvcName ;
         BSONObj                                _tmpCoordCfgObj ;

         BOOLEAN                                _isStandalone ;
         
         INT32                                  _nodeSerialNum ;
         BOOLEAN                                _isTaskFail ;
         ossSpinSLatch                          _taskLatch ;
         ossEvent                               _taskEvent ;
         UINT64                                 _eventID ;

         INT32                                  _progress ;
         INT32                                  _errno ;
         CHAR                                   _detail[OMA_BUFF_SIZE + 1] ;

         // groups have been created
         set<string>                       _existGroups ;
         
   } ;
   typedef _omaInstDBBusTask omaInstDBBusTask ;
   
   /*
      remove db business task
   */
   class _omaRemoveDBBusTask : public _omaTask
   {
      public:
         _omaRemoveDBBusTask ( INT64 taskID ) ;
         virtual ~_omaRemoveDBBusTask () ;

      public:
         INT32 init( const BSONObj &info, void *ptr = NULL ) ;
         INT32 doit() ;

      public:
         INT32 updateProgressToTask( INT32 serialNum, RemoveDBResult &instResult,
                                     BOOLEAN needToNotify = FALSE ) ;
         INT32 updateProgressToTask( INT32 errNum, const CHAR *pDetail,
                                     const CHAR *pRole, OMA_TASK_STATUS status ) ;
         void setErrInfo( INT32 errNum, const CHAR *pDetail ) ;
         string getTmpCoordSvcName() ;

      private:
         INT32 _initTaskInfo( BSONObj &info ) ;
         INT32 _initRemoveAndResultInfo( BSONObj &hostInfo,
                                         RemoveDBBusInfo &info ) ;
         INT32 _initResultOrder( BSONObj &info ) ;
         void  _getInfoToRemove( BSONObj &obj ) ;
         void  _buildResultInfo( BOOLEAN isStandalone,
                                 pair<string, string> &p,
                                 BSONArrayBuilder &bab ) ;
         void  _buildUpdateTaskObj( BSONObj &retObj ) ;
         INT32 _calculateProgress() ;
         INT32 _updateProgressToOM() ;
         void  _setRetErr( INT32 errNum ) ;

      private:
         INT32 _saveTmpCoordInfo( BSONObj &info ) ;
         INT32 _installTmpCoord() ;
         INT32 _removeTmpCoord() ;
         INT32 _removeStandalone() ;
         INT32 _removeCatalog() ;
         INT32 _removeCoord() ;
         INT32 _removeDataRG() ;

      private:
         BSONObj                           _removeDBBusRawInfo ;
         BOOLEAN                           _isStandalone ;
         vector< pair<string, string> >    _resultOrder ;
         // uninstall and result info
         vector<RemoveDBBusInfo>           _standalone ;
         vector<RemoveDBBusInfo>           _catalog ;
         vector<RemoveDBBusInfo>           _coord ;
         vector<RemoveDBBusInfo>           _data ;

      private:
         // temporary coord info
         string                            _tmpCoordSvcName ;
         BSONObj                           _tmpCoordCfgObj ;
         // auth info
         BSONObj                           _authInfo ;
         
      private:
         INT32                             _nodeSerialNum ;
         ossSpinSLatch                     _taskLatch ;

      private:
         INT32                             _progress ;
         INT32                             _errno ;
         CHAR                              _detail[OMA_BUFF_SIZE + 1] ;
   } ;
   typedef _omaRemoveDBBusTask omaRemoveDBBusTask ;

   /*
      zookeeper task base 
   */
   class _omaZNBusTaskBase : public _omaTask
   {
      public:
         _omaZNBusTaskBase ( INT64 taskID ) ;
         virtual ~_omaZNBusTaskBase () ;

      public:
         virtual INT32 init( const BSONObj &info, void *ptr = NULL ) = 0 ;
         virtual INT32 doit() = 0 ;

      public:
         void    setIsTaskFail() ;
         BOOLEAN getIsTaskFail() ;

      public:
         ZNInfo*    getZNInfo() ;
         INT32      updateProgressToTask( INT32 serialNum,
                                          ZNResultInfo &resultInfo ) ;
         INT32      updateProgressToTask( INT32 serialNum,
                                          INT32 errNum,
                                          const CHAR *pDetail,
                                          OMA_TASK_STATUS status ) ;
         void       notifyUpdateProgress() ;
         void       setErrInfo( INT32 errNum, const CHAR *pDetail ) ;
         
      protected:
         INT32   _initZNInfo( BSONObj &info ) ;
         void    _initZNResult() ;
         INT32   _removeZNode( ZNInfo &znodeInfo ) ;

      protected:
         INT32   _waitAndUpdateProgress() ;
         BOOLEAN _isTaskFinish() ;
         INT32   _calculateProgress() ;
         void    _buildUpdateTaskObj( BSONObj &retObj ) ; 
         INT32   _updateProgressToOM() ;
         void    _setRetErr( INT32 errNum ) ;
         void    _setResultToFail() ;

      protected:
         // znode raw info
         BSONObj                           _ZNRawInfo ;
         // znode info
         vector<ZNInfo>                    _ZNInfo ;
         // result
         map< INT32, ZNResultInfo >        _ZNResult ;
         
         BOOLEAN                           _isTaskFail ;
         
      protected:
         ossSpinSLatch                     _taskLatch ;
         ossEvent                          _taskEvent ;
         UINT64                            _eventID ; 

         INT32                             _progress ;
         INT32                             _errno ;
         CHAR                              _detail[OMA_BUFF_SIZE + 1] ;
   } ;
   typedef _omaZNBusTaskBase omaZNBusTaskBase ;

   /*
      insall zookeeper task
   */
   class _omaInstZNBusTask : public _omaZNBusTaskBase
   {
      public:
         _omaInstZNBusTask ( INT64 taskID ) ;
         virtual ~_omaInstZNBusTask () ;

      public:
         INT32 init( const BSONObj &info, void *ptr = NULL ) ;
         INT32 doit() ;
       
      private:
         INT32   _addZNodes() ;
         INT32   _checkAndCleanEnv() ;
         INT32   _checkZNodes() ;
         BOOLEAN _needToRollback() ;
         INT32   _rollback( BOOLEAN isRestart ) ;   
   } ;
   typedef _omaZNBusTaskBase omaZNBusTaskBase ;

   /*
      remove zookeeper task
   */
   class _omaRemoveZNBusTask : public _omaZNBusTaskBase
   {
      public:
         _omaRemoveZNBusTask ( INT64 taskID ) ;
         virtual ~_omaRemoveZNBusTask () ;

      public:
         virtual INT32 init( const BSONObj &info, void *ptr = NULL ) ;
         virtual INT32 doit() ;

      private:
         INT32 _removeZNodes() ;
   } ;
   typedef _omaRemoveZNBusTask omaRemoveZNBusTask ;

   /*
      ssql execute task
   */

   typedef struct ssqlRowData_s{
      UINT64 rowNum ;
      string rowData ;
      ssqlRowData_s()
      {
         rowNum  = 0 ;
         rowData = "" ;
      }

      ssqlRowData_s( const ssqlRowData_s &right )
      {
         rowNum  = right.rowNum ;
         rowData = right.rowData ;
      }

   } ssqlRowData_t ;

   class _omaSsqlExecTask : public _omaTask
   {
      public:
         _omaSsqlExecTask( INT64 taskID ) ;
         virtual ~_omaSsqlExecTask() ;

      public:
         virtual INT32        init( const BSONObj &info, void *ptr = NULL ) ;
         virtual INT32        doit() ;

      public:
         INT32                getSqlData( list<ssqlRowData_t> &data, 
                                          BOOLEAN &isFinish ) ;

      private:
         INT32                _cleanTask() ;
         INT32                _executeSsql( string &pipeFile ) ;
         INT32                _updateTaskStatus2OM( INT32 status ) ;
         INT32                _waitOMReadData() ;
         INT32                _waitAgentReadData() ;
         INT32                _getLines( const CHAR *newData, INT32 length, 
                                         INT32 maxLines ) ;
         INT32                _readDataFromPipe( OSSFILE *file, 
                                                 INT32 maxLines ) ; 
         INT32                _getPsql() ;
         INT32                _select( OSSFILE *file, INT32 timeout ) ;

      private:
         SsqlExecInfo         _ssqlInfo ;
         BOOLEAN              _isCleanTask ;

         string               _errorDetail ;
         INT32                _saveRC ;

         ossAutoEvent         _dataReadyEvent ; // agent have read data
         ossAutoEvent         _readDataEvent ;  // om have read data

         ossSpinSLatch        _taskLatch ;
         list<ssqlRowData_t>  _readDataList ;
         UINT64               _rowNum ;
         string               _lastLeftData ;
         BOOLEAN              _readFinish ;
         
   } ;
   typedef _omaSsqlExecTask omaSsqlExecTask ;

}




#endif
