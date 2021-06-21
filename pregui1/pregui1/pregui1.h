#pragma once
#include "ui_pregui1.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <time.h>

#include <QMainWindow>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QTextCodec>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QScreen>
#include <QGuiApplication>
#include <QDesktopServices>
#include <QFile>
#include <QUrl>
#include <QProgressDialog> 

using namespace std;
using namespace cv;

class pregui1 : public QMainWindow
{
	Q_OBJECT

public:
	pregui1(QWidget *parent = Q_NULLPTR);
	Mat src1, src2, image;
	vector<Point> Qpic;
	int Qnum = 0;
	String path;
	QFileInfo fileInfo;
	QString filePath;
	QString fileName;
	QTextCodec *code = QTextCodec::codecForName("gb18030");

	int rx, ry;
	vector<Point> q;
	Point2f Qk[4];
	int autoOpen = 1;
	int autoPopup = 1;
	
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);

	void Qshow(QString filename);
	

	QPoint mMousePoint;
	bool mDragWindow;
	bool manual = false;
	
	

private:
	Ui::pregui1Class ui;

private slots:
	void Qclear();
	void on_checkBox_clicked();
	void on_checkBox_1_clicked();
	void on_pushButton_clicked();
	void on_pushButton_1_clicked();
	void on_pushButton_2_clicked();
	

};
	
class Pre3
{
public:

	Mat src;
	Mat src_cp;
	Mat plate;
	vector<Point> pix;
	Point2f P[4];

	Mat set(String path);

	bool color(Mat src);

	void grayproc(); 

	bool contours();

	Mat Plate(Mat src, Point2f P[4], int x, int y, int angle, size_t maxi);
	
private:

	Mat hsv;
	Mat gray;
	Mat proc; 
	Mat dst;
	

	int ch;
	String path;
	int pixel;
	int conpix;
	size_t maxi = -1;
	vector<Point> p, max;
	

};

Mat compress(Mat src);

QImage Mat2QImage(Mat& image);



