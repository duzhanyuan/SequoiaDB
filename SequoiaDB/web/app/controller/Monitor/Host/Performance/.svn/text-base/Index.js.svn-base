(function(){
   var sacApp = window.SdbSacManagerModule ;
   //控制器
   sacApp.controllerProvider.register( 'Host.Performance.Index.Ctrl', function( $scope, SdbRest, SdbFunction, InheritSize, FormModal ){
    
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
      
      $scope.disk['Ram'] = {} ;
      $scope.disk['Ram']['options'] = window.SdbSacManagerConf.RamBarEchart ;
      $scope.disk['Ram']['value'] = [ [ 0, 0, true, false ] ] ;
   } ) ;

}());