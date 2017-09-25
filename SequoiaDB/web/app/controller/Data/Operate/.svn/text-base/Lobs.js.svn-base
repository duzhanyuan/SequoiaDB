(function(){
   var sacApp = window.SdbSacManagerModule ;
   var GridId ;
   sacApp.controllerProvider.register( 'Data.Lob.Lobs.Ctrl', function( $scope, $compile, $location, SdbRest, InheritSize, SdbFunction, FormModal){
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

      //调整外层div宽高
      InheritSize.append( $( '#LobIndex' ) ) ;
      $( '#LobIndex > div' ).each( function( index, ele ){
         InheritSize.append( ele ) ;
      } ) ;
      $( '#GridParent > div' ).each( function( index, ele ){
         InheritSize.append( ele ) ;
      } ) ;

      //cs.cl
      $scope.fullName = csName + '.' + clName ;
      //是否非条件查询
      $scope.isNotFilter = true ;
      //每页显示的记录数
      $scope.limit = 30 ;
      //Lob集合
      $scope.lobContent = [] ;
      //当前显示页
      $scope.setCurrent = 1 ;
      $scope.current = 1 ;
      //总页数
      $scope.total = 0 ;
      //总记录数描述
      $scope.recordTotal = '' ;

      //上一页
      $scope.previous = function(){
         _DataOperateLob.previous( $scope, $compile, SdbFunction ) ;
      }

      //下一页
      $scope.nextPage = function(){
         _DataOperateLob.nextPage( $scope, $compile, SdbFunction ) ;
      }

      //检查输入的页数格式
      $scope.checkCurrent = function(){
         _DataOperateLob.checkCurrent( $scope ) ;
      }

      //跳转到指定页
      $scope.gotoPate = function( event ){
         _DataOperateLob.gotoPate( $scope, $compile, SdbFunction, event ) ;
      }

      //查询所有Lob
      $scope.queryAll = function(){
         _DataOperateLob.queryAll( $scope, $compile, SdbFunction, SdbRest ) ;
      }
    
      //删除Lob记录
      $scope.LobDelete = function( index ){
         _DataOperateLob.LobDelete( $scope, SdbRest, index ) ;
      }

      //查询lob
      $scope.LobQuery = function(){
         _DataOperateLob.LobQuery( $scope, $compile, SdbFunction ) ;
      }
      
      //显示lob的详细信息
      $scope.showLobInfo = function( oid ){
         _DataOperateLob.showLobInfo( $scope, oid ) ;
      }

      //查询所有Lob
      _DataOperateLob.queryAll( $scope, $compile, SdbFunction, SdbRest ) ;
   } ) ;
}()) ;