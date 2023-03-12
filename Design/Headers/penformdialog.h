  #ifndef PENFORMDIALOG_H
#define PENFORMDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class penFormDialog;
}

class penFormDialog : public QDialog
{
    Q_OBJECT

public:
    explicit penFormDialog(QWidget *parent = nullptr);
    ~penFormDialog();

    MainWindow *p;

    QColor c;

public slots:
    void on_buttonBox_accepted();

    void on_pushButton_clicked();

private:
    Ui::penFormDialog *ui;

signals:
     void sendcolor(QString);
};




#endif // PENFORMDIALOG_H
