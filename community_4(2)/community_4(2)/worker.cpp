#include "worker.h"
#include "ui_worker.h"
#include <QSqlQuery>
#include <QString>
#include<QLineEdit>
#include <QMessageBox>
#include <QSqlError>
#include <QTextEdit>
#include <QDebug>
Worker::Worker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Worker)
{
    ui->setupUi(this);
    //提交报修处理信息
    connect(ui->pushButton_4, &QPushButton::clicked, this, &Worker::on_pushButton_4_clicked);

    //展示repair表
    model = new QStandardItemModel(0, 6, this);
    model->setHorizontalHeaderLabels({"name","phone","address","status","trouble","comment"});
    ui->tableView_4->setModel(model);

    //住址查询业主
    connect(ui->pushButton_2, &QPushButton::clicked, this, &Worker::on_queryByaddress_clicked);

    //展示请销假表
    model1 = new QStandardItemModel(0, 6, this);
       model1->setHorizontalHeaderLabels({"序号", "姓名","电话","请假审批状态","日期","理由","请假始末","账户"});
       model2 = new QStandardItemModel(0,5,this);
       model2->setHorizontalHeaderLabels({"name","phone","status","date","reason"});
       ui->tableView_3->setModel(model2);
       ui->tableView_3->setModel(model1);
       tableModel = new QSqlTableModel(this, QSqlDatabase::database());
       tableModel->setFilter("status = '未审批'");
       tableModel->setHeaderData(0, Qt::Horizontal, "ID");
       tableModel->setHeaderData(1, Qt::Horizontal, "姓名");
       tableModel->setHeaderData(2, Qt::Horizontal, "电话");
       tableModel->setHeaderData(3, Qt::Horizontal, "状态");
       tableModel->setHeaderData(4, Qt::Horizontal, "日期");
       tableModel->setHeaderData(5, Qt::Horizontal, "原因");
       tableModel->setTable("time_off");
       //申请请假
       connect(ui->pushButton_5, &QPushButton::clicked, this, &Worker::on_pushButton_5_clicked);
       connect(ui->pushButton_6, &QPushButton::clicked, this, &Worker::on_pushButton_6_clicked);

     //出勤情况模块
       connect(ui->pushButton_9, &QPushButton::clicked, this, &Worker::on_pushButton_9_clicked);
       connect(ui->pushButton_11, &QPushButton::clicked, this, &Worker::on_pushButton_11_clicked);

       // 车位管理相关按钮连接
           connect(ui->queryParkingButton, &QPushButton::clicked, this, &Worker::on_queryParkingSpaces_clicked);// 查询所有车位信息
           connect(ui->modifyParkingButton, &QPushButton::clicked, this, &Worker::on_modifyParkingSpace_clicked);// 修改车位费用
           connect(ui->addParkingButton, &QPushButton::clicked, this, &Worker::on_addParkingSpace_clicked); // 添加车位
      // 初始化车位管理模型
        parkingModel = new QStandardItemModel(0, 2, this);
        parkingModel->setHorizontalHeaderLabels({"车位编号", "车位费用"});
        ui->parkingTableView->setModel(parkingModel);

        // 物业收费管理模块按钮连接
           connect(ui->propertyFeeSubmitBtn, &QPushButton::clicked, this, &Worker::on_propertyFeeSubmitBtn_clicked);
       //连接物业费查询按钮
       connect(ui->queryPropertyFeeButton, &QPushButton::clicked, this, &Worker::on_queryPropertyFee_clicked);

       //缴费记录[停车位]
       model3 = new QStandardItemModel(0, 4, this);
       model3->setHorizontalHeaderLabels({ "姓名","电话","停车位","费用","申请日期","缴费情况"});
       ui->tableView_2->setModel(model3);
       //缴费记录[]
       model4 = new QStandardItemModel(0, 4, this);
       model4->setHorizontalHeaderLabels({ "姓名","电话","物业费用","缴费日期","物业费日期","缴费情况"});
       ui->tableView_2->setModel(model4);
      // 缴费记录-》刷新
       connect(ui->pushButton, &QPushButton::clicked, this, &Worker::on_pushButton_clicked);

}



Worker::~Worker()
{
    delete ui;
}

//-----------------------------------获取账号-----------------------------
void Worker::setAccountAndPassword(const QString& account, const QString& password)
{
    currentAccount=account;
    currentPassword=password;
     loadDataByAccount();
}

//---------------------------------------------------------------Worker维修管理-----------------------------------------------------------------------------
//-----------------------------------------提交报修处理信息--------------------------------
void Worker::on_pushButton_4_clicked()
{
    QString address_repair = ui->lineEdit_9->text().trimmed();//地址
    QString name = ui->lineEdit_10->text().trimmed();//姓名
    QString cost = ui->lineEdit_11->text().trimmed();//费用
    QString note = ui->textEdit->toPlainText().trimmed();//备注
    QString status_repair = ui->comboBox->currentText().trimmed();//维修状态

    if (address_repair.isEmpty() || name.isEmpty() || cost.isEmpty()|| note.isEmpty()|| status_repair.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "必填字段不能为空！");
        return;
    }
     //修改
    QSqlQuery query;
    query.prepare("UPDATE repair SET  cost = ? ,note = ?,status_repair = ? WHERE address_repair = ? AND name = ?");

    query.addBindValue(cost);
    query.addBindValue(note);
    query.addBindValue(status_repair);
    query.addBindValue(address_repair);
    query.addBindValue(name);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            QMessageBox::information(this, "成功", "提交报修处理信息成功！");
        } else {
            QMessageBox::warning(this, "失败", "提交报修处理信息失败！");
        }
    } else {
        QMessageBox::critical(this, "错误", "repair数据库更新出错：" + query.lastError().text());
    }
    //清理
    ui->lineEdit_9->clear();
        ui->lineEdit_10->clear();
        ui->lineEdit_11->clear();
        ui->textEdit->clear();
        ui->comboBox->setCurrentIndex(0);
}
//-----------------------------------------展示repair表--------------------------------
void Worker::show_repair_table()
{
    QSqlQuery cha;
        cha.prepare("SELECT * FROM repair ");
        model->removeRows(0, model->rowCount());
        if (cha.exec())
        {
            while(cha.next())
            {
                QList<QStandardItem*>rowItems;
                rowItems.append(new QStandardItem(cha.value(1).toString()));
                rowItems.append(new QStandardItem(cha.value(2).toString()));
                rowItems.append(new QStandardItem(cha.value(3).toString()));
                rowItems.append(new QStandardItem(cha.value(6).toString()));
                rowItems.append(new QStandardItem(cha.value(4).toString()));
                rowItems.append(new QStandardItem(cha.value(8).toString()));

                model->appendRow(rowItems);
       }
    }
}
//---------------------------------------------------------------Worker业主信息管理-----------------------------------------------------------------------------
//按住址查询
void Worker::on_queryByaddress_clicked()
{
    QString address = ui->lineEdit_4->text().trimmed();//address接收

    // 检查必要字段
    if (address.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "地址不能为空！");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE address=?");
    query.addBindValue(address);

    if (!query.exec()) {
        QMessageBox::critical(this, "查询失败", "数据库查询出错：" + query.lastError().text());
        return;
    }

    if (query.next()) { // 确保查询返回有效记录
        QMessageBox::information(this, "查询成功", "用户信息查询成功！");

        ui->lineEdit->setText(query.value("address").toString());//房间号
        ui->lineEdit_3->setText(query.value("name").toString()); //姓名
        ui->lineEdit_2->setText(query.value("phone").toString());//电话
    } else {
        QMessageBox::warning(this, "查询失败", "未找到该地址对应的用户！");
    }
}
//---------------------------------------------------------------Worker个人中心-----------------------------------------------------------------------------
void Worker::showWorkerMassage(const QString &name, const QString &phone, const QString &address){
    ui->lineEdit_17->setText(name);
    ui->lineEdit_18->setText(phone);
    ui->lineEdit_19->setText(address);
}
//---------------------------------------------------------------Worker请销假磨块-----------------------------------------------------------------------------
//-----------------------------------------展示请假信息表--------------------------------
void Worker::loadDataByAccount()
{
    // 假设你的数据库表名为 "repairs"，并且有一个 "account" 字段作为索引
    tableModel->setTable("time_off");

    // 设置查询条件：WHERE account = 'xxx'
    tableModel->setFilter(QString("m_account = '%1'").arg(currentAccount));

    // 执行查询
    if (!tableModel->select()) {
        qDebug() << "Error loading data:" << tableModel->lastError().text();
        return;
    }

    model1->removeRows(0, model1->rowCount());

    // 将QSqlTableModel的数据复制到QStandardItemModel
    for (int i = 0; i < tableModel->rowCount(); ++i) {
        QList<QStandardItem*> rowItems;
        for (int j = 0; j < tableModel->columnCount(); ++j) {
            QStandardItem* item = new QStandardItem(tableModel->data(tableModel->index(i, j)).toString());
            rowItems.append(item);
        }
        model1->appendRow(rowItems);
    }


    // 调整表格列宽以适应内容
    ui->tableView_3->resizeColumnsToContents();
}
//-----------------------------------------申请请假--------------------------------
void Worker::on_pushButton_5_clicked()
{
    QString m_name = ui->lineEdit_14->text().trimmed();
    QString m_phone = ui->lineEdit_15->text().trimmed();
    QString m_time = ui->lineEdit_13->text().trimmed();
    QString m_reason = ui->lineEdit_12->text().trimmed();
   QString apply_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    // 输入验证
    if (m_name.isEmpty() || m_phone.isEmpty() || m_time.isEmpty() || m_reason.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "请填写所有必填字段");
        return;
    }

    // 使用数据库连接的查询对象
    QSqlQuery query;

    // 使用预处理语句防止SQL注入
    query.prepare("INSERT INTO time_off (name, phone, status, date, reason,apply_time,m_account) "
                  "VALUES (?,?,?,?,?,?,?)");

    // 绑定参数
    query.addBindValue(m_name);
    query.addBindValue(m_phone);
    query.addBindValue("未审批 ");
    query.addBindValue(m_time);
    query.addBindValue(m_reason);
    query.addBindValue(apply_time);
    query.addBindValue(currentAccount);

    // 执行SQL并处理结果
    if (query.exec()) {
        QMessageBox::information(this, "成功", "请假申请已提交");

        // 清空正确的输入框
        ui->lineEdit_14->clear();
        ui->lineEdit_15->clear();
        ui->lineEdit_13->clear();
        ui->lineEdit_12->clear();
    } else {
        // 显示详细错误信息
        QMessageBox::critical(this, "错误", "提交失败：" + query.lastError().text());
    }
}
//-----------------------------------------销假--------------------------------
void Worker::on_pushButton_6_clicked()
{
    QModelIndexList selectedIndexes = ui->tableView_3->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先选择要审批的假条");
        return;
    }
    // 遍历选中的行索引
    for (const QModelIndex& index : selectedIndexes) {
        int row = index.row();
        QString name = model2->data(model2->index(row, 0)).toString(); // 获取姓名列的值

        QSqlQuery updateQuery; // 在循环内每次创建新的QSqlQuery对象
        updateQuery.prepare("UPDATE time_off SET status = ? WHERE m_account = ? AND status = ?");
        updateQuery.addBindValue("已销假");  // 绑定第一个参数
        updateQuery.addBindValue(currentAccount);
        updateQuery.addBindValue("已批准");  // 绑定第三个参数

        if (!updateQuery.exec()) {
            QMessageBox::warning(this, "错误", "批准操作失败: " + updateQuery.lastError().text());
            return; // 只要有一次执行失败就退出函数
        }
    }
   //成功后提示
    QMessageBox::information(this,"销假成功","销假成功");
    // 刷新表格数据
    model2->removeRows(0, model2->rowCount());
        loadDataByAccount();
}

//-------------------------------------------------------------------出勤打卡情况--------------------------------------------------------------------------------
void Worker::on_pushButton_9_clicked()
{
    // 获取时间与账号
    QString shangban = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString account = currentAccount;

    // 从 users 表根据 account 查询对应的 name
    QString name;
    QSqlQuery nameQuery;
    nameQuery.prepare("SELECT name FROM users WHERE account = :account");
    nameQuery.bindValue(":account", account);
    if (nameQuery.exec() && nameQuery.next()) {
        name = nameQuery.value(0).toString();
    } else {
        QMessageBox::critical(this, "错误", "获取姓名失败: " + nameQuery.lastError().text());
        return;
    }

    // 时间有效性校验（8点前禁止打卡）
    QTime currentTime = QTime::fromString(shangban.right(8), "hh:mm:ss");
    QTime earlyThreshold = QTime::fromString("08:00:00", "hh:mm:ss");
    if (currentTime < earlyThreshold) {
        QMessageBox::critical(this, "错误", "8点前禁止打卡，请稍后再试");
        return;
    }

    // 判断迟到
    QTime currentHhMm = QTime::fromString(shangban.right(8), "hh:mm:ss");
    QTime lateThreshold = QTime::fromString("09:00:00", "hh:mm:ss");
    QString zaoqingkuang = (currentHhMm > lateThreshold)? "迟到" : "正常";
    bool hasInserted = false;
    QSqlQuery query;
    // 根据账号查询，且对应字段改为账号
    query.prepare("SELECT * FROM dakabiao WHERE 账号 = :account AND 上班 IS NULL");
    query.bindValue(":account", account);
    if (query.exec() && query.next()) {
        QSqlQuery updateQuery;
        // 更新语句中使用账号、姓名和对应字段
        updateQuery.prepare("UPDATE dakabiao SET 上班 = :shangban, 早打卡 = :state, 姓名 = :name WHERE 账号 = :account AND 上班 IS NULL");
        updateQuery.bindValue(":shangban", shangban);
        updateQuery.bindValue(":state", zaoqingkuang);
        updateQuery.bindValue(":name", name);
        updateQuery.bindValue(":account", account);
        if (updateQuery.exec()) {
            QMessageBox::information(this, "打卡成功", "上班时间已更新，状态：" + zaoqingkuang);
        } else {
            QMessageBox::critical(this, "错误", "更新失败: " + updateQuery.lastError().text());
        }
    } else {
        // 插入语句中使用账号、姓名和对应字段
        query.prepare("INSERT INTO dakabiao (上班, 账号, 姓名, 早打卡) VALUES (:shangban, :account, :name, :state)");
        query.bindValue(":shangban", shangban);
        query.bindValue(":account", account);
        query.bindValue(":name", name);
        query.bindValue(":state", zaoqingkuang);
        if (query.exec()) {
            QMessageBox::information(this, "打卡成功", "上班时间已记录，状态：" + zaoqingkuang);
            hasInserted = true;
        } else {
            QMessageBox::critical(this, "错误", "插入失败: " + query.lastError().text());
        }
    }

    QSqlTableModel *model = new QSqlTableModel(this);
    model->setTable("dakabiao");
    model->select();
    ui->tableView_5->setModel(model);
    ui->tableView_5->resizeColumnsToContents();

    // 防止重复操作的保险逻辑
    if (hasInserted) {
        return;
    }
}

void Worker::on_pushButton_11_clicked()
{
    // 获取时间与账号
    QString xiaban = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString account = currentAccount;

    // 从 users 表根据 account 查询对应的 name
    QString name;
    QSqlQuery nameQuery;
    nameQuery.prepare("SELECT name FROM users WHERE account = :account");
    nameQuery.bindValue(":account", account);
    if (nameQuery.exec() && nameQuery.next()) {
        name = nameQuery.value(0).toString();
    } else {
        QMessageBox::critical(this, "错误", "获取姓名失败: " + nameQuery.lastError().text());
        return;
    }

    // 时间有效性校验（23:00点后禁止打卡）
    QTime currentHhMm_1 = QTime::fromString(xiaban.right(8), "hh:mm:ss");
    QTime earlyThreshold_1 = QTime::fromString("23:00:00", "hh:mm:ss");
    if (currentHhMm_1 > earlyThreshold_1) {
        QMessageBox::critical(this, "错误", "23点后禁止打卡，请稍后再试");
        return;
    }

    // 判断时间
    QTime currentHhMm = QTime::fromString(xiaban.right(8), "hh:mm:ss");
    QTime earlyThreshold = QTime::fromString("19:30:00", "hh:mm:ss");
    QString wanqingkuang = "正常";
    if (currentHhMm < earlyThreshold) {
        wanqingkuang = "早退";
    }

    // 查询是否已有未下班记录，基于账号查询
    QSqlQuery query;
    query.prepare("SELECT * FROM dakabiao WHERE 账号 = :account AND 下班 IS NULL");
    query.bindValue(":account", account);
    if (!query.exec() ||!query.next()) {
        QMessageBox::information(this, "提示", "请先进行上班打卡");
        return;
    }

    // 更新下班时间与状态，基于账号更新，同时带上姓名
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE dakabiao SET 下班 = :xiaban, 晚打卡 = :state, 姓名 = :name WHERE 账号 = :account AND 下班 IS NULL");
    updateQuery.bindValue(":xiaban", xiaban);
    updateQuery.bindValue(":state", wanqingkuang);
    updateQuery.bindValue(":name", name);
    updateQuery.bindValue(":account", account);
    if (updateQuery.exec()) {
        QMessageBox::information(this, "打卡成功", "下班时间已记录，状态：" + wanqingkuang);
    } else {
        QMessageBox::critical(this, "错误", "打卡失败：" + updateQuery.lastError().text());
    }

    QSqlTableModel *model = new QSqlTableModel(this);
    model->setTable("dakabiao");
    model->select();
    ui->tableView_5->setModel(model);
    ui->tableView_5->resizeColumnsToContents();
}

void Worker::on_pushButton_10_clicked()
{
    model2->removeRows(0, model2->rowCount());
        loadDataByAccount();
}
//-------------------------------------------------------------------车位管理模块--------------------------------------------------------------------------------
// ----------------------------------查询所有车位信息--------------------------------
void Worker::on_queryParkingSpaces_clicked()
{
    // 创建SQL查询对象
    QSqlQuery query;
    // 准备SQL查询语句，从parking表中获取车位编号、费用和状态
    query.prepare("SELECT parking_id, cost, status FROM parking");

    // 清空现有表格数据，避免重复显示
    parkingModel->removeRows(0, parkingModel->rowCount());

    // 执行查询并处理结果
    if (query.exec()) {
        // 遍历查询结果集
        while (query.next()) {
            QList<QStandardItem*> rowItems;
            // 添加车位编号列数据
            rowItems.append(new QStandardItem(query.value("parking_id").toString()));
            // 添加车位费用列数据
            rowItems.append(new QStandardItem(query.value("cost").toString()));

            // 获取状态值并转换为对应的文本描述（0=未售出，1=已售出）
            int statusValue = query.value("status").toInt();
            QString statusText = (statusValue == 0) ? "未售出" : "已售出";
            rowItems.append(new QStandardItem(statusText));

            // 将行数据添加到表格模型
            parkingModel->appendRow(rowItems);
        }
        // 设置表格模型的水平表头，新增“车位状态”列
        parkingModel->setHorizontalHeaderLabels({"车位编号", "车位费用", "车位状态"});
    } else {
        // 显示查询错误信息
        QMessageBox::critical(this, "查询失败",
                              "查询车位数据出错：" + query.lastError().text());
    }
}
//--------------------------------修改车位费用-------------------------------
void Worker::on_modifyParkingSpace_clicked()
{
    // 获取用户输入的车位编号（去除首尾空格）
    QString parkingId = ui->parkingIdEdit->text().trimmed();
    // 获取用户输入的车位费用（去除首尾空格）
    QString price = ui->parkingPriceEdit->text().trimmed();

    // 输入合法性检查：非空校验
    if (parkingId.isEmpty() || price.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "车位编号和费用不能为空！");
        return;
    }

    // 输入合法性检查：费用格式校验（价格不能小于 0 ）
    bool isPriceValid = false;
    double priceVal = price.toDouble(&isPriceValid);
    if (!isPriceValid || priceVal < 0) {
        QMessageBox::warning(this, "输入错误", "费用需为非负数值！");
        return;
    }

    // 创建 SQL 查询对象
    QSqlQuery query;
    // 准备 SQL 更新语句，增加条件：仅修改 status 为 0 的车位
    query.prepare("UPDATE parking SET cost = ? WHERE parking_id = ? AND status = 0");
    // 绑定参数：费用值
    query.addBindValue(priceVal);
    // 绑定参数：车位编号
    query.addBindValue(parkingId);

    // 执行更新并处理结果
    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            QMessageBox::information(this, "修改成功", "车位费用已更新！");
            // 刷新车位列表，显示最新数据
            on_queryParkingSpaces_clicked();
        } else {
            // 若影响行数为 0 ，分情况提示（车位不存在 / status 不为 0 ）
            QSqlQuery checkQuery;
            checkQuery.prepare("SELECT COUNT(*) FROM parking WHERE parking_id = ?");
            checkQuery.addBindValue(parkingId);
            checkQuery.exec();
            checkQuery.next();
            if (checkQuery.value(0).toInt() == 0) {
                QMessageBox::warning(this, "修改失败", "未找到该车位编号！");
            } else {
                QMessageBox::warning(this, "修改失败", "该车位状态非未售出（status != 0 ），无法修改价格！");
            }
        }
    } else {
        // 显示更新错误信息
        QMessageBox::critical(this, "修改出错",
                              "更新费用失败：" + query.lastError().text());
    }
}
//------------------------------添加车位-----------------------------
void Worker::on_addParkingSpace_clicked()
{
    QString parkingId = ui->parkingIdEdit->text().trimmed();
    QString price = ui->parkingPriceEdit->text().trimmed();

    // 输入合法性检查
    if (parkingId.isEmpty() || price.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "车位编号和费用必须填写！");
        return;
    }

    // 修复：显式定义 isPriceValid 变量
    bool isPriceValid = false;
    double priceVal = price.toDouble(&isPriceValid);
    if (!isPriceValid || priceVal < 0) {
        QMessageBox::warning(this, "输入错误", "费用需为有效非负数值！");
        return;
    }

    // 检查车位编号是否已存在
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM parking WHERE parking_id = ?");
    checkQuery.addBindValue(parkingId);
    if (checkQuery.exec() && checkQuery.next()) {
        if (checkQuery.value(0).toInt() > 0) {
            QMessageBox::warning(this, "添加失败", "该车位编号已存在！");
            return;
        }
    } else {
        QMessageBox::critical(this, "查询失败", "验证车位编号时出错：" + checkQuery.lastError().text());
        return;
    }

    // 执行插入
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO parking (parking_id, cost, status) VALUES (?, ?, 0)");
    insertQuery.addBindValue(parkingId);
    insertQuery.addBindValue(priceVal);

    if (insertQuery.exec()) {
        QMessageBox::information(this, "添加成功", "新车位已录入系统！");
        ui->parkingIdEdit->clear();
        ui->parkingPriceEdit->clear();
        on_queryParkingSpaces_clicked();
    } else {
        QMessageBox::critical(this, "添加失败", "插入数据出错：" + insertQuery.lastError().text());
    }
}
//-------------------------------------------------------------------物业收费管理模块--------------------------------------------------------------------------------
// ----------------------------------物业费缴费发布--------------------------------
void Worker::on_propertyFeeSubmitBtn_clicked()
{
    QString priceStr = ui->priceEdit->text().trimmed(); // 价格维护
    QString dateStr = ui->dateEdit->text().trimmed();   // 发布日期(yyyy-mm)

    // 验证价格
    bool priceValid;
    double price = priceStr.toDouble(&priceValid);
    if (!priceValid || price < 0) {
        QMessageBox::warning(this, "输入错误", "价格必须为非负数字！");
        return;
    }

    // 验证日期格式
    QRegularExpression dateRegex("^(\\d{4})-(\\d{2})$");
    QRegularExpressionMatch match = dateRegex.match(dateStr);
    if (!match.hasMatch()) {
        QMessageBox::warning(this, "输入错误", "日期格式必须为 yyyy-mm！");
        return;
    }

    // 提取年和月进行进一步验证
    int year = match.captured(1).toInt();
    int month = match.captured(2).toInt();

    // 获取当前年份和明年年份
    QDate currentDate = QDate::currentDate();
    int currentYear = currentDate.year();
    int nextYear = currentYear + 1;

    // 检查年份范围（仅允许今年和明年）
    if (year != currentYear && year != nextYear) {
        QMessageBox::warning(this, "输入错误",
            QString("年份必须为 %1 或 %2！").arg(currentYear).arg(nextYear));
        return;
    }

    // 检查月份范围（严格1-12）
    if (month < 1 || month > 12) {
        QMessageBox::warning(this, "输入错误", "月份必须在1-12之间！");
        return;
    }

    // --------------------------新增：检测该月份是否已发布缴费记录--------------------------
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM community_cost WHERE community_cost_y_m = ?");
    checkQuery.addBindValue(dateStr); // 用输入的yyyy-mm匹配

    if (!checkQuery.exec()) {
        QMessageBox::critical(this, "查询失败", "检测历史记录出错：" + checkQuery.lastError().text());
        return;
    }

    int existingCount = 0;
    if (checkQuery.next()) {
        existingCount = checkQuery.value(0).toInt();
    }

    // 如果该月份已有记录，询问是否修改
    if (existingCount > 0) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "月份已发布",
            QString("检测到 %1 已有缴费记录，是否修改该月费用？").arg(dateStr),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            // 用户选择不修改，直接返回
            return;
        } else {
            // 用户选择修改，先删除该月份的所有记录
            QSqlQuery deleteQuery;
            deleteQuery.prepare("DELETE FROM community_cost WHERE community_cost_y_m = ?");
            deleteQuery.addBindValue(dateStr);

            if (!deleteQuery.exec()) {
                QMessageBox::critical(this, "删除失败", "删除历史记录出错：" + deleteQuery.lastError().text());
                return;
            }
        }
    }
    // --------------------------------------------------------------------------------

    // 查询所有【业主】的姓名和电话
    QSqlQuery query;
    query.prepare("SELECT name, phone FROM users WHERE role = '业主'");
    if (!query.exec()) {
        QMessageBox::critical(this, "查询失败", "数据库查询出错：" + query.lastError().text());
        return;
    }

    // 开始事务（提升批量插入性能）
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    while (query.next()) {
        QString name = query.value("name").toString();
        QString phone = query.value("phone").toString();

        // 插入数据到 community_cost 表
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO community_cost (name, phone, community_cost, community_cost_y_m, community_cost_status) VALUES (?,?,?,?,?)");
        insertQuery.addBindValue(name);
        insertQuery.addBindValue(phone);
        insertQuery.addBindValue(price);
        insertQuery.addBindValue(dateStr);
        insertQuery.addBindValue("未缴费");

        if (!insertQuery.exec()) {
            db.rollback(); // 回滚事务
            QMessageBox::critical(this, "插入失败", "数据库插入出错：" + insertQuery.lastError().text());
            return;
        }
    }

    // 提交事务
    if (db.commit()) {
        QMessageBox::information(this, "成功", "物业收费信息发布成功！");
    } else {
        QMessageBox::critical(this, "事务失败", "提交事务出错：" + db.lastError().text());
    }
}
// ----------------------------------查询业主缴费--------------------------------
void Worker::on_queryPropertyFee_clicked()
{
    QString inputName = ui->nameEdit_11->text().trimmed();
    QString inputPhone = ui->phoneEdit_11->text().trimmed();

    if (inputName.isEmpty() || inputPhone.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "姓名和手机号不能为空！");
        return;
    }

    int currentYear = QDateTime::currentDateTime().date().year();

    // 查询当前年份该业主的物业费总和（已缴费状态才计入）
    QSqlQuery sumQuery;
    sumQuery.prepare("SELECT SUM(CAST(community_cost AS REAL)) "
                     "FROM community_cost "
                     "WHERE name = ? AND phone = ? AND community_cost_status = '已缴费' AND substr(community_cost_y_m, 1, 4) = ?");
    sumQuery.addBindValue(inputName);
    sumQuery.addBindValue(inputPhone);
    sumQuery.addBindValue(QString::number(currentYear));

    if (!sumQuery.exec()) {
        qDebug() << "SQL Error:" << sumQuery.lastError().text();
        qDebug() << "Failed Query:" << sumQuery.lastQuery();
        qDebug() << "Bound Values:" << sumQuery.boundValues();
        QMessageBox::critical(this, "查询失败", "查询物业费总和出错：" + sumQuery.lastError().text());
        return;
    }

    double totalCost = 0.0;
    if (sumQuery.next()) {
        totalCost = sumQuery.value(0).toDouble();
    }
    // 只显示费用数字，格式化为两位小数
    ui->totalFeeLabel->setText(QString::number(totalCost, 'f', 2));

    // 查询最近一次缴费记录（使用community_cost_date字段）
    QSqlQuery recentQuery;
    recentQuery.prepare("SELECT community_cost_date "
                        "FROM community_cost "
                        "WHERE name = ? AND phone = ? "
                        "ORDER BY community_cost_date DESC "
                        "LIMIT 1");
    recentQuery.addBindValue(inputName);
    recentQuery.addBindValue(inputPhone);

    if (!recentQuery.exec()) {
        QMessageBox::critical(this, "查询失败", "查询最近缴费记录出错：" + recentQuery.lastError().text());
        return;
    }

    if (recentQuery.next()) {
        QString date = recentQuery.value("community_cost_date").toString();
        // 只显示日期
        ui->recentPaymentLabel->setText(date);
    } else {
        ui->recentPaymentLabel->setText("未查询到该业主的缴费记录");
    }
}
//----------------------------------------------------------------------Worker缴费记录----------------------------------------------------------
void Worker::on_pushButton_clicked()
{
    QString costkinds=ui->comboBox_2->currentText();

    if(costkinds=="物业费"){
        ui->tableView_2->setModel(nullptr);
        costkinds="community_cost";
        tableModel->setTable(costkinds);
         tableModel->setFilter(QString("community_cost_status = '%1'").arg("已缴费"));
         if (!tableModel->select()) {
             qDebug() << "Error loading data:" << tableModel->lastError().text();
             return;
         }

         model4->removeRows(0, model4->rowCount());

         // 将QSqlTableModel的数据复制到QStandardItemModel
         for (int i = 0; i < tableModel->rowCount(); ++i) {
             QList<QStandardItem*> rowItems;
             for (int j = 0; j < tableModel->columnCount(); ++j) {
                 QStandardItem* item = new QStandardItem(tableModel->data(tableModel->index(i, j)).toString());
                 rowItems.append(item);
             }
             model4->appendRow(rowItems);
         }

           ui->tableView_2->setModel(model4);
         // 调整表格列宽以适应内容
    }
    else if(costkinds=="车位费"){
        ui->tableView_2->setModel(nullptr);
        costkinds="parking";
        tableModel->setTable(costkinds);
         tableModel->setFilter(QString("缴费情况 = '%1'").arg("已缴费"));
         if (!tableModel->select()) {
             qDebug() << "Error loading data:" << tableModel->lastError().text();
             return;
         }

         model3->removeRows(0, model3->rowCount());

         // 将QSqlTableModel的数据复制到QStandardItemModel
         for (int i = 0; i < tableModel->rowCount(); ++i) {
             QList<QStandardItem*> rowItems;
             for (int j = 1; j < tableModel->columnCount(); ++j) {
                 if(j==5){
                     continue;
                 }
                 QStandardItem* item = new QStandardItem(tableModel->data(tableModel->index(i, j)).toString());
                 rowItems.append(item);
             }
             model3->appendRow(rowItems);
         }

         ui->tableView_2->setModel(model3);
         // 调整表格列宽以适应内容
    }else if(costkinds=="未缴物业费"){
        ui->tableView_2->setModel(nullptr);
        costkinds="community_cost";
        tableModel->setTable(costkinds);
         tableModel->setFilter(QString("community_cost_status = '%1'").arg("未缴费"));
         if (!tableModel->select()) {
             qDebug() << "Error loading data:" << tableModel->lastError().text();
             return;
         }

         model4->removeRows(0, model4->rowCount());

         // 将QSqlTableModel的数据复制到QStandardItemModel
         for (int i = 0; i < tableModel->rowCount(); ++i) {
             QList<QStandardItem*> rowItems;
             for (int j = 0; j < tableModel->columnCount(); ++j) {
                 QStandardItem* item = new QStandardItem(tableModel->data(tableModel->index(i, j)).toString());
                 rowItems.append(item);
             }
             model4->appendRow(rowItems);
         }

         ui->tableView_2->setModel(model4);
         // 调整表格列宽以适应内容
    }else if(costkinds=="未缴车位费"){
        ui->tableView_2->setModel(nullptr);
        costkinds="parking";
        tableModel->setTable(costkinds);
         tableModel->setFilter(QString("缴费情况 = '%1'AND status='1'").arg("未缴费"));
         if (!tableModel->select()) {
             qDebug() << "Error loading data:" << tableModel->lastError().text();
             return;
         }

         model3->removeRows(0, model3->rowCount());

         // 将QSqlTableModel的数据复制到QStandardItemModel
         for (int i = 0; i < tableModel->rowCount(); ++i) {
             QList<QStandardItem*> rowItems;
             for (int j = 1; j < tableModel->columnCount(); ++j) {
                 if(j==5){
                     continue;
                 }
                 QStandardItem* item = new QStandardItem(tableModel->data(tableModel->index(i, j)).toString());
                 rowItems.append(item);
             }
             model3->appendRow(rowItems);
         }

         ui->tableView_2->setModel(model3);
         // 调整表格列宽以适应内容
    }

    // 执行查询
}


