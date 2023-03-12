#ifndef TEXTEDITDIALOG_H
#define TEXTEDITDIALOG_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class textEditDialog;
}

class textEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit textEditDialog(QWidget *parent = nullptr);
    ~textEditDialog();
    MainWindow *p;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::textEditDialog *ui;
};

#endif // TEXTEDITDIALOG_H
