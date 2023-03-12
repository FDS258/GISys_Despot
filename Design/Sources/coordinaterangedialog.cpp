#include "coordinaterangedialog.h"
#include "ui_coordinaterangedialog.h"

coordinaterangeDialog::coordinaterangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::coordinaterangeDialog)
{
    ui->setupUi(this);
}

coordinaterangeDialog::~coordinaterangeDialog()
{
    delete ui;
}

void coordinaterangeDialog::on_buttonBox_accepted()
{
    p->east=ui->lineEdit_east->text().toDouble();
    p->west=ui->lineEdit_west->text().toDouble();
    p->south=ui->lineEdit_south->text().toDouble();
    p->north=ui->lineEdit_north->text().toDouble();
    p->show();
}
