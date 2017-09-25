//部署模式
var _deployModel = null ;

//业务配置
var _businessConfig = null ;

//业务参数
var _businessPara = null ;

//分区组列表
var _nodeList = [] ;

//当前所选择的分区组编号
var _selectNodeNum = -1 ;

//指定节点
function selectNode( nodeId )
{
    //加载节点参数
    function loadNodeConf( config )
    {
        $( '#HostName_np' ).val( config['HostName'] ) ;
        $.each( config, function( key, value ){
            $( '#' + key + '_np' ).val( value ) ;
        } ) ;
    }
    $.each( _businessPara['Config'], function( index, nodeInfo ){
        if( nodeInfo['zooid'] === nodeId )
        {
            loadNodeConf( _businessPara['Config'][index] ) ;
            sdbjs.parts.tabList.unActive( 'nodeTabList', _selectNodeNum ) ;
            sdbjs.parts.tabList.active( 'nodeTabList', index ) ;
            _selectNodeNum = index ;
            sdbjs.parts.panelBox.update( 'nodeInfoBar', htmlEncode( sdbjs.fun.sprintf( _languagePack['modzookeeper']['rightPanel']['title'], nodeId ) ) ) ;
            return false ;
        }
    } ) ;
}

//删除节点
function removeNode( nodeId )
{
    var line = -1 ;
    $.each( _businessPara['Config'], function( index, nodeInfo ){
        if( nodeInfo['zooid'] === nodeId )
        {
            line = index ;
            return false ;
        }
    } ) ;
    if( line < 0 ) return false ;
    sdbjs.parts.tabList.remove( 'nodeTabList', line ) ;
    _businessPara['Config'].splice( line, 1 ) ;
	sdbjs.fun.saveData( 'SdbConfigInfo', JSON.stringify( _businessPara ) ) ;
    if( line == _selectNodeNum )
    {
        if( _businessPara['Config'].length > 0 )
        {
            selectNode( _businessPara['Config'][0]['zooid'] ) ;
        }
        else
        {
            _selectNodeNum = -1 ;
        }
    }
    sdbjs.fun.removeNode( 'tabListTable_' + line ) ;
    return true ;
}

//在左边列表添加一个节点
function addNodeList( index, nodeId )
{
    sdbjs.parts.tabList.add( 'nodeTabList', function( liObj ){
        $( liObj ).css( 'zoom', 1 ) ;
        sdbjs.parts.tableBox.create( liObj, 'tabListTable_' + index ) ;
        sdbjs.parts.tableBox.update( 'tabListTable_' + index, 'compact' ) ;
        sdbjs.parts.tableBox.addBody( 'tabListTable_' + index, [{ 'text': function( tdObj ){
            var newDiv_1 = $( '<div></div>' ).css( { 'padding': '2px 2px 2px 0', 'font-weight': 'bold' } ).text( _languagePack['modzookeeper']['leftPanel']['tab'] + nodeId ) ;
            $( tdObj ).append( newDiv_1 ).css( 'cursor', 'pointer' ) ;
            sdbjs.fun.addClick( tdObj, 'selectNode("' + nodeId + '")' ) ;
        } }, { 'text': function( tdObj ){
            $( tdObj ).append( '<button class="btn btn-lg btn-default" onclick="removeNode(\'' + nodeId + '\')"><img width="10" src="./images/smallicon/blacks/16x16/delete.png"></button>' ) ;
        }, width: 20 } ] ) ;
    } ) ;
}

//打开创建节点的模态框
function openCreateNodeModal()
{
    var role, groupname, line ;
	var selectObj = sdbjs.fun.getNode( 'nodeModelSelect', 'selectBox' ) ;
	//初始化
	$( selectObj['obj'] ).val( '0' ) ;
	//隐藏
	sdbjs.parts.tableBox.updateBody( 'createNodeTable', 2, 0, function( tdObj ){
		$( tdObj ).hide() ;
	} ) ;
	sdbjs.parts.tableBox.updateBody( 'createNodeTable', 2, 1, function( tdObj ){
		$( tdObj ).hide() ;
	} ) ;
	//清空
	//sdbjs.parts.selectBox.empty( 'nodeIdSelect' ) ;
    $( '#createNodeId' ).val( '' ) ;
	sdbjs.parts.selectBox.empty( 'nodeCopySelect' ) ;
	//读取所有主机
	/*$.each( _businessConfig['HostInfo'], function( index, hostInfo ){
		sdbjs.parts.selectBox.add( 'nodeIdSelect', hostInfo['HostName'], hostInfo['HostName'] ) ;
	} ) ;*/
	//读取所有节点
	$.each( _businessPara['Config'], function( index, nodeInfo ){
		sdbjs.parts.selectBox.add( 'nodeCopySelect', nodeInfo['zooid'], nodeInfo['zooid'] ) ;
	} ) ;
	//'确定'
	sdbjs.parts.buttonBox.update( 'createNodeOK', htmlEncode( _languagePack['public']['button']['ok'] ), 'primary', null, 'setCreateNodeModel()' ) ;
	sdbjs.parts.modalBox.show( 'createNode' ) ;
	$( selectObj['obj'] ).get(0).focus() ;
    sdbjs.fun.setCSS( 'createNodeAlert', { 'display': 'none' } ) ;
}

//创建节点
function createNewNode( nodeId, type, copyNodeId )
{
    var newPara = {} ;
    newPara['HostName'] = _businessConfig['HostInfo'][0]['HostName'] ;
    if( type == '0' )
    {
        //默认值
        $.each( _businessPara['Property'], function( index, property ){
            newPara[ property['Name'] ] = property['Default'] ;
        } ) ;
    }
    else if( type == '1' )
    {
        //复制节点
        $.each( _businessPara['Config'], function( index, nodeInfo ){
            if( nodeInfo['zooid'] == copyNodeId )
            {
                newPara['HostName'] = nodeInfo['HostName'] ;
                $.each( nodeInfo, function( key, value ){
                    newPara[ key ] = value ;
                } ) ;
                return false ;
            }
        } ) ;
    }
    newPara['zooid'] = nodeId;
    _businessPara['Config'].push( newPara ) ;
    sdbjs.fun.saveData( 'SdbConfigInfo', JSON.stringify( _businessPara ) ) ;
    addNodeList( _businessPara['Config'].length - 1, nodeId ) ;
    selectNode( nodeId ) ;
}

//设置创建节点模式
function setCreateNodeModel()
{
	var selectObj_1 = sdbjs.fun.getNode( 'nodeModelSelect', 'selectBox' ) ;
	var selectObj_2 = sdbjs.fun.getNode( 'nodeCopySelect', 'selectBox' ) ;
	var model = $( selectObj_1['obj'] ).val() ;
	var copyNodeId = $( selectObj_2['obj'] ).val() ;
	var NodeId = $( '#createNodeId' ).val() ;
    var rc = true ;
    if( !isIntNumber( NodeId ) || NodeId < 0 )
    {
        sdbjs.fun.setCSS( 'createNodeAlert', { 'display': 'block' } ) ;
        //'Error: 节点ID已经存在。'
        sdbjs.parts.alertBox.update( 'createNodeAlert', htmlEncode( _languagePack['error']['web']['modzookeeper'][0] ), 'danger' ) ;
        return;
    }
    $.each( _businessPara['Config'], function( index, nodeInfo ){
        if( NodeId == nodeInfo['zooid'] )
        {
            rc = false ;
            return false ;
        }
    } ) ;
    if( rc == true )
    {
	   sdbjs.parts.modalBox.hide( 'createNode' ) ;
	   createNewNode( NodeId, model, copyNodeId ) ;
    }
    else
    {
        sdbjs.fun.setCSS( 'createNodeAlert', { 'display': 'block' } ) ;
        //'Error: 节点ID已经存在。'
        sdbjs.parts.alertBox.update( 'createNodeAlert', htmlEncode( _languagePack['error']['web']['modzookeeper'][1] ), 'danger' ) ;
    }
}

//保存节点参数
function saveNodePara( paraName )
{
    if( _selectNodeNum < 0 ) return;
    _businessPara['Config'][_selectNodeNum][paraName] = $( '#' + paraName + '_np' ).val() ;
    sdbjs.fun.saveData( 'SdbConfigInfo', JSON.stringify( _businessPara ) ) ;
}

//--------------------- 初始化 -------------------

//返回
function returnPage()
{
	gotoPage( 'index.html' ) ;
}

//下一步
function nextPage()
{
	var tempBusinessPara = {} ;
	tempBusinessPara['ClusterName'] = _businessPara['ClusterName'] ;
	tempBusinessPara['BusinessType'] = _businessPara['BusinessType'] ;
	tempBusinessPara['BusinessName'] = _businessPara['BusinessName'] ;
	tempBusinessPara['DeployMod'] = _businessPara['DeployMod'] ;
	tempBusinessPara['Config'] = _businessPara['Config'] ;
	
	sdbjs.parts.loadingBox.show( 'loading' ) ;
	restAddBusiness( true, function( jsonArr, textStatus, jqXHR ){
		var taskID = jsonArr[0]['TaskID'] ;
		sdbjs.fun.saveData( 'SdbTaskID', taskID ) ;
		gotoPage( 'installzookeeper.html' ) ;
	}, function( json ){
		sdbjs.parts.loadingBox.hide( 'loading' ) ;
		showProcessError( json['detail'] ) ;
	}, null, tempBusinessPara ) ;
	return ;
}

//加载业务配置
function loadBusinessConf()
{
	var rc = true ;
	if( _businessPara === null )
	{
		sdbjs.parts.loadingBox.show( 'loading' ) ;
		restGetBusinessConfig( false, function( jsonArr, textStatus, jqXHR ){
			_businessPara = jsonArr[0] ;
		}, function( json ){
			rc = false ;
			showProcessError( json['detail'] ) ;
		}, function(){
			sdbjs.parts.loadingBox.hide( 'loading' ) ;
		}, _businessConfig ) ;
	}
	return rc ;
}

//加载节点
function loadNodeList()
{
	sdbjs.parts.loadingBox.show( 'loading' ) ;
	var tempGroupList = {} ;
	//遍历所有节点
	$.each( _businessPara['Config'], function( index, nodeInfo ){
		//创建节点
        addNodeList( index, nodeInfo['zooid'] ) ;
        if( index == 0 )
        {
            selectNode( nodeInfo['zooid'] ) ;
        }
	} ) ;
	sdbjs.parts.loadingBox.hide( 'loading' ) ;
}

//加载配置参数列表
function loadNodePara()
{
	//遍历所有配置项
	$.each( _businessPara['Property'], function( index, property ){
		var newInputObj = createHtmlInput( property['Display'], property['Valid'], '', property['Edit'] ) ;
		if( newInputObj !== null )
		{
			$( newInputObj ).attr( 'id', property['Name'] + '_np' ).on( 'input propertychange', function(){
                (function(id){
                    saveNodePara( id ) ;
                }(property['Name']));
            } ) ;
			sdbjs.parts.gridBox.addBody( 'modNodeConfGrid', [{ 'text': htmlEncode( property['WebName'] ), 'width': '20%' },
																 { 'text': function( tdObj ){
																	 $( tdObj ).append( newInputObj ) ;
																 }, 'width': '40%' },
																 { 'text': htmlEncode( property['Desc'] ), 'width': '40%' } ]  ) ;
		}
        if( index == 0 )
        {
            var hostListStr = '' ;
            $.each( _businessConfig['HostInfo'], function( index2, tmpHostName ){
                if( index2 > 0 ) hostListStr += ',' ;
                hostListStr += tmpHostName['HostName']
            } ) ;
            newInputObj = createHtmlInput( 'select box', hostListStr, '', 'true' ) ;
            $( newInputObj ).attr( 'id', 'HostName_np' ).on( 'input propertychange', function(){
                (function(id){
                    saveNodePara( id ) ;
                }('HostName'));
            } ) ;
            sdbjs.parts.gridBox.addBody( 'modNodeConfGrid', [{ 'text': htmlEncode( 'HostName' ), 'width': '20%' },
                                                             { 'text': function( tdObj ){
                                                                 $( tdObj ).append( newInputObj ) ;
                                                             }, 'width': '40%' },
                                                             { 'text': '', 'width': '40%' } ]  ) ;
        }
	} ) ;
}

function createDynamicHtml()
{
	sdbjs.parts.loadingBox.show( 'loading' ) ;
	if( loadBusinessConf() )
	{
        loadNodePara() ;
		loadNodeList() ;
	}
	sdbjs.parts.loadingBox.hide( 'loading' ) ;
}

function createHtml()
{
	createPublicHtml() ;

	/* 分页 */
	sdbjs.parts.tabPageBox.create( 'top2', 'tab' ) ;
	sdbjs.fun.setCSS( 'tab', { 'padding-top': 5 } ) ;
	if( _deployModel === 'Deploy' )
	{
		//'扫描主机'
		sdbjs.parts.tabPageBox.add( 'tab', '<img width="14" src="./images/smallicon/blacks/16x16/zoom.png"> ' + htmlEncode( _languagePack['public']['tabPage'][2] ), false, null ) ;
		//'添加主机'
		sdbjs.parts.tabPageBox.add( 'tab', '<img width="14" src="./images/smallicon/blacks/16x16/layers_1.png"> ' + htmlEncode( _languagePack['public']['tabPage'][3] ), false, null );
		//'安装主机'
		sdbjs.parts.tabPageBox.add( 'tab', '<img width="14" src="./images/smallicon/blacks/16x16/cog.png"> ' + htmlEncode( _languagePack['public']['tabPage'][4] ), false, null );
	}
	//'修改业务'
	sdbjs.parts.tabPageBox.add( 'tab', '<img width="14" src="./images/smallicon/blacks/16x16/doc_lines_stright.png"> ' + htmlEncode( _languagePack['public']['tabPage'][6] ), true, null );
	//'安装业务'
	sdbjs.parts.tabPageBox.add( 'tab', '<img width="14" src="./images/smallicon/blacks/16x16/cog.png"> ' + htmlEncode( _languagePack['public']['tabPage'][7] ), false, null );
	
	/* 左边框架 */
	sdbjs.parts.divBox.create( 'middle', 'middle-left', 460, 'variable' ) ;
	sdbjs.fun.setCSS( 'middle-left', { 'float': 'left', 'padding': '10px' } ) ;
	
	/* 左边 状态栏 */
	sdbjs.parts.panelBox.create( 'middle-left', 'groupListBar', 'auto', 'variable' ) ;
	sdbjs.parts.panelBox.update( 'groupListBar', function( panelTitle ){
        sdbjs.parts.divBox.create( panelTitle['name'], 'addZookeeperNode' ) ;
        sdbjs.fun.setClass( 'addZookeeperNode', 'pull-right' ) ;
        sdbjs.parts.divBox.update( 'addZookeeperNode', function( divObj ){
            $( divObj ).append( '<button class="btn btn-lg btn-default" onclick="openCreateNodeModal()">' + _languagePack['modzookeeper']['leftPanel']['button'] + '</button>' ) ;
        } ) ;

		sdbjs.parts.divBox.create( panelTitle['name'], 'businessNameDiv' ) ;
		sdbjs.fun.setCSS( 'businessNameDiv', { 'width': 315, 'text-overflow': 'ellipsis', 'overflow': 'hidden', 'white-space': 'nowrap' } ) ;
		sdbjs.parts.divBox.update( 'businessNameDiv', function( divObj ){
			//'业务：'
			$( divObj ).text( _languagePack['modzookeeper']['leftPanel']['title'] + _businessConfig['BusinessName'] ) ;
		} ) ;
		$( panelTitle['obj'] ).append( '<div class="clear-float" style="display:block;"></div>' ) ;
	}, function( panelBody ){
		
		sdbjs.parts.divBox.create( panelBody['name'], 'groupBottomDiv', 'auto', 'variable' ) ;
		sdbjs.parts.tabList.create( 'groupBottomDiv', 'nodeTabList', 'auto', 'variable' ) ;
	} ) ;

	/* 右边框架 */
	sdbjs.parts.divBox.create( 'middle', 'middle-right', 'variable', 'variable' ) ;
	sdbjs.fun.setCSS( 'middle-right', { 'float': 'left', 'padding': '10px', 'padding-left': 0 } ) ;
	
	/* 右边 主机列表 */
	sdbjs.parts.panelBox.create( 'middle-right', 'nodeInfoBar', 'auto', 'variable' ) ;
	sdbjs.fun.setCSS( 'nodeInfoBar', { 'overflow': 'auto', 'position': 'relative' } ) ;
	//'节点列表'
	sdbjs.parts.panelBox.update( 'nodeInfoBar', '', function( panelBody ){
		sdbjs.parts.gridBox.create( panelBody['name'], 'modNodeConfGrid', 'auto', 'variable' ) ;
        //'属性' '值' '说明'
        sdbjs.parts.gridBox.addTitle( 'modNodeConfGrid', [{ 'text': htmlEncode( _languagePack['modzookeeper']['modNodeConfModal']['grid'][0] ), 'width': '20%' },
                                                                          { 'text': htmlEncode( _languagePack['modzookeeper']['modNodeConfModal']['grid'][1] ), 'width': '40%' },
                                                                          { 'text': htmlEncode( _languagePack['modzookeeper']['modNodeConfModal']['grid'][2] ), 'width': '40%' } ]  ) ;
	} ) ;
	
	/* ** */
	sdbjs.parts.divBox.create( 'middle', 'middle-clear', 0, 0 ) ;
	sdbjs.fun.setClass( 'middle-clear', 'clear-float' ) ;

	//返回 下一步
	sdbjs.parts.buttonBox.create( 'operate', 'deployReturn' ) ;
	sdbjs.parts.buttonBox.update( 'deployReturn', function( buttonObj ){
		//'返回'
		$( buttonObj ).text( _languagePack['public']['button']['return'] ) ;
		sdbjs.fun.addClick( buttonObj, 'returnPage()' ) ;
	}, 'primary' ) ;
	var operateNode = sdbjs.fun.getNode( 'operate', 'divBox' ) ;
	$( operateNode['obj'] ).append( '&nbsp;' ) ;
	sdbjs.parts.buttonBox.create( 'operate', 'deployNext' ) ;
	sdbjs.parts.buttonBox.update( 'deployNext', function( buttonObj ){
		//'下一步'
		$( buttonObj ).text( _languagePack['public']['button']['next'] ) ;
		sdbjs.fun.addClick( buttonObj, 'nextPage()' ) ;
	}, 'primary' ) ;
	
	//创建节点的弹窗
	sdbjs.parts.modalBox.create( $( document.body ), 'createNode' ) ;
	//'添加节点'
	sdbjs.parts.modalBox.update( 'createNode', htmlEncode( _languagePack['modzookeeper']['createNodeModal']['title'] ), function( bodyObj ){
		sdbjs.parts.tableBox.create( bodyObj, 'createNodeTable' ) ;
		sdbjs.parts.tableBox.update( 'createNodeTable', 'loosen' ) ;
		//'创建节点模式：'
		//'默认配置创建' '无配置创建' '复制节点配置创建'
		sdbjs.parts.tableBox.addBody( 'createNodeTable', [{ 'text': htmlEncode( _languagePack['modzookeeper']['createNodeModal']['table'][0] ), 'width': 180 },
																		  { 'text': function( tdObj ){
																			  sdbjs.parts.selectBox.create( tdObj, 'nodeModelSelect' ) ;
																			  sdbjs.parts.selectBox.add( 'nodeModelSelect', _languagePack['modzookeeper']['createNodeModal']['select'][0], '0' ) ;
																			  sdbjs.parts.selectBox.add( 'nodeModelSelect', _languagePack['modzookeeper']['createNodeModal']['select'][1], '1' ) ;
																		  } } ] ) ;
		//'节点的主机名：'
		sdbjs.parts.tableBox.addBody( 'createNodeTable', [{ 'text': htmlEncode( _languagePack['modzookeeper']['createNodeModal']['table'][1] ), 'width': 100 },
																		  { 'text': function( tdObj ){
																			  //sdbjs.parts.selectBox.create( tdObj, 'nodeIdSelect' ) ;
                                                                              $( tdObj ).append( '<input class="form-control" type="text" id="createNodeId">' ) ;
																		  } } ] ) ;
		//'复制的节点：'
		sdbjs.parts.tableBox.addBody( 'createNodeTable', [{ 'text': function( tdObj ){
																			  $( tdObj ).text( _languagePack['modzookeeper']['createNodeModal']['table'][2] ).hide() ;
																		  }, 'width': 100 },
																		  { 'text': function( tdObj ){
																			  sdbjs.parts.selectBox.create( tdObj, 'nodeCopySelect' ) ;
																			  $( tdObj ).hide() ;
																		  } } ] ) ;
		var selectObj = sdbjs.fun.getNode( 'nodeModelSelect', 'selectBox' ) ;
		$( selectObj['obj'] ).change( function(){
			if( $( this ).val() === '1' )
			{
				sdbjs.parts.tableBox.updateBody( 'createNodeTable', 2, 0, function( tdObj ){
					$( tdObj ).show() ;
				} ) ;
				sdbjs.parts.tableBox.updateBody( 'createNodeTable', 2, 1, function( tdObj ){
					$( tdObj ).show() ;
				} ) ;
			}
			else
			{
				sdbjs.parts.tableBox.updateBody( 'createNodeTable', 2, 0, function( tdObj ){
					$( tdObj ).hide() ;
				} ) ;
				sdbjs.parts.tableBox.updateBody( 'createNodeTable', 2, 1, function( tdObj ){
					$( tdObj ).hide() ;
				} ) ;
			}
		} ) ;
	}, function( footObj ){
        sdbjs.parts.tableBox.create( footObj, 'createNodeFootTable' ) ;
		sdbjs.parts.tableBox.addBody( 'createNodeFootTable', [{ 'text': function( tdObj ){
                                                                    sdbjs.parts.alertBox.create( tdObj, 'createNodeAlert' ) ;
                                                                    sdbjs.fun.setCSS( 'createNodeAlert', { 'display': 'none', 'padding': '8px', 'text-align': 'left' } ) ;
                                                                } },
                                                                { 'text': function( tdObj ){
                                                                    $( tdObj ).css( 'text-align', 'right' ) ;
                                                                    sdbjs.parts.buttonBox.create( tdObj, 'createNodeOK' ) ;
                                                                    $( tdObj ).append( '&nbsp;' ) ;
                                                                    sdbjs.parts.buttonBox.create( tdObj, 'createNodeClose' ) ;
                                                                    //'确定'
                                                                    sdbjs.parts.buttonBox.update( 'createNodeOK', htmlEncode( _languagePack['public']['button']['ok'] ), 'primary', null, '' ) ;
                                                                    //'关闭'
                                                                    sdbjs.parts.buttonBox.update( 'createNodeClose', function( buttonObj ){
                                                                        $( buttonObj ).text( _languagePack['public']['button']['close'] ).attr( 'data-toggle', 'modalBox' ).attr( 'data-target', 'createNode' ) ;
                                                                    }, 'primary' ) ;
                                                                }, 'width': 120  } ] ) ;
	} ) ;	
}

function checkReady()
{
	var rc = true ;
	_businessConfig = sdbjs.fun.getData( 'SdbBusinessConfig' ) ;
	if( _businessConfig === null )
	{
		rc = false ;
		gotoPage( 'index.html' ) ;
	}
	else
	{
		_businessConfig = JSON.parse( _businessConfig ) ;
        if( _businessConfig['HostInfo'].length <= 0 )
        {
            rc = false ;
            showProcessError( _languagePack['error']['web']['module'][0] ) ;
        }
	}
	if( _businessConfig['DeployMod'] !== 'distribution' )
	{
		rc = false ;
		gotoPage( 'index.html' ) ;
	}
	_businessPara = sdbjs.fun.getData( 'SdbConfigInfo' ) ;
	if( _businessPara !== null )
	{
		_businessPara = JSON.parse( _businessPara ) ;
	}
	_deployModel = sdbjs.fun.getData( 'SdbDeployModel' ) ;
	if( _deployModel === null || ( _deployModel !== 'AddBusiness' && _deployModel !== 'Deploy' ) )
	{
		rc = false ;
		gotoPage( 'index.html' ) ;
	}
	return rc ;
}

$(document).ready(function(){
	if( checkReady() === true )
	{
		//sdbjs.fun.saveData( 'SdbStep', 'modzookeeper' ) ;
		createHtml() ;
		createDynamicHtml() ;
	}
} ) ;

$(window).resize(function(){
	redrawNodePataModal() ;
} ) ;