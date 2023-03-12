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

	//����ͼ��Ԫ����ģ��
	QStandardItemModel* ImgMetaModel()
	{
		return imgMetaModel;
	};
	
	//����ͼ��Ԫ����ģ��
	void SetMetaModel(QStandardItemModel* model)
	{
		this->imgMetaModel = model;
	};


	
	// �����ļ��б�����ģ��
	QStandardItemModel* FileListModel()
	{
		return fileListModel;
	};
	


	// ����fileListModelͼ���ļ��б�����ģ��
	void SetFileListModel(QStandardItemModel* model)
	{
		this->fileListModel = model;
	};

	QSize sizeHint() const;

	public slots:
	
	//�Ŵ�ͼ��
	void ZoomIn()
	{
		ScaleImg(1.2);
	};
	
	// ��Сͼ��
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

	void ShowBand(GDALRasterBand* band);//���ݲ�����Ŀ��ʾͼ��
	void ShowImg(QList<GDALRasterBand*> *imgBand);//��ʾդ������ͼ��
	void ShowImgInfor(const QString filename);//��ʾ������Ϣ
	void ShowFileList(const QString filename);//��ʾ�ļ����ƺͲ���
	unsigned char* ImgSketch(float* buffer, GDALRasterBand* currentBand, int bandSize, double noValue);
	

	//ͼ������
	void ScaleImg(double factor)
	{
		m_scaleFactor *= factor;
		QMatrix matrix;
		matrix.scale(m_scaleFactor, m_scaleFactor);
		this->setMatrix(matrix);
	};


	QStandardItemModel *imgMetaModel;//ͼ��Ԫ����ģ��
	

	GDALDataset *poDataset;//ͼ�����ݼ�


	float m_scaleFactor;//��������


	
	
	QStandardItemModel *fileListModel;// �ļ��б�����ģ��

	float m_floatfactor;//ͼ��ߴ�

	bool m_showColor;//�Ƿ�Ϊ������ͼ��

	
	QPoint lastEventCursorPos;

};
