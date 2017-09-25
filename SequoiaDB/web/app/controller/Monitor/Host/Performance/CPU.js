(function(){
   var sacApp = window.SdbSacManagerModule ;
   //¿ØÖÆÆ÷
   sacApp.controllerProvider.register( 'Performance.CPU.Index.Ctrl', function( $scope, $compile, SdbRest, SdbFunction, InheritSize, SdbFunction, FormModal ){
  
      $scope.charts = {}; 
      $scope.charts['Storage'] = {} ;
      $scope.charts['Storage']['options'] = window.SdbSacManagerConf.DiskStorageEchart ;
      $scope.charts['Storage']['value'] = [ [ 0, 0, true, false ] ] ;

      $scope.charts['Read'] = {} ;
      $scope.charts['Read']['options'] = window.SdbSacManagerConf.DiskReadEchart ;
      $scope.charts['Read']['value'] = [ [ 0, 0, true, false ] ] ;

      $scope.charts['Write'] = {} ;
      $scope.charts['Write']['options'] = window.SdbSacManagerConf.DiskWriteEchart ;
      $scope.charts['Write']['value'] = [ [ 0, 0, true, false ] ] ;
      
      $scope.charts['Cpu'] = {} ;
      $scope.charts['Cpu']['options'] = window.SdbSacManagerConf.CpuEchart ;
      $scope.charts['Cpu']['value'] = [ [ 0, 0, true, false ] ] ;
   } ) ;

}());