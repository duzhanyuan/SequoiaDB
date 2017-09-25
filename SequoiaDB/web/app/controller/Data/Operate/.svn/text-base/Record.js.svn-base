(function(){
   var sacApp = window.SdbSacManagerModule ;
   //控制器
   sacApp.controllerProvider.register( 'Data.Operate.Record.Ctrl', function( $scope, $compile, $location, Loading, SdbRest, InheritSize, SdbFunction, FormModal ){
      var clusterName = SdbFunction.LocalData( 'SdbClusterName' ) ;
      if( clusterName == null )
      {
         SdbFunction.LocalData( 'SdbModuleMode', null ) ;
         SdbFunction.LocalData( 'SdbModuleName', null ) ;
         window.location.href = '/deployment/index.html' ;
         return;
      }

      var moduleMode = SdbFunction.LocalData( 'SdbModuleMode' ) ;
      var moduleName = SdbFunction.LocalData( 'SdbModuleName' ) ;
      if( moduleMode == null || moduleName == null )
      {
         $location.path( 'Data/Overview/Index' ) ;
         return;
      }

      var csName = SdbFunction.LocalData( 'SdbCsName' ) ;
      var clName = SdbFunction.LocalData( 'SdbClName' ) ;
      var clType = SdbFunction.LocalData( 'SdbClType' ) ;
      if( csName == null || clName == null || clType == null )
      {
         $location.path( 'Data/Operate/Index' ) ;
         return;
      }

      printfDebug( 'Cluster: ' + clusterName + ', Module: ' + moduleName + ', cs: ' + csName + ', cl: ' + clName ) ;

      //修正宽高
      InheritSize.append( $( '#OperateRecord' ) ) ;
      $( '#OperateRecord > div' ).each( function( index, ele ){
         InheritSize.append( ele ) ;
      } ) ;
      $( '#GridParent > div' ).each( function( index, ele ){
         InheritSize.append( ele ) ;
      } ) ;

      $scope.fullName = csName + '.' + clName ;
      $scope.clType = clType ;

      //初始化
      _DataOperateRecord.init( $scope ) ;

      _DataOperateRecord.getIndexInfo( $scope, SdbRest ) ;

      //查询所有
      $scope.queryAll = function(){
         _DataOperateRecord.queryAll( $scope, SdbRest, SdbFunction ) ;
      }

      //上一页
      $scope.previous = function(){
         _DataOperateRecord.previous( $scope, SdbRest, SdbFunction ) ;
      }

      //下一页
      $scope.nextPage = function(){
         _DataOperateRecord.nextPage( $scope, SdbRest, SdbFunction ) ;
      }

      //跳转到指定页
      $scope.gotoPate = function( event ){
         _DataOperateRecord.gotoPate( $scope, SdbRest, SdbFunction, event ) ;
      }

      //检查输入的页数格式
      $scope.checkCurrent = function(){
         _DataOperateRecord.checkCurrent( $scope ) ;
      }

      //显示方式
      $scope.show = function( type ){
         _DataOperateRecord.show( $scope, $compile, SdbFunction, type ) ;
      }

      //查询数据
      _DataOperateRecord.queryRecord( $scope, SdbRest, SdbFunction, $scope.queryFilter, true ) ;

      //创建插入弹窗
      $scope.Insert = function( recordIndex ){
         _DataOperateRecord.createInsertModel( $scope, SdbRest, SdbFunction, recordIndex ) ;
      }

      //创建编辑记录弹窗
      $scope.Edit = function( recordIndex ){
         _DataOperateRecord.createEditModel( $scope, SdbRest, SdbFunction, recordIndex ) ;
      }

      //创建查询弹窗
      $scope.Query = function(){
         _DataOperateRecord.createQueryModel( $scope, SdbRest, SdbFunction ) ;
      }

      //创建更新弹窗
      $scope.Update = function(){
         _DataOperateRecord.createUpdateModel( $scope, SdbRest, SdbFunction ) ;
      }

      //创建删除弹窗
      $scope.Delete = function(){
         _DataOperateRecord.createDeleteModel( $scope, SdbRest, SdbFunction ) ;
      }

      //创建删除记录提示
      $scope.DeleteRecord = function( recordIndex ){
         _DataOperateRecord.createDeleteRecordModel( $scope, SdbRest, SdbFunction, recordIndex ) ;
      }
   } ) ;
}());