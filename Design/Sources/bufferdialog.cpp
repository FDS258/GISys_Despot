#include "bufferdialog.h"
#include "ui_bufferdialog.h"

bufferDialog::bufferDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bufferDialog)
{
    ui->setupUi(this);
}

bufferDialog::~bufferDialog()
{
    delete ui;
}

void bufferDialog::on_buttonBox_accepted()
{
    p->r=ui->lineEdit->text().toInt();
    p->buffertype=ui->comboBox->currentText();
    int size = p->data.size();
    int sizeTemp;
    //正向生成
    QPoint endDot,endDot1;
    QVector<QPoint> endP,temp;
    endDot = p->data[0];//正向第一点端点
    endDot1 = p->data[1];//第一点连接点
    endP.push_back(endDot);
    endP.push_back(endDot1);
    temp=p->endPoint(endP, p->r);
    //bufferResult.insert(bufferResult.end(), temp.begin(), temp.end());//添加到结果集中
    sizeTemp = temp.size();
    for (int i = 0; i < sizeTemp; i++) {
        p->bufferResult.push_back(temp[i]);
    }
    //中间各点
    QPoint dot1, dot2, dot3;
    QVector <QPoint>tempV;
    QPoint tempP;
    for (int i = 0; i < size - 2; i++)
     {
        temp.clear();
        dot1 = p->data[i];
        dot2 = p->data[i + 1];
        dot3 = p->data[i + 2];
        temp.push_back(dot1);
        temp.push_back(dot2);
        temp.push_back(dot3);
        if (p->lineJudge(temp))
        {//若共线
            tempP = p->line(temp, p->r);
            p->bufferResult.push_back(tempP);
        }
        else
        {
            int index;
            index = p->dotJudge(temp);//判断凸凹点
            if (index == 1)
            {//凸点
                tempV.clear();
                tempV = p->convexPoint(temp, p->r);
                //bufferResult.insert(bufferResult.end(), tempV.begin(), tempV.end());//添加到结果集中
                sizeTemp = tempV.size();
                for (int i = 0; i < sizeTemp; i++)
                {
                    p->bufferResult.push_back(tempV[i]);
                }
            }
            else if (index == -1)
            {
                tempV.clear();
                tempV = p->concavePoint(temp, p->r);
                //bufferResult.insert(bufferResult.end(), tempV.begin(), tempV.end());//添加到结果集中
                sizeTemp = tempV.size();
                for (int i = 0; i < sizeTemp; i++)
                {
                    p->bufferResult.push_back(tempV[i]);
                }
            }
            else
            {
                qDebug() << "共线" << endl;
            }
        }
    }
        //反向生成
        endDot = p->data[size-1];//反向第一点端点
        endDot1 = p->data[size-2];//第一点连接点
        endP.clear();
        endP.push_back(endDot);
        endP.push_back(endDot1);
        temp.clear();
        temp = p->endPoint(endP, p->r);
        //bufferResult.insert(bufferResult.end(), temp.begin(), temp.end());//添加到结果集中
        sizeTemp = temp.size();
        for (int i = 0; i < sizeTemp; i++)
        {
            p->bufferResult.push_back(temp[i]);
        }
        //中间各点
        for (int i = size-1; i > 1; i--)
        {
            temp.clear();
            dot1 = p->data[i];
            dot2 = p->data[i - 1];
            dot3 = p->data[i - 2];
            temp.push_back(dot1);
            temp.push_back(dot2);
            temp.push_back(dot3);
            if (p->lineJudge(temp))
            {//若共线
                 tempP = p->line(temp, p->r);
                p->bufferResult.push_back(tempP);
            }
            else
            {
                int index;
                index = p->dotJudge(temp);//判断凸凹点
                if (index == 1)
                {//凸点
                    tempV.clear();
                    tempV = p->convexPoint(temp, p->r);
                    //bufferResult.insert(bufferResult.end(), tempV.begin(), tempV.end());//添加到结果中
                    sizeTemp = tempV.size();
                    for (int i = 0; i < sizeTemp; i++)
                    {
                        p->bufferResult.push_back(tempV[i]);
                    }
                }
                else if (index == -1)
                {
                    tempV.clear();
                    tempV = p->concavePoint(temp, p->r);
                    //bufferResult.insert(bufferResult.end(), tempV.begin(), tempV.end());//添加到结果中
                    sizeTemp = tempV.size();
                    for (int i = 0; i < sizeTemp; i++)
                    {
                        p->bufferResult.push_back(tempV[i]);
                    }
                }
                else
                {
                    qDebug() << "共线" << endl;
                }
            }
        }
        QPoint first = p->bufferResult[0];
        p->bufferResult.push_back(first);
    text="缓冲区分析成功";
    p->trigger=150;
    p->show();
}
