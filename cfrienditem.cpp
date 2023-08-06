#include "cfrienditem.h"
#include "ui_cfrienditem.h"

CFriendItem::CFriendItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFriendItem)
{
    ui->setupUi(this);

}

CFriendItem::~CFriendItem()
{
    delete ui;
}


//后面四个参数是有默认值的，只有状态参数没有默认值(这里暂不考虑默认值)
void CFriendItem::updateInfo(int status, int id, QString name, int iconId, QString feeling)
{
    this->status = status;
    this->id = id;
    this->name = name;
    this->iconId = iconId;
    this->feeling = feeling;

    dialog.setId(id,name);   //给聊天窗口指定id
    dialog.setWindowTitle(name);

    ui->lb_name->setText(name);
    ui->lb_signed->setText(feeling);

    //根据头像id来进行头像查找
    if(iconId >=0 && iconId<=35)        //判断头像id是否有效
    {

        //:/tx/0.png
        QString imagePath=QString(":/tx/%1.png").arg(iconId);
        image=QImage(imagePath);
        QPixmap pixmap=QPixmap::fromImage(image);
       // pixmap.scaled(40,40);     //这里缩放没用，需要去label控件那里勾选icon自适应大小
        ui->lb_icon->setPixmap(pixmap);

    }
}

void CFriendItem::updataStatus(int status)
{
    this->status = status;   //更新状态
}


//重写鼠标双击事件
void CFriendItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    dialog.showNormal();
}


