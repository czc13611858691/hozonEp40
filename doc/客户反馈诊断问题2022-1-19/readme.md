![微信图片_20220119090819](D:\02_Projects\HOZON_EP40\Software\hozonEp40\doc\客户反馈诊断问题2022-1-19\readme.assets\微信图片_20220119090819.png)![image-20220119091100024](D:\02_Projects\HOZON_EP40\Software\hozonEp40\doc\客户反馈诊断问题2022-1-19\readme.assets\image-20220119091100024.png)



NAX 0x50

主节点诊断



**未请求的服务，也会自动响应其他服务**

![image-20220119093631443](D:\02_Projects\HOZON_EP40\Software\hozonEp40\doc\客户反馈诊断问题2022-1-19\readme.assets\image-20220119093631443.png)



部分服务中少加了break



F19D	2

F1BF	2

F1C0	2 	BREAK

F1D0	3

default  BREAK



**流控制帧响应格式不对**

![image-20220119093858371](D:\02_Projects\HOZON_EP40\Software\hozonEp40\doc\客户反馈诊断问题2022-1-19\readme.assets\image-20220119093858371.png)

![image-20220119094246104](D:\02_Projects\HOZON_EP40\Software\hozonEp40\doc\客户反馈诊断问题2022-1-19\readme.assets\image-20220119094246104.png)

![image-20220119094423989](D:\02_Projects\HOZON_EP40\Software\hozonEp40\doc\客户反馈诊断问题2022-1-19\readme.assets\image-20220119094423989.png)



0x3C:	50	03	22	F1	9D	00	00	00

0x3D:	50	10	06	62	F1	9D	20	21

0x3D:	50	21	12	26	00	00	00	00



62	F1	9D	20	21	12