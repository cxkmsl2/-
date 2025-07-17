// Community/main.cpp
#include "mainwindow.h"
#include <QApplication>
#include "loginwindow.h"
#include "worker.h" // 假设存在这些窗口类头文件
#include "manager.h"
#include "owner.h"
#include"manager.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    loginwindow l;

    Manager adminWindow;
    Worker staffWindow;
    Owner ownerWindow;

    QObject::connect(&l, &loginwindow::showAdminMain, &adminWindow, &Manager::show);
    QObject::connect(&l, &loginwindow::showStaffMain, &staffWindow, &Worker::show);

    l.show();

   //业主个人中心展示
    QObject::connect(&l,&loginwindow::showOwnerMain,[ &l,&ownerWindow](){l.tryon(ownerWindow);ownerWindow.show();});
    //展示Worker的repair表   &&&  Worker的个人中心
    QObject::connect(&l, &loginwindow::showStaffMain, [ &l,&staffWindow](){l.show_repair(staffWindow);staffWindow.show();l.showWorker(staffWindow);});


    return a.exec();
}
