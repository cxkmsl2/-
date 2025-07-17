#ifndef OWNER_H
#define OWNER_H

#include <QWidget>
#include <QStandardItemModel>
namespace Ui {
class Owner;
}

class Owner : public QWidget
{
    Q_OBJECT

public:
    explicit Owner(QWidget *parent = nullptr);
    ~Owner();
void tryin(const QString& name, const QString& phone,const QString& address);
void setAccountAndPassword(const QString& account,const QString& password);
void on_modifyBtn_clicked();//修改个人信息按钮
void on_repair_btn_clicked();//维修上报按钮
void on_find_btn_clicked();//维修查询
void on_judge_btn_clicked();//报修评价
void on_delpushButton_2_clicked();//注册车辆信息
void on_delpushButton_clicked();//删除车辆信息
void on_shenpushButton_clicked();//申请车位
void refresh();//刷新显示  车辆信息表  可申请车位表  我的车位表
void on_complaintSubmitBtn_clicked(); //处理投诉提交的槽函数
//缴费-》自主缴费
void showtableview_8();
void cost();

private slots:
//缴费-》缴费查询
void on_pushButton_13_clicked();

private:
    Ui::Owner *ui;
    QString currentAccount;
    QString currentPassword;
 QString   currentname;
  QString  currentphone;
    QStandardItemModel* model;
    QStandardItemModel* model1;
    QStandardItemModel* model2;
    QStandardItemModel* model3;
};

#endif // OWNER_H
