(function(){
   var sacApp = window.SdbSacManagerModule ;
   var GridId ;
   sacApp.controllerProvider.register( 'Data.Operate.Index.Ctrl', function( $scope, $compile, $location, SdbRest, InheritSize, SdbFunction ){
      var clusterName = SdbFunction.LocalData( 'SdbClusterName' ) ;
      if( clusterName == null )
      {
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

      printfDebug( 'Cluster: ' + clusterName + ', Module: ' + moduleName + ', Mode: ' + moduleMode ) ;
      
      //修正宽高
      InheritSize.append( $( '#OperateIndex' ) ) ;
      $( '#OperateIndex > div' ).each( function( index, ele ){
         InheritSize.append( ele ) ;
      } ) ;

      //初始化
      _DataOperateIndex.init( $scope, moduleName, moduleMode ) ;

      //页面跳转
      $scope.gotoRecord = function( listIndex ){
         _DataOperateIndex.gotoRecord( $scope, $location, SdbFunction, listIndex ) ;
      }

      //Lob页面跳转
      $scope.gotoLob = function( listIndex ){
         _DataOperateIndex.gotoLob( $scope, $location, SdbFunction, listIndex ) ;
      }

      //上一页
      $scope.previous = function(){
         _DataOperateIndex.previous( $scope, $compile ) ;
      }

      //检查输入的页数格式
      $scope.checkCurrent = function(){
         _DataOperateIndex.checkCurrent( $scope ) ;
      }

      //跳转到指定页
      $scope.gotoPate = function( event ){
         _DataOperateIndex.gotoPate( $scope, $compile, event ) ;
      }

      //下一页
      $scope.nextPage = function(){
         _DataOperateIndex.nextPage( $scope, $compile ) ;
      }

      //获取集合列表
      $scope.getCLList = function(){
         _DataOperateIndex.getCLList( $scope, $compile, SdbRest, moduleName, moduleMode ) ;
      }
      
      _DataOperateIndex.getCLList( $scope, $compile, SdbRest, moduleName, moduleMode ) ;
   } ) ;
}());