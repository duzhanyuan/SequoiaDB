(function(){
   var sacApp = window.SdbSacManagerModule ;
   //¿ØÖÆÆ÷
   sacApp.controllerProvider.register( 'Performance.Memory.Index.Ctrl', function( $scope, $compile, SdbRest, SdbFunction, InheritSize, SdbFunction, FormModal ){

      InheritSize.append( $( '#PerformanceMemory' ) ) ;
         $( '#PerformanceMemory > div' ).each( function( index, ele ){
            InheritSize.append( ele ) ;
      } ) ;
      $( '.content > div' ).each( function( index, ele ){
            InheritSize.append( ele ) ;
      } ) ;
      $scope.charts = {}; 

      $scope.charts['Memory'] = {} ;
      $scope.charts['Memory']['options'] = window.SdbSacManagerConf.MemoryEchart ;
      $scope.charts['Memory']['value'] = [ [ 0, 0, true, false ] ] ;
      
      $scope.charts['MemoryBar'] = {} ;
      $scope.charts['MemoryBar']['options'] = window.SdbSacManagerConf.MemoryLessEchart ;
      $scope.charts['MemoryBar']['value'] = [ [ 0, 0, true, false ] ] ;
   } ) ;

}());