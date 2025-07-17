// Community/database.cpp
#include "database.h"
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
database::database()
{
    initDatabase();
}

database::~database()
{
    if (db.isOpen()) {
        db.close();
    }
}
QSqlDatabase database::db;
//----------------------------------------数据库检查-------------------------------------------------
//初始化数据库连接
bool database::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("E:/SQliteSmart/SQlite1.db");

    if (!db.open()) {
        qDebug() << "数据库打开失败：" << db.lastError().text();
        return false;
    }
    return true;
}
//获取一个已打开的 QSqlDatabase 对象
QSqlDatabase database::getDatabase()
{
    if (!db.isOpen()) {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("E:/SQliteSmart/SQlite1.db");
        if (!db.open()) {
            qDebug() << "数据库打开失败：" << db.lastError().text();
        }
    }
    return db;
}
//----------------------------------------登录-------------------------------------------------
database::LoginResult database::validateLogin(const QString& account, const QString& password)
{
    LoginResult result;
    result.success = false;
    result.role = "";

    QSqlQuery query;
    query.prepare("SELECT role FROM users WHERE account=? AND password=?");
    query.addBindValue(account);
    query.addBindValue(password);

    if (!query.exec()) {
        result.errorMessage = "数据库查询失败：" + query.lastError().text();
        return result;
    }

    if (query.next()) {
        result.success = true;
        result.role = query.value("role").toString();
    } else {
        // 进一步判断是用户不存在还是密码错误
        QSqlQuery checkUserQuery;
        checkUserQuery.prepare("SELECT 1 FROM users WHERE account=?");
        checkUserQuery.addBindValue(account);
        if (checkUserQuery.exec() && checkUserQuery.next()) {
            result.errorMessage = "密码错误";
        } else {
            result.errorMessage = "用户不存在";
        }
    }

    return result;
}

//---------------------------------------------------------------------人事管理模块--------------------------------------------------------------------------------
//----------------------------------------注册-------------------------------------------------
database::LoginResult database::validLogin(const QString& account, const QString& password, const QString& address,
                                         const QString& role, const QString& phone, const QString& idcard,
                                         const QString& name)
{
    LoginResult result;
    result.success = false;
//账号查重
    QSqlQuery query;
    query.prepare("SELECT *FROM users WHERE account = ? ");
    query.addBindValue(account);
    if(query.exec())
    {
        if(query.next())
        {
            QMessageBox::warning(nullptr, "错误", "账号已存在:\n" + query.lastError().text());
            return result;
        }
    }
 //查重成功在插入
    query.prepare("INSERT INTO users (account,password,address,role,phone,idcard,name) VALUES (?,?,?,?,?,?,?)");
    query.addBindValue(account);
    query.addBindValue(password);
    query.addBindValue(address);
    query.addBindValue(role);
    query.addBindValue(phone);
    query.addBindValue(idcard);
    query.addBindValue(name);

    if (query.exec()) {
        if(query.numRowsAffected()>0){
        result.success = true;
        // 插入成功时显示弹窗
        QMessageBox::information(nullptr, "成功", "用户信息插入成功!");
    } else {
        result.errorMessage = "插入失败: " + query.lastError().text();
        qDebug() << "插入错误:" << query.lastError();
        // 插入失败时也显示弹窗
        QMessageBox::warning(nullptr, "错误", "用户信息插入失败:\n" + query.lastError().text());
    }

} return result;
}
//----------------------------------------删除-------------------------------------------------
database::LoginResult database::validdelete(const QString& account, const QString& password, const QString& address,const QString& role, const QString& phone, const QString& idcard,const QString& name)
{
    LoginResult result;
    result.success = false;

    QSqlQuery query;
    query.prepare("Delete From users Where account=? And password=? And address=? And role=? And phone=? And idcard=? And name=?");
    query.addBindValue(account);
    query.addBindValue(password);
    query.addBindValue(address);
    query.addBindValue(role);
    query.addBindValue(phone);
    query.addBindValue(idcard);
    query.addBindValue(name);

    if (query.exec()) {
        if(query.numRowsAffected()>0){
        result.success = true;
        // 插入成功时显示弹窗
        QMessageBox::information(nullptr, "成功", "用户信息删除成功!");
    } else {
        result.errorMessage = "删除失败: " + query.lastError().text();
        qDebug() << "删除错误:" << query.lastError();
        // 插入失败时也显示弹窗
        QMessageBox::warning(nullptr, "错误", "用户信息删除失败:\n" + query.lastError().text());
    }

} return result;
    }

