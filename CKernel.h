#pragma once
#include "tcpServerMediator.h"
#include "CMySql.h"
#include <map>
#include <list>

class CKernel;
#define BIND_PROTOCOL_PFUNC(x)  m_pFunc[(x)-_DEF_PROTOCOL_BASE-1]
typedef void (CKernel::*pFunc)(long, char*, int);    //����ָ������


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

	void sendOnlineInfo(int id);   //��idָ���û����������ߺ��ѷ�����������Ϣ
	void sendOfflineInfo(int id);   //��idָ���û����������ߺ��ѷ�����������Ϣ

public:
	INetMediator* m_pMediator;
	pFunc m_pFunc[PROTOCOL_NUM];
	CMySql m_sql;
	map<int, SOCKET> mpIdToSocket;    //�����������û�id��Ӧ��socket


};

