#pragma once
#include "INet.h"
#include<Windows.h>
#include<winsock.h>
#include<iostream>
#include<list>
#include<map>



class UdpNet :public INet
{
public:
	UdpNet(INetMediator* pMediator);
	~UdpNet();
	//��ʼ������
    bool InitNet();
	//��������
	bool SendData(long ISendlp, char* buf, int nLen);
	//�ر�����
	void UninitNet();

	//typedef struct RecvInfo {
	//	long ISendIp;
	//	char* buf;
	//	int nLen;
	//};
	//static std::list<RecvInfo*> RecvInfoLst;
	//static SOCKET sock;


protected:
	//��������
	void RecvData();
	//�̺߳�������������
	 static unsigned __stdcall RecvThread(void* IpVoid);


	SOCKET m_sock;
	HANDLE m_handle;
	bool m_isStop;
	//�����û�id��Ӧ��ip��ַ��UDP��
	map<long, sockaddr_in> m_mapIdToSockAddr_in;   //��Ϊ���������������������tcp��ʵ�ֵģ����Դ˳�ԱӦͬʱ�����Tcp���ֺ���ʹ��
	long num;   //��sock��ַ��ŷ������


};

