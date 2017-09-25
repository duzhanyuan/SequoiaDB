//动态加载模块文件
var resolveFun = function( files ){
   return {
      deps: function( $q, $rootScope ){
         var deferred = $q.defer();
         var dependencies = files ;
         var i = 0 ;
         function loadjs( fileName )
         {
            $.getScript( fileName, function(){
               ++i ;
               if( i == dependencies.length )
               {
                  $rootScope.$apply( function(){
                     deferred.resolve() ;
                  } ) ;
               }
               else
               {
                  loadjs( dependencies[i] ) ;
               }
            } ) ;
         }
         if( dependencies.length > 0 )
         {
            loadjs( dependencies[0] ) ;
         }
         return deferred.promise ;
      }
   }
} ;

//格式化
var sprintf = function( format )
{
	var len = arguments.length;
	var strLen = format.length ;
	var newStr = '' ;
	for( var i = 0, k = 1; i < strLen; ++i )
	{
		var chars = format.charAt( i ) ;
		if( chars == '\\' && ( i + 1 < strLen ) && format.charAt( i + 1 ) == '?' )
		{
			newStr += '?' ;
			++i ;
		}
		else if( chars == '?' && k < len )
		{
			newStr += ( '' + arguments[k] ) ;
			++k ;
		}
		else
		{
			newStr += chars ;
		}
	}
	return newStr ;
} ;

//保留多少位小数
function fixedNumber( x, num )
{
   if( isNaN( x ) )
   {
      return x ;
   }
   var y = parseFloat( x );
   var z = Math.pow( 10, num );
   y = Math.round( y * z ) / z ;
   return y ;
}

//字符串补位
function pad( num, n, chars )
{
   chars = ( typeof( chars ) == 'undefined' ? '0' : chars ) ;
   var len = num.toString().length;
   while( len < n )
   {
      num = '0' + num ;
      ++len ;
   }
   return num ;
}

//获取对象的属性数量
function getObjectSize( obj )
{
   var len = 0 ;
   if( typeof( obj ) == 'object' )
   {
      $.each( obj, function(){
         ++len ;
      } ) ;
   }
   return len ;
}

//格式化日期
function timeFormat( date, fmt )
{
   var o = {
      "M+": date.getMonth() + 1,
      "d+": date.getDate(),
      "h+": date.getHours(),
      "m+": date.getMinutes(),
      "s+": date.getSeconds(),
      "q+": Math.floor( ( date.getMonth() + 3 ) / 3 ),
      "S" : date.getMilliseconds()
   } ;
   if( /(y+)/.test( fmt ) )
   {
      fmt = fmt.replace( RegExp.$1, ( date.getFullYear() + "" ).substr( 4 - RegExp.$1.length ) ) ;
   }
   for ( var k in o )
   {
      if( new RegExp( "(" + k + ")" ).test( fmt ) )
      {
         fmt = fmt.replace( RegExp.$1, ( RegExp.$1.length == 1) ? ( o[k] ) : ( ( "00" + o[k] ).substr( ( "" + o[k] ).length ) ) ) ;
      }
   }
   return fmt ;
}

//删除两端空格
function trim( str )
{　　
   return str.replace( /(^\s*)|(\s*$)/g, '' ) ; 
}

//判断是不是数组
function isArray( object ) {
   //判断length属性是否是可枚举的 对于数组 将得到false
   return object && typeof( object ) === 'object' && typeof( object.length ) === 'number' &&
            typeof( object.splice ) === 'function' && !( object.propertyIsEnumerable( 'length' ) ) ;
}

//自动判断类型并转换
//hasQuotes 如果设置成true，那么如果带有 "xxx"，则转换成 xxxx 的字符串
function autoTypeConvert( val, hasQuotes )
{
   if( typeof( val ) == 'string' )
   {
      var valLen = val.length ;
      if( valLen > 0 )
      {
         if( hasQuotes == true )
         {
            if( valLen > 1 && val.charAt(0) == '"' && val.charAt(valLen - 1) == '"' )
            {
               return val.substr( 1, valLen - 2 ) ;
            }
         }
         if( val.toLowerCase() == 'null' )
         {
            val = null ;
         }
         else if( val.toLowerCase() == 'true' )
         {
            val = true ;
         }
         else if( val.toLowerCase() == 'false' )
         {
            val = false ;
         }
         else if( val == 'minKey' )
         {
            val = { '$minkey': 1 } ;
         }
         else if( val == 'maxKey' )
         {
            val = { '$maxkey': 1 } ;
         }
         else if( val == 'undefined' )
         {
            val = { '$undefined': 1 } ;
         }
         else if( !isNaN( val ) )
         {
            val = Number( val ) ;
         }
      }
   }
   return val ;
}

/*
   数组结构 -> json
   例:
   [
      { key: 'Object', type: 'Object', level: 0, isOpen: false, val: [
         { key: 'a', type: 'Auto', level: 1, isOpen: false, val: '123' },
         { key: 'b', type: 'Object', level: 1, isOpen: false, val: [
            { key: 'c', type: 'String', level: 2, isOpen: false, val: 'hello' },
            { key: 'd', type: 'Auto', level: 2, isOpen: false, val: 'true' }
         ] },
         { key: 'e', type: 'Array', level: 1, isOpen: false, val: [
            { key: '', type: 'Auto', level: 2, isOpen: false, val: '7' },
            { key: '', type: 'Auto', level: 2, isOpen: false, val: '8' },
            { key: '', type: 'Auto', level: 2, isOpen: false, val: '9' }
         ] },
      ] }
   ]
   转成
   {
      a : 123,
      b : {
        c: "hello",
        d: true
      }
      e: [ 7, 8, 9 ]
   }
*/
function array2Json( array, parentType )
{
   if( typeof( parentType ) == 'undefined' || ( parentType != 'Object' && parentType != 'Array' ) ) parentType = 'Object' ;
   var json ;
   if( parentType == 'Object' )
   {
      json = {} ;
   }
   else
   {
      json = [] ;
   }
   $.each( array, function( index, field ){
      if( field['type'] == 'Object' && field['level'] == 0 )
      {
         json = array2Json( field['val'], field['type'] ) ;
         return false ;
      }
      else if( field['type'] == 'Object' || field['type'] == 'Array' )
      {
         var val = field['val'] ;
         if( parentType == 'Object' )
         {
            json[ field['key'] ] = array2Json( val, field['type'] ) ;
         }
         else
         {
            json.push( array2Json( val, field['type'] ) ) ;
         }
      }
      else if( field['type'] == 'Auto' )
      {
         var val = trim( field['val'] ) ;
         val = autoTypeConvert( val ) ;
         if( parentType == 'Object' )
         {
            json[ field['key'] ] = val ;
         }
         else
         {
            json.push( val ) ;
         }
      }
      else if( field['type'] == 'String' )
      {
         var val = field['val'] ;
         if( parentType == 'Object' )
         {
            json[ field['key'] ] = val ;
         }
         else
         {
            json.push( val ) ;
         }
      }
      else if( field['type'] == 'Binary' )
      {
         var binary = trim( field['val'] ) ;
         var binType = '' ;
         if( binary.length > 0 && binary.charAt(0) == '(' && binary.indexOf( ')' ) >= 0 )
         {
            var right = binary.indexOf( ')' ) ;
            binType = binary.substr( 1, right - 1 ) ;
            binary = binary.substr( right + 1 ) ;
         }
         if( parentType == 'Object' )
         {
            json[ field['key'] ] = { '$binary': binary, '$type': binType } ;
         }
         else
         {
            json.push( { '$binary': binary, '$type': binType } ) ;
         }
      }
      else if( field['type'] == 'Timestamp' )
      {
         var val = trim( field['val'] ) ;
         if( parentType == 'Object' )
         {
            json[ field['key'] ] = { '$timestamp': val } ;
         }
         else
         {
            json.push( { '$timestamp': val } ) ;
         }
      }
      else if( field['type'] == 'Date' )
      {
         var val = trim( field['val'] ) ;
         if( parentType == 'Object' )
         {
            json[ field['key'] ] = { '$date': val } ;
         }
         else
         {
            json.push( { '$date': val } ) ;
         }
      }
      else if( field['type'] == 'ObjectId' )
      {
         var val = trim( field['val'] ) ;
         val = pad( val, 24, '0' ) ;
         if( parentType == 'Object' )
         {
            json[ field['key'] ] = { '$oid': val } ;
         }
         else
         {
            json.push( { '$oid': val } ) ;
         }
      }
      else if( field['type'] == 'Regex' )
      {
         var regex = trim( field['val'] ) ;
         var options = '' ;
         if( regex.charAt(0) == '/' && regex.indexOf( '/', 1 ) > 0 )
         {
            var right = regex.indexOf( '/', 1 ) ;
            options = regex.substr( right + 1 ) ;
            regex = regex.substr( 1, right - 1 ) ;
         }
         if( parentType == 'Object' )
         {
            json[ field['key'] ] = { '$regex': regex, '$options': options } ;
         }
         else
         {
            json.push( { '$regex': regex, '$options': options } ) ;
         }
      }
   } ) ;
   return json ;
}

/*
   json -> 数组结构
   例:
   {
      a : 123,
      b : {
        c: "hello",
        d: true
      }
      e: [ 7, 8, 9 ]
   }
   转成
   [
      { key: 'Object', type: 'Object', level: 0, isOpen: false, val: [
         { key: 'a', type: 'Auto', level: 1, isOpen: false, val: '123' },
         { key: 'b', type: 'Object', level: 1, isOpen: false, val: [
            { key: 'c', type: 'String', level: 2, isOpen: false, val: 'hello' },
            { key: 'd', type: 'Auto', level: 2, isOpen: false, val: 'true' }
         ] },
         { key: 'e', type: 'Array', level: 1, isOpen: false, val: [
            { key: '', type: 'Auto', level: 2, isOpen: false, val: '7' },
            { key: '', type: 'Auto', level: 2, isOpen: false, val: '8' },
            { key: '', type: 'Auto', level: 2, isOpen: false, val: '9' }
         ] },
      ] }
   ]
*/
function json2Array( json, level, exact )
{
   if( isNaN( level ) ) level = 0 ;
   if( typeof( exact ) == 'undefined' ) exact = false ;
   var array = [] ;
   if( level == 0 )
   {
      var child = json2Array( json, level + 1, exact ) ;
      array.push( { key: 'Object', type: 'Object', level: 0, isOpen: false, val: child } ) ;
      return array ;
   }
   $.each( json, function( key, value ){
      key = key + '' ;
      var valueType = typeof( value ) ;
      if( valueType == 'object' )
      {
         if( value == null )
         {
            value = 'null' ;
            valueType = 'Auto' ;
            if( exact == true )
            {
               valueType = 'Null' ;
            }
         }
         else if( isArray( value ) )
         {
            value = json2Array( value, level + 1, exact ) ;
            valueType = 'Array' ;
         }
         else if( typeof( value['$binary'] ) == 'string' && typeof( value['$type'] ) == 'string' )
         {
            var binary = value['$binary'] ;
            var binType = value['$type'] ;
            value = binary ;
            if( binType.length > 0 )
            {
                value = '(' + binType + ')' + value ;
            }
            valueType = 'Binary' ;
         }
         else if( typeof( value['$timestamp'] ) == 'string' )
         {
            value = value['$timestamp'] ;
            valueType = 'Timestamp' ;
         }
         else if( typeof( value['$date'] ) == 'string' )
         {
            value = value['$date'] ;
            valueType = 'Date' ;
         }
         /*
         else if( typeof( value['$code'] ) == 'string' )
         {
            value = value['$code'] ;
            valueType = 'Code' ;
         }
         */
         else if( typeof( value['$minKey'] ) == 'number' )
         {
            value = 'minKey' ;
            valueType = 'Auto' ;
            if( exact == true )
            {
               valueType = 'MinKey' ;
            }
         }
         else if( typeof( value['$maxKey'] ) == 'number')
         {
            value = 'maxKey' ;
            valueType = 'Auto' ;
            if( exact == true )
            {
               valueType = 'MaxKey' ;
            }
         }
         else if( typeof( value['$undefined'] ) == 'number' )
         {
            value = 'undefined' ;
            valueType = 'Auto' ;
            if( exact == true )
            {
               valueType = 'Undefined' ;
            }
         }
         else if( typeof( value['$oid'] ) == 'string' )
         {
            value = value['$oid'] ;
            value = pad( value, 24, '0' ) ;
            valueType = 'ObjectId' ;
         }
         else if( typeof( value['$regex'] ) == 'string' && typeof( value['$options'] ) == 'string' )
         {
            value = '/' + value['$regex'] + '/' + value['$options'] ;
            valueType = 'Regex' ;
         }
         else
         {
            value = json2Array( value, level + 1, exact ) ;
            valueType = 'Object' ;
         }
      }
      else if( valueType == 'boolean' )
      {
         value = ( value ? 'true' : 'false' ) ;
         valueType = 'Auto' ;
         if( exact == true )
         {
            valueType = 'Bool' ;
         }
      }
      else if (valueType == 'number')
      {
         if( value == Number.POSITIVE_INFINITY )
         {
            value = '1.7976931348623157e+308' ;
         }
         else if( value == Number.NEGATIVE_INFINITY )
         {
            value = '-1.7976931348623157e+308' ;
         }
         else
         {
            value = value + '' ;
         }
         valueType = 'Auto' ;
         if( exact == true )
         {
            valueType = 'Number' ;
         }
      }
      else if (valueType == 'string')
      {
         value = value ;
         valueType = 'String' ;
      }
      else
      {
         
      }
      array.push( { key: key, type: valueType, level: level, isOpen: false, val: value } ) ;
   } ) ;
   return array ;
}

//打印调试
function printfDebug( text )
{
   try
   {
      if( window.SdbDebug == true )
         console.warn( text ) ;
   }
   catch( e ){}
}

//带小数就进位
function numberCarry( num )
{
   var intNum = parseInt( num ) ;
   if( intNum != num )
   {
      num = intNum + 1 ;
   }
   return num ;
}

//获取操作系统信息
function getSystemInfo()
{
   var nAgt = navigator.userAgent;
   var os = 'unknown' ;
   var clientStrings = [
       { s: 'Windows 10', r: /(Windows 10.0|Windows NT 10.0)/ },
       { s: 'Windows 8.1', r: /(Windows 8.1|Windows NT 6.3)/ },
       { s: 'Windows 8', r: /(Windows 8|Windows NT 6.2)/ },
       { s: 'Windows 7', r: /(Windows 7|Windows NT 6.1)/ },
       { s: 'Windows Vista', r: /Windows NT 6.0/ },
       { s: 'Windows Server 2003', r: /Windows NT 5.2/ },
       { s: 'Windows XP', r: /(Windows NT 5.1|Windows XP)/ },
       { s: 'Windows 2000', r: /(Windows NT 5.0|Windows 2000)/ },
       { s: 'Windows ME', r: /(Win 9x 4.90|Windows ME)/ },
       { s: 'Windows 98', r: /(Windows 98|Win98)/ },
       { s: 'Windows 95', r: /(Windows 95|Win95|Windows_95)/ },
       { s: 'Windows NT 4.0', r: /(Windows NT 4.0|WinNT4.0|WinNT|Windows NT)/ },
       { s: 'Windows CE', r: /Windows CE/ },
       { s: 'Windows 3.11', r: /Win16/ },
       { s: 'Android', r: /Android/ },
       { s: 'Open BSD', r: /OpenBSD/ },
       { s: 'Sun OS', r: /SunOS/ },
       { s: 'Linux', r: /(Linux|X11)/ },
       { s: 'iOS', r: /(iPhone|iPad|iPod)/ },
       { s: 'Mac OS X', r: /Mac OS X/ },
       { s: 'Mac OS', r: /(MacPPC|MacIntel|Mac_PowerPC|Macintosh)/ },
       { s: 'QNX', r: /QNX/ },
       { s: 'UNIX', r: /UNIX/ },
       { s: 'BeOS', r: /BeOS/ },
       { s: 'OS/2', r: /OS\/2/ },
       { s: 'Search Bot', r: /(nuhk|Googlebot|Yammybot|Openbot|Slurp|MSNBot|Ask Jeeves\/Teoma|ia_archiver)/ }
   ];
   for (var id in clientStrings)
   {
      var cs = clientStrings[id];
      if (cs.r.test(nAgt))
      {
         os = cs.s;
         break;
      }
   }
   var osVersion = 'unknown';
   if (/Windows/.test(os)) {
      osVersion = /Windows (.*)/.exec(os)[1];
      os = 'Windows';
   }
   switch( os )
   {
   case 'Mac OS X':
      osVersion = /Mac OS X (10[\.\_\d]+)/.exec(nAgt)[1];
      break;
   case 'Android':
      osVersion = /Android ([\.\_\d]+)/.exec(nAgt)[1];
      break;
   case 'iOS':
      osVersion = /OS (\d+)_(\d+)_?(\d+)?/.exec(nVer);
      osVersion = osVersion[1] + '.' + osVersion[2] + '.' + (osVersion[3] | 0);
      break;
   }
   return [os, osVersion] ;
}

//IE7不支持对象用indexOf，为了兼容所以加上代码
if(!Array.indexOf)
{
    Array.prototype.indexOf = function(obj)
    {              
        for(var i=0; i<this.length; i++)
        {
            if(this[i]==obj)
            {
                return i;
            }
        }
        return -1;
    }
}

//解析condition的值
function parseConditionValue( condition )
{
   var filter = {} ;
   var jobj = condition ;
   if( jobj['condition'].length > 1 || ( jobj['condition'].length == 1 && jobj['condition'][0]['field'].length > 0 ) )
   {
      filter = [] ;
      $.each( jobj['condition'], function( index, field ){
         var subCondition = {} ;
         var fieldValue = autoTypeConvert( trim( field['value'] ), true ) ;
         switch( field['logic'] )
         {
         case '>':
            subCondition[ field['field'] ] = { '$gt': fieldValue } ;
            break ;
         case '>=':
            subCondition[ field['field'] ] = { '$gte': fieldValue } ;
            break ;
         case '<':
            subCondition[ field['field'] ] = { '$lt': fieldValue } ;
            break ;
         case '<=':
            subCondition[ field['field'] ] = { '$lte': fieldValue } ;
            break ;
         case '!=':
            subCondition[ field['field'] ] = { '$ne': fieldValue } ;
            break ;
         case '=':
            subCondition[ field['field'] ] = fieldValue ;
            break ;
         case 'size':
            subCondition[ field['field'] ] = { '$size': fieldValue } ;
            break ;
         case 'regex':
            var regex = trim( fieldValue ) ;
            var options = '' ;
            if( regex.charAt(0) == '/' && regex.indexOf( '/', 1 ) > 0 )
            {
               var right = regex.indexOf( '/', 1 ) ;
               options = regex.substr( right + 1 ) ;
               regex = regex.substr( 1, right - 1 ) ;
            }
            subCondition[ field['field'] ] = { '$regex': regex, '$options': options } ;
            break ;
         case 'type':
            if( isNaN( fieldValue ) )
            {
               fieldValue = fieldValue.toLowerCase() ;
            }
            switch( fieldValue )
            {
            case 'int':
            case 'int32':
            case 'integer':
               fieldValue = 16 ;
               break ;
            case 'long':
            case 'long long':
            case 'int64':
               fieldValue = 18 ;
               break ;
            case 'double':
            case 'float':
               fieldValue = 1 ;
               break ;
            case 'string':
            case 'char':
            case '""':
               fieldValue = 2 ;
               break ;
            case 'objectid':
            case '_id':
            case 'id':
               fieldValue = 7 ;
               break ;
            case 'bool':
            case 'boolean':
            case 'true':
            case 'false':
               fieldValue = 8 ;
               break ;
            case 'date':
               fieldValue = 9 ;
               break ;
            case 'time':
            case 'timestamp':
               fieldValue = 17 ;
               break ;
            case 'bin':
            case 'binary':
            case 'data':
            case 'binary data':
               fieldValue = 5 ;
               break ;
            case 'regex':
            case 'regular':
            case 'regular expression':
            case 'regexp':
            case 're':
               fieldValue = 11 ;
               break ;
            case 'object':
            case 'obj':
               fieldValue = 3 ;
               break ;
            case 'array':
            case 'arr':
               fieldValue = 4 ;
               break ;
            case 'null':
            case 'nil':
               fieldValue = 10 ;
               break ;
            default:
               fieldValue = parseInt( fieldValue ) ;
               break ;
            }
            subCondition[ field['field'] ] = { '$type': fieldValue } ;
            break ;
         case 'null':
            subCondition[ field['field'] ] = { '$isnull': 1 } ;
            break ;
         case 'notnull':
            subCondition[ field['field'] ] = { '$isnull': 0 } ;
            break ;
         case 'exists':
            subCondition[ field['field'] ] = { '$exists': 1 } ;
            break ;
         case 'notexists':
            subCondition[ field['field'] ] = { '$exists': 0 } ;
            break ;
         }
         filter.push( subCondition ) ;
      } ) ;
      if( jobj['model'] == 'or' )
      {
         filter = { '$or': filter } ;
      }
      else
      {
         filter = { '$and': filter } ;
      }
   }
   return filter ;
}

//解析selector的值
function parseSelectorValue( jobj ){
   var selector = {} ;
   if( jobj.length > 1 || ( jobj.length == 1 && jobj[0]['field'].length > 0 ) )
   {
      $.each( jobj, function( index, field ){
         selector[ field['field'] ] = 1 ;
      } ) ;
   }
   return selector ;
}

//解析sort的值
function parseSortValue( jobj ){
   var sort = {} ;
   if( jobj.length > 1 || ( jobj.length == 1 && jobj[0]['field'].length > 0 ) )
   {
      $.each( jobj, function( index, field ){
         sort[ field['field'] ] = autoTypeConvert( field['order'], false )  ;
      } ) ;
   }
   return sort ;
}

//解析hint的值
function parseHintValue( jobj )
{
   var hint = {} ;
   if( jobj === 1 )
   {
      hint[''] = null ;
   }
   else
   {
      hint[''] = jobj ;
   }
   return hint ;
}

//解析updator的值
function parseUpdatorValue( jobj ){
   var updator = {} ;
   $.each( jobj, function( index, field ){
      var fieldValue = autoTypeConvert( field['value'], true ) ;
      updator[ field['field'] ] = fieldValue ;
   } ) ;
   updator = { '$set': updator } ;
   return updator ;
}

//解析indexDef的值
function parseIndexDefValue( jobj ){
   var indexDef = {} ;
   if( jobj.length > 1 || ( jobj.length == 1 && jobj[0]['field'].length > 0 ) )
   {
      $.each( jobj, function( index, field ){
         indexDef[ field['field'] ] = autoTypeConvert( field['order'], false )  ;
      } ) ;
   }
   return indexDef ;
}

/*
   str    是字符串
   state  是状态机，结构如下
   {
      status: 0,         <---  状态
      length: 0,         <---  一行的长度
      header: [          <---  表头
         6, 4, 5 , 10    <---  对应每列字段的最大长度
      ],
      field: [           <---  字段列表
         { name: "field_1", type: "int" }  <---  字段名 和 字段类型
         { name: "field_2", type: "text" }
      ],
      value: [           <---  获得的数据
         "aaaa",
         "bbbb"
      ],
      isJoin: false,     <---  当前行是不是上一行的追加内容
      attr: [
         "Has OIDs: no",
         "Options: appendonly=true",
         "Distributed randomly"
      ],
      rc: true           <---  执行成功或失败
      result: 'ERROR:  syntax error at or near "aaa"'   <---  执行成功或失败返回的内容
   }
   status 当前状态 0: 未发现表头， 1：找到表头， 2：找到字段表， 3：表头结束， 4：内容， 5：内容结束， 6：其他数据， 7:解析错误, 8:解析返回结果
*/
function parseSSQL( str, state )
{
   if( !state )
   {
      state = { 'status': 0 } ;
   }
   if( typeof( str ) != 'string' )
   {
      state['status'] = 7 ;
      return state ;
   }
   var sqlStrlen = function(str){
      var len = 0;
      for (var i=0; i<str.length; i++) { 
         var c = str.charCodeAt(i); 
         if ((c >= 0x0001 && c <= 0x007e) || (0xff60<=c && c<=0xff9f)) { 
            len++; 
         } 
         else { 
            len+=2; 
         } 
      } 
      return len;
   }
   var parseSSQLResult = function( result, state ){
      if( state['status'] != 8 )
      {
         if( result.indexOf( 'ERROR:' ) == 0 )
         {
            state['status'] = 8 ;
            state['rc'] = false ;
         }
         else if( result.indexOf( 'psql:' ) == 0 )
         {
            state['status'] = 8 ;
            state['rc'] = false ;
         }
         else if( result.indexOf( 'CREATE DATABASE' ) == 0 )
         {
            state['status'] = 8 ;
            state['rc'] = true ;
         }
         else if( result.indexOf( 'DROP DATABASE' ) == 0 )
         {
            state['status'] = 8 ;
            state['rc'] = true ;
         }
         else if( result.indexOf( 'CREATE TABLE' ) == 0 )
         {
            state['status'] = 8 ;
            state['rc'] = true ;
         }
         else if( result.indexOf( 'DROP TABLE' ) == 0 )
         {
            state['status'] = 8 ;
            state['rc'] = true ;
         }
         else if( result.indexOf( 'ALTER TABLE' ) == 0 )
         {
            state['status'] = 8 ;
            state['rc'] = true ;
         }
         else if( result.indexOf( 'INSERT ' ) == 0 )
         {
            state['status'] = 8 ;
            state['rc'] = true ;
         }
      }
      if( state['status'] == 8 )
      {
         if( typeof( state['result'] ) == 'undefined' )
         {
            state['result'] = '' ;
         }
         else
         {
            state['result'] += ', ' ;
         }
         state['result'] += result ;
      }
      return state ;
   }
   var parseSSQLHeader = function( header, state ){
      var len, char, start, end, fieldMaxLen ;
      len = header.length ;
      if( len < 5 )
      {
         return parseSSQLResult( header, state ) ;
      }
      if( header.charAt(0) != '+' )
      {
         return parseSSQLResult( header, state ) ;
      }
      state['header'] = [] ;
      state['length'] = len ;
      start = 0 ;
      for( var i = 1; i < len; ++i )
      {
         char = header.charAt(i) ;
         if( char == '-' )
         {
            continue ;
         }
         else if( char == '+' )
         {
            end = i ;
            if( end - start < 4 )
            {
               state['status'] = 7 ;
               return state ;
            }
            state['header'].push( end - start - 3 ) ;
            start = i ;
         }
         else
         {
            break ;
         }
      }
      state['status'] = 1 ;
      return state ;
   }
   var parseSSQLField = function( fields, state ){
      var len, start, end ;
      len = fields.length ;
      if( len != state['length'] )
      {
         state['status'] = 7 ;
         return state ;
      }
      if( str.charAt(0) != '|' )
      {
         state['status'] = 7 ;
         return state ;
      }
      if( state['field'] && state['field'].length > 0 )
      {
         state['status'] = 2 ;
         return state ; 
      }
      state['field'] = [] ;
      start = 0 ;
      for( var i = 0; i < state['header'].length; ++i )
      {
         start = start + 2 ;
         state['field'].push( { 'name': trim( fields.substr( start, state['header'][i] ) ) } ) ;
         start = start + state['header'][i] + 1 ;
      }
      state['status'] = 2 ;
      return state ; 
   }
   var parseSSQLEndHeader = function( header, state ){
      var len, char, start, end ;
      len = header.length ;
      if( len != state['length'] )
      {
         state['status'] = 7 ;
         return state ;
      }
      if( header.charAt(0) != '+' ) return state ;
      state['length'] = len ;
      start = 0 ;
      for( var i = 1; i < len; ++i )
      {
         char = header.charAt(i) ;
         if( char == '-' )
         {
            continue ;
         }
         else if( char == '+' )
         {
            end = i ;
            if( end - start < 4 )
            {
               state['status'] = 7 ;
               return state ;
            }
            start = i ;
         }
         else
         {
            break ;
         }
      }
      state['status'] = 3 ;
      return state ; 
   }
   var parseSSQLContent = function( content, state ){
      var len, start, end ;
      len = sqlStrlen( content ) ;
      state['isJoin'] = false ;
      if( len != state['length'] )
      {
         state['status'] = 7 ;
         return state ;
      }
      if( str.charAt(0) == '|' )
      {
         state['status'] = 4 ;
         state['value'] = [] ;
         start = 0 ;
         for( var i = 0; i < state['header'].length; ++i )
         {
            start = start + 2 ;
            state['value'].push( trim( content.substr( start, state['header'][i] ) ) ) ;
            start = start + state['header'][i] + 1 ;
         }
      }
      else if( str.charAt(0) == '+' )
      {
         state['status'] = 5 ;
      }
      else
      {
         state['status'] = 7 ;
      }
      return state ; 
   }
   var parseSSQLAttr = function( attr, state ){
      state['status'] = 6 ;
      state['value'] = [] ;
      if( typeof( state['attr'] ) == 'undefined' )
      {
         state['attr'] = [] ;
      }
      attr = trim( attr ) ;
      if( attr.length > 0 )
      {
         state['attr'].push( attr ) ;
      }
      return state ;
   }
   switch( state['status'] )
   {
   case 0:
      state = parseSSQLHeader( str, state ) ;
      break ;
   case 1:
      state = parseSSQLField( str, state ) ;
      break ;
   case 2:
      state = parseSSQLEndHeader( str, state ) ;
      break ;
   case 3:
   case 4:
      state = parseSSQLContent( str, state ) ;
      break ;
   case 5:
   case 6:
      state = parseSSQLAttr( str, state ) ;
      break ;
   case 8:
      state = parseSSQLResult( str, state ) ;
   }
   return state ;
}

//sql转义
function sqlEscape( sql )
{
   sql = sql.replace( /\\/g, "\\\\" ) ;
   return "'" + sql.replace( /'/g, "\\'" ) + "'" ;
}