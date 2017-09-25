/*******************************************************************************

   Copyright (C) 2012-2014 SequoiaDB Ltd.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*******************************************************************************/
/*
@description: common function for all the js files in current document
@modify list:
   2014-7-26 Zhaobo Tan  Init
*/

var FILE_NAME_FUNC = "func.js" ;
var errMsg         = "" ;
var rc             = SDB_OK ;

/* *****************************************************************************
@discretion: handle system exception
@author: Tanzhaobo
@parameter
   exp[object]: all kinds of exceptions
@return void
***************************************************************************** */
function SYSEXPHANDLE( exp )
{
   if ( "number" != typeof(exp) )
   {
      if ( "object" == typeof(exp) )
         setLastErrMsg( exp.message ) ;
      else
         setLastErrMsg( exp + "" ) ;
      setLastError( SDB_SYS ) ;
   }
}

/* *****************************************************************************
@discretion: get last error number
@author: Tanzhaobo
@parameter void
@return
   errno[int] the error number
***************************************************************************** */
function GETLASTERROR ()
{
   var errno = getLastError() ;
   if ( undefined == errno || "number" != typeof(errno) || 0 < errno )
      errno = SDB_SYS ;
   return errno ;
}

/* *****************************************************************************
@discretion: get last error message
@author: Tanzhaobo
@parameter void
@return
   errmsg[string]: the error message
***************************************************************************** */
function GETLASTERRMSG ()
{
   var errmsg = getLastErrMsg() ;
   if ( undefined == errmsg )
      errmsg = "" ;
   return errmsg ;
}

/* *****************************************************************************
@discretion: handle exception
@author: Tanzhaobo
@parameter
   exp[object]: the exception
   msg[string]: error message 
@return void
***************************************************************************** */
function exception_handle( exp, msg )
{
   setLastErrMsg( msg ) ;
   if ( "number" == typeof( exp ) && exp < 0 )
   {
      setLastError( exp ) ;
      throw exp ;
   }
   else
   {
      setLastError( SDB_SYS ) ;
      throw SDB_SYS ;
   }
}

/* *****************************************************************************
@discretion: remove the "\n" or "\n\r" in the end of string
@author: Tanzhaobo
@parameter
   str[string]: the string to deal with
@return
   retStr[string]: the return string without "\n" or "\n\r"
***************************************************************************** */
function removeLineBreak ( str )
{
   var retStr = str ;

   if ( SYS_LINUX == SYS_TYPE )
   {
      var i = str.indexOf( "\n" ) ;
      if ( -1 != i )
      {
         var substr = str.substring(0, i);
         retStr = substr ;
      }
      else
      {
         retStr = str ;
      }
   }
   else
   {
      // TODO:
   }
   return retStr ;
}

/* *****************************************************************************
@discretion: gen a string with the format "YYYY-MM-DD-HH:mm:ss.fff"
@author: Tanzhaobo
@parameter
@return
   retStr[string]: a string with the format "YYYY-MM-DD-HH:mm:ss.fff"
                   to express current timestamp
***************************************************************************** */
function genTimeStamp()
{
   var retStr = null ;
   var dateVar = new Date() ;
   var dateStr = dateVar.toLocaleDateString() ;
   var timeStr = dateVar.toLocaleTimeString() ;
   var millStr = _sprintf('%03s', '' + dateVar.getMilliseconds()) ;
   var strs = dateStr.split( '/' ) ;
   retStr = strs[2] + "-" + strs[0] + "-" + strs[1] + "-" + timeStr + "." + millStr ;

   return retStr ;
}

/* *****************************************************************************
@discretion: sleep n milliseconds
@author: YouBin Lin
@parameter
  n:      milliseconds
@return
***************************************************************************** */
function sleep(n)
{
   var start = new Date().getTime();
   while( true )
   {
      if ( new Date().getTime() - start > n )
      {
         break ;
      }
   }
}

/* *****************************************************************************
@discretion: check whether it's in local host environment
@author: Tanzhaobo
@parameter
   ssh[object]: Ssh object
@return
   [bool]: whether it's in local host environment
***************************************************************************** */
function isInLocalHost( ssh )
{
   var ip1 = ssh.getLocalIP() ;
   var ip2 = ssh.getPeerIP() ;
   if( ip1 == ip2 )
      return true ;
   else
      return false ;
}

/* *****************************************************************************
@discretion: adapt path with "/"(linux) or "\\"(window) in the end
@author: Tanzhaobo
@parameter
   path[string]: a path
@return
   [string]: a path with "/" or "\\" in the end
***************************************************************************** */
function adaptPath( path )
{
   var s = "" ;
   var i = -1 ;
   if ( SYS_LINUX == SYS_TYPE )
   {
      s = "/" ;
   }
   else
   {
      s = "\\" ;
   }
   i = path.lastIndexOf( s ) ;
   if ( i != ( path.length -1 ) )
   {
      path += s ;
   }
   return path ;
}

/* *****************************************************************************
@discretion: judge whethe a value is array or not
@author: Tanzhaobo
@parameter
   value[]: input value
@return
   [bool]: return whethe the value is array
***************************************************************************** */
function isArray( value )
{
   return Object.prototype.toString.call(value) === '[object Array]' ;
}

/* *****************************************************************************
@discretion: judge whethe a value is array or not
@author: Tanzhaobo
@parameter
   port[int]: input port
@return
   ret[bool]: return whether the port is a reserved port
***************************************************************************** */
function isReservedPort( port )
{
   var ret = false ;
   var len = OMA_RESERVED_PORT.length ;
   for ( var i = 0; i < len; i++ )
   {
      var val = OMA_RESERVED_PORT[i] ;
      var flag = isArray( val ) ;
      if ( flag )
      {
         var port1 = val[0] ;
         var port2 = val[1] ;
         if ( port1 <= port && port <= port2 )
         {

            ret = true ;
            break ;
         } 
      }
      else
      {
         if ( port === val )
         {
            ret = true ;
            break ;
         }
      }
   }
   return ret ;
}

/* *****************************************************************************
@discretion: mimic "sprintf" in "C" simply 
@author: Tanzhaobo
@parameter
   format[string]: e.g. "a = ?, b = ?, is it right/? "
@return
   newStr[string]: e.g. "a = 1, b = 2, is it right? "
@usage
   var str = sprintf( "a = ?, b = ?, is it right/? ", 1, 2, '?' ) ;
***************************************************************************** */
function sprintf( format )
{
   var len = arguments.length ;
   var strLen = format.length ;
   var newStr = '' ;
   for ( var i = 0, k = 1; i < strLen; i++ )
   {
      var char = format.charAt( i ) ;
      if ( char == '\\' && (i + 1 < strLen) && format.charAt(i + 1) == '?' )
      {
         newStr += '?' ;
         i++ ;
      }
      else if ( char == '?' && k < len )
      {
         newStr += ( '' + arguments[k] ) ;
         ++k ;
      }
      else
      {
         newStr += char ;
      }
   }
   return newStr ;
}

/* *****************************************************************************
@discretion: create temporary directory in remote host
@author: Tanzhaobo
@parameter
   ssh[object]: ssh object
@return void
***************************************************************************** */
function createTmpDir( ssh )
{
   var str = "" ;
   // directories make in target host /tmp   
   var dirs = [ OMA_PATH_TEMP_OMA_DIR,
                OMA_PATH_TEMP_BIN_DIR,
                OMA_PATH_TEMP_PACKET_DIR,
                OMA_PATH_TEMP_CONF_DIR,
                OMA_PATH_TEMP_DATA_DIR,
                OMA_PATH_TMP_WEB_DIR,
                OMA_PATH_TEMP_LOG_DIR,
                OMA_PATH_TEMP_LOCAL_DIR,
                OMA_PATH_TEMP_SPT_DIR,
                OMA_PATH_TEMP_TEMP_DIR ] ;
   try
   {
      if ( SYS_LINUX == SYS_TYPE )
      {
        // rm /tmp/omatmp
        str = "rm " + OMA_PATH_TEMP_OMA_DIR + " -rf " ;
        ssh.exec( str ) ;
        // mkdir dirs
        for ( var i = 0; i < dirs.length; i++ )
        {
           str = "mkdir -p " + dirs[i] ;
           ssh.exec( str ) ;
        }
      }
      else
      {
         // TODO: tanzhaobo
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to create temporary directory in host[" + ssh.getPeerIP() + "]" ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
      exception_handle( rc, errMsg ) ;
   }
}

/* *****************************************************************************
@discretion: remove the temporary directory and files in temporary directory
@author: Tanzhaobo
@parameter
   ssh[object]: ssh object
@return void
***************************************************************************** */
function removeTmpDir( ssh )
{
   var str = "" ;
   
   try
   {   
      if ( SYS_LINUX == SYS_TYPE )
      {
         str = "rm -rf " + OMA_PATH_TEMP_OMA_DIR2 ;
         ssh.exec( str ) ;
      }
      else
      {
         // TODO:
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to remove temporary directory in host [" + ssh.getPeerIP() + "]" ;
      PD_LOG( arguments, PDWARNING, FILE_NAME_FUNC,
              errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
   }
}

/* *****************************************************************************
@discretion: remove the temporary directory but leave log files
@author: Tanzhaobo
@parameter
   ssh[object]: ssh object
@return void
***************************************************************************** */
function removeTmpDir2( ssh )
{
   var str = "" ;
   // directories need to be removed in target host
   var dirs = [ OMA_PATH_TEMP_BIN_DIR,
                OMA_PATH_TEMP_PACKET_DIR,
                OMA_PATH_TEMP_SPT_DIR,
                OMA_PATH_TEMP_LOCAL_DIR,
                OMA_PATH_TMP_WEB_DIR ] ;
   try
   {
      if ( SYS_LINUX == SYS_TYPE )
      {
        // remove dirs
        for ( var i = 0; i < dirs.length; i++ )
        {
           str = "rm -rf " + dirs[i] ;
           ssh.exec( str ) ;
        }
      }
      else
      {
         // TODO: tanzhaobo
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to remove temporary directory in host[" + ssh.getPeerIP() + "]" ;
      PD_LOG( arguments, PDWARNING, FILE_NAME_FUNC,
              errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
   }
}

/* *****************************************************************************
@discretion: get the script's path
@author: Tanzhaobo
@parameter
   path[string]: the path of current program working
                 e.g. /opt/sequoiadb/bin
@return
   retStr[string]: the path of script files
***************************************************************************** */
function getSptPath( path )
{
   var retStr = "" ;
   var str = "" ;
   var pos = -1 ;

   if ( SYS_LINUX == SYS_TYPE )
   {
      retStr = adaptPath( path ) ;
      str = "/" ;
      pos = path.lastIndexOf( str, retStr.length - 2 ) ;
      if ( -1 == pos )
      {
         setLastErrMsg( "Invalid sdbcm running path: " + path ) ;
         setLastError( SDB_INVALIDARG ) ;
         throw SDB_INVALIDARG ;
      }
      retStr = path.substring( 0, pos + 1 ) + "conf/script/" ;
   }
   else
   {
      // TODO:
   }
   return retStr ;
}

/******************************************************************************
@discretion: create the psql's task workpath
@author: YouBin Lin
@parameter
   taskID:    task's id
@return
***************************************************************************** */
function createPsqlTaskWorkPath( taskID )
{
   var taskWorkPath = "" ;
   
   try
   {
      if ( SYS_LINUX == SYS_TYPE )
      {
         taskWorkPath = OMA_PATH_OMA_WORK_TASK_DIR + '/' + taskID ;
         File.mkdir( taskWorkPath ) ;
      }
      else
      {
         // TODO:
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to create task work path[" + taskWorkPath + "]" ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
      exception_handle( rc, errMsg ) ;
   }

   return ;
}

/******************************************************************************
@discretion: remove the psql's task workpath
@author: YouBin Lin
@parameter
   taskID:    task's id
@return
***************************************************************************** */
function removePsqlTaskWorkPath( taskID )
{
   var taskWorkPath = "" ;
   try
   {
      if ( SYS_LINUX == SYS_TYPE )
      {
         taskWorkPath = OMA_PATH_OMA_WORK_TASK_DIR + '/' + taskID ;
         var isPathExist = File.exist( taskWorkPath ) ;
         if ( isPathExist == true )
         {
            File.remove( taskWorkPath ) ;
         }
         else
         {
            PD_LOG( arguments, PDEVENT, FILE_NAME_FUNC,
                    "task work path is not exist[" + taskWorkPath + "]" ) ;
         }
      }
      else
      {
         // TODO:
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to remove task work path[" + taskWorkPath + "]" ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
      exception_handle( rc, errMsg ) ;
   }

   return ;
}

/******************************************************************************
@discretion: check if task's work dirpath exists
@author: YouBin Lin
@parameter
   taskID[INT64]: task's id
@return
   true/false
***************************************************************************** */
function isTaskWorkDirExist( taskID )
{
   var isExist = false ;
   if ( SYS_LINUX == SYS_TYPE )
   {
      var taskDir  = OMA_PATH_OMA_WORK_TASK_DIR + '/' + taskID ;
      isExist = File.exist( taskDir ) ;
   }
   
   return isExist ;
}

/******************************************************************************
@discretion: create the psql's resultfile
@author: YouBin Lin
@parameter
   taskID:    task's id
@return
   retStr[string]: the path of psql's result file path
***************************************************************************** */
function createPsqlResultFilePath( taskID )
{
   var retStr   = "" ;
   var totalCmd = "" ;
   try
   {
      var cmd = new Cmd() ;
      if ( SYS_LINUX == SYS_TYPE )
      {
         retStr = OMA_PATH_OMA_WORK_TASK_DIR + '/' + taskID + '/' + OMA_FILE_PSQL_FIFO_FILE ;
         totalCmd = "mkfifo " + retStr ;
         cmd.run( totalCmd ) ;
      }
      else
      {
         // TODO:
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to run cmd[" + totalCmd + "]" ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
      exception_handle( rc, errMsg ) ;
   }

   return retStr ;
}

/******************************************************************************
@discretion: remove the psql's resultfile
@author: YouBin Lin
@parameter
   taskID:    task's id
@return
***************************************************************************** */
function removePsqlResultFile( taskID )
{
   var resultFile = "" ;
   try
   {
      if ( SYS_LINUX == SYS_TYPE )
      {
         resultFile = OMA_PATH_OMA_WORK_TASK_DIR + '/' + taskID + '/' + OMA_FILE_PSQL_FIFO_FILE ;
         var isFileExist = File.exist( resultFile ) ;
         if ( isFileExist == true )
         {
            var cmd = new Cmd() ;
            cmd.run( "rm " + resultFile ) ;
         }
      }
      else
      {
         // TODO:
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to remove result file[" + resultFile + "]" ;
      PD_LOG2( taskID, arguments, PDERROR, FILE_NAME_FUNC,
               errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
      exception_handle( rc, errMsg ) ;
   }

   return ;
}

/******************************************************************************
@discretion: create the psql's pid file
@author: YouBin Lin
@parameter
   taskID: task's id
@return
   retStr[string]: the path of psql's pid file path
***************************************************************************** */
function createPsqlPidFilePath( taskID )
{
   var retStr   = "" ;
   var totalCmd = "" ;
   try
   {
      var cmd = new Cmd() ;
      if ( SYS_LINUX == SYS_TYPE )
      {
         retStr = OMA_PATH_OMA_WORK_TASK_DIR + '/' + taskID + '/' + OMA_FILE_PSQL_PID_FILE ;
         totalCmd = "touch " + retStr ;
         cmd.run( totalCmd ) ;
      }
      else
      {
         // TODO:
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to run cmd[" + totalCmd + "]" ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
      exception_handle( rc, errMsg ) ;
   }

   return retStr ;
}

/******************************************************************************
@discretion: check this pid is psql or not
@author: YouBin Lin
@parameter
   pid:     pid
@return
   true/false
***************************************************************************** */
function isPsqlProc( pid )
{
   var isPsql = true ;
   var cmd    = new Cmd() ;
   var result = "" ;
   try
   {
      result = cmd.run( "ps -ef | grep psql | grep -v grep | grep " + pid ) ;
   }
   catch( e )
   {
   }
   
   if ( result == "" )
   {
      isPsql = false ;
   }
   
   return isPsql ;
}

/******************************************************************************
@discretion: get pid from the psql's pid file
@author: YouBin Lin
@parameter
   taskID[INT64]: task's id
@return
   pid
***************************************************************************** */
function getPidFromPsqlPidFile( taskID )
{
   var pidFile  = "" ;
   var pid      = "" ;
   try
   {
      var cmd = new Cmd() ;
      if ( SYS_LINUX == SYS_TYPE )
      {
         pidFile  = OMA_PATH_OMA_WORK_TASK_DIR + '/' + taskID + '/' + OMA_FILE_PSQL_PID_FILE ;
         var isPidFileExist = File.exist( pidFile ) ;
         if ( isPidFileExist == true )
         {
            var myFile     = new File( pidFile ) ;
            var isPsql     = false ;
            try
            {
               pid = myFile.read() ;
            }
            catch ( e )
            {
               SYSEXPHANDLE( e ) ;
               var tmpRC = GETLASTERROR() ;
               if ( -9 != tmpRC )
               {
                  exception_handle( tmpRC, GETLASTERRMSG() ) ;
               }
               else
               {
                  pid = null ;
               }
            }
            
            isPsql = isPsqlProc( pid ) ;
            if ( !isPsql )
            {
               pid = null ;
            }  
         }
         else
         {
            pid = null ;
         }
      }
      else
      {
         // TODO:
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to read from pidfile[" + pidFile + "]" ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
      exception_handle( rc, errMsg ) ;
   }

   return pid ;
}

/******************************************************************************
@discretion: remove the psql's pid file
@author: YouBin Lin
@parameter
   taskID[INT64]: task's id
@return
***************************************************************************** */
function removePsqlPidFile( taskID )
{
   var pidFile = "" ;
   try
   {
      if ( SYS_LINUX == SYS_TYPE )
      {
         pidFile = OMA_PATH_OMA_WORK_TASK_DIR + '/' + taskID + '/' + OMA_FILE_PSQL_PID_FILE ;
         var isPidFileExist = File.exist( pidFile ) ;
         if ( isPidFileExist == true )
         {
            File.remove( pidFile ) ;
         }
      }
      else
      {
         // TODO:
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to remove pid file[" + pidFile + "]" ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
      exception_handle( rc, errMsg ) ;
   }

   return ;
}

/******************************************************************************
@discretion: write pid to the psql's pid file
@author: YouBin Lin
@parameter
   pid_file[string]: the path of pid file
                     e.g. /tmp/omagent/task/1/pid.txt
   pid[string]:      pid
@return
***************************************************************************** */
function writeSsqlPidFile( pid_file, pid )
{
   try
   {
      if ( SYS_LINUX == SYS_TYPE )
      {
         pid = pid + "" ;
         var myFile = new File( pid_file ) ;
         myFile.write( pid ) ;
      }
      else
      {
         // TODO:
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to write pid_file[" + pid_file + "]" ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              errMsg + ", rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
      exception_handle( rc, errMsg ) ;
   }

   return ;
}

/* *****************************************************************************
@discretion: get the psql's library path
@author: YouBin Lin
@parameter
   path[string]: the path of current program working
                 e.g. /opt/sequoiadb/bin
@return
   retStr[string]: the path of psql's library file path
***************************************************************************** */
function getPsqlLibPath( path )
{
   var retStr = "" ;
   var str = "" ;
   var pos = -1 ;

   if ( SYS_LINUX == SYS_TYPE )
   {
      retStr = adaptPath( path ) + "lib";
   }
   else
   {
      // TODO:
   }
   return retStr ;
}

/* *****************************************************************************
@discretion: get the psql's file path
@author: YouBin Lin
@parameter
   path[string]: the path of current program working
                 e.g. /opt/sequoiadb/bin
@return
   retStr[string]: the path of psql's file path
***************************************************************************** */
function getPsqlFile( path )
{
   var retStr = "" ;

   if ( SYS_LINUX == SYS_TYPE )
   {
      retStr = adaptPath( path ) + "psql";
   }
   else
   {
      // TODO:
   }
   return retStr ;
}

/* *****************************************************************************
@discretion: get the service of local formal sdbcm
@author: Tanzhaobo
@parameter void
@exception
@return
   retStr[string]: the service of local formal sdbcm
***************************************************************************** */
/* *****************************************************************************
@discretion: get the service of local formal sdbcm
@author: Tanzhaobo
@parameter void
@exception
@return
   retStr[string]: the service of local formal sdbcm
***************************************************************************** */
function getLocalCMSvc()
{
   var retStr      = "" ;
   var option      = null ;
   var filter      = null ;
   var arr         = null ;
   var obj         = null ;
   
   try
   {
      option = eval( '(' + '{role:"cm",mode:"run",showalone:false}' + ')' ) ;
      filter = eval( '(' + '{type:"sdbcm"}' + ')' ) ;
      arr = Sdbtool.listNodes( option, filter ) ;
      if ( arr.size() > 0 )
      {
         obj = eval( '(' + arr.pos() + ')' ) ;
         retStr = obj[SvcName2] ;
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      errMsg = "Failed to get the service of local sdbcm" ;
      rc = GETLASTERROR() ;
      PD_LOG2( task_id, arguments, PDERROR, FILE_NAME_FUNC,
               sprintf( errMsg + ", rc: ?, detail: ?", rc, GETLASTERRMSG() ) ) ;
      exception_handle( SDB_SYS, errMsg ) ; 
   }
   return retStr ;
}

/* *****************************************************************************
@discretion: get a usable port from local host
@author: Tanzhaobo
@parameter void
@return
   retPort[nunber]: return a usable port or OMA_PORT_INVALID
***************************************************************************** */
function getAUsablePortFromLocal()
{
   var retPort = OMA_PORT_INVALID ;
   var flag = false ;   

   if ( SYS_LINUX == SYS_TYPE )
   {
      for ( var port = OMA_PORT_TEMP_AGENT_PORT ;
            port <= OMA_PORT_MAX; port++ )
      {
         flag = isReservedPort( port ) ;
         if ( flag )
         {
            continue ;
         } 
         var ret =  eval( '(' + System.sniffPort( port ) + ')' ) ;
         flag = ret[Usable];
         if ( flag )
         {
            retPort = port ;
            break ;
         }
      }
   }
   else
   {
      // TODO:
   }
   return retPort ;
}

/* *****************************************************************************
@discretion: get a usable port from remote host
@author: Tanzhaobo
@parameter
   ssh[object]: ssh object
@return
   retPort[nunber]: return a usable port or OMA_PORT_INVALID
***************************************************************************** */
function getAUsablePortFromRemote( ssh )
{
   var retPort = OMA_PORT_INVALID ;
   var str = "" ;
   var port = OMA_PORT_TEMP_AGENT_PORT ;
   var flag = false ;

   if ( SYS_LINUX == SYS_TYPE )
   {
      for ( var port = OMA_PORT_TEMP_AGENT_PORT ;
            port <= OMA_PORT_MAX; port++ )
      {
         flag = isReservedPort( port ) ;
         if ( flag )
         {
            continue ;
         }
         str = "netstat -nap | grep " + port + " | grep -v grep" ;
         try
         {
            ssh.exec( str ) ;
         }
         catch ( e )
         {
            SYSEXPHANDLE( e ) ;
            var ret = ssh.getLastRet() ;
            if ( 1 == ret )
            {
               retPort = port ;
            }
            else
            {
               retPort = OMA_PORT_INVALID ;
            }
            break ;
         }
      }
   }
   else
   {
      // TODO:
   }
   return retPort ;
}

/* *****************************************************************************
@discretion: get the right place to change the owner of a directory
@author: Tanzhaobo
@parameter
   ssh[object]: ssh object
@return
   retPort[nunber]: return a dirctory path
***************************************************************************** */
function getThePlaceToChangeOwner( path )
{
   var retStr = path ;
   var pos = -1 ;
   if ( SYS_LINUX == SYS_TYPE )
   {
      var arr = path.split( '/' ) ;
      var num = arr.length ;
      // in case: "/" or "/opt"
      if ( num <= 2 )
      {
         return path ;
      }
      pos = path.lastIndexOf( '/' ) ;
      // in case: "/opt/"
      if ( pos == path.length -1 && num == 3 )
      {
         return path ;
      }
      // otherwise
      var len = arr[num - 1].length ; ;
      // in case: "/opt/.../123/345"
      if ( len )
      {
         pos = path.length - 1 - len ;
         retStr = path.substring( 0, pos ) ;
      }
      // in case: "/opt/.../123/345/"
      else
      {
         len = arr[num - 2].length ;
         pos = path.length - 1 - len - 1 ;
         retStr = path.substring( 0, pos ) ;
      }
   }
   else
   {
     // TODO:
   }

   return retStr ;
}

/* *****************************************************************************
@discretion: change the owner of the directory
@author: Tanzhaobo
@parameter
   ssh[object]: ssh object
   path[string]: the path of the directory
   user[string]: the user to change to
   userGroup[sring]: the user group to change to
@return void
***************************************************************************** */
function changeDirOwner( ssh, path, user, userGroup )
{
   var ret = null ;
   var str = null ;
   var cmd = null ;
   if ( SYS_LINUX == SYS_TYPE )
   {
      cmd = " mkdir -p " + path ;
      try
      {
         ssh.exec( cmd ) ;
      }
      catch ( e )
      {
         SYSEXPHANDLE( e ) ;
         rc = GETLASTERROR() ;
         errMsg = sprintf( "Failed to create path[?] in host[?]", path, ssh.getPeerIP() ) ;
         PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
                 sprintf( errMsg + ", rc: ?, detail: ?", rc, GETLASTERRMSG() ) ) ;
         exception_handle( rc, errMsg ) ;
      }
      path = getThePlaceToChangeOwner( path ) ;
      str = user + ":" + userGroup ;
      cmd = " chown -R " + str + " " + path ;
      try
      {
         ssh.exec( cmd ) ;
      }
      catch ( e )
      {
         SYSEXPHANDLE( e ) ;
         rc = GETLASTERROR() ;
         errMsg = sprintf( "Failed to change path[?]'s owner to [?] in host[?]",
                           path, str, ssh.getPeerIP() ) ;
         PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
                 sprintf( errMsg + ", rc: ?, detail: ?", rc, GETLASTERRMSG() ) ) ;
         exception_handle( rc, errMsg ) ;
      }
   }
   else
   {
      // TODO: windows
   }
}

/* *****************************************************************************
@discretion: change the mode of file or directory
@author: Tanzhaobo
@parameter
   ssh[object]: ssh object
   path[string]: file or directory
   mode[string]: the mode to change to, e.g. "755"
@return void
***************************************************************************** */
function changeFileOrDirMode( ssh, path, mode )
{
   var str = "" ;
   if ( SYS_LINUX == SYS_TYPE )
   {
      try
      {
         str = "chmod -R " + mode + " " + path ;
         ssh.exec( str ) ;
      }
      catch( e )
      {
         SYSEXPHANDLE( e ) ;
         errMsg = "Failed to change file or directory[" + path + "]'s mode to mode[" + mode + "]" ;
         rc = GETLASTERROR() ;
         PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
                 errMsg + "rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
         exception_handle( rc, errMsg ) ;
      }
   }
   else
   {
      // TODO:
   }
}

/* *****************************************************************************
@discretion: check wether catalog is running or not
@author: Tanzhaobo
@parameter
   db[object]: Sdb object
@return 
   [bool]
***************************************************************************** */
function isCatalogRunning( db )
{
   var rg = null ;
   try
   {
      rg = db.getCatalogRG() ;
   }
   catch ( e )
   {
      return false ;
   }
   return true ;
}

/* *****************************************************************************
@discretion: get the port of OM Agent in specified host from local sdbcm config file
@parameter
   hostName[string]: install host name
@return
   retPort[string]: the OM Agent's port in target host, defaul to be 11790
***************************************************************************** */
function getOMASvcFromCfgFile( hostname )
{
   var retPort = null ;
   try
   {
      var localhostname = System.getHostName() ;
      if ( hostname == localhostname )
      {
         retPort = Oma.getAOmaSvcName( hostname ) ;
      }
      else
      {
         var key = hostname + OMA_MISC_CONFIG_PORT ;
         var obj =  eval ( '(' + Oma.getOmaConfigs() + ')' ) ;
         PD_LOG( arguments, PDDEBUG, FILE_NAME_FUNC,
                 sprintf( "obj info is[?]", JSON.stringify(obj) ) ) ;
         retPort = obj[key]
         if ( "undefined" == typeof(retPort) )
            retPort = OMA_PORT_DEFAULT_SDBCM_PORT + "" ;
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      errMsg = sprintf( "Failed to get OM Agent's service of host[?] from local OM Agent's config file", hostname ) ;
      rc = GETLASTERROR() ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              errMsg + "rc: " + rc + ", detail: " + GETLASTERRMSG() ) ;
      exception_handle( rc, errMsg ) ;
   }

   return retPort + "" ;
}

/* *****************************************************************************
@discretion: get local host name or ip address from hosts table
@parameter void
@return
   retArr[Array]: local host name or ip address
***************************************************************************** */
function _getLocalHostNameOrIP( type )
{
   var retArr = [] ;
   var cmd = new Cmd() ;
   try
   {
      if ( "hostname" == type )
      {
         retArr.push( System.getHostName() ) ;
      }
      else if ( "ip" == type )
      {
         var cmd = new Cmd() ;
         var str = "" ;
         var arr = [] ;
         if ( SYS_LINUX == SYS_TYPE )
         {
            str = removeLineBreak( cmd.run( "hostname -i" ) ) ;
         }
         else
         {
            // TODO:
         }
         arr = str.split(" ") ;
         if ( 0 == arr.length )
         {
            exception_handle( SDB_SYS, "No ip address to get" ) ;
         }
         for ( var i = 0; i < arr.length; i++ )
         {
            if ( "127.0.0.1" != arr[i] )
            {
               retArr.push( arr[i] ) ;
               break ;
            }
         }
         if ( 0 == retArr.length )
         {
            retArr.push( arr[0] ) ;
         }
      }
      else if ( "ips" == type )
      {
         var netCardInfo = eval( '(' + System.getNetcardInfo() + ')' ) ;
         var tmpArr = netCardInfo[Netcards] ;
         for( var i = 0; i < tmpArr.length; i++ )
         {
            retArr.push( tmpArr[i][Ip] ) ;
         }
      }
      else
      {
        throw SDB_INVALIDARG ;
      }

      return retArr ;
   }
   catch ( e )
   {
      SYSEXPHANDLE( e ) ;
      errMsg = "Failed to get localhost host name or ip addresses" ;
      exception_handle( e, errMsg ) ;
   }
   return retStr ;
}

/* *****************************************************************************
@discretion: get local host name from hosts table
@parameter void
@return
   [string]: local host name
***************************************************************************** */
function getLocalHostName()
{
   var arr = _getLocalHostNameOrIP( "hostname" ) ;
   return arr[0] ;
}

/* *****************************************************************************
@discretion: get local ips from net card
@parameter void
@return
   [Array]: local ip addresses
***************************************************************************** */
function getLocalIPs()
{
   return _getLocalHostNameOrIP( "ips" ) ;
}

/* *****************************************************************************
@discretion: get local ips from net card
@parameter void
@return
   [string]: local ip address
***************************************************************************** */
function getLocalIP()
{
   var arr = _getLocalHostNameOrIP( "ip" ) ;
   return arr[0] ;
}

/* *****************************************************************************
@discretion: get task id
@author: Tanzhaobo
@parameter
   obj[object]: object, e.g. { "TaskID":1 }
@return
   task_id[number]: return the task id
***************************************************************************** */
function getTaskID( obj )
{
   var task_id = null ;
   try
   {
      task_id = obj[TaskID] ;
      if ( "number" != typeof(task_id) )
         exception_handle( SDB_SYS, "Task id is not a number: " + task_id ) ;
   }
   catch ( e )
   {
      SYSEXPHANDLE( e ) ;
      errMsg = "Failed to get task id" ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              sprintf( errMsg + ", rc: ?, detail: ?", GETLASTERROR(), GETLASTERRMSG() ) ) ;
      exception_handle( SDB_SYS, errMsg ) ;
   }
   return task_id ;
}

/* *****************************************************************************
@discretion: set the name of task log file
@author: Tanzhaobo
@parameter
   task_id[number]:
@return void
***************************************************************************** */
function setTaskLogFileName( task_id )
{
   try
   {
      var task_log_dir = LOG_FILE_PATH + Task ;
      if ( false == File.exist( task_log_dir ) )
         File.mkdir( task_log_dir ) ;
      LOG_FILE_NAME = adaptPath( Task ) + task_id + ".log" ;
   }
   catch ( e )
   {
      SYSEXPHANDLE( e ) ;
      errMsg = sprintf( "Failed to create js log file for task[?]", task_id ) ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              sprintf( errMsg + ", rc: ?, detail: ?", GETLASTERROR(), GETLASTERRMSG() ) ) ;
      exception_handle( SDB_SYS, errMsg ) ;
   }
}

/* *****************************************************************************
@discretion: push some tool programs and js scripts to target host for checking
@author: Tanzhaobo
@parameter
   ssh[object]: Ssh object
@return void
***************************************************************************** */
function pushProgAndSpt( ssh, progs, spts )
{
   var src = "" ;
   var dest = "" ;
   var local_prog_path = "" ;
   var local_spt_path  = ""  ;

   try
   {
      // 1. get tool program's path
      try
      {
         local_prog_path = adaptPath( System.getEWD() ) ;
      }
      catch( e )
      {
         SYSEXPHANDLE( e ) ;
         rc = GETLASTERROR() ;
         errMsg = "Failed to get local tool program's path" ;
         PD_LOG2( task_id, arguments, PDERROR, FILE_NAME_FUNC,
                  sprintf( errMsg + ", rc: ?, detail: ?", rc, GETLASTERRMSG() ) ) ;
         exception_handle( rc, errMsg ) ;
      }
      PD_LOG2( task_id, arguments, PDDEBUG, FILE_NAME_FUNC,
               "Local tool program's path is: " + local_prog_path ) ;
      // 2. get js script file's path
      try
      {
         local_spt_path = getSptPath( local_prog_path ) ;
      }
      catch( e )
      {
         SYSEXPHANDLE( e ) ;
         rc = GETLASTERROR() ;
         errMsg = "Failed to get local js script files' path" ;
         PD_LOG2( task_id, arguments, PDERROR, FILE_NAME_FUNC,
                  sprintf( errMsg + ", rc: ?, detail: ?", rc, GETLASTERRMSG() ) ) ;
         exception_handle( rc, errMsg ) ;
      }
      PD_LOG2( task_id, arguments, PDDEBUG, FILE_NAME_FUNC,
               "Local js script file's path is: " + local_spt_path ) ;
      
      // 3. push program and script
      if ( SYS_LINUX == SYS_TYPE )
      {
         // push programs
         for ( var i = 0; i < progs.length; i++ )
         {
            src = local_prog_path + progs[i] ;
            dest = OMA_PATH_TEMP_BIN_DIR + progs[i] ;
            ssh.push( src, dest ) ;
         }
         
         // push js files
         for ( var i = 0; i < spts.length; i++ )
         {
            src = local_spt_path + spts[i] ;
            dest = OMA_PATH_TEMP_SPT_DIR + spts[i] ;
            ssh.push( src, dest ) ;
         }
      }
      else
      {
         // TODO:
      }
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = "Failed to push programs to host[" + ssh.getPeerIP() + "]" ;
      PD_LOG2( task_id, arguments, PDERROR, FILE_NAME_FUNC,
               sprintf( errMsg + " src[?], dest[?], rc: ?, detail: ?",
               src, dest, rc, GETLASTERRMSG() ) ) ;
      exception_handle( rc, errMsg ) ;
   }
}

/* *****************************************************************************
@discretion: get install info from /etc/default/sequoiadb
@author: Tanzhaobo
@parameter void
@return
   installInfoObj[object]: info object of installed SequoiaDB
***************************************************************************** */
function getInstallInfoObj()
{
   var installInfoObj = null ;
   try
   {
      installInfoObj = eval( '(' + Oma.getOmaInstallInfo() + ')' ) ;
   }
   catch( e )
   {
      SYSEXPHANDLE( e ) ;
      rc = GETLASTERROR() ;
      errMsg = sprintf( "Failed to get SequoiaDB install info in host[?]",
                        System.getHostName() ) ;
      PD_LOG( arguments, PDERROR, FILE_NAME_FUNC,
              sprintf( errMsg + ", rc: ?, detail: ?", rc, GETLASTERRMSG() ) ) ;
      exception_handle( rc, errMsg ) ;
   }
   return installInfoObj ;
}

/* *****************************************************************************
@discretion: get the name of install packet
@author: Tanzhaobo
@parameter
   packet[string]: the full name of the packet,
                   e.g. /tmp/packet/sequoiadb-1.8-linux_x86_64-installer.run
@return
   packetname[string]: the name of the install packet
***************************************************************************** */
function getPacketName( packet )
{
   var s = "" ;
   var i = 1 ;
   var packetname = "" ;
   if ( SYS_LINUX == SYS_TYPE )
   {
      s = "/" ;
      i = packet.lastIndexOf( s ) ;
      if ( -1 != i )
         packetname = packet.substring( i+1 ) ;
      else
         packetname = packet ;
   }
   else
   {
      // TODO:
   }
   return packetname ;
}
