#include "penformdialog.h"
#include "ui_penformdialog.h"
#include <QColorDialog>

penFormDialog::penFormDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::penFormDialog)
{
    ui->setupUi(this);
}

penFormDialog::~penFormDialog()
{
    delete ui;
}

void penFormDialog::on_buttonBox_accepted()
{
    if(ui->comboBox_3->currentText()=="全局")
    {
        p->drawPattern=ui->comboBox->currentText();
        p->linePattern=ui->comboBox_2->currentText();
        p->fontPattern=ui->fontComboBox->currentFont();
        p->color=c;
        p->wid=ui->lineEdit->text().toInt();
        p->fontPattern.setPointSize(ui->spinBox->value());
        p->show();
        ui->fontComboBox->setCurrentFont(p->fontPattern);
        ui->spinBox->setValue(p->fontPattern.pointSize());
    }
    else if(ui->comboBox_3->currentText()=="框选")
    {
        p->seledrawPattern=ui->comboBox->currentText();
        p->selelinePattern=ui->comboBox_2->currentText();
        p->selefontPattern=ui->fontComboBox->currentFont();
        p->selecolor=c;
        p->selewid=ui->lineEdit->text().toInt();
        p->selefontPattern.setPointSize(ui->spinBox->value());
        p->show();
        ui->fontComboBox->setCurrentFont(p->selefontPattern);
        ui->spinBox->setValue(p->selefontPattern.pointSize());
    }
}

void penFormDialog::on_pushButton_clicked()
{
    c = QColorDialog::getColor(p->color, this,tr("选择编辑颜色"),QColorDialog::ShowAlphaChannel);
}

