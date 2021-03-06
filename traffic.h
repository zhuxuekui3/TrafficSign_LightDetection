#ifndef TRAFFIC_H
#define TRAFFIC_H

#include <iostream>
#include <fstream>
#include "opencv2/opencv.hpp"
#include <vector>
#include <stdlib.h>

#define IS_SAVE 1  //摄像头采集信息是否保存
#define OPENMP 0//是否使用OPENMP并行处理
#define ISDEBUG_TL 0//调试时用来输出中间调试图像及信息(Traffic Light)
#define ISDEBUG_TS 0//调试时用来输出中间调试图像及信息(Traffic Sign)
#define IS_SHOW_RESULT 1//是否显示检测结果,不显示图像结果处理速度更快
#define GREEN_PIXEL_LABEL 255
#define RED_PIXEL_LABEL 128
#define NON_BLOB_PIXEL_LABEL 0
#define ROIHeight 480
#define ROIWidth 0
#define PI 3.1415
#define RESULT_G 0
#define RESULT_R 1
#define RESULT_NON 2
#define BIASX 50  //矩形框偏移


#define PosSamNO    69 //正样本个数
#define HORZ_PosSamNO    42 //正样本个数
#define NegSamNO 2000   //负样本个数
#define HORZ_NegSamNO 3042
#define HardExampleNO 18
#define HORZ_HardExampleNO 21

//形状信息定义
#define TRIANGLE 0
#define CIRCLE 1
#define HEXA 2
#define RECTANGLE 3

const int HOG_TLVertical_Height = 30;
const int HOG_TLVertical_Width = 15;
const int HOG_TLHorz_Height = 15;
const int HOG_TLHorz_Width = 30;
//HardExample：负样本个数。如果HardExampleNO大于0，表示处理完初始负样本集后，继续处理HardExample负样本集。
//不使用HardExample时必须设置为0，因为特征向量矩阵和特征类别矩阵的维数初始化时用到这个值
int const containerLen=12;//length of the time sequence filter
int const deleteThresholdNum = 9;//when the number of undetected TL is larger than deleteThresholdNum, delete the corresponding tracked object
int const occurenceThresholdNum = 4;//when the number of detected TL is larger than occurenceThresholdNum, draw the corresponding detection window on the image
using namespace std;
using namespace cv;


class DetecResult{
public:
	int LightResult[8];
	//int LightPos[8];
	Rect LightPos[8];
	DetecResult()
	{
		for(int i=0;i<8;i++)
		{
			LightResult[i]=RESULT_NON;
		}

	}

};

struct ShapeRecResult{
//public:
	Rect box;
	int color;
	int shape;
};

class RectTracker{
public:
	ShapeRecResult trackedBox;
	bool isDraw;//检测框是否可以显示在当前帧上
	deque<int> signs;

	//计算该跟踪目标是否可以被删除(返回true表示可以被删除)
	bool isCanDelete(){
		int len = signs.size();
		int zeroNum = 0;
		if (len < containerLen)return false;

		for (int i = 0; i < len; i++){
			if (signs[i] == 0)zeroNum++;
		}

		if (zeroNum>deleteThresholdNum)return true;
		else
			return false;
	}


	//计算该跟踪目标检测框是否可以在图像上显示
	bool calcDraw(){
		int len = signs.size();
		int occurenceNum = 0;
		if (len < containerLen)return false;
		for (int i = 0; i < len; i++){
			if (signs[i] == 1)occurenceNum++;
		}
			
		if (occurenceNum >= occurenceThresholdNum)
			return true;
		else
			return false;
	}
};


class MySVM : public CvSVM
{
public:
	//获得SVM的决策函数中的alpha数组
	double * get_alpha_vector()
	{
		return this->decision_func->alpha;
	}

	//获得SVM的决策函数中的rho参数,即偏移量
	float get_rho()
	{
		return this->decision_func->rho;
	}
};

IplImage* colorSegmentation(IplImage* inputImage);
void showHist(Mat src);
void hogSVMTrain( HOGDescriptor &myHOG,bool TRAIN);
void BoxDetect(Mat src_test,HOGDescriptor &myHOG,vector<Rect> &found_filtered);
void rgb2hsi(int red, int green, int blue, int& hue, int& saturation, int& intensity );
void setLabel(cv::Mat& im, const std::string label, Rect r);//show the label information
int detect_result(Mat src_test,vector<Rect> &found_filtered,DetecResult *detct,char Direct);
int SortRect(Mat src_test,int num,DetecResult *Rst,char Direct);
int HOGTrainingTrafficSign(const String path,HOGDescriptor &hog,int num_folder,int imgWidth,int imgHeight,String XMLName);//training SVM multi-Classifier for traffic signs recognition
Mat ShapeRecognize(Mat src,vector<ShapeRecResult>&shapeResult);

#endif