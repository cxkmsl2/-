#ifndef WORKER_H
#define WORKER_H

#include <QWidget>
#include <QStandardItem>
#include <QSqlTableModel>
#include <QTableView>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlDatabase>
namespace Ui {
class Worker;
}

class Worker : public QWidget
{
    Q_OBJECT

public:
    explicit Worker(QWidget *parent = nullptr);
    ~Worker();
    void on_pushButton_4_clicked();//Worker提交报修处理信息
    void show_repair_table();//展示repair表
    void on_queryByaddress_clicked();//按照地址查询业主

    //Worker个人中心
    void showWorkerMassage(const QString& name, const QString& phone,const QString& address);
    void setAccountAndPassword(const QString& account,const QString& password);
    //Worker请销假
    void loadDataByAccount();//申请请假

    void on_pushButton_6_clicked();//销假按钮

    void on_pushButton_5_clicked();//申请请假

    //worker打卡
    void on_pushButton_9_clicked(); //上班打卡按钮

    void on_pushButton_11_clicked();  //下班打卡按钮

    // 车位管理相关函数
      void on_queryParkingSpaces_clicked(); // 查询所有车位信息
      void on_modifyParkingSpace_clicked(); // 修改车位费用
      void on_addParkingSpace_clicked();    // 添加车位

      // 物业收费管理模块槽函数
         void on_propertyFeeSubmitBtn_clicked();

         void on_queryPropertyFee_clicked(); // 查询物业费用的槽函数
//               void loadcommunity_cost();//缴费记录

private slots:
    void on_pushButton_10_clicked();
    void on_pushButton_clicked();//缴费记录--》刷新

private:
    Ui::Worker *ui;
    QStandardItemModel* model;
    QString currentAccount;
    QString currentPassword;

    //展示请销假模块表
    QStandardItemModel* model1;
    QStandardItemModel* model2;
    QSqlTableModel *tableModel;

    // 车位管理相关模型
        QStandardItemModel* parkingModel;
    //缴费记录
    QStandardItemModel* model3;
    QStandardItemModel* model4;
        QString costkinds;
};

#endif // WORKER_H
