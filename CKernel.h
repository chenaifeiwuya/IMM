#pragma once
#include "tcpServerMediator.h"
#include "CMySql.h"
#include <map>
#include <list>

class CKernel;
#define BIND_PROTOCOL_PFUNC(x)  m_pFunc[(x)-_DEF_PROTOCOL_BASE-1]
typedef void (CKernel::*pFunc)(long, char*, int);    //函数指针数组


class CKernel
{
public:
	CKernel();
	~CKernel();
public:
	void initMapFunc();


public:
	void dealReadyData(long Ip, char* buf, int len);
	void dealRegisterRq(long Ip, char* buf, int len);
	void dealLoginRq(long Ip, char* buf, int len);
	void dealFriendInfoRq(long Ip, char* buf, int len);
	void dealOfflineRq(long Ip, char* buf, int len);
	void dealChatRq(long Ip, char* buf, int len);
	void dealAddFriednRq(long Ip, char* buf, int len);
	void dealAddFriendRs(long Ip, char* buf, int len);

	void sendOnlineInfo(int id);   //向id指定用户的所有在线好友发送其上线消息
	void sendOfflineInfo(int id);   //向id指定用户的所有在线好友发送其下线消息

public:
	INetMediator* m_pMediator;
	pFunc m_pFunc[PROTOCOL_NUM];
	CMySql m_sql;
	map<int, SOCKET> mpIdToSocket;    //保存已上线用户id对应的socket


};

