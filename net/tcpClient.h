#pragma once
#include"INet.h"
#include "INetMediator.h"
#include"pakdef.h"
#include<map>
class tcpClient:public INet
{
public:
    tcpClient(INetMediator* pMediator);
    ~tcpClient();
    //初始化网络
    bool InitNet();
    //发送数据
    bool SendData(long ISendIp, char* buf, int nLen);
    //关闭网络
    void UninitNet();

protected:
    //接收数据
    void RecvData();
    //接收数据的线程
    static unsigned __stdcall RecvThread(void* IpVoid);


    SOCKET m_sock;   //套接字
    HANDLE m_handle;   //线程句柄
    bool m_isStop;
    sockaddr_in serverAddr;

  /*  map<long, SOCKET> sockAddr_Server_Client;*/
    //FILE* pFile;   //文件指针
    //STRU_FILE_INFO_RQ rq;//当前传输文件的请求信息
    //sockaddr_in serverAddr;//sockaddr_in是系统封装的一个结构体，具体包含了成员变量：sin_family、sin_addr、sin_zero
};

