// Opencvtest.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	
	Mat image = imread("lena.bmp");
	Mat grayimage;
	Mat thimage;
	Mat connectimg;
	int pixel=0;
	int value[256];
	cvtColor(image,grayimage, CV_BGR2GRAY);
	cvtColor(image,thimage, CV_BGR2GRAY);
	cvtColor(thimage,connectimg, CV_GRAY2BGR);
	imshow("initial",grayimage);

	
	// threshold
	for(int i=0; i<grayimage.rows; i++){
		for(int j=0; j<grayimage.cols; j++){
			pixel = grayimage.at<uchar>(i,j);
			if(pixel >= 128){
				thimage.at<uchar>(i,j) = 255;
			}else{
				thimage.at<uchar>(i,j) = 0;
			}
		}
	}
	imshow("binary",thimage);

	//histogram
	
	//initial
	for(int i=0; i<256; i++){
		value[i] = 0;
	}
	//calculate
	for(int i=0; i<grayimage.rows; i++){
		for(int j=0; j<grayimage.cols; j++){
			pixel = grayimage.at<uchar>(i,j);
			value[pixel]++;
		}
	}
	//find largest
	int largest=0;
	for(int i=0; i<256; i++){
		if(largest<value[i]){
			largest = value[i] ;
		}
	}
	//set largest
	for(int i=0; i<256; i++){
		value[i] = value[i]*255/largest;
	}

	//draw histogram
	Mat histImg (256,256,CV_8UC3,Scalar(0,0,0));
	for(int i=0; i<256; i++){
		line(histImg,Point(i,256-value[i]),Point(i,256),Scalar(255,255,255));
	}
	imshow("histogram",histImg);


	//connected component

	int label = 1;
	int lpixel = 0;
	int upixel = 0;
	Mat imgarray(512, 512, CV_32FC1);
	Mat check(512, 512, CV_32FC1);

	


	//initial
	for(int i=0; i<thimage.rows; i++){
		for(int j=0; j<thimage.cols; j++){
			imgarray.at<int>(i,j) = 0 ;
			check.at<int>(i,j) = 0 ;
		}
	}
	//top down
	for(int i=0; i<thimage.rows; i++){
		for(int j=0; j<thimage.cols; j++){
			pixel = thimage.at<uchar>(i,j);
			if(i==0 && j==0){
				if(pixel == 0){

				}else{
					//label
					 imgarray.at<int>(i,j) = label;
					label++;
				}//end pixel
			}else if(i==0 && j!=0){
				lpixel = thimage.at<uchar>(i,j-1);
				if(pixel == 0){

				}else if(pixel == 255 && lpixel == 255){
					imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1);
				}else{
					//label
					imgarray.at<int>(i,j) = label;
					label++;
				}//end pixel
			}else if(i!=0 && j==0){
				upixel = thimage.at<uchar>(i-1,j);
				if(pixel == 0){

				}else if(pixel == 255 && upixel ==255){
					imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j);
				}else{
					//label
					imgarray.at<int>(i,j) = label;
					label++;
				}//end pixel
			}else{
				lpixel = thimage.at<uchar>(i,j-1);
				upixel = thimage.at<uchar>(i-1,j);
				if(pixel == 0){

				}else if(pixel == 255 && lpixel == 0 && upixel == 0){
					//label
					imgarray.at<int>(i,j) = label;
					label++;
				}else if(pixel == 255 && lpixel == 255 && upixel == 0){
					imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1);
				}else if(pixel == 255 && lpixel == 0 && upixel == 255){
					imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j);
				}else{
					//connected
					if(imgarray.at<int>(i-1,j) <= imgarray.at<int>(i,j-1)){
						imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j);
					}else{
						imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1);
					}
				}//end pixel
			}//end i,j zero or else
			
		}//end j
	}// end i


	bool repeat = true ;

	while(repeat){

		repeat = false;
		
		//check array change
		for(int i=0; i<thimage.rows; i++){
			for(int j=0; j<thimage.cols; j++){
				if(check.at<int>(i,j) != imgarray.at<int>(i,j)){
					repeat = true;
				}
				check.at<int>(i,j) = imgarray.at<int>(i,j) ;
			}
		}

		if(repeat){
			//bottom up
			lpixel = 0;
			upixel = 0;
			int rpixel = 0;
			int dpixel = 0;
			for(int i=thimage.rows-1; i>=0; i--){
				for(int j=thimage.cols-1; j>=0; j--){
					pixel = imgarray.at<int>(i,j);
					if(i==thimage.rows-1 && j==thimage.cols-1){//corner right down
						lpixel = imgarray.at<int>(i,j-1);
						upixel = imgarray.at<int>(i-1,j);
						if(pixel == 0){

						}else{
							if(lpixel == upixel == 0){

							}else if(lpixel == 0 && upixel != 0){
								if(upixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
								}
							}else if(lpixel != 0 && upixel == 0){
								if(lpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
								}
							}else{
								if(upixel < lpixel){
									if(upixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(lpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}
								}
							}
						}
					}else if(i==0 && j==thimage.cols-1){//corner right up
						lpixel = imgarray.at<int>(i,j-1);
						dpixel = imgarray.at<int>(i+1,j);
						if(pixel == 0){

						}else{
							if(lpixel == dpixel == 0){

							}else if(lpixel == 0 && dpixel != 0){
								if(dpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
								}
							}else if(lpixel != 0 && dpixel == 0){
								if(lpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
								}
							}else{
								if(dpixel < lpixel){
									if(dpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}else{
									if(lpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}
								}
							}
						}
					}else if(i==thimage.rows-1 && j==0){//corner left down
						rpixel = imgarray.at<int>(i,j+1);
						upixel = imgarray.at<int>(i-1,j);
						if(pixel == 0){

						}else{
							if(rpixel == upixel == 0){

							}else if(rpixel == 0 && upixel != 0){
								if(upixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
								}
							}else if(rpixel != 0 && upixel == 0){
								if(rpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
								}
							}else{
								if(upixel < rpixel){
									if(upixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(rpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}
								}
							}
						}
					}else if(i==0 && j==0){//corner left up
						rpixel = imgarray.at<int>(i,j+1);
						dpixel = imgarray.at<int>(i+1,j);
						if(pixel == 0){

						}else{
							if(rpixel == dpixel == 0){

							}else if(rpixel == 0 && dpixel != 0){
								if(dpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
								}
							}else if(rpixel != 0 && dpixel == 0){
								if(rpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
								}
							}else{
								if(dpixel < rpixel){
									if(dpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}else{
									if(rpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}
								}
							}
						}
					}else if(i==0 && j!=thimage.cols-1 && j!=0){//edge up
						lpixel = imgarray.at<int>(i,j-1);
						rpixel = imgarray.at<int>(i,j+1);
						dpixel = imgarray.at<int>(i+1,j);
						if(pixel == 0){

						}else{
							if(lpixel==rpixel==dpixel==0){

							}else if(lpixel==0 && rpixel!=0 && dpixel!=0){
								if(dpixel < rpixel){
									if(dpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}else{
									if(rpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}
								}
							}else if(lpixel!=0 && rpixel==0 && dpixel!=0){
								if(dpixel < lpixel){
									if(dpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}else{
									if(lpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}
								}
							}else if(lpixel!=0 && rpixel!=0 && dpixel==0){
								if(rpixel < lpixel){
									if(rpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}
								}else{
									if(lpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}
								}
							}else if(lpixel==0 && rpixel==0 && dpixel!=0){
								if(dpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
								}
							}else if(lpixel==0 && rpixel!=0 && dpixel==0){
								if(rpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
								}
							}else if(lpixel!=0 && rpixel==0 && dpixel==0){
								if(lpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
								}
							}else{
								if(lpixel<rpixel){
									if(lpixel<dpixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}else{
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}else{
									if(rpixel<dpixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}else{
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}
							}
						}
					}else if(i==thimage.rows-1 && j!=thimage.cols-1 && j!=0){//edge down
						lpixel = imgarray.at<int>(i,j-1);
						rpixel = imgarray.at<int>(i,j+1);
						upixel = imgarray.at<int>(i-1,j);
						if(pixel == 0){

						}else{
							if(lpixel==rpixel==upixel==0){

							}else if(lpixel==0 && rpixel!=0 && upixel!=0){
								if(upixel < rpixel){
									if(upixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(rpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}
								}
							}else if(lpixel!=0 && rpixel==0 && upixel!=0){
								if(upixel < lpixel){
									if(upixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(lpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}
								}
							}else if(lpixel!=0 && rpixel!=0 && upixel==0){
								if(rpixel < lpixel){
									if(rpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}
								}else{
									if(lpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}
								}
							}else if(lpixel==0 && rpixel==0 && upixel!=0){
								if(upixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
								}
							}else if(lpixel==0 && rpixel!=0 && upixel==0){
								if(rpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
								}
							}else if(lpixel!=0 && rpixel==0 && upixel==0){
								if(lpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
								}
							}else{
								if(lpixel<rpixel){
									if(lpixel<upixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}else{
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(rpixel<upixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}else{
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}
							}
						}
					}else if(i!=0 && i!=thimage.rows-1 && j==0){//edge left
						upixel = imgarray.at<int>(i-1,j);
						rpixel = imgarray.at<int>(i,j+1);
						dpixel = imgarray.at<int>(i+1,j);
						if(pixel == 0){

						}else{
							if(dpixel==rpixel==upixel==0){

							}else if(dpixel==0 && rpixel!=0 && upixel!=0){
								if(upixel < rpixel){
									if(upixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(rpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}
								}
							}else if(dpixel!=0 && rpixel==0 && upixel!=0){
								if(upixel < dpixel){
									if(upixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(dpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}
							}else if(dpixel!=0 && rpixel!=0 && upixel==0){
								if(rpixel < dpixel){
									if(rpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}
								}else{
									if(dpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}
							}else if(dpixel==0 && rpixel==0 && upixel!=0){
								if(upixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
								}
							}else if(dpixel==0 && rpixel!=0 && upixel==0){
								if(rpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
								}
							}else if(dpixel!=0 && rpixel==0 && upixel==0){
								if(dpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
								}
							}else{
								if(dpixel<rpixel){
									if(dpixel<upixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}else{
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(rpixel<upixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}else{
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}
							}
						}
					}else if(i!=0 && i!=thimage.rows-1 && j==thimage.cols-1){//edge right
						upixel = imgarray.at<int>(i-1,j);
						dpixel = imgarray.at<int>(i+1,j);
						lpixel = imgarray.at<int>(i,j-1);
						if(pixel == 0){

						}else{
							if(dpixel==lpixel==upixel==0){

							}else if(dpixel==0 && lpixel!=0 && upixel!=0){
								if(upixel < lpixel){
									if(upixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(lpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}
								}
							}else if(dpixel!=0 && lpixel==0 && upixel!=0){
								if(upixel < dpixel){
									if(upixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(dpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}
							}else if(dpixel!=0 && lpixel!=0 && upixel==0){
								if(lpixel < dpixel){
									if(lpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}
								}else{
									if(dpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}
							}else if(dpixel==0 && lpixel==0 && upixel!=0){
								if(upixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
								}
							}else if(dpixel==0 && lpixel!=0 && upixel==0){
								if(lpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
								}
							}else if(dpixel!=0 && lpixel==0 && upixel==0){
								if(dpixel < pixel){
									imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
								}
							}else{
								if(dpixel<lpixel){
									if(dpixel<upixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}else{
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else{
									if(lpixel<upixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}else{
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}
							}
						}
					}else{//not edge or corner
						upixel = imgarray.at<int>(i-1,j);
						dpixel = imgarray.at<int>(i+1,j);
						lpixel = imgarray.at<int>(i,j-1);
						rpixel = imgarray.at<int>(i,j+1);
						if(pixel == 0){

						}else{
							if(upixel == 0){//the same as edge up
								if(lpixel==rpixel==dpixel==0){

								}else if(lpixel==0 && rpixel!=0 && dpixel!=0){
									if(dpixel < rpixel){
										if(dpixel < pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
										}
									}else{
										if(rpixel < pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
										}
									}
								}else if(lpixel!=0 && rpixel==0 && dpixel!=0){
									if(dpixel < lpixel){
										if(dpixel < pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
										}
									}else{
										if(lpixel < pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
										}
									}
								}else if(lpixel!=0 && rpixel!=0 && dpixel==0){
									if(rpixel < lpixel){
										if(rpixel < pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
										}
									}else{
										if(lpixel < pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
										}
									}
								}else if(lpixel==0 && rpixel==0 && dpixel!=0){
									if(dpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
									}
								}else if(lpixel==0 && rpixel!=0 && dpixel==0){
									if(rpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
									}
								}else if(lpixel!=0 && rpixel==0 && dpixel==0){
									if(lpixel < pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
									}
								}else{
									if(lpixel<rpixel){
										if(lpixel<dpixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
										}else{
											imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
										}
									}else{
										if(rpixel<dpixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
										}else{
											imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
										}
									}
								}
							}else{//upixel!=0  everyone check with upixel
								if(dpixel==lpixel==rpixel==0){
									if(upixel<pixel){
										imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
									}
								}else if(dpixel==0 && lpixel!=0 && rpixel!=0){
									if(lpixel<rpixel){
										if(lpixel<upixel){
											if(lpixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
											}
										}else{
											if(upixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
											}
										}
									}else{
										if(rpixel<upixel){
											if(rpixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
											}
										}else{
											if(upixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
											}
										}
									}
								}else if(dpixel!=0 && lpixel==0 && rpixel !=0){
									if(dpixel<rpixel){
										if(dpixel<upixel){
											if(dpixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
											}
										}else{
											if(upixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
											}
										}
									}else{
										if(rpixel<upixel){
											if(rpixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
											}
										}else{
											if(upixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
											}
										}
									}
								}else if(dpixel!=0 && lpixel!=0 && rpixel ==0){
									if(dpixel<lpixel){
										if(dpixel<upixel){
											if(dpixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
											}
										}else{
											if(upixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
											}
										}
									}else{
										if(lpixel<upixel){
											if(lpixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
											}
										}else{
											if(upixel<pixel){
												imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
											}
										}
									}
								}else if(dpixel==0 && lpixel==0 && rpixel !=0){
									if(rpixel<upixel){
										if(rpixel<pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
										}
									}else{
										if(upixel<pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
										}
									}
								}else if(dpixel==0 && lpixel!=0 && rpixel ==0){
									if(lpixel<upixel){
										if(lpixel<pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
										}
									}else{
										if(upixel<pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
										}
									}
								}else if(dpixel!=0 && lpixel==0 && rpixel ==0){
									if(dpixel<upixel){
										if(dpixel<pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
										}
									}else{
										if(upixel<pixel){
											imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
										}
									}
								}else{//always non equal to zero
									if(upixel<dpixel){
										if(upixel<lpixel){
											if(upixel<rpixel){
												if(upixel<pixel){
													imgarray.at<int>(i,j) = imgarray.at<int>(i-1,j) ;
												}
											}else{
												if(rpixel<pixel){
													imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
												}
											}
										}else{
											if(lpixel<rpixel){
												if(lpixel<pixel){
													imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
												}
											}else{
												if(rpixel<pixel){
													imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
												}
											}
										}
									}else{
										if(dpixel<lpixel){
											if(dpixel<rpixel){
												if(dpixel<pixel){
													imgarray.at<int>(i,j) = imgarray.at<int>(i+1,j) ;
												}
											}else{
												if(rpixel<pixel){
													imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
												}
											}
										}else{
											if(lpixel<rpixel){
												if(lpixel<pixel){
													imgarray.at<int>(i,j) = imgarray.at<int>(i,j-1) ;
												}
											}else{
												if(rpixel<pixel){
													imgarray.at<int>(i,j) = imgarray.at<int>(i,j+1) ;
												}
											}
										}
									}
								}
							}
						}
					}
					
				}//end j
			}//end i

		}//end if(repeat)
		
	}//end while
	
	//initial
	int count [10000];
	for(int i=0; i<10000; i++){
		count[i] = 0;
	}

	//find pixel > 500
	for(int i=0; i<thimage.rows; i++){
		for(int j=0; j<thimage.cols; j++){
			pixel = imgarray.at<int>(i,j);
			if(pixel != 0){
				count[pixel]++ ;
			}
		}
	}
	int find[100];
	for(int i=0; i<100 ;i++){
		find[i] = 0;
	}
	int c = 0;
	for(int i=0; i<10000; i++){
		if(count[i] > 500){
			find[c] = i;
			c++;
			// find contain which label has pixel > 500
		}
	}
	for(int i=0; i<100 ;i++){
		if(find[i]!=0){
			cout<<find[i]<<endl;
		}
	}

	//find the point to draw
	int up[20];
	int down[20];
	int left[20];
	int right[20];

	for(int i=0; i<20; i++){
		up[i] = 1000;
		down[i] = -1;
		left[i] = 1000;
		right[i] = -1;
	}

	for(int i=0; i<thimage.rows; i++){
		for(int j=0; j<thimage.cols; j++){
			pixel = imgarray.at<int>(i,j);
			for(int k=0; k<c; k++){
				if(pixel == find[k]){//find the label
					if(i<up[k]){
						up[k] = i;
					}
					if(i>down[k]){
						down[k] = i;
					}
					if(j<left[k]){
						left[k] = j;
					}
					if(j>right[k]){
						right[k] = j;
					}
				}
			}
		}
	}

	for(int i=0; i<c; i++){
		rectangle(connectimg,Point(left[i],up[i]),Point(right[i],down[i]),Scalar( 0, 0, 255 ),1,8);
	}

	imshow("connect",connectimg);
	waitKey(0);
	return 0;
}