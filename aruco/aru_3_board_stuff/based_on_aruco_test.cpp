/*****************************
Copyright 2011 Rafael Muñoz Salinas. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Rafael Muñoz Salinas ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Rafael Muñoz Salinas OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Rafael Muñoz Salinas.
********************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>


#include "./cArduinoSOURCE/cArduino.cpp"



#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

using namespace cv;
using namespace aruco;

string TheInputVideo;
string TheIntrinsicFile;
string TheBoardConfigFile;
string TheBoardConfigFile2;
string TheBoardConfigFileLab;
bool The3DInfoAvailable=false;
float TheMarkerSize1=-1;
float TheMarkerSize2=-1;
float TheMarkerSizeLab=-1;
float TheMarkerSizeSensor=-1; // HARD CODED 
VideoCapture TheVideoCapturer;
Mat TheInputImage,TheInputImageCopy;
CameraParameters TheCameraParameters;
BoardConfiguration TheBoardConfig;
BoardConfiguration TheBoardConfig2;
BoardConfiguration TheBoardConfigLab;
BoardDetector TheBoardDetector;
BoardDetector TheBoardDetector2;
BoardDetector TheBoardDetectorLab;

float scaleFactorForVectors = 0.001;

string TheOutVideoFilePath;
cv::VideoWriter VWriter;

void cvTackBarEvents(int pos,void*);
pair<double,double> AvrgTime(0,0) ;//determines the average time required for detection
double ThresParam1,ThresParam2;
int iThresParam1,iThresParam2;
int waitTime=0;

bool badflag = false;

Mat processedInFromArduinoLastTime = (Mat_<float>(3,1) << 0, 0, 0);


bool recording = false;


/************************************
 *
 *
 *
 *
 ************************************/
 

#include "testfile1.cpp"

#include "readArguments.cpp"
 
#include "joljdrawingfuncs.cpp"




void processKey(char k) {
    switch (k) {
    case 's':
        if (waitTime==0) waitTime=10;
        else waitTime=0;
        break;

/*    case 'p':
        if (MDetector.getCornerRefinementMethod()==MarkerDetector::SUBPIX)
            MDetector.setCornerRefinementMethod(MarkerDetector::NONE);
        else
            MDetector.setCornerRefinementMethod(MarkerDetector::SUBPIX);
        break;*/
    }
}

/************************************
 *
 *
 *
 *
 ************************************/
int main(int argc,char **argv)
{
	
    testingStuff();
	
	//begin copy-paste from http://stackoverflow.com/questions/11550021/converting-a-mat-file-from-matlab-into-cvmat-matrix-in-opencv
		//Mat oneVect;
		//Mat useVecLat;
		Mat someVects;
		//Mat zeroYzero;

		

		
        string vectorsThatWeReadFile  = "../../intermediate_data_files/vectors_to_show_in_final_step.yml"; // FIX THIS
		//string demoFile = "~/vectors_to_show_in_final_step.yml";

        FileStorage fsvec( vectorsThatWeReadFile, FileStorage::READ);

        fsvec["someVects"] >> someVects;

        fsvec.release(); //close the file
		
		//end copy-paste from http://stackoverflow.com/questions/11550021/converting-a-mat-file-from-matlab-into-cvmat-matrix-in-opencv
		






	
	
	
	
	
	
	
	
	
	
	//Mat hey = (Mat_<float>(3,1) << -42, -43, -44);

	//	cout << fabs(hey.at<float>(0,1)) <<endl;

	cArduino arduino(ArduinoBaundRate::B9600bps);
	
	if(!arduino.isOpen())
	{
		std::cerr<<"can't open arduino"<<endl;

		//return 1;
	}
	else
	{
		//cerr<<"arduino open at "<<arduino.getDeviceName()<<endl;
	}

	
    //try
    //{
        if (  readArguments (argc,argv)==false) return 0;


        if (recording)
            waitTime = 100;
        else
            waitTime = 10;


//parse arguments
        TheBoardConfig.readFromFile(TheBoardConfigFile);
        TheBoardConfig2.readFromFile(TheBoardConfigFile2);
        TheBoardConfigLab.readFromFile(TheBoardConfigFileLab);
        cerr<<TheInputVideo<<endl;
        //read from camera or from  file
        if (TheInputVideo=="live") {
            cerr<<"Got this farr far"<<waitTime<<endl;
			TheVideoCapturer.open(1);
			if (!TheVideoCapturer.isOpened()) {
				cerr << "I suppose you're on the desktop." <<endl;
				TheVideoCapturer.open(0);
			}
            //waitTime=100;
        }
        else if (TheInputVideo=="live0") {
            TheVideoCapturer.open(0);
            //waitTime=100;
        }
        else if (TheInputVideo=="live1") {
            TheVideoCapturer.open(1);
            //waitTime=100;
        }
        
        else TheVideoCapturer.open(TheInputVideo);
        //check video is open
        if (!TheVideoCapturer.isOpened()) {
            cerr<<"Could not open video"<<endl;
            return -1;

        }

        //read first image to get the dimensions
        TheVideoCapturer>>TheInputImage;

        //Open outputvideo
        if ( TheOutVideoFilePath!="")
            VWriter.open(TheOutVideoFilePath,CV_FOURCC('M','J','P','G'),15,TheInputImage.size());

        //read camera parameters if passed
        if (TheIntrinsicFile!="") {
            TheCameraParameters.readFromXMLFile(TheIntrinsicFile);
            TheCameraParameters.resize(TheInputImage.size());
        }

        //Create gui

        cv::namedWindow("thres",1);
        cv::namedWindow("in",1);
	    TheBoardDetector.setParams(TheBoardConfig,TheCameraParameters,TheMarkerSize1);
	    TheBoardDetector.getMarkerDetector().getThresholdParams( ThresParam1,ThresParam2);
	    TheBoardDetector.getMarkerDetector().enableErosion(true);//for chessboards
	    //TRYING TO SWITCH TO MARKERS//TheBoardDetector2.setParams(TheBoardConfig2,TheCameraParameters,TheMarkerSize2);
	    //TRYING TO SWITCH TO MARKERS//TheBoardDetector2.getMarkerDetector().getThresholdParams( ThresParam1,ThresParam2);
	    //TRYING TO SWITCH TO MARKERS//TheBoardDetector2.getMarkerDetector().enableErosion(true);//for chessboards
	    TheBoardDetectorLab.setParams(TheBoardConfigLab,TheCameraParameters,TheMarkerSizeLab);
	    TheBoardDetectorLab.getMarkerDetector().getThresholdParams( ThresParam1,ThresParam2);
	    TheBoardDetectorLab.getMarkerDetector().enableErosion(true);//for chessboards
	    
        iThresParam1=ThresParam1;
        iThresParam2=ThresParam2;
        cv::createTrackbar("ThresParam1", "in",&iThresParam1, 13, cvTackBarEvents);
        cv::createTrackbar("ThresParam2", "in",&iThresParam2, 13, cvTackBarEvents);
        char key=0;
        int index=0;
        //capture until press ESC or until the end of the video
        while ( key!=27 //27 is Escape
                && TheVideoCapturer.grab())
        {
#include "doTheMainLoopStuff.cpp"
        } 
        // END WHILE LOOP


    //} catch (std::exception &ex)

    //{
    //    cout<<"Exception :"<<ex.what()<<endl;
    //}

}
/************************************
 *
 *
 *
 *
 ************************************/

void cvTackBarEvents(int pos,void*)
{
    if (iThresParam1<3) iThresParam1=3;
    if (iThresParam1%2!=1) iThresParam1++;
    if (ThresParam2<1) ThresParam2=1;
    ThresParam1=iThresParam1;
    ThresParam2=iThresParam2;
    TheBoardDetector.getMarkerDetector().setThresholdParams(ThresParam1,ThresParam2);
    //TRYING TO SWITCH TO MARKERS//TheBoardDetector2.getMarkerDetector().setThresholdParams(ThresParam1,ThresParam2);
    TheBoardDetectorLab.getMarkerDetector().setThresholdParams(ThresParam1,ThresParam2);
//recompute
//Detection of the board
    /* WHY IS THIS HERE?
    float probDetect=TheBoardDetector.detect( TheInputImage);
    float probDetect2=TheBoardDetector2.detect( TheInputImage);
    float probDetectLab=TheBoardDetectorLab.detect( TheInputImage);
    if (TheCameraParameters.isValid() && probDetect>0.2)
    {
        aruco::CvDrawingUtils::draw3dAxis(TheInputImageCopy,TheBoardDetector.getDetectedBoard(),TheCameraParameters);
        aruco::CvDrawingUtils::draw3dAxis(TheInputImageCopy,TheBoardDetector2.getDetectedBoard(),TheCameraParameters);
        aruco::CvDrawingUtils::draw3dAxis(TheInputImageCopy,TheBoardDetectorLab.getDetectedBoard(),TheCameraParameters);
	}

    cv::imshow("in",TheInputImageCopy);
    cv::imshow("thres",TheBoardDetector.getMarkerDetector().getThresholdedImage());
    */
}



