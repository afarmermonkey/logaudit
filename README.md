# logaudit
日志审计系统  

# 环境配置
# windows 环境：
#MySQL Version：MySQL8.0.13   
#Visual Stdio 2019          
#doc文件里提供MySQL Workbench的免安装版
# Linux环境：
#Ubuntu18.04       
#MySQL Version:MySQL8.0.13   
#gcc version: 7.05   
#make version: GNU Make 4.1   
# 项目介绍 
本项目是对Mysql的C++封装的跨平台的实际应用，对系统日志进行审计，项目包含agent,center,client三个模块  
分别对应事件采集，事件分析和事件获取，采用分布式部署方法，三个模块相对独立。 
项目中审计规则只针对用户的登录成功和失败进行，感兴趣的可以增加审计规则。项目没有做UI界面，  
自己可以移值到Qt界面上使用。 
agent 可以实时读取Linux的日志文件  
center 对Linux的用户登录情况进行了审计  
client 是跨平台的事件获取模块，可以分析agent和center模块保存的数据库进行操作，  
包括用户登录（有防止sql注入功能，密码隐显的跨平台处理）、分页显示、  
sql的查询测试（包括索引，非索引，like查询）各种查询的时间测试。其他功能可以根据需求自己添加。  
以上功能都可以移值到UI界面上，形成一个成熟的项目
