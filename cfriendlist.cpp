#include "cfriendlist.h"
#include "ui_cfriendlist.h"
#include <QLabel>
#include <QDebug>
CFriendList::CFriendList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFriendList)
{
    OnlineNum=0;
    OfflineNum=0;

    ui->setupUi(this);
    //给层new对象
       m_layoutOnline = new QVBoxLayout;
       m_layoutOffline = new QVBoxLayout;
       //设置小控件距离大控件上下左右的间距
       m_layoutOnline->setContentsMargins(0, 0, 0, 0);
       m_layoutOffline->setContentsMargins(0, 0, 0, 0);

       //将布局设置到窗口上
       ui->wdg_online->setLayout(m_layoutOnline);
       ui->wdg_offline->setLayout(m_layoutOffline);


}

CFriendList::~CFriendList()
{
    delete ui;
    //回收每个好友窗口
    for(auto ite=mapItToFriendItem.begin();ite!=mapItToFriendItem.end();)
    {
        CFriendItem* item=*ite;
        item->hide();
        delete item;
        ite=mapItToFriendItem.erase(ite);
    }
    delete m_layoutOnline;
    delete m_layoutOffline;
}


//更新好友信息
void CFriendList::slot_updataFriendinfo(int id, QString name, int iconid, QString feeling, int status)
{
    //首先判断该好友信息是否已存在
    if(mapItToFriendItem.count(id)>0)
    {
        //如果已存在，则只更新状态
        CFriendItem* item=mapItToFriendItem[id];
        //查看其原状态
        if(item->status == STATUS_ONLINE)
        {
            OnlineNum--;
        }
        else
        {
            OfflineNum--;
        }
        item->updataStatus(status);

        //并且将其从原窗口转移到新窗口中
        item->setParent(NULL);   //将其父窗口设为空即从原窗口移除了（移除不是删除）

        if(status == STATUS_ONLINE)
        {
            //添加到在线窗口
            m_layoutOnline->addWidget(item);
            OnlineNum++;
            ui->sca_inOnline->setFixedHeight(OnlineNum * (item->height()) + 20);
            ui->sca_inOffline->setFixedHeight(OfflineNum * (item->height()) + 20);
        }
        else
        {
            //添加到离线窗口
            m_layoutOffline->addWidget(item);
            OfflineNum++;
            qDebug()<<item->sizeHint().height();
            ui->sca_inOnline->setFixedHeight(OnlineNum * (item->height()) + 20);   //改变scrollArea内部窗口的大小，以适用所有好友窗口
            ui->sca_inOffline->setFixedHeight(OfflineNum * (item->height()) + 20);
        }

    }
    else   //否则创建新的好友对象，并添加到map中保存
    {

        CFriendItem* item=new CFriendItem(this);

        //绑定信号和槽函数
        QObject::connect(&(item->dialog),SIGNAL(SIG_sendChatMessage(int,QString)), \
                         this,SLOT(slot_sendChatMessage(int ,QString )));

        item->updateInfo(status,id,name,iconid,feeling);
        //item->setVisible(true);
           // item->show();
        //如果是在线状态，就添加到在线窗口，否则添加到离线窗口
        if(status == STATUS_ONLINE)
        {

            m_layoutOnline->addWidget(item);
            OnlineNum++;
        }
        else
        {
            m_layoutOffline->addWidget(item);
            OfflineNum++;
        }
        mapItToFriendItem[id]=item;
        ui->sca_inOnline->setFixedHeight(OnlineNum * (item->height()) + 20); //改变scrollArea内部窗口的大小，以适用所有好友窗口
        ui->sca_inOffline->setFixedHeight(OfflineNum * (item->height()) + 20);

    }
}

//更新自己的信息
void CFriendList::slot_updataUserInfo(QString name, QString feeling)
{
    ui->lb_name->setText(name);
    ui->lb_signed->setText(feeling);
    QImage image = QImage(":/tx/0.png");
    ui->lb_icon->setPixmap(QPixmap::fromImage(image));
}


//重写退出事件
void CFriendList::closeEvent(QCloseEvent *event)
{

    //弹出一个窗口询问用户是否关闭
    if(QMessageBox::Yes == QMessageBox::information(this,"提示","是否确认退出？",QMessageBox::Yes | QMessageBox::No))
    {
        //确认退出，发送信号通知kernel，开始回收资源执行退出函数
        Q_EMIT SIG_exit();
    }
    else
    {
        //否则忽视掉这个事件
        event->ignore();
        //关于事件，还需要仔细学习
    }
}

void CFriendList::dealChatRq(int id, QString text)
{
    if(mapItToFriendItem.count(id) > 0)
    {
        //如果存在这个好友
        mapItToFriendItem[id]->dialog.dealChatRq(text);
        //并显示这个聊天窗口
        mapItToFriendItem[id]->dialog.showNormal();
    }
}

void CFriendList::slot_sendChatMessage(int id, QString text)
{
    Q_EMIT SIG_sendChatMessage( id,text);
}

void CFriendList::on_pb_addFriend_clicked()
{
    //弹出窗口让用户输入对方手机号进行添加
    QString name=QInputDialog::getText(this,"添加好友","请输入好友昵称");
    //判断是否为空
    QString copyName(name);
    if(copyName.remove(" ") == "")
    {
        QMessageBox::information(this,"提示","昵称不能为空！");
        return;
    }

    //判断该name是否已经在好友列表里面
    for(auto ite: mapItToFriendItem)
    {
        if(ite->name == name)
        {
            //如果当前添加好友已经在好友列表里面，则弹出消息框提示对方已经是用户的好友
            QMessageBox::information(this,"提示","请勿重复添加好友");
            return;
        }
    }
    //发送信号通知kernel让其发送添加好友请求
    Q_EMIT SIG_addFriend(name);
}
