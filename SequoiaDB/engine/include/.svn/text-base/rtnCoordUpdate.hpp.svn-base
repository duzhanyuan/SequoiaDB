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

   Source File Name = rtnCoordUpdate.hpp

   Descriptive Name =

   When/how to use:

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          11/28/2012  YW  Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef RTNCOORDUPDATE_HPP__
#define RTNCOORDUPDATE_HPP__

#include "rtnCoordOperator.hpp"
#include "../bson/bson.h"

namespace engine
{
   class rtnCoordUpdate : public rtnCoordTransOperator
   {
   public:
      virtual INT32 execute( MsgHeader *pMsg,
                             pmdEDUCB *cb,
                             INT64 &contextID,
                             rtnContextBuf *buf ) ;

   protected:
      virtual void               _prepareForTrans( pmdEDUCB *cb,
                                                   MsgHeader *pMsg ) ;

      virtual INT32              _prepareMainCLOp( CoordCataInfoPtr &cataInfo,
                                                   CoordGroupSubCLMap &grpSubCl,
                                                   rtnSendMsgIn &inMsg,
                                                   rtnSendOptions &options,
                                                   netMultiRouteAgent *pRouteAgent,
                                                   pmdEDUCB *cb,
                                                   rtnProcessResult &result,
                                                   ossValuePtr &outPtr ) ;

      virtual void               _doneMainCLOp( ossValuePtr itPtr,
                                                CoordCataInfoPtr &cataInfo,
                                                CoordGroupSubCLMap &grpSubCl,
                                                rtnSendMsgIn &inMsg,
                                                rtnSendOptions &options,
                                                netMultiRouteAgent *pRouteAgent,
                                                pmdEDUCB *cb,
                                                rtnProcessResult &result ) ;

   private:

      INT32 checkIfIncludeShardingKey ( const CoordCataInfoPtr &cataInfo,
                                        const CHAR *pUpdator,
                                        BOOLEAN &isInclude,
                                        pmdEDUCB *cb ) ;

      INT32 checkModifierForSubCL ( const CoordSubCLlist &subCLList,
                                    const CHAR *pUpdator,
                                    pmdEDUCB *cb ) ;

      BSONObj _buildNewSelector( const BSONObj &selector,
                                 const CoordSubCLlist &subCLList ) ;
   };
}

#endif


