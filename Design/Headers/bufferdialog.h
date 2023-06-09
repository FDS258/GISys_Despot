﻿#ifndef BUFFERDIALOG_H
#define BUFFERDIALOG_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class bufferDialog;
}

class bufferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit bufferDialog(QWidget *parent = nullptr);
    ~bufferDialog();

    MainWindow *p=new MainWindow;
    QString text="正在进行缓冲区分析";

private slots:
    void on_buttonBox_accepted();

private:
    Ui::bufferDialog *ui;
};

#endif // BUFFERDIALOG_H
