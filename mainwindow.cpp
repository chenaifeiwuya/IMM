#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_register_clicked()    //注册按钮
{
    //获取控件中的值
    QString tel=ui->le_tel_register->text();
    QString name=ui->le_name_register->text();
    QString password=ui->le_password_register->text();
    //tel不能有除数字以外的字符
    //name不能全为空格
    //密码不能包含空格，位数必须大于八位
    QString copy_name=name.remove(" ");
    if(copy_name.size() == 0)
    {
        QMessageBox::information(this,"提示","name不能为空",QMessageBox::Yes);
        return;
    }
    else if(name.size()>6)
    {
        QMessageBox::information(this,"提示","name长度不能大于6",QMessageBox::Yes);
        return;
    }
    else if(tel.size() != 11)
    {
        QMessageBox::information(this,"提示","请输入合法手机号",QMessageBox::Yes);
        return;
    }
    else if(password.size() != password.remove(" ").size())
    {
        QMessageBox::information(this,"提示","密码中不能有空格",QMessageBox::Yes);
        return;
    }

    Q_EMIT SIG_registerRq(tel,name,password);


}


//登录槽函数
void MainWindow::on_pb_login_clicked()
{
    //获取控件中的值
    QString tel=ui->le_tel_login->text();
    QString password=ui->le_password_login->text();
    //tel不能有除数字以外的字符
    //密码不能包含空格，位数必须大于八位

    if(tel.size() != 11)
    {
        QMessageBox::information(this,"提示","请输入合法手机号",QMessageBox::Yes);
        return;
    }
    else if(password.size() != password.remove(" ").size())
    {
        QMessageBox::information(this,"提示","密码中不能有空格",QMessageBox::Yes);
        return;
    }

    Q_EMIT SIG_loginRq(tel,password);
}
