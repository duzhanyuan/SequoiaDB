#include <stdio.h>
#include <gtest/gtest.h>
#include "testcommon.h"
#include "client.h"

TEST(lob,sdbNext)
{
   INT32 rc = SDB_OK ;
   // initialize the word environment
   rc = initEnv( HOST, SERVER, USER, PASSWD ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // initialize local variables
   sdbConnectionHandle db            = 0 ;
   sdbCollectionHandle cl            = 0 ;
   sdbCursorHandle cur               = 0 ;
   sdbLobHandle lob                  = 0 ;
   INT32 NUM                         = 10 ;
   SINT64 count                      = 0 ;
   bson_oid_t oid ;
   bson obj ;
   INT32 bufSize = 1000 ;
   SINT64 lobSize = -1 ;
   UINT64 createTime = -1 ;
   CHAR buf[bufSize] ;
   for ( INT32 i = 0; i < bufSize; i++ )
   {
      buf[i] = 'a' ;
   }
   // connect to database
   rc = sdbConnect ( HOST, SERVER, USER, PASSWD, &db ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get collection
   rc = getCollection ( db,
                        COLLECTION_FULL_NAME,
                        &cl ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // open lob 
   bson_oid_gen( &oid ) ; 
   rc = sdbOpenLob( cl, &oid, SDB_LOB_CREATEONLY, &lob ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get lob size 
   rc = sdbGetLobSize( lob, &lobSize ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   ASSERT_EQ( 0, lobSize ) ;
   // get lob create time
   rc = sdbGetLobCreateTime( lob, &createTime ) ;
   ASSERT_EQ( 0, createTime ) ;
   ASSERT_EQ( 0, createTime ) ;
   // write lob 
   rc = sdbWriteLob( lob, buf, bufSize ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get lob size 
   rc = sdbGetLobSize( lob, &lobSize ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   ASSERT_EQ( bufSize, lobSize ) ;
   // write lob
   rc = sdbWriteLob( lob, buf, bufSize ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // get lob size
   rc = sdbGetLobSize( lob, &lobSize ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   ASSERT_EQ( 2 * bufSize, lobSize ) ;
   // get lob create time
   rc = sdbGetLobCreateTime( lob, &createTime ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   ASSERT_EQ( 0, createTime ) ;
   // close lob
   rc = sdbCloseLob ( &lob ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   // remove lob
   rc = sdbRemoveLob( cl, &oid ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
    
   // disconnect the connection
   sdbDisconnect ( db ) ;
   //release the local variables
   sdbReleaseCursor ( cur ) ;
   sdbReleaseCollection ( cl ) ;
   sdbReleaseConnection ( db ) ;
}

