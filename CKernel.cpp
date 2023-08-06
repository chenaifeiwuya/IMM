#include "CKernel.h"

CKernel::CKernel()
{

	initMapFunc();
	m_pMediator = new TcpServerMediator(this);
	//打开网络
	if (!m_pMediator->OpenNet())
	{
		cout << "网络打开失败" << endl;
		exit(0);
	}

	//链接数据库
	//本次数据库使用的是曾经创建的数据库，并未新创建
	if (!m_sql.ConnectMySql("127.0.0.1", "root", "mmm959825", "0103sql"))
	{
		//如果连接失败
		cout << "数据库连接失败" << endl;
		exit(0);
	}
	else
	{
		cout << "数据库连接成功" << endl;
	}

}

CKernel::~CKernel()
{
	//回收资源
	//1:回收中介者类  关闭网路
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

void CKernel::dealReadyData(long Ip, char* buf, int len)    //注意，这里的Ip保存的是一个SOCKET类型，是socket套接字
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
	//处理注册请求
	STRU_TCP_REGISTER_RQ* rq = (STRU_TCP_REGISTER_RQ*)buf;
	STRU_TCP_REGISTER_RS rs;

	//首先对数据进行校验(本项目种此步省略)

	//然后查询数据库，查看该手机号是否已经注册过了
	char sqlBuf[1024];
	list<string> sqlList;
	sprintf_s(sqlBuf, "select tel from t_user where tel = \"%s\";", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf, 1, sqlList))
	{
		//执行出错返回false
		cout << "select failed with:  " << sqlBuf << endl;
	}
	//若已注册，则返回用户已存在
	if (sqlList.size() != 0)
	{
		rs.result = USER_IS_EXIST;
	}
	//若未注册，则将信息写入
	else
	{
		rs.result = REGISTER_SUCCESS;
		//将信息写入
		sprintf_s(sqlBuf, "insert into t_user (tel,password,name) values(\"%s\",\"%s\",\"%s\");", rq->tel, rq->password, rq->name);
		if (!m_sql.UpdateMySql(sqlBuf))
		{
			//执行失败
			cout << "insert failed with:  " << sqlBuf << endl;
			return;
		}
		else
		{
			cout << "insert success!" << endl;
		}


	}	
	//返回注册成功的信息
	m_pMediator->SendData(Ip, (char*)&rs, sizeof(rs));
}

void CKernel::dealLoginRq(long Ip, char* buf, int len)
{
	//处理登录请求
	STRU_TCP_LOGIN_RQ* rq = (STRU_TCP_LOGIN_RQ*)buf;
	STRU_TCP_LOGIN_RS rs;

	//查询数据库中是否存在该用户
	char sqlBuf[1024];
	list<string> sqlList;
	sprintf_s(sqlBuf, "select id,password,name,feeling from t_user where tel = \"%s\";", rq->tel);
	if (!m_sql.SelectMySql(sqlBuf, 4, sqlList))
	{
		//如果查询失败
		cout << "select failed with: " << sqlBuf << endl;
		return;
	}
	if (sqlList.size() == 0)
	{
		//不存在此用户
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
		//先判断其密码是否正确
		if (password != rq->password)
		{
			//返回密码错误
			rs.result = PASSWORD_ERROR;
		}
		else {
			//存在此用户  且密码正确
			//判断其是否已在线，若在线则阻止其登录

			if (/*mpIdToSocket.count(id) > 0*/ 0)   //此功能暂时不用，方便测试
			{
				//已上线
				rs.result = USER_IS_ONLINE;   //返回告诉他用户已在线
			}
			else
			{

				//否则，将其保存到map
				mpIdToSocket[id] = Ip;    //这里的Ip表示的是一个SOCKET
				//返回登录成功消息
				rs.result = LOGIN_SUCCESS;
				rs.userId = id;
				strcpy_s(rs.name, name.c_str());
				strcpy_s(rs.feeling, feeling.c_str());
				//TODO  ：发消息告诉他的所有好友当前用户上线了
				sendOnlineInfo(id);
				
			}
		}

	}
	m_pMediator->SendData(Ip, (char*)&rs, sizeof(rs));
}

//处理获取好友信息的请求
void CKernel::dealFriendInfoRq(long Ip, char* buf, int len)
{
	STRU_TCP_FRIEND_INFO* rq = (STRU_TCP_FRIEND_INFO*)buf;
	STRU_TCP_FRIEND_INFO rs;
	//查询数据库，获得当前请求者的所有好友信息
	char sqlBuf[1024];
	list<string> sqlList;
	sprintf_s(sqlBuf, "select id,name,icon,feeling from t_user where id in (select idB from t_friend where idA = %d);", rq->userId);

	if (!m_sql.SelectMySql(sqlBuf, 4, sqlList))
	{
		//如果查询失败，打印错误，并返回
		cout << "select failed with :  " << sqlBuf << endl;
		return;
	}
	//开始将所有信息发送给当前id持有者
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
		
		//判断好友是否在线上
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
	//处理下线请求
	STRU_TCP_OFFLINE_RQ* rq = (STRU_TCP_OFFLINE_RQ*)buf;
	//先判断其是否在连接缓存中
	if (mpIdToSocket.count(rq->userId) > 0)
	{
		//如果在自己的缓存连接中
		auto ite = mpIdToSocket.find(rq->userId);
		mpIdToSocket.erase(ite);


		//遍历其所有好友，将下线信息发给他的所有好友
		sendOfflineInfo(rq->userId);
		
	}
}

//处理聊天信息请求
void CKernel::dealChatRq(long Ip, char* buf, int len)
{
	//拆包
	STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)buf;

	//查看是发给谁的
	//首先看对方上线没有，如果未上线，则直接返回
	if (mpIdToSocket.count(rq->friendId) == 0)
		return;
	//若上线了，则将包转发给他
	m_pMediator->SendData(mpIdToSocket[rq->friendId], buf, len);
}

void CKernel::dealAddFriednRq(long Ip, char* buf, int len)
{
	//拆包查看是添加谁的
	//TODO
	STRU_TCP_ADD_FRIEND_RQ* rq = (STRU_TCP_ADD_FRIEND_RQ*)buf;
	//根据name查询被添加用户的id
	char sqlBuf[1024];
	list<string> sqlList;
	sprintf_s(sqlBuf,"select id from t_user where name = \"%s\"; ",rq->friendName);
	if (!m_sql.SelectMySql(sqlBuf, 1, sqlList))
	{
		cout << "select failed with:  " << sqlBuf << endl;
		return;
	}
	if (sqlList.size() == 0)   //如果数据库中没有，则直接返回即可
		return;
	int id = atoi(sqlList.front().c_str());   //字符串转整型
	//再根据id查询该用户当前是否在线上，如果不在线上则直接返回
	if (mpIdToSocket.count(id) > 0)
	{
		//如果在线上，则向其发送添加好友请求
		m_pMediator->SendData(mpIdToSocket[id], buf, len);

	}
	//如果不在线上则直接返回
	return;


}

//处理添加好友回复
void CKernel::dealAddFriendRs(long Ip, char* buf, int len)
{
	//拆包
	STRU_TCP_ADD_FRIEND_RS* rs = (STRU_TCP_ADD_FRIEND_RS*)buf;
	//判断是否添加成功
	if (rs->result == ADD_FRIEND_SUCCESS)
	{
		//若添加成功，则写入数据库，并发送给添加者添加成功的消息
		//写入数据库
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
	
	//将添加结果返还给添加者
	if (mpIdToSocket.count(rs->userId) > 0)
	{
		//先判断其是否还在线
		//如果在线
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
	
	//查询当前id拥有者的所有好友
	sprintf_s(sqlBuf, "select idB from t_friend where idA = %d;", id);
	if (!m_sql.SelectMySql(sqlBuf,1,sqlList))
	{
		//查询出错
		cout << "select error:  " << sqlBuf << endl;
		return;
	}
	int idFriend = 0;
	while (sqlList.size() != 0)
	{
		idFriend = atoi(sqlList.front().c_str());
		sqlList.pop_front();
		//判断其好友是否在线,若在线则发送，否则不发送
		if (mpIdToSocket.count(idFriend) > 0)
		{
			//好友在线
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

	//查询当前id拥有者的所有好友
	sprintf_s(sqlBuf, "select idB from t_friend where idA = %d;", id);
	if (!m_sql.SelectMySql(sqlBuf, 1, sqlList))
	{
		//查询出错
		cout << "select error:  " << sqlBuf << endl;
		return;
	}
	int idFriend = 0;
	while (sqlList.size() != 0)
	{
		idFriend = atoi(sqlList.front().c_str());
		sqlList.pop_front();
		//判断其好友是否在线,若在线则发送，否则不发送
		if (mpIdToSocket.count(idFriend) > 0)
		{
			//好友在线
			m_pMediator->SendData(mpIdToSocket[idFriend], (char*)&rq, sizeof(rq));
		}
	}
}
