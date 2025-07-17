#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "database.h"
#include "owner.h"
#include "worker.h"
namespace Ui {
class loginwindow;
}

class loginwindow : public QDialog
{
    Q_OBJECT

public:
    explicit loginwindow(QWidget *parent = nullptr);
    ~loginwindow();

signals:
    void showAdminMain();
    void showStaffMain();
    void showOwnerMain();

private slots:
    void on_loginBtn_clicked();
    void on_exitBtn_clicked();

private:
    Ui::loginwindow *ui;
    database db;
    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    // 添加获取账号和密码的函数声明
       void getAccountAndPassword();
       QString account;
       QString password;
public slots:
   void tryon(Owner& owner);
   void show_repair(Worker& worker);
   void showWorker(Worker& worker);
};

#endif // LOGINWINDOW_H
