#pragma once
#include"INet.h"
#include"packDef.h"
#include<map>
#include "tcpServerMediator.h"
class tcpServer:public INet
{
public:
    tcpServer(INetMediator* pMediator);
    ~tcpServer();
    //��ʼ������
    bool InitNet();
    //��������
    bool SendData(long ISendIp, char* buf, int nLen);
    //�ر�����
    void UninitNet();

protected:
    //��������
    void RecvData();
    //   �������ӵ��߳�
    static unsigned __stdcall RecvConnectThread(void* IpVoid);
    //�������ݵ��߳�
    static unsigned __stdcall RecvThread(void* IpVoid);


    SOCKET m_sock;   //�׽���
    HANDLE m_handle;   //�߳̾��
    bool m_isStop;
    //����ÿ���ͻ��˶�Ӧ�Ľ������ݵ��߳̾����key�ǿͻ��˶�Ӧ��socket
    map<SOCKET, HANDLE> m_mapSocketToHandle;    //�����ڻ�����Դʱ������Դ
    //�����ÿ���ͻ���ͨ�ŵ�socket��key���߳�id
    map<unsigned int, SOCKET> m_mapThreadIdToSocket;    //�������߳��и����߳�id��ȡSOCKET
    

  /*  map<long, SOCKET> sockAddr_Server_Client;*/
    //FILE* pFile;   //�ļ�ָ��
    //STRU_FILE_INFO_RQ rq;//��ǰ�����ļ���������Ϣ
    //sockaddr_in serverAddr;//sockaddr_in��ϵͳ��װ��һ���ṹ�壬��������˳�Ա������sin_family��sin_addr��sin_zero
};

