#include "tcpServer.h"
#include<process.h>

tcpServer::tcpServer(INetMediator* pMediator)
{
	m_pNediator = pMediator;
}

tcpServer::~tcpServer()
{

}

bool tcpServer::InitNet()
{
    m_isStop = false;
    //1��ѡ��Ŀ--���ؿ�
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("Could not find a usable version of Winsock.dll\n");
        return false;
    }
    else {
        printf("The Winsock 2.2 dll was found okay\n");
    }

    //2:����--�����׽���

    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  //�����׽���
    if (INVALID_SOCKET == m_sock) {
        std::cout << "socket error:" << WSAGetLastError() << std::endl;

        return false;
    }
    else {
        cout << "socket success" << endl;
    }

    //3����ip��ַ
    //�����������ַ��Ϣ
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_DEF_TCP_PORT);
    serverAddr.sin_addr.S_un.S_addr =/* INADDR_ANY;*/ inet_addr _DEF_TCP_SERVER_IP;

    err = bind(m_sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (SOCKET_ERROR == err) {
        cout << "bind error:" << WSAGetLastError() << endl;
        closesocket(m_sock);
        WSACleanup();
        return false;
    }
    else {
        cout << "bind success" << endl;
    }

    //4:����
    err = listen(m_sock, 10);  //10Ϊ�������Ӷ��е���󳤶�(������������������,����������������ϵͳ���������ȹ���)
    if (SOCKET_ERROR == err) {
        cout << "listen error:" << WSAGetLastError() << endl;
        closesocket(m_sock);
        WSACleanup();
        return false;
    }
    else
    {
        cout << "listen success" << endl;
    }

    //5:����һ���������ӵ��߳�
    m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvConnectThread,(void*)this, 0, NULL);
    return true;

}


bool tcpServer::SendData(long ISendIp, char* buf, int nLen)
{
    // Tcp�������ֽ����ģ����Ի����ճ�����⣺
    //1���ж���������Ϸ���
    if (!buf || nLen <= 0) {
        cout << "�������Ϸ� " << endl;
        return false;
    }

    //2���������ݰ�����
    if (send(ISendIp, (char*)&nLen, sizeof(int), 0) <= 0) {   //ISendIpʹsockt
        cout << "TcpServerNet::SendData error: " << WSAGetLastError() << endl;
        return false;
    }

    //3�����Ͱ�����
    if (send(ISendIp, buf, nLen, 0) <= 0) {
        cout << "TcpServerNet::SendData error: " << WSAGetLastError() << endl;
        return false;
    }

    return true;
}

//�ر�����
void tcpServer::UninitNet()
{
    //1:�ر��̣߳������˳���־λΪtrue����һ��ʱ����߳��Լ��˳����رվ��
    m_isStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {
            TerminateThread(m_handle, -1);
        }
        CloseHandle(m_handle);
        m_handle = NULL;
    }

    for (auto ite = m_mapSocketToHandle.begin(); ite != m_mapSocketToHandle.end();) {
        HANDLE handle = ite->second;
        if (handle) {
            if (WAIT_TIMEOUT == WaitForSingleObject(handle, 100)) {
                TerminateThread(handle, -1);
            }
            CloseHandle(handle);
            handle = NULL;
        }
        //�Ƴ�ʧЧ��map�ڵ�
        ite = m_mapSocketToHandle.erase(ite);
    }

    //2:�ر�socket
    if (m_sock && INVALID_SOCKET != m_sock) {
        closesocket(m_sock);
    }

    for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();)
    {
        SOCKET sock = ite->second;
        if (sock && INVALID_SOCKET != sock) {
            closesocket(sock);
        }
        //�Ƴ�ʧЧ��map�ڵ�
        ite = m_mapThreadIdToSocket.erase(ite);
    }

    //3:ж�ؿ�
    WSACleanup();
}

//�̺߳��������ý������ݵĺ���RecvData
unsigned __stdcall tcpServer::RecvThread(void* IpVoid)
{
    tcpServer* pThis = (tcpServer*)IpVoid;
    pThis->RecvData();
    return 1;
}


//��������
void tcpServer::RecvData()
{
    cout << " tcpServer::RecvData " << endl;
    //1:˯һ�������Ϊ�߳��Ǵ��������У����ǰ�socket����map����Ҫһ��ʱ�䣬������Ҫ�̵߳�һ�����ȡsocket
    Sleep(10);
    //2:ȡ��socket
    SOCKET sock = m_mapThreadIdToSocket[GetCurrentThreadId()];  //GetCurrentThreadId()�������Եõ����������߳��е��߳�id
    //3:�ж�socket�Ϸ���
    if (!sock && INVALID_SOCKET == sock) {
        cout << "tcpServer::RecvData socket error " << endl;
        return;
    }
    //4:��������
  
    //��ΪTcp�������ֽ����ģ����Ի���ճ�����⡣
    //���������
    //1����Ϣ����
    //2�����ڷ��Ͱ�ǰ���ȷ��Ͱ���С
    //3��������
    //4���ڰ�ͷ���߽�β�����ñ�־λ

    //�м�offset���㣡��������
    int nRecvNum = 0;
    int nPackSize = 0;
    int offset = 0;
    while (!m_isStop) {
        //�Ƚ��հ��Ĵ�С
        nRecvNum = recv(sock, (char*)&nPackSize, sizeof(int), 0);
        if (nRecvNum > 0) {
            //���հ�����
            char* packBuf = new char[nPackSize];
            offset = 0;
            while (nPackSize > 0) {
                nRecvNum = recv(sock, packBuf + offset, nPackSize, 0);   //nPackSizeΪ�ӻ�����ȡ�����ݵ���󳤶�
                if (nRecvNum > 0) {
                    offset += nRecvNum;
                    nPackSize -= nRecvNum;
                }
            }
            //�����н���
            if (m_pNediator) {
                m_pNediator->DealData(sock, packBuf, offset);
                //TODO:���տռ� delete[] -- kernel֪��ʲôʱ��ʹ�������ݣ��ٻ���
            }
        }
        else if (0 == nRecvNum || 10054 == WSAGetLastError()) {   //����ֵΪ0��ʾ���ӶϿ�
            cout << "TcpServerNet::RecvData close connection" << endl;
            //������Դ
            auto ite = m_mapThreadIdToSocket.find(GetCurrentThreadId());  //���ݵ�ǰ�߳�id��õ�ǰsocket���ڵĵ�����
            if(ite != m_mapThreadIdToSocket.end())   //�ж�һ�¸õ������Ƿ�Ϊĩβ
                m_mapThreadIdToSocket.erase(ite);
            break;
        }
        else {
            cout << "TcpServerNet::RecvData error" << WSAGetLastError() << endl;
        }
    }
}

//�̺߳��������տͻ��˵�����
unsigned __stdcall tcpServer::RecvConnectThread(void* IpVoid)
{
    cout << " tcpServer::RecvConnectThread " << endl;
    tcpServer* pThis = (tcpServer*)IpVoid;
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    unsigned int threadId = 0;
    while (!pThis->m_isStop)
    {
        cout << "TcpServerNet::AcceptThread accept" << endl;
        //��������
        SOCKET sockWaiter = accept(pThis->m_sock, (sockaddr*)&clientAddr, &clientAddrSize);
        cout << "threadId: " << threadId << endl;
        //����һ���������ݵ��߳�
        HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, (void*)pThis, 0, &threadId);
        //����socket��map�� key���߳�id
        pThis->m_mapThreadIdToSocket[threadId] = sockWaiter;
        //�����߳̾��
        pThis->m_mapSocketToHandle[sockWaiter] = handle;
    }
	return 1;
}

