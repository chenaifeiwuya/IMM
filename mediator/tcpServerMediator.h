#pragma once
#include"INetMediator.h"
#include"tcpServer.h"
    class TcpServerMediator :public INetMediator
    {

    public:
        TcpServerMediator(CKernel* kernel);
        ~TcpServerMediator();
        //������
        bool OpenNet();
        //��������
        bool SendData(long ISendIp, char* buf, int nLen);
        //�ر�����
        void CloseNet();
        //��������
        void DealData(long ISendIp, char* buf, int nLen);


      

    };



