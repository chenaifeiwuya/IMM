#ifndef CFRIENDITEM_H
#define CFRIENDITEM_H

#include <QWidget>
#include <QImage>
#include "chatdialog.h"
#include <QMouseEvent>

namespace Ui {
class CFriendItem;
}

class CFriendItem : public QWidget
{
    Q_OBJECT

public:
    explicit CFriendItem(QWidget *parent = 0);
    ~CFriendItem();
    void updateInfo(int status, int id=0,QString name="",int iconId=0,QString feeling="");
    void updataStatus(int status);
    void mouseDoubleClickEvent(QMouseEvent* event) override;  //重写鼠标双击事件
private:
    Ui::CFriendItem *ui;
public:

    int id;
    QString name;
    int iconId;
    QString feeling;
    int status;
    QImage image;  //头像
    chatDialog dialog;   //与当前用户的聊天窗口

};

#endif // CFRIENDITEM_H
