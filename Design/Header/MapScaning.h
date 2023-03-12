#pragma once

#include <QGraphicsView>
#include <QStandardItemModel>
#include <QPixmap>
#include<QMessageBox>
#include <QFileInfo>
#include <QWheelEvent>
#include <QScrollBar>
#include <QGraphicsPixmapItem>
#include<QMouseEvent>
#include "gdal_priv.h"
#include"ogrsf_frmts.h"

using namespace std;

class MapScaning : public QGraphicsView
{
	Q_OBJECT

public:
	MapScaning(QWidget *parent);
	~MapScaning();

	void ReadImg(const QString imgPath);
	void CloseCurrentImg();

	//返回图像元数据模型
	QStandardItemModel* ImgMetaModel()
	{
		return imgMetaModel;
	};
	
	//设置图像元数据模型
	void SetMetaModel(QStandardItemModel* model)
	{
		this->imgMetaModel = model;
	};


	
	// 返回文件列表数据模型
	QStandardItemModel* FileListModel()
	{
		return fileListModel;
	};
	


	// 设置fileListModel图像文件列表数据模型
	void SetFileListModel(QStandardItemModel* model)
	{
		this->fileListModel = model;
	};

	QSize sizeHint() const;

	public slots:
	
	//放大图像
	void ZoomIn()
	{
		ScaleImg(1.2);
	};
	
	// 缩小图像
	void ZoomOut()
	{
		ScaleImg(0.8);
	};

protected:

	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:

	void ShowBand(GDALRasterBand* band);//根据波段数目显示图像
	void ShowImg(QList<GDALRasterBand*> *imgBand);//显示栅格数据图像
	void ShowImgInfor(const QString filename);//显示数据信息
	void ShowFileList(const QString filename);//显示文件名称和波段
	unsigned char* ImgSketch(float* buffer, GDALRasterBand* currentBand, int bandSize, double noValue);
	

	//图像缩放
	void ScaleImg(double factor)
	{
		m_scaleFactor *= factor;
		QMatrix matrix;
		matrix.scale(m_scaleFactor, m_scaleFactor);
		this->setMatrix(matrix);
	};


	QStandardItemModel *imgMetaModel;//图像元数据模型
	

	GDALDataset *poDataset;//图像数据集


	float m_scaleFactor;//缩放因子


	
	
	QStandardItemModel *fileListModel;// 文件列表数据模型

	float m_floatfactor;//图像尺寸

	bool m_showColor;//是否为单波段图像

	
	QPoint lastEventCursorPos;

};
