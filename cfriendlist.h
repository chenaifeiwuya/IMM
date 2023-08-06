#ifndef CFRIENDLIST_H
#define CFRIENDLIST_H

#include "cfrienditem.h"
#include <QWidget>
#include <QLayout>
#include <QMap>
#include <QMessageBox>
#include <QCloseEvent>
#include <QInputDialog>
#include "pakdef.h"

namespace Ui {
class CFriendList;
}

class CFriendList : public QWidget
{
    Q_OBJECT

public:
    explicit CFriendList(QWidget *parent = 0);
    ~CFriendList();

signals:
    void SIG_exit();
    void SIG_sendChatMessage(int id,QString text);
    void SIG_addFriend(QString name);

public slots:
    void slot_updataFriendinfo(int id,QString name,int iconid, QString feeling, int status);
    void slot_updataUserInfo(QString name,QString feeling);   //更新用户name，  更新用户签名
    void slot_sendChatMessage(int id,QString text);

public:
     void closeEvent(QCloseEvent *event) override;
     void dealChatRq(int id,QString text);

private slots:
     void on_pb_addFriend_clicked();

private:
    Ui::CFriendList *ui;
    QVBoxLayout* m_layoutOnline;    //QVBoxLayout垂直布局的层(要在大控件中装小控件就需要布局（画布）)
    QVBoxLayout* m_layoutOffline;
    QMap<int,CFriendItem*> mapItToFriendItem;    //key值为id，实值为好友窗口对象
    int OnlineNum;   //在线人数数量
    int OfflineNum;     //离线人数数量
};

#endif // CFRIENDLIST_H
