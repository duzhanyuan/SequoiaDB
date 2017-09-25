/*******************************************************************************
   Copyright (C) 2012-2014 SequoiaDB Ltd.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*******************************************************************************/
#ifndef CLIENTIMPL_HPP__
#define CLIENTIMPL_HPP__
#include "core.hpp"
#include "client.hpp"
#include "ossSocket.hpp"
#include <set>
#if defined CLIENT_THREAD_SAFE
#include "ossLatch.hpp"
#endif
using namespace bson ;
namespace sdbclient
{
#define CLIENT_COLLECTION_NAMESZ           127
#define CLIENT_CS_NAMESZ                   127
#define CLIENT_REPLICAGROUP_NAMESZ         127
#define CLIENT_DOMAIN_NAMESZ               127
#define CLIENT_DC_NAMESZ                   127
   class _sdbCollectionSpaceImpl ;
   class _sdbCollectionImpl ;
   class _sdbReplicaGroupImpl ;
   class _sdbNodeImpl ;
   class _sdbDomainImpl ;
   class _sdbDataCenterImpl ;
   class _sdbLobImpl ;
   class _sdbImpl ;

   /*
      _sdbCursorImpl
   */
   class _sdbCursorImpl : public _sdbCursor
   {
   private :
      _sdbCursorImpl ( const _sdbCursorImpl& other ) ;
      _sdbCursorImpl& operator=( const _sdbCursorImpl& ) ;
      _sdbImpl *_connection ;
      _sdbCollectionImpl *_collection ;
      CHAR *_pSendBuffer ;
      INT32 _sendBufferSize ;
      CHAR *_pReceiveBuffer ;
      INT32 _receiveBufferSize ;
      BSONObj *_modifiedCurrent ;
      BOOLEAN _isDeleteCurrent ;
      SINT64 _contextID ;
      BSONObj _hintObj ;
      BOOLEAN _isClosed ;

      INT64 _totalRead ;
      INT32 _offset ;
      void _setConnection ( _sdb *connection ) ;
      void _killCursor () ;
      INT32 _readNextBuffer () ;
      void _setCollection ( _sdbCollectionImpl *collection ) ;
      void _dropConnection()
      {
         _connection = NULL ;
      }

      friend class _sdbCollectionImpl ;
      friend class _sdbNodeImpl ;
      friend class _sdbImpl ;
   public :
      _sdbCursorImpl () ;
      ~_sdbCursorImpl () ;
      INT32 next          ( BSONObj &obj ) ;
      INT32 current       ( BSONObj &obj ) ;
      INT32 close () ;
      //INT32 updateCurrent ( BSONObj &rule ) ;
      //INT32 delCurrent    () ;
   } ;
   
   typedef class _sdbCursorImpl sdbCursorImpl ;

   /*
      _sdbCollectionImpl
   */
   class _sdbCollectionImpl : public _sdbCollection
   {
   private :
      _sdbCollectionImpl ( const _sdbCollectionImpl& other ) ;
      _sdbCollectionImpl& operator=( const _sdbCollectionImpl& ) ;
#if defined CLIENT_THREAD_SAFE
      ossSpinSLatch            _mutex ;
#endif
      _sdbImpl                *_connection ;
      CHAR                    *_pSendBuffer ;
      INT32                    _sendBufferSize ;
      CHAR                    *_pReceiveBuffer ;
      INT32                    _receiveBufferSize ;
      CHAR                    *_pAppendOIDBuffer ;
      INT32                    _appendOIDBufferSize ;
      std::set<ossValuePtr> _cursors ;
      CHAR _collectionSpaceName [ CLIENT_CS_NAMESZ+1 ] ;
      CHAR _collectionName      [ CLIENT_COLLECTION_NAMESZ+1 ] ;
      CHAR _collectionFullName  [ CLIENT_COLLECTION_NAMESZ +
                                  CLIENT_CS_NAMESZ +
                                  1 ] ;
      INT32 _setName ( const CHAR *pCollectionFullName ) ;
      void _setConnection ( _sdb *connection ) ;
      void* _getConnection () ;
      void _dropConnection()
      {
         _connection = NULL ;
      }
      void _regCursor ( _sdbCursorImpl *cursor )
      {
         lock () ;
         _cursors.insert ( (ossValuePtr)cursor ) ;
         unlock () ;
      }
      void _unregCursor ( _sdbCursorImpl * cursor )
      {
         lock () ;
         _cursors.erase ( (ossValuePtr)cursor ) ;
         unlock () ;
      }
      //void _renameAttempt ( const CHAR *pOldName, const CHAR *pNewName ) ;

      INT32 _queryAndModify  ( _sdbCursor **cursor,
                               const BSONObj &condition,
                               const BSONObj &selected,
                               const BSONObj &orderBy,
                               const BSONObj &hint,
                               const BSONObj &update,
                               INT64 numToSkip,
                               INT64 numToReturn,
                               INT32 flag,
                               BOOLEAN isUpdate,
                               BOOLEAN returnNew ) ;

      INT32 _update ( const BSONObj &rule,
                      const BSONObj &condition,
                      const BSONObj &hint,
                      INT32 flag ) ;
      INT32 _appendOID ( const BSONObj &input,
                         BSONObj &output ) ;
      INT32 _runCmdOfLob ( const CHAR *cmd, const BSONObj &obj,
                           _sdbCursor **cursor ) ;
#if defined CLIENT_THREAD_SAFE
      void lock ()
      {
         _mutex.get () ;
      }
      void unlock ()
      {
         _mutex.release () ;
      }
#else
      void lock ()
      {
      }
      void unlock ()
      {
      }
#endif
      friend class _sdbCollectionSpaceImpl ;
      friend class _sdbImpl ;
      friend class _sdbCursorImpl ;
   public :
      _sdbCollectionImpl () ;
      _sdbCollectionImpl ( CHAR *pCollectionFullName ) ;
      _sdbCollectionImpl ( CHAR *pCollectionSpaceName,
                           CHAR *pCollectionName ) ;
      ~_sdbCollectionImpl () ;
      // get the total number of records for a given condition, if the condition
      // is NULL then match all records in the collection
      INT32 getCount ( SINT64 &count,
                       const BSONObj &condition ) ;
      // insert a bson object into current collection
      // given:
      // object ( required )
      INT32 bulkInsert ( SINT32 flags,
                         vector<BSONObj> &obj
                       ) ;
      INT32 insert ( const BSONObj &obj, OID *id ) ;
      // update bson object from current collection
      // given:
      // update rule ( required )
      // update condition ( optional )
      // hint ( optional )
      INT32 update ( const BSONObj &rule,
                     const BSONObj &condition = _sdbStaticObject,
                     const BSONObj &hint = _sdbStaticObject
                   ) ;

      // update bson object from current collection, if no record has been
      // updated, it will insert a new record that modified from an empty bson
      // object
      // given:
      // update rule ( required )
      // update condition ( optional )
      // hint ( optional )
      // setOnInsert ( optional )
      INT32 upsert ( const BSONObj &rule,
                     const BSONObj &condition = _sdbStaticObject,
                     const BSONObj &hint = _sdbStaticObject,
                     const BSONObj &setOnInsert = _sdbStaticObject
                   ) ;
      // delete bson objects from current collection
      // given:
      // delete condition ( optional )
      // hint ( optional )
      INT32 del    ( const BSONObj &condition = _sdbStaticObject,
                     const BSONObj &hint = _sdbStaticObject
                   ) ;

      // query objects from current collection
      // given:
      // query condition ( optional )
      // query selected def ( optional )
      // query orderby ( optional )
      // hint ( optional )
      // output: sdbCursor ( required )
      INT32 query  ( _sdbCursor **cursor,
                     const BSONObj &condition = _sdbStaticObject,
                     const BSONObj &selected  = _sdbStaticObject,
                     const BSONObj &orderBy   = _sdbStaticObject,
                     const BSONObj &hint      = _sdbStaticObject,
                     INT64 numToSkip          = 0,
                     INT64 numToReturn        = -1,
                     INT32 flag               = 0
                   ) ;

      INT32 query  ( sdbCursor &cursor,
                     const BSONObj &condition = _sdbStaticObject,
                     const BSONObj &selected  = _sdbStaticObject,
                     const BSONObj &orderBy   = _sdbStaticObject,
                     const BSONObj &hint      = _sdbStaticObject,
                     INT64 numToSkip          = 0,
                     INT64 numToReturn        = -1,
                     INT32 flag               = 0
                   )
      {
         return query ( &cursor.pCursor,
                        condition, selected, orderBy, hint,
                        numToSkip, numToReturn, flag ) ;
      }

      INT32 queryOne( bson::BSONObj &obj,
                      const bson::BSONObj &condition = _sdbStaticObject,
                      const bson::BSONObj &selected  = _sdbStaticObject,
                      const bson::BSONObj &orderBy   = _sdbStaticObject,
                      const bson::BSONObj &hint      = _sdbStaticObject,
                      INT64 numToSkip    = 0,
                      INT32 flag = 0 ) ;

      // query objects from current collection and update
      // given:
      // update rule ( required )
      // query condition ( optional )
      // query selected def ( optional )
      // query orderby ( optional )
      // hint ( optional )
      // flag ( optional )
      // returnNew ( optioinal )
      // output: sdbCursor ( required )
      INT32 queryAndUpdate  ( _sdbCursor **cursor,
                              const BSONObj &update,
                              const BSONObj &condition = _sdbStaticObject,
                              const BSONObj &selected  = _sdbStaticObject,
                              const BSONObj &orderBy   = _sdbStaticObject,
                              const BSONObj &hint      = _sdbStaticObject,
                              INT64 numToSkip          = 0,
                              INT64 numToReturn        = -1,
                              INT32 flag               = 0,
                              BOOLEAN returnNew        = FALSE )
      {
         return _queryAndModify( cursor, condition, selected, orderBy,
                                 hint, update, numToSkip, numToReturn,
                                 flag, TRUE, returnNew ) ;
      }

      // query objects from current collection and remove
      // given:
      // query condition ( optional )
      // query selected def ( optional )
      // query orderby ( optional )
      // hint ( optional )
      // flag ( optional )
      // output: sdbCursor ( required )
      INT32 queryAndRemove  ( _sdbCursor **cursor,
                              const BSONObj &condition = _sdbStaticObject,
                              const BSONObj &selected  = _sdbStaticObject,
                              const BSONObj &orderBy   = _sdbStaticObject,
                              const BSONObj &hint      = _sdbStaticObject,
                              INT64 numToSkip          = 0,
                              INT64 numToReturn        = -1,
                              INT32 flag               = 0 )
      {
         return _queryAndModify( cursor, condition, selected, orderBy,
                                 hint, _sdbStaticObject, numToSkip, numToReturn,
                                 flag, FALSE, FALSE ) ;
      }

      //INT32 rename ( const CHAR *pNewName ) ;
      // create an index for the current collection
      // given:
      // index definition ( required )
      // index name ( required )
      // uniqueness ( required )
      INT32 createIndex ( const BSONObj &indexDef, const CHAR *pName,
                          BOOLEAN isUnique, BOOLEAN isEnforced ) ;
      INT32 createIndex ( const BSONObj &indexDef,
                          const CHAR *pName,
                          BOOLEAN isUnique,
                          BOOLEAN isEnforced,
                          INT32 sortBufferSize ) ;
      INT32 getIndexes ( _sdbCursor **cursor,
                         const CHAR *pName ) ;
      INT32 getIndexes ( sdbCursor &cursor,
                         const CHAR *pName )
      {
         return getIndexes ( &cursor.pCursor, pName ) ;
      }
      INT32 dropIndex ( const CHAR *pName ) ;
      INT32 create () ;
      INT32 drop () ;
      const CHAR *getCollectionName ()
      {
         return &_collectionName[0] ;
      }
      const CHAR *getCSName ()
      {
         return &_collectionSpaceName[0] ;
      }
      const CHAR *getFullName ()
      {
         return &_collectionFullName[0] ;
      }
      INT32 split ( const CHAR *pSourceReplicaGroupName,
                    const CHAR *pTargetReplicaGroupName,
                    const BSONObj &splitCondition,
                    const bson::BSONObj &splitEndCondition = _sdbStaticObject ) ;
      INT32 split ( const CHAR *pSourceReplicaGroupName,
                    const CHAR *pTargetReplicaGroupName,
                    FLOAT64 percent ) ;
      INT32 splitAsync ( SINT64 &taskID,
               const CHAR *pSourceReplicaGroupName,
                    const CHAR *pTargetReplicaGroupName,
                    const bson::BSONObj &splitCondition,
                    const bson::BSONObj &splitEndCondition = _sdbStaticObject ) ;
      INT32 splitAsync ( const CHAR *pSourceReplicaGroupName,
                    const CHAR *pTargetReplicaGroupName,
                    FLOAT64 percent,
                    SINT64 &taskID ) ;
      // aggregate
      INT32 aggregate ( _sdbCursor **cursor,
                     std::vector<bson::BSONObj> &obj
                   ) ;
      INT32 aggregate ( sdbCursor &cursor,
                     std::vector<bson::BSONObj> &obj
                   )
      {
         return aggregate ( &cursor.pCursor, obj ) ;
      }
      INT32 getQueryMeta  ( _sdbCursor **cursor,
                     const BSONObj &condition = _sdbStaticObject,
                     const BSONObj &orderBy = _sdbStaticObject,
                     const BSONObj &hint = _sdbStaticObject,
                     INT64 numToSkip = 0,
                     INT64 numToReturn = -1
                   ) ;
      INT32 getQueryMeta  ( sdbCursor &cursor,
                     const BSONObj &condition = _sdbStaticObject,
                     const BSONObj &orderBy = _sdbStaticObject,
                     const BSONObj &hint = _sdbStaticObject,
                     INT64 numToSkip = 0,
                     INT64 numToReturn = -1
                   )
      {
         return getQueryMeta ( &cursor.pCursor,
                        condition, orderBy, hint,
                        numToSkip, numToReturn ) ;
      }

      INT32 attachCollection ( const CHAR *subClFullName,
                                      const bson::BSONObj &options) ;
      INT32 detachCollection ( const CHAR *subClFullName) ;

      INT32 alterCollection ( const bson::BSONObj &options ) ;

      /// explain
      INT32 explain ( _sdbCursor **cursor,
                              const bson::BSONObj &condition = _sdbStaticObject,
                              const bson::BSONObj &select    = _sdbStaticObject,
                              const bson::BSONObj &orderBy   = _sdbStaticObject,
                              const bson::BSONObj &hint      = _sdbStaticObject,
                              INT64 numToSkip                = 0,
                              INT64 numToReturn              = -1,
                              INT32 flag                     = 0,
                              const bson::BSONObj &options   = _sdbStaticObject ) ;

      virtual INT32 explain ( sdbCursor &cursor,
                              const bson::BSONObj &condition = _sdbStaticObject,
                              const bson::BSONObj &select    = _sdbStaticObject,
                              const bson::BSONObj &orderBy   = _sdbStaticObject,
                              const bson::BSONObj &hint      = _sdbStaticObject,
                              INT64 numToSkip                = 0,
                              INT64 numToReturn              = -1,
                              INT32 flag                     = 0,
                              const bson::BSONObj &options   = _sdbStaticObject )
      {
         return explain( &cursor.pCursor, condition, select, orderBy, hint,
                         numToSkip, numToReturn, flag, options ) ;
      }

      /// lob
      INT32 createLob( _sdbLob **lob, const bson::OID *oid = NULL ) ;
      
      virtual INT32 createLob( sdbLob &lob, const bson::OID *oid = NULL )
      {
         return createLob( &lob.pLob, oid ) ;
      }

      virtual INT32 removeLob( const bson::OID &oid ) ;

      INT32 openLob( _sdbLob **lob, const bson::OID &oid ) ;
      
      virtual INT32 openLob( sdbLob &lob, const bson::OID &oid )
      {
         return openLob( &lob.pLob, oid ) ;
      }

      INT32 listLobs ( _sdbCursor **cursor ) ;
      
      virtual INT32 listLobs( sdbCursor &cursor )
      {
         return listLobs( &cursor.pCursor ) ;
      }

      /// truncate
      INT32 truncate() ;

      /// create/drop index
      INT32 createIdIndex( const bson::BSONObj &options = _sdbStaticObject ) ;

      INT32 dropIdIndex() ;
      
   private:
      INT32 _alterCollection1( const bson::BSONObj &options ) ;
      INT32 _alterCollection2( const bson::BSONObj &options ) ;
      INT32 _createIndex ( const BSONObj &indexDef, const CHAR *pName,
                           BOOLEAN isUnique, BOOLEAN isEnforced,
                           INT32 sortBufferSize ) ;

   } ;
   
   typedef class _sdbCollectionImpl sdbCollectionImpl ;

   /*
      _sdbNodeImpl
   */
#define SDB_NODE_INVALID_NODEID -1
   class _sdbNodeImpl : public _sdbNode
   {
   private :
      _sdbNodeImpl ( const _sdbNodeImpl& other ) ;
      _sdbNodeImpl& operator=( const _sdbNodeImpl& ) ;
#if defined CLIENT_THREAD_SAFE
      ossSpinSLatch _mutex ;
#endif
      _sdbImpl                *_connection ;
      CHAR                     _hostName [ OSS_MAX_HOSTNAME + 1 ] ;
      CHAR                     _serviceName [ OSS_MAX_SERVICENAME + 1 ] ;
      CHAR                     _nodeName [ OSS_MAX_HOSTNAME +
                                           OSS_MAX_SERVICENAME + 2 ] ;
      INT32                    _replicaGroupID ;
      INT32                    _nodeID ;
      void _dropConnection()
      {
         _connection = NULL ;
      }
      INT32 _stopStart ( BOOLEAN start ) ;
      friend class _sdbReplicaGroupImpl ;
      friend class _sdbImpl ;
   public :
      _sdbNodeImpl () ;
      ~_sdbNodeImpl () ;
      // directly connect to the current node
      INT32 connect ( _sdb **dbConn ) ;
      INT32 connect ( sdb &dbConn )
      {
         return connect ( &dbConn.pSDB ) ;
      }

      // get status of the current node
      sdbNodeStatus getStatus () ;

      // get the hostname
      const CHAR *getHostName ()
      {
         return _hostName ;
      }

      // get the service name
      const CHAR *getServiceName ()
      {
         return _serviceName ;
      }

      // get the node id
      const CHAR *getNodeName ()
      {
         return _nodeName ;
      }

      // stop the node
      INT32 stop () { return _stopStart ( FALSE ) ; }

      // start the node
      INT32 start () { return _stopStart ( TRUE ) ; }

      // modify config for the current node
/*      INT32 modifyConfig ( std::map<std::string,std::string> &config ) ;*/
   } ;

   typedef class _sdbNodeImpl sdbNodeImpl ;

   /*
      _sdbReplicaGroupImpl
   */
   class _sdbReplicaGroupImpl : public _sdbReplicaGroup
   {
   private :
      _sdbReplicaGroupImpl ( const _sdbReplicaGroupImpl& other ) ;
      _sdbReplicaGroupImpl& operator=( const _sdbReplicaGroupImpl& ) ;
#if defined CLIENT_THREAD_SAFE
      ossSpinSLatch _mutex ;
#endif
      _sdbImpl                *_connection ;
      BOOLEAN                 _isCatalog ;
      INT32                   _replicaGroupID ;
      CHAR                    _replicaGroupName [ CLIENT_REPLICAGROUP_NAMESZ+1 ] ;
      void _dropConnection()
      {
         _connection = NULL ;
      }
      INT32 _stopStart ( BOOLEAN start ) ;
      INT32 _extractNode ( _sdbNode **node,
                           const CHAR *primaryData ) ;
      friend class _sdbImpl ;
   public :
      _sdbReplicaGroupImpl () ;
      ~_sdbReplicaGroupImpl () ;

      // get number of logical nodes
      INT32 getNodeNum ( sdbNodeStatus status, INT32 *num ) ;

      // list all nodes in the current replica group
      INT32 getDetail ( BSONObj &result ) ;

      INT32 getMaster ( _sdbNode **node ) ;
      INT32 getMaster ( sdbNode &node )
      {
         return getMaster ( &node.pNode ) ;
      }

      INT32 getSlave ( _sdbNode **node ) ;
      INT32 getSlave ( sdbNode &node )
      {
         return getSlave ( &node.pNode ) ;
      }

      INT32 getNode ( const CHAR *pNodeName,
                      _sdbNode **node ) ;
      INT32 getNode ( const CHAR *pNodeName,
                      sdbNode &node )
      {
         return getNode ( pNodeName, &node.pNode ) ;
      }

      INT32 getNode ( const CHAR *pHostName,
                      const CHAR *pServiceName,
                      _sdbNode **node ) ;
      INT32 getNode ( const CHAR *pHostName,
                      const CHAR *pServiceName,
                      sdbNode &node )
      {
         return getNode ( pHostName, pServiceName, &node.pNode ) ;
      }
      // create a new node in current replica group
      INT32 createNode ( const CHAR *pHostName,
                         const CHAR *pServiceName,
                         const CHAR *pDatabasePath,
                         std::map<std::string,std::string> &config ) ;

      INT32 createNode ( const CHAR *pHostName,
                         const CHAR *pServiceName,
                         const CHAR *pDatabasePath,
                         const bson::BSONObj &options = _sdbStaticObject ) ;

      // remove the specified node in current replica group
      INT32 removeNode ( const CHAR *pHostName,
                         const CHAR *pServiceName,
                         const BSONObj &configure = _sdbStaticObject ) ;

      // activate the replica group
      INT32 start () ;

      // stop the entire replica group
      INT32 stop () ;

      // get replica group name
      const CHAR *getName ()
      {
         return _replicaGroupName ;
      }

      // whether the current replica group is catalog replica group or not
      BOOLEAN isCatalog ()
      {
         return _isCatalog ;
      }

      INT32 attachNode( const CHAR *pHostName,
                        const CHAR *pSvcName,
                        const bson::BSONObj &options = _sdbStaticObject ) ;
      INT32 detachNode( const CHAR *pHostName,
                        const CHAR *pSvcName,
                        const bson::BSONObj &options = _sdbStaticObject ) ;
      
   } ;
   
   typedef class _sdbReplicaGroupImpl sdbReplicaGroupImpl ;

   /*
      _sdbCollectionSpaceImpl
   */
   class _sdbCollectionSpaceImpl : public _sdbCollectionSpace
   {
   private :
      _sdbCollectionSpaceImpl ( const _sdbCollectionSpaceImpl& other ) ;
      _sdbCollectionSpaceImpl& operator=( const _sdbCollectionSpaceImpl& ) ;
#if defined CLIENT_THREAD_SAFE
      ossSpinSLatch _mutex ;
#endif
      _sdbImpl                *_connection ;
      CHAR                    *_pSendBuffer ;
      INT32                    _sendBufferSize ;
      CHAR                    *_pReceiveBuffer ;
      INT32                    _receiveBufferSize ;
      CHAR _collectionSpaceName [ CLIENT_CS_NAMESZ+1 ] ;
      void _setConnection ( _sdb *connection ) ;
      INT32 _setName ( const CHAR *pCollectionSpaceName ) ;
      void _dropConnection()
      {
         _connection = NULL ;
      }

      friend class _sdbImpl ;
   public :
      _sdbCollectionSpaceImpl () ;
      _sdbCollectionSpaceImpl ( CHAR *pCollectionSpaceName ) ;
      ~_sdbCollectionSpaceImpl () ;
      // get a collection object
      INT32 getCollection ( const CHAR *pCollectionName,
                            _sdbCollection **collection ) ;
      INT32 getCollection ( const CHAR *pCollectionName,
                            sdbCollection &collection )
      {
         return getCollection ( pCollectionName,
                                &collection.pCollection ) ;
      }
      // create a new collection object
      INT32 createCollection ( const CHAR *pCollection,
                               _sdbCollection **collection ) ;
      INT32 createCollection ( const CHAR *pCollection,
                               sdbCollection &collection )
      {
         return createCollection ( pCollection,
                                   &collection.pCollection ) ;
      }
      // create a new shareded collection object
      INT32 createCollection ( const CHAR *pCollection,
                               const BSONObj &options,
                               _sdbCollection **collection ) ;
      INT32 createCollection ( const CHAR *pCollection,
                               const BSONObj &options,
                               sdbCollection &collection )
      {
         return createCollection ( pCollection,
                                   options,
                                   &collection.pCollection ) ;
      }
      // drop an existing collection
      INT32 dropCollection ( const CHAR *pCollection ) ;

      // create a collection space with current collection space name
      INT32 create () ;
      // drop a collection space with current collection space name
      INT32 drop () ;

      const CHAR *getCSName ()
      {
         return &_collectionSpaceName[0] ;
      }
   } ;

   typedef class _sdbCollectionSpaceImpl sdbCollectionSpaceImpl ;

   /*
      _sdbDomainImpl
   */
   class _sdbDomainImpl : public _sdbDomain
   {
   private :
      _sdbDomainImpl ( const _sdbDomainImpl& other ) ;
      _sdbDomainImpl& operator= ( const _sdbDomainImpl& other ) ;
#if defined CLIENT_THREAD_SAFE
      ossSpinSLatch           _mutex ;
#endif
      _sdbImpl                *_connection ;
      CHAR                    *_pSendBuffer ;
      INT32                   _sendBufferSize ;
      CHAR                    *_pReceiveBuffer ;
      INT32                   _receiveBufferSize ;
      CHAR _domainName[ CLIENT_DOMAIN_NAMESZ+1 ] ;

      void _setConnection ( _sdb *connection ) ;
      void _dropConnection()
      {
         _connection = NULL ;
      }
      INT32 _setName ( const CHAR *pDomainName ) ;

      friend class _sdbImpl ;
   public :
      _sdbDomainImpl () ;
      _sdbDomainImpl ( const CHAR *pDomainName ) ;
      ~_sdbDomainImpl () ;

      const CHAR* getName ()
      {
         return _domainName ;
      }

      INT32 alterDomain ( const bson::BSONObj &options ) ;

      INT32 listCollectionSpacesInDomain ( _sdbCursor **cursor ) ;

      INT32 listCollectionSpacesInDomain ( sdbCursor &cursor )
      {
         return listCollectionSpacesInDomain ( &cursor.pCursor ) ;
      }

      INT32 listCollectionsInDomain ( _sdbCursor **cursor ) ;

      INT32 listCollectionsInDomain ( sdbCursor &cursor )
      {
         return listCollectionsInDomain ( &cursor.pCursor ) ;
      }

   } ;
   typedef class _sdbDomainImpl sdbDomainImpl ;

   /*
      _sdbDataCenterImpl
   */
   class _sdbDataCenterImpl : public _sdbDataCenter
   {
      friend class _sdbImpl ;
      
   private :
      _sdbDataCenterImpl ( const _sdbDataCenterImpl& other ) ;
      _sdbDataCenterImpl& operator= ( const _sdbDataCenterImpl& other ) ;
#if defined CLIENT_THREAD_SAFE
      ossSpinSLatch           _mutex ;
#endif
      _sdbImpl                *_connection ;
      CHAR                    *_pSendBuffer ;
      INT32                   _sendBufferSize ;
      CHAR                    *_pReceiveBuffer ;
      INT32                   _receiveBufferSize ;
      CHAR _dcName[ CLIENT_DC_NAMESZ+1 ] ;

   private:
      INT32 _setName ( const CHAR *pClusterName,
                       const CHAR *pBusinessName ) ;
      void _setConnection ( _sdb *connection ) ;
      void _dropConnection()
      {
         _connection = NULL ;
      }

   public :
      _sdbDataCenterImpl () ;
      ~_sdbDataCenterImpl () ;

   public :
      const CHAR* getName ()
      {
         return _dcName ;
      }
      INT32 getDetail( bson::BSONObj &retInfo ) ;
      INT32 activateDC() ;
      INT32 deactivateDC() ;
      INT32 enableReadOnly( BOOLEAN isReadOnly ) ;
      INT32 createImage( const CHAR *pCataAddrList ) ;
      INT32 removeImage() ;
      INT32 enableImage() ;
      INT32 disableImage() ;
      INT32 attachGroups( const bson::BSONObj &info ) ;
      INT32 detachGroups( const bson::BSONObj &info ) ;

   private :
      INT32 _DCCommon( const CHAR *pValue, const bson::BSONObj *pInfo = NULL ) ;

   } ;
   typedef class _sdbDataCenterImpl sdbDataCenterImpl ;

   /*
      _sdbLobImpl
   */
   class _sdbLobImpl : public _sdbLob
   {
   private :
      _sdbLobImpl ( const _sdbLobImpl& other ) ;
      _sdbLobImpl& operator= ( const _sdbLobImpl& other ) ;
#if defined CLIENT_THREAD_SAFE
      ossSpinSLatch           _mutex ;
#endif
      _sdbImpl                *_connection ;
      _sdbCollectionImpl      *_collection ;
      CHAR                    *_pSendBuffer ;
      INT32                   _sendBufferSize ;
      CHAR                    *_pReceiveBuffer ;
      INT32                   _receiveBufferSize ;

      BOOLEAN                 _isOpen ;
      SINT64                  _contextID ;
      INT32                   _mode ;
      bson::OID                _oid ;
      UINT64                  _createTime ;
      SINT64                  _lobSize ;
      SINT64                  _currentOffset ;
      SINT64                  _cachedOffset ;
      UINT32                  _cachedSize ;
      UINT32                  _pageSize ;
      const CHAR              *_dataCache ;

      void _setConnection( _sdb *pConnection ) ;
      void _dropConnection()
      {
         _connection = NULL ;
      }
      void _setCollection( _sdbCollectionImpl *pCollection ) ;
      void _cleanup () ;
      BOOLEAN _dataCached() ;
      void _readInCache( void *buf, UINT32 len, UINT32 *read ) ;
      UINT32 _reviseReadLen( UINT32 needLen ) ;
      INT32 _onceRead( CHAR *buf, UINT32 len, UINT32 *read ) ;

      friend class _sdbImpl ;
      friend class _sdbCollectionImpl ;
   public :
      _sdbLobImpl () ;
      ~_sdbLobImpl () ;

      virtual INT32 close () ;

      virtual INT32 read ( UINT32 len, CHAR *buf, UINT32 *read ) ;

      virtual INT32 write ( const CHAR *buf, UINT32 len ) ;

      virtual INT32 seek ( SINT64 size, SDB_LOB_SEEK whence ) ;

      virtual INT32 isClosed( BOOLEAN &flag ) ;

      virtual INT32 getOid( bson::OID &oid ) ;

      virtual INT32 getSize( SINT64 *size ) ;
         
      virtual INT32 getCreateTime ( UINT64 *millis ) ;

      virtual BOOLEAN isClosed() ;

      virtual bson::OID getOid() ;

      virtual SINT64 getSize() ;
         
      virtual UINT64 getCreateTime () ;
      
   } ;

   typedef class _sdbLobImpl sdbLobImpl ;

   /*
      _sdbImpl
   */
   class _sdbImpl : public _sdb
   {
   private :
      _sdbImpl ( const _sdbImpl& other ) ;
      _sdbImpl& operator=( const _sdbImpl& ) ;
#if defined CLIENT_THREAD_SAFE
      ossSpinSLatch            _mutex ;
#endif
      ossSocket               *_sock ;
      CHAR                     _hostName [ OSS_MAX_HOSTNAME + 1 ] ;
      UINT16                   _port ;
      CHAR                    *_pSendBuffer ;
      INT32                    _sendBufferSize ;
      CHAR                    *_pReceiveBuffer ;
      INT32                    _receiveBufferSize ;
      BOOLEAN                  _endianConvert ;
      BOOLEAN                  _useSSL ;
      std::set<ossValuePtr>    _cursors ;
      std::set<ossValuePtr>    _collections ;
      std::set<ossValuePtr>    _collectionspaces ;
      std::set<ossValuePtr>    _nodes ;
      std::set<ossValuePtr>    _replicaGroups ;
      std::set<ossValuePtr>    _domains ;
      std::set<ossValuePtr>    _dataCenters ;
      std::set<ossValuePtr>    _lobs ;

      void _disconnect () ;
      INT32 _send ( CHAR *pBuffer ) ;
      INT32 _recv ( CHAR **ppBuffer, INT32 *size ) ;
      INT32 _recvExtract ( CHAR **ppBuffer, INT32 *size, SINT64 &contextID,
                           BOOLEAN &result ) ;
      INT32 _reallocBuffer ( CHAR **ppBuffer, INT32 *size, INT32 newSize ) ;
      INT32 _getRetInfo ( CHAR **ppBuffer, INT32 *size,
                          SINT64 contextID, _sdbCursor **ppCursor ) ;
      INT32 _runCommand ( const CHAR *pString, BOOLEAN &result,
                          const BSONObj *arg1 = NULL, const BSONObj *arg2 = NULL,
                          const BSONObj *arg3 = NULL, const BSONObj *arg4 = NULL ) ;
      INT32 _runCommand ( const CHAR *pString,
                          const BSONObj *arg1 = NULL,
                          const BSONObj *arg2 = NULL,
                          const BSONObj *arg3 = NULL,
                          const BSONObj *arg4 = NULL,
                          SINT32 flag = 0,
                          UINT64 reqID = 0,
                          SINT64 numToSkip = -1,
                          SINT64 numToReturn = -1,
                          _sdbCursor **ppCursor = NULL ) ;
      INT32 _buildEmptyCursor( _sdbCursor **ppCursor ) ;
      INT32 _requestSysInfo () ;
      void _regCursor ( _sdbCursorImpl *cursor )
      {
         lock () ;
         _cursors.insert ( (ossValuePtr)cursor ) ;
         unlock () ;
      }
      void _regCollection ( _sdbCollectionImpl *collection )
      {
         lock () ;
         _collections.insert ( (ossValuePtr)collection ) ;
         unlock () ;
      }
      void _regCollectionSpace ( _sdbCollectionSpaceImpl *collectionspace )
      {
         lock () ;
         _collectionspaces.insert ( (ossValuePtr)collectionspace ) ;
         unlock () ;
      }
      void _regNode ( _sdbNodeImpl *node )
      {
         lock () ;
         _nodes.insert ( (ossValuePtr)node ) ;
         unlock () ;
      }
      void _regReplicaGroup ( _sdbReplicaGroupImpl *replicaGroup )
      {
         lock () ;
         _replicaGroups.insert ( (ossValuePtr)replicaGroup ) ;
         unlock () ;
      }
      void _regDomain ( _sdbDomainImpl *domain )
      {
         lock () ;
         _domains.insert ( (ossValuePtr)domain ) ;
         unlock () ;
      }
      void _regDataCenter ( _sdbDataCenterImpl *dc )
      {
         lock () ;
         _dataCenters.insert ( (ossValuePtr)dc ) ;
         unlock () ;
      }
      void _regLob ( _sdbLobImpl *lob )
      {
         lock () ;
         _lobs.insert ( (ossValuePtr)lob ) ;
         unlock () ;
      }
      void _unregCursor ( _sdbCursorImpl *cursor )
      {
         lock () ;
         _cursors.erase ( (ossValuePtr)cursor ) ;
         unlock () ;
      }
      // this function changes collection names for all collections
      // and cursors objects, usually this function should be called by
      // _sdbCollectionImpl::rename
      /*void _changeCollectionName ( const CHAR *pCollectionSpaceName,
                                   const CHAR *pCollectionOldName,
                                   const CHAR *pCollectionNewName )
      {
         std::set<ossValuePtr>::iterator it ;
         INT32 newNameLen = ossStrlen ( pCollectionNewName ) ;
         if ( newNameLen > CLIENT_COLLECTION_NAMESZ )
            return ;
         // rename collection name for all child collection objects
         for ( it = _collections.begin(); it != _collections.end(); ++it )
         {
            _sdbCollectionImpl *collection = (_sdbCollectionImpl*)(*it) ;
            collection->_renameAttempt ( pCollectionOldName,
                                         pCollectionNewName ) ;
         }
      }*/
      void _unregCollection ( _sdbCollectionImpl *collection )
      {
         lock () ;
         _collections.erase ( (ossValuePtr)collection ) ;
         unlock () ;
      }
      void _unregCollectionSpace ( _sdbCollectionSpaceImpl *collectionspace )
      {
         lock () ;
         _collectionspaces.erase ( (ossValuePtr)collectionspace ) ;
         unlock () ;
      }
      void _unregNode ( _sdbNodeImpl *node )
      {
         lock () ;
         _nodes.erase ( (ossValuePtr)node ) ;
         unlock () ;
      }
      void _unregReplicaGroup ( _sdbReplicaGroupImpl *replicaGroup )
      {
         lock () ;
         _replicaGroups.erase ( (ossValuePtr)replicaGroup ) ;
         unlock () ;
      }
      void _unregDomain ( _sdbDomainImpl *domain )
      {
         lock () ;
         _domains.erase ( (ossValuePtr)domain ) ;
         unlock () ;
      }
      void _unregDataCenter ( _sdbDataCenterImpl *dc )
      {
         lock () ;
         _dataCenters.erase ( (ossValuePtr)dc ) ;
         unlock () ;
      }
      void _unregLob ( _sdbLobImpl *lob )
      {
         lock () ;
         _lobs.erase ( (ossValuePtr)lob ) ;
         unlock () ;
      }

      INT32 _connect( const CHAR *pHostName,
                      UINT16 port ) ;

      friend class _sdbCollectionSpaceImpl ;
      friend class _sdbCollectionImpl ;
      friend class _sdbCursorImpl ;
      friend class _sdbNodeImpl ;
      friend class _sdbReplicaGroupImpl ;
      friend class _sdbDomainImpl ;
      friend class _sdbDataCenterImpl ;
      friend class _sdbLobImpl ;
   public :
      _sdbImpl ( BOOLEAN useSSL = FALSE ) ;
      ~_sdbImpl () ;
      INT32 connect ( const CHAR *pHostName,
                      UINT16 port ) ;
      INT32 connect ( const CHAR *pHostName,
                      UINT16 port,
                      const CHAR *pUsrName,
                      const CHAR *pPasswd ) ;
      INT32 connect ( const CHAR *pHostName,
                      const CHAR *pServiceName ) ;
      INT32 connect ( const CHAR *pHostName,
                      const CHAR *pServiceName,
                      const CHAR *pUsrName,
                      const CHAR *pPasswd ) ;
      INT32 connect ( const CHAR **pConnAddrs,
                      INT32 arrSize,
                      const CHAR *pUsrName,
                      const CHAR *pPasswd ) ;
      void disconnect () ;
      BOOLEAN isConnected ()
      { return NULL != _sock ; }

      INT32 createUsr( const CHAR *pUsrName,
                       const CHAR *pPasswd ) ;

      INT32 removeUsr( const CHAR *pUsrName,
                       const CHAR *pPasswd ) ;

      INT32 getSnapshot ( _sdbCursor **cursor,
                          INT32 snapType,
                          const BSONObj &condition = _sdbStaticObject,
                          const BSONObj &selector = _sdbStaticObject,
                          const BSONObj &orderBy = _sdbStaticObject
                         ) ;

      INT32 getSnapshot ( sdbCursor &cursor,
                          INT32 snapType,
                          const BSONObj &condition = _sdbStaticObject,
                          const BSONObj &selector = _sdbStaticObject,
                          const BSONObj &orderBy = _sdbStaticObject
                         )
      {
         return getSnapshot ( &cursor.pCursor,
                              snapType,
                              condition,
                              selector,
                              orderBy ) ;
      }

      INT32 getList ( _sdbCursor **cursor,
                      INT32 snapType,
                      const BSONObj &condition = _sdbStaticObject,
                      const BSONObj &selector = _sdbStaticObject,
                      const BSONObj &orderBy = _sdbStaticObject
                    ) ;

      INT32 getList ( sdbCursor &cursor,
                      INT32 snapType,
                      const BSONObj &condition = _sdbStaticObject,
                      const BSONObj &selector = _sdbStaticObject,
                      const BSONObj &orderBy = _sdbStaticObject
                    )
      {
         return getList ( &cursor.pCursor, snapType, condition,
                          selector, orderBy ) ;
      }

      INT32 resetSnapshot ( const BSONObj &condition = _sdbStaticObject ) ;

      #if defined CLIENT_THREAD_SAFE
      void lock ()
      {
         _mutex.get () ;
      }
      void unlock ()
      {
         _mutex.release () ;
      }
      #else
      void lock ()
      {
      }
      void unlock ()
      {
      }
      #endif
      INT32 getCollection ( const CHAR *pCollectionFullName,
                            _sdbCollection **collection ) ;

      INT32 getCollection ( const CHAR *pCollectionFullName,
                            sdbCollection &collection )
      {
         return getCollection ( pCollectionFullName, &collection.pCollection ) ;
      }

      INT32 getCollectionSpace ( const CHAR *pCollectionSpaceName,
                                 _sdbCollectionSpace **cs ) ;

      INT32 getCollectionSpace ( const CHAR *pCollectionSpaceName,
                                 sdbCollectionSpace &cs )
      {
         return getCollectionSpace ( pCollectionSpaceName,
                                     &cs.pCollectionSpace ) ;
      }

      INT32 createCollectionSpace ( const CHAR *pCollectionSpaceName,
                                    INT32 iPageSize,
                                    _sdbCollectionSpace **cs ) ;

      INT32 createCollectionSpace ( const CHAR *pCollectionSpaceName,
                                    INT32 iPageSize,
                                    sdbCollectionSpace &cs )
      {
         return createCollectionSpace ( pCollectionSpaceName, iPageSize,
                                        &cs.pCollectionSpace ) ;
      }

      INT32 createCollectionSpace ( const CHAR *pCollectionSpaceName,
                                    const bson::BSONObj &options,
                                    _sdbCollectionSpace **cs
                                  ) ;

      INT32 createCollectionSpace ( const CHAR *pCollectionSpaceName,
                                    const bson::BSONObj &options,
                                    sdbCollectionSpace &cs
                                  )
      {
         return createCollectionSpace ( pCollectionSpaceName, options,
                                        &cs.pCollectionSpace ) ;
      }

      INT32 dropCollectionSpace ( const CHAR *pCollectionSpaceName ) ;

      INT32 listCollectionSpaces ( _sdbCursor **result ) ;

      INT32 listCollectionSpaces ( sdbCursor &result )
      {
         return listCollectionSpaces ( &result.pCursor ) ;
      }

      INT32 listCollections ( _sdbCursor **result ) ;

      INT32 listCollections ( sdbCursor &result )
      {
         return listCollections ( &result.pCursor ) ;
      }

      INT32 listReplicaGroups ( _sdbCursor **result ) ;

      INT32 listReplicaGroups ( sdbCursor &result )
      {
         return listReplicaGroups ( &result.pCursor ) ;
      }

      INT32 getReplicaGroup ( const CHAR *pName, _sdbReplicaGroup **result ) ;

      INT32 getReplicaGroup ( const CHAR *pName, sdbReplicaGroup &result )
      {
         return getReplicaGroup ( pName, &result.pReplicaGroup ) ;
      }

      INT32 getReplicaGroup ( INT32 id, _sdbReplicaGroup **result ) ;

      INT32 getReplicaGroup ( INT32 id, sdbReplicaGroup &result )
      {
         return getReplicaGroup ( id, &result.pReplicaGroup ) ;
      }

      INT32 createReplicaGroup ( const CHAR *pName, _sdbReplicaGroup **replicaGroup ) ;

      INT32 createReplicaGroup ( const CHAR *pName, sdbReplicaGroup &replicaGroup )
      {
         return createReplicaGroup ( pName, &replicaGroup.pReplicaGroup ) ;
      }

      INT32 removeReplicaGroup ( const CHAR *pName ) ;

      INT32 createReplicaCataGroup (  const CHAR *pHostName,
                               const CHAR *pServiceName,
                               const CHAR *pDatabasePath,
                               const BSONObj &configure ) ;

      INT32 activateReplicaGroup ( const CHAR *pName, _sdbReplicaGroup **replicaGroup ) ;
      INT32 activateReplicaGroup ( const CHAR *pName, sdbReplicaGroup &replicaGroup )
      {
         return activateReplicaGroup( pName, &replicaGroup.pReplicaGroup ) ;
      }

      // sql
      INT32 execUpdate( const CHAR *sql ) ;
      INT32 exec( const CHAR *sql, sdbCursor &result )
      {
         return exec( sql, &result.pCursor ) ;
      }
      INT32 exec( const CHAR *sql, _sdbCursor **result ) ;

      // transation
      INT32 transactionBegin() ;
      INT32 transactionCommit() ;
      INT32 transactionRollback() ;

      // flush config file
      INT32 flushConfigure( const bson::BSONObj &options ) ;

      // stored procedure
      INT32 crtJSProcedure ( const CHAR *code ) ;
      INT32 rmProcedure( const CHAR *spName ) ;
      INT32 listProcedures( _sdbCursor **cursor, const bson::BSONObj &condition ) ;
      INT32 listProcedures( sdbCursor &cursor, const bson::BSONObj &condition )
      {
         return listProcedures ( &cursor.pCursor, condition ) ;
      }

      INT32 evalJS( const CHAR *code,
                    SDB_SPD_RES_TYPE &type,
                    _sdbCursor **cursor,
                    bson::BSONObj &errmsg ) ; 
      INT32 evalJS( const CHAR *code,
                    SDB_SPD_RES_TYPE &type,
                    sdbCursor &cursor,
                    bson::BSONObj &errmsg )
      {
         return evalJS( code, type, &cursor.pCursor, errmsg ) ;
      }

      // bakeup
      INT32 backupOffline ( const bson::BSONObj &options) ;
      INT32 listBackup ( _sdbCursor **cursor,
                         const bson::BSONObj &options,
                         const bson::BSONObj &condition = _sdbStaticObject,
                         const bson::BSONObj &selector = _sdbStaticObject,
                         const bson::BSONObj &orderBy = _sdbStaticObject) ;
      INT32 listBackup ( sdbCursor &cursor,
                         const bson::BSONObj &options,
                         const bson::BSONObj &condition = _sdbStaticObject,
                         const bson::BSONObj &selector = _sdbStaticObject,
                         const bson::BSONObj &orderBy = _sdbStaticObject)
      {
         return listBackup ( &cursor.pCursor, options, condition, selector, orderBy ) ;
      }
      INT32 removeBackup ( const bson::BSONObj &options ) ;

      // task
      INT32 listTasks ( _sdbCursor **cursor,
                        const bson::BSONObj &condition = _sdbStaticObject,
                        const bson::BSONObj &selector = _sdbStaticObject,
                        const bson::BSONObj &orderBy = _sdbStaticObject,
                        const bson::BSONObj &hint = _sdbStaticObject) ;
      INT32 listTasks ( sdbCursor &cursor,
                        const bson::BSONObj &condition = _sdbStaticObject,
                        const bson::BSONObj &selector = _sdbStaticObject,
                        const bson::BSONObj &orderBy = _sdbStaticObject,
                        const bson::BSONObj &hint = _sdbStaticObject)
      {
         return listTasks ( &cursor.pCursor, condition,
                             selector, orderBy, hint ) ;
      }
      INT32 waitTasks ( const SINT64 *taskIDs,
                        SINT32 num ) ;
      INT32 cancelTask ( SINT64 taskID,
                         BOOLEAN isAsync ) ;
      // set session attribute
      INT32 setSessionAttr ( const bson::BSONObj &options =
                                     _sdbStaticObject) ;
      // close all cursors
      INT32 closeAllCursors ();

      // connection is closed
      INT32 isValid( BOOLEAN *result ) ;
      BOOLEAN isValid() ;

      // domain
      INT32 createDomain ( const CHAR *pDomainName,
                           const bson::BSONObj &options,
                           _sdbDomain **domain ) ;
      
      INT32 createDomain ( const CHAR *pDomainName,
                           const bson::BSONObj &options,
                           sdbDomain &domain )
      {
         return createDomain ( pDomainName, options, &domain.pDomain ) ;
      }

      INT32 dropDomain ( const CHAR *pDomainName ) ;

      INT32 getDomain ( const CHAR *pDomainName,
                        _sdbDomain **domain ) ;

      INT32 getDomain ( const CHAR *pDomainName,
                        sdbDomain &domain )
      {
         return getDomain ( pDomainName, &domain.pDomain ) ;
      }

      INT32 listDomains ( _sdbCursor **cursor,
                          const bson::BSONObj &condition,
                          const bson::BSONObj &selector,
                          const bson::BSONObj &orderBy,
                          const bson::BSONObj &hint
                         ) ;
      
      INT32 listDomains ( sdbCursor &cursor,
                          const bson::BSONObj &condition,
                          const bson::BSONObj &selector,
                          const bson::BSONObj &orderBy,
                          const bson::BSONObj &hint
                         )
      {
         return listDomains ( &cursor.pCursor, condition, selector, orderBy, hint ) ;
      }

      INT32 getDC( _sdbDataCenter **dc ) ;
      
      INT32 getDC( sdbDataCenter &dc )
      {
         return getDC( &dc.pDC ) ;
      }

/*      INT32 modifyConfig ( INT32 nodeID,
                           std::map<std::string,std::string> &config ) ;

      INT32 getConfig ( INT32 nodeID,
                        std::map<std::string,std::string> &config ) ;

      INT32 modifyConfig ( std::map<std::string,std::string> &config )
      {
         return modifyConfig ( CURRENT_NODEID, config ) ;
      }

      INT32 getConfig ( std::map<std::string,std::string> &config )
      {
         return getConfig ( CURRENT_NODEID, config ) ;
      }*/
   } ;
   typedef class _sdbImpl sdbImpl ;
}

#endif
