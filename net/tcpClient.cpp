#include "tcpClient.h"
#include<process.h>

tcpClient::tcpClient(INetMediator* pMediator)
{
	m_pNediator = pMediator;
}

tcpClient::~tcpClient()
{

}

bool tcpClient::InitNet()
{
    //1：选项目--加载库
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("Could not find a usable version of Winsock.dll\n");
        return false;
    }
    else {
        printf("The Winsock 2.2 dll was found okay\n");
    }

    //2:雇人--创建套接字

    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  //创建套接字
    if (INVALID_SOCKET == m_sock) {
        std::cout << "socket error:" << WSAGetLastError() << std::endl;

        return false;
    }
    else {
        std::cout << "socket success" << std::endl;
    }

    //3：绑定ip地址
    //初始化服务器地址信息

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_DEF_TCP_PORT);
    serverAddr.sin_addr.S_un.S_addr = inet_addr _DEF_TCP_SERVER_IP;
    //开始与服务器建立链接
    err=connect(m_sock,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    if(err==SOCKET_ERROR)
    {
        std::cout << "connect error:" << WSAGetLastError() << std::endl;
        m_isStop=true;
        return false;
    }


    //创建线程开始收数据
    //5:创建一个接受连接的线程
    m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread,(void*)this, 0, NULL);
    return true;

}


bool tcpClient::SendData(long ISendIp, char* buf, int nLen)   //tcp客户端这里的ISendIp不起作用，因为客户端数据都是发给服务器的
{
    // Tcp是面向字节流的，所以会产生粘包问题：
    //1：判断输入参数合法性
    if (!buf || nLen <= 0) {
        cout << "参数不合法 " << endl;
        return false;
    }

    //2：发送数据包长度
    if (send(m_sock, (char*)&nLen, sizeof(int), 0) <= 0) {   //ISendIp使sockt
        cout << "TcpServerNet::SendData error: " << WSAGetLastError() << endl;
        return false;
    }

    //3：发送包内容
    if (send(m_sock, buf, nLen, 0) <= 0) {
        cout << "TcpServerNet::SendData error: " << WSAGetLastError() << endl;
        return false;
    }

    return true;
}

//关闭网络
void tcpClient::UninitNet()
{
    //1:关闭线程：设置退出标志位为true，等一段时间等线程自己退出，关闭句柄
    m_isStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {
            TerminateThread(m_handle, -1);
        }
        CloseHandle(m_handle);
        m_handle = NULL;
    }
    //2:关闭socket
    if (m_sock && INVALID_SOCKET != m_sock) {
        closesocket(m_sock);
    }

    //3:卸载库
    WSACleanup();
}

//线程函数：调用接收数据的函数RecvData
unsigned __stdcall tcpClient::RecvThread(void* IpVoid)
{
    tcpClient* pThis = (tcpClient*)IpVoid;
    pThis->RecvData();
    return 1;
}


//接收数据
void tcpClient::RecvData()
{


    //接收数据
  
    //因为Tcp是面向字节流的，所以会有粘包问题。
    //解决方法：
    //1：消息定长
    //2：在在发送包前面先发送包大小
    //3：短连接
    //4：在包头或者结尾处设置标志位

    //切记offset清零！！！！！
    int nRecvNum = 0;
    int nPackSize = 0;
    int offset = 0;
    while (!m_isStop) {
        //先接收包的大小
        nRecvNum = recv(m_sock, (char*)&nPackSize, sizeof(int), 0);
        if (nRecvNum > 0) {
            //接收包内容
            char* packBuf = new char[nPackSize];
            offset = 0;
            while (nPackSize > 0) {
                nRecvNum = recv(m_sock, packBuf + offset, nPackSize, 0);   //nPackSize为从缓冲区取的数据的最大长度
                if (nRecvNum > 0) {
                    offset += nRecvNum;
                    nPackSize -= nRecvNum;
                }
            }
            //传给中介者
            if (m_pNediator) {
                m_pNediator->DealData(m_sock, packBuf, offset);
                //TODO:回收空间 delete[] -- kernel知道什么时候使用完数据，再回收
            }
        }
        else if (0 == nRecvNum || 10054 == WSAGetLastError()) {
            cout << "TcpServerNet::RecvData close connection" << endl;
            break;
        }
        else {
            cout << "TcpServerNet::RecvData error" << WSAGetLastError() << endl;
        }
    }
}



