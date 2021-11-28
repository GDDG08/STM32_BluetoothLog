

#Dream Chaser

## 介绍

**STM32蓝牙远程调试板载代码部分，可以实现任意变量的定时定频发送，按需发送，远程控制，自定义调参**

需搭配[安卓客户端](https://gitee.com/bitrm2022hardware/dream-chaser-debug)使用

**欢迎`提issue反馈问题、许愿新功能，star` or `pull request`**  

****



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

**波特率：115200**

**无奇偶校验**

**字长8**

**停止位1**

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

```c++
const uint32_t Const_BTlog_HEART_SENT_PERIOD = 10;  // (ms)
```

由于客户端数据稳定性与数据完整性处理未经大量设备环境测试，暂不建议使用过高频率（>500）



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

添加数据

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



- ## 数据接收

###### 由于Android客户端规划问题，暂时不开放相关用法，如有需求，请联系作者或提交Issue



## 注意事项

- 发送频率不应过高



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
   Any one who has a or more GIRLFRIENDS SHOULD NOT use this file, 
   untill the number comes to zero, 
   either due to natural factor or human factor.
   See the License for the specific language governing permissions and
   limitations under the License.
```