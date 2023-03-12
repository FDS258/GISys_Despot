#ifndef CLUSTERDIALOG_H
#define CLUSTERDIALOG_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class clusterDialog;
}

class clusterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit clusterDialog(QWidget *parent = nullptr);
    ~clusterDialog();

    MainWindow *p;
private slots:
    void on_buttonBox_accepted();

private:
    Ui::clusterDialog *ui;
};

#endif // CLUSTERDIALOG_H
