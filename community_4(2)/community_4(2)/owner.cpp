#include "owner.h"
#include "ui_owner.h"
#include "database.h"
#include <QSqlQuery>
#include <QString>
#include<QLineEdit>
#include <QMessageBox>
#include <QSqlError>
#include <QTextEdit>
 #include <QDateTime>
#include <QSqlQueryModel>
#include <QRegularExpression>
Owner::Owner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Owner)
{
    ui->setupUi(this);
   connect(ui->modifyBtn, &QPushButton::clicked, this, &Owner::on_modifyBtn_clicked);//修改Owner个人主页
   connect(ui->repair_btn, &QPushButton::clicked, this, &Owner::on_repair_btn_clicked);//报修插入
   connect(ui->query, &QPushButton::clicked, this, &Owner::on_find_btn_clicked);//报修查询
    connect(ui->remark, &QPushButton::clicked, this, &Owner::on_judge_btn_clicked);//报修评价
    connect(ui->delpushButton_2, &QPushButton::clicked, this, &Owner::on_delpushButton_2_clicked);//注册车辆信息
    connect(ui->delpushButton, &QPushButton::clicked, this, &Owner::on_delpushButton_clicked);//删除车辆信息
    connect(ui->shenpushButton, &QPushButton::clicked, this, &Owner::on_shenpushButton_clicked);//申请车位信息
    connect(ui->complaintSubmitBtn, &QPushButton::clicked, this, &Owner::on_complaintSubmitBtn_clicked); //连接投诉提交按钮信号

    connect(ui->pushButton_2, &QPushButton::clicked,this,&Owner::refresh);//刷新刷新显示  车辆信息表  可申请车位表  我的车位表
    model = new QStandardItemModel(0, 4, this);
        model->setHorizontalHeaderLabels({"姓名","电话","车牌","品牌"});
        ui->tableView_6->setModel(model);
        model1 = new QStandardItemModel(0, 2, this);
        model1->setHorizontalHeaderLabels({"车位","车位价格"});
        ui->tableView_5->setModel(model1);
        model2 = new QStandardItemModel(0, 2, this);
        model2->setHorizontalHeaderLabels({"车位","车位价格"});
        ui->tableView_4->setModel(model2);
        //缴费->自主缴费
        connect(ui->pushButton_17, &QPushButton::clicked,this,&Owner::showtableview_8);
        connect(ui->pushButton_16, &QPushButton::clicked,this,&Owner::cost);
        model3 = new QStandardItemModel(0, 8, this);
        model3->setHorizontalHeaderLabels({"姓名","电话","物业费","物业费发布","物业费缴费状态","车位编号","车位费","车位费缴费情况"});
        ui->tableView_8->setModel(model3);

}

Owner::~Owner()
{
    delete ui;
}
//-----------------------------------获取账号-----------------------------
void Owner::setAccountAndPassword(const QString& account, const QString& password)
{
    currentAccount=account;
    currentPassword=password;
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE account=? AND password=?");
    query.addBindValue(currentAccount);
    query.addBindValue(currentPassword);
    if(query.exec()&&query.next()){
        currentname=query.value("name").toString();
        currentphone=query.value("phone").toString();
    }
}

//------------------------------------------------------------------Owner个人主页--------------------------------------------------------------------------
//---------------------------展示Owner个人主页---------------------------------
void Owner::tryin(const QString& name, const QString& phone,const QString& address  ){
    ui->lineEdit_2->setText(name);
    ui->lineEdit_5->setText(phone);
    ui->lineEdit_3->setText(address);
}
//--------------------------修改Owner个人主页----------------------------------
void Owner::on_modifyBtn_clicked()
{
    QString name = ui->lineEdit_2->text().trimmed();
    QString phone = ui->lineEdit_5->text().trimmed();
    QString address = ui->lineEdit_3->text().trimmed();

    if (name.isEmpty() || phone.isEmpty() || address.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "必填字段不能为空！");
        return;
    }
    QSqlQuery query;
    query.prepare("UPDATE users SET name = ?, phone = ?, address = ? WHERE account = ? And password= ?");
    query.addBindValue(name);
    query.addBindValue(phone);
    query.addBindValue(address);
    query.addBindValue(currentAccount);
    query.addBindValue(currentPassword);
    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            QMessageBox::information(this, "成功", "用户信息修改成功！");
        } else {
            QMessageBox::warning(this, "失败", "未找到对应的用户信息，修改失败！");
        }
    } else {
        QMessageBox::critical(this, "错误", "数据库更新出错：" + query.lastError().text());
    }
}

//---------------------------------------------------------------Owner维修管理-----------------------------------------------------------------------------
//------------------------------------------报修插入----------------------------------
void Owner::on_repair_btn_clicked()
{
    // 获取输入值（问题，姓名，电话，报修地址）
    QString trouble = ui->mendtroubleEdit->toPlainText().trimmed();//问题
    QString name_repair = ui->nameEdit_2->text().trimmed();//姓名
    QString phone_repair = ui->phoneEdit_2->text().trimmed();//电话
    QString address_repair = ui->mendaddressEdit->text().trimmed(); // 报修地址


    // 检查必要字段
    if (trouble.isEmpty() || name_repair.isEmpty() || phone_repair.isEmpty() || address_repair.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "必填字段不能为空！");
        return;
    }

    // 执行报修
    QSqlQuery query;
    query.prepare("INSERT INTO repair (name,phone,address_repair,trouble) VALUES (?,?,?,?)");
    query.addBindValue(name_repair);
    query.addBindValue(phone_repair);
    query.addBindValue(address_repair);
    query.addBindValue(trouble);

    if (!query.exec()) {
        QMessageBox::critical(this, "查询失败", "数据库查询出错：" + query.lastError().text());
        return;
    }

    if (query.numRowsAffected()) { // 确保成功插入报修记录
        QMessageBox::information(this, "报修成功", "报修信息上传成功！");

        ui->nameEdit_2->clear();//姓名
        ui->phoneEdit_2->clear(); //电话
        ui->mendaddressEdit->clear();// 报修地址
        ui->mendtroubleEdit->clear();//问题

    } else {
        QMessageBox::warning(this, "报修失败", "报修信息上传失败");
    }
}
//------------------------------------------报修查询----------------------------------
void Owner::on_find_btn_clicked()
{
    // 获取输入值（姓名，电话）
    QString name = ui->nameEdit_3->text().trimmed();//姓名
    QString phone = ui->phoneEdit_3->text().trimmed();//电话

    // 检查必要字段
    if ( name.isEmpty() || phone.isEmpty() ) {
        QMessageBox::warning(this, "输入错误", "必填字段不能为空！");
        return;
    }

    // 执行查询
    QSqlQuery query;
    query.prepare("SELECT * FROM repair WHERE name= ? AND phone=?");
    query.addBindValue(name);
    query.addBindValue(phone);

    if (!query.exec()) {
        QMessageBox::critical(this, "查询失败", "数据库查询出错：" + query.lastError().text());
        return;
    }

    // 检查是否有结果
    if (query.next()) {
        // 显示查询结果
        ui->troubleEdit->setText(query.value("trouble").toString()); // 问题
        ui->priceEdit->setText(query.value("cost").toString()); // 价格
        ui->conditionEdit->setText(query.value("note").toString()); // 备注
        ui->processEdit->setText(query.value("status_repair").toString()); // 进度

        QMessageBox::information(this, "查询成功", "查询报修信息成功！");
    } else {
        // 没有找到匹配的记录
        QMessageBox::information(this, "未找到记录", "没有找到匹配的报修信息！");

        // 清空显示区域
        ui->troubleEdit->clear();
        ui->priceEdit->clear();
        ui->conditionEdit->clear();
        ui->processEdit->clear();
    }
}
//------------------------------------------报修评价----------------------------------
void Owner::on_judge_btn_clicked()
{
    // 获取输入值（评价，姓名，电话 ）
    QString comment = ui->comboBox->currentText().trimmed();//评价
    QString name = ui->nameEdit_3->text().trimmed();//姓名
    QString phone = ui->phoneEdit_3->text().trimmed();//电话

    // 检查必要字段
    if ( name.isEmpty() || phone.isEmpty() || comment.isEmpty() ) {
        QMessageBox::warning(this, "输入错误", "必填字段不能为空！");
        return;
    }

    // 执行查询
    QSqlQuery query;
    query.prepare("UPDATE repair SET comment = ? WHERE name = ? AND phone = ?");
    query.addBindValue(comment);
    query.addBindValue(name);
    query.addBindValue(phone);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            QMessageBox::information(this, "成功", "提交报修评价信息成功！");
        } else {
            QMessageBox::warning(this, "失败", "未找到匹配的报修记录，提交评价失败！");
        }
    } else {
        QMessageBox::critical(this, "错误", "数据库更新出错：" + query.lastError().text());
    }
}
//---------------------------------------------------------------Owner车位管理-----------------------------------------------------------------------------
//------------------------------------------注册车辆信息----------------------------------
void Owner::on_delpushButton_2_clicked()
{
    // 获取输入值（车牌，姓名，电话，品牌）
    QString name = ui->numlineEdit_3->text().trimmed();//姓名
    QString car_id = ui->numlineEdit_2->text().trimmed();//车牌
    QString phone = ui->brandlineEdit_3->text().trimmed();//电话
    QString car_brand = ui->brandlineEdit_2->text().trimmed();//品牌


    // 检查必要字段
    if (name.isEmpty() || car_id.isEmpty() || phone.isEmpty() || car_brand.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "必填字段不能为空！【PS:页面上方的姓名，手机号需一并填写】");
        return;
    }

    // 执行注册
    QSqlQuery query;
    query.prepare("INSERT INTO carMessage (name,car_id,phone,car_brand) VALUES (?,?,?,?)");
    query.addBindValue(name);
    query.addBindValue(car_id);
    query.addBindValue(phone);
    query.addBindValue(car_brand);

    if (!query.exec()) {
        QMessageBox::critical(this, "查询失败", "数据库查询出错：" + query.lastError().text());
        return;
    }

    if (query.numRowsAffected()) { // 确保成功注册车辆信息
        QMessageBox::information(this, "注册成功", "车辆信息注册成功！");

        ui->numlineEdit_3->clear();//姓名
        ui->numlineEdit_2->clear(); //车牌
        ui->brandlineEdit_3->clear();// 电话
        ui->brandlineEdit_2->clear();//品牌

    } else {
        QMessageBox::warning(this, "注册失败", "车辆信息注册失败");
    }
}
//------------------------------------------删除车辆信息----------------------------------
void Owner::on_delpushButton_clicked()
{
    // 获取输入值（车牌，姓名，电话，品牌）
    QString name = ui->numlineEdit_3->text().trimmed();//姓名
    QString car_id = ui->numlineEdit_2->text().trimmed();//车牌
    QString phone = ui->brandlineEdit_3->text().trimmed();//电话
    QString car_brand = ui->brandlineEdit_2->text().trimmed();//品牌


    // 检查必要字段
    if (name.isEmpty() || car_id.isEmpty() || phone.isEmpty() || car_brand.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "必填字段不能为空！【PS:页面上方的姓名，手机号需一并填写】");
        return;
    }

    // 执行删除
    QSqlQuery query;
    query.prepare("Delete From carMessage Where name=? And car_id=? And phone=? And car_brand=? ");
    query.addBindValue(name);
    query.addBindValue(car_id);
    query.addBindValue(phone);
    query.addBindValue(car_brand);

    if (!query.exec()) {
        QMessageBox::critical(this, "查询失败", "数据库查询出错：" + query.lastError().text());
        return;
    }

    if (query.numRowsAffected()) { // 确保成功删除车辆注册信息
        QMessageBox::information(this, "删除成功", "车辆信息删除成功！");

        ui->numlineEdit_3->clear();//姓名
        ui->numlineEdit_2->clear(); //车牌
        ui->brandlineEdit_3->clear();// 电话
        ui->brandlineEdit_2->clear();//品牌

    } else {
        QMessageBox::warning(this, "删除失败", "车辆信息删除失败");
    }
}

//---------------------------------------------申请车位--------------------------------
void Owner::on_shenpushButton_clicked()
{
    // 获取输入值（车位号，姓名，电话）
    QString name = ui->parklineEdit_3->text().trimmed();//姓名
    QString phone = ui->parklineEdit_2->text().trimmed();//电话
    QString parking_id = ui->parklineEdit->text().trimmed();//车位号

    // 检查必要字段
    if (name.isEmpty() || phone.isEmpty() || parking_id.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "姓名，电话，车位编号不能为空！");
        return;
    }

    // 获取当前时间，格式化为 datetime 字符串，适配数据库存储
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");

    // 执行申请，同时更新 date 字段
    QSqlQuery query;
    query.prepare("UPDATE parking SET name = COALESCE(?, name), phone = COALESCE(?, phone), status = 1, date = ? WHERE parking_id = ?");
    query.addBindValue(name);
    query.addBindValue(phone);
    query.addBindValue(currentDateTime);  // 绑定当前时间
    query.addBindValue(parking_id);

    if (!query.exec()) {
        QMessageBox::critical(this, "查询失败", "数据库查询出错：" + query.lastError().text());
        return;
    }

    if (query.numRowsAffected()) { // 确保成功申请车位
        QMessageBox::information(this, "申请成功", "申请车位成功！");

        ui->parklineEdit_3->clear();//姓名
        ui->parklineEdit_2->clear();// 电话
        ui->parklineEdit->clear();//车位号

    } else {
        QMessageBox::warning(this, "申请失败", "申请车位失败");
    }
}
//------------------------------------------刷新显示  车辆信息表  可申请车位表  我的车位表----------------------------------
void Owner::refresh()
{
    QSqlQuery a;
    QSqlQuery b;
    QSqlQuery c;
        a.prepare("SELECT * FROM carMessage WHERE name=? AND phone=?");


       QString ownername=ui->numlineEdit_3->text().trimmed();
       QString ownerphone=ui->brandlineEdit_3->text().trimmed();
       if (ownername.isEmpty() || ownerphone.isEmpty()) {
           QMessageBox::warning(this, "输入错误", "姓名和手机号不能为空！");
           return;
       }
       a.addBindValue(ownername);
       a.addBindValue(ownerphone);
        model->removeRows(0, model->rowCount());
        if (a.exec())
        {
            while(a.next())
            {
                QList<QStandardItem*>rowItems;
                rowItems.append(new QStandardItem(a.value(1).toString()));
                rowItems.append(new QStandardItem(a.value(2).toString()));
                rowItems.append(new QStandardItem(a.value(3).toString()));

                rowItems.append(new QStandardItem(a.value(4).toString()));


                model->appendRow(rowItems);
       }
    }
        b.prepare("SELECT * FROM parking WHERE status=0");
        model1->removeRows(0, model1->rowCount());
        if (b.exec())
        {
            while(b.next())
            {
                QList<QStandardItem*>rowItems1;

                rowItems1.append(new QStandardItem(b.value(3).toString()));

                rowItems1.append(new QStandardItem(b.value(4).toString()));


                model1->appendRow(rowItems1);
       }
    }
        c.prepare("SELECT * FROM parking WHERE  name=? AND phone=? ");
        if (ownername.isEmpty() || ownerphone.isEmpty()) {
            QMessageBox::warning(this, "输入错误", "姓名和手机号不能为空！");
            return;
        }
        c.addBindValue(ownername);
        c.addBindValue(ownerphone);
        model2->removeRows(0, model1->rowCount());
        if (c.exec())
        {
            while(c.next())
            {
                QList<QStandardItem*>rowItems2;
                rowItems2.append(new QStandardItem(c.value(3).toString()));

                rowItems2.append(new QStandardItem(c.value(4).toString()));


                model2->appendRow(rowItems2);
       }
    }
}
//---------------------------------------------------------------Owner投诉-----------------------------------------------------------------------------
// 处理投诉提交的槽函数
void Owner::on_complaintSubmitBtn_clicked()
{
    // 获取投诉内容
    QString complaint = ui->complaintEdit->toPlainText().trimmed(); // 假设投诉内容输入框名为 complaintEdit

    // 获取姓名和电话
    QString name = ui->lineEdit_2->text().trimmed();
    QString phone = ui->lineEdit_5->text().trimmed();

    // 检查必要字段
    if (complaint.isEmpty() || name.isEmpty() || phone.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "必填字段不能为空！");
        return;
    }

    // 新增：检查投诉内容字数（20-400字）
    int complaintLength = complaint.length();
    if (complaintLength < 20) {
        QMessageBox::warning(this, "内容过短", "投诉内容需至少20个字！");
        return;
    }
    if (complaintLength > 400) {
        QMessageBox::warning(this, "内容过长", "投诉内容不能超过400个字！");
        return;
    }

    // 执行插入操作
    QSqlQuery query;
    query.prepare("INSERT INTO users_complaint (name, phone, complaint) VALUES (?,?,?)");
    query.addBindValue(name);
    query.addBindValue(phone);
    query.addBindValue(complaint);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            QMessageBox::information(this, "成功", "投诉信息提交成功！");
            ui->complaintEdit->clear(); // 清空投诉内容输入框
        } else {
            QMessageBox::warning(this, "失败", "投诉信息提交失败！");
        }
    } else {
        QMessageBox::critical(this, "错误", "数据库插入出错：" + query.lastError().text());
    }
}
//---------------------------------------------------------------缴费管理-----------------------------------------------------------------------------
//-----------------------------------------自主缴费------------------------------------
void Owner::showtableview_8(){
    model3->removeRows(0, model3->rowCount());

    QSqlQuery a, b;
    QString ownername = currentname;
    QString ownerphone = currentphone;

    // 查询未缴物业费
    a.prepare("SELECT community_cost, community_cost_y_m, community_cost_status FROM community_cost WHERE name=? AND phone=? AND community_cost_status=?");
    a.addBindValue(ownername);
    a.addBindValue(ownerphone);
    a.addBindValue("未缴费");

    // 查询未缴车位费
    b.prepare("SELECT cost, parking_id, 缴费情况 FROM parking WHERE name=? AND phone=? AND 缴费情况=?");
    b.addBindValue(ownername);
    b.addBindValue(ownerphone);
    b.addBindValue("未缴费");

    if (a.exec() && b.exec()) {
        bool hasPropertyFee = a.next();
        bool hasParkingFee = b.next();

        if (!hasPropertyFee && !hasParkingFee) {
            QMessageBox::information(this, "查询结果", "恭喜！您没有未缴纳的费用。");
            return;
        }

        // 处理物业费
        while (hasPropertyFee) {
            QList<QStandardItem*> rowItems;
            rowItems.append(new QStandardItem(ownername));
            rowItems.append(new QStandardItem(ownerphone));
            rowItems.append(new QStandardItem(a.value(0).toString())); // 物业费金额
            rowItems.append(new QStandardItem(a.value(1).toString())); // 物业费日期（年月）
            rowItems.append(new QStandardItem(a.value(2).toString())); // 缴费状态
            rowItems.append(new QStandardItem("")); // 车位费金额（留空）
            rowItems.append(new QStandardItem("")); // 车位号（留空）
            rowItems.append(new QStandardItem("")); // 车位缴费状态（留空）
            model3->appendRow(rowItems);
            hasPropertyFee = a.next();
        }

        // 处理车位费
        while (hasParkingFee) {
            QList<QStandardItem*> rowItems;
            rowItems.append(new QStandardItem(ownername));
            rowItems.append(new QStandardItem(ownerphone));
            rowItems.append(new QStandardItem("")); // 物业费金额（留空）
            rowItems.append(new QStandardItem("")); // 物业费日期（留空）
            rowItems.append(new QStandardItem("")); // 物业费缴费状态（留空）
            rowItems.append(new QStandardItem(b.value(0).toString())); // 车位费金额
            rowItems.append(new QStandardItem(b.value(1).toString())); // 车位号
            rowItems.append(new QStandardItem(b.value(2).toString())); // 缴费状态
            model3->appendRow(rowItems);
            hasParkingFee = b.next();
        }
    } else {
        QMessageBox::warning(this, "查询失败", "数据库查询出错：" + a.lastError().text() + " " + b.lastError().text());
    }
}

void Owner::cost(){
    QSqlQuery a;
    QSqlQuery b;
    QString ownername = currentname;
    QString ownerphone = currentphone;
    bool success = false;

    if(ui->comboBox_4->currentText() == "车位费"){
        b.prepare("UPDATE parking SET 缴费情况 = ? WHERE name = ? AND phone = ? AND 缴费情况 = ?");
        b.addBindValue("已缴费");
        b.addBindValue(ownername);
        b.addBindValue(ownerphone);
        b.addBindValue("未缴费");

        if(b.exec()){
            if(b.numRowsAffected() > 0) {
                QMessageBox::information(this, "缴费成功", "车位费缴费成功！");
                success = true;
            } else {
                QMessageBox::information(this, "无需缴费", "您没有未缴纳的车位费！");
            }
        } else {
            QMessageBox::warning(this, "缴费失败", "车位费缴费失败: " + b.lastError().text());
        }
    }
    else if(ui->comboBox_4->currentText() == "物业费"){
        // 获取当前操作日期（格式：YYYY-MM-DD）
        QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");

        // 修改UPDATE语句，新增更新community_cost_date字段
        a.prepare("UPDATE community_cost SET "
                  "community_cost_status = ?, "  // 更新缴费状态
                  "community_cost_date = ? "     // 写入当前操作日期
                  "WHERE name = ? "
                  "AND phone = ? "
                  "AND community_cost_status = ?");

        // 绑定参数：状态、当前日期、用户名、电话、原状态（未缴费）
        a.addBindValue("已缴费");
        a.addBindValue(currentDate);
        a.addBindValue(ownername);
        a.addBindValue(ownerphone);
        a.addBindValue("未缴费");

        if(a.exec()){
            if(a.numRowsAffected() > 0) {
                QMessageBox::information(this, "缴费成功", "物业费缴费成功！");
                success = true;
            } else {
                QMessageBox::information(this, "无需缴费", "您没有未缴纳的物业费！");
            }
        } else {
            QMessageBox::warning(this, "缴费失败", "物业费缴费失败: " + a.lastError().text());
        }
    }

    if(success) {
        showtableview_8();
    }
}
//-----------------------------------------缴费查询------------------------------------
void Owner::on_pushButton_13_clicked()
{
    // 获取登录用户姓名（从 users 表查询）
    QString account = currentAccount;
    QString name;
    QSqlQuery nameQuery;
    nameQuery.prepare("SELECT name FROM users WHERE account = :account");
    nameQuery.bindValue(":account", account);
    if (nameQuery.exec() && nameQuery.next())
    {
        name = nameQuery.value(0).toString();
    }
    else
    {
        QMessageBox::critical(this, "错误", "获取姓名失败：" + nameQuery.lastError().text());
        return;
    }

    // 获取界面输入
    QString feeType = ui->comboBox_3->currentText();
    QString inputTime = ui->textEdit->toPlainText().trimmed();

    // 校验时间格式（XXXX-XX）
    QRegularExpression timeRegex("^\\d{4}-(\\d{2})$");
    QRegularExpressionMatch match = timeRegex.match(inputTime);
    if (!match.hasMatch())
    {
        QMessageBox::warning(this, "格式错误", "时间需为 XXXX-XX 格式，请重新输入！");
        return;
    }

    // 提取月份并校验范围（01-12）
    QString monthStr = match.captured(1);
    bool ok;
    int month = monthStr.toInt(&ok);
    if (!ok || month < 1 || month > 12) {
        QMessageBox::warning(this, "月份错误", "月份必须在01-12之间！");
        return;
    }

    QString yearMonth = inputTime;

    // 清空 tableView 旧数据
    ui->tableView_7->setModel(nullptr);

    // 根据缴费类型拼SQL
    QSqlQuery query;
    QString sql;

    if (feeType == "物业费")
    {
            sql = QString(R"(
                SELECT
                name AS 姓名,
                phone AS 电话,
                -- 未缴费时缴费日期为空
                CASE WHEN community_cost_status = '未缴费' THEN '' ELSE community_cost_date END AS 缴费日期,
                community_cost_y_m AS 发布日期,
                community_cost_status AS 缴费情况
                FROM community_cost
                WHERE name = :name
                AND community_cost_y_m = :yearMonth)");
            query.prepare(sql);
            query.bindValue(":name", name);
            query.bindValue(":yearMonth", yearMonth);
    }
    else if (feeType == "车位费")
    {
            sql = QString(R"(
                SELECT
                name AS 姓名,
                phone AS 电话,
                parking_id AS 车牌号,
                cost AS 停车费,
                date AS 日期,
                缴费情况 AS 缴费情况
                FROM parking
                WHERE name = :name
                AND strftime('%Y-%m', date) = :yearMonth)");
            query.prepare(sql);
            query.bindValue(":name", name);
            query.bindValue(":yearMonth", yearMonth);
    }
    else
    {
        QMessageBox::warning(this, "参数错误", "请选择正确的缴费类型！");
        return;
    }

    // 执行查询并展示结果
    if (query.exec())
    {
        QSqlQueryModel *model = new QSqlQueryModel(this);
        model->setQuery(query);
        ui->tableView_7->setModel(model);
        ui->tableView_7->resizeColumnsToContents();
    }
    else
    {
        QMessageBox::critical(this, "查询失败",
                             "数据库错误：" + query.lastError().text() +
                             "\nSQL：" + sql);
    }
}












