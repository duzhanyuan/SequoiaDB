(function(){
   var sacApp = window.SdbSacManagerModule ;
   //¿ØÖÆÆ÷
   sacApp.controllerProvider.register( 'Performance.Disk.Index.Ctrl', function( $scope, $compile, SdbRest, SdbFunction, InheritSize, SdbFunction, FormModal ){
      
    
      //var chart = echarts.init( document.getElementById('test') ) ;
      //$( window ).resize( function(){
      //  chart.resize() ;
      //} ) ;
      $scope.disk = {}; 
      $scope.disk['Storage'] = {} ;
      $scope.disk['Storage']['options'] = window.SdbSacManagerConf.DiskStorageEchart ;
      $scope.disk['Storage']['value'] = [ [ 0, 0, true, false ] ] ;

      $scope.disk['Read'] = {} ;
      $scope.disk['Read']['options'] = window.SdbSacManagerConf.DiskReadEchart ;
      $scope.disk['Read']['value'] = [ [ 0, 0, true, false ] ] ;

      $scope.disk['Write'] = {} ;
      $scope.disk['Write']['options'] = window.SdbSacManagerConf.DiskWriteEchart ;
      $scope.disk['Write']['value'] = [ [ 0, 0, true, false ] ] ;
      
      $scope.disk['Cpu'] = {} ;
      $scope.disk['Cpu']['options'] = window.SdbSacManagerConf.CpuEchart ;
      $scope.disk['Cpu']['value'] = [ [ 0, 0, true, false ] ] ;
   } ) ;

}());