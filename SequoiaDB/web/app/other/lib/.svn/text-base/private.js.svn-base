
// --------------------- Index ---------------------
var _IndexPublic = {} ;

//创建错误弹窗
_IndexPublic.createErrorModel = function( $scope, context ){
   $scope.Components.Confirm.type = 3 ;
   $scope.Components.Confirm.noOK = true ;
   $scope.Components.Confirm.noClose = true ;
   $scope.Components.Confirm.context = context ;
   $scope.Components.Confirm.isShow = true ;
}

//语言控制器
_IndexPublic.languageCtrl = function( $scope, text ){
   var newText = text ;
   if( $scope.Language == 'en' )
   {
      function setLanguage()
      {
         if( typeof( window.SdbSacLanguage[text] ) == 'undefined' )
         {
            printfDebug( '"' + text + '" 没翻译！' ) ;
         }
         else
         {
            newText = window.SdbSacLanguage[text] ;
         }
      }
      if( typeof( window.SdbSacLanguage ) == 'undefined' )
      {
         //获取语言
         $.ajax( './app/language/English.json', { 'async': false, 'success': function( reqData ){
            window.SdbSacLanguage = JSON.parse( reqData ) ;
            setLanguage() ;
         }, 'error': function( XMLHttpRequest, textStatus, errorThrown ){
            window.SdbSacLanguage = {} ;
            _IndexPublic.createErrorModel( $scope, 'Can not find the language file, please try to refresh your browser by pressing F5.' ) ;
         } } ) ;
      }
      else
      {
         setLanguage() ;
      }
   }
   return newText ;
}

// --------------------- Index.Left ---------------------
var _IndexLeft = {} ;

//更新导航信息
_IndexLeft.updateNav = function( $scope, $rootScope, SdbRest, callBack )
{
   //获取业务实例列表
   var data = { 'cmd': 'query business', 'sort': JSON.stringify( { 'BusinessName': 1, 'ClusterName': 1 } ) } ;
   SdbRest.OmOperation( data, function( instanceList ){
      $rootScope.initNav() ;
      var navMenuLength = $scope.Left.navMenu.length ;
      $.each( instanceList, function( index, moduleInfo ){
         var thisModule = { 'name': moduleInfo['BusinessName'],
                            'type': moduleInfo['BusinessType'],
                            'mode': moduleInfo['DeployMod'],
                            'cluster': moduleInfo['ClusterName'] } ;
         for( var i = 0; i < navMenuLength; ++i )
         {
            if( isArray( $scope.Left.navMenu[i]['list'] ) == true )
            {
               var moduleLength = $scope.Left.navMenu[i]['list'].length ;
               for( var k = 0; k < moduleLength; ++k )
               {
                  if( $scope.Left.navMenu[i]['list'][k]['title'].toLocaleLowerCase() == moduleInfo['BusinessType'].toLocaleLowerCase() )
                  {
                     $scope.Left.navMenu[i]['list'][k]['list'].push( thisModule ) ;
                  }
               }
            }
         }
      } ) ;
      $scope.$apply() ;
      if( typeof( callBack ) == 'function' )
      {
         callBack( instanceList, $scope.Left.navMenu ) ;
      }
   }, function( errorInfo ){
      $scope.Components.Confirm.isShow = true ;
      $scope.Components.Confirm.type = 1 ;
      $scope.Components.Confirm.title = $scope.autoLanguage( '获取数据失败' ) ;
      $scope.Components.Confirm.okText = $scope.autoLanguage( '重试' ) ;
      $scope.Components.Confirm.closeText = $scope.autoLanguage( '取消' ) ;
      $scope.Components.Confirm.context = sprintf( $scope.autoLanguage( '错误码: ?, ?。需要重试吗?' ), errorInfo['errno'], errorInfo['description'] ) ;
      $scope.Components.Confirm.ok = function(){
         $scope.Components.Confirm.isShow = false ;
      }
   }, function(){
      _IndexPublic.createErrorModel( $scope, $scope.autoLanguage( '网络连接错误，请尝试按F5刷新浏览器。' ) ) ;
   } ) ;
}

//激活导航要激活的业务的索引
_IndexLeft.getActiveIndex = function( $rootScope, SdbFunction, navMenu )
{
   var defaultIndex   = [ -1, -1, -1 ] ;
   var cursorIndex    = [ -1, -1, -1 ] ;
   var cursorModule   = $rootScope.Url.Module ;
   var cursorCluster  = SdbFunction.LocalData( 'SdbClusterName' ) ;
   var cursorInstance = SdbFunction.LocalData( 'SdbModuleName' ) ;
   switch( cursorModule )
   {
   case 'Data':
      cursorIndex[0] = 0 ;
      break ;
   case 'Monitor':
      cursorIndex[0] = 1 ;
      break ;
   case 'Strategy':
      cursorIndex[0] = 2 ;
      break ;
   case 'Deploy':
      cursorIndex[0] = 4 ;
      break ;
   default:
      cursorIndex[0] = 0 ;
      break ;
   }
   defaultIndex[0] = cursorIndex[0] ;
   if( typeof( navMenu[ cursorIndex[0] ]['list'] ) != 'undefined' )
   {
      $.each( navMenu[ cursorIndex[0] ]['list'], function( index1, moduleNav ){
         $.each( moduleNav['list'], function( index2, instanceNav ){
            if( defaultIndex[1] == -1 )
            {
               defaultIndex[1] = index1 ;
            }
            if( defaultIndex[2] == -1 )
            {
               defaultIndex[2] = index2 ;
            }
            if( instanceNav['cluster'] == cursorCluster &&
                instanceNav['name'] == cursorInstance )
            {
               cursorIndex[1] = index1 ;
               cursorIndex[2] = index2 ;
               return false ;
            }
         } ) ;
         if( cursorIndex[0] >= 0 && cursorIndex[1] >= 0 && cursorIndex[2] >= 0 )
         {
            return false
         }
      } ) ;
   }
   if( cursorIndex[0] >= 0 && cursorIndex[1] >= 0 && cursorIndex[2] >= 0 )
   {
      return cursorIndex ;
   }
   else
   {
      return defaultIndex ;
   }
}

// --------------------- Index.Bottom ---------------------
var _IndexBottom = {} ;

//获取系统时间
_IndexBottom.getSystemTime = function( $scope )
{
   var times = $.now() ;
   setInterval( function(){
      var date = new Date( times ) ;
      var year = date.getFullYear() ;
      var hour = date.getHours() ;
      var minute = date.getMinutes() ;
      var second = date.getSeconds() ;
      $scope.Bottom.year = year ;
      $scope.Bottom.nowtime = pad( hour, 2 ) + ':' + pad( minute, 2 ) + ':' + pad( second, 2 ) ;
      $scope.$apply() ;
      times += 1000 ;
   }, 1000 ) ;
}

//获取ping值
_IndexBottom.checkPing = function( $scope, SdbRest )
{
   SdbRest.getPing( function( times ){
      if( times >= 0 )
      {
         $scope.Bottom.sysStatus = $scope.autoLanguage( '良好' ) ;
         $scope.Bottom.statusColor = 'success' ;
         setTimeout( function(){
            _IndexBottom.checkPing( $scope, SdbRest ) ;
         }, 5000 ) ;
      }
      else
      {
         $scope.Bottom.sysStatus = $scope.autoLanguage( '网络错误' ) ;
         $scope.Bottom.statusColor = 'error' ;
         _IndexPublic.createErrorModel( $scope, $scope.autoLanguage( '网络连接错误，请尝试按F5刷新浏览器。' ) ) ;
      }
      $scope.$apply() ;
   } ) ;
}

// --------------------- Index.Top ---------------------
var _IndexTop = {} ;

//创建修改密码弹窗
_IndexTop.createPasswdModel = function( $scope, SdbRest ){
   $scope.Components.Modal.icon = 'fa-lock' ;
   $scope.Components.Modal.title = $scope.autoLanguage( '修改密码' ) ;
   $scope.Components.Modal.isShow = true ;
   $scope.Components.Modal.form = {
      inputList: [
         {
            "name": "password",
            "webName": $scope.autoLanguage( "密码" ),
            "type": "password",
            "required": true,
            "value": "",
            "valid": {
               "min": 1,
               "max": 1024
            }
         },
         {
            "name": "newPassword",
            "webName": $scope.autoLanguage( "新密码" ),
            "type": "password",
            "required": true,
            "value": "",
            "valid": {
               "min": 1,
               "max": 1024
            }
         }
      ]
   } ;
   $scope.Components.Modal.Context = '<div form-create para="data.form"></div>' ;
   $scope.Components.Modal.ok = function(){
      var isAllClear = $scope.Components.Modal.form.check() ;
      if( isAllClear )
      {
         var value = $scope.Components.Modal.form.getValue() ;
         SdbRest.ChangePasswd( 'admin', value['password'], value['newPassword'], function( json ){
            $scope.Components.Confirm.isShow = true ;
            $scope.Components.Confirm.type = 4 ;
            $scope.Components.Confirm.closeText = $scope.autoLanguage( '取消' ) ;
            $scope.Components.Confirm.context = $scope.autoLanguage( '修改密码成功。' ) ;
            $scope.Components.Confirm.noOK = true ;
         }, function( errorInfo ){
            $scope.Components.Confirm.isShow = true ;
            $scope.Components.Confirm.type = 1 ;
            $scope.Components.Confirm.title = $scope.autoLanguage( '修改密码失败。' ) ;
            $scope.Components.Confirm.okText = $scope.autoLanguage( '重试' ) ;
            $scope.Components.Confirm.closeText = $scope.autoLanguage( '取消' ) ;
            $scope.Components.Confirm.context = sprintf( $scope.autoLanguage( '错误码: ?, ?。' ), errorInfo['errno'], errorInfo['detail'] ) ;
            $scope.Components.Confirm.ok = function(){
               $scope.Components.Confirm.isShow = false ;
               _IndexTop.createPasswdModel( $scope, SdbRest ) ;
            }
         }, function(){
            _IndexPublic.createErrorModel( $scope, $scope.autoLanguage( '网络连接错误，请尝试按F5刷新浏览器。' ) ) ;
         }, function(){
            //关闭弹窗
            $scope.Components.Modal.isShow = false ;
         } ) ;
      }
      return isAllClear ;
   }
}

//登出
_IndexTop.logout = function( $location, SdbFunction ){
   //删除会话
   SdbFunction.LocalData( 'SdbSessionID', null ) ;
   //删除用户名
   SdbFunction.LocalData( 'SdbUser', null ) ;
   //删除选中的集群
   SdbFunction.LocalData( 'SdbClusterName', null ) ;
   //删除选中的业务
   SdbFunction.LocalData( 'SdbModuleType', null ) ;
   SdbFunction.LocalData( 'SdbModuleMode', null ) ;
   SdbFunction.LocalData( 'SdbModuleName', null ) ;
   //删除cs
   SdbFunction.LocalData( 'SdbCsName', null ) ;
   //删除cl
   SdbFunction.LocalData( 'SdbClType', null ) ;
   SdbFunction.LocalData( 'SdbClName', null ) ;
   window.location.href = '/login.html' ;
}