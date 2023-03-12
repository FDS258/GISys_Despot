#include "sqleditdialog.h"
#include "ui_sqleditdialog.h"

sqleditDialog::sqleditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sqleditDialog)
{
    ui->setupUi(this);
}

sqleditDialog::~sqleditDialog()
{
    delete ui;
}

void sqleditDialog::on_buttonBox_accepted()
{
    p->columnName=ui->lineEdit->text();
    p->tableType=ui->comboBox->currentText();
    p->tableName=ui->lineEdit_2->text();
    if(p->tableName!="")
    {
        QString str=QString("CREATE TABLE %1(startPnx float,startPny float,endPntx float,endPntx float,gType char(50),text char(50),order int)").arg(p->tableName);
        QSqlQuery query;
        query.exec(str);
    }
    if(p->columnName!="")
    {
        QString str=QString("ALTER TABLE %1 ADD '%2' '%3' NULL").arg(p->tableName).arg(p->columnName).arg(p->tableType);
        QSqlQuery query;
        query.exec(str);
    }
    p->show();
}
