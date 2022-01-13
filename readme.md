e3样件

无震动无压力



软硬件版本号

![image-20211217090406944](D:\02_Projects\HOZON_EP40\Software\E3\readme.assets\image-20211217090406944.png)



原理图

左前

 [EP40-MAIN_ver2.pdf](doc\EP40-MAIN_ver2.pdf) 

左后

 [EP40-RL.pdf](doc\EP40-RL.pdf) 

右前

 [EP40-FR-RR.pdf](doc\EP40-FR-RR.pdf) 

右后

 [EP40-FR-RR.pdf](doc\EP40-FR-RR.pdf) 



bootloader功能和诊断功能的添加

bootloader部分的代码左后，右前，右后（avr128da28）公用一份代码

此部分原版工程压缩包 [lin_boot_avr128da28.zip](code\boot\lin_boot_avr128da28.zip) 

左前(avr64da64)单独一份代码

此部分原版工程压缩包  [lin_boot_avr64da64.zip](code\boot\lin_boot_avr64da64.zip) 



lin通信矩阵表

前 lin msg id 0x21

后 lin msg id 0x22



左前和左后挂在主机DDCU下

 [EP40_DDCU_LIN1_Matrix_V2.1_Release0910.xlsx](doc\EP40_DDCU_LIN1_Matrix_V2.1_Release0910.xlsx) 

 [EP40_DDCU_LIN1_Matrix_V2.1_Release0910.ldf](doc\EP40_DDCU_LIN1_Matrix_V2.1_Release0910.ldf) 

右前和右后挂在主机PDCU下

 [EP40_PDCU_LIN1_Matrix_V2.1_Release0910.xlsx](doc\EP40_PDCU_LIN1_Matrix_V2.1_Release0910.xlsx) 

 [EP40_PDCU_LIN1_Matrix_V2.1_Release0910.ldf](doc\EP40_PDCU_LIN1_Matrix_V2.1_Release0910.ldf) 



功能描述

电容按键开关

一个电容膜片上有三个按键和一个滑条

两个按键和滑条控制窗的上升和下降功能，

上升和下降分为手动和自动

还有一个按键作为特殊功能按键，没有功能的作为自定义功能按键



lin 睡眠唤醒功能 4s 0x3c



主驾功能按键

rear切换前后窗控制



按键声音提示



背光控制

0x01-夜晚模式-低亮度

0x00-白天模式-高亮度

按键等级控制，只有夜晚模式会收到此变量的控制，默认为Level 3

Rear按键有单独指示灯点亮

玻璃窗锁止有单独指示灯点亮功能



诊断功能



刷写功能



接口定义

![image-20211214141513168](D:\02_Projects\HOZON_EP40\Software\E3\readme.assets\image-20211214141513168.png)



右前生产文件制作演示

canoe工程 [EP40_update_software.cfg](code\canoe\EP40_update_software.cfg) 

boot工程 [lin_stack.atsln](code\boot\avr128da28\lin_stack.atsln) 

app工程 [AVR128DA28_FR_RR.atsln](code\app\RFGLS\AVR128DA28_FR_RR.atsln) 

打开canoe工程调度表执行

打开boot工程配置如下

![image-20211214150941271](D:\02_Projects\HOZON_EP40\Software\E3\readme.assets\image-20211214150941271.png)

![test](D:\02_Projects\HOZON_EP40\Software\E3\readme.assets\image-20211214151053584.png)

执行读取版本命令

![image-20211214151131606](D:\02_Projects\HOZON_EP40\Software\E3\readme.assets\image-20211214151131606.png)

0x3d会返回数据

读取eeprom

写入eeprom第一个字节为0x55

再次执行canoe读取版本指令

无3d返回数据



打开app工程

配置如下

![image-20211214151639608](D:\02_Projects\HOZON_EP40\Software\E3\readme.assets\image-20211214151639608.png)

此时从机返回数据功能正常



**生产文件制作**

读取

![image-20211214151955968](D:\02_Projects\HOZON_EP40\Software\E3\readme.assets\image-20211214151955968.png)

生成

![image-20211214152346051](D:\02_Projects\HOZON_EP40\Software\E3\readme.assets\image-20211214152346051.png)



右后代码除了改变id其他相同，因此仅仅复制右前工程，改变了一下id,重新生成生产文件，其他不改变

左后移植右后的代码，改变touch文件和led文件



注意4个开关的nad是不同的所以boot程序也是不同的

**4个开关通用测试**

- [ ] lin可以正常通信，从机发送帧响应
- [ ] lin通信4s没调度，可以进入睡眠模式，并且进入睡眠模式后可以正常唤醒
- [ ] lin通信接收到0x3C睡眠指示可以进入睡眠模式，并且进入睡眠模式后可以正常唤醒
- [ ] lin通信白天模式，开关背光高亮度
- [ ] lin通信夜晚模式，开关背光低亮度，同时背光等级可以被主节点控制
- [ ] 按键和滑条可以正常发送长按和短按信号

**左前开关测试**

- [ ] 

**左后开关测试**

- [ ] 

**右前开关测试**

- [ ] 

**右后开关测试**

- [ ] 

