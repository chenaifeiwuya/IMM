#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>
#include <QMessageBox>
#include <QKeyEvent>

namespace Ui {
class chatDialog;
}

class chatDialog : public QWidget
{
    Q_OBJECT

public:
    explicit chatDialog(QWidget *parent = 0);
    ~chatDialog();



public:
    void setId(int id,QString name);  //设置对应的id

signals:
    void SIG_sendChatMessage(int id,QString text);

private slots:
    void on_pb_send_clicked();

public:
    void dealChatRq(QString text ,QString name="");
    void keyPressEvent(QKeyEvent* event) override;

private:
    Ui::chatDialog *ui;
    int id;  //用于记录当前聊天窗口是和谁的聊天窗口
    QString name;
};

#endif // CHATDIALOG_H
