#include"UdpMediator.h"
#include"UdpNet.h"
#include"CKernel.h"

UdpMediator::UdpMediator()
{
	m_pNet = new UdpNet(this);   //结合UdpNet的构造函数来看
}

UdpMediator::~UdpMediator() {
	//回收资源
	if (m_pNet) {
		delete m_pNet;
		m_pNet = NULL;
	}
}

//打开网络
bool UdpMediator::OpenNet()
{
    if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
}

//发送数据
bool UdpMediator::SendData(long ISendIp, char* buf, int nLen)
{
	if (!m_pNet->SendData(ISendIp, buf, nLen)) {
		return false;
	}
	return true;
}

//关闭网络
void UdpMediator::CloseNet()
{
	m_pNet->UninitNet();
}


//接收数据
void UdpMediator::DealData(long ISendIp, char* buf, int nLen)
{


        //把接收到的数据发给ckernel
    //p_Kernel->dealReadyUdpData( ISendIp,  buf, nLen);
}
