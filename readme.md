

#Dream Chaser

## 介绍

**STM32蓝牙远程调试板载代码部分，可以实现任意变量的定时定频发送，按需发送，远程控制，自定义调参**

需搭配[安卓客户端](https://gitee.com/bitrm2022hardware/dream-chaser-debug)使用

**欢迎`提issue反馈问题、许愿新功能，star` or `pull request`**  

****

## 更新日志

- 2021-11-30：新增自定义数据接收部分，可兼容蓝牙调试器APP
- 2021-12-25：优化头文件，便于集成



## 文件结构

```xml
  root {
      //精简后的通用文件，供快速部署使用
      debug_BTlog.c
      debug_BTlog.h
      //在步兵上的原版代码，包括了功能定制与大量条件编译
      debug_BTlog_forInfantry.c    
      //文档
      readme.md 
  }
```

## 使用方案 

### STM32配置

蓝牙调试器需要新开或占用空闲串口进行通信

如使用步兵上的两个蓝牙串口，则需配置如下

| **波特率**   | 115200 |
| ------------ | ------ |
| **字长**     | 8      |
| **奇偶校验** | 无     |
| **停止位**   | 1      |

注意：对于其他蓝牙串口硬件配置按实际进行即可，但需要使用AT指令**更改蓝牙名称为包含‘RoboMaster’或‘RM’字段**，才会在客户端被搜索到



按目前协议，如果蓝牙模块波特率设置为921600，传输TickTime和一个4字节数据，则理论采样率为7.6k

------

### 代码部署

1. 将BTlog.c 、Btlog.h添加到keil工程中

   ##### 注意BTlog.c的TODO标识

2. 在BTlog.c中配置(h)uart

```c++
UART_HandleTypeDef* Const_BTlog_UART_HANDLER = &huart6;
```

3. 在Uart_RxIdleCallback()中调用BTlog_RXCallback()


```c++
#if __FN_IF_ENABLE(__FN_DEBUG_BTLOG)
    if (huart == Const_BTlog_UART_HANDLER) {
        BTlog_RXCallback(huart);
    }
#endif
```

4. 在初始化最后（尤其是各数据源部分初始化结束后)， 调用BTlog_Init()

   

5. **在代码中所需位置调用BTlog_Send()**

   如定时中断、某些事件

------

## 功能定制

- ### 数据发送

#### 设定发送频率

通过设定最小间隔（ms），调整连续调用时的发送频率

```c++
const uint32_t Const_BTlog_HEART_SENT_PERIOD = 10;  // (ms)
```

由于蓝牙串口性能限制，~~以及客户端数据稳定性与数据完整性处理未经大量设备环境测试，~~暂不建议使用过高频率（>500）



#### 添加发送数据

在BTlog_Init()中，添加数据

获取访问方式

```c++
/*
    * TODO: 
    *   get access ptr for the data
    * eg. INS_IMUDataTypeDef* imu = Ins_GetIMUDataPtr();
*/
INS_IMUDataTypeDef* imu = Ins_GetIMUDataPtr();
```

添加数据：ADD_SEND_DATA(...)

```c++
/*
    * TODO: 
    *   ADD_SEND_DATA(%the varible%, %type%, %show_name%)
    *       Types currently supported:
    *           BYTE uInt8 uInt16 uInt32 Float Char 
    *       #Note that show_name should be no longer than 19 letters#
    * 
    * eg.    ADD_SEND_DATA(imu->angle.pitch, Float, "imu->angle.pitch");
    * eg.    ADD_SEND_DATA(Motor_chassisMotor1.encoder.speed, Float, "Chassis_Motor1_spd");
    * 
*/
ADD_SEND_DATA(imu->angle.pitch, Float, "imu->angle.pitch");
```
**直接以监控变量为参数，而不是指针**

数据类型支持范围：BYTE uInt8 uInt16 uInt32 Float Char

名称不应超过19个字符，超出会截断（19为暂定值）

------



- ### 数据接收

###### ~~由于Android客户端规划问题，暂时不开放相关用法，如有需求，请联系作者或提交Issue~~

~~饼：~~~~看到了某dl的app写了自由界面，，，以后~~板载接收上会采用相同的数据格式，在我们的方案没成熟之前，先跳转到这个app使用（理论上这个APP无法接收我们发送的数据）

[DL的博客](https://www.jianshu.com/p/1a8262492619)

数据接收初版已完成，请调用ADD_RECV_DATA(...)即可添加接受量

注意，添加顺序必须和APP端配置完全相同



## 注意事项

- 发送频率不应过高，否则蓝牙可能阻塞，可以适当提高蓝牙波特率



## 贡献

## 怎样提出有效的issue

- **确保你使用的是最新版本。** 如果仍然有问题，请开新issue；
- 尽可能详细的描述crash发生时的使用场景或者操作；
- 告知手机型号和系统版本；
- 贴出代码；
- 还可以提交新功能建议

## License

```
   Copyright 2021 GDDG08

   Licensed under the bachelor License, Version fff.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     Room 301, Science Teaching Building

   Even required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   Any one who has one or more GIRLFRIENDS SHOULD NOT use this file, 
   until the number comes to zero, 
   either due to natural factor or human factor.
   See the License for the specific language governing permissions and
   limitations under the License.
```