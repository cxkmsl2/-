#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>
#include "database.h"
#include  <QSqlQuery>
loginwindow::loginwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginwindow)
{
    ui->setupUi(this);
    this->setWindowTitle("智慧小区系统 - 登录");
     usernameEdit=ui->lineEditUsername;
     passwordEdit=ui->lineEditpassword;
       if (!db.initDatabase()) {
           QMessageBox::critical(this, "数据库错误", "无法连接到数据库，请检查文件是否存在！");
           this->close();
       }
}

loginwindow::~loginwindow()
{
    delete ui;
}
//------------------------------------------------------------------------------登录模块【起始窗口】------------------------------------------------------------------------
void loginwindow::on_loginBtn_clicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password =passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "用户名或密码不能为空！");
        return;
    }

    database::LoginResult result = db.validateLogin(username, password);

    if (result.success) {
        if (result.role == "物业管理人员") {
            emit showAdminMain();
        } else if (result.role == "物业工作人员") {
            emit showStaffMain();
        } else if (result.role == "业主") {
            emit showOwnerMain();
        } else {
            QMessageBox::warning(this, "角色错误", "未知用户角色，请联系管理员！");
            return;
        }
        this->hide();
    } else {
        QMessageBox::warning(this, "登录失败", result.errorMessage);
    }
}

void loginwindow::on_exitBtn_clicked(){
    this->close();
}

// 实现获取账号和密码的函数（便于后续使用）【要显式调用】
void loginwindow::getAccountAndPassword()
{
    account = usernameEdit->text().trimmed();
    password = passwordEdit->text();
}

//----------------------------------------------------------------------------OWner模块--------------------------------------------------------------
//-------------------------------------Owner个人主页----------------
void  loginwindow::tryon(Owner& owner){
    QString username = usernameEdit->text().trimmed();
    QString password =passwordEdit->text();
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE account=? AND password=?");
    query.addBindValue(username);
    query.addBindValue(password);
    //-----------------------------展示个人信息---------------------
if(query.exec()&&query.next()){
  //实现展示个人信息
 owner.tryin(query.value("name").toString(),query.value("phone").toString(),query.value("address").toString());
 //实现修改个人信息
 getAccountAndPassword();
 owner.setAccountAndPassword(account,password);
}  }

//------------------------------------------------------------------------Worker模块----------------------------------------------------------------
//------------------------------展示repair表------------------------------
void loginwindow::show_repair(Worker& worker)
{
    worker.show_repair_table();
}

//------------------------------Worker个人中心---------------------------------
void loginwindow::showWorker(Worker& worker)
{
    QString username = usernameEdit->text().trimmed();
    QString password =passwordEdit->text();
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE account=? AND password=?");
    query.addBindValue(username);
    query.addBindValue(password);

if(query.exec()&&query.next()){
  //实现展示个人信息
 worker.showWorkerMassage(query.value("name").toString(),query.value("phone").toString(),query.value("address").toString());
 //实现修改个人信息
 getAccountAndPassword();
 worker.setAccountAndPassword(account,password);
}
}

