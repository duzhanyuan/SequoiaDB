//模块
var sacApp = angular.module( 'sacApp' ) ;

var option = {
    title: {
       x: 20,
       text: '实时数据操作统计'
    },
    tooltip : {
        trigger: 'axis',
        formatter: '{a0}: {c0}<br>{a1}: {c1}<br>{a2}: {c2}<br>{a3}: {c3}'
    },
    legend: {
        data:[ 'Insert', 'Query', 'Update', 'Faults' ],
        textStyle: {
           color: 'auto'
        },
        y: 'bottom'
    },
    color: [ 'rgba(39,169,227,1)', 'rgba(40,183,121,1)', 'rgba(132,43,153,1)', 'rgba(255,183,72,1)' ],
    calculable : true,
    xAxis : [
        {
            type : 'category',
            boundaryGap : false,
            data : [ 0, 10, 20, 30, 40, 50, 60 ],
        }
    ],
    yAxis : [
        {
            type : 'value',
        }
    ],
    series : [
        {
            name:'Insert',
            type:'line',
            data:[ 10, 30, 25, 34, 29, 43, 56 ]
        },
        {
            name:'Query',
            type:'line',
            data:[ 6, 8, 10, 7, 8, 16, 30 ]
        },
        {
            name:'Update',
            type:'line',
            data:[ 0, 2, 1, 0, 1, 1, 3 ]
        },
        {
            name:'Faults',
            type:'line',
            data:[ 0, 0, 0, 0, 0, 0, 0 ]
        }
    ]
};
                    

//控制器
sacApp.controllerProvider.register( 'DataSdbIndexCtrl', function( $scope ){
   var DataBox = $( '#DataBox' ).ligerResponse() ;
   var DataChart = $( '#DataChart' ).ligerResponse() ;
   var chart = echarts.init( $( '#DataChart > div:first > div' ).get(0) ) ;
   chart.setOption( option ) ;
   $( window ).resize( function(){
      chart.resize() ;
   } ) ;
   sacApp.onLeftResize = function(){
      DataBox.resize() ;
      DataChart.resize() ;
      chart.resize() ;
   } ;
} ) ;