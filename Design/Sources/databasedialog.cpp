#include "databasedialog.h"
#include "ui_databasedialog.h"

dataBaseDialog::dataBaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dataBaseDialog)
{
    ui->setupUi(this);
}

dataBaseDialog::~dataBaseDialog()
{
    delete ui;
}

void dataBaseDialog::on_buttonBox_accepted()
{
    p->dataBaseName=ui->lineEdit->text();
    p->dbuserName=ui->lineEdit_2->text();
    p->dbpassWord=ui->lineEdit_3->text();
    p->show();
    p->openDatabase();
}
