#include "CKernel.h"

CKernel::CKernel()
{

	initMapFunc();
	m_pMediator = new TcpServerMediator(this);
	//������
	if (!m_pMediator->OpenNet())
	{
		cout << "�����ʧ��" << endl;
		exit(0);
	}

	//�������ݿ�
	//�������ݿ�ʹ�õ����������������ݿ⣬��δ�´���
	if (!m_sql.ConnectMySql("127.0.0.1", "root", "mmm959825", "0103sql"))
	{
		//�������ʧ��
		cout << "���ݿ�����ʧ��" << endl;
		exit(0);
	}
	else
	{
		cout << "���ݿ����ӳɹ�" << endl;
	}

}

CKernel::~CKernel()
{
	//������Դ
	//1:�����н�����  �ر���·
	if (m_pMediator) {
		m_pMediator->CloseNet();
		delete m_pMediator;
		m_pMediator = NULL;
	}
	
}

void CKernel::initMapFunc()
{
	BIND_PROTOCOL_PFUNC(_DEF_TCP_REGISTER_RQ) = &CKernel::dealRegisterRq;
	BIND_PROTOCOL_PFUNC(_DEF_TCP_LOGIN_RQ) = &CKernel::dealLoginRq;
	BIND_PROTOCOL_PFUNC(_DEF_TCP_FRIEND_INFO) = &CKernel::dealFriendInfoRq;
	BIND_PROTOCOL_PFUNC(_DEF_TCP_OFFLINE) = &CKernel::dealOfflineRq;
	BIND_PROTOCOL_PFUNC(_DEF_PROTOCOL_CHAT_RQ) = &CKernel::dealChatRq;
	BIND_PROTOCOL_PFUNC(_DEF_TCP_ADD_FRIEND_RQ) = &CKernel::dealAddFriednRq;
	BIND_PROTOCOL_PFUNC(_DEF_TCP_ADD_FRIEND_RS) = &CKernel::dealAddFriendRs;
	//int num = _DEF_TCP_REGISTER_RQ - _DEF_PROTOCOL_BASE - 1;
	//m_pFunc[] = &CKernel::dealRegisterRq;
}

void CKernel::dealReadyData(long Ip, char* buf, int len)    //ע�⣬�����Ip�������һ��SOCKET���ͣ���socket�׽���
{
	int num = *(int*)buf;
	num -= (_DEF_PROTOCOL_BASE+1);
	if (num >= 0 && num < PROTOCOL_NUM)
	{
		(this->*(m_pFunc[num]))(Ip, buf, len);
	}
	else
	{
		cout << "dealReadyData Error" << endl;
	}
	return;
}

void CKernel::dealRegisterRq(long Ip, char* buf, int len)
{
	cout << "dealRegisterRq" << endl;
	//����ע������
	STRU_TCP_REGISTER_RQ* rq = (STRU_TCP_REGISTER_RQ*)buf;
	STRU_TCP_REGISTER_RS rs;

	//���ȶ����ݽ���У��(����Ŀ�ִ˲�ʡ��)

	//Ȼ���ѯ���ݿ⣬�鿴���ֻ����Ƿ��Ѿ�ע�����
	char sqlBuf[1024];
	list<string> sqlList;
	sprintf_s(sqlBuf, "select tel from t_user where tel = \"%s\";", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf, 1, sqlList))
	{
		//ִ�г�����false
		cout << "select failed with:  " << sqlBuf << endl;
	}
	//����ע�ᣬ�򷵻��û��Ѵ���
	if (sqlList.size() != 0)
	{
		rs.result = USER_IS_EXIST;
	}
	//��δע�ᣬ����Ϣд��
	else
	{
		rs.result = REGISTER_SUCCESS;
		//����Ϣд��
		sprintf_s(sqlBuf, "insert into t_user (tel,password,name) values(\"%s\",\"%s\",\"%s\");", rq->tel, rq->password, rq->name);
		if (!m_sql.UpdateMySql(sqlBuf))
		{
			//ִ��ʧ��
			cout << "insert failed with:  " << sqlBuf << endl;
			return;
		}
		else
		{
			cout << "insert success!" << endl;
		}


	}	
	//����ע��ɹ�����Ϣ
	m_pMediator->SendData(Ip, (char*)&rs, sizeof(rs));
}

void CKernel::dealLoginRq(long Ip, char* buf, int len)
{
	//�����¼����
	STRU_TCP_LOGIN_RQ* rq = (STRU_TCP_LOGIN_RQ*)buf;
	STRU_TCP_LOGIN_RS rs;

	//��ѯ���ݿ����Ƿ���ڸ��û�
	char sqlBuf[1024];
	list<string> sqlList;
	sprintf_s(sqlBuf, "select id,password,name,feeling from t_user where tel = \"%s\";", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf, 4, sqlList))
	{
		//�����ѯʧ��
		cout << "select failed with: " << sqlBuf << endl;
		return;
	}
	if (sqlList.size() == 0)
	{
		//�����ڴ��û�
		rs.result = USER_IS_NOTEXIST;
	}
	else
	{
		int id = atoi(sqlList.front().c_str());
		sqlList.pop_front();
		string password = sqlList.front();
		sqlList.pop_front();
		string name = sqlList.front();
		sqlList.pop_front();
		string feeling = sqlList.front();
		sqlList.pop_front();
		//���ж��������Ƿ���ȷ
		if (password != rq->password)
		{
			//�����������
			rs.result = PASSWORD_ERROR;
		}
		else {
			//���ڴ��û�  ��������ȷ
			//�ж����Ƿ������ߣ�����������ֹ���¼

			if (/*mpIdToSocket.count(id) > 0*/ 0)   //�˹�����ʱ���ã��������
			{
				//������
				rs.result = USER_IS_ONLINE;   //���ظ������û�������
			}
			else
			{

				//���򣬽��䱣�浽map
				mpIdToSocket[id] = Ip;    //�����Ip��ʾ����һ��SOCKET
				//���ص�¼�ɹ���Ϣ
				rs.result = LOGIN_SUCCESS;
				rs.userId = id;
				strcpy_s(rs.name, name.c_str());
				strcpy_s(rs.feeling, feeling.c_str());
				//TODO  ������Ϣ�����������к��ѵ�ǰ�û�������
				sendOnlineInfo(id);
				
			}
		}

	}
	m_pMediator->SendData(Ip, (char*)&rs, sizeof(rs));
}

//�����ȡ������Ϣ������
void CKernel::dealFriendInfoRq(long Ip, char* buf, int len)
{
	STRU_TCP_FRIEND_INFO* rq = (STRU_TCP_FRIEND_INFO*)buf;
	STRU_TCP_FRIEND_INFO rs;
	//��ѯ���ݿ⣬��õ�ǰ�����ߵ����к�����Ϣ
	char sqlBuf[1024];
	list<string> sqlList;
	sprintf_s(sqlBuf, "select id,name,icon,feeling from t_user where id in (select idB from t_friend where idA = %d);", rq->userId);

	if (!m_sql.SelectMySql(sqlBuf, 4, sqlList))
	{
		//�����ѯʧ�ܣ���ӡ���󣬲�����
		cout << "select failed with :  " << sqlBuf << endl;
		return;
	}
	//��ʼ��������Ϣ���͸���ǰid������
	if (mpIdToSocket.count(rq->userId) == 0)
	{
		cout << "id is not exist in map" << endl;
		return;
	}
	SOCKET sock = mpIdToSocket[rq->userId];
	while (sqlList.size() != 0)
	{
		rs.userId = atoi(sqlList.front().c_str());
		sqlList.pop_front();
		strcpy_s(rs.name, sqlList.front().c_str());
		sqlList.pop_front();
		rs.iconId = atoi(sqlList.front().c_str());
		sqlList.pop_front();
		strcpy_s(rs.feeling, sqlList.front().c_str());
		sqlList.pop_front();
		
		//�жϺ����Ƿ�������
		if (mpIdToSocket.count(rs.userId) > 0)
		{
			rs.status = STATUS_ONLINE;
		}
		else
		{
			rs.status = STATUS_OFFLINE;
		}

		m_pMediator->SendData(sock, (char*)&rs, sizeof(rs));
	}

}

void CKernel::dealOfflineRq(long Ip, char* buf, int len)
{
	//������������
	STRU_TCP_OFFLINE_RQ* rq = (STRU_TCP_OFFLINE_RQ*)buf;
	//���ж����Ƿ������ӻ�����
	if (mpIdToSocket.count(rq->userId) > 0)
	{
		//������Լ��Ļ���������
		auto ite = mpIdToSocket.find(rq->userId);
		mpIdToSocket.erase(ite);


		//���������к��ѣ���������Ϣ�����������к���
		sendOfflineInfo(rq->userId);
		
	}
}

//����������Ϣ����
void CKernel::dealChatRq(long Ip, char* buf, int len)
{
	//���
	STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)buf;

	//�鿴�Ƿ���˭��
	//���ȿ��Է�����û�У����δ���ߣ���ֱ�ӷ���
	if (mpIdToSocket.count(rq->friendId) == 0)
		return;
	//�������ˣ��򽫰�ת������
	m_pMediator->SendData(mpIdToSocket[rq->friendId], buf, len);
}

void CKernel::dealAddFriednRq(long Ip, char* buf, int len)
{
	//����鿴�����˭��
	//TODO
	STRU_TCP_ADD_FRIEND_RQ* rq = (STRU_TCP_ADD_FRIEND_RQ*)buf;
	//����name��ѯ������û���id
	char sqlBuf[1024];
	list<string> sqlList;
	sprintf_s(sqlBuf,"select id from t_user where name = \"%s\"; ",rq->friendName);
	if (!m_sql.SelectMySql(sqlBuf, 1, sqlList))
	{
		cout << "select failed with:  " << sqlBuf << endl;
		return;
	}
	if (sqlList.size() == 0)   //������ݿ���û�У���ֱ�ӷ��ؼ���
		return;
	int id = atoi(sqlList.front().c_str());   //�ַ���ת����
	//�ٸ���id��ѯ���û���ǰ�Ƿ������ϣ��������������ֱ�ӷ���
	if (mpIdToSocket.count(id) > 0)
	{
		//��������ϣ������䷢����Ӻ�������
		m_pMediator->SendData(mpIdToSocket[id], buf, len);

	}
	//�������������ֱ�ӷ���
	return;


}

//������Ӻ��ѻظ�
void CKernel::dealAddFriendRs(long Ip, char* buf, int len)
{
	//���
	STRU_TCP_ADD_FRIEND_RS* rs = (STRU_TCP_ADD_FRIEND_RS*)buf;
	//�ж��Ƿ���ӳɹ�
	if (rs->result == ADD_FRIEND_SUCCESS)
	{
		//����ӳɹ�����д�����ݿ⣬�����͸��������ӳɹ�����Ϣ
		//д�����ݿ�
		char sqlBuf[1024];
		sprintf_s(sqlBuf, "insert into t_friend values (%d , %d);", rs->userId, rs->friendId);
		if (!m_sql.UpdateMySql(sqlBuf))
		{
			cout << "insert failed with:  " << sqlBuf << endl;
			return;
		}
		sprintf_s(sqlBuf, "insert into t_friend values (%d , %d);", rs->friendId, rs->userId);
		if (!m_sql.UpdateMySql(sqlBuf))
		{
			cout << "insert failed with:  " << sqlBuf << endl;
			return;
		}
	}
	
	//����ӽ�������������
	if (mpIdToSocket.count(rs->userId) > 0)
	{
		//���ж����Ƿ�����
		//�������
		m_pMediator->SendData(mpIdToSocket[rs->userId], buf, len);
	}
	return;

}

void CKernel::sendOnlineInfo(int id ) 
{
	STRU_TCP_FRIEND_INFO rq;
	rq.userId = id;
	rq.status = STATUS_ONLINE;
	char sqlBuf[1024];
	list<string> sqlList;
	
	//��ѯ��ǰidӵ���ߵ����к���
	sprintf_s(sqlBuf, "select idB from t_friend where idA = %d;", id);
	if (!m_sql.SelectMySql(sqlBuf,1,sqlList))
	{
		//��ѯ����
		cout << "select error:  " << sqlBuf << endl;
		return;
	}
	int idFriend = 0;
	while (sqlList.size() != 0)
	{
		idFriend = atoi(sqlList.front().c_str());
		sqlList.pop_front();
		//�ж�������Ƿ�����,���������ͣ����򲻷���
		if (mpIdToSocket.count(idFriend) > 0)
		{
			//��������
			m_pMediator->SendData(mpIdToSocket[idFriend], (char*)&rq, sizeof(rq));
		}
	}
}

void CKernel::sendOfflineInfo(int id)
{
	STRU_TCP_FRIEND_INFO rq;
	rq.userId = id;
	rq.status = STATUS_OFFLINE;
	char sqlBuf[1024];
	list<string> sqlList;

	//��ѯ��ǰidӵ���ߵ����к���
	sprintf_s(sqlBuf, "select idB from t_friend where idA = %d;", id);
	if (!m_sql.SelectMySql(sqlBuf, 1, sqlList))
	{
		//��ѯ����
		cout << "select error:  " << sqlBuf << endl;
		return;
	}
	int idFriend = 0;
	while (sqlList.size() != 0)
	{
		idFriend = atoi(sqlList.front().c_str());
		sqlList.pop_front();
		//�ж�������Ƿ�����,���������ͣ����򲻷���
		if (mpIdToSocket.count(idFriend) > 0)
		{
			//��������
			m_pMediator->SendData(mpIdToSocket[idFriend], (char*)&rq, sizeof(rq));
		}
	}
}
