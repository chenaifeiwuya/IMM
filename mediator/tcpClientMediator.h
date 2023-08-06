#pragma once
#include"INetMediator.h"
#include"tcpClient.h"
    class tcpClientMediator :public INetMediator
    {
        Q_OBJECT
    public:
        tcpClientMediator();
        ~tcpClientMediator();
        //???????
        bool OpenNet();
        //????????
        bool SendData(long ISendIp, char* buf, int nLen);
        //???????
        void CloseNet();
        //????????
        void DealData(long ISendIp, char* buf, int nLen);
};



