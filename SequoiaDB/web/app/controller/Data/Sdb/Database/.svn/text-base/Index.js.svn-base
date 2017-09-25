(function(){
   var sacApp = window.SdbSacManagerModule ;
   //控制器
   sacApp.controllerProvider.register( 'Data.Database.Index.Ctrl', function( $scope, $location, SdbFunction, SdbRest, InheritSize, FormModal ){
      var clusterName = SdbFunction.LocalData( 'SdbClusterName' ) ;
      var moduleType = SdbFunction.LocalData( 'SdbModuleType' ) ;
      var moduleMode = SdbFunction.LocalData( 'SdbModuleMode' ) ;
      var moduleName = SdbFunction.LocalData( 'SdbModuleName' ) ;
      if( clusterName == null || moduleType != 'sequoiadb' || moduleMode == null || moduleName == null )
      {
         $location.path( '/Transfer' ) ;
         return;
      }

      var isHideSubCl = SdbFunction.LocalData( 'SdbHidePartition' ) ? true : false ;

      //初始化参数
      _DataDatabaseIndex.init( $scope, clusterName, moduleName, moduleMode, isHideSubCl ) ;

      //获取分区组列表
      _DataDatabaseIndex.getGroupList( $scope, SdbRest ) ;
      
      //跳到记录页面
      $scope.gotoRecord = function( csIndex, clIndex ){
         _DataDatabaseIndex.gotoRecord( $scope, $location, SdbFunction, csIndex, clIndex ) ;
      }

      //跳到Lob页面
      $scope.gotoLob = function( csIndex, clIndex ){
         _DataDatabaseIndex.gotoLob( $scope, $location, SdbFunction, csIndex, clIndex ) ;
      }

      //过滤CS和cl
      $scope.search = function( fullName ){
         _DataDatabaseIndex.searchCSAndCL( $scope, fullName ) ;
      }

      //展开cs下所有的cl
      $scope.clTableShow = function( csName ){
         _DataDatabaseIndex.clTableShow( $scope, csName ) ;
      }

      //收起cs下所有的cl
      $scope.clTableHide = function( csName ){
         _DataDatabaseIndex.clTableHide( $scope, csName ) ;
      }

      //展示CS属性
      $scope.showCSInfo = function( index ){
         _DataDatabaseIndex.showCSInfo( $scope, index ) ;
      }

      //展示CL属性
      $scope.showCLInfo = function( csIndex, clIndex ){
         _DataDatabaseIndex.showCLInfo( $scope, csIndex, clIndex ) ;
      }

      //打开 创建集合空间 的窗口
      $scope.showCreateCS = function(){
         _DataDatabaseIndex.showCreateCS( $scope, SdbRest ) ;
      }

      //打开 删除集合空间 的窗口
      $scope.showRemoveCS = function(){
         _DataDatabaseIndex.showRemoveCS( $scope, SdbRest ) ;
      }

      //打开 创建集合 的窗口
      $scope.showCreateCL = function(){
         _DataDatabaseIndex.showCreateCL( $scope, SdbRest ) ;
      }

      //打开 删除集合 的窗口
      $scope.showRemoveCL = function(){
         _DataDatabaseIndex.showRemoveCL( $scope, SdbRest ) ;
      }

      //打开 挂载集合 的窗口
      $scope.showAttachCL = function(){
         _DataDatabaseIndex.showAttachCL( $scope, SdbRest ) ;
      }

      //打开 切分数据 的窗口
      $scope.showSplit = function(){
         _DataDatabaseIndex.showSplit( $scope, SdbRest ) ;
      }

      //打开 创建索引 的窗口
      $scope.showCreateIndex = function(){
         _DataDatabaseIndex.showCreateIndex( $scope, SdbRest ) ;
      }

      //打开 删除索引 的窗口
      $scope.showRemoveIndex = function(){
         _DataDatabaseIndex.showRemoveIndex( $scope, SdbRest ) ;
      }

      //打开 索引详细 的窗口
      $scope.showIndex = function( csIndex, clIndex ){
         _DataDatabaseIndex.showIndex( $scope, SdbRest, csIndex, clIndex ) ;
      }

      //打开 切分范围 的窗口
      $scope.showPartitions = function(){
         _DataDatabaseIndex.showPartitions( $scope ) ;
      }

      //显示不同分区组的信息
      $scope.showGroupInfo = function( index ){
         _DataDatabaseIndex.showGroupInfo( $scope, index ) ;
      }

      //是否显示子集合
      $scope.switchShowSubCl = function( event ){
         _DataDatabaseIndex.isShowSubCl( $scope, SdbFunction, event ) ;
      }

      //获取cs信息
      _DataDatabaseIndex.getCSInfo( $scope, SdbRest ) ;

   } ) ;
}());