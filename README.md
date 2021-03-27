# [PFSafetyGuard](https://github.com/AgentGuo/PFSafetyGuard):sunflower:

## 1. Get start

### 1.1 项目介绍

项目主要有四个部分：

* [testCode](https://github.com/AgentGuo/PFSafetyGuard/tree/main/testCode)：测试程序，主要包含一些我们需要抓取的API和异常操作
* [PFDLL](https://github.com/AgentGuo/PFSafetyGuard/tree/main/PFDLL)：定义了需要HOOK的winAPI和替换的函数
* [syringe](https://github.com/AgentGuo/PFSafetyGuard/tree/main/syringe)：注射器程序，主要将[PFDLL](https://github.com/AgentGuo/PFSafetyGuard/tree/main/PFDLL)程序中的替换函数替换测试程序[testCode](https://github.com/AgentGuo/PFSafetyGuard/tree/main/testCode)中的API
* [PFSafetyGuard](https://github.com/AgentGuo/PFSafetyGuard/tree/main/PFSafetyGuard)：图形界面程序，接收[PFDLL](https://github.com/AgentGuo/PFSafetyGuard/tree/main/PFDLL)程序勾取的信息，然后做一些行为异常分析

### 1.2 run it

项目运行环境：

* detours库：[配置过程](https://blog.csdn.net/weixin_44338712/article/details/115261358)
* QT 5.9

然后需要修改程序中硬编码的一些路径（懒得改了:laughing:）

* [PFSafetyGuard/syringe/syringe/Source.cpp](https://github.com/AgentGuo/PFSafetyGuard/blob/main/syringe/syringe/Source.cpp)：line34 - line39
* [PFSafetyGuard/PFSafetyGuard/myThread.cpp](https://github.com/AgentGuo/PFSafetyGuard/blob/main/PFSafetyGuard/myThread.cpp)：line24

then run it

## 2. 简要展示

![1](./img/1.png)

## 3. 详细说明

### 3.1 整体思路

系统整体设计思路

![2](./img/2.png)

### 3.2 HOOK API

主要完成了以下18个winAPI的HOOK，主要涉及弹窗API、文件打开读写API、堆操作API、注册表操作API、网络通信API（socket）

~~~cpp
DetourAttach(&(PVOID&)OldMessageBoxW, NewMessageBoxW);
DetourAttach(&(PVOID&)OldMessageBoxA, NewMessageBoxA);
DetourAttach(&(PVOID&)OldCreateFile, NewCreateFile);
DetourAttach(&(PVOID&)OldWriteFile, NewWriteFile);
DetourAttach(&(PVOID&)OldReadFile, NewReadFile);
DetourAttach(&(PVOID&)OldHeapCreate, NewHeapCreate);
DetourAttach(&(PVOID&)OldHeapDestory, NewHeapDestory);
DetourAttach(&(PVOID&)OldHeapFree, NewHeapFree);
DetourAttach(&(PVOID&)OldRegCreateKeyEx, NewRegCreateKeyEx);
DetourAttach(&(PVOID&)OldRegSetValueEx, NewRegSetValueEx);
DetourAttach(&(PVOID&)OldRegDeleteValue, NewRegDeleteValue);
DetourAttach(&(PVOID&)OldRegCloseKey, NewRegCloseKey);
DetourAttach(&(PVOID&)OldRegOpenKeyEx, NewRegOpenKeyEx);
DetourAttach(&(PVOID&)Oldsocket, Newsocket);
DetourAttach(&(PVOID&)Oldbind, Newbind);
DetourAttach(&(PVOID&)Oldsend, Newsend);
DetourAttach(&(PVOID&)Oldconnect, Newconnect);
DetourAttach(&(PVOID&)Oldrecv, Newrecv);
~~~

举个栗子，比如成功勾取一次socket通信过程：

![3](./img/3.png)

### 3.3 异常行为分析

主要完成了五种软件行为分析

* 修改可执行文件（.exe .dll .ocx）
* 自我复制
* 对多个文件夹下的文件进行读写
* 堆重复释放
* 修改注册表开机启动项

