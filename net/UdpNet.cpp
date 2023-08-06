#include"UdpNet.h"
#include"UdpMediator.h"
#include"packDef.h"
#include<process.h>


//�������纯����
//#pragma comment(lib,"Ws2_32.lib")

UdpNet::UdpNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_handle(0),m_isStop(false)
{
	m_pNediator = pMediator;


}




UdpNet::~UdpNet() {
	m_isStop = true;
	UninitNet();
}

//��ʼ�����磬���ؿ⣬�����׽��֣���ip��ַ������һ���̣߳��������ݣ�
bool UdpNet::InitNet()
{
	

	//1:ѡ��Ŀ-----���ؿ�
	WORD wVersionRequested;
	WSADATA wsaData;  //WSADATA��һ�����ݽṹ��
	//����ṹ�������洢��WSAStartup�������ú󷵻ص�Windows Sockets���ݡ�������Winsock.dllִ�е����ݡ�
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		/*
		Tell the user that we could not find a suable
		*/

		printf("WSAStartup failed with error: %d\n", err);
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/*
		Tell the user that we could not find a usable
		WinSock DLL.
		*/
		printf("Could not find a usable version of Winsock.dll\n");

		return false;

	}
	else {
		printf("The Winsock 2.2 dll was found okay\n");
	}


	//2:����------�����׽���
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_sock)
	{
		std::cout << "socket error: " << WSAGetLastError() << std::endl;

		return false;
	}
	else {
		std::cout << "socket success." << std::endl;
	}

//	//����㲥Ȩ��
//	BOOL bval = TRUE;
//	setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bval, sizeof(bval));




	//3.ѡ��ַ-----��ip��ַ
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(_DEF_UDP_PORT);        //�����ͱ����������ֽ����������ֽ���
	sockAddr.sin_addr.S_un.S_addr = /*INADDR_ANY;  */  inet_addr _DEF_TCP_SERVER_IP;  
     err = bind(m_sock, (sockaddr*)&sockAddr, sizeof(sockAddr));
    if (SOCKET_ERROR == err) {
        std::cout << "bind error: " << WSAGetLastError() << std::endl;
        return false;
    }
    else
    {
        std::cout << "bind success. " << std::endl;
    }

	//==================================================================
	//4:�����߳̽�������
	//CreateThread��Ӧ�Ľ����̵߳ĺ�����ExitThread.
	//ExitThread�ڽ����̵߳�ʱ������߳�������c++����ʱ��ĺ���������strcpy�����ຯ����ִ�е�ʱ�������ռ䣬�������տռ䣬
	//ExitThread���˳���ʱ��Ҳ�����æ��������Ŀռ䣬�����ͻ�����ڴ�й©��
	//beginthreadex��Ӧ�Ľ����̵߳ĺ�����_endthreadex��_endthreadex�ڽ����̵߳�ʱ�򣬻��Ȼ����߳�������Ŀռ䣬�ڵ���ExitThread�����̡߳�

	m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, (void*)this, 0, NULL);

	return true;
}


//�̺߳��������ý������ݵĺ���RecvData
unsigned __stdcall UdpNet::RecvThread(void* IpVoid)
{
	UdpNet* pThis = (UdpNet*)IpVoid;
	pThis->RecvData();
	return 1;
}



//��������
bool UdpNet::SendData(long lSendIp, char* buf, int nLen)
{
	if (m_mapIdToSockAddr_in.count(lSendIp) > 0)
	{
		sockaddr_in addr = m_mapIdToSockAddr_in[lSendIp];

		//        QString ip=QString("192.168.1.100");
			//addr.sin_addr.S_un.S_addr = inet_addr((ip.c_str())) ;   //��ip��ַ�̶�Ϊ��������ip��ַ
		if (sendto(m_sock, buf, nLen, 0, (sockaddr*)&addr, sizeof(addr)) <= 0) {
			cout << "UdpNet::SendData" << WSAGetLastError() << endl;
			return false;
		}
		return true;
	}
	cout << " �޴��� " << endl;
	return false;


}

//�ر�����
void UdpNet::UninitNet()
{
	//1:�ر��߳�
	m_isStop = true;
	if (m_handle) {
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = NULL;
	}
	//2���ر�socket
	if (m_sock && INVALID_SOCKET != m_sock) {
		closesocket(m_sock);
	}

	//3:ж�ؿ�
	WSACleanup();
}

//��������
void UdpNet::RecvData()
{


	int nRecvNum = 0;
    char recvBuf[10 *1024 ] = "";
	sockaddr_in addrFrom;
	int addrFromSize = sizeof(addrFrom);
	while (!m_isStop) {
		nRecvNum = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrFrom, &addrFromSize);
		if (nRecvNum > 0) {
			//���յ����ݣ�newһ���¿ռ䣬�����������ݣ���ֹrecvBuf���ٴ�д�룬��һ�ν��յ������ݾ�û����
			char* packBuf = new char[nRecvNum];
			memcpy(packBuf, recvBuf, nRecvNum);
			//�����н���

				if (m_pNediator) {
					m_pNediator->DealData(addrFrom.sin_addr.S_un.S_addr, packBuf, nRecvNum);
					//TODO�����տռ�delete[]----kernel֪��ʲôʱ��ʹ�������ݣ��ٻ���
				}
			
		}
	}



}


