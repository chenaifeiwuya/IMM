#ifndef CKERNEL_H
#define CKERNEL_H
#include <QObject>
#include "tcpClientMediator.h"
#include "UdpMediator.h"
#include <QDebug>
#include "pakdef.h"
#include "mainwindow.h"
#include "cfriendlist.h"
#include <QTextCodec>
#include <QMap>
#include "cfrienditem.h"

class CKernel;
typedef void (CKernel::*(pFunc))(long, char*, int);    //函数指针数组


class CKernel:public QObject
{
    Q_OBJECT
public:
    CKernel();
    ~CKernel();

signals:
    void SIG_updataFriendInfo(int,QString,int,QString ,int status);
    void SIG_updataUserInfo(QString ,QString);

    //处理服务器信息槽函数
public slots:
    void slot_dealReadyData(long Ip, char* buf, int len);
    void slot_dealRegisterRs(long Ip,char* buf, int len);
    void slot_dealLoginRs(long Ip, char* buf, int len);
    void slot_dealFriendInfoRq(long Ip,char* buf, int len);
    void slot_dealChatRq(long Ip,char* buf,int len);
    void slot_dealAddFriendRq(long Ip,char* buf, int len);
    void slot_dealAddFriendRs(long Ip, char* buf, int len);


    //处理本地信息槽函数
    void slot_registerRq(QString tel, QString name, QString password);
    void slot_loginRq(QString tel,QString password);
    void slot_exit();
    void slot_sendChatMessage(int id,QString text);
    void slot_addFriend(QString name);

public:
    void getFriendInfo();    //发送获取好友信息的请求


public:
    string utf8_To_GB2312(const QString &utf8);
    QString GB2312_To_utf8(const string& GB2312);
    void initMapFunc();
private:
    INetMediator* m_pMediator;
    MainWindow m_loginWindow;
    CFriendList m_friendListWindow;
    pFunc m_pFunc[PROTOCOL_NUM];

    int m_id;   //我的id
    QString m_name;  //我的name

};

#endif // CKERNEL_H
