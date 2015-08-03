#include <stdio.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include<opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

Mat frame;

int cartoonifyFrame(Mat &src,Mat &dst)
{
    //get mask
    Mat gray;
    cvtColor(src,gray,CV_BGR2GRAY);
    const int MEDIAN_BLUR_FILTER_SIZE=7;
    medianBlur(gray,gray,MEDIAN_BLUR_FILTER_SIZE);
    Mat edges;
    const int LAPLACIAN_FILTER_SIZE=5;
    Laplacian(gray,edges,CV_8U,LAPLACIAN_FILTER_SIZE);
    Mat mask;
    const int EDGES_THRESHOLD =80;
    threshold(edges,mask,EDGES_THRESHOLD,255,THRESH_BINARY_INV);
    src.copyTo(dst,mask);

    //bilateral filt
    Size size = src.size();
    Size smallSize;
    smallSize.width=size.width/2;
    smallSize.height=size.height/2;
    int i;
    int ksize=9;//filter size ,has a large effet on processing speed
    double sigmaColor=9;//filter color strength
    double sigmaSpace=7;//spactial streng,affected speed
    int repetitions = 4;//repet times
    Mat smallImg=Mat(smallSize,CV_8UC3);
    resize(src,smallImg,smallSize,0,0);
    Mat tmp = Mat(smallSize,CV_8UC3);
    for(i = 0;i<repetitions;i++){
       bilateralFilter(smallImg,tmp,ksize,sigmaColor,sigmaSpace);
       bilateralFilter(tmp,smallImg,ksize,sigmaColor,sigmaSpace);
    }
    Mat bigImg;
    resize(smallImg,bigImg,size,0,0);
    dst.setTo(0);
    bigImg.copyTo(dst,mask);
}
int main(int argc,char * argv[])
{
    //init camera
    int camNumber=0;
    if(argc>1){
        camNumber=atoi(argv[1]);
    }
	VideoCapture webcam;
    webcam.open(camNumber);

    if(!webcam.isOpened()){
        printf("cam open error!\n");
    	exit(1);
    }

    webcam.set(CV_CAP_PROP_FRAME_WIDTH,640);
    webcam.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    while(webcam.read(frame)){//grab
        //process 
        Mat frame_display;
        cartoonifyFrame(frame,frame_display);
        //display
        imshow("show~!",frame_display);
        char key=waitKey(20);
        if(key==27){
            break;
        }
    }
    printf("break\n");
}

