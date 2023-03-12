#include "clusterdialog.h"
#include "ui_clusterdialog.h"

clusterDialog::clusterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::clusterDialog)
{
    ui->setupUi(this);
}

clusterDialog::~clusterDialog()
{
    delete ui;
}

void clusterDialog::on_buttonBox_accepted()
{
    p->threshold=ui->comboBox->currentText().toInt();
    p->minptnums=ui->lineEdit->text().toInt();
    for (int i=0;i<p->pnts.size();i++)
    {
        dbscanpoint db(p->pnts[i].x(),p->pnts[i].y());
        p->Inpts.append(db);
    }
    p->clusterpnts=p->getCluster(p->Inpts,p->threshold,p->minptnums,p->clusternum);
    update();
    p->trigger=200;
    p->show();
}
