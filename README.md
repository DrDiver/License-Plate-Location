# License-Plate-Location
一个简单的车牌定位软件，用VS+OpenCV+Qt开发

一、开发环境
============
Visiual Studio 2017

OpenCV 3.4.13

Qt 5.14.2


![p0 (2)](https://user-images.githubusercontent.com/57578729/122718624-be57f300-d29f-11eb-9c31-7964dc037206.png)

二、实现过程
==============
1.颜色识别

![p01](https://user-images.githubusercontent.com/57578729/123499162-3ab75100-d667-11eb-9d6f-dd8cd1474e5b.png)

2.边缘识别

![p02](https://user-images.githubusercontent.com/57578729/123499201-97b30700-d667-11eb-9140-706d4c933de1.png)

三、轮廓判断
1.OpenCV坐标系和Rect类
OpenCV坐标系x轴水平向右为正，y轴垂直向下为正;

Rect类返回四个角点，其中P[0]为顶点中x值最小的点，顺时针旋转，依次为P[1],P[2],P[3]；

Rect类从P[0]顺时针旋转碰到的第一条边为width（宽），另一条边为height（高）；

其中width与x轴的夹角为返回的角度angle，取值范围为0-90°（角度制）。




