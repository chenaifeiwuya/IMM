#pragma once
#include<iostream>
#include<winsock.h>
#include<set>
#include<thread>

using namespace std;

#pragma comment(lib,"Ws2_32.lib")

class INetMediator;   //�ȸ��߱�����������࣬����û�б���
class INet
{
public:
	INet() {}
	virtual ~INet(){}

	//��ʼ������
    virtual bool InitNet() = 0;
	//��������
	 virtual bool SendData(long ISendIp, char* buf, int nLend=0)=0;

	//�ر�����
	virtual void UninitNet() = 0;


    //��long���͵�ip��ַת����string���͵�ip��ַ
    static std::string GetIpString(long ip)
    {
        sockaddr_in sockAddr;
        sockAddr.sin_addr.S_un.S_addr = ip;
        return inet_ntoa(sockAddr.sin_addr);  //inet_ntoa()�������ַת���ɡ�.��������ַ�����ʽ
    }
    //char*, std::string, QString (char*�ǻ����������ͣ�std::string��QString�Ƿ�װ�õ��࣬�����������Ϳ��Ը���ֱ�Ӹ�ֵ��
    //std::string = char*, QString = char*
    //std::string ����c_str()ת����char*
    //QString ���Ե���toStdStringת����std::string

    //��ȡ����IP��ַ�б�
    static set<long> GetValidIpList()
    {
        set<long> setIp;
        long ip = 0;
        int i=0;
        //��ȡ��������
        char nameBuf[100]="";
        gethostname(nameBuf, 100);
        //2:������������ȡip��ַ�б�
        struct hostent *remoteHost = gethostbyname(nameBuf);
        while(remoteHost->h_addr_list[i] != 0){
            ip = *(u_long *) remoteHost->h_addr_list[i++];
            setIp.insert(ip);
        }
        return setIp;
    }

protected:
	//��������
	virtual void RecvData() = 0;
	INetMediator* m_pNediator;   //�н�����ָ�룬����ָ��ָ������ӿ�

};
