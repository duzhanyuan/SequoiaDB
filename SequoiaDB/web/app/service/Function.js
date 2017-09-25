(function () {
   var sacApp = window.SdbSacManagerModule;
   sacApp.service( 'SdbFunction', function() {
      var g = this;
      g.repeatList = [] ;
      //设置循环结束的回调
      g.setEndOfRepeat = function( name, num, nestFn, endFn ){
         var hasFind = false ;
         $.each( g.repeatList, function( index, value ){
            if( value['key'] == name )
            {
               g.repeatList[index]['curNum'] = 0 ;
               g.repeatList[index]['num'] = num ;
               g.repeatList[index]['nestFn'] = nestFn ;
               g.repeatList[index]['endFn'] = endFn ;
               hasFind = true ;
               return false ;
            }
         } ) ;
         if( hasFind == false )
         {
            g.repeatList.push( { key: name, num: num, curNum: 0, nestFn: nestFn, endFn: endFn } ) ;
         }
      }
      //检查循环是否结束，回调调用
      g.checkEndOfRepeat = function( name, element ){
         $.each( g.repeatList, function( index, value ){
            if( value['key'] == name )
            {
               ++g.repeatList[index]['curNum'] ;
               var nestFn = g.repeatList[index]['nestFn'] ;
               if( typeof( nestFn ) == 'function' )
               {
                  nestFn( g.repeatList[index]['curNum'] - 1, element ) ;
               }
               if( g.repeatList[index]['curNum'] == g.repeatList[index]['num'] )
               {
                  var endFn = g.repeatList[index]['endFn'] ;
                  g.repeatList[index]['curNum'] = 0 ;
                  if( typeof( endFn ) == 'function' )
                  {
                     endFn() ;
                  }
               }
               return false ;
            }
         } ) ;
      }

      //获取json的键列表
      g.getJsonKeys = function( json, maxKey, keyList, keyWord )
      {
         if( typeof( maxKey ) == 'undefined' ) maxKey = 0 ;
         if( typeof( keyList ) == 'undefined' ) keyList = [] ;
         if( typeof( keyWord ) == 'undefined' ) keyWord = '' ;
         if( keyWord.length > 0 ) keyWord += '.' ;
         if( keyList.length >= maxKey && maxKey > 0 ) return keyList ;
         $.each( json, function( key, value ){
            if( keyList.length >= maxKey && maxKey > 0 ) return false ;
            var newKey = keyWord + key ;
            var valueType = typeof (value);
            if( valueType == 'object' )
            {
               if( value == null )
               {
                  if( $.inArray( newKey, keyList ) == -1 )
                  {
                     keyList.push( newKey ) ;
                  }
               }
               else if( isArray( value ) )
               {
                  keyList = g.getJsonKeys( value, maxKey, keyList, newKey ) ;
               }
               else if( typeof( value['$binary'] ) == 'string' && typeof( value['$type'] ) == 'string' )
               {
                  if( $.inArray( newKey, keyList ) == -1 )
                  {
                     keyList.push( newKey ) ;
                  }
               }
               else if( typeof( value['$timestamp'] ) == 'string' )
               {
                  if( $.inArray( newKey, keyList ) == -1 )
                  {
                     keyList.push( newKey ) ;
                  }
               }
               else if( typeof( value['$date'] ) == 'string' )
               {
                  if( $.inArray( newKey, keyList ) == -1 )
                  {
                     keyList.push( newKey ) ;
                  }
               }
               else if( typeof( value['$code'] ) == 'string' )
               {
                  if( $.inArray( newKey, keyList ) == -1 )
                  {
                     keyList.push( newKey ) ;
                  }
               }
               else if( typeof( value['$minKey'] ) == 'number' )
               {
                  if( $.inArray( newKey, keyList ) == -1 )
                  {
                     keyList.push( newKey ) ;
                  }
               }
               else if( typeof( value['$maxKey'] ) == 'number')
               {
                  if( $.inArray( newKey, keyList ) == -1 )
                  {
                     keyList.push( newKey ) ;
                  }
               }
               else if( typeof( value['$undefined'] ) == 'number' )
               {
                  if( $.inArray( newKey, keyList ) == -1 )
                  {
                     keyList.push( newKey ) ;
                  }
               }
               else if( typeof( value['$oid'] ) == 'string' )
               {
                  if( $.inArray( newKey, keyList ) == -1 )
                  {
                     keyList.push( newKey ) ;
                  }
               }
               else if( typeof( value['$regex'] ) == 'string' && typeof( value['$options'] ) == 'string' )
               {
                  if( $.inArray( newKey, keyList ) == -1 )
                  {
                     keyList.push( newKey ) ;
                  }
               }
               else
               {
                  keyList = g.getJsonKeys( value, maxKey, keyList, newKey ) ;
               }
            }
            else
            {
               if( $.inArray( newKey, keyList ) == -1 )
               {
                  keyList.push( newKey ) ;
               }
            }
         } ) ;
         return keyList ;
      }

      //获取json的值
      g.getJsonValues = function( json, keyList, valueList ){
         function getFieldValue( json2, key )
         {
            var pointIndex = key.indexOf( '.' ) ;
            if( pointIndex > 0 )
            {
               var fields = key.split( '.', 2 ) ;
               if( typeof( json2[ fields[0] ] ) == 'undefined' )
               {
                  return '' ;
               }
               else
               {
                  return getFieldValue( json2[ fields[0] ], key.substr( pointIndex + 1 ) ) ;
               }
            }
            else
            {
               var value = json2[ key ] ;
               var valueType = typeof( value ) ;
               if( valueType == 'object' )
               {
                  if( value == null )
                  {
                     return 'null' ;
                  }
                  else if( typeof( value['$binary'] ) == 'string' && typeof( value['$type'] ) == 'string' )
                  {
                     return value['$binary'] ;
                  }
                  else if( typeof( value['$timestamp'] ) == 'string' )
                  {
                      return value['$timestamp'] ;
                  }
                  else if( typeof( value['$date'] ) == 'string' )
                  {
                      return value['$date'] ;
                  }
                  else if( typeof( value['$code'] ) == 'string' )
                  {
                     return value['$code'] ;
                  }
                  else if( typeof( value['$minKey'] ) == 'number' )
                  {
                     return 'minKey' ;
                  }
                  else if( typeof( value['$maxKey'] ) == 'number')
                  {
                     return 'maxKey' ;
                  }
                  else if( typeof( value['$undefined'] ) == 'number' )
                  {
                     return 'undefined' ;
                  }
                  else if( typeof( value['$oid'] ) == 'string' )
                  {
                     return value['$oid'] ;
                  }
                  else if( typeof( value['$regex'] ) == 'string' && typeof( value['$options'] ) == 'string' )
                  {
                     return value['$regex'] ;
                  }
                  else if( isArray( value ) )
                  {
                     return '[ Array ]' ;
                  }
                  else
                  {
                     return '[ Object ]' ;
                  }
               }
               else if( valueType == 'boolean' )
               {
                  var newVal = value ? 'true' : 'false' ;
                  return newVal ;
               }
               else
               {
                  return value ;
               }
            }
         }
         if( typeof( valueList ) == 'undefined' ) valueList = [] ;
         $.each( keyList, function( index, key ){
            if( key == '' )
            {
               valueList.push( '' ) ;
               return true ;
            }
            var value = getFieldValue( json, key ) ;
            valueList.push( value ) ;
         } ) ;
         return valueList ;
      }

      //获取浏览器类型和版本
      g.getBrowserInfo = function()
      {
	      var agent = window.navigator.userAgent.toLowerCase() ;
	      var regStr_ie = /msie [\d.]+;/gi ;
	      var regStr_ff = /firefox\/[\d.]+/gi ;
	      var regStr_chrome = /chrome\/[\d.]+/gi ;
	      var regStr_saf = /safari\/[\d.]+/gi ;
	      var temp = '' ;
	      var info = [] ;
	      if( agent.indexOf( 'msie' ) > 0 )
	      {
		      temp = agent.match( regStr_ie ) ;
		      info.push( 'ie' ) ;
	      }
	      else if( agent.indexOf( 'firefox' ) > 0 )
	      {
		      temp = agent.match( regStr_ff ) ;
		      info.push( 'firefox' ) ;
	      }
	      else if( agent.indexOf( 'chrome' ) > 0 )
	      {
		      temp = agent.match( regStr_chrome ) ;
		      info.push( 'chrome' ) ;
	      }
	      else if( agent.indexOf( 'safari' ) > 0 && agent.indexOf( 'chrome' ) < 0 )
	      {
		      temp = agent.match( regStr_saf ) ;
		      info.push( 'safari' ) ;
	      }
	      else
	      {
		      if( agent.indexOf( 'trident' ) > 0 && agent.indexOf( 'rv' ) > 0 )
		      {
			      info.push( 'ie' ) ;
			      temp = '11' ;
		      }
		      else
		      {
			      temp = '0' ;
			      info.push( 'unknow' ) ;
		      }
	      }
	      verinfo = ( temp + '' ).replace( /[^0-9.]/ig, '' ) ;
	      info.push( parseInt( verinfo ) ) ;
	      return info ;
      }

      //判断浏览器可以使用什么存储方式
      g._userdata = {} ;
      g.setBrowserStorage = function()
      {
	      var browser = g.getBrowserInfo() ;
         var storageType ;
	      if( browser[0] === 'ie' && browser[1] <= 7 )
	      {
            storageType = 'cookie' ;
	      }
	      else
	      {
		      if( window.localStorage )
		      {
			      storageType = 'localStorage' ;
		      }
		      else
		      {
			      if( navigator.cookieEnabled === true )
			      {
				      storageType = 'cookie' ;
			      }
			      else
			      {
				      storageType = '' ;
			      }
		      }
	      }
	      return storageType ;
      }

      g.storageType = g.setBrowserStorage() ;
      //本地数据操作
      g.LocalData = function( key, value ){
         if( typeof( value ) == 'undefined' )
         {
            //读取本地数据
            var newValue = null ;
	         if ( g.storageType === 'localStorage' )
	         {
		         newValue = window.localStorage.getItem( key ) ;
	         }
	         else if ( g.storageType === 'cookie' )
	         {
		         newValue = $.cookie( key ) ;
	         }
	         return newValue ;
         }
         else if( value == null )
         {
            //删除本地数据
            if ( g.storageType === 'localStorage' )
	         {
		         window.localStorage.removeItem( key ) ;
	         }
	         else if ( g.storageType === 'cookie' )
	         {
		         $.removeCookie( key ) ;
	         }
         }
         else
         {
            //写入本地数据
            if ( g.storageType === 'localStorage' )
	         {
		         window.localStorage.setItem( key, value ) ;
	         }
	         else if ( g.storageType === 'cookie' )
	         {
		         var saveTime = new Date() ;
		         saveTime.setDate( saveTime.getDate() + 365 ) ;
		         $.cookie( key, value, { 'expires': saveTime } ) ;
	         }
         }
      }
      //中转站跳转控制
      g.TransferCtr = function( $location, SdbRest ){
         var data = { 'cmd': 'query business', 'sort': JSON.stringify( { 'BusinessName': 1, 'ClusterName': 1 } ) } ;
         SdbRest.OmOperation( data, function( moduleList ){
            if( moduleList.length > 0 )
            {
               var moduleInfo = moduleList[0] ;
               g.LocalData( 'SdbClusterName', moduleInfo['ClusterName'] ) ;
               g.LocalData( 'SdbModuleName', moduleInfo['BusinessName'] ) ;
               g.LocalData( 'SdbModuleType', moduleInfo['BusinessType'] ) ;
               g.LocalData( 'SdbModuleMode', moduleInfo['DeployMod'] ) ;
               var params = { 'r': new Date().getTime() } ;
               switch( moduleInfo['BusinessType'] )
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
                  window.location.href = '/deployment/index.html' ;
                  break ;
               }
            }
            else
            {
               window.location.href = '/deployment/index.html' ;
            }
         }, function( errorInfo ){
            window.location.href = '/deployment/index.html' ;
         }, function(){
            window.location.href = '/deployment/index.html' ;
         } ) ;
      }
   } ) ;
}());