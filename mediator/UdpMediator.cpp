#include"UdpMediator.h"
#include"UdpNet.h"
#include"../CKernel.h"

UdpMediator::UdpMediator(CKernel* kernel)
{
	m_pNet = new UdpNet(this);   //���UdpNet�Ĺ��캯������
	p_Kernel = kernel;
}

UdpMediator::~UdpMediator() {
	//������Դ
	if (m_pNet) {
		delete m_pNet;
		m_pNet = NULL;
	}
}

//������
bool UdpMediator::OpenNet()
{
    if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
}

//��������
bool UdpMediator::SendData(long ISendIp, char* buf, int nLen)
{
	if (!m_pNet->SendData(ISendIp, buf, nLen)) {
		return false;
	}
	return true;
}

//�ر�����
void UdpMediator::CloseNet()
{
	m_pNet->UninitNet();
}


//��������
void UdpMediator::DealData(long ISendIp, char* buf, int nLen)
{


        //�ѽ��յ������ݷ���ckernel
	p_Kernel->dealReadyData( ISendIp,  buf, nLen);
}
