#ifndef MANAGER_H
#define MANAGER_H
#include"database.h"
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include<QLineEdit>
#include<QComboBox>
#include <QStandardItemModel>
#include <QPushButton>
#include <QStandardItem>
#include <QMessageBox>

namespace Ui {
class Manager;
}

class Manager : public QWidget
{
    Q_OBJECT

public:
    explicit Manager(QWidget *parent = nullptr);
    void refreshLeaveTable();
    ~Manager();


struct LoginResult {
    bool success;
    QString role;
    QString errorMessage;






};

void on_insert_clicked();
void on_delete_clicked();
void on_query_clicked();


private slots:

void on_pushButton_7_clicked();

void on_pushButton_6_clicked();

void on_pushButton_8_clicked();

void on_pushButton_5_clicked();

void on_pushButton_16_clicked();

//出勤管理--->假条审批
void on_pushButton_14_clicked();

void on_pushButton_17_clicked();

void on_pushButton_15_clicked();

// 查询投诉信息的槽函数
    void on_queryComplaint_clicked();

// 月度收支查询槽函数
void on_monthlyIncomeQuery_clicked();

private:
    Ui::Manager *ui;
    class database db;
    QLineEdit* accountEdit;
    QLineEdit* passwordEdit;
    QLineEdit* phoneDdit;
    QLineEdit* idcardEdit;
    QLineEdit* nameEdit;
    QComboBox* roleBox;
    QLineEdit* addressEdit;
    QLineEdit* chuqingzhe;
    QSqlQuery* pushBotton_5;
    QStandardItemModel* model;
    QStandardItemModel* model1;
    QStandardItemModel* model2;
    // 用于显示投诉信息的模型
    QStandardItemModel* complaintModel;
    QStandardItemModel* communityCostModel;  // 社区费用模型
    QStandardItemModel* parkingModel;       //停车费模型


};

#endif // MANAGER_H
