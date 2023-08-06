#include "tcpClientMediator.h"
#include "CKernel.h"

tcpClientMediator::tcpClientMediator()
{
    m_pNet = new tcpClient(this);   //结合UdpNet的构造函数来看

}

tcpClientMediator::~tcpClientMediator()
{
    delete m_pNet;
}

//打开网络：建立连接
bool tcpClientMediator::OpenNet()
{
    if (m_pNet->InitNet())
    {
        return true;
    }
    return false;
}

//发送数据
bool tcpClientMediator::SendData(long ISendIp, char* buf, int nLen)
{
    if (m_pNet->SendData(ISendIp, buf, nLen))
        return true;

    return false;
}

//关闭网络
void tcpClientMediator::CloseNet()
{
    m_pNet->UninitNet();
}

//接收数据
void tcpClientMediator::DealData(long ISendIp, char* buf, int nLen)
{
    //TODO:通过信号把接收到的数据发给ckernel
    cout << "TcpServerMediator::DealData " << buf << endl;
    //测试代码
    //SendData(ISendIp, buf, nLen);
//Q_EMIT SIG_ReadData(0,buf,0);

    //把接受到的数据发给kernel
    Q_EMIT SIG_dealData(ISendIp, buf, nLen);
}
