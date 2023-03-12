#include "mainwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QtGui>
#include <QString>
#include <QTextCodec>
#include <QtSql>
#include <QTextCodec>
#include <QSplashScreen>
#include <shellapi.h>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //加载并显示启动画面
    QSplashScreen splash(QPixmap("C:\\Users\\31808\\Documents\\gisSystem\\start.png"));
    splash.setDisabled(true); //禁用用户的输入事件响应
    splash.show();
    splash.showMessage(QObject::tr("欢迎使用gisSystem"),Qt::AlignLeft|Qt::AlignBottom,Qt::red);
    splash.showMessage(QObject::tr("加载地图数据..."),Qt::AlignLeft|Qt::AlignBottom,Qt::black);
    splash.showMessage(QObject::tr("加载数据库..."),Qt::AlignLeft|Qt::AlignBottom,Qt::black);
    //同时创建主视图对象
    MainWindow w;
    splash.showMessage(QObject::tr("主视窗正在启动中..."),Qt::AlignLeft|Qt::AlignBottom,Qt::black);
    w.show();
    splash.finish(&w);
    w.openDatabase();
    return a.exec();
}

