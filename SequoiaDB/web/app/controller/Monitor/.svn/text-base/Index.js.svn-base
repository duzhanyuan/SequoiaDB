(function(){
   var sacApp = window.SdbSacManagerModule ;
   //控制器
   sacApp.controllerProvider.register( 'Data.SQL.Index.Ctrl', function( $scope, $location, SdbFunction, SdbRest, InheritSize, FormModal ){
      var clusterName = SdbFunction.LocalData( 'SdbClusterName' ) ;
      if( clusterName == null )
      {
         SdbFunction.LocalData( 'SdbModuleMode', null ) ;
         SdbFunction.LocalData( 'SdbModuleName', null ) ;
         window.location.href = '/deployment/index.html' ;
         return;
      }

      InheritSize.append( $( '#SQLIndex' ) ) ;
      InheritSize.append( $( '#SQLIndex' ) ) ;
      $( '#SQLIndex > div' ).each( function( index, ele ){
         InheritSize.append( ele ) ;
      } ) ;

      $scope.parseSql = function(){
         
         var sql = SQLParser.parse( $scope.sqlcmd ).toString() ;
         $scope.sqlcmd2 = sql ;
      }
   } ) ;
}());