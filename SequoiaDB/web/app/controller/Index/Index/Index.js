(function(){
   var sacApp = window.SdbSacManagerModule ;
   //控制器
   sacApp.controllerProvider.register( 'Index.Index.Index.Ctrl', function( $scope, SdbRest, InheritSize ){

      InheritSize.append( $( '#ClusterBox' ) ) ;
      $( '#ClusterBox > div' ).each( function( index, ele ){
         InheritSize.append( ele ) ;
      } ) ;

      function getCLusterList(){
         var data = { 'cmd': 'query cluster' } ;
         SdbRest.OmOperation( data, function( json ){
            $scope.QueryCluster = json ;
            $scope.$apply() ;
         }, function( errorInfo ){
         }, function(){
            _IndexPublic.createErrorModel( $scope, $scope.autoLanguage( '网络连接错误，请尝试按F5刷新浏览器。' ) ) ;
         }, null, false ) ;
      }
      getCLusterList() ;
   } ) ;
}());