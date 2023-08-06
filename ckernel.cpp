#include "ckernel.h"


#define BIND_PROTOCOL_PFUNC(x)  (m_pFunc[(x)-_DEF_PROTOCOL_BASE-1])

CKernel::CKernel()
{
    initMapFunc();


    m_pMediator=new tcpClientMediator;
    if(!m_pMediator->OpenNet())
    {
        exit(0);   //网络打开失败
    }
    //char *buf = "hello world!";
    //m_pMediator->SendData(0,buf,sizeof("hello world!"));
    QObject::connect(m_pMediator,SIGNAL(SIG_dealData(long,char*,int)),this,SLOT(slot_dealReadyData(long,char*,int)));

    //绑定登录注册界面类与本类的信号和槽
    QObject::connect(&m_loginWindow,SIGNAL(SIG_registerRq(QString,QString,QString)),this,SLOT(slot_registerRq(QString, QString,QString)));
    QObject::connect(&m_loginWindow,SIGNAL(SIG_loginRq(QString,QString)),this,SLOT(slot_loginRq(QString,QString)));


    //绑定好友界面类与本类的信号和槽
    QObject::connect(this,SIGNAL(SIG_updataFriendInfo(int,QString,int,QString ,int)),  \
                     &m_friendListWindow,SLOT(slot_updataFriendinfo(int,QString,int,QString ,int)));
    QObject::connect(this,SIGNAL(SIG_updataUserInfo(QString,QString )),  \
                     &m_friendListWindow,SLOT(slot_updataUserInfo(QString,QString)));
    QObject::connect(&m_friendListWindow,SIGNAL(SIG_sendChatMessage(int,QString)), \
                     this,SLOT(slot_sendChatMessage(int ,QString )));
    QObject::connect(&m_friendListWindow,SIGNAL(SIG_addFriend(QString)), \
                     this,SLOT(slot_addFriend(QString )));
    m_loginWindow.show();   //显示登录注册界面



}

CKernel::~CKernel()
{
    slot_exit();
}

//初始化
void CKernel::initMapFunc()
{
    BIND_PROTOCOL_PFUNC(_DEF_TCP_REGISTER_RS) = &CKernel::slot_dealRegisterRs;
    BIND_PROTOCOL_PFUNC(_DEF_TCP_LOGIN_RS) = &CKernel::slot_dealLoginRs;
    BIND_PROTOCOL_PFUNC(_DEF_TCP_FRIEND_INFO) = &CKernel::slot_dealFriendInfoRq;
    BIND_PROTOCOL_PFUNC(_DEF_PROTOCOL_CHAT_RQ) = &CKernel::slot_dealChatRq;
    BIND_PROTOCOL_PFUNC(_DEF_TCP_ADD_FRIEND_RQ) = &CKernel::slot_dealAddFriendRq;
    BIND_PROTOCOL_PFUNC(_DEF_TCP_ADD_FRIEND_RS) = &CKernel::slot_dealAddFriendRs;
}


void CKernel::slot_dealReadyData(long Ip, char* buf, int len)
{
    int num=*(int*)buf;
    num -= (_DEF_PROTOCOL_BASE+1);
    if(num>=0 && num<PROTOCOL_NUM)  //判断num是否合法
    {
        (this->*(m_pFunc[num]))(Ip,buf,len);
    }
    else
    {
        qDebug()<<"deal ReadyData Error!";
    }
    delete[] buf;
    return;
}


//处理注册回复
void CKernel::slot_dealRegisterRs(long Ip, char *buf, int len)
{
    STRU_TCP_REGISTER_RS* rs = (STRU_TCP_REGISTER_RS*)buf;
    qDebug()<<len<<"   "<<sizeof(STRU_TCP_REGISTER_RS)<<endl;
    if(rs->result == REGISTER_SUCCESS)
    {
        QMessageBox::about(&m_loginWindow,"提示","注册成功！");
    }
    else
    {
        QMessageBox::about(&m_loginWindow,"提示","注册失败，用户已存在!");
    }
    return ;
}

//处理登录回复
void CKernel::slot_dealLoginRs(long Ip, char *buf, int len)
{

    //取出数据判断是否登录成功
    STRU_TCP_LOGIN_RS* rs=(STRU_TCP_LOGIN_RS*)buf;
    //若登录失败，则弹出消息窗口提示用户是因为什么登录失败
    if(rs->result == USER_IS_NOTEXIST)
    {
        QMessageBox::about(&m_loginWindow,"提示","登录失败，用户不存在!");
        return;
    }
    else if(rs->result == PASSWORD_ERROR)
    {
        QMessageBox::about(&m_loginWindow,"提示","登录失败，密码错误!");
        return;
    }
    else if(rs->result == USER_IS_ONLINE)
    {
        QMessageBox::about(&m_loginWindow,"提示","登录失败，您已在别处登录，若不是您登录请尽快修改密码!");
        return;
    }
    //若登录成功


    //更新用户信息
    //name 和 feeling可能会有汉字，编码格式需要转变
    QString name=GB2312_To_utf8(string(rs->name));
    QString feeling=GB2312_To_utf8(string(rs->feeling));
        m_id = rs->userId;
        m_name = name;
    Q_EMIT SIG_updataUserInfo(name,feeling);

        //登录窗口隐藏
    m_loginWindow.hide();
        //TODO:  显示主界面
    m_friendListWindow.show();
        //TODO:  并发送一个获取好友在线消息的请求
    getFriendInfo();    //获取好友信息
}



//处理好友信息请求
//需要注意的是：处理接收到的数据的函数到时候都不会和主线程在一个线程中，又由于Qt控件只有其拥有者线程才能改变，
//但是由于中介者是通过发信号的方式来通知kernel类的，所以此时的ckernel类的槽函数并非在接收数据线程里处理，
//而是在主线程中处理，所以这里可以直接对控件进行操作

void CKernel::slot_dealFriendInfoRq(long Ip, char *buf, int len)
{
    STRU_TCP_FRIEND_INFO* rq=(STRU_TCP_FRIEND_INFO*)buf;
    //转码
    QString name=GB2312_To_utf8(string(rq->name));
    QString feeling=GB2312_To_utf8(string(rq->feeling));
    SIG_updataFriendInfo(rq->userId,name,rq->iconId,feeling,rq->status);
}

//处理聊天信息请求
void CKernel::slot_dealChatRq(long Ip, char *buf, int len)
{
    STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)buf;
    //由于发送的时候没有转换编码格式
    //接收暂时也不转编码格式
    //直接将信息传递给好友窗口
    m_friendListWindow.dealChatRq(rq->userId,QString(rq->content));
}

//处理添加好友请求
void CKernel::slot_dealAddFriendRq(long Ip, char *buf, int len)
{
    STRU_TCP_ADD_FRIEND_RQ* rq=(STRU_TCP_ADD_FRIEND_RQ*)buf;
    STRU_TCP_ADD_FRIEND_RS rs;
    rs.friendId = m_id;
    //需要转变编码格式
    string rsName = utf8_To_GB2312(m_name);
    strcpy_s(rs.friendName,rsName.c_str());
    rs.userId = rq->userId;

    QString addName = GB2312_To_utf8(string(rq->userName));
    //弹出提示信息告诉用户是谁加他的好友
    if(QMessageBox::Yes == QMessageBox::information(&m_friendListWindow,"提示",QString("%1 请求添加您为好友，是否同意？").arg(addName),\
                             QMessageBox::Yes | QMessageBox::No))
    {
        //返回同意好友请求的数据包
        rs.result = ADD_FRIEND_SUCCESS;
    }
    else
    {
        //返回不同意好友请求的数据包
        rs.result = ADD_FRIEND_FAILED;
    }
    m_pMediator->SendData(0,(char*)&rs,sizeof(rs));
    //获取好友信息请求
    Sleep(10);
    getFriendInfo();
}

//处理添加好友回复
void CKernel::slot_dealAddFriendRs(long Ip, char *buf, int len)
{
    STRU_TCP_ADD_FRIEND_RS* rs = (STRU_TCP_ADD_FRIEND_RS*)buf;
    QString friendName=GB2312_To_utf8(string(rs->friendName));
    //判断是否添加成功
    if(rs->result == ADD_FRIEND_SUCCESS)
    {
        //提示添加成功
        //并向服务器发送一个获取好友信息的请求
        QMessageBox::information(&m_friendListWindow,"提示",QString("%1 已同意您的好友请求!").arg(friendName),QMessageBox::Ok);
        getFriendInfo();
    }
    else
    {
        //否则提示添加失败
        QMessageBox::information(&m_friendListWindow,"提示",QString("%1 拒绝了您的好友请求!").arg(friendName),QMessageBox::Ok);
    }
    return;
}


//发送注册请求
void CKernel::slot_registerRq(QString tel, QString name, QString password)
{
    STRU_TCP_REGISTER_RQ rq;
    //name可能会有汉字  需要utf8 转 GB2312
    string Strname=utf8_To_GB2312(name);
    strcpy_s(rq.name,Strname.c_str());
    strcpy_s(rq.tel,tel.toStdString().c_str());
    strcpy_s(rq.password,password.toStdString().c_str());

    m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//发送登录请求
void CKernel::slot_loginRq(QString tel, QString password)
{
    STRU_TCP_LOGIN_RQ rq;
    //没有汉字，所以不用utf8转 GB2312
    strcpy_s(rq.tel,tel.toStdString().c_str());
    strcpy_s(rq.password,password.toStdString().c_str());

    m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//资源回收函数
void CKernel::slot_exit()
{
    //首先告诉服务器，自己要下线了
    STRU_TCP_OFFLINE_RQ rq;
    rq.userId = m_id;
    m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
    //然后开始回收资源
    m_loginWindow.hide();
    m_friendListWindow.hide();
    m_pMediator->CloseNet();  //关闭网络
    Sleep(20);  //等待20ms，让线程自己退出
    delete m_pMediator;      //回收资源
    exit(0);  //退出，程序结束
}

void CKernel::slot_sendChatMessage(int id, QString text)
{
    qDebug()<<__func__;
    STRU_TCP_CHAT_RQ rq;
    rq.userId = m_id;
    rq.friendId = id;
    strcpy_s(rq.content,text.toStdString().c_str());
    m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
}

//发送添加好友请求
void CKernel::slot_addFriend(QString name)
{
    //判断是否为自己的name，若是则提示不能添加自己为好友
    if(name == m_name)
    {
        QMessageBox::information(&m_friendListWindow,"提示","请勿添加自己为好友",QMessageBox::Ok);
        return ;
    }
    STRU_TCP_ADD_FRIEND_RQ rq;
    //需要转变编码格式
    string rqName = utf8_To_GB2312(name);
    strcpy_s(rq.friendName,rqName.c_str());
    rq.userId = m_id;
    string myName = utf8_To_GB2312(m_name);
    strcpy_s(rq.userName,myName.c_str());
    m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
}


//发送获取好友信息的请求
void CKernel::getFriendInfo()
{
    //发送一个空的好友信息结构体告诉服务器
    STRU_TCP_FRIEND_INFO rq;
    rq.userId = m_id;
    m_pMediator->SendData(0,(char*)&rq,sizeof(rq));

}

string CKernel::utf8_To_GB2312(const QString &utf8_)
{
    QTextCodec* gb2312code = QTextCodec::codecForName("gb2312");
    QByteArray ba=gb2312code->fromUnicode(utf8_);
    char *buf=new char[ba.size()+1];
    strcpy_s(buf,ba.size()+1,ba.data());
    string result(buf);
    delete[] buf;
    return result;
}

QString CKernel::GB2312_To_utf8(const string &GB2312)
{
    QTextCodec* gb2312code = QTextCodec::codecForName("gb2312");
    QString result=gb2312code->toUnicode(GB2312.c_str());
    return result;
}

