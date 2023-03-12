#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPoint>
#include <vector>
#include <QVector>
#include <QList>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QtGui>
#include <QString>
#include <QTextCodec>
#include <QtSql>
#include <QStandardItemModel>
#include <QGraphicsView>
#include <QStandardItemModel>
#include <QPixmap>
#include<QMessageBox>
#include <QFileInfo>
#include <QWheelEvent>
#include <QScrollBar>
#include <QGraphicsPixmapItem>
#include<QMouseEvent>
#define pi 3.1415926535897932384626433832795
#define EARTH_RADIUS 6378.137 //地球半径 KM
using namespace std;

//聚类点
struct dbscanpoint
{
    int x;
    int y;
    int ncluster ;//聚类簇
    int pointtype=3;//1 corepoint 2 boardpoint 3 noisepoint
    QColor clusterColor=Qt::black;
    int visited = 0;
    QVector<int>adjptNum;//每个核心点的给定阈值范围内其他核心点数目
    dbscanpoint() { ; }
    dbscanpoint(int x, int y,int pointtype =3,int visited=0) {
        this->x = x;
        this->y = y;
    }
};

//手绘线
typedef struct myLine
{
    QPointF startPnt;
    QPointF endPnt;
}myLine;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int currentPixCount=1;//当前图层编号
//    QVector <QPixmap> pixm[5]; //图层
    QPixmap *currentPix;//当前图层

    QPixmap pixm; //存储图层

    QString filePath="C:\\Users\\31808\\Documents\\gisSystem\\";             //文件路径
    QString picName="C:\\Users\\31808\\Documents\\gisSystem\\gisMap1.png";   //默认图像文件名
    QString fileName="C:\\Users\\31808\\Documents\\gisSystem\\default1.csv"; //默认表格文件名

    QString dataBaseName="gisSystem"; //数据源
    QString dbuserName="sa";            //用户名
    QString dbpassWord="123";           //密码

    QString drawPattern="";   //绘制画笔样式
    QString linePattern="";   //绘制线样式
    QFont fontPattern;        //绘制字体样式
    QColor color=Qt::black;   //绘制颜色
    int wid=1;                //绘制线宽

    QString seledrawPattern="";   //框选画笔样式
    QString selelinePattern="";   //框选线样式
    QFont selefontPattern;        //框选字体样式
    QColor selecolor=Qt::red;     //框选颜色
    int selewid=1;                //框选线宽

    QMenu *menu;         //右键菜单
    QWidget *w=nullptr;  //多图层

    QString tableName="";  //表名
    QString columnName=""; //列名
    QString tableType="";  //表类型

    //坐标范围
    double east=135.08;
    double south=3.85;
    double west=73.55;
    double north=53.55;
    double wToe=&east-&west;
    double sTon=&north-&south;
    double screenwToe;
    double screensTon;
    double currentLongitude;
    double currentLatitude;

    //图形移动
    double movex=0.0;        //x移动量
    double movey=0.0;        //y移动量
    QPointF movep;       //移动位置

    //绘制
    QPointF startPnt;    //鼠标起点
    QPointF endPnt;      //鼠标终点
    QLineF currentline;  //当前线
    QRectF currentrect;  //当前矩形
    QRectF currentellipse;//当前圆
    QString currenttext; //注记
    QVector <QPointF> currentPolyline; //当前多段线
    QVector <QPointF> currentPolygon;  //当前多边形

    QRectF seleRect;     //框选矩形

    //标记
    bool isPressed;      //鼠标是否按下
    int dragTrigger;     //拖曳标记
    int trigger=-100;    //绘图事件标记
    bool moveFlag=false; //移动标记
    bool disFlag=false;  //距离分析标记

//    1
//    2
//    3
//    4
//    5
//    6
//    7
//    8
//    9
//    10
//    50
//    100
//    150
//    200

    //工具条可视性
    bool drawVisiable=true;
    bool editVisiable=true;
    bool anaVisiable=true;
    bool dbVisiable=true;
    bool tableVisiable=true;

    vector<myLine*> handLines;  //手绘线缓存
    QVector <QPointF> pnts;     //点缓存
    QVector <QLineF> lines;     //线缓存
    QVector <QRectF> ellipses;  //椭圆缓存
    QVector <QRectF> rects;     //矩形缓存
    QVector <QString> texts;    //注记缓存
    QVector <QPointF> textsLoc; //注记位置缓存
    QVector <QVector<QPointF>> polylines;  //多段线缓存
    QVector <QVector<QPointF>> polygons;   //多边形缓存


    QVector <QPointF*> selepnts;     //框选点缓存
    QVector <QLineF*> selelines;     //框选线缓存
    QVector <QRectF*> seleellipses;  //框选椭圆缓存
    QVector <QRectF*> selerects;     //框选矩形缓存
    QVector <QString*> seletexts;    //框选注记缓存
    QVector <QPointF*> seletextsLoc; //框选注记位置缓存
    QVector <QVector<QPointF>> selepolylines;  //框选多段线缓存
    QVector <QVector<QPointF>> selepolygons;    //框选多边形缓存

    QVector <QPointF*> clickpnts;     //点选点缓存
    QVector <QLineF*> clicklines;     //点选线缓存
    QVector <QRectF*> clickellipses;  //点选椭圆缓存
    QVector <QRectF*> clickrects;     //点选矩形缓存
    QVector <QString*> clicktexts;    //点选注记缓存
    QVector <QPointF*> clicktextsLoc; //点选注记位置缓存
    QVector <QVector<QPointF>> clickpolylines;  //点选折线缓存
    QVector <QVector<QPointF>> clickpolygons;   //点选多边形缓存

    void clickDataSet();

    //打开数据库
    void openDatabase();

    //多图层绘制
    void paintLayer(QWidget *w);

    double getLongitude(QPointF p);

    double getLatitude(QPointF p);

    double rad(double d)
    {
        return d * pi /180.0;
    }

    double RealDistance(double lat1,double lng1,double lat2,double lng2);

    void setMatrix(const QMatrix &matrix, bool combine = false);
    //重载
    void paintEvent(QPaintEvent *);    
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);

    //创建右键菜单
    void createMenu();

    //缓冲区
    int r=5;//缓冲区半径
    QString buffertype="";//缓冲区类型
    QVector <QPoint> data;
    QVector <QPoint> bufferResult;
    bool lineJudge(QVector <QPoint> a);
    int dotJudge(QVector <QPoint> a);
    QVector<QPoint> endPoint(QVector<QPoint> a,int r);
    QVector<QPoint> convexPoint(QVector<QPoint> a,int r);
    QVector<QPoint> concavePoint(QVector<QPoint> a,int r);
    QPoint line(QVector<QPoint> a, int r);

    //聚类
    QVector <QColor> c;
    QColor color_1=Qt::green;
    QColor color_2=Qt::yellow;
    int threshold;
    int minptnums;
    int clusternum;
    dbscanpoint mStartpt;//指示每次核心点聚簇的起点
    QVector<dbscanpoint>mvCorepts;
    QVector<dbscanpoint>mvInsertpts;
    QVector<int>mvClusteringIndex;//聚类核心点地址
    QVector<dbscanpoint>Inpts;//聚类输入点集
    QVector <dbscanpoint> clusterpnts;//聚类结果点集
    QVector<dbscanpoint> getCluster(QVector<dbscanpoint>Inpts,int threshold,int minptnums,int &clusternum);
    double calDistance(dbscanpoint p1, dbscanpoint p2);

    //b样条曲线
    bool showCtrlNode=true;
    bool showCurveNode=true;
    int currentK=3;
    QVector<QPointF> ctrlPoints;  // 控制点
    QVector<QPointF> curvePoints; // 曲线上的点

    void generateCurve();         // 生成B样条曲线

    double N(int k, int i, double u);
    double N1(int i, double u);
    double N2(int i, double u);
    double N3(int i, double u);   // 三次B样条的基函数

    int currentNode;
    int getCurrentNode(QPointF t);

private slots:
    void on_actionhandSketch_triggered();

    void on_actiondelete_triggered();

    void on_actionpointEdit_triggered();

    void on_actionlineEdit_triggered();

    void on_actionrecEdit_triggered();

    void on_actionellipseEdit_triggered();

    void on_actiondataBase_triggered();

    void on_actionopenDataBase_triggered();

    void on_actionSaveToSql_triggered();

    void on_actionsetColor_triggered();

    void on_actionnoSelect_triggered();

    void on_actiondataPort_triggered();

    void on_actionOpenFile_triggered();

    void on_actionconnectToSql_triggered();

    void on_actionconnect_triggered();

    void on_actiontextEdit_triggered();

    void on_actionbrLine_triggered();

    void on_actionregionEdit_triggered();

    void on_actionMove_triggered();

    void on_actionopendb_triggered();

    void on_actionbuffer_triggered();

    void on_actionnewLayer_triggered();

    void on_actionnewLayerGroup_triggered();

    void on_actiondbScanCluster_triggered();

    void on_actionrefresh_triggered();

    void on_actiondeleteAnalysis_triggered();

    void on_actionsaveToFile_triggered();

    void on_actionsaveToPic_triggered();

    void on_actionsaveFile_triggered();

    void on_actionsavePic_triggered();

    void on_actionsetCoordinate_triggered();

    void on_actionsqlEdit_triggered();

    void on_actionDraw_triggered();

    void on_actionEdit_triggered();

    void on_actionAnalysis_triggered();

    void on_actionDb_triggered();

    void on_actionbSplineCurve_triggered();

    void on_actionTable_triggered();

    void on_actionrealDis_triggered();

    void on_actiondataOrigin_triggered();

    void on_actioninstruction_triggered();

//    void on_actiontif_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
