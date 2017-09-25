(function(){
   var sacApp = window.SdbSacManagerModule ;
   sacApp.service( 'SdbRest', function( $q, Loading, SdbFunction ){
      var g = this ;
      function restBeforeSend( jqXHR )
      {
	      var id = SdbFunction.LocalData( 'SdbSessionID' ) ;
	      if( id !== null )
	      {
		      jqXHR.setRequestHeader( 'SdbSessionID', id ) ;
	      }
	      var language = SdbFunction.LocalData( 'SdbLanguage' )
	      if( language !== null )
	      {
		      jqXHR.setRequestHeader( 'SdbLanguage', language ) ;
	      }
      }

      //发送请求
      g._post = function( data, before, success, failed, error, complete, showLoading ){
         if( typeof( showLoading ) == 'undefined' ) showLoading = true ;
         if( showLoading )
         {
            Loading.create() ;
         }
         $.ajax( { 'type': 'POST', 'url': '/', 'data': data, 'success': function( json, textStatus, jqXHR ){
            json = trim( json ) ;
            if( json.length == 0 && typeof( failed ) === 'function' )
            {
               //收到响应，但是没有任何数据
               failed( { "errno": -10, "description": "System error", "detail": "No rest response data." } ) ;
            }
            else
            {
               var jsonArr = g._parseJsons( json ) ;
               if( jsonArr.length == 0 )
               {
                  //有数据，但是没有记录，理论上不会发生
                  if( typeof( failed ) === 'function' ) failed( { "errno": -10, "description": "System error", "detail": "Rest response data error." } ) ;
               }
               else if( jsonArr[0]['errno'] === 0 && typeof( success ) == 'function' )
               {
                  jsonArr.splice( 0, 1 ) ;
                  success( jsonArr, textStatus, jqXHR ) ;
               }
               else if( jsonArr[0]['errno'] === -62 )
               {
                  //session id 不存在
                  window.location.href = './login.html#/Login' ;
               }
               else if( typeof( failed ) === 'function' )
               {
                  //其他错误
                  failed( jsonArr[0] ) ;
               }
            }
         }, 'error': function( XMLHttpRequest, textStatus, errorThrown ) {
            if( typeof( error ) === 'function' ) error( XMLHttpRequest, textStatus, errorThrown ) ;
         }, 'complete': function ( XMLHttpRequest, textStatus ) {
            if( typeof( complete ) == 'function' ) complete( XMLHttpRequest, textStatus ) ;
            if( showLoading )
            {
               Loading.cancel() ;
            }
         }, 'beforeSend': function( XMLHttpRequest ){
            restBeforeSend( XMLHttpRequest ) ;
            if( typeof( before ) === 'function' )
            {
               before( XMLHttpRequest ) ;
            }
         } } ) ;
      }

      //发送请求(新版，异步调用)
      g._post2 = function( data, showLoading ){
         var defferred = $q.defer() ;
         if( typeof( showLoading ) == 'undefined' ) showLoading = true ;
         if( showLoading )
         {
            Loading.create() ;
         }
         $.ajax( { 'type': 'POST', 'url': '/', 'data': data, 'success': function( json, textStatus, jqXHR ){
            json = trim( json ) ;
            if( json.length == 0 )
            {
               //收到响应，但是没有任何数据
               defferred.reject( { "errno": -10, "description": "System error", "detail": "No rest response data." } ) ;
            }
            else
            {
               var jsonArr = g._parseJsons( json ) ;
               if( jsonArr.length == 0 )
               {
                  //有数据，但是没有记录，理论上不会发生
                  defferred.reject( { "errno": -10, "description": "System error", "detail": "Rest response data error." } ) ;
               }
               else if( jsonArr[0]['errno'] === 0 )
               {
                  jsonArr.splice( 0, 1 ) ;
                  defferred.resolve( jsonArr ) ;
               }
               else if( jsonArr[0]['errno'] === -62 )
               {
                  //session id 不存在
                  window.location.href = './login.html#/Login' ;
               }
               else
               {
                  //其他错误
                  defferred.reject( jsonArr[0] ) ;
               }
            }
         }, 'error': function( XMLHttpRequest, textStatus, errorThrown ) {
            defferred.reject() ;
         }, 'complete': function ( XMLHttpRequest, textStatus ) {
            if( showLoading )
            {
               Loading.cancel() ;
            }
         }, 'beforeSend': function( XMLHttpRequest ){
            restBeforeSend( XMLHttpRequest ) ;
         } } ) ;
         return defferred.promise ;
      }

      //测试发送
      g._postTest = function( url, success, failed, error )
      {
         Loading.create() ;
         g.getFile( url, true, function( jsons ){
            var jsonList = g._parseJsons( jsons ) ;
            if( jsonList[0]['errno'] == 0 )
            {
               jsonList.splice( 0, 1 ) ;
               if( typeof( success ) == 'function' )
               {
                  success( jsonList ) ;
               }
            }
            else
            {
               if( typeof( failed ) == 'function' )
               {
                  failed( jsonList[0] ) ;
               }
            }
            Loading.cancel() ;
         } ) ;
      }

      //解析响应的Json
      g._parseJsons = function( str )
      {
	      var json_array = [] ;
	      var i = 0, len = str.length ;
	      var chars, level, isEsc, isString, start, end, subStr, json ;
	      while( i < len )
	      {
		      while( i < len ){	chars = str.charAt( i ) ;	if( chars === '{' ){	break ;	}	++i ;	}
		      level = 0, isEsc = false, isString = false, start = i ;
		      while( i < len )
		      {
			      chars = str.charAt( i ) ;
			      if( isEsc ){	isEsc = false ;	}
			      else
			      {
				      if( ( chars === '{' || chars === '[' ) && isString === false ){	++level ;	}
				      else if( ( chars === '}' || chars === ']' ) && isString === false )
				      {
					      --level ;
					      if( level === 0 )
					      {
						      ++i ;
						      end = i ;
						      subStr = str.substring( start, end ) ;
                        try{
                           json = JSON.parse( subStr ) ;
                        }catch(e){
                           subStr = subStr.replace( /[^"-]inf/, '1.7976931348623157e+308' ) ;
                           subStr = subStr.replace( /[^"]-inf/, '-1.7976931348623157e+308' ) ;
                           try{
                              json = JSON.parse( subStr ) ;
                           }catch(e){
                              break;
                           }
                        }
						      json_array.push( json ) ;
						      break ;
					      }
				      }
				      else if( chars === '"' ){	isString = !isString ;	}
				      else if( chars === '\\' ){	isEsc = true ;	}
			      }
			      ++i ;
		      }
	      }
	      return json_array ;
      }

      //获取文件
      g.getFile = function( url, async, success, error )
      {
         $.ajax( {
            'async': async,
            'url': url,
            'type': 'GET',
            'dataType': 'text',
            'success': success,
            'error': error
         } ) ;
      }

      //获取配置文件
      g.getConfig = function( fileName, success )
      {
         var language = SdbFunction.LocalData( 'SdbLanguage' ) ;
         var newUrl = './config/' + fileName + '_' + language ;
         $.get( newUrl, {}, function( reData ){
            success( reData ) ;
         }, 'json' ) ;
      }

      //获取语言文件
      g.getLanguage = function( fileName, success )
      {
         var language = SdbFunction.LocalData( 'SdbLanguage' ) ;
         var newUrl = './app/language/' + fileName + '_' + language ;
         $.get( newUrl, {}, function( reData ){
            success( reData ) ;
         }, 'json' ) ;
      }
      
      //om系统操作
      g.OmOperation = function( data, success, failed, error, complete, showLoading ){
         g._post( data, null, success, failed, error, complete, showLoading ) ;
      }

      //数据操作
      g.DataOperation = function( data, success, failed, error, complete ){
         g._post( data, function( jqXHR ){
	         var clusterName = SdbFunction.LocalData( 'SdbClusterName' ) ;
	         if( clusterName !== null )
	         {
		         jqXHR.setRequestHeader( 'SdbClusterName', clusterName ) ;
	         }
	         var businessName = SdbFunction.LocalData( 'SdbModuleName' )
	         if( businessName !== null )
	         {
		         jqXHR.setRequestHeader( 'SdbBusinessName', businessName ) ;
	         }
         }, success, failed, error, complete ) ;
      }

      //sequoiasql操作
      g.SequoiaSQL = function( data, success, failed, error, complete ){
         var clusterName = SdbFunction.LocalData( 'SdbClusterName' ) ;
	      if( clusterName != null )
	      {
		      data['ClusterName'] = clusterName ;
	      }
	      var businessName = SdbFunction.LocalData( 'SdbModuleName' )
	      if( businessName != null )
	      {
            data['BusinessName'] = businessName ;
	      }
         Loading.create() ;
         g._post( data, null, function( returnData ){
            var taskDefferred = $q.when( returnData[0] ) ;
            var taskPromise = taskDefferred.promise ;
            var taskID = returnData[0] ;
            var queryTask = function(){
               var taskData = { 'cmd': 'query task', 'filter': JSON.stringify( taskID ) } ;
               g._post( taskData, null, function( taskInfo ){
                  if( taskInfo[0]['Status'] == 0 )
                  {
                     queryTask()
                     return ;
                  }
                  if( taskInfo[0]['Status'] == 3 || taskInfo[0]['Status'] == 4 )
                  {
                     success( taskInfo[0]['ResultInfo'], true ) ;
                     Loading.cancel() ;
                     return ;
                  }
                  success( taskInfo[0]['ResultInfo'], false ) ;
                  setTimeout( queryTask, 100 ) ;
               }, failed, error, complete, false ) ;
            }
            queryTask() ;
         }, failed, error, complete ) ;
      }

      //SQL(自动获取cluster和module)
      g.Exec = function( sql, success, failed, error, complete, showLoading ){
         var data = { 'cmd': 'exec', 'sql': sql } ;
         g._post( data, function( jqXHR ){
	         var clusterName = SdbFunction.LocalData( 'SdbClusterName' ) ;
	         if( clusterName !== null )
	         {
		         jqXHR.setRequestHeader( 'SdbClusterName', clusterName ) ;
	         }
	         var businessName = SdbFunction.LocalData( 'SdbModuleName' )
	         if( businessName !== null )
	         {
		         jqXHR.setRequestHeader( 'SdbBusinessName', businessName ) ;
	         }
         }, success, failed, error, complete, showLoading ) ;
      }

      //SQL(手工设置cluster和module)
      g.Exec2 = function( clusterName, businessName, sql, success, failed, error, complete, showLoading ){
         var data = { 'cmd': 'exec', 'sql': sql } ;
         g._post( data, function( jqXHR ){
	         if( clusterName !== null )
	         {
		         jqXHR.setRequestHeader( 'SdbClusterName', clusterName ) ;
	         }
	         if( businessName !== null )
	         {
		         jqXHR.setRequestHeader( 'SdbBusinessName', businessName ) ;
	         }
         }, success, failed, error, complete, showLoading ) ;
      }

      //登录
      g.Login = function( username, password, success, failed, error, complete ){
         password = $.md5( password ) ;
         var timestamp = parseInt( ( new Date().getTime() ) / 1000 ) ;
	      var data = { 'cmd' : 'login', 'user': username, 'passwd': password, 'Timestamp': timestamp } ;
         g._post( data, null, success, failed, error, complete, false ) ;
      }

      //修改密码
      g.ChangePasswd = function( username, password, newPassword, success, failed, error, complete ){
         var timestamp = parseInt( ( new Date().getTime() ) / 1000 ) ;
         password = $.md5( password ) ;
         newPassword = $.md5( newPassword ) ;
	      var data = { 'cmd' : 'change passwd', 'User': username, 'Passwd': password, 'Newpasswd': newPassword, 'Timestamp': timestamp } ;
         g._post( data, null, success, failed, error, complete, false ) ;
      }

      g.getPing = function( complete ){
         var time1 = $.now() ;
         g.getFile( './app/language/test', true, function( text ){
            var time2 = $.now() ;
            complete( time2 - time1 ) ;
         }, function(){
            complete( -1 ) ;
         } ) ;
      }

   } ) ;
}());