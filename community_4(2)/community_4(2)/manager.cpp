#include "manager.h"
#include "ui_manager.h"
#include <QSqlError>
#include <QDebug>
#include<QMessageBox>
#include"database.h"

Manager::Manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Manager)
{
    ui->setupUi(this);

     connect(ui->insert, &QPushButton::clicked, this, &Manager::on_insert_clicked);//插入

     connect(ui->Delete, &QPushButton::clicked, this, &Manager::on_delete_clicked);//删除

     connect(ui->query, &QPushButton::clicked, this, &Manager::on_query_clicked);//查询模块（人事信息）

    connect(ui->pushButton_15, &QPushButton::clicked, this, &Manager::on_pushButton_15_clicked);
     //查询（出勤）
    model = new QStandardItemModel(0, 3, this);
       model->setHorizontalHeaderLabels({"姓名", "账号","上班","早打卡","下班","晚打卡"});
    ui->tableView_2->setModel(model);
    //人事信息（ALL）
    model1 = new QStandardItemModel(0,5,this);
    model1->setHorizontalHeaderLabels({"身份","姓名","身份证","手机号","住址"});
    ui->tableView->setModel(model1);
    //请假人员信息
    model2 = new QStandardItemModel(0,5,this);
    ui->tableView_4->setModel(model2);
    refreshLeaveTable();

    // 初始化投诉信息模型
    complaintModel = new QStandardItemModel(0, 3, this);
    complaintModel->setHorizontalHeaderLabels({"姓名", "电话", "投诉内容"});
    ui->tableView_complaint->setModel(complaintModel); // 假设你有一个名为 tableView_complaint 的表格视图

    // 连接查询投诉信息的按钮
    connect(ui->queryComplaintButton, &QPushButton::clicked, this, &Manager::on_queryComplaint_clicked);

    // 首次加载时显示投诉信息
    on_queryComplaint_clicked();
  //------------------------------月度收支模块------------------------
    // 初始化社区费用模型
        communityCostModel = new QStandardItemModel(0, 8, this);
        communityCostModel->setHorizontalHeaderLabels({
            "表名", "缴费日期", "费用类型", "费用金额",
            "业主姓名", "联系电话", "车位ID", "缴费状态"
        });
        ui->tableView_communityCost->setModel(communityCostModel);

        // 初始化停车费模型
        parkingModel = new QStandardItemModel(0, 8, this);
        parkingModel->setHorizontalHeaderLabels({
            "表名", "缴费日期", "费用类型", "费用金额",
            "业主姓名", "联系电话", "车位ID", "缴费状态"
        });
        ui->tableView_parking->setModel(parkingModel);

        // 可选：设置表格样式
        ui->tableView_communityCost->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableView_parking->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//链接查询物业费，车位费按钮
        connect(ui->monthlyIncomeQuery, &QPushButton::clicked, this, &Manager::on_monthlyIncomeQuery_clicked);


}

Manager::~Manager()
{
    delete ui;
}
//--------------------------------------------------------------人事管理模块------------------------------------------------------------------------
//------------------------------------------------绑定插入按钮（注册）-----------------------------------------
void Manager::on_insert_clicked()
{
    // 获取输入值（修复ID和IDCard混淆问题）
    QString account = ui->accountEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();
    QString role = ui->roleBox->currentText().trimmed();
    QString phone = ui->phoneEdit->text().trimmed();       // 手机号
    QString address = ui->addressEdit->text().trimmed();       // 登录账号
    QString idcard = ui->idcardEdit->text().trimmed(); // 身份证
    QString name = ui->nameEdit->text().trimmed();

    // 检查必要字段
    if (account.isEmpty() || password.isEmpty() || address.isEmpty() || name.isEmpty()|| phone.isEmpty()|| idcard.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "必填字段不能为空！");
        return;
    }

    // 执行插入
    database::LoginResult result = db.validLogin(account, password, address, role, phone, idcard, name);

    // 处理结果
    if (result.success) {
        QMessageBox::information(this, "成功", "用户添加成功！");
        // 可选：清空输入框
        ui->accountEdit->clear();
        ui->passwordEdit->clear();
        ui->phoneEdit->clear();
        ui->addressEdit->clear();
        ui->idcardEdit->clear();
        ui->nameEdit->clear();
    } else {
        QMessageBox::warning(this, "失败", result.errorMessage);
    }
}
//------------------------------------------------绑定删除按钮（删除）------------------------------------------------
void Manager::on_delete_clicked()
{
    // 获取输入值（修复ID和IDCard混淆问题）
    QString account = ui->accountEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();
    QString role = ui->roleBox->currentText().trimmed();
    QString phone = ui->phoneEdit->text().trimmed();       // 手机号
    QString address = ui->addressEdit->text().trimmed();       // 登录账号
    QString idcard = ui->idcardEdit->text().trimmed(); // 身份证
    QString name = ui->nameEdit->text().trimmed();

    // 检查必要字段
    if (account.isEmpty() || password.isEmpty() || address.isEmpty() || name.isEmpty()|| phone.isEmpty()|| idcard.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "必填字段不能为空！");
        return;
    }

    // 执行删除
    database::LoginResult result = db.validdelete(account, password, address, role, phone, idcard, name);

    // 处理结果
    if (result.success) {
        QMessageBox::information(this, "成功", "用户删除成功！");
        // 可选：清空输入框
        ui->accountEdit->clear();
        ui->passwordEdit->clear();
        ui->phoneEdit->clear();
        ui->addressEdit->clear();
        ui->idcardEdit->clear();
        ui->nameEdit->clear();
        // ...其他字段清空...
    } else {
        QMessageBox::warning(this, "失败", result.errorMessage);
    }
}
//--------------------------------------------------------------人员信息查询模块----------------------------------------------------------------------------000000000000000000000000000000000000000000000000000000000000000000000000000000000000
void Manager::on_query_clicked()
{
    QString name = ui->nameEdit_query->text().trimmed();//name接收

    // 检查必要字段
    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "用户名不能为空！");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE name=?");
    query.addBindValue(name);

    if (!query.exec()) {
        QMessageBox::critical(this, "查询失败", "数据库查询出错：" + query.lastError().text());
        return;
    }

    if (query.next()) { // 确保查询返回有效记录
        QMessageBox::information(this, "查询成功", "用户信息查询成功！");

        ui->accountEdit_query->setText(query.value("account").toString());
        ui->passwordEdit_query->setText(query.value("password").toString()); // 建议使用掩码显示
        ui->phoneEdit_query->setText(query.value("phone").toString());
        ui->addressEdit_query->setText(query.value("address").toString());
        ui->idcardEdit_query->setText(query.value("idcard").toString());
        ui->roleEdit_query->setText(query.value("role").toString());
    } else {
        QMessageBox::warning(this, "查询失败", "未找到该用户名对应的用户！");
    }
}

//-----------------------------------------------------------------出勤管理模块-----------------------------------------------------------------------------------
//------------------------------------------------------月度出勤------------------------------------------------------------------------------
//---------------------------------------单个查询---------------------------------------------
void Manager::on_pushButton_7_clicked()
{
    QRegularExpression re("[,;]");
    QString chuqingzhe = ui->lineEdit_5->text().trimmed();
    if (chuqingzhe.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "出勤人员查询不能为空！");
        return;
    }
    if(re.match(chuqingzhe).hasMatch()){
        QMessageBox::warning(this, "输入错误", "出勤人员查询不能为多人!");
        return;
    }
    QSqlQuery cha;
    cha.prepare("SELECT 姓名, 账号, 上班, 早打卡, 下班, 晚打卡 FROM dakabiao WHERE 姓名 = ?");
    cha.addBindValue(chuqingzhe);
    bool jiancha = false;

    model->removeRows(0, model->rowCount());
    if (cha.exec())
    {
        while(cha.next())
        {
            QList<QStandardItem*>rowItems;
            rowItems.append(new QStandardItem(cha.value(0).toString()));
            rowItems.append(new QStandardItem(cha.value(1).toString()));
            rowItems.append(new QStandardItem(cha.value(2).toString()));
            rowItems.append(new QStandardItem(cha.value(3).toString()));
            rowItems.append(new QStandardItem(cha.value(4).toString()));
            rowItems.append(new QStandardItem(cha.value(5).toString()));
            rowItems.append(new QStandardItem(cha.value(6).toString()));
            model->appendRow(rowItems);
            jiancha = true;
        }
        if(!jiancha) {
            QMessageBox::warning(this,"未查询到","未查询到相应人员");
        }
    }
}
//---------------------------------------全部查询---------------------------------------------
//【分隔符,;】
void Manager::on_pushButton_6_clicked()
{
    QRegularExpression re("[,;]");
    QString chuqingzhe = ui->lineEdit_5->text().trimmed();
    if (chuqingzhe.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "全部出勤人员查询不能为空！(查询格式：name,name...)");
        return;
    }
    model->removeRows(0, model->rowCount());
    if(re.match(chuqingzhe).hasMatch()){
        QStringList parts = chuqingzhe.split(re);
        for (const QString & ren : parts) {
            QSqlQuery cha;
            cha.prepare("SELECT 姓名, 账号, 上班, 早打卡, 下班, 晚打卡 FROM dakabiao WHERE 姓名 = ?");
            cha.addBindValue(ren.trimmed());
            bool jiancha = false;

            if (cha.exec())
            {
                while(cha.next())
                {
                    QList<QStandardItem*>rowItems;
                    rowItems.append(new QStandardItem(cha.value(0).toString()));
                    rowItems.append(new QStandardItem(cha.value(1).toString()));
                    rowItems.append(new QStandardItem(cha.value(2).toString()));
                    rowItems.append(new QStandardItem(cha.value(3).toString()));
                    rowItems.append(new QStandardItem(cha.value(4).toString()));
                    rowItems.append(new QStandardItem(cha.value(5).toString()));
                    rowItems.append(new QStandardItem(cha.value(6).toString()));
                    model->appendRow(rowItems);
                    jiancha = true;
                }
            }
            if(!jiancha) {
                QMessageBox::warning(this,"未查询到","未查询到相应人员");
            }
        }
    }
    else{
        QMessageBox::warning(this, "输入错误", "格式错误(查询格式：name,name...)");
        return;
    }
}
//---------------------------------------删除出勤信息--------------------------------------------
void Manager::on_pushButton_8_clicked()
{
    QRegularExpression re("[,;]");
    QString chuqingzhe = ui->lineEdit_5->text().trimmed();
    if (chuqingzhe.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "出勤人员删除不能为空！");
        return;
    }

    // 标记是否有删除操作成功
    bool deleteSuccess = false;

    if(re.match(chuqingzhe).hasMatch()){
        QStringList parts = chuqingzhe.split(re);
        for (const QString & ren : parts) {
            // 1. 查询人员是否存在
            QSqlQuery cha;
            cha.prepare("SELECT 姓名 FROM dakabiao WHERE 姓名=? ");
            cha.addBindValue(ren);

            model->removeRows(0, model->rowCount());
            bool jiancha = false;

            if (cha.exec())
            {
                while(cha.next())
                {
                    QList<QStandardItem*>rowItems;
                    rowItems.append(new QStandardItem(cha.value(0).toString()));
                    rowItems.append(new QStandardItem(cha.value(1).toString()));
                    rowItems.append(new QStandardItem(cha.value(2).toString()));
                    rowItems.append(new QStandardItem(cha.value(3).toString()));
                    rowItems.append(new QStandardItem(cha.value(4).toString()));
                    rowItems.append(new QStandardItem(cha.value(5).toString()));
                    rowItems.append(new QStandardItem(cha.value(6).toString()));
                    model->appendRow(rowItems);
                    jiancha = true;
                }
            }

            if(!jiancha) {
                QMessageBox::warning(this,"未查询到","未查询到相应人员: " + ren);
                continue;
            }

            // 2. 直接从数据库中删除出勤记录
            QSqlQuery delQuery;
            delQuery.prepare("DELETE FROM dakabiao WHERE name = ?");
            delQuery.addBindValue(ren);

            if (delQuery.exec()) {
                deleteSuccess = true;
                QMessageBox::information(this, "成功", "已删除 " + ren + " 的出勤记录");
            } else {
                QMessageBox::warning(this, "删除失败", "无法删除 " + ren + " 的出勤记录: " + delQuery.lastError().text());
            }
        }
    }
    else{
        // 单人员处理逻辑
        QSqlQuery cha;
        cha.prepare("SELECT 姓名, FROM kaoqingbiao WHERE 姓名=? ");
        cha.addBindValue(chuqingzhe);

        model->removeRows(0, model->rowCount());
        bool jiancha = false;

        if (cha.exec())
        {
            while(cha.next())
            {
                QList<QStandardItem*>rowItems;
                rowItems.append(new QStandardItem(cha.value(0).toString()));
                rowItems.append(new QStandardItem(cha.value(1).toString()));
                rowItems.append(new QStandardItem(cha.value(2).toString()));
                rowItems.append(new QStandardItem(cha.value(3).toString()));
                rowItems.append(new QStandardItem(cha.value(4).toString()));
                rowItems.append(new QStandardItem(cha.value(5).toString()));
                rowItems.append(new QStandardItem(cha.value(6).toString()));
                model->appendRow(rowItems);
                jiancha = true;
            }

            if(!jiancha) {
                QMessageBox::warning(this,"未查询到","未查询到相应人员");
                return;
            }
        }

        // 直接从数据库中删除出勤记录
        QSqlQuery delQuery;
        delQuery.prepare("DELETE FROM dakabiao WHERE 姓名 = ?");
        delQuery.addBindValue(chuqingzhe);

        if (delQuery.exec()) {
            deleteSuccess = true;
            QMessageBox::information(this, "成功", "已删除 " + chuqingzhe + " 的出勤记录");
        } else {
            QMessageBox::warning(this, "删除失败", "无法删除 " + chuqingzhe + " 的出勤记录: " + delQuery.lastError().text());
        }
    }

    if (deleteSuccess) {
        // 刷新数据显示
        /*refreshAttendanceTable(); */// 建议添加此函数来刷新表格
    }
}
//--------------------------------------------------------------假条审批模块------------------------------------------------------------------------
// 刷新表格的函数（简化版）
void Manager::refreshLeaveTable()
{
    model2->clear();
    //请假人员信息
    model2->setHorizontalHeaderLabels({"姓名","电话","请假审批状态","请假时间","原因"});
    QSqlQuery query("SELECT name,phone,status,date,reason FROM time_off");
    while (query.next()) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(query.value(0).toString()));
        row.append(new QStandardItem(query.value(1).toString()));
        row.append(new QStandardItem(query.value(2).toString()));
        row.append(new QStandardItem(query.value(3).toString()));
        row.append(new QStandardItem(query.value(4).toString()));
        model2->appendRow(row);
    }
}
void Manager::on_pushButton_14_clicked()
{
    QRegularExpression re("[,;]");
    QString chuqingzhe = ui->lineEdit_14->text().trimmed();
    qDebug() << "Input text:" << chuqingzhe;
    if (chuqingzhe.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请假人员查询不能为空！");
        return;
    }
    model2->removeRows(0, model2->rowCount());
//--------------------------------------------------------------多人员查询
    if(re.match(chuqingzhe).hasMatch()){
        QStringList parts = chuqingzhe.split(re);
        for (const QString & ren : parts) {
            // 1. 查询人员是否存在
            QSqlQuery cha;
            cha.prepare("SELECT name,phone,status,date,reason FROM time_off WHERE name=? ");
            cha.addBindValue(ren);

            bool jiancha = false;

            if (cha.exec())
            {
                while(cha.next())
                {
                    QList<QStandardItem*>rowItems;
                    rowItems.append(new QStandardItem(cha.value(0).toString()));
                    rowItems.append(new QStandardItem(cha.value(1).toString()));
                    rowItems.append(new QStandardItem(cha.value(2).toString()));
                    rowItems.append(new QStandardItem(cha.value(3).toString()));
                    rowItems.append(new QStandardItem(cha.value(4).toString()));
                    model2->appendRow(rowItems);
                    jiancha = true;
                }
            }


            if(!jiancha) {
                QMessageBox::warning(this,"未查询到","未查询到相应人员: " + ren);
                continue;
            }

        }


    }else{
//--------------------------------------------------------------单人员查询
        QSqlQuery cha;
        cha.prepare("SELECT name,phone,status,date,reason FROM time_off WHERE name=? ");
        cha.addBindValue(chuqingzhe);

        model2->removeRows(0, model2->rowCount());
        bool jiancha = false;

        if (cha.exec())
        {
            while(cha.next())
            {
                QList<QStandardItem*>rowItems;
                rowItems.append(new QStandardItem(cha.value(0).toString()));
                rowItems.append(new QStandardItem(cha.value(1).toString()));
                rowItems.append(new QStandardItem(cha.value(2).toString()));
                rowItems.append(new QStandardItem(cha.value(3).toString()));
                rowItems.append(new QStandardItem(cha.value(4).toString()));
                model2->appendRow(rowItems);
                jiancha = true;
            }

            if(!jiancha) {
                QMessageBox::warning(this,"未查询到","未查询到相应人员");
                return;
            }
        }
    }
}
//--------------------------------------------------------------不批准
void Manager::on_pushButton_17_clicked()
{


    QModelIndexList selectedIndexes = ui->tableView_4->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要审批的假条");
        return;
    }

    // 遍历选中的行
    for (const QModelIndex& index : selectedIndexes) {
        int row = index.row();
        // 获取选中的姓名并去除前后空格（处理格式问题）
        QString name = model2->data(model2->index(row, 0)).toString().trimmed();
        qDebug() << "选中的姓名（处理后）:" << name;

        // 先查询数据库中该姓名的实际记录，验证姓名和状态
        QSqlDatabase db = database::getDatabase();
          QSqlQuery query(db);
        query.prepare("SELECT name, status FROM time_off WHERE TRIM(name)=?");
        query.addBindValue(name); // 精确匹配处理后的姓名
        if (query.exec()) {
            if (query.next()) {
                // 数据库中实际存储的姓名和状态
                QString dbName = query.value("name").toString();
                QString dbStatus = query.value("status").toString();
                qDebug() << "数据库中实际姓名:" << dbName << "，实际状态:" << dbStatus;

                // 验证状态是否为"待审批"（与数据库实际状态匹配）
                if (dbStatus != "待审批") {
                    QMessageBox::warning(this, "提示", "该假条状态不是'待审批'，无法操作");
                    continue; // 跳过当前行，处理下一行
                }
            } else {
                // 未找到该姓名的记录
                qDebug() << "数据库中无姓名为" << name << "的记录";
                QMessageBox::warning(this, "提示", "数据库中不存在该员工的假条记录");
                continue;
            }
        } else {
            // 查询失败
            qDebug() << "查询姓名失败:" << query.lastError().text();
            QMessageBox::warning(this, "错误", "查询失败：" + query.lastError().text());
            continue;
        }

        // 执行"不批准"更新操作（状态改为"批准"，条件匹配"待审批"）
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE time_off SET status='已批准' WHERE TRIM(name)=? AND status=?");
        updateQuery.addBindValue(name); // 绑定处理后的姓名
        updateQuery.addBindValue("待审批"); // 仅更新状态为"待审批"的记录

        bool execResult = updateQuery.exec();
        qDebug() << "不批准更新结果:" << execResult;
        if (!execResult) {
            // SQL执行失败
            qDebug() << "更新错误详情:" << updateQuery.lastError().text();
            QMessageBox::warning(this, "错误", "批准操作失败：" + updateQuery.lastError().text());
            return;
        }

        // 检查是否有记录被更新
        int affectedRows = updateQuery.numRowsAffected();
        qDebug() << "批准操作影响行数:" << affectedRows;
        if (affectedRows == 0) {
            QMessageBox::information(this, "提示", "未找到符合条件的假条（姓名或状态不匹配）");
        } else {
            QMessageBox::information(this, "成功", "已标记为已批准！");
        }
    }

    // 刷新表格数据，保持显示与数据库同步
    refreshLeaveTable();
}

//--------------------------------------------------------------批准
void Manager::on_pushButton_15_clicked()
{
    qDebug() << "Button clicked (不批准操作)";

    QModelIndexList selectedIndexes = ui->tableView_4->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要审批的假条");
        return;
    }

    // 遍历选中的行
    for (const QModelIndex& index : selectedIndexes) {
        int row = index.row();
        // 获取选中的姓名并去除前后空格（处理格式问题）
        QString name = model2->data(model2->index(row, 0)).toString().trimmed();
        qDebug() << "选中的姓名（处理后）:" << name;

        // 先查询数据库中该姓名的实际记录，验证姓名和状态
        QSqlDatabase db = database::getDatabase();
          QSqlQuery query(db);
        query.prepare("SELECT name, status FROM time_off WHERE TRIM(name)=?");
        query.addBindValue(name); // 精确匹配处理后的姓名
        if (query.exec()) {
            if (query.next()) {
                // 数据库中实际存储的姓名和状态
                QString dbName = query.value("name").toString();
                QString dbStatus = query.value("status").toString();
                qDebug() << "数据库中实际姓名:" << dbName << "，实际状态:" << dbStatus;

                // 验证状态是否为"待审批"（与数据库实际状态匹配）
                if (dbStatus != "待审批") {
                    QMessageBox::warning(this, "提示", "该假条状态不是'待审批'，无法操作");
                    continue; // 跳过当前行，处理下一行
                }
            } else {
                // 未找到该姓名的记录
                qDebug() << "数据库中无姓名为" << name << "的记录";
                QMessageBox::warning(this, "提示", "数据库中不存在该员工的假条记录");
                continue;
            }
        } else {
            // 查询失败
            qDebug() << "查询姓名失败:" << query.lastError().text();
            QMessageBox::warning(this, "错误", "查询失败：" + query.lastError().text());
            continue;
        }

        // 执行"不批准"更新操作（状态改为"不批准"，条件匹配"待审批"）
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE time_off SET status='不批准' WHERE TRIM(name)=? AND status=?");
        updateQuery.addBindValue(name); // 绑定处理后的姓名
        updateQuery.addBindValue("待审批"); // 仅更新状态为"待审批"的记录

        bool execResult = updateQuery.exec();
        qDebug() << "不批准更新结果:" << execResult;
        if (!execResult) {
            // SQL执行失败
            qDebug() << "更新错误详情:" << updateQuery.lastError().text();
            QMessageBox::warning(this, "错误", "不批准操作失败：" + updateQuery.lastError().text());
            return;
        }

        // 检查是否有记录被更新
        int affectedRows = updateQuery.numRowsAffected();
        qDebug() << "不批准操作影响行数:" << affectedRows;
        if (affectedRows == 0) {
            QMessageBox::information(this, "提示", "未找到符合条件的假条（姓名或状态不匹配）");
        } else {
            QMessageBox::information(this, "成功", "已标记为不批准！");
        }
    }

    // 刷新表格数据，保持显示与数据库同步
    refreshLeaveTable();
}

//------------------------------------------------------------人事信息模块（ALL）[显示所有物业人员]----------------------------------------------------------------
void Manager::on_pushButton_5_clicked()
{
    QSqlQuery query ;
    model1->removeRows(0, model1->rowCount());
    if (query.exec("SELECT role,name,idcard,phone,address FROM users"))
    {
        while(query.next())
        {
            QList<QStandardItem*>rowItems;
            rowItems.append(new QStandardItem(query.value(0).toString()));
            rowItems.append(new QStandardItem(query.value(1).toString()));
            rowItems.append(new QStandardItem(query.value(2).toString()));
            rowItems.append(new QStandardItem(query.value(3).toString()));
            rowItems.append(new QStandardItem(query.value(4).toString()));
            model1->appendRow(rowItems);
        }
    }
}
//------------------------------------------------------------系统设置模块（初始化）------------------------------------------------------------------------------
void Manager::on_pushButton_16_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("危险操作确认");
    msgBox.setText("当你点击确定按钮时，系统内所有的数据将会被清空，请谨慎选择!!");
    QPushButton *confirm = msgBox.addButton("确定",QMessageBox::ActionRole);
    QPushButton *cancel = msgBox.addButton("取消",QMessageBox::RejectRole);
    msgBox.exec();
    if(msgBox.clickedButton() == confirm ){
        QSqlQuery query ;
        if(query.exec("DELETE FROM users"))
        {
            model1->removeRows(0,model1->rowCount());
            QMessageBox::information(this,"操作完成","数据已经成功清空！");
        }
    }
    if(msgBox.clickedButton() == cancel ){
        return;
    }
}

//------------------------------------------------------------业主投诉模块------------------------------------------------------------------------------
// 查询投诉信息的槽函数
void Manager::on_queryComplaint_clicked()
{
    QSqlQuery query;
    query.prepare("SELECT name, phone, complaint FROM users_complaint");

    if (!query.exec()) {
        QMessageBox::critical(this, "查询失败", "数据库查询出错：" + query.lastError().text());
        return;
    }

    // 清空表格
    complaintModel->removeRows(0, complaintModel->rowCount());

    while (query.next()) {
        QList<QStandardItem*> rowItems;
        rowItems.append(new QStandardItem(query.value("name").toString()));
        rowItems.append(new QStandardItem(query.value("phone").toString()));
        rowItems.append(new QStandardItem(query.value("complaint").toString()));
        complaintModel->appendRow(rowItems);
    }
}

//------------------------------------------------------------月度收支模块------------------------------------------------------------------------------
void Manager::on_monthlyIncomeQuery_clicked()
{
    // 获取用户输入的年月
    QString yearMonth = ui->yearMonthEdit->text().trimmed();
    if (yearMonth.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请输入年月！");
        return;
    }

    // 清空之前的查询结果
    communityCostModel->removeRows(0, communityCostModel->rowCount());
    parkingModel->removeRows(0, parkingModel->rowCount());

    double totalIncome = 0.0; // 初始化总收入

    // 查询 community_cost 表
    QSqlQuery costQuery(database::getDatabase());
    costQuery.prepare(R"(
        SELECT
            name AS name,
            phone AS phone,
            community_cost AS cost,
            community_cost_date AS date,
            community_cost_y_m AS year_month,
            community_cost_status AS status
        FROM community_cost
        WHERE community_cost_status = '已缴费'
          AND community_cost_y_m LIKE :yearMonth
    )");
    costQuery.bindValue(":yearMonth", yearMonth + "%");

    if (costQuery.exec()) {
        while (costQuery.next()) {
            QList<QStandardItem*> rowItems;
            rowItems.append(new QStandardItem("物业费表"));
            rowItems.append(new QStandardItem(costQuery.value("date").toString()));
            rowItems.append(new QStandardItem("物业费"));
            double cost = costQuery.value("cost").toDouble();
            rowItems.append(new QStandardItem(QString::number(cost, 'f', 2)));
            rowItems.append(new QStandardItem(costQuery.value("name").toString()));
            rowItems.append(new QStandardItem(costQuery.value("phone").toString()));
            rowItems.append(new QStandardItem(""));
            rowItems.append(new QStandardItem(costQuery.value("status").toString()));

            communityCostModel->appendRow(rowItems);
            totalIncome += cost; // 累加社区费用到总收入
        }
    } else {
        qDebug() << "社区费用查询错误:" << costQuery.lastError().text();
    }

    // 查询 parking 表
    QSqlQuery parkingQuery(database::getDatabase());
    parkingQuery.prepare(R"(
        SELECT
            name,
            phone,
            parking_id,
            cost,
            date
        FROM parking
        WHERE 缴费情况 = '已缴费'
          AND date LIKE :yearMonth
    )");
    parkingQuery.bindValue(":yearMonth", yearMonth + "%");

    if (parkingQuery.exec()) {
        while (parkingQuery.next()) {
            QList<QStandardItem*> rowItems;
            rowItems.append(new QStandardItem("停车费表"));
            rowItems.append(new QStandardItem(parkingQuery.value("date").toString()));
            rowItems.append(new QStandardItem("停车费"));
            double cost = parkingQuery.value("cost").toDouble();
            rowItems.append(new QStandardItem(QString::number(cost, 'f', 2)));
            rowItems.append(new QStandardItem(parkingQuery.value("name").toString()));
            rowItems.append(new QStandardItem(parkingQuery.value("phone").toString()));
            rowItems.append(new QStandardItem(parkingQuery.value("parking_id").toString()));
            rowItems.append(new QStandardItem("已缴费"));

            parkingModel->appendRow(rowItems);
            totalIncome += cost; // 累加停车费用到总收入
        }
    } else {
        qDebug() << "停车费查询错误:" << parkingQuery.lastError().text();
    }

    // 在QLineEdit中显示总收入（假设控件名为 totalIncomeEdit）
        ui->totalIncomeEdit->setText(QString::number(totalIncome, 'f', 2));

        // 可选：设置为只读模式，防止用户修改
        ui->totalIncomeEdit->setReadOnly(true);
}
