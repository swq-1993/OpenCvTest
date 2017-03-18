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
	Mat src = imread("1.png", 0);//ͼƬ������ӵ�����Ŀ¼��,Ҳ���Ǻ�Main.cpp�ļ�����һ���ļ����£�����
	if (!src.data)
	{
		cout << "��ȡͼƬ������ȷ��Ŀ¼���Ƿ���imread����ָ��ͼƬ����~�� " << endl;
		return false;
	}
	int a = getOptimalDFTSize(src.rows);//ͼƬ���ȿ��
	int b = getOptimalDFTSize(src.cols);
	cout << a << endl;
	cout << b << endl;
	imshow("����ͼƬ", src);
	Mat padded;
	//����߽���
	copyMakeBorder(src, padded, 0, a - src.rows, 0, b - src.cols, BORDER_CONSTANT, Scalar::all(0));
	cout << padded.size() << endl;

	Mat planes[] = { Mat_<float>(padded),Mat::zeros(padded.size(), CV_32F) };
	Mat complex;
	merge(planes, 2, complex);
	//DFT����
	dft(complex, complex);
	cout << "dft()֮��comlpex" << complex.size() << endl;
	//magniude()�����������ֵ��������ֵ������
	split(complex, planes);
	magnitude(planes[0], planes[1], planes[0]);
	Mat magtitudeImg = planes[0];
	//ת��Ϊ�����߶�
	magtitudeImg += Scalar::all(1);
	log(magtitudeImg, magtitudeImg);
	//��ȥ���䲿��
	magtitudeImg = magtitudeImg(Rect(0, 0, magtitudeImg.cols&-2, magtitudeImg.rows&-2));
	//x&-2����x��-2��λ���룬��-2�Ķ�������ʽ��2�Ķ�����ȡ����һ�Ľ�������ǲ�������⣩��2 �Ķ����ƽ���ǣ�������8λ��ʾ��ʵ��������32λ������������ʽ��һ���ģ�Ϊ������������8λ��ʾ��0000 0010����-2�Ķ�������ʽΪ��1111 1110����x��-2��λ����󣬲���x����������ż�������x������һ��ż����
	cout << "magtitudeImg.size=" << magtitudeImg.size() << endl;
	//�������Ļ�
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

	//��һ��������ʾͼ��
	normalize(magtitudeImg, magtitudeImg, 0.0, 1.0, NORM_MINMAX);
	imshow("��άͼ��Ƶ��", magtitudeImg);
	waitKey(0);
	return 0;
}