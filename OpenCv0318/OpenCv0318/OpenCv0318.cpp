#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\core.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("1.png", 0);//图片必须添加到工程目录下,也就是和Main.cpp文件放在一个文件夹下！！！
	if (!src.data)
	{
		cout << "读取图片错误，请确定目录下是否有imread函数指定图片存在~！ " << endl;
		return false;
	}
	int a = getOptimalDFTSize(src.rows);//图片长度宽度
	int b = getOptimalDFTSize(src.cols);
	cout << a << endl;
	cout << b << endl;
	imshow("测试图片", src);
	Mat padded;
	//处理边界卷积
	copyMakeBorder(src, padded, 0, a - src.rows, 0, b - src.cols, BORDER_CONSTANT, Scalar::all(0));
	cout << padded.size() << endl;

	Mat planes[] = { Mat_<float>(padded),Mat::zeros(padded.size(), CV_32F) };
	Mat complex;
	merge(planes, 2, complex);
	//DFT处理
	dft(complex, complex);
	cout << "dft()之后，comlpex" << complex.size() << endl;
	//magniude()函数计算幅度值，将幅度值对数化
	split(complex, planes);
	magnitude(planes[0], planes[1], planes[0]);
	Mat magtitudeImg = planes[0];
	//转化为对数尺度
	magtitudeImg += Scalar::all(1);
	log(magtitudeImg, magtitudeImg);
	//减去补充部分
	magtitudeImg = magtitudeImg(Rect(0, 0, magtitudeImg.cols&-2, magtitudeImg.rows&-2));
	//x&-2代表x与-2按位相与，而-2的二进制形式是2的二进制取反加一的结果（这是补码的问题）。2 的二进制结果是（假设用8位表示，实际整型是32位，但是描述方式是一样的，为便于描述，用8位表示）0000 0010，则-2的二进制形式为：1111 1110，在x与-2按位相与后，不管x是奇数还是偶数，最后x都会变成一个偶数。
	cout << "magtitudeImg.size=" << magtitudeImg.size() << endl;
	//坐标中心化
	int cx = magtitudeImg.cols / 2;
	int cy = magtitudeImg.rows / 2;
	Mat quadrant1 = magtitudeImg(Rect(0, 0, cx, cy));
	Mat quadrant2 = magtitudeImg(Rect(cx, 0, cx, cy));
	Mat quadrant3 = magtitudeImg(Rect(0, cy, cx, cy));
	Mat quadrant4 = magtitudeImg(Rect(cx, cy, cx, cy));

	Mat temp;
	quadrant1.copyTo(temp);
	quadrant4.copyTo(quadrant1);
	temp.copyTo(quadrant4);

	quadrant2.copyTo(temp);
	quadrant3.copyTo(quadrant2);
	temp.copyTo(quadrant3);

	//归一化，并显示图像
	normalize(magtitudeImg, magtitudeImg, 0.0, 1.0, NORM_MINMAX);
	imshow("二维图像频谱", magtitudeImg);
	waitKey(0);
	return 0;
}