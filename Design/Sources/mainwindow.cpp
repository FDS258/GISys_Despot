#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QMessageBox>
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QPixmap>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QColorDialog>
#include <QString>
#include <QFileDialog>
#include <QtSvg/QSvgGenerator>
#include <QSqlQueryModel>
#include "penformdialog.h"
#include "databasedialog.h"
#include "texteditdialog.h"
#include "clusterdialog.h"
#include "bufferdialog.h"
#include "coordinaterangedialog.h"
#include "sqleditdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    installEventFilter(this);
    this->setMouseTracking(true); // 这个只有click鼠标后移动要素才会调用鼠标move的事件，否则不用按也会移动要素
    this->isPressed = false;
    screenwToe=this->width()-ui->treeWidget->x();
    screensTon=this->height()-ui->treeWidget->y();
    currentLongitude=west+((cursor().pos().x()-ui->treeWidget->pos().x())/screenwToe*wToe);
    currentLatitude=north-((cursor().pos().y()-ui->treeWidget->pos().y())*sTon/screensTon);
    menu=new QMenu();
    menu->addSeparator();
    menu->setStyleSheet("border:0.5px solid gray;background-color:white;color:black;");
    createMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete menu;
}

//点选数据判断
void MainWindow::clickDataSet()
{
    movep=cursor().pos();
    //手绘板线
    for(unsigned int i=0;i<handLines.size();i++)
    {
    }

    //点
    for(int i=0;i<pnts.size();i++)
    {
        QPointF p1=cursor().pos();
        p1.setX(p1.x()+5);
        p1.setY(p1.y()+5);
        QPointF p2=cursor().pos();
        p2.setX(p2.x()-5);
        p2.setY(p2.y()-5);
        QRectF r;
        r.setTopLeft(p2);
        r.setBottomRight(p1);
        if(r.contains(pnts[i]))
        {
            clicklines.append(&lines[i]);
        }
    }

    //线
    for(int i=0;i<lines.size();i++)
    {
        if((cursor().pos().x()-lines[i].x1())/(lines[i].x2()-lines[i].x1())==(cursor().pos().y()-lines[i].y1())/(lines[i].y2()-lines[i].y1()))
        {
            clicklines.append(&lines[i]);
        }
    }

    //矩形
    for(int i=0;i<rects.size();i++)
    {
        if(rects[i].contains(cursor().pos()))
        {
            clickrects.append(&rects[i]);
        }
    }

    //圆
    for(int i=0;i<ellipses.size();i++)
    {
        if(ellipses[i].contains(cursor().pos()))
        {
            clickellipses.append(&ellipses[i]);
        }
    }
}

//打开数据库
void MainWindow::openDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");   //数据库驱动类型为SQL Server
    qDebug()<<"ODBC driver?"<<db.isValid();
    db.setHostName("localhost");                         //选择本地主机，127.0.1.1
    db.setDatabaseName(dataBaseName);                    //设置数据源名称
    db.setUserName(dbuserName);                            //登录用户
    db.setPassword(dbpassWord);                            //登录密码
    if(!db.open())
    {
        qDebug()<<db.lastError().text();
        ui->textEdit->setText("数据库打开失败");
        QMessageBox::warning(this,tr("警告"),tr("数据库连接失败\n"),QMessageBox::Ok);
    }
    else
    {
        qDebug()<<"database open success!";
        ui->textEdit->setText("数据库打开成功");
    }
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("select * from basicGraphs");
    ui->tableView->setModel(model);
    //隔行变色
    ui->tableView->setAlternatingRowColors(true);
}

//计算经度
double MainWindow::getLongitude(QPointF p)
{
    double longitude=west+((p.x()-ui->treeWidget->pos().x())/screenwToe*wToe);
    return longitude;
}

//计算纬度
double MainWindow::getLatitude(QPointF p)
{
    double latitude=north-((p.y()-ui->treeWidget->pos().y())*sTon/screensTon);
    return latitude;
}

//计算实地距离
double MainWindow::RealDistance(double lat1, double lng1, double lat2, double lng2)
{
    double a;
    double b;
    double radLat1 = rad(lat1);
    double radLat2 = rad(lat2);
    a = radLat1 - radLat2;
    b = rad(lng1) - rad(lng2);
    double s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
    s = s * EARTH_RADIUS;
    s = s * 1000;
    return s;
}

//重写绘图事件
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QPixmap pix(this->width(),this->height());
    pix.fill(Qt::white);//用白色填充
    QPainter painter(&pix);
    //创建画笔
    QPen pen;
    QBrush brush;

    //绘图
    if(trigger>0)
    {
        //普通绘制
        pen.setWidth(wid);
        pen.setColor(color);
        brush.setColor(color);
        pen.setStyle(Qt::SolidLine);
        painter.setFont(fontPattern);

        //设置填充样式
        if(drawPattern=="实心")
        {
            brush.setStyle(Qt::SolidPattern);
            painter.setBrush(brush);
        }
        else if (drawPattern=="渐变1")
        {
            brush.setStyle(Qt::Dense1Pattern);
            painter.setBrush(brush);
        }
        else if (drawPattern=="渐变2")
        {
            brush.setStyle(Qt::Dense2Pattern);
            painter.setBrush(brush);
        }
        else if (drawPattern=="渐变3")
        {
            brush.setStyle(Qt::Dense3Pattern);
            painter.setBrush(brush);
        }
        else if (drawPattern=="渐变4")
        {
            brush.setStyle(Qt::Dense4Pattern);
            painter.setBrush(brush);
        }
        else if (drawPattern=="渐变5")
        {
            brush.setStyle(Qt::Dense5Pattern);
            painter.setBrush(brush);
        }
        else if (drawPattern=="渐变6")
        {
            brush.setStyle(Qt::Dense6Pattern);
            painter.setBrush(brush);
        }
        else if (drawPattern=="渐变7")
        {
            brush.setStyle(Qt::Dense7Pattern);
            painter.setBrush(brush);
        }
        else
        {
            painter.setPen(pen);
        }

        //设置画笔样式
        if(linePattern=="虚线")
        {
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);
        }
        else if (linePattern=="点虚线")
        {
            pen.setStyle(Qt::DashDotLine);
            painter.setPen(pen);
        }
        else if(linePattern=="传统虚线")
        {
            pen.setStyle(Qt::CustomDashLine);
            painter.setPen(pen);
        }
        else if (linePattern=="两点虚线")
        {
            pen.setStyle(Qt::DashDotDotLine);
            painter.setPen(pen);
        }
        else if (linePattern=="点线")
        {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }
        else
        {
            painter.setPen(pen);
        }

        if(isPressed)
        {
            painter.drawLine(currentline);
            painter.drawRect(currentrect);
            painter.drawEllipse(currentellipse);
        }

        //点击要素绘制
        //点击要素线
        for(int i=0;i<clicklines.size();i++)
        {
            QLineF line=*clicklines[i];
            painter.drawLine(line.x1()+movex,line.x2()+movex,line.y1()+movey,line.y2()+movey);
        }

        //点击要素矩形
        for(int i=0;i<clickrects.size();i++)
        {
            QRectF rect=*clickrects[i];
            rect.setTop(clickrects[i]->top()+movey);
            rect.setBottom(clickrects[i]->bottom()+movey);
            rect.setLeft(clickrects[i]->left()+movex);
            rect.setRight(clickrects[i]->right()+movex);
            painter.drawRect(rect);
        }

        //点击要素圆
        for(int i=0;i<clickellipses.size();i++)
        {
            QRectF ellipse=*clickellipses[i];
            ellipse.setTop(clickrects[i]->top()+movey);
            ellipse.setBottom(clickrects[i]->bottom()+movey);
            ellipse.setLeft(clickrects[i]->left()+movex);
            ellipse.setRight(clickrects[i]->right()+movex);
            painter.drawEllipse(ellipse);
        }

        //点击要素多段线
        for(int i=0;i<clickpolylines.size();i++)
        {
            for(int j=1;j<clickpolylines[i].size();j++)
            {
                QPointF p1=clickpolylines[i][j-1];
                QPointF p2=clickpolylines[i][j];
                painter.drawLine(p1,p2);
            }
        }

        //点击要素多边形
        for(int i=0;i<clickpolygons.size();i++)
        {
            for(int j=1;j<clickpolygons[i].size();j++)
            {
                QPointF p1=clickpolygons[i][j-1];
                QPointF p2=clickpolygons[i][j];
                painter.drawLine(p1,p2);
            }
            painter.drawLine(clickpolygons[i][0],clickpolygons[i][clickpolygons[i].size()-1]);
        }

        //改变画笔
        pen.setWidth(wid+5);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);

        //点击点
        for(int i = 0;i<clickpnts.size();i++)
        {
            painter.drawPoint(clickpnts[i]->x()+movex,clickpnts[i]->y()+movey);
        }

        //点击要素注记
        for(int i=0;i<clicktexts.size();i++)
        {
            QString text=*clicktexts[i];
            QPointF textLoc=*clicktextsLoc[i];
            painter.drawPoint(textLoc);
            painter.drawText(textLoc,text);
        }

        //普通要素
        //改变画笔
        pen.setWidth(wid);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);

        //手绘板线
        for(unsigned int i=0;i<handLines.size();i++)
        {
            myLine* pLine = handLines[i];
            painter.drawLine(pLine->startPnt,pLine->endPnt);
        }

        //绘制线
        for(int i=0;i<lines.size();i++)
        {
            QLineF line=lines[i];
            painter.drawLine(line);
            painter.drawText((line.p1()+line.p2())/2,"L"+QString::number(i+1));
            painter.drawText(line.p1(),"P1");
            painter.drawText(line.p2(),"P2");
        }

        //绘制矩形
        for(int i=0;i<rects.size();i++)
        {
            QRectF rect=rects[i];
            painter.drawRect(rect);
            painter.drawText((rect.topLeft()+rect.bottomRight())/2,"Rect"+QString::number(i+1));
            painter.drawText(rect.topLeft(),"P1");
            painter.drawText(rect.topRight(),"P2");
            painter.drawText(rect.bottomLeft(),"P3");
            painter.drawText(rect.bottomRight(),"P4");
        }

        //绘制圆
        for(int i=0;i<ellipses.size();i++)
        {
            QRectF ellipse=ellipses[i];
            painter.drawEllipse(ellipse);
            painter.drawText((ellipse.topLeft()+ellipse.bottomRight())/2,"Circle"+QString::number(i+1));
        }

        //绘制多段线
        if(disFlag==false)
        {
            for(int k=1;k<currentPolyline.size();k++)
            {
                painter.drawLine(currentPolyline[k-1],currentPolyline[k]);
                painter.drawText(currentPolyline[k-1],"P"+QString::number(k-1));
                painter.drawText(currentPolyline[k],"P"+QString::number(k));
            }
        }
        else
        {
            for(int k=1;k<currentPolyline.size();k++)
            {
                double dis=RealDistance(getLatitude(currentPolyline[k-1]),getLongitude(currentPolyline[k-1]),getLatitude(currentPolyline[k]),getLongitude(currentPolyline[k]));
                painter.drawLine(currentPolyline[k-1],currentPolyline[k]);
                painter.drawText(currentPolyline[k-1],"P"+QString::number(k-1));
                painter.drawText(currentPolyline[k],"P"+QString::number(k));
                painter.drawText((currentPolyline[k]+currentPolyline[k-1])/2,"P"+QString::number(k-1)+"P"+QString::number(k)+":"+QString::number(dis));
            }
        }

        for(int i=0;i<polylines.size();i++)
        {
            if(disFlag==false)
            {
                for(int j=1;j<polylines[i].size();j++)
                {
                    QPointF p1=polylines[i][j-1];
                    QPointF p2=polylines[i][j];
                    painter.drawLine(p1,p2);
                    painter.drawText(p1,"P"+QString::number(j-1));
                    painter.drawText(p2,"P"+QString::number(j));
                }
            }
            else
            {
                for(int j=1;j<polylines[i].size();j++)
                {
//                    QString::fromLocal8Bit("%1").arg(1.1);
//                    QString::fromStdString(to_string());
                    QPointF p1=polylines[i][j-1];
                    QPointF p2=polylines[i][j];
                    double dis=RealDistance(getLatitude(p1),getLongitude(p1),getLatitude(p2),getLongitude(p2));
                    painter.drawLine(p1,p2);
                    painter.drawText(p1,"P"+QString::number(j-1));
                    painter.drawText(p2,"P"+QString::number(j));
                    painter.drawText((p1+p2)/2,"P"+QString::number(j-1)+"P"+QString::number(j)+":"+QString::number(dis));
                }
            }
        }

        //绘制多边形
        for(int k=1;k<currentPolygon.size();k++)
        {
            painter.drawLine(currentPolygon[k-1],currentPolygon[k]);
            painter.drawText(currentPolygon[k-1],"P"+QString::number(k-1));
            painter.drawText(currentPolygon[k],"P"+QString::number(k));
        }
        for(int i=0;i<polygons.size();i++)
        {
            for(int j=1;j<polygons[i].size();j++)
            {
                QPointF p1=polygons[i][j-1];
                QPointF p2=polygons[i][j];
                painter.drawLine(p1,p2);
                painter.drawText(p1,"P"+QString::number(j-1));
                painter.drawText(p2,"P"+QString::number(j));
            }
            painter.drawLine(polygons[i][0],polygons[i][polygons[i].size()-1]);
        }

        //绘制缓冲区数据
        if(trigger==150)
        {
            int size = data.size();
            for (int i = 0; i < size - 1; i++) {

                int x1, y1, x2, y2;
                x1 = data[i].x();
                y1 = data[i].y();
                x2 = data[i + 1].x();
                y2 = data[i + 1].y();
                painter.drawLine(x1, y1, x2, y2);
            }
            int sizeR = bufferResult.size();
            for (int i = 0; i < sizeR-1; i++) {
                int Rx1,Rx2,Ry1, Ry2;
                Rx1 = bufferResult[i].x();
                Rx2 = bufferResult[i+1].x();
                Ry1 = bufferResult[i].y();
                Ry2 = bufferResult[i + 1].y();
                painter.drawLine(Rx1, Ry1,Rx2, Ry2);
            }
        }

        //绘制b样条线
        for (int i = 0; i < ctrlPoints.size(); i++)
        {
            painter.drawPoint(ctrlPoints[i]);
        }

//        QPen ctrlPen2(QColor(255, 0, 0));
//        ctrlPen2.setWidth(1);
//        ctrlPen2.setStyle(Qt::DashDotDotLine);
//        painter.setPen(ctrlPen2);
        pen.setStyle(Qt::DashDotDotLine);
        for (int i = 0; i < ctrlPoints.size() - 1; i++)
        {
            painter.drawLine(ctrlPoints[i], ctrlPoints[i + 1]);
        }

//        QPen curvePen(QColor(0, 0, 0));
//        curvePen.setWidth(2);
//        painter.setPen(curvePen);

        for (int i = 0; i < curvePoints.size() - 1; i++)
        {
            painter.drawLine(curvePoints[i], curvePoints[i + 1]);
        }

        pen.setStyle(Qt::SolidLine);

        if (showCtrlNode)
        {
//            QPen pen1(QColor(0, 0, 0));
//            painter.setPen(pen1);
//            painter.setFont(QFont("Consolas", 12));
            for (int i = 0; i < ctrlPoints.size(); i++)
            {
                QPointF t(ctrlPoints[i].x() + 20, ctrlPoints[i].y());
                painter.drawText(t, QString("P%1").arg(i));
            }
        }


        if (showCurveNode)
        {
//            QPen pen2(QColor(0, 0, 0));
//            pen2.setWidth(5);
//            painter.setPen(pen2);
//            painter.setFont(QFont("Consolas", 12));
            for (double u = currentK; u <= ctrlPoints.size(); u += 1)
            {
                QPointF tmp(10, 20);
                for (int i = 0; i < ctrlPoints.size(); i++)
                {
                    QPointF t = ctrlPoints[i];

                    t *= N(currentK, i, u);

                    tmp += t;
                }
                painter.drawText(tmp, QString("u=%1").arg(u));
                painter.drawPoint(tmp.x() - 10, tmp.y() - 20);
            }
        }

        //改变画笔
        pen.setWidth(wid+5);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);

        //绘制点
        for(int i = 0;i<pnts.size();i++)
        {
            painter.drawPoint(pnts[i]);
            painter.drawText(pnts[i],"Pnt"+QString::number(i+1));
        }

        //绘制注记
        for(int i=0;i<texts.size();i++)
        {
            QString text=texts[i];
            QPointF textLoc=textsLoc[i];
            painter.drawText(textLoc,text);
            painter.drawPoint(textLoc);
        }

        //聚类点绘制
        if(trigger==200)
        {
            for (int i=0;i<clusterpnts.size();i++)
            {
                if(clusterpnts[i].ncluster%2==0)
                {
                    pen.setColor(Qt::blue);
                }
                else if(clusterpnts[i].ncluster==clusternum-1)
                {
                    pen.setColor(Qt::green);
                }
                else
                {
                    pen.setColor(Qt::yellow);
                }
                painter.setPen(pen);
                painter.drawPoint(clusterpnts[i].x,clusterpnts[i].y);
            }
        }

        //框选绘制
        pen.setWidth(8);
        pen.setColor(selecolor);
        pen.setStyle(Qt::DashLine);//选择框线形为虚线
        painter.setPen(pen);
        brush.setStyle(Qt::NoBrush);
        painter.setBrush(brush);
        painter.drawRect(seleRect);

        pen.setWidth(selewid);
        brush.setColor(selecolor);

        pen.setStyle(Qt::SolidLine);

        painter.setFont(fontPattern);

        //设置填充样式
        if(seledrawPattern=="实心")
        {
            brush.setStyle(Qt::SolidPattern);
            painter.setBrush(brush);
        }
        else if (seledrawPattern=="渐变1")
        {
            brush.setStyle(Qt::Dense1Pattern);
            painter.setBrush(brush);
        }
        else if (seledrawPattern=="渐变2")
        {
            brush.setStyle(Qt::Dense2Pattern);
            painter.setBrush(brush);
        }
        else if (seledrawPattern=="渐变3")
        {
            brush.setStyle(Qt::Dense3Pattern);
            painter.setBrush(brush);
        }
        else if (seledrawPattern=="渐变4")
        {
            brush.setStyle(Qt::Dense4Pattern);
            painter.setBrush(brush);
        }
        else if (seledrawPattern=="渐变5")
        {
            brush.setStyle(Qt::Dense5Pattern);
            painter.setBrush(brush);
        }
        else if (seledrawPattern=="渐变6")
        {
            brush.setStyle(Qt::Dense6Pattern);
            painter.setBrush(brush);
        }
        else if (seledrawPattern=="渐变7")
        {
            brush.setStyle(Qt::Dense7Pattern);
            painter.setBrush(brush);
        }
        else
        {
            painter.setPen(pen);
        }

        //设置画笔样式
        if(selelinePattern=="虚线")
        {
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);
        }
        else if (selelinePattern=="点虚线")
        {
            pen.setStyle(Qt::DashDotLine);
            painter.setPen(pen);
        }
        else if(selelinePattern=="传统虚线")
        {
            pen.setStyle(Qt::CustomDashLine);
            painter.setPen(pen);
        }
        else if (selelinePattern=="两点虚线")
        {
            pen.setStyle(Qt::DashDotDotLine);
            painter.setPen(pen);
        }
        else if (selelinePattern=="点线")
        {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
        }
        else
        {
            painter.setPen(pen);
        }

        //框选线
        for(int i=0;i<selelines.size();i++)
        {
            QLineF line=*selelines[i];
            painter.drawLine(line.x1()+movex,line.y1()+movey,line.x2()+movex,line.y2()+movey);
        }

        //框选矩形
        for(int i=0;i<selerects.size();i++)
        {
            QRectF rect=*selerects[i];
            rect.setTop(selerects[i]->top()+movey);
            rect.setBottom(selerects[i]->bottom()+movey);
            rect.setLeft(selerects[i]->left()+movex);
            rect.setRight(selerects[i]->right()+movex);
            painter.drawRect(rect);
        }

        //框选圆
        for(int i=0;i<seleellipses.size();i++)
        {
            QRectF ellipse=*seleellipses[i];
            ellipse.setTop(seleellipses[i]->top()+movey);
            ellipse.setBottom(seleellipses[i]->bottom()+movey);
            ellipse.setLeft(seleellipses[i]->left()+movex);
            ellipse.setRight(seleellipses[i]->right()+movex);
            painter.drawEllipse(ellipse);
        }

        //框选多段线
        for(int i=0;i<selepolylines.size();i++)
        {
            for(int j=1;j<selepolylines[i].size();j++)
            {
                QPointF p1=selepolylines[i][j-1];
                QPointF p2=selepolylines[i][j];
                painter.drawLine(p1,p2);
            }
        }

        //框选多边形
        for(int i=0;i<selepolygons.size();i++)
        {
            for(int j=1;j<selepolygons[i].size();j++)
            {
                QPointF p1=selepolygons[i][j-1];
                QPointF p2=selepolygons[i][j];
                painter.drawLine(p1,p2);
            }
            painter.drawLine(selepolygons[i][0],selepolygons[i][selepolygons[i].size()-1]);
        }

        pen.setWidth(selewid+5);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);

        //框选点
        for(int i = 0;i<selepnts.size();i++)
        {
            QPointF pnt=*selepnts[i];
            pnt.setX(selepnts[i]->x()+movex);
            pnt.setY(selepnts[i]->y()+movey);
            painter.drawPoint(pnt);
        }

        //框选注记
        for(int i=0;i<seletexts.size();i++)
        {
            QString text=*seletexts[i];
            QPointF textLoc=*seletextsLoc[i];
            textLoc.setX(seletextsLoc[i]->x()+movex);
            textLoc.setY(seletextsLoc[i]->y()+movey);
            painter.drawText(textLoc,text);
            painter.drawPoint(textLoc);
        }
    }

    pixm=pix;
    p.drawPixmap(0,0,pix);
    painter.begin(this);
}

//重写鼠标点击事件
void MainWindow::mousePressEvent(QMouseEvent *e)
{
    ui->lineEdit->setText(QString::number(west+((e->pos().x()-ui->treeWidget->pos().x())/screenwToe*wToe))+","+QString::number(north-((e->pos().y()-ui->treeWidget->pos().y())*sTon/screensTon)));
    switch (trigger)
    {
    //绘制点
    case 1:
    {
        if(e->button()==Qt::LeftButton)
        {
            setCursor(Qt::PointingHandCursor);
            startPnt=e->pos();
            this->pnts.push_back(startPnt);
            this->isPressed = true;
        }
        break;
    }

    //绘制线
    case 2:
    {
        if (e->button()==Qt::LeftButton)
        {
            setCursor(Qt::PointingHandCursor);
            startPnt=e->pos();
            this->isPressed = true;
        }
        break;
    }

    //绘制圆
    case 3:
    {
        if (e->button()==Qt::LeftButton)
        {
            setCursor(Qt::PointingHandCursor);
            startPnt=e->pos();
            this->isPressed = true;
        }
        break;
    }

    //手绘板
    case 4:
    {
        if(e->button()==Qt::LeftButton)
        {
            setCursor(Qt::PointingHandCursor);
            startPnt = e->pos();
            endPnt = e->pos();
            this->isPressed = true;
        }
        break;
    }

    //绘制矩形
    case 5:
    {
        if(e->button()==Qt::LeftButton)
        {
            setCursor(Qt::PointingHandCursor);
            startPnt = e->pos();
            this->isPressed = true;
        }
        break;
    }

    //从数据库打开
    case 6:
    {
        if(e->button()==Qt::LeftButton)
        {
            setCursor(Qt::WaitCursor);
        }
        break;
    }

    //绘制注记
    case 7:
    {
        if(e->button()==Qt::LeftButton)
        {
            setCursor(Qt::PointingHandCursor);
            startPnt = e->pos();
            this->isPressed = true;
        }
        break;
    }

    //绘制多段线
    case 8:
    {
        if(e->button()==Qt::LeftButton)
        {
            setCursor(Qt::PointingHandCursor);
            startPnt=e->pos();
            currentPolyline.append(startPnt);
            update();
        }
        else if (e->button()==Qt::RightButton)
        {
            setCursor(Qt::ArrowCursor);
            endPnt=e->pos();
            polylines.append(currentPolyline);
            currentPolyline.clear();
            update();
        }
        break;
    }

    //绘制多边形
    case 9:
    {
        if(e->button()==Qt::LeftButton)
        {
            setCursor(Qt::PointingHandCursor);
            startPnt=e->pos();
            currentPolygon.append(startPnt);
            update();
        }
        else if (e->button()==Qt::RightButton)
        {
            setCursor(Qt::ArrowCursor);
            endPnt=e->pos();
            polygons.append(currentPolygon);
            currentPolygon.clear();
            update();
        }
        break;
    }

    //绘制b样条曲线
    case 10:
    {
        if (e->button() == Qt::LeftButton)
        {
            setCursor(QCursor(Qt::ClosedHandCursor));

            int i = getCurrentNode(e->pos());
            if (i >= 0)
            {
                currentNode = i;
                return;
            }

            ctrlPoints.push_back(e->pos());
            currentNode = ctrlPoints.size() - 1;
            generateCurve();
            update();
        }
        else if (e->button()==Qt::RightButton)
        {
            ctrlPoints.clear();
            curvePoints.clear();
        }
        break;
    }

    //点击移动要素
    case 50:
    {
        if (e->button()==Qt::LeftButton)
        {
            setCursor(Qt::SizeAllCursor);
            startPnt=e->pos();
            ui->textEdit->setText("移动要素状态");
            clickDataSet();
        }
        break;
    }
    }

    //框选绘制要素
    if(e->modifiers() == Qt::CTRL)
    {
        trigger=100;
        setCursor(Qt::SizeAllCursor);
        startPnt=e->pos();
        ui->textEdit->setText("框选状态");
    }

    //框选移动要素
    else if(e->modifiers()==Qt::SHIFT)
    {
        trigger=100;
        setCursor(Qt::SizeAllCursor);
        startPnt=e->pos();
        ui->textEdit->setText("框选状态");
    }

    //图幅刷新
    else if(e->modifiers()==Qt::Key_F5)
    {
        this->update();
        ui->textEdit->setText("已刷新");
    }

    //右键菜单
    else if(trigger!=8&&trigger!=9&&trigger!=10&&e->button()==Qt::RightButton)
    {
        setCursor(Qt::ArrowCursor);
        menu->move(cursor().pos());
        menu->show();
    }
}

//重写鼠标拖曳事件
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    ui->lineEdit->setText(QString::number(west+((e->pos().x()-ui->treeWidget->pos().x())/screenwToe*wToe))+","+QString::number(north-((e->pos().y()-ui->treeWidget->pos().y())*sTon/screensTon)));

    switch (trigger)
    {
    //绘制点
    case 1:
    {
        //绘制点
        if(this->isPressed)
        {
            setCursor(Qt::PointingHandCursor);
        }
        break;

    }

    //绘制线
    case 2:
    {
        if (this->isPressed)
        {
            setCursor(Qt::PointingHandCursor);
            endPnt = e->pos();
            currentline.setP1(startPnt);
            currentline.setP2(endPnt);
            update();
        }
        break;
    }

    //绘制圆
    case 3:
    {
        if (this->isPressed)
        {
            setCursor(Qt::PointingHandCursor);
            endPnt=e->pos();
            currentellipse.setTopLeft(startPnt);
            currentellipse.setBottomRight(endPnt);
            update();
        }
        break;
    }

    //手绘板
    case 4:
    {
        if(this->isPressed)
        {
            setCursor(Qt::PointingHandCursor);
            endPnt = e->pos();
            myLine* line = new myLine;  //put the new line into vector
            line->startPnt = startPnt;
            line->endPnt = endPnt;
            this->handLines.push_back(line);
            update();
            startPnt = endPnt;
        }
        break;
    }

    //绘制矩形
    case 5:
    {
        if(this->isPressed)
        {
            setCursor(Qt::PointingHandCursor);
            setCursor(Qt::PointingHandCursor);
            endPnt=e->pos();
            currentrect.setTopLeft(startPnt);
            currentrect.setBottomRight(endPnt);
            update();
        }
        break;
    }

    //从数据库打开
    case 6:
    {
        if(this->isPressed)
        {
            setCursor(Qt::PointingHandCursor);
        }
        break;
    }

    //绘制注记
    case 7:
    {
        if(this->isPressed)
        {
            setCursor(Qt::PointingHandCursor);
        }
        break;
    }

    //绘制多段线
    case 8:
    {
        if (this->isPressed)
        {
            setCursor(Qt::PointingHandCursor);
        }
        break;
    }

    //绘制多边形
    case 9:
    {
        if (this->isPressed)
        {
            setCursor(Qt::PointingHandCursor);
        }
        break;
    }

    //绘制b样条曲线
    case 10:
    {
        if (this->isPressed)
        {
            ctrlPoints.removeAt(currentNode);
            ctrlPoints.insert(currentNode, e->pos());
            generateCurve();
            update();
            return;
        }
        if (getCurrentNode(e->pos()) >= 0)
        {
            setCursor(QCursor(Qt::SizeAllCursor));
        }
        else
        {
            setCursor(QCursor(Qt::ArrowCursor));
        }
        break;
    }

    //点击移动要素
    case 50:
    {
        endPnt = e->pos();
        movex=endPnt.x()-startPnt.y();
        movey=endPnt.y()-startPnt.y();
        update();
        break;
    }
    }

    //框选绘制
    if(e->modifiers() == Qt::CTRL)
    {
        endPnt = e->pos();
        seleRect.setTopLeft(startPnt);
        seleRect.setBottomRight(endPnt);
        update();
    }

    //框选移动要素
    else if(e->modifiers() == Qt::SHIFT)
    {
        endPnt = e->pos();
        seleRect.setTopLeft(startPnt);
        seleRect.setBottomRight(endPnt);
        update();
    }
}

//重写鼠标释放事件
void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    ui->lineEdit->setText(QString::number(west+((e->pos().x()-ui->treeWidget->pos().x())/screenwToe*wToe))+","+QString::number(north-((e->pos().y()-ui->treeWidget->pos().y())*sTon/screensTon)));

    switch (trigger)
    {
    //绘制点
    case 1:
    {
        setCursor(Qt::ArrowCursor);
        this->isPressed = false;
        update();
        break;
    }

    //绘制线
    case 2:
    {
        setCursor(Qt::ArrowCursor);
        endPnt=e->pos();
        QLineF newline(startPnt,endPnt);
        this->lines.push_back(newline);
        this->isPressed = false;
        currentline.setLine(0,0,0,0);
        update();
        break;
    }

    case 3:
    {
        endPnt=e->pos();
        setCursor(Qt::ArrowCursor);
        QRectF newellipse(startPnt,endPnt);
        this->ellipses.push_back(newellipse);
        this->isPressed=false;
        currentellipse.setTop(0);
        currentellipse.setBottom(0);
        currentellipse.setLeft(0);
        currentellipse.setRight(0);
        update();
        break;
    }

    //手绘板
    case 4:
    {
        setCursor(Qt::ArrowCursor);
        this->isPressed = false;
        break;
    }

    //绘制矩形
    case 5:
    {
        endPnt=e->pos();
        setCursor(Qt::ArrowCursor);
        QRectF newrect(startPnt,endPnt);
        this->rects.push_back(newrect);
        this->isPressed=false;
        currentrect.setTop(0);
        currentrect.setBottom(0);
        currentrect.setLeft(0);
        currentrect.setRight(0);
        update();
        break;
    }

    //从数据库打开
    case 6:
    {
        setCursor(Qt::ArrowCursor);
        this->isPressed = false;
        break;
    }

    //绘制注记
    case 7:
    {
        setCursor(Qt::ArrowCursor);
        this->isPressed = false;
        texts.append(currenttext);
        textsLoc.append(startPnt);
        ui->actiontextEdit->setText("注记绘制");
        trigger=100;
        update();
        break;
    }

    //绘制多段线
    case 8:
    {
        break;
    }

    //绘制多边形
    case 9:
    {
        break;
    }

    //绘制b样条曲线
    case 10:
    {
       setCursor(QCursor(Qt::ArrowCursor));
       break;
    }

    //点击移动要素
    case 50:
    {
        endPnt = e->pos();
        movex=endPnt.x()-startPnt.y();
        movey=endPnt.y()-startPnt.y();
        setCursor(Qt::ArrowCursor);
        this->isPressed = false;
        update();
        ui->textEdit->setText(" ");
        ui->actionMove->setText("移动要素");
    }
    }

    //框选操作
    if(e->modifiers() == Qt::CTRL)
    {
        endPnt = e->pos();
        setCursor(Qt::ArrowCursor);
        this->isPressed = false;
        seleRect.setRect(0,0,0,0);
        ui->textEdit->setText(" ");

        //框选点
        for(int i=0;i<pnts.size();i++)
        {
            double startPntX=pnts[i].x();
            double startPntY=pnts[i].y();
            if(startPnt.x()<startPntX&&startPntX<endPnt.x()&&startPntY>startPnt.y()&&startPntY<endPnt.y())
            {
                QPoint pnt=pnts[i].toPoint();
                data.append(pnt);
                selepnts.append(&pnts[i]);
            }
        }

        //框选线
        for(int i=0;i<lines.size();i++)
        {
            QLineF line=lines[i];
            double startPntX=line.p1().x();
            double startPntY=line.p1().y();
            double endPntX=line.p2().x();
            double endPntY=line.p2().y();
            if(startPnt.x()<startPntX&&startPntX<endPnt.x()&&startPntY>startPnt.y()&&startPntY<endPnt.y()&&startPnt.x()<endPntX&&endPntX<endPnt.x()&&endPntY>startPnt.y()&&endPntY<endPnt.y())
            {
                QLineF l(startPntX,startPntY,endPntX,endPntY);
                selelines.append(&l);
            }
        }

        //框选矩形
        for(int i=0;i<rects.size();i++)
        {
            QRectF rect=rects[i];
            double startPntX=rect.topLeft().x();
            double startPntY=rect.topLeft().y();
            double endPntX=rect.bottomRight().x();
            double endPntY=rect.bottomRight().y();
            if(startPnt.x()<startPntX&&startPntX<endPnt.x()&&startPntY>startPnt.y()&&startPntY<endPnt.y()&&startPnt.x()<endPntX&&endPntX<endPnt.x()&&endPntY>startPnt.y()&&endPntY<endPnt.y())
            {
                QPointF tl(startPntX,startPntY);
                QPointF br(endPntX,endPntY);
                QRectF r(tl,br);
                selerects.append(&rects[i]);
            }
        }

        //框选圆
        for(int i=0;i<ellipses.size();i++)
        {
            QRectF ellipse=ellipses[i];
            double startPntX=ellipse.topLeft().x();
            double startPntY=ellipse.topLeft().y();
            double endPntX=ellipse.bottomRight().x();
            double endPntY=ellipse.bottomRight().y();
            if(startPnt.x()<startPntX&&startPntX<endPnt.x()&&startPntY>startPnt.y()&&startPntY<endPnt.y()&&startPnt.x()<endPntX&&endPntX<endPnt.x()&&endPntY>startPnt.y()&&endPntY<endPnt.y())
            {
                QPointF tl(startPntX,startPntY);
                QPointF br(endPntX,endPntY);
                QRectF e(tl,br);
                seleellipses.append(&ellipses[i]);
            }
        }

        //框选注记
        for(int i=0;i<texts.size();i++)
        {
            double startPntX=textsLoc[i].x();
            double startPntY=textsLoc[i].y();
            if(startPnt.x()<startPntX&&startPntX<endPnt.x()&&startPntY>startPnt.y()&&startPntY<endPnt.y())
            {
                seletextsLoc.append(&textsLoc[i]);
                seletexts.append(&texts[i]);
            }
        }
        update();
    }
}

//重写重设大小事件
void MainWindow::resizeEvent(QResizeEvent *e)
{
    QImage i(this->width(),this->height(),QImage::Format_RGB888);
    i.fill(Qt::white);
}

//创建右键菜单
void MainWindow::createMenu()
{
//    QAction *act=new QAction("移动要素");
//    menu->addAction(act);
//    connect(act, SIGNAL(triggered()), this, SLOT(getmoveflag()));
    menu->addAction(pixm,"移动要素",this,SLOT(on_actionMove_triggered()));
    menu->addAction(pixm,"删除要素",this,SLOT(on_actiondelete_triggered()));
    menu->addAction(pixm,"刷新图幅",this,SLOT(on_actionrefresh_triggered()));
    menu->addAction(pixm,"清空图幅",this,SLOT(on_actiondelete_triggered()));
    menu->addAction(pixm,"改变样式",this,SLOT(on_actionsetColor_triggered()));
    menu->addAction(pixm,"连接到数据库",this,SLOT(on_actionconnectToSql_triggered()));
    menu->addAction(pixm,"验证数据库连接",this,SLOT(on_actionopendb_triggered()));

}

//true共线，false不共线
bool MainWindow::lineJudge(QVector<QPoint> a)
{
    QPoint dot1, dot2, dot3;
    dot1 = a[0];
    dot2 = a[1];
    dot3 = a[2];
    //处理分母为零的情况
    if ((a[2].x() - a[0].x()) == 0) {
        if (a[0].x() == a[1].x()) {
            return true;
        }
        else
        {
            return false;
        }
    }
    else {
        double Dy = a[2].y() - a[0].y();
        double Dx = a[2].x() - a[0].x();
        double k = (Dy) / (Dx);//y=kx+b----y-kx-b=0;
        double b = a[0].y() - k * a[0].x();
        double d = abs(a[1].y() - k * a[1].x() - b) / sqrt(pow(k, 2) + 1);
        if (d <= 0.5) {
            return true;
        }
        else
        {
            return false;
        }
    }
}

//返回1凸点，返回-1凹点，返回0共线
int MainWindow::dotJudge(QVector<QPoint> a)
{
    int index;
    QPoint dot1, dot2, dot3;
    dot1 = a[0];
    dot2 = a[1];
    dot3 = a[2];
    int vectorAX = dot2.x() -dot1.x();
    int vectorAY= dot2.y() - dot1.y();
    int vectorBX = dot3.x() - dot2.x();
    int vectorBY = dot3.y() - dot2.y();
    int S = vectorAX * vectorBY - vectorAY * vectorBX;
    if (S > 0) {
        index = 1;//逆时针，凸点
    }
    else if (S < 0) {
        index = -1;//顺时针，凹点
    }
    else {
        index = 0;//共线
    }
    return index;
}

//端点圆弧（输入两个点的vector，输出未知size 的vector）
QVector<QPoint> MainWindow::endPoint(QVector<QPoint> a, int r)
{
    QVector<QPoint> result;
    QPoint dot1, dot2;
    dot1 = a[0];//端点
    dot2 = a[1];//端点连接点
    int Xa = dot1.x();
    int Xb = dot2.x();
    int Ya = dot1.y();
    int Yb = dot2.y();
    double Dx = Xb - Xa;
    double Dy = Yb - Ya;
    //角度计算
    double alpha;
    if (Dx > 0) {
        alpha = atan(Dy / Dx);//返回弧度
    }
    else if (Dx < 0) {
        alpha = atan(Dy / Dx) + 3.1419526;
    }
    else if (Dx = 0) {
        if (Yb > Ya) {
            alpha = 3.1415926 / 2;
        }
        else {
            alpha = 3.1415926 * 3 / 2;
        }
    }
    else
    {
        qDebug() << "erro" << endl;
    }
    //第i点坐标
    double e = 3.1415926 / 6;// 步长pai/36
    int x, y;
    int im = 6 + 1;
    QPoint temp;
    for (int i = 0; i < im; i++) {
        x = Xa + r * cos(alpha + i * e+3.1415926/2);
        y = Ya + r * sin(alpha + i* e + 3.1415926 / 2);
        temp.setX(x);
        temp.setY(y);
        result.push_back(temp);
    }
    return result;
}

//凸点（输入三个点的vector，返回未知size的vector
QVector<QPoint> MainWindow::convexPoint(QVector<QPoint> a, int r)
{
    QVector<QPoint> result;
    QPoint dot1, dot2, dot3;
    dot1 = a[0];//A
    dot2 = a[1];//B
    dot3 = a[2];//C
    int Xa = dot1.x();
    int Ya = dot1.y();
    int Xb= dot2.x();
    int Yb = dot2.y();
    int Xc = dot3.x();
    int Yc = dot3.y();
    double Dy = Ya - Yb;
    double Dx = Xa - Xb;
    double alpha;
    if (Dx > 0) {
        alpha = atan(Dy / Dx);//返回弧度
    }
    else if (Dx < 0) {
        double a10 = Dy / Dx;
        alpha = atan(Dy / Dx) + 3.1419526;
    }
    else if (Dx == 0) {
        if (Yb > Ya) {
            alpha = 3.1415926 / 2;
        }
        else {
            alpha = 3.1415926 * 3 / 2;
        }
    }
    else
    {
        qDebug() << "erro" << endl;
    }
    double sigma;//角CBA
    int VectorBAx = Xa- Xb;
    int VectorBCx = Xc - Xb;
    int VectorBCy = Yc - Yb;
    int VectorBAy = Ya - Yb;
    double Modulus =sqrt( pow(VectorBAx, 2) + pow(VectorBAy, 2))* sqrt(pow(VectorBCx, 2) + pow(VectorBCy, 2));
    sigma =3.1415926-acos((VectorBAx*VectorBCx + VectorBAy * VectorBCy)/Modulus);
    double e = 3.1415926 / 6;// 步长pai/36
    int k;//求点个数

    double judge = fmod(sigma, e);
    if (judge == 0) {
        k = sigma / e;
    }
    else
    {
        k = sigma / e + 1;
    }
    k++;
    QPoint temp;
    int x, y;
    for (int i = 0; i < k; i++) {
        x = Xb + r * cos(alpha + i * e + 3.1415926 / 2);
        y = Yb + r * sin(alpha + i * e + 3.1415926 / 2);
        temp.setX(x);
        temp.setY(y);
        result.push_back(temp);
    }
    return result;
}

//凹点（通过vector输入三个点，返回包含一个点的vector）
QVector<QPoint> MainWindow::concavePoint(QVector<QPoint> a, int r)
{
    QVector<QPoint> result;
    QPoint dot1, dot2, dot3;
    dot1 = a[0];//A
    dot2 = a[1];//B
    dot3 = a[2];//C
    int Xa = dot1.x();
    int Ya = dot1.y();
    int Xb = dot2.x();
    int Yb = dot2.y();
    int Xc = dot3.x();
    int Yc = dot3.y();
    double alphaMax,alphaMin;
    double DyCB = Yc - Yb;
    double DxCB = Xc - Xb;
    double alphaCB;
    if (DxCB > 0) {
        alphaCB = atan(DyCB / DxCB);//返回弧度
    }
    else if (DxCB < 0) {
        alphaCB = atan(DyCB / DxCB) + 3.1419526;
    }
    else if (DxCB = 0) {
        if (Yc > Yb) {
            alphaCB = 3.1415926 / 2;
        }
        else {
            alphaCB = 3.1415926 * 3 / 2;
        }
    }
    else
    {
        qDebug() << "error" << endl;
    }
    if (alphaCB < 0) {
        alphaCB = alphaCB + 2 * 3.1415926;
    }
    double DyAB = Ya - Yb;
    double DxAB = Xa - Xb;
    double alphaAB;
    if (DxAB > 0) {
        alphaAB = atan(DyAB / DxAB);//返回弧度
    }
    else if (DxAB < 0) {
        alphaAB = atan(DyAB / DxAB) + 3.1419526;
    }
    else if (DxAB == 0) {
        if (Ya > Yb) {
            alphaAB = 3.1415926 / 2;
        }
        else {
            alphaAB = 3.1415926 * 3 / 2;
        }
    }
    else
    {
        qDebug() << "erro" << endl;
    }
    if (alphaAB < 0) {
        alphaAB = alphaAB + 2 * 3.1415926;
    }
    if (alphaAB >= alphaCB) {
        alphaMax = alphaAB;
        alphaMin = alphaCB;
    }
    else
    {
        alphaMax = alphaCB;
        alphaMin = alphaAB;
    }
    double Dalpha=alphaMax-alphaMin;

    double sigma;//角CBA
    int VectorBAx = Xa - Xb;
    int VectorBCx = Xc - Xb;
    int VectorBCy = Yc - Yb;
    int VectorBAy = Ya- Yb;
    double Modulus = sqrt(pow(VectorBAx, 2) + pow(VectorBAy, 2)) * sqrt(pow(VectorBCx, 2) + pow(VectorBCy, 2));
    sigma =acos((VectorBAx*VectorBCx + VectorBAy * VectorBCy) / Modulus);
    double theta;
    if (Dalpha < 3.1415926) {
        theta = alphaMax - sigma / 2;
    }
    else
    {
        theta = alphaMin - sigma / 2;
    }
    //double theta =alpha-sigma/2;
    double r0 = r / sin(sigma / 2);
    QPoint Presult;
    int x, y;
    x = Xb + r0 * cos(theta);
    y = Yb + r0 * sin(theta);
    Presult.setX(x);
    Presult.setY(y);
    result.push_back(Presult);
    return result;
}

//三点共线情况下（输入三个点的vector，输出一个点）
QPoint MainWindow::line(QVector<QPoint> a, int r)
{
    QPoint dot1, dot2;
    dot1 = a[0];//端点
    dot2 = a[1];//端点连接点
    int Xa = dot1.x();
    int Xb = dot2.x();
    int Ya = dot1.y();
    int Yb = dot2.y();
    double Dx = Xb - Xa;
    double Dy = Yb - Ya;
    //角度计算
    double alpha;
    if (Dx > 0) {
        alpha = atan(Dy / Dx);//返回弧度
    }
    else if (Dx < 0) {
        alpha = atan(Dy / Dx) + 3.1419526;
    }
    else if (Dx = 0) {
        if (Yb > Ya) {
            alpha = 3.1415926 / 2;
        }
        else {
            alpha = 3.1415926 * 3 / 2;
        }
    }
    else
    {
        qDebug() << "error" << endl;
    }
    QPoint result;
    int x = Xb + r * sin(alpha);
    int y = Yb + r * cos(alpha);
    result.setX(x);
    result.setY(y);
    return result;
}

//聚类求解
QVector<dbscanpoint> MainWindow::getCluster(QVector<dbscanpoint> Inpts, int threshold, int minptnums, int &clusternum)
{
    int ptNums = Inpts.size();

    //寻找核心点存入mvCorepts
    for (int i = 0; i < ptNums; i++) {

        int index = 1;

        for (int j = 0; j < ptNums; j++)
        {
        double distance = calDistance(Inpts[i], Inpts[j]);
            if(distance==0)
                continue;
            if (distance < threshold) {
                index++;
            }
        }
        if (index >= minptnums)
        {
            Inpts[i].pointtype = 1;//标记corepoint
            mvCorepts.push_back(Inpts[i]);//存入corepoint
/*			mvInsertpts.erase(mvInsertpts.begin() + i);//删除核心点*/
        }
    }

    QVector<dbscanpoint>remaincorepts;

    remaincorepts = mvCorepts;


    //计算每个核心点的阈值范围内的其他核心点
    int coreptNum = mvCorepts.size();

    for (int i = 0; i < coreptNum; i++) {
        for (int j = i + 1; j < coreptNum; j++) {
            double coreptDistance=calDistance(mvCorepts[i], mvCorepts[j]);
            if (coreptDistance < threshold) {
                mvCorepts[i].adjptNum.push_back(j);
                mvCorepts[j].adjptNum.push_back(i);
            }
        }
    }


    //corepoint聚簇
    vector<dbscanpoint>adjapts;
    int nclass = 0;
    for (int i = 0; i < coreptNum; i++) {

        if(mvCorepts[i].visited==1)
            continue;

        mvCorepts[i].ncluster = nclass;
        mvCorepts[i].visited = 1;

        nclass++;

        adjapts.push_back(mvCorepts[i]);

        while (adjapts.size()>0)
        {
            dbscanpoint curpt = adjapts[adjapts.size()-1];
            int adjptsNum = curpt.adjptNum.size();
            adjapts.pop_back();

            for (int j = 0; j < adjptsNum; j++)
            {
                if(mvCorepts[curpt.adjptNum[j]].visited==1)
                    continue;
                mvCorepts[curpt.adjptNum[j]].ncluster = mvCorepts[i].ncluster;
                mvCorepts[curpt.adjptNum[j]].visited=1;
                adjapts.push_back(mvCorepts[curpt.adjptNum[j]]);

            }
        }
    }

    //boardpoint处理

    for (int i = 0; i < ptNums; i++)
    {
        if (Inpts[i].pointtype == 1)
        {
            Inpts[i].visited = 1;
            continue;
        }
        if(Inpts[i].visited==1)
            continue;

        for (int j = 0; j < coreptNum; j++)
        {

        sign:
            if (calDistance(Inpts[i], mvCorepts[j]) < threshold)
            {
                Inpts[i].ncluster = mvCorepts[j].ncluster;
                Inpts[i].visited = 1;
                Inpts[i].pointtype = 2;
                break;
            }
        }
    }


    //噪声点处理

    for (int i = 0; i < ptNums; i++) {


        if (Inpts[i].visited == 1)
            continue;

        Inpts[i].ncluster = nclass + 1;
    }



    clusternum = nclass + 2;

    for (int i = 0; i < ptNums; i++) {



        if (Inpts[i].pointtype == 1) {


            Inpts[i].ncluster = 0;
        }
    }

    for (int i = 0; i < coreptNum; i++) {

        Inpts.push_back(mvCorepts[i]);

    }

    return Inpts;
}

//两点距离
double MainWindow::calDistance(dbscanpoint p1, dbscanpoint p2)
{
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

//b样条曲线
void MainWindow::generateCurve()
{
    curvePoints.clear();
        for (double u = currentK; u < ctrlPoints.size(); u += 0.01)
        {
            QPointF tmp(0, 0);
            for (int i = 0; i < ctrlPoints.size(); i++)
            {
                QPointF t = ctrlPoints[i];

                t *= N(currentK, i, u);

                tmp += t;
            }
            curvePoints.push_back(tmp);
        }
}

double MainWindow::N(int k, int i, double u)
{
    switch (k)
    {
    case 1:
        return N1(i, u);
    case 2:
        return N2(i, u);
    case 3:
        return N3(i, u);
    }
}

double MainWindow::N1(int i, double u)
{
    double t = u - i;

    if (0 <= t && t < 1)
        return t;
    if (1 <= t && t < 2)
        return 2 - t;
    return 0;
}

double MainWindow::N2(int i, double u)
{
    double t = u - i;

    if (0 <= t && t < 1)
        return 0.5*t*t;
    if (1 <= t && t < 2)
        return 3 * t - t*t - 1.5;
    if (2 <= t && t < 3)
        return 0.5*pow(3 - t, 2);
    return 0;
}

double MainWindow::N3(int i, double u)
{
    double t = u - i;
    double a = 1.0 / 6.0;

    if (0 <= t && t < 1)
        return a*t*t*t;
    if (1 <= t && t < 2)
        return a*(-3 * pow(t - 1, 3) + 3 * pow(t - 1, 2) + 3 * (t - 1) + 1);
    if (2 <= t && t < 3)
        return a*(3 * pow(t - 2, 3) - 6 * pow(t - 2, 2) + 4);
    if (3 <= t && t < 4)
        return a*pow(4 - t, 3);
    return 0;
}

int MainWindow::getCurrentNode(QPointF t)
{
    for (int i = 0; i < ctrlPoints.size(); i++)
    {
        double dx = ctrlPoints[i].x() - t.x();
        double dy = ctrlPoints[i].y() - t.y();

        double length = sqrt(dx*dx + dy*dy);

        if (length < 10)
        {
            return i;
        }
    }
    return -1;
}

//清屏
void MainWindow::on_actiondelete_triggered()
{
    handLines.clear();
    pnts.clear();
    lines.clear();
    rects.clear();
    ellipses.clear();
    texts.clear();
    textsLoc.clear();
    polygons.clear();
    polylines.clear();
    selepnts.clear();
    selelines.clear();
    selerects.clear();
    seleellipses.clear();
    seletexts.clear();
    seletextsLoc.clear();
    selepolygons.clear();
    selepolylines.clear();
    data.clear();
    bufferResult.clear();
    Inpts.clear();
    clusterpnts.clear();
    ctrlPoints.clear();
    curvePoints.clear();
    update();
    ui->actionbSplineCurve->setText("b样条曲线绘制");
    ui->actionMove->setText("移动要素");
    ui->actionpointEdit->setText("点绘制");
    ui->actionlineEdit->setText("线绘制");
    ui->actionellipseEdit->setText("圆绘制");
    ui->actionhandSketch->setText("手绘板");
    ui->actionrecEdit->setText("矩形绘制");
    ui->actiontextEdit->setText("注记绘制");
    ui->actionbrLine->setText("多段线绘制");
    ui->actionregionEdit->setText("区绘制");
    ui->textEdit->setText("清屏操作完成");
    ui->menu_3->setTitle(" ");
}

//绘制点要素
void MainWindow::on_actionpointEdit_triggered()
{
    if(trigger==1)
    {
        trigger=-100;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->actionrecEdit->setText("矩形绘制");
        ui->menu_3->setTitle(" ");
    }
    else
    {
        trigger=1;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionpointEdit->setText("点绘制*");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->actionrecEdit->setText("矩形绘制");
        ui->menu_3->setTitle("*");
    }
}

//绘制线要素
void MainWindow::on_actionlineEdit_triggered()
{
    if(trigger==2)
    {
        trigger=-100;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->actionrecEdit->setText("矩形绘制");
        ui->menu_3->setTitle(" ");
    }
    else
    {
        trigger=2;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionlineEdit->setText("线绘制*");
        ui->actionpointEdit->setText("点绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->actionrecEdit->setText("矩形绘制");
        ui->menu_3->setTitle("*");
    }
}

//绘制圆
void MainWindow::on_actionellipseEdit_triggered()
{
    if(trigger==3)
    {
        trigger=-100;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->actionrecEdit->setText("矩形绘制");
        ui->menu_3->setTitle(" ");
    }
    else
    {
        trigger=3;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionellipseEdit->setText("圆绘制*");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->actionrecEdit->setText("矩形绘制");
        ui->menu_3->setTitle("*");
    }
}

//手绘板
void MainWindow::on_actionhandSketch_triggered()
{
    if(trigger==4)
    {
        trigger=-100;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionrecEdit->setText("矩形绘制");
        ui->menu_3->setTitle(" ");
    }
    else
    {
        trigger=4;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionhandSketch->setText("手绘板*");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionrecEdit->setText("矩形绘制");
        ui->menu_3->setTitle("*");
    }
}

//绘制矩形要素
void MainWindow::on_actionrecEdit_triggered()
{
    if(trigger==5)
    {
        trigger=-100;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionrecEdit->setText("矩形绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->menu_3->setTitle(" ");
    }
    else
    {
        trigger=5;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionrecEdit->setText("矩形绘制*");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->menu_3->setTitle("*");
    }
}

//注记绘制
void MainWindow::on_actiontextEdit_triggered()
{
    if(trigger==7)
    {
        trigger=-100;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionrecEdit->setText("矩形绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->menu_3->setTitle(" ");
    }
    else
    {
        textEditDialog *t=new textEditDialog;
        t->p=this;
        t->show();
        trigger=7;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制*");
        ui->actionrecEdit->setText("矩形绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->menu_3->setTitle("*");
    }
}

//b样条曲线
void MainWindow::on_actionbSplineCurve_triggered()
{
    if(trigger==10)
    {
        trigger=-100;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->actionrecEdit->setText("矩形绘制");
        ui->menu_3->setTitle(" ");
    }
    else
    {
        trigger=10;
        ui->actionbSplineCurve->setText("b样条曲线绘制*");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->actionrecEdit->setText("矩形绘制");
        ui->menu_3->setTitle("*");
    }
}

//多段线绘制
void MainWindow::on_actionbrLine_triggered()
{
    if(trigger==8)
    {
        trigger=-100;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionrecEdit->setText("矩形绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->menu_3->setTitle(" ");
    }
    else
    {
        trigger=8;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制*");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionrecEdit->setText("矩形绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->menu_3->setTitle("*");
    }
}

//区绘制
void MainWindow::on_actionregionEdit_triggered()
{
    if(trigger==9)
    {
        trigger=-100;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区域绘制*");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionrecEdit->setText("矩形绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->menu_3->setTitle(" ");
    }
    else
    {
        trigger=9;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区域绘制*");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionrecEdit->setText("矩形绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->menu_3->setTitle("*");
    }
}

//清除框选触发动作
void MainWindow::on_actionnoSelect_triggered()
{
    selepnts.clear();
    selelines.clear();
    selerects.clear();
    seleellipses.clear();
    seletexts.clear();
    seletextsLoc.clear();
    selepolygons.clear();
    selepolylines.clear();
    data.clear();
    bufferResult.clear();
    Inpts.clear();
    clusterpnts.clear();
    update();
}

//快速打开文件
void MainWindow::on_actiondataPort_triggered()
{
    fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), filePath,  tr("csv文件(*.csv);;txt文件(*.txt)"));
    on_actionopenDataBase_triggered();
}

//打开文件
void MainWindow::on_actionOpenFile_triggered()
{
    fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), filePath,  tr("csv文件(*.csv);;txt文件(*.txt)"));
    on_actionopenDataBase_triggered();
}

//从数据库打开
void MainWindow::on_actionopenDataBase_triggered()
{
    trigger=6;
    double startPntX;
    double startPntY;
    double endPntX;
    double endPntY;

    QString str = QString("select *from basicGraphs where gType = '%1'").arg("line");
    QSqlQuery query;
    query.exec(str);

    QLineF line;
    while (query.next())
    {
        startPntX=query.value(0).toInt();
        startPntY=query.value(1).toInt();
        endPntX=query.value(2).toInt();
        endPntY=query.value(3).toInt();
        line.setLine(startPntX,startPntY,endPntX,endPntY);
        lines.append(line);
    }

    str = QString("select *from basicGraphs where gType = '%1'").arg("rect");
    query.exec(str);

    QRectF rect;
    while (query.next())
    {
        startPntX=query.value(0).toDouble();
        startPntY=query.value(1).toDouble();
        endPntX=query.value(2).toDouble();
        endPntY=query.value(3).toDouble();
        QPointF st(startPntX,startPntY);
        QPointF en(endPntX,endPntY);
        rect.setTopLeft(st);
        rect.setBottomRight(en);
        rects.append(rect);
    }

    str = QString("select *from basicGraphs where gType = '%1'").arg("ellipse");
    query.exec(str);

    QRectF ellipse;
    while (query.next())
    {
        startPntX=query.value(0).toDouble();
        startPntY=query.value(1).toDouble();
        endPntX=query.value(2).toDouble();
        endPntY=query.value(3).toDouble();
        QPointF st(startPntX,startPntY);
        QPointF en(endPntX,endPntY);
        ellipse.setTopLeft(st);
        ellipse.setBottomRight(en);
        ellipses.append(ellipse);
    }

    str = QString("select *from basicGraphs where gType = '%1'").arg("pnt");
    query.exec(str);

    QPointF pnt;
    while (query.next())
    {
        startPntX=query.value(0).toDouble();
        startPntY=query.value(1).toDouble();
        pnt.setX(startPntX);
        pnt.setY(startPntY);
        pnts.append(pnt);
    }

    str = QString("select *from basicGraphs where gType = '%1'").arg("text");
    query.exec(str);

    QPointF textloc;
    QString text;
    while (query.next())
    {
        startPntX=query.value(0).toDouble();
        startPntY=query.value(1).toDouble();
        text=query.value(5).toString();
        textloc.setX(startPntX);
        textloc.setY(startPntY);
        textsLoc.append(textloc);
        texts.append(text);
    }

    str = QString("select *from basicGraphs where gType = '%1'").arg("polyline");
    query.exec(str);

    int polylineorder;
    while (query.next())
    {
        startPntX=query.value(0).toDouble();
        startPntY=query.value(1).toDouble();
        polylineorder=query.value(6).toInt();
        pnt.setX(startPntX);
        pnt.setY(startPntY);
        polylines[polylineorder].append(pnt);
    }

    str = QString("select *from basicGraphs where gType = '%1'").arg("polygon");
    query.exec(str);

    int polygonorder;
    while (query.next())
    {
        startPntX=query.value(0).toDouble();
        startPntY=query.value(1).toDouble();
        polygonorder=query.value(6).toInt();
        pnt.setX(startPntX);
        pnt.setY(startPntY);
        polylines[polygonorder].append(pnt);
    }

    update();
    ui->textEdit->setText("打开成功");
}

//存储到数据库
void MainWindow::on_actionSaveToSql_triggered()
{
    /*//存储手绘板线
    for(unsigned int i=0;i<handLines.size();i++)
    {
        myLine* pLine = handLines[i];
        double startPntX=pLine->startPnt.x();
        double startPntY=pLine->startPnt.y();
        double endPntX=pLine->endPnt.x();
        double endPntY=pLine->endPnt.y();
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("handline");
        QSqlQuery query;
        query.exec(str);
    }*/

    //存储点
    for(int i=0;i<pnts.size();i++)
    {
        double startPntX=pnts[i].x();
        double startPntY=pnts[i].y();
        double endPntX=0.0;
        double endPntY=0.0;
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("pnt");
        QSqlQuery query;
        query.exec(str);
    }

    //存储线
    for(int i=0;i<lines.size();i++)
    {
        QLineF line=lines[i];
        double startPntX=line.p1().x();
        double startPntY=line.p1().y();
        double endPntX=line.p2().x();
        double endPntY=line.p2().y();
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("line");
        QSqlQuery query;
        query.exec(str);
    }

    //存储矩形
    for(int i=0;i<rects.size();i++)
    {
        QRectF rect=rects[i];
        double startPntX=rect.topLeft().x();
        double startPntY=rect.topLeft().y();
        double endPntX=rect.bottomRight().x();
        double endPntY=rect.bottomRight().y();
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("rect");
        QSqlQuery query;
        query.exec(str);
    }

    //存储圆
    for(int i=0;i<ellipses.size();i++)
    {
        QRectF ellipse=ellipses[i];
        double startPntX=ellipse.topLeft().x();
        double startPntY=ellipse.topLeft().y();
        double endPntX=ellipse.bottomRight().x();
        double endPntY=ellipse.bottomRight().y();
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("ellipse");
        QSqlQuery query;
        query.exec(str);
    }

    //存储注记
    for(int i=0;i<texts.size();i++)
    {
        QString text=texts[i];
        QPointF textLoc=textsLoc[i];
        double startPntX=textLoc.x();
        double startPntY=textLoc.y();
        double endPntX=0.0;
        double endPntY=0.0;
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType,text) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("text").arg(text);
        QSqlQuery query;
        query.exec(str);
    }

    //存储多段线
    for(int i=0;i<polylines.size();i++)
    {
        for(int j=0;j<polylines[i].size();j++)
        {
            double startPntX=polylines[i][j].x();
            double startPntY=polylines[i][j].y();
            double endPntX=0.0;
            double endPntY=0.0;
            QString str=QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType,text,order) values('%1', '%2', '%3','%4','%5','%6')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("polyline").arg("").arg(i);
            QSqlQuery query;
            query.exec(str);
        }
    }

    //存储多边形
    for(int i=0;i<polygons.size();i++)
    {
        for(int j=0;j<polygons[i].size();j++)
        {
            double startPntX=polygons[i][j].x();
            double startPntY=polygons[i][j].y();
            double endPntX=0.0;
            double endPntY=0.0;
            QString str=QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType,text,order) values('%1', '%2', '%3','%4','%5','%6')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("polygon").arg("").arg(i);
            QSqlQuery query;
            query.exec(str);
        }
    }

    handLines.clear();
    pnts.clear();
    lines.clear();
    rects.clear();
    ellipses.clear();
    texts.clear();
    textsLoc.clear();
    polygons.clear();
    polylines.clear();
    selepnts.clear();
    selelines.clear();
    selerects.clear();
    seleellipses.clear();
    seletexts.clear();
    seletextsLoc.clear();
    selepolygons.clear();
    selepolylines.clear();
    data.clear();
    bufferResult.clear();
    Inpts.clear();
    clusterpnts.clear();
    update();
    ui->actionbSplineCurve->setText("b样条曲线绘制");
    ui->actionMove->setText("移动要素");
    ui->actionpointEdit->setText("点绘制");
    ui->actionlineEdit->setText("线绘制");
    ui->actionellipseEdit->setText("圆绘制");
    ui->actionhandSketch->setText("手绘板");
    ui->actionrecEdit->setText("矩形绘制");
    ui->actiontextEdit->setText("注记绘制");
    ui->actionbrLine->setText("多段线绘制");
    ui->actionregionEdit->setText("区绘制");
    ui->menu_3->setTitle(" ");
    ui->textEdit->setText("保存成功");
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("select * from basicGraphs");
    ui->tableView->setModel(model);
    //隔行变色
    ui->tableView->setAlternatingRowColors(true);
}

//快速存储到数据库
void MainWindow::on_actiondataBase_triggered()
{

    /*//存储手绘板线
    for(unsigned int i=0;i<handLines.size();i++)
    {
        myLine* pLine = handLines[i];
        double startPntX=pLine->startPnt.x();
        double startPntY=pLine->startPnt.y();
        double endPntX=pLine->endPnt.x();
        double endPntY=pLine->endPnt.y();
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("handline");
        QSqlQuery query;
        query.exec(str);
    }*/

    //存储点
    for(int i=0;i<pnts.size();i++)
    {
        double startPntX=pnts[i].x();
        double startPntY=pnts[i].y();
        double endPntX=0.0;
        double endPntY=0.0;
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("pnt");
        QSqlQuery query;
        query.exec(str);
    }

    //存储线
    for(int i=0;i<lines.size();i++)
    {
        QLineF line=lines[i];
        double startPntX=line.p1().x();
        double startPntY=line.p1().y();
        double endPntX=line.p2().x();
        double endPntY=line.p2().y();
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("line");
        QSqlQuery query;
        query.exec(str);
    }

    //存储矩形
    for(int i=0;i<rects.size();i++)
    {
        QRectF rect=rects[i];
        double startPntX=rect.topLeft().x();
        double startPntY=rect.topLeft().y();
        double endPntX=rect.bottomRight().x();
        double endPntY=rect.bottomRight().y();
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("rect");
        QSqlQuery query;
        query.exec(str);
    }

    //存储圆
    for(int i=0;i<ellipses.size();i++)
    {
        QRectF ellipse=ellipses[i];
        double startPntX=ellipse.topLeft().x();
        double startPntY=ellipse.topLeft().y();
        double endPntX=ellipse.bottomRight().x();
        double endPntY=ellipse.bottomRight().y();
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType) values('%1', '%2', '%3','%4','%5')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("ellipse");
        QSqlQuery query;
        query.exec(str);
    }

    //存储注记
    for(int i=0;i<texts.size();i++)
    {
        QString text=texts[i];
        QPointF textLoc=textsLoc[i];
        double startPntX=textLoc.x();
        double startPntY=textLoc.y();
        double endPntX=0.0;
        double endPntY=0.0;
        QString str = QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType,text) values('%1', '%2', '%3','%4','%5','%6')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("text").arg(text);
        QSqlQuery query;
        query.exec(str);
    }

    //存储多段线
    for(int i=0;i<polylines.size();i++)
    {
        for(int j=0;j<polylines[i].size();j++)
        {
            double startPntX=polylines[i][j].x();
            double startPntY=polylines[i][j].y();
            double endPntX=0.0;
            double endPntY=0.0;
            QString str=QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType,text,order) values('%1', '%2', '%3','%4','%5','%6')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("polyline").arg("").arg(i);
            QSqlQuery query;
            query.exec(str);
        }
    }

    //存储多边形
    for(int i=0;i<polygons.size();i++)
    {
        for(int j=0;j<polygons[i].size();j++)
        {
            double startPntX=polygons[i][j].x();
            double startPntY=polygons[i][j].y();
            double endPntX=0.0;
            double endPntY=0.0;
            QString str=QString("insert into basicGraphs(startPntX,startPntY,endPntX,endPntY,gType,text,order) values('%1', '%2', '%3','%4','%5','%6')").arg(startPntX).arg(startPntY).arg(endPntX).arg(endPntY).arg("polygon").arg("").arg(i);
            QSqlQuery query;
            query.exec(str);
        }
    }

    //删除缓存并存储到数据库
    handLines.clear();
    pnts.clear();
    lines.clear();
    rects.clear();
    ellipses.clear();
    texts.clear();
    textsLoc.clear();
    polygons.clear();
    polylines.clear();
    selepnts.clear();
    selelines.clear();
    selerects.clear();
    seleellipses.clear();
    seletexts.clear();
    seletextsLoc.clear();
    selepolygons.clear();
    selepolylines.clear();
    data.clear();
    bufferResult.clear();
    Inpts.clear();
    clusterpnts.clear();
    update();
    ui->actionbSplineCurve->setText("b样条曲线绘制");
    ui->actionMove->setText("移动要素");
    ui->actionpointEdit->setText("点绘制");
    ui->actionlineEdit->setText("线绘制");
    ui->actionellipseEdit->setText("圆绘制");
    ui->actionhandSketch->setText("手绘板");
    ui->actionrecEdit->setText("矩形绘制");
    ui->actiontextEdit->setText("注记绘制");
    ui->actionbrLine->setText("多段线绘制");
    ui->actionregionEdit->setText("区绘制");
    ui->menu_3->setTitle(" ");
    ui->textEdit->setText("保存成功");
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("select * from basicGraphs");
    ui->tableView->setModel(model);
    //隔行变色
    ui->tableView->setAlternatingRowColors(true);
}

//连接数据库
void MainWindow::on_actionconnectToSql_triggered()
{
    dataBaseDialog *d=new dataBaseDialog;
    d->p=this;
    d->show();
}

//快速连接
void MainWindow::on_actionconnect_triggered()
{
    dataBaseDialog *d=new dataBaseDialog;
    d->p=this;
    d->show();
}

//样式编辑器
void MainWindow::on_actionsetColor_triggered()
{
    selepnts.clear();
    selelines.clear();
    selerects.clear();
    seleellipses.clear();
    seletexts.clear();
    seletextsLoc.clear();
    selepolygons.clear();
    selepolylines.clear();
    update();
    penFormDialog *pen=new penFormDialog;
    pen->p=this;
    pen->show();
}

//移动要素
void MainWindow::on_actionMove_triggered()
{
    if(trigger==50)
    {
        trigger=-100;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionrecEdit->setText("矩形绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->menu_3->setTitle(" ");
        movex=0.0;
        movey=0.0;
    }
    else
    {
        trigger=50;
        ui->actionbSplineCurve->setText("b样条曲线绘制");
        ui->actionMove->setText("移动要素*");
        ui->actionregionEdit->setText("区绘制");
        ui->actionbrLine->setText("多段线绘制");
        ui->actiontextEdit->setText("注记绘制");
        ui->actionrecEdit->setText("矩形绘制");
        ui->actionpointEdit->setText("点绘制");
        ui->actionlineEdit->setText("线绘制");
        ui->actionellipseEdit->setText("圆绘制");
        ui->actionhandSketch->setText("手绘板");
        ui->menu_3->setTitle("*");
    }
}

//右键菜单连接数据库
void MainWindow::on_actionopendb_triggered()
{
    openDatabase();
}

//缓冲区
void MainWindow::on_actionbuffer_triggered()
{
    bufferDialog *b=new bufferDialog;
    b->p=this;
    b->show();
    update();
    trigger=1;
}

//聚类分析
void MainWindow::on_actiondbScanCluster_triggered()
{
    clusterDialog *c=new clusterDialog;
    c->p=this;
    c->show();
    trigger=1;
}

//新建图层
void MainWindow::on_actionnewLayer_triggered()
{
    currentPixCount++;
    QStringList contentList;
    contentList << QString("图层%1").arg(currentPixCount);
    QTreeWidgetItem *i=new QTreeWidgetItem(contentList);
    ui->treeWidget->addTopLevelItem(i);
}

//添加图层组
void MainWindow::on_actionnewLayerGroup_triggered()
{
    currentPixCount++;
    QStringList contentList;
    contentList << QString("图层%1").arg(currentPixCount);
    QTreeWidgetItem *i=new QTreeWidgetItem(contentList);
}

//图幅刷新
void MainWindow::on_actionrefresh_triggered()
{
    this->update();
    ui->textEdit->setText("已刷新");
}

//删除分析结果
void MainWindow::on_actiondeleteAnalysis_triggered()
{
    data.clear();
    bufferResult.clear();
    Inpts.clear();
    clusterpnts.clear();
    polylines.clear();
    ui->textEdit->setText("删除分析结果成功");
    update();
}

//保存为文件
void MainWindow::on_actionsaveToFile_triggered()
{
    fileName=QFileDialog::getSaveFileName(this,tr("保存"),filePath,tr("csv文件(*.csv);;txt文件(*.txt)"));
    QFile outFile(fileName);
    QStringList writelines;
    writelines.append("startPntx,startPntY,endPntX,endPntY,gType,text,order\n");

    //保存点
    for(int i = 0;i<pnts.size();i++)
    {
        QPointF p1=pnts[i];
        QString str=QString::number(p1.x())+","+QString::number(p1.y())+" , ,pnt\n";
        writelines.append(str);
    }

    //保存线
    for(int i=0;i<lines.size();i++)
    {
        QLineF line=lines[i];
        QString str=QString::number(line.x1())+","+QString::number(line.y1())+","+QString::number(line.x2())+","+QString::number(line.y2())+",line\n";
        writelines.append(str);
    }

    //保存矩形
    for(int i=0;i<rects.size();i++)
    {
        QRectF rect=rects[i];
        QString str=QString::number(rect.topLeft().x())+","+QString::number(rect.topLeft().y())+","+QString::number(rect.bottomRight().x())+","+QString::number(rect.bottomRight().y())+",rect\n";
        writelines.append(str);
    }

    //保存圆
    for(int i=0;i<ellipses.size();i++)
    {
        QRectF ellipse=ellipses[i];
        QString str=QString::number(ellipse.topLeft().x())+","+QString::number(ellipse.topLeft().y())+","+QString::number(ellipse.bottomRight().x())+","+QString::number(ellipse.bottomRight().y())+",ellipse\n";
        writelines.append(str);
    }

    //保存注记
    for(int i=0;i<texts.size();i++)
    {
        QString text=texts[i];
        QPointF textLoc=textsLoc[i];
        QString str=QString::number(textLoc.x())+","+QString::number(textLoc.y())+",,text,"+text+"\n";
        writelines.append(str);
    }

    //保存折线
    for(int i=0;i<polylines.size();i++)
    {
        for(int j=0;j<polylines[i].size();j++)
        {
            QPointF p1=polylines[i][j];
            QString str=QString::number(p1.x())+","+QString::number(p1.y())+" , ,polyline, ,"+QString::number(i)+"\n";
            writelines.append(str);
        }
    }

    //保存多边形
    for(int i=0;i<polygons.size();i++)
    {
        for(int j=0;j<polygons[i].size();j++)
        {
            QPointF p1=polygons[i][j];
            QString str=QString::number(p1.x())+","+QString::number(p1.y())+" , ,polygon, ,"+QString::number(i)+"\n";
            writelines.append(str);
        }
    }

    //lines << "00,01,02\n" << "10,11,12\n" << "20,21,22";
    if (outFile.open(QIODevice::WriteOnly))
    {
        for (int i = 0; i < lines.size(); i++)
        {
            outFile.write(writelines[i].toStdString().c_str());
        }
        outFile.close();
    }
}

//另存为图像
void MainWindow::on_actionsaveToPic_triggered()
{
    picName=QFileDialog::getSaveFileName(this,tr("另存为图像"),filePath,tr("png文件(*.png);;jpg文件(*.jpg)"));
    pixm.save(picName);
}

//保存到文件
void MainWindow::on_actionsaveFile_triggered()
{
    QFile outFile(fileName);
    QStringList writelines;
    writelines.append("startPntx,startPntY,endPntX,endPntY,gType,text,order\n");


    //保存点
    for(int i = 0;i<pnts.size();i++)
    {
        QPointF p1=pnts[i];
        QString str=QString::number(p1.x())+","+QString::number(p1.y())+" , ,pnt\n";
        writelines.append(str);
    }

    //保存线
    for(int i=0;i<lines.size();i++)
    {
        QLineF line=lines[i];
        QString str=QString::number(line.x1())+","+QString::number(line.y1())+","+QString::number(line.x2())+","+QString::number(line.y2())+",line\n";
        writelines.append(str);
    }

    //保存矩形
    for(int i=0;i<rects.size();i++)
    {
        QRectF rect=rects[i];
        QString str=QString::number(rect.topLeft().x())+","+QString::number(rect.topLeft().y())+","+QString::number(rect.bottomRight().x())+","+QString::number(rect.bottomRight().y())+",rect\n";
        writelines.append(str);
    }

    //保存圆
    for(int i=0;i<ellipses.size();i++)
    {
        QRectF ellipse=ellipses[i];
        QString str=QString::number(ellipse.topLeft().x())+","+QString::number(ellipse.topLeft().y())+","+QString::number(ellipse.bottomRight().x())+","+QString::number(ellipse.bottomRight().y())+",ellipse\n";
        writelines.append(str);
    }

    //保存注记
    for(int i=0;i<texts.size();i++)
    {
        QString text=texts[i];
        QPointF textLoc=textsLoc[i];
        QString str=QString::number(textLoc.x())+","+QString::number(textLoc.y())+",,text,"+text+"\n";
        writelines.append(str);
    }

    //保存折线
    for(int i=0;i<polylines.size();i++)
    {
        for(int j=0;j<polylines[i].size();j++)
        {
            QPointF p1=polylines[i][j];
            QString str=QString::number(p1.x())+","+QString::number(p1.y())+" , ,polyline, ,"+QString::number(i)+"\n";
            writelines.append(str);
        }
    }

    //保存多边形
    for(int i=0;i<polygons.size();i++)
    {
        for(int j=0;j<polygons[i].size();j++)
        {
            QPointF p1=polygons[i][j];
            QString str=QString::number(p1.x())+","+QString::number(p1.y())+" , ,polygon, ,"+QString::number(i)+"\n";
            writelines.append(str);
        }
    }

    //lines << "00,01,02\n" << "10,11,12\n" << "20,21,22";
    if (outFile.open(QIODevice::WriteOnly))
    {
        for (int i = 0; i < lines.size(); i++)
        {
            outFile.write(writelines[i].toStdString().c_str());
        }
        outFile.close();
    }
}

//保存到图像
void MainWindow::on_actionsavePic_triggered()
{
    pixm.save(picName);
}

//设置坐标范围
void MainWindow::on_actionsetCoordinate_triggered()
{
    coordinaterangeDialog *c=new coordinaterangeDialog;
    c->p=this;
    c->show();
    update();
}

//编辑数据库
void MainWindow::on_actionsqlEdit_triggered()
{
    sqleditDialog *s=new sqleditDialog;
    s->p=this;
    s->show();
    ui->textEdit->setText("编辑数据库成功");
}

//绘制工具条
void MainWindow::on_actionDraw_triggered()
{
    drawVisiable=!drawVisiable;
    ui->toolBar_2->setVisible(drawVisiable);
    if(drawVisiable==true)
    {
        ui->actionDraw->setText("√ 绘制工具条");
    }
    else
    {
        ui->actionDraw->setText("  绘制工具条");
    }
}

//编辑工具条
void MainWindow::on_actionEdit_triggered()
{
    editVisiable=!editVisiable;
    ui->toolBar->setVisible(editVisiable);
    if(editVisiable==true)
    {
        ui->actionEdit->setText("√ 编辑工具条");
    }
    else
    {
        ui->actionEdit->setText("  编辑工具条");
    }
}

//分析工具条
void MainWindow::on_actionAnalysis_triggered()
{
    anaVisiable=!anaVisiable;
    ui->toolBar_3->setVisible(anaVisiable);
    if(anaVisiable==true)
    {
        ui->actionAnalysis->setText("√ 分析工具条");
    }
    else
    {
        ui->actionAnalysis->setText("  分析工具条");
    }
}

//数据库工具条
void MainWindow::on_actionDb_triggered()
{
    dbVisiable=!dbVisiable;
    ui->toolBar_4->setVisible(dbVisiable);
    if(dbVisiable==true)
    {
        ui->actionDb->setText("√ 数据库工具条");
    }
    else
    {
        ui->actionDb->setText("  数据库工具条");
    }
}

//属性表视窗
void MainWindow::on_actionTable_triggered()
{
    tableVisiable=!tableVisiable;
    ui->tableView->setVisible(tableVisiable);
    if(tableVisiable==true)
    {
        ui->actionTable->setText("√ 属性表视窗");
    }
    else
    {
        ui->actionTable->setText("  属性表视窗");
    }
}

//距离分析
void MainWindow::on_actionrealDis_triggered()
{
    if(trigger!=8||disFlag!=true)
    {
        trigger=8;
        disFlag=true;
        ui->actionrealDis->setText("距离分析*");
    }
    else
    {
        trigger=-100;
        disFlag=false;
        ui->actionrealDis->setText("距离分析");
    }
}

//ODBC数据源
void MainWindow::on_actiondataOrigin_triggered()
{
    QString odbc="C:\\Windows\\syswow64\\odbcad32.exe";
    QDesktopServices::openUrl(QUrl(odbc));

//    QStringList o;
//    o<<"C:\\Windows\\syswow64\\odbcad32.exe";
//    QProcess::execute("ODBC数据源",o);

//    QProcess *process;
//    process = new QProcess(this);//加载开始
//    process->start("C:\\Windows\\syswow64\\odbcad32.exe");//打开程序
//    process->close();//关闭程序
}

void MainWindow::on_actioninstruction_triggered()
{

}

