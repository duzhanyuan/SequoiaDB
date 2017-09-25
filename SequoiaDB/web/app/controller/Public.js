(function(){
   var sacApp = window.SdbSacManagerModule ;
   //全局模板
   sacApp.controller( 'Index.Ctrl', function( $scope, $window, $rootScope, $location, Tip, SdbFunction, Loading, SdbRest ){

      //校验登录信息
      if( SdbFunction.LocalData( 'SdbUser' ) === null || SdbFunction.LocalData( 'SdbSessionID' ) === null )
		{
			window.location.href = '/login.html' ;
         return;
      }
      //设置默认语言
      if( SdbFunction.LocalData( 'SdbLanguage' ) == null )
      {
         SdbFunction.LocalData( 'SdbLanguage', 'zh-CN' ) ;
      }
      //预加载模板
      $scope.Templates = {} ;
      $scope.Templates.Top = './app/template/Public/Top.html' ;
      $scope.Templates.Left = './app/template/Public/Left.html' ;
      $scope.Templates.Bottom = './app/template/Public/Bottom.html' ;
      //获取语言
      $scope.Language = SdbFunction.LocalData( 'SdbLanguage' ) ;
      //初始化提示标签
      Tip.create() ;
      Tip.auto() ;
      //-------- 全局变量 ---------
      $rootScope.Url = { Module: '', Action: '', Method: '' } ;
      $rootScope.onResize = 0 ;
      //-------- 全局组件 ---------
      $rootScope.Components = {} ;
      $rootScope.Components.Confirm = {} ;
      $rootScope.Components.Modal = {} ;
      //-------- 全局函数 ---------
      //格式化
      $rootScope.sprintf = sprintf ;
      //判断数组
      $rootScope.isArray = isArray ;
      //数值补位
      $rootScope.pad = pad ;
      //语言控制
      $rootScope.autoLanguage = function( text ){
         return _IndexPublic.languageCtrl( $scope, text ) ;
      }
      //初始化导航（实际实现在下面Left）
      $rootScope.initNav = function(){} ;
      //更新导航(实际实现在下面Left)
      $rootScope.updateNav = function(){} ;
      //更新Url变量
      $rootScope.updateUrl = function(){
         var route = $location.url().split( '/' ) ;
         $rootScope.Url.Module = route[1] ;
         $rootScope.Url.Action = route[2] ;
         $rootScope.Url.Method = route[3] ;
      } ;
      //触发自定义的onResize
      $rootScope.bindResize = function(){
         var random = 0 ;
         while( random == $rootScope.onResize ) random = Math.random() ;
         $rootScope.onResize = random ;
      } ;
      //排版参数
      $rootScope.layout = {} ;
      $rootScope.layout.top     = { height: 40 } ;
      $rootScope.layout.content = { offsetY: -40 } ;
      $rootScope.layout.bottom  = { height: 40 } ;
      $rootScope.layout.left    = { width: 260 } ;
      $rootScope.layout.centre  = { offsetX: -260, marginLeft: 260 } ;
   } ) ;

   //顶部
   sacApp.controller( 'Index.Top.Ctrl', function( $scope, $location, SdbFunction, SdbRest ){
      $scope.Top = {} ;
      //给用户菜单创建一个遮罩
      var mask = $( '<div class="mask-screen unalpha"></div>' ).on( 'click', function(){
         $( '#userMenu' ).hide() ;
         mask.detach() ;
      } ) ;
      $( '#userMenu li' ).on( 'click', function(){
         $( '#userMenu' ).hide() ;
         mask.detach() ;
      } ) ;
      $scope.Top.showUserMenu = function(){
         $( '#userMenu' ).show() ;
         mask.appendTo( document.body ) ;
      }
      //修改密码弹窗
      $scope.showChangePasswd = function(){
         _IndexTop.createPasswdModel( $scope, SdbRest ) ;
      }
      //登出
      $scope.logout = function(){
         _IndexTop.logout( $location, SdbFunction ) ;
      }
   } ) ;
   //左边
   sacApp.controller( 'Index.Left.Ctrl', function( $scope, $rootScope, $location, SdbRest, SdbFunction ){
      $scope.Left = {} ;
      $scope.Left.nav1 = { width: 80 } ;
      $scope.Left.nav2 = { width: 180, marginLeft: 80 } ;
      $scope.Left.nav2Show = true ;
      $scope.Left.nav1Btn = { 'visibility': 'hidden' } ;
      $scope.Left.nav2Btn = { 'visibility': 'hidden' } ;
      //初始化导航列表
      $rootScope.initNav = function(){
         $scope.Left.navMenu = [
            {
               'text': $scope.autoLanguage( '数据' ),
               'module': 'Data',
               'icon': 'fa-database',
               'list': [
                  {
                     'title': 'SequoiaDB',
                     'list': []
                  },
                  {
                     'title': 'SequoiaSQL',
                     'list': []
                  },
                  {
                     'title': 'Hdfs',
                     'list': []
                  },
                  {
                     'title': 'Spark',
                     'list': []
                  },
                  {
                     'title': 'Yarn',
                     'list': []
                  }
               ]
            },
            {
               'text': $scope.autoLanguage( '监控' ),
               'module': 'Monitor',
               'icon': 'fa-flash',
               'list': [
                  {
                     'title': 'SequoiaDB',
                     'list': []
                  }
               ]
            },
            /*
            {
               'text': $scope.autoLanguage( '策略' ),
               'module': 'Strategy',
               'icon': 'fa-tasks',
               'list': [
                  {
                     'title': 'SequoiaDB',
                     'list': []
                  }
               ]
            },
            */
            {
               'text': $scope.autoLanguage( '部署' ),
               'module': 'Deploy2',
               'icon': 'fa-share-alt',
               'action': '/deployment/index.html'
            }
            /*,
            {
               'text': $scope.autoLanguage( '新版部署' ),
               'module': 'Deploy',
               'icon': 'fa-share-alt',
               'action': '/#/Deploy/Index/Index'
            }*/
         ] ;
      } ;
      $rootScope.updateNav = function( callBack ){
         _IndexLeft.updateNav( $scope, $rootScope, SdbRest, callBack ) ;
      } ;

      //更新url地址信息
      $rootScope.updateUrl() ;

      if( $rootScope.Url.Module == 'Deploy' )
      {
         $rootScope.layout.left    = { width: 80 } ;
         $rootScope.layout.centre  = { offsetX: -80, marginLeft: 80 } ;
         $scope.Left.nav2 = { width: 0, marginLeft: 0 } ;
         $scope.Left.nav2Show = false ;
      }

      //更新导航
      $rootScope.updateNav( function( instanceList, navMenu ){
         $scope.cursorIndex = _IndexLeft.getActiveIndex( $rootScope, SdbFunction, navMenu ) ;
         $scope.showModuleIndex = $scope.cursorIndex[0] ;
         if( $scope.Left.navMenu[ $scope.showModuleIndex ]['module'] == 'Deploy' )
         {
            $scope.Left.nav1Btn = { 'visibility': 'hidden' } ;
            $scope.Left.nav2Btn = { 'visibility': 'hidden' } ;
         }
         else
         {
            $scope.Left.nav1Btn = { 'visibility': 'hidden' } ;
            $scope.Left.nav2Btn = { 'visibility': 'visible' } ;
         }
      } ) ;

      $scope.toggleNav2 = function(){
         if( $scope.Left.nav2Show == true )
         {
            $scope.Left.nav1Btn = { 'visibility': 'visible' } ;
            $scope.Left.nav2Btn = { 'visibility': 'hidden' } ;
            $rootScope.layout.left    = { width: 80 } ;
            $rootScope.layout.centre  = { offsetX: -80, marginLeft: 80 } ;
            $scope.Left.nav2 = { width: 0, marginLeft: 0 } ;
         }
         else
         {
            $scope.Left.nav1Btn = { 'visibility': 'hidden' } ;
            $scope.Left.nav2Btn = { 'visibility': 'visible' } ;
            $rootScope.layout.left    = { width: 260 } ;
            $rootScope.layout.centre  = { offsetX: -260, marginLeft: 260 } ;
            $scope.Left.nav1 = { width: 80 } ;
            $scope.Left.nav2 = { width: 180, marginLeft: 80 } ;
         }
         $scope.Left.nav2Show = !$scope.Left.nav2Show ;
         $rootScope.bindResize() ;
      }

      $rootScope.gotoModule = function( moduleIndex, activeIndex, instanceIndex ){
         var clusterName = $scope.Left.navMenu[ moduleIndex ]['list'][ activeIndex ]['list'][ instanceIndex ]['cluster'] ;
         var moduleName  = $scope.Left.navMenu[ moduleIndex ]['list'][ activeIndex ]['list'][ instanceIndex ]['name'] ;
         var moduleType  = $scope.Left.navMenu[ moduleIndex ]['list'][ activeIndex ]['list'][ instanceIndex ]['type'] ;
         var moduleMode  = $scope.Left.navMenu[ moduleIndex ]['list'][ activeIndex ]['list'][ instanceIndex ]['mode'] ;
         SdbFunction.LocalData( 'SdbClusterName', clusterName ) ;
         SdbFunction.LocalData( 'SdbModuleType', moduleType ) ;
         SdbFunction.LocalData( 'SdbModuleMode', moduleMode ) ;
         SdbFunction.LocalData( 'SdbModuleName', moduleName ) ;
         var params = { 'r': new Date().getTime() } ;
         if( $scope.Left.navMenu[ moduleIndex ]['module'] == 'Data' )
         {
            switch( moduleType )
            {
            case 'sequoiadb':
               $location.path( '/Data/SDB-Database/Index' ).search( params ) ; break ;
            case 'sequoiasql':
               $location.path( '/Data/SQL-Metadata/Index' ).search( params ) ; break ;
            case 'hdfs':
               $location.path( '/Data/HDFS-web/Index' ).search( params ) ; break ;
            case 'spark':
               $location.path( '/Data/SPARK-web/Index' ).search( params ) ; break ;
            case 'yarn':
               $location.path( '/Data/YARN-web/Index' ).search( params ) ; break ;
            default:
               break ;
            }
         }
         else if( $scope.Left.navMenu[ moduleIndex ]['module'] == 'Monitor' )
         {
            switch( moduleType )
            {
            case 'sequoiadb':
               $location.path( '/Monitor/HostPerformance/Index' ).search( params ) ; break ;
            default:
               break ;
            }
         }
         else if( $scope.Left.navMenu[ moduleIndex ]['module'] == 'Strategy' )
         {
            switch( moduleType )
            {
            case 'sequoiadb':
               $location.path( '/Strategy/SDB/Index' ).search( params ) ; break ;
            default:
               break ;
            }
         }
         $scope.cursorIndex[0] = moduleIndex ;
         $scope.cursorIndex[1] = activeIndex ;
         $scope.cursorIndex[2] = instanceIndex ;
      }

      $rootScope.$on( '$locationChangeStart', function( event, newUrl, oldUrl ){
         printfDebug( '切换路由 ' + newUrl + ' form ' + oldUrl ) ;
         $rootScope.updateUrl() ;
         $scope.cursorIndex = _IndexLeft.getActiveIndex( $rootScope, SdbFunction, $scope.Left.navMenu ) ;
         $scope.showModuleIndex = $scope.cursorIndex[0] ;
         if( $scope.Left.navMenu[ $scope.showModuleIndex ]['module'] == 'Deploy' )
         {
            $scope.Left.nav1Btn = { 'visibility': 'hidden' } ;
            $scope.Left.nav2Btn = { 'visibility': 'hidden' } ;
            $rootScope.layout.left    = { width: 80 } ;
            $rootScope.layout.centre  = { offsetX: -80, marginLeft: 80 } ;
            $scope.Left.nav2 = { width: 0, marginLeft: 0 } ;
            $scope.cursorIndex[1] = -1 ;
            $scope.cursorIndex[2] = -1 ;
         }
         else
         {
            $scope.Left.nav1Btn = { 'visibility': 'hidden' } ;
            $scope.Left.nav2Btn = { 'visibility': 'visible' } ;
            $rootScope.layout.left    = { width: 260 } ;
            $rootScope.layout.centre  = { offsetX: -260, marginLeft: 260 } ;
            $scope.Left.nav1 = { width: 80 } ;
            $scope.Left.nav2 = { width: 180, marginLeft: 80 } ;
            $scope.Left.nav2Show = true ;
         }
      } ) ;

      $scope.selectLeftModule = function( moduleIndex ){
         $scope.showModuleIndex = moduleIndex ;
         if( $scope.Left.navMenu[ $scope.showModuleIndex ]['module'] == 'Deploy' )
         {
            $scope.Left.nav1Btn = { 'visibility': 'hidden' } ;
            $scope.Left.nav2Btn = { 'visibility': 'hidden' } ;
            $rootScope.layout.left    = { width: 80 } ;
            $rootScope.layout.centre  = { offsetX: -80, marginLeft: 80 } ;
            $scope.Left.nav2 = { width: 0, marginLeft: 0 } ;
            $scope.cursorIndex[1] = -1 ;
            $scope.cursorIndex[2] = -1 ;
         }
         else
         {
            $scope.Left.nav1Btn = { 'visibility': 'hidden' } ;
            $scope.Left.nav2Btn = { 'visibility': 'visible' } ;
            $rootScope.layout.left    = { width: 260 } ;
            $rootScope.layout.centre  = { offsetX: -260, marginLeft: 260 } ;
            $scope.Left.nav1 = { width: 80 } ;
            $scope.Left.nav2 = { width: 180, marginLeft: 80 } ;
            $scope.Left.nav2Show = true ;
         }
      }
   } ) ;
   //底部
   sacApp.controller( 'Index.Bottom.Ctrl', function( $scope, SdbRest ){
      $scope.Bottom = {} ;
      //获取系统时间
      _IndexBottom.getSystemTime( $scope ) ;
      //获取系统状态
      _IndexBottom.checkPing( $scope, SdbRest ) ;
   } ) ;
}());