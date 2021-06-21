#include "pregui1.h"
int cnt; //标号

pregui1::pregui1(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::FramelessWindowHint);
	//this->setWindowTitle(QString("License Plate Labeling"));

}

void pregui1::Qclear()//清空内容
{
	ui.label_1->clear();
	ui.label_2->clear();
	ui.label_3->clear();
	ui.label_4->clear();

	ui.label_1->setGeometry(0, 0, 880, 600);
	ui.label_2->setGeometry(0, 630, 880, 230);

}

void pregui1::on_checkBox_clicked()//自动打开车牌文件夹
{
	if (ui.checkBox->isChecked())
	{
		autoOpen = 0;
	}
	else
	{
		autoOpen = 1;
	}
}

void pregui1::on_checkBox_1_clicked()//手动标注时自动弹出预览车牌
{
	if (ui.checkBox_1->isChecked())
	{
		 autoPopup = 0;
	}
	else
	{
		autoPopup = 1;
	}
}

void pregui1::on_pushButton_clicked()//手动标注
{
	Qclear();
	manual = true;
	fileName = QFileDialog::getOpenFileName(this,
		tr("Open Image"), ".", tr("Image File (*.png *.jpg *.jpeg *.bmp)"));
	if (fileName.length() == 0)
	{
		QMessageBox::information(NULL, tr("Error"), tr("You didn't select any files."));
	}
	else
	{
		QTextCodec *code = QTextCodec::codecForName("gb18030");
		path = code->fromUnicode(fileName).data();

		image = imread(path);
		QImage Q = Mat2QImage(image);

		ui.label_1->setPixmap(QPixmap::fromImage(Q));
		ui.label_1->resize(Q.size());
		ui.label_1->show();

		ui.label_3->setText("Please right click four times to mark the license plate.");

		ui.label_4->setText("0/1");
	}

}

void pregui1::on_pushButton_1_clicked()//另存为
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("Images (*.png *.jpg *.jpeg *.bmp)")); 
	QScreen *screen = QGuiApplication::primaryScreen();
	screen->grabWindow(ui.label_2->winId()).save(filename);

}

void pregui1::on_pushButton_2_clicked()//选择图片
{
	Qclear();
	QString strs;
	QStringList file_list, output_name;
	QStringList str_path_list = QFileDialog::getOpenFileNames(this, 
		tr("Open Image"), ".", tr("Image File (*.png *.jpg *.jpeg *.bmp)"));
	int n = str_path_list.size();
	
	QProgressDialog dialog(tr(""), tr(""), 0, n, this);
	dialog.setFixedSize(300, 100);
	dialog.setWindowTitle(tr("dialog"));
	dialog.setWindowModality(Qt::WindowModal);

	dialog.show();

	for (int i = 0; i < n; i++) 
	{
		ui.label_4->setText(QString::number(i+1)+"/"+ QString::number(n));//进度

		QString str_path = str_path_list[i];
		Qshow(str_path);
		
		dialog.setValue(i);
		QCoreApplication::processEvents();
		if (dialog.wasCanceled())
			break;
		
	}

	dialog.setValue(n);

	if (autoOpen)
	{
		QString Name = "plate";
		QString Path = QString("%1/%2").arg(filePath).arg(Name);
		QDesktopServices::openUrl(QUrl(Path));

	}
	
}

void pregui1::Qshow(QString filename)//标注车牌并保存
{
	Pre3 pre3;
	path = code->fromUnicode(filename).data();
	Mat src = pre3.set(path);

	fileInfo = QFileInfo(filename);
	filePath = fileInfo.absolutePath();//绝对路径
	fileName = fileInfo.fileName();//文件名

	QDir dir;
	dir.cd(filePath);
	if (!dir.exists("plate"))
	{
		dir.mkdir("plate");
	}
	if (!dir.exists("compress"))
	{
		dir.mkdir("compress");
	}

	QString imageName1 = QString(filePath + "/%1/%2").arg("compress").arg(fileName);
	image = imread(path);
	Mat img = compress(image);
	QImage Q = Mat2QImage(img);
	Q.save(imageName1);

	if (pre3.color(src))
	{
		ui.label_3->setText("error");
		FileStorage fs("error.yaml", FileStorage::APPEND);
		if (!fs.isOpened())
			cout << "failed" << endl;
		fs << "path" << String(path);
		fs << "error" << false;
		fs.release();
	}

	else 
	{
		Qpic = pre3.pix;
		src1 = compress(pre3.src_cp);
		src2 = pre3.plate;
		Mat src3;
		cv::resize(src2, src3, Size(src2.cols * 2, src2.rows * 2));
		src = compress(pre3.src);
		
		QImage Q1 = Mat2QImage(src1);
		QImage Q2 = Mat2QImage(src2);
		QImage Q3 = Mat2QImage(src3);
		QImage Q1_cp = Mat2QImage(src);

		/*ui.label_1->setPixmap(QPixmap::fromImage(Q1.scaled(500, 500, Qt::KeepAspectRatio)));
        ui.label_1->resize(500,500);
		ui.label_1->show();*/

		ui.label_1->setPixmap(QPixmap::fromImage(Q1));
		ui.label_1->resize(Q1.size());
		ui.label_1->show();

		ui.label_2->setPixmap(QPixmap::fromImage(Q3));
		ui.label_2->resize(Q3.size());
		ui.label_2->show();
		
		ui.label_3->setText( fileInfo.fileName() + " points\n"
			"[" + QString::number(Qpic[0].x) + "," + QString::number(Qpic[0].y) + "]\n"
			"[" + QString::number(Qpic[1].x) + "," + QString::number(Qpic[1].y) + "]\n"
			"[" + QString::number(Qpic[2].x) + "," + QString::number(Qpic[2].y) + "]\n"
			"[" + QString::number(Qpic[3].x) + "," + QString::number(Qpic[3].y) + "]\n");

		//QString imageName1 = QString(filePath + "/%1/%2").arg("compress").arg(fileName);
		//Q1_cp.save(imageName1);

		QString imageName2 = QString(filePath+"/%1/%2").arg("plate").arg(fileName);
		Q2.save(imageName2);

		String Qpath = code->fromUnicode(fileName).data();
		
		FileStorage fs("test.yaml", FileStorage::APPEND);

		if (!fs.isOpened())
			cout << "failed" << endl;

		fs << "path" << String(Qpath);
		fs << "plate point" << "[" << Qpic[0] << Qpic[1] << Qpic[2] << Qpic[3] << "]";

		fs.release();

	}

}

void pregui1::mousePressEvent(QMouseEvent *e)
{
	// 鼠标左键按下
	if (e->button() == Qt::LeftButton) 
	{
		mMousePoint = e->globalPos() - this->pos();
		e->accept();
		mDragWindow = true;
	}

	// 鼠标右键按下
	else if (e->button() == Qt::RightButton && manual)
	{
		Qk[Qnum].x = e->x();
		Qk[Qnum].y = e->y();

		q.push_back(Qk[Qnum]);
		Qnum++;
	}

	if (Qnum == 4)
	{
		Pre3 pre4;

		Mat src = pre4.set(path);
		Mat src_cp = pre4.set(path);

		bool pause = false;
		for (size_t i = 0; i < 4; i++)
		{
			if (Qk[i].x < 0 || Qk[i].x > src.cols || Qk[i].y < 0 || Qk[i].y > src.rows)
			{
				pause = true;
				break;

			}
			line(src_cp, Qk[i], Qk[(i + 1) % 4], Scalar(0, 0, 255), 2);//框出手动标记轮廓
				
		}
			
		if (pause)
		{
			QMessageBox::information(NULL, tr("Error"), tr("Please retry."));
			Qnum = 0;
			q.clear();
		}
		else
		{
			minAreaRect(q).points(Qk);
			Rect rect = boundingRect(q);
		
			Mat ROI = src(Rect(rect.x, rect.y, rect.width, rect.height));

			//imshow("ROI", ROI);

			rx = int(minAreaRect(q).size.width);
			ry = int(minAreaRect(q).size.height);

			Mat plate0 = pre4.Plate(src, Qk, rx, ry, 0, -1);
			QImage Q0 = Mat2QImage(plate0);
			Point Q[4];

			for (size_t i = 0; i < 4; i++) Q[i] = Point(Qk[i]);//坐标变为int类
	
			QImage Qsrc = Mat2QImage(src_cp);
			ui.label_1->setPixmap(QPixmap::fromImage(Qsrc));
			ui.label_1->resize(Qsrc.size());
			ui.label_1->show();

			ui.label_2->setPixmap(QPixmap::fromImage(Q0));
			ui.label_2->resize(Q0.size());
			ui.label_2->show();

			if (autoPopup)
			{
				QLabel* labelImage = new QLabel(this, Qt::Dialog | Qt::WindowCloseButtonHint);
				labelImage->setWindowTitle("Plate");
				labelImage->resize(QSize(Q0.width(), Q0.height()));
				labelImage->setPixmap(QPixmap::fromImage(Q0));
				labelImage->show();

			}

			fileInfo = QFileInfo(fileName);
			filePath = fileInfo.absolutePath();//绝对路径
			fileName = fileInfo.fileName();//文件名
			String Qpath = code->fromUnicode(fileName).data();

			ui.label_3->setText( fileInfo.fileName() + " points\n"
				"[" + QString::number(Q[0].x) + "," + QString::number(Q[0].y) + "]\n"
				"[" + QString::number(Q[1].x) + "," + QString::number(Q[1].y) + "]\n"
				"[" + QString::number(Q[2].x) + "," + QString::number(Q[2].y) + "]\n"
				"[" + QString::number(Q[3].x) + "," + QString::number(Q[3].y) + "]\n");
		
			ui.label_4->setText("1/1");

			FileStorage fs("point.yaml", FileStorage::APPEND);
			if (!fs.isOpened())
				cout << "failed" << endl;

			fs << "path" << String(Qpath);
			fs << "plate point" << "[" << Q[0] << Q[1] << Q[2] << Q[3] << "]";
			fs.release();

			Qnum = 0;
			q.clear();

		}
		
	}

}

void pregui1::mouseReleaseEvent(QMouseEvent *)
{
	mDragWindow = false;
}

void pregui1::mouseMoveEvent(QMouseEvent *e)
{
	if (mDragWindow && (e->buttons() && Qt::LeftButton)) 
	{
		this->move(e->globalPos() - mMousePoint);
		e->accept();
	}
}

//警惕中文字符

Mat Pre3::set(String path)
{
	src = imread(path);
	if (!src.data)
		cout << "error" << endl;
	
	pixel = src.rows * src.cols;
	conpix = size_t(pixel / 500);

	src_cp = imread(path);//副本
	
	return src;

}

Mat compress(Mat src)//分辨率压缩至880*600以下
{
	float x = src.cols, y = src.rows;
	float a = x / 880, b = y / 600;
	if (a <= 1 && b <= 1) src = src;
	else
	{
		if (a > b) resize(src, src, Size(int(src.cols / a), int(src.rows / a)));
		else resize(src, src, Size(int(src.cols / b), int(src.rows / b)));

	}

	return src;

}

bool Pre3::color(Mat src)
{
	GaussianBlur(src, proc, Size(5, 5), 0);

	cvtColor(proc, hsv, COLOR_BGR2HSV);

	double low_H[3] = { 100, 11, 38 };
	double high_H[3] = { 124, 26, 90 };

	double low_S = 100;  double high_S = 255;
	double low_V = 70;  double high_V = 255;

	for (ch = 0; ch < 3; ch++)
	{
		inRange(hsv, Scalar(low_H[ch], low_S, low_V), Scalar(high_H[ch], high_S, high_V), proc);

		Mat element = getStructuringElement(MORPH_RECT, Size(13, 3));
		morphologyEx(proc, dst, MORPH_CLOSE, element);

		//imshow("dst", dst);

		if (contours())
		{
			String color;
			switch (ch)
			{
			case 0: color = "蓝色"; break;
			case 1: color = "黄色"; break;
			case 2: color = "绿色"; break;
			}

			//cout << color << "车牌" << endl << endl;
			break;

		}

	}

	if (ch == 3)
	{
		cout << "颜色标注失败，进行灰度处理" << endl;
		grayproc();
		if (contours())
		{
			
		}
		else
		{
			ch = -1;
			cout << "车牌标注失败,请手动标点" << endl;

		}

	}

	return ch == -1 ? true : false;

}

void Pre3::grayproc()
{
	cvtColor(src, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, proc, Size(5, 5), 0);
	//imshow("高斯滤波 ", proc);

	Mat grad;
	Sobel(proc, grad, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad, proc);
	//imshow("数值方向Sobel ", proc);

	cv::threshold(proc, proc, 100, 255, THRESH_BINARY);
	//imshow("阈值 ", proc);

	Mat element = getStructuringElement(MORPH_RECT, Size(17, 5));
	morphologyEx(proc, dst, MORPH_CLOSE, element);
	//imshow("闭运算 ", dst);


}

bool Pre3::contours()
{
	vector< vector< Point> > contours;
	findContours(dst, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	size_t n = contours.size(), temp = 0;
	const int m = 1500; //注意取值大小

	int x[m], y[m], angle[m], area1[m], area2[m], rate1[m], rate2[m], rate3[m];

	// Rect注意
	// 1. x轴水平向右，y轴垂直向下;
	// 2. p0为顶点中x值最小的点，顺时针旋转，依次为p1,p2,p3；
	// 3. 从p0顺时针旋转碰到的第一条边为width，另一条边为height；
	// 4. width与x轴的夹角为返回的角度angle，取值0-90°，为角度制；

	for (size_t i = 0; i < n; i++)
	{
		p = contours[i];

		//if (ch == 3)
			//approxPolyDP(p, p, 5, true);

		bool pause = false; //判断坐标是否越界

		minAreaRect(p).points(P);

		for (size_t i = 0; i < 4; i++)
		{
			if (P[i].x < 0 || P[i].y < 0 || P[i].x > src.cols-1 || P[i].y > src.rows-1 ) 
				pause = true;

		}
		if (pause) continue;

		x[i] = int(minAreaRect(p).size.width);
		y[i] = int(minAreaRect(p).size.height);
		int theta = int(minAreaRect(p).angle);
		
		
		area1[i] = int(contourArea(p));
		if (area1[i] < conpix) continue;

		area2[i] = int(x[i] * y[i]);
		if (area2[i] < conpix) continue;

		rate1[i] = int(100 * area1[i] / area2[i]);

		if (ch == 2)
		{
			if (rate1[i] < 60) continue;
		}
		else if (ch == 3)
		{
			if (rate1[i] < 35) continue;
		}

		else if (rate1[i] < 70) continue;

		if (x[i] > y[i])
		{
			angle[i] = theta;
			rate2[i] = int(100 * x[i] / y[i]);

		}
		else
		{
			angle[i] = 90 - theta;
			rate2[i] = int(100 * y[i] / x[i]);
		}

		if (angle[i] > 40) continue;

		if (ch != 2)
		{
			if (rate2[i] < 180 || rate2[i] > 450) continue;
		}
		if (ch == 2)
		{
			if (rate2[i] < 300 || rate2[i] > 660) continue;
		}


		rate3[i] = int(1000 * area2[i] / pixel);
		if (rate3[i] > 120 || rate3[i] < 2) continue;

		/*cout << i << "\t" << area1[i] << "\t\t" << area2[i] << "\t\t";
		cout << rate1[i] << " % \t" << rate2[i] << "%\t";
		cout << rate3[i] << " ‰\t" << angle[i] << "°" << endl;*/

		if (area1[i] > temp)
		{
			max = p;
			temp = area1[i];
			maxi = i;

		}

	}

	if (maxi != -1)
	{
		minAreaRect(max).points(P);
		plate = Plate(src, P, x[maxi], y[maxi], angle[maxi], maxi);
		
		FileStorage fs("data.yaml", FileStorage::APPEND);

		if (!fs.isOpened())
			cout << "failed" << endl;


		cnt++;
		fs << "data" << "[" << cnt << ch << angle[maxi] << area1[maxi];
		fs << rate1[maxi] << rate2[maxi] << rate3[maxi]  << "]";

		
	
		fs.release();


		int area = plate.cols * plate.rows;
		if (area < 1e3)
		{
			pix.clear();
			maxi = -1;

		}

	}

	return maxi != -1 ? true : false;

}

Mat Pre3::Plate(Mat src, Point2f P[4], int x, int y, int angle, size_t maxi)
{
	if (maxi != -1)
	{
		/*cout << "序号\t" << "轮廓面积\t" << "矩形面积\t";
		cout << "占比1\t" << "长宽比\t" << "占比2\t" << "倾角" << endl;*/

	}

	//for (size_t i = 0; i < max.size(); i++)
		//line(src, max[i], max[(i + 1) % max.size()], Scalar(255, 255, 0), 1);


	float a = float(1.50);
	float b = float(3.14 / 180);//角度转弧度

	if (ch == 2) //车牌上边缘拓宽，因只能检测绿色
	{
		if (x > y)
		{
			P[1] = Point2f(P[0].x + a * y * sin(b * angle),
				           P[0].y - a * y * cos(b * angle));
			P[2] = Point2f(P[3].x + a * y * sin(b * angle),
				           P[3].y - a * y * cos(b * angle));

		}
		else
		{
			P[0] = Point2f(P[3].x - a * x * sin(b * angle),
				           P[3].y - a * x * cos(b * angle));
			P[1] = Point2f(P[2].x - a * x * sin(b * angle),
				           P[2].y - a * x * cos(b * angle));

		}

	}

	bool flag = true;
	for (size_t i = 0; i < 4; i++)
	{
		if (P[i].x < 0 || P[i].y < 0 || P[i].x > src.cols - 1 || P[i].y > src.rows - 1)
		{
			P[0] = P[1] = P[2]= P[3] = Point2f(0, 0);
			plate = Mat::zeros(1,1,CV_8U);
			flag = false;
			
			break;
		}

	}
	if (flag) 
	{
		for (size_t i = 0; i < 4; i++)
		{
			line(src_cp, P[i], P[(i + 1) % 4], Scalar(0, 0, 255), 3);
			pix.push_back(P[i]);

		}

		if (maxi != -1)
			cout << endl << maxi << "号轮廓车牌定位 " << endl << pix << endl;

		//透视变换
		Rect rect = boundingRect(pix);

		Mat ROI = src(Rect(rect.x, rect.y, rect.width, rect.height));
		//rectangle(src, rect, Scalar(0, 255, 255), 2);

		//imshow("ROI", ROI);
		

		vector<Point2f> pp(4), dp(4);

		pp[0] = Point(0, pix[0].y - pix[1].y);
		pp[1] = Point(pix[1].x - pix[0].x, 0);
		pp[2] = Point(pix[2].x - pix[0].x, pix[2].y - pix[1].y);
		pp[3] = Point(pix[3].x - pix[0].x, pix[3].y - pix[1].y);


		if (y > x)
		{
			if (ch == 2) x = int(a * x);

			dp[0] = Point(0, 0);
			dp[1] = Point(y, 0);
			dp[2] = Point(y, x);
			dp[3] = Point(0, x);

			Mat warp = getPerspectiveTransform(pp, dp);
			warpPerspective(ROI, plate, warp, Size(y, x));

		}
		else
		{
			if (ch == 2) y = int(a * y);

			dp[0] = Point(0, y);
			dp[1] = Point(0, 0);
			dp[2] = Point(x, 0);
			dp[3] = Point(x, y);

			Mat warp = getPerspectiveTransform(pp, dp);
			warpPerspective(ROI, plate, warp, Size(x, y));

		}

		//imshow("车牌", plate);
	}
	return plate;
}

QImage Mat2QImage(Mat& image)
{
	QImage img;

	if (image.channels() == 3) 
	{
		cvtColor(image, image, CV_BGR2RGB);
		img = QImage((const unsigned char*)(image.data), image.cols, image.rows,
			image.cols * image.channels(), QImage::Format_RGB888);
	}
	else if (image.channels() == 1) 
	{
		img = QImage((const unsigned char*)(image.data), image.cols, image.rows,
			image.cols * image.channels(), QImage::Format_ARGB32);
	}
	else 
	{
		img = QImage((const unsigned char*)(image.data), image.cols, image.rows,
			image.cols * image.channels(), QImage::Format_RGB888);
	}

	return img;
}
