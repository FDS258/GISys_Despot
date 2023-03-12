#include "texteditdialog.h"
#include "ui_texteditdialog.h"

textEditDialog::textEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::textEditDialog)
{
    ui->setupUi(this);
}

textEditDialog::~textEditDialog()
{
    delete ui;
}

void textEditDialog::on_buttonBox_accepted()
{
    p->currenttext=ui->lineEdit->text();
    p->show();
}
