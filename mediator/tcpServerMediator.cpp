#include "tcpServerMediator.h"
#include "../CKernel.h"

TcpServerMediator::TcpServerMediator(CKernel* kernel)
{
    m_pNet = new tcpServer(this);   //���UdpNet�Ĺ��캯������
    p_Kernel = kernel;

}

TcpServerMediator::~TcpServerMediator()
{
    m_pNet->UninitNet();
    delete m_pNet;
    m_pNet = NULL;
}

//�����磺��������
bool TcpServerMediator::OpenNet()
{
    if (m_pNet->InitNet())
    {
        return true;
    }
    return false;
}

//��������
bool TcpServerMediator::SendData(long ISendIp, char* buf, int nLen)
{
    if (m_pNet->SendData(ISendIp, buf, nLen))  
        return true;

    return false;
}

//�ر�����
void TcpServerMediator::CloseNet()
{
    return;
}

//��������
void TcpServerMediator::DealData(long ISendIp, char* buf, int nLen)
{
    //TODO:ͨ���źŰѽ��յ������ݷ���ckernel
    cout << "TcpServerMediator::DealData " << buf << endl;
    //���Դ���
    //SendData(ISendIp, buf, nLen);
//Q_EMIT SIG_ReadData(0,buf,0);

    //�ѽ��ܵ������ݷ���kernel
    p_Kernel->dealReadyData(ISendIp, buf, nLen);
}
