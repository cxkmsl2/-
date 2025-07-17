
// Community/database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

class database
{
public:
    database();
        static QSqlDatabase getDatabase();
    ~database();
    bool initDatabase();
    struct LoginResult {
        bool success;
        QString role;
        QString errorMessage;
    };
//--------------------------------------物业管理人员头文件--------------------------------------
LoginResult validateLogin(const QString& account, const QString& password);
LoginResult validLogin(const QString& account, const QString& password,const QString& address,const QString& role,const QString& phone,const QString& idcard,const QString& name);
LoginResult validdelete(const QString& account, const QString& password,const QString& address,const QString& role,const QString& phone,const QString& idcard,const QString& name);


private:
      static QSqlDatabase db;
};

#endif // DATABASE_H
