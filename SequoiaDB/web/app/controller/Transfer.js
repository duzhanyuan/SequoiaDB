(function(){
   var sacApp = window.SdbSacManagerModule ;
   //控制器
   sacApp.controllerProvider.register( 'Transfer', function( $scope, $rootScope, $location, SdbRest, SdbFunction ){
      //SdbFunction.TransferCtr( $location, SdbRest ) ;
      $rootScope.updateNav( function( instanceList, navMenu ){
         var cursorIndex = _IndexLeft.getActiveIndex( $rootScope, SdbFunction, navMenu ) ;
         $rootScope.gotoModule( cursorIndex[0], cursorIndex[1], cursorIndex[2] ) ;
      } ) ;
   } ) ;
}());