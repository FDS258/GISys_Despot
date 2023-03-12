#ifndef DATABASEDIALOG_H
#define DATABASEDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class dataBaseDialog;
}

class dataBaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit dataBaseDialog(QWidget *parent = nullptr);
    ~dataBaseDialog();

    MainWindow *p;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::dataBaseDialog *ui;
};

#endif // DATABASEDIALOG_H
