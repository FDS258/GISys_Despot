#ifndef COORDINATERANGEDIALOG_H
#define COORDINATERANGEDIALOG_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class coordinaterangeDialog;
}

class coordinaterangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit coordinaterangeDialog(QWidget *parent = nullptr);
    ~coordinaterangeDialog();

    MainWindow *p;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::coordinaterangeDialog *ui;
};

#endif // COORDINATERANGEDIALOG_H
