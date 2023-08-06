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
	//初始化网络
    bool InitNet();
	//发送数据
	bool SendData(long ISendlp, char* buf, int nLen);
	//关闭网络
	void UninitNet();

	//typedef struct RecvInfo {
	//	long ISendIp;
	//	char* buf;
	//	int nLen;
	//};
	//static std::list<RecvInfo*> RecvInfoLst;
	//static SOCKET sock;


protected:
	//接收数据
	void RecvData();
	//线程函数，接收数据
	 static unsigned __stdcall RecvThread(void* IpVoid);


	SOCKET m_sock;
	HANDLE m_handle;
	bool m_isStop;
	//保存用户id对应的ip地址（UDP）
	map<long, sockaddr_in> m_mapIdToSockAddr_in;   //因为上线请求和下线请求是在tcp中实现的，所以此成员应同时配合着Tcp部分函数使用
	long num;   //对sock地址编号方便查找


};

