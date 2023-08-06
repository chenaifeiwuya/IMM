#pragma once
#include"INetMediator.h"

class UdpMediator :public INetMediator
{
   



    //�ѽ��յ������ݷ��͸�kernel
    void SIG_ReadData(long lSendIp , char* buf , int nLen);

public:
	UdpMediator(CKernel* kernel);
	~UdpMediator();
	//������
    bool OpenNet();
	//��������
	bool SendData(long ISendIp, char* buf, int nLen);
	//�ر�����
	void CloseNet();
	//��������
	void DealData(long ISendIp, char* buf, int nLen);
};
