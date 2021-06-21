//-----【程序说明】-----------------------------------
//  程序说明：车牌标注 debug
//  IDE版本：Visual Studio 2017
//  OpenCV版本：3.4.13
//  2021年5月 Created by 许睿晗
//----------------------------------------------------

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <time.h>

using namespace std;

using namespace cv;


Mat zip(Mat src)//分辨率压缩至880*600以下
{
	float x = src.cols, y = src.rows;
	float a = x / 880, b = y / 600;
	if (a <= 1 && b <= 1) src = src;
	else
	{
		if (a > b) resize(src, src, Size(int(src.cols / a), int(src.rows / a)), (0, 0), (0, 0), 3);
		else resize(src, src, Size(int(src.cols / b), int(src.rows / b)), (0, 0), (0, 0), 3);

	}
	//cout << a << "\t" << b << endl;
	
	return src;

}


class Pre3
{
public:

	Mat set(String path)
	{
		src = imread(path);
		if (!src.data)
			cout << "error" << endl;
		
		pixel = src.rows * src.cols;

		//cout << "OpenCV版本：" << CV_VERSION << endl;
		src_cp = imread(path);//副本
	
		//imshow("zip", src);

		return src;

	}


	bool color(Mat src)
	{
		GaussianBlur(src, proc, Size(5, 5), 0);
		imwrite("5.jpg", proc);
		cvtColor(proc, hsv, COLOR_BGR2HSV);
		imwrite("6.jpg", hsv);
		double low_H[3] = { 100, 11, 38 };
		double high_H[3] = { 124, 26, 90 };

		double low_S = 100;  double high_S = 255;
		double low_V = 70;  double high_V = 255;
		
		for (ch = 0; ch < 3; ch++)
		{
			inRange(hsv, Scalar(low_H[ch], low_S, low_V), Scalar(high_H[ch], high_S, high_V), proc);
			imwrite("7.jpg", proc);;
			Mat element = getStructuringElement(MORPH_RECT, Size(13, 3));
			morphologyEx(proc, dst, MORPH_CLOSE, element);
			
			imshow("dst", dst);
			imwrite("color.jpg", dst);

			if (contours())
			{
				String color;
				switch (ch)
				{
				case 0: color = "蓝色"; break;
				case 1: color = "黄色"; break;
				case 2: color = "绿色"; break;
				}

				cout << color << "车牌" << endl << endl;
				break;

			}

		}

		if (ch == 3)
		{
			cout << "颜色标注失败，进行灰度处理" << endl;

			//pix.clear();
			grayproc();

			if (contours())
			{
				cout << maxi << endl;
			}
			else
			{
				ch = -1;
				cout << "车牌标注失败,请手动标点" << endl;

			}

		}

		return ch == -1 ? true : false;

	}

	void grayproc()
	{
		cvtColor(src, gray, COLOR_BGR2GRAY);
		GaussianBlur(gray, proc, Size(5, 5), 0);
		imshow("高斯滤波 ", proc);
		imwrite("1.jpg", proc);
		Mat grad;
		Sobel(proc, grad, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
		convertScaleAbs(grad, proc);
		imshow("数值方向Sobel ", proc);
		imwrite("2.jpg", proc);

		cv::threshold(proc, proc, 120, 255, THRESH_BINARY);
		imshow("阈值 ", proc);
		imwrite("3.jpg", proc);

		Mat element = getStructuringElement(MORPH_RECT, Size(17, 5));
		morphologyEx(proc, dst, MORPH_CLOSE, element);
		//imshow("闭运算 ", dst);
		//imwrite("4.jpg", dst);
		t = Mat::zeros(dst.rows, dst.cols, CV_8UC3);
		vector<Mat> channels;
		for (int i = 0; i < 3; i++)
		{
			channels.push_back(dst);
		}
		merge(channels, t);

	}

	bool contours()
	{
		vector< vector< Point> > contours;
		findContours(dst, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		size_t n = contours.size(), temp = 0;
		const int m = 1000; //注意取值大小

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
		/*	for (size_t i = 0; i < 4; i++)
				line(t, P[i], P[(i + 1) % 4], Scalar(200, 200, 0), 2);
			imshow("闭运算 ", t);
			imwrite("4.jpg", t);*/

			for (size_t i = 0; i < 4; i++)
			{
				if (P[i].x < 0 || P[i].y < 0 || P[i].x > src.cols - 1 || P[i].y > src.rows - 1)
					pause = true;

			}
			if (pause) continue;

			x[i] = int(minAreaRect(p).size.width);
			y[i] = int(minAreaRect(p).size.height);
			int theta = int(minAreaRect(p).angle);

			area1[i] = int(contourArea(p));
			if (area1[i] < 200) continue;

			area2[i] = int(x[i] * y[i]);
			if (area2[i] < 200) continue;

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

			cout << i << "\t" << area1[i] << "\t\t" << area2[i] << "\t\t";
			cout << rate1[i] << " % \t" << rate2[i] << "%\t";
			cout << rate3[i] << " ‰\t" << angle[i] << "°" << endl;

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
			//cout << plate.cols <<"\t"<< plate.rows << endl;
			int area = plate.cols * plate.rows;
			if (area < 1e3)
			{
				pix.clear();
				maxi = -1;

			}

		}

		return maxi != -1 ? true : false;


	}

	Mat Plate(Mat src, Point2f P[4], int x, int y, int angle, size_t maxi)
	{
		if (maxi != -1)
		{
			cout << "序号\t" << "轮廓面积\t" << "矩形面积\t";
			cout << "占比1\t" << "长宽比\t" << "占比2\t" << "倾角" << endl;

		}

		for (size_t i = 0; i < max.size(); i++)
			line(src, max[i], max[(i + 1) % max.size()], Scalar(255, 255, 0), 2);


		float a = float(1.40);
		float b = float(3.14 / 180);

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
				P[0] = P[1] = P[2] = P[3] = Point2f(0, 0);
				bigger = Mat::zeros(1, 1, CV_8U);
				
				flag = false;
				break;
			}

		}
		if (flag)
		{

			for (size_t i = 0; i < 4; i++)
			{
				line(src, P[i], P[(i + 1) % 4], Scalar(0, 0, 255), 3);
				pix.push_back(P[i]);

			}

			//imshow("车牌标注", src);
			if (maxi != -1)
				cout << endl << maxi << "号轮廓车牌定位" << endl << pix << endl;

			//文件写入

			FileStorage fs("test.yaml", FileStorage::APPEND);

			//判断是否正常打开
			if (!fs.isOpened())
				cout << "failed" << endl;

			fs << "point" << "[" << pix[0] << pix[1] << pix[2] << pix[3] << "]";

			fs.release();

			//透视变换
			Rect rect = boundingRect(pix);

			Mat ROI = src(Rect(rect.x, rect.y, rect.width, rect.height));

			
			imshow("ROI", ROI);
			imwrite("ROI.jpg", ROI);

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

			
			resize(plate, bigger, Size(plate.cols * 3, plate.rows * 3));
			imshow("bigger", bigger);
			imwrite("bigger.jpg", plate);
		}
		imshow("src", src);
		imwrite("src.jpg",src);
		return plate;
	
		//return src;

	}

private:
	Mat bigger;
	Mat src, src_cp;
	Mat hsv;
	Mat gray;
	Mat proc;
	Mat dst;
	Mat plate;
	Mat t;

	int ch;
	String path;
	size_t pixel;
	size_t maxi = -1;
	vector<Point> p, max, pix;
	Point2f P[4];

};

int k = 0;
vector<Point> q;

void onMouse(int event, int x, int y, int flags, void* param)
{
	Point2f Q[4];
	Mat* plate = reinterpret_cast<Mat*>(param);
	if (event == EVENT_LBUTTONDOWN)  //鼠标左键返回坐标
	{
		Q[k] = Point(x, y);
		q.push_back(Q[k]);
		cout << Q[k] << endl;
		k++;
		if (k == 4)
		{
			minAreaRect(q).points(Q);
			Rect rect = boundingRect(q);

			Mat ROI = (*plate)(Rect(rect.x, rect.y, rect.width, rect.height));

			imshow("ROI", ROI);

			int x = int(minAreaRect(q).size.width);
			int y = int(minAreaRect(q).size.height);

			Pre3 pre;
			Mat plate0 = pre.Plate((*plate), Q, x, y, 0, -1);
			imshow("plate", plate0);

			k = 0;
			q.clear();

		}

	}

}

int main()
{
	Pre3 pre3;

	int i = 90;
	stringstream stream;
	stream << "pic/" << i << ".jpg";
	String path = stream.str();

	Mat src = pre3.set(path);

	if (pre3.color(src))
	{
		imshow("src", src);
		cv::setMouseCallback("src", onMouse,
			reinterpret_cast<void*> (&src));

	}

	waitKey(0);

	return 0;

}

