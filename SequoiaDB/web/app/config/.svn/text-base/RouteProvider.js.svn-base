(function(){
   var sacApp = window.SdbSacManagerModule ;
   sacApp.config( function( $sceProvider, $routeProvider, $controllerProvider, $compileProvider ){
      //控制器工厂
      window.SdbSacManagerModule.controllerProvider = $controllerProvider ;
      //指令工厂
      window.SdbSacManagerModule.compileProvider = $compileProvider ;
      //兼容IE7
      $sceProvider.enabled( false ) ;
      /* --- 路由列表 -- */
      var aRoute = window.SdbSacManagerConf.nowRoute ;
      var len = aRoute.length ;
      for( var i = 0; i < len; ++i )
      {
         $routeProvider.when( aRoute[i]['path'], aRoute[i]['options'] ) ;
      }
      //默认访问
      $routeProvider.otherwise( window.SdbSacManagerConf.defaultRoute ) ;
   } ) ;
   //自定义排序
   sacApp.filter( 'orderObjectBy', function(){
      return function( items, field, reverse ){
         var filtered = [] ;
         angular.forEach( items, function( item ){
            filtered.push( item ) ;
         } ) ;
         filtered.sort( function( a, b ){
            var fields = field.split( '.' ) ;
            angular.forEach( fields, function( key ){
               a = a[key] ;
               b = b[key] ;
            } ) ;
            if( typeof( a ) == 'undefined' || a == null )
            {
               a = '' ;
            }
            if( typeof( b ) == 'undefined' || b == null )
            {
               b = '' ;
            }
            if( typeof( a ) != typeof( b ) )
            {
               a = String( a ) ;
               b = String( b ) ;
            }
            return ( a > b ? 1 : -1 ) ;
         } ) ;
         if( reverse )
         {
            filtered.reverse() ;
         }
         return filtered ;
      } ;
   } ) ;
}());