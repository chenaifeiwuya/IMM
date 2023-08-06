#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void SIG_registerRq(QString tel,QString name,QString password);
    void SIG_loginRq(QString tel,QString password);
private slots:
    void on_pb_register_clicked();


    void on_pb_login_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
