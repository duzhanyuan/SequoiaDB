(function(){
   window.SdbSacManagerConf.recordEchart = {
       tooltip: {
           trigger: 'axis',
           formatter: '{a0}: {c0}'
       },
       animation: false,
       addDataAnimation: false,
       legend: {
           borderWidth: 1,
           padding: [ 5, 10, 5, 10 ],
           borderColor: '#727272',
           data: [ 'Record insert' ],
           textStyle: {
              color: 'auto'
           },
           y: 'bottom'
       },
       color: [ 'rgba(39,169,227,1)' ],
       calculable: true,
       xAxis : [
           {
               type : 'category',
               boundaryGap : false,
               data : [ '0', '5', '10', '15', '20', '25', '30' ]
           }
       ],
       yAxis : [
           {
               type : 'value'
           }
       ],
       grid: {
          x: 55,
          y: 20,
          x2: 20,
          y2: 60
       },
       series : [
           {
               name: 'Record insert',
               type: 'line',
               data: [ 0, 0, 0, 0, 0, 0, 0 ]
           }
       ]
   } ;          
   window.SdbSacManagerConf.DiskStorageEchart = {
       title: {
           show:true,
           name: 'disk used'
       },
       tooltip: {
           enterable: true ,
           trigger: 'axis',
           //显示百分比和占用大小
           formatter: '{a0}: {c0}%'
       },
       animation: false,
       addDataAnimation: false,
       color: [ 'rgba(39,169,227,1)' ],
       calculable: true,
       xAxis : [
           {
               type : 'category',
               boundaryGap : false,
               data : [ '0', '5', '10', '15', '20', '25', '30' ],
               splitLine : {
                   lineStyle : {
                       color: ['#fff']
                   }
               }
           }
       ],
       yAxis : [
           {
               type : 'value',
               max : 100,
               axisLabel : {
                   formatter: '{value}%'
               }
           }
       ],
       grid: {
          x: 55,
          y: 20,
          x2: 20,
          y2: 60,
          borderColor:'#000'
       },
       series : [
           {   
               itemStyle: { normal: { areaStyle: {type: 'mint'} } },
               smooth: true,
               name: 'Used',
               type: 'line',
               data: [ 40, 50, 60, 70, 80, 90, 98 ]
           }
       ]
   } ;  
   
   window.SdbSacManagerConf.DiskReadEchart = {
       title: {
           show: true,
           name: 'disk used'
       },
       tooltip: {
           enterable: true ,
           trigger: 'axis',
           //显示百分比和占用大小
           formatter: '{a0}: {c0}%'
       },
       animation: false,
       addDataAnimation: false,
       color: [ 'rgba(39,169,227,1)' ],
       calculable: true,
       xAxis : [
           {
               type : 'category',
               boundaryGap : false,
               data : [ '0', '5', '10', '15', '20', '25', '30' ],
               splitLine : {
                   lineStyle : {
                       color: ['#fff']
                   }
               }
           }
       ],
       yAxis : [
           {
               type : 'value',
               max : 50,
               axisLabel : {
                   formatter: '{value}Mb/s'
               }
           }
       ],
       grid: {
          x: 55,
          y: 20,
          x2: 20,
          y2: 60,
          borderColor:'#000'
       },
       series : [
           {   
               symbol: 'none',
               smooth:true,
               itemStyle: {normal: {areaStyle: {type: 'default'}}},
               name: 'Used',
               type: 'line',
               data: [ 15, 17, 15, 14, 16, 15, 15 ]
           }
       ]
   } ;
   window.SdbSacManagerConf.DiskWriteEchart = {
       title: {
           show:true,
           name: 'disk used'
       },
       tooltip: {
           enterable: true ,
           trigger: 'axis',
           //显示百分比和占用大小
           formatter: '{a0}: {c0}%,{a1}: {c1}%'
       },
       animation: false,
       addDataAnimation: false,
       color: [ 'rgba(39,169,227,1)' ],
       calculable: true,
       xAxis : [
           {
               type : 'category',
               boundaryGap : false,
               data : [ '0', '5', '10', '15', '20', '25', '30' ],
               splitLine : {
                   lineStyle : {
                       color: ['#fff']
                   }
               }
           }
       ],
       yAxis : [
           {
               type : 'value',
               max : 50,
               axisLabel : {
                   formatter: '{value}'
               }
           }
       ],
       grid: {
          x: 55,
          y: 20,
          x2: 20,
          y2: 60,
          borderColor:'#000'
       },
       series : [
           {   
               smooth:true,
               name: 'Write',
               stack: 'Write',
               itemStyle: { normal: { areaStyle: { type: 'default' } } },
               type: 'line',
               data: [ 21, 19, 18, 20, 22, 20, 17 ]
           },
           {   
               smooth:true,
               name: 'Read',
               stack: 'Read',
               itemStyle: { normal: { areaStyle: { color:'#AED4C2', type: 'default' } } },
               type: 'line',
               data: [ 28, 15, 13, 22, 20, 20, 17 ]
           }
       ]
   } ;
   window.SdbSacManagerConf.CpuEchart = {
      title: {
         show:true,
         name:'CPU used'
      },
      tooltip: {
         enterable: true ,
         trigger: 'axis',
         //显示百分比和占用大小
         formatter: '{a}: {c}%'
      },
      grid: {
         x: 55,
         x2:20,
         y: 20,
         y2: 60,
         borderColor:'#000'
      },
      animation: false,
      addDataAnimation: false,
      color: [ 'rgba(39,169,227,1)' ],
      calculable: true,
      xAxis : [
         {
            type : 'category',
            boundaryGap : false,
               splitLine : {
                   lineStyle : {
                       color: ['#fff']
                   }
               },
            data : [ '0', '5', '10', '15', '20', '25', '30' ]
         }
      ],
      yAxis : [
         {
            type : 'value',
            max : 100,
            data : [ '0', '20', '40', '60', '80', '100' ],
            axisLabel : {
                   formatter: '{value}%'
               }
         }
      ],
      series : [
         {
            type:'line',
            smooth:true,
            itemStyle: {normal: {areaStyle: {type: 'default'}}},
            data:[10, 12, 21, 54, 60, 58, 55]
         }
      ]
   } ;
   window.SdbSacManagerConf.MemoryLessEchart = {
      tooltip : {
           trigger: 'axis',
           axisPointer : {            // 坐标轴指示器，坐标轴触发有效
               type : 'shadow'        // 默认为直线，可选为：'line' | 'shadow'
           }
       },
       
       toolbox: {
           show : false
       },
       grid : {
          x:30,
          y:10,
          y2:0,
          x2:0,
          borderColor : '#fff'
       },
       calculable : true,
       xAxis : [
           {
              show : false,
              type : 'value'
           }
       ],
       yAxis : [
           {
              show :　false,
               type : 'category',
               data : ['1']
           }
       ],
       series : [
           {
             
           },
           {
               name:'系统',
               type:'bar',
               stack: '总量',
               itemStyle: {
                  normal: {
                     label: {
                        show: true,
                        position: 'insideRight',
                        formatter:'{c0}%', 
                        textStyle:{
                           color:'#FFF',
                           fontSize:16
                        }
                     },
                     color: '#ddd'
                  }
               },
               data:[20]
           },
           {
               name:'用户',
               type:'bar',
               stack: '总量',
               itemStyle: {
                  normal: {
                     label: {
                        show: true,
                        position: 'insideRight',
                        formatter:'{c0}%', 
                        textStyle:{
                           color:'#FFF',
                           fontSize:16
                        }
                     },
                     color: '#21bbee'
                  }
               },
               data:[30]
           },
           {
               name:'缓存',
               type:'bar',
               stack: '总量',
               itemStyle: {
                  normal: { 
                     label: {
                        show: true,
                        position: 'insideRight',
                        formatter:'{c0}%', 
                        textStyle:
                           {
                              color:'#FFF',
                              fontSize:16
                           } 
                        },
                      color: '#FF7F50'
                     }
                  },
               data:[8]
           },
           {
               name:'空闲',
               type:'bar',
               stack: '总量',
               itemStyle: {
                  normal: {
                     label: {
                        show: true,
                        position: 'insideRight',
                        formatter:'{c0}%', 
                        textStyle:{
                           color:'#FFF',
                           fontSize:16
                        }
                     },
                     color: '#18bc9a'
                  }
               },
               data:[42]
           }
       ]
   }
   window.SdbSacManagerConf.RamBarEchart = {
      tooltip : {
           trigger: 'axis',
           axisPointer : {            // 坐标轴指示器，坐标轴触发有效
               type : 'shadow'        // 默认为直线，可选为：'line' | 'shadow'
           },
         formatter: '{c0}Mb'
       },
       toolbox: {
           show : false
       },
       grid : {
          x:0,
          y:0,
          y2:0,
          x2:0,
          borderColor : '#fff'
       },
       calculable : true,
       xAxis : [
           {
              show : false,
              type : 'value'
           }
       ],
       yAxis : [
           {
              show :　false,
               type : 'category',
               data : ['1']
           }
       ],
       series : [
           {
             
           },
           {
               name:'已使用',
               type:'bar',
               stack: '总量',
               itemStyle: {
                  normal: {label : {show: true, position: 'insideRight',formatter:'{c0}'},color: '#21BBEE' }
               },
               data:[2516]
           },
           {
               name:'空闲',
               type:'bar',
               stack: '总量',
               itemStyle: {
                  normal: { color: '#DDD' }
                  },
               data:[4096]
           }
       ]
   }

   window.SdbSacManagerConf.MemoryEchart = {
       title: {
           show:true,
           name: 'Ram used'
       },
       tooltip: {
           enterable: true ,
           trigger: 'axis',
           //显示百分比和占用大小
           formatter: '{a0}: {c0}%'
       },
       animation: false,
       addDataAnimation: false,
       color: [ 'rgba(39,169,227,1)' ],
       calculable: true,
       xAxis : [
           {
               type : 'category',
               boundaryGap : false,
               data : [ '0', '5', '10', '15', '20', '25', '30' ],
               splitLine : {
                   lineStyle : {
                       color: ['#fff']
                   }
               }
           }
       ],
       yAxis : [
           {
               type : 'value',
               max : 100,
               axisLabel : {
                   formatter: '{value}%'
               }
           }
       ],
       grid: {
          x: 55,
          y: 20,
          x2: 20,
          y2: 60,
          borderColor:'#000'
       },
       series : [
           {   
               itemStyle: { normal: { areaStyle: {type: 'mint'} } },
               smooth: true,
               name: 'Used',
               type: 'line',
               data: [ 40, 50, 60, 65, 54, 50, 53 ]
           }
       ]
   } ;  
   
   window.SdbSacManagerConf.NetwordEchart = {
       title: {
           show:true,
           name: 'disk used'
       },
       tooltip: {
           enterable: true ,
           trigger: 'axis',
           //显示百分比和占用大小
           formatter: '{a0}: {c0}Kb'
       },
       animation: false,
       addDataAnimation: false,
       color: [ 'rgba(39,169,227,1)' ],
       calculable: true,
       xAxis : [
           {
               type : 'category',
               boundaryGap : false,
               data : [ '0', '5', '10', '15', '20', '25', '30' ],
               splitLine : {
                   lineStyle : {
                       color: ['#fff']
                   }
               }
           }
       ],
       yAxis : [
           {
               type : 'value',
               axisLabel : {
                   formatter: '{value}kb'
               }
           }
       ],
       grid: {
          x: 55,
          y: 20,
          x2: 20,
          y2: 60,
          borderColor:'#000'
       },
       series : [
           {   
               smooth:true,
               name: 'Write',
               stack: 'Write',
               itemStyle: { normal: { areaStyle: { type: 'default' } } },
               type: 'line',
               data: [ 21, 19, 18, 20, 22, 20, 17 ]
           }
       ]
   } ;
}());