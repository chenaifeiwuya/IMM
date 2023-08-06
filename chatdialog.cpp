#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QTime>

chatDialog::chatDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chatDialog)
{
    ui->setupUi(this);

}

chatDialog::~chatDialog()
{
    delete ui;
}

void chatDialog::setId(int id,QString name)
{
    this->id = id;
    this->name = name;
}

//发送消息函数
void chatDialog::on_pb_send_clicked()
{
    QString text = ui->le_insert->toPlainText();  //获取输入框的文本信息
    if(text.remove(" ").size() == 0)
    {
        //若文本内容为空
        QMessageBox::information(this,"提示","请先输入信息!",QMessageBox::Yes);
        return;
    }
    dealChatRq(text,"我");
     ui->le_insert->setText("");  //发送消息框清空
    //如果不为空，则发信号通知ckernel有数据要发送
    Q_EMIT SIG_sendChatMessage(id,text);
}

//在控件上显示聊天信息
void chatDialog::dealChatRq(QString text, QString defaultName)   //name有一个默认值，默认值为空
{
    if(defaultName == ""){
        //显示到浏览器窗口上，格式【ip】时间...
        ui->tb_chatRecord->append(QString("【%1】 %2").arg(name).arg(QTime::currentTime().toString("hh:mm:ss")));  //append()在text edit的最后添加一个新的段落。
        ui->tb_chatRecord->append(text);   //insertPlainText()
    }
    else
    {
        //显示到浏览器窗口上，格式【ip】时间...
        ui->tb_chatRecord->append(QString("【%1】 %2").arg(defaultName).arg(QTime::currentTime().toString("hh:mm:ss")));  //append()在text edit的最后添加一个新的段落。
        ui->tb_chatRecord->append(text);   //insertPlainText()
    }
}

//重写按键按下事件
void chatDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == QKeyEvent::Enter)
    {
        on_pb_send_clicked();
    }
}
