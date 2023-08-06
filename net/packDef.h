#pragma once

#define _DEF_UDP_PORT		(23145)
#define _DEF_MAX_SIZE		(100)
#define _DEF_CONTENT_MAX    (1024)
#define _DEF_BUFFER_SIZE    (4096)
#define _DEF_TCP_PORT       (23146)
#define _DEF_TCP_SERVER_IP  ("192.168.1.253")


//协议头
#define _DEF_PROTOCOL_BASE        (1000)

//上线请求和回复
#define _DEF_PROTOCOL_ONLINE_RQ   (_DEF_PROTOCOL_BASE + 1)
#define _DEF_PROTOCOL_ONLINE_RS   (_DEF_PROTOCOL_BASE + 2)

//聊天请求
#define _DEF_PROTOCOL_CHAT_RQ       (_DEF_PROTOCOL_BASE + 3)
//下线请求
#define _DEF_PROTOCOL_OFFLINE_RQ     (_DEF_PROTOCOL_BASE + 4)



//注册
#define _DEF_TCP_REGISTER_RQ      (_DEF_PROTOCOL_BASE + 10)
#define _DEF_TCP_REGISTER_RS      (_DEF_PROTOCOL_BASE + 11)

//登录
#define _DEF_TCP_LOGIN_RQ         (_DEF_PROTOCOL_BASE + 12)
#define _DEF_TCP_LOGIN_RS		  (_DEF_PROTOCOL_BASE + 13)

//好友信息
#define _DEF_TCP_FRIEND_INFO      (_DEF_PROTOCOL_BASE + 14)
//t添加好友
#define _DEF_TCP_ADD_FRIEND_RQ    (_DEF_PROTOCOL_BASE + 15)
#define _DEF_TCP_ADD_FRIEND_RS    (_DEF_PROTOCOL_BASE + 16)
//下线
#define _DEF_TCP_OFFLINE          (_DEF_PROTOCOL_BASE + 17)

//用于表示协议末尾以及协议数量
#define _DEF_PROTOCOL_END           (_DEF_PROTOCOL_BASE + 18)
#define PROTOCOL_NUM                (_DEF_PROTOCOL_END - _DEF_PROTOCOL_BASE - 1)

//UDP   协议结构体
//上线请求和回复
//上线请求/回复：协议头，自己的名字
struct STRU_ONLINE
{
	STRU_ONLINE() :type(_DEF_PROTOCOL_ONLINE_RQ)
	{
		memset(name, 0, _DEF_MAX_SIZE);   //初始化字符数组name为全0
	}
	int type;
	//unsigned long ip;
	char name[_DEF_MAX_SIZE];
};

//聊天请求：协议头，聊天内容
struct STRU_CHAT
{
	STRU_CHAT() :type(_DEF_PROTOCOL_CHAT_RQ)
	{
		memset(content, 0, _DEF_CONTENT_MAX);
	}
	int type;
	char content[_DEF_CONTENT_MAX];
};


//下线请求：协议头
struct STRU_OFFLINE
{
	STRU_OFFLINE() :type(_DEF_PROTOCOL_OFFLINE_RQ)
	{

	}
	int type;
};


//TCP协议请求
//注册

typedef int PackType;
// 注册请求:协议头、手机号、昵称、密码
typedef struct STRU_TCP_REGISTER_RQ {
	STRU_TCP_REGISTER_RQ() :type(_DEF_TCP_REGISTER_RQ)
	{
		memset(tel, 0, _DEF_MAX_SIZE);
		memset(name, 0, _DEF_MAX_SIZE);
		memset(password, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	char tel[_DEF_MAX_SIZE];
	char name[_DEF_MAX_SIZE];
	char password[_DEF_MAX_SIZE];
}STRU_TCP_REGISTER_RQ;

// 注册回复:协议头、注册结果(用户已经存在、注册成功)

#define REGISTER_SUCCESS (0)
#define USER_IS_EXIST (1)

typedef struct STRU_TCP_REGISTER_RS {
	STRU_TCP_REGISTER_RS() : type(_DEF_TCP_REGISTER_RS), result(REGISTER_SUCCESS)
	{

	}
	PackType type;
	int result;
}STRU_TCP_REGISTER_RS;


//登录请求：协议头
typedef struct STRU_TCP_LOGIN_RQ {
	STRU_TCP_LOGIN_RQ() :type(_DEF_TCP_LOGIN_RQ)
	{
		memset(tel, 0, _DEF_MAX_SIZE);
		memset(password, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	char tel[_DEF_MAX_SIZE];
	char password[_DEF_MAX_SIZE];
}STRU_TCP_LOGIN_RQ;

//登录回复：协议头，登录结果
#define LOGIN_SUCCESS (0)
#define PASSWORD_ERROR (1)
#define USER_IS_NOTEXIST (2)
#define USER_IS_ONLINE (3)
typedef struct STRU_TCP_LOGIN_RS {
	STRU_TCP_LOGIN_RS() :type(_DEF_TCP_LOGIN_RS), userId(0), result(LOGIN_SUCCESS)
	{
		memset(name, 0, _DEF_MAX_SIZE);
		memset(feeling, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	int userId;
	char name[_DEF_MAX_SIZE];
	char feeling[_DEF_MAX_SIZE];
	int result;
}STRU_TCP_LOGIN_RS;




//聊天请求：协议头，聊天内容，我是谁userId， 发给谁friendId
typedef struct STRU_TCP_CHAT_RQ {
	STRU_TCP_CHAT_RQ() :type(_DEF_PROTOCOL_CHAT_RQ), userId(0), friendId(0)
	{
		memset(content, 0, _DEF_CONTENT_MAX);
	}
	PackType type;
	int userId;
	int friendId;
	char content[_DEF_CONTENT_MAX];
}STRU_TCP_CHAT_RQ;



//添加好友请求：协议头
typedef struct STRU_TCP_ADD_FRIEND_RQ {
	STRU_TCP_ADD_FRIEND_RQ() : type(_DEF_TCP_ADD_FRIEND_RQ), userId(0)
	{
		memset(userName, 0, _DEF_MAX_SIZE);
		memset(friendName, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	int userId;
	char userName[_DEF_MAX_SIZE];
	char friendName[_DEF_MAX_SIZE];
}STRU_TCP_ADD_FRIEND_RQ;

//添加好友回复：协议头

#define ADD_FRIEND_SUCCESS (0)
#define ADD_FRIEND_FAILED (1)

typedef struct STRU_TCP_ADD_FRIEND_RS {
	STRU_TCP_ADD_FRIEND_RS() : type(_DEF_TCP_ADD_FRIEND_RS), userId(0)
	{

		memset(friendName, 0, _DEF_MAX_SIZE);
	}
	PackType type;
	int userId;
	int friendId;
	int result;
	char friendName[_DEF_MAX_SIZE];
}STRU_TCP_ADD_FRIEND_RS;

//下线请求：协议头
typedef struct STRU_TCP_OFFLINE_RQ {
	STRU_TCP_OFFLINE_RQ() :type(_DEF_TCP_OFFLINE), userId(0)
	{

	}
	PackType type;
	int userId;
}STRU_TCP_OFFLINE_RQ;



//好友信息：协议头， 用户id， 昵称， 签名， 头像id， 状态
//好友上线时也用这个包通知，如果包内只有状态信息则只更新状态即可

#define STATUS_ONLINE (0)
#define STATUS_OFFLINE (1)
typedef struct STRU_TCP_FRIEND_INFO {
	STRU_TCP_FRIEND_INFO() :type(_DEF_TCP_FRIEND_INFO), userId(0), iconId(0), status(STATUS_ONLINE)
	{

	}
	PackType type;
	int userId;
	int iconId;    //头像id
	int status;
	char name[_DEF_MAX_SIZE];
	char feeling[_DEF_MAX_SIZE];
}STRU_TCP_FRIEND_INFO;

