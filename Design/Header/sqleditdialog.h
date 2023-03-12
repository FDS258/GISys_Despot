#ifndef SQLEDITDIALOG_H
#define SQLEDITDIALOG_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class sqleditDialog;
}

class sqleditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit sqleditDialog(QWidget *parent = nullptr);
    ~sqleditDialog();

    MainWindow *p;
private slots:
    void on_buttonBox_accepted();

private:
    Ui::sqleditDialog *ui;
};

#endif // SQLEDITDIALOG_H
