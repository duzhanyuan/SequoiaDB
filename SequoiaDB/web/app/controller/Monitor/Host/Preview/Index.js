(function(){
   var sacApp = window.SdbSacManagerModule ;
   //控制器
   sacApp.controllerProvider.register( 'Monitor.Preview.Index.Ctrl', function( $scope, SdbRest, SdbFunction, InheritSize, FormModal ){
      InheritSize.append( $( '#MonitorPreview' ) ) ;
         $( '#MonitorPreview > div' ).each( function( index, ele ){
            InheritSize.append( ele ) ;
      } ) ;
   } ) ;
   //记录视图

}());