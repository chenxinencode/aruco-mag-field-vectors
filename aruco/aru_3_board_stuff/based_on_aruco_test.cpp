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
 
 
 //from PhilLab response on http://stackoverflow.com/questions/10161351/opencv-how-to-plot-velocity-vectors-as-arrows-in-using-single-static-image
// I changed the three occurances of "line" to "cv::line"
// and the "InputOutputArray img" to "cv::Mat &img"
static void arrowedLine(cv::Mat &img, //InputOutputArray img, 
Point pt1, Point pt2, const Scalar& color,
int thickness=1, int line_type=8, int shift=0, double tipLength=0.1)
{
    const double tipSize = norm(pt1-pt2)*tipLength; // Factor to normalize the size of the tip depending on the length of the arrow
    cv::line(img, pt1, pt2, color, thickness, line_type, shift);
    const double angle = atan2( (double) pt1.y - pt2.y, (double) pt1.x - pt2.x );
    Point p(cvRound(pt2.x + tipSize * cos(angle + CV_PI / 4)),
    cvRound(pt2.y + tipSize * sin(angle + CV_PI / 4)));
    cv::line(img, p, pt2, color, thickness, line_type, shift);
    p.x = cvRound(pt2.x + tipSize * cos(angle - CV_PI / 4));
    p.y = cvRound(pt2.y + tipSize * sin(angle - CV_PI / 4));
    cv::line(img, p, pt2, color, thickness, line_type, shift);
}
 
void drawVecAtPosOLDJUSTFORMARKERS(cv::Mat &Image,Marker &m,const CameraParameters &CP,cv::Mat &locationData)
{

    float size=m.ssize*1;
    Mat objectPoints (2,3,CV_32FC1);
    
    
    
    //location of tail (x y z)
    objectPoints.at<float>(0,0)=size*locationData.at<float>(0,0); 
    objectPoints.at<float>(0,1)=size*locationData.at<float>(0,1);
    objectPoints.at<float>(0,2)=size*locationData.at<float>(0,2);
    
    
    // to make these lengths, must offset position of tail
    objectPoints.at<float>(1,0)=objectPoints.at<float>(0,0) + size*locationData.at<float>(0,3); 
    objectPoints.at<float>(1,1)=objectPoints.at<float>(0,1) + size*locationData.at<float>(0,4); 
    objectPoints.at<float>(1,2)=objectPoints.at<float>(0,2) + size*locationData.at<float>(0,5); 

    vector<Point2f> imagePoints;
    cv::projectPoints( objectPoints, m.Rvec,m.Tvec, CP.CameraMatrix,CP.Distorsion,   imagePoints);

// note syntax...
// void line(Mat& img, Point pt1, Point pt2, const Scalar& color,        int thickness=1, int lineType=8,  int shift=0)
// void arrowedLine(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int line_type=8, int shift=0, double tipLength=0.1)

    arrowedLine(Image,imagePoints[0],imagePoints[1],Scalar(255,255,255,255),1,CV_AA);
    putText(Image,"awesome", imagePoints[1],FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,255,255,255),2);
}


void drawVecAtPos(cv::Mat &Image,Board &B,const CameraParameters &CP, cv::Mat &locationData, string toWrite)
{
	
	float size=1; // B[0].ssize*1;
	
    Mat objectPoints (2,3,CV_32FC1);
    
    
    
    //location of tail (x y z)
    objectPoints.at<float>(0,0)=size*locationData.at<float>(0,0); 
    objectPoints.at<float>(0,1)=size*locationData.at<float>(0,1);
    objectPoints.at<float>(0,2)=size*locationData.at<float>(0,2);
    
    
    // to make these lengths, must offset position of tail
    objectPoints.at<float>(1,0)=objectPoints.at<float>(0,0) + size*locationData.at<float>(0,3); 
    objectPoints.at<float>(1,1)=objectPoints.at<float>(0,1) + size*locationData.at<float>(0,4); 
    objectPoints.at<float>(1,2)=objectPoints.at<float>(0,2) + size*locationData.at<float>(0,5); 

    vector<Point2f> imagePoints;
    //cv::projectPoints( objectPoints, m.Rvec,m.Tvec, CP.CameraMatrix,CP.Distorsion,   imagePoints);
    projectPoints( objectPoints, B.Rvec,B.Tvec, CP.CameraMatrix, CP.Distorsion,   imagePoints);

// note syntax...
// void line(Mat& img, Point pt1, Point pt2, const Scalar& color,        int thickness=1, int lineType=8,  int shift=0)
// void arrowedLine(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int line_type=8, int shift=0, double tipLength=0.1)

    arrowedLine(Image,imagePoints[0],imagePoints[1],Scalar(255,255,255,255),1,CV_AA);
    putText(Image,toWrite, imagePoints[1],FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,255,255,255),2);
	
}

void drawVecsAtPosTesting(cv::Mat &Image,Board &B,const CameraParameters &CP,
	cv::Mat &locationData, Mat &R33forLab, Mat &R33forSensorSideNumber1, Board &BProbe)
{
Mat SensorTvec = R33forLab.inv()*(BProbe.Tvec - B.Tvec);

//Mat SensorTvec = TheBoardDetectorLab.getDetectedBoard().Tvec; //MORNING!// this needs to come from a hand-held "sensor" (Probably a BIG 2 by 2 board.)
	
	//cerr << SensorTvec <<endl;
	//cerr << SensorTvec.at<float>(0,2) <<endl;
  //cout << locationData.rows<<endl;
  float size=1;//m.ssize*1;
    Mat objectPoints (2,3,CV_32FC1);
    for (unsigned int vecnum=0; vecnum < locationData.rows; vecnum++) {
		//cout << locationData.at<float>(0,0) << endl;
		//cout << locationData.at<float>(0,1) << endl;
		//cout << locationData.at<float>(0,2) << endl;
		//cout << locationData.at<float>(1,0) << endl;
		//cout << locationData.at<float>(1,1) << endl;
		//cout << locationData.at<float>(1,2) << endl;
		//cout <<locationData<<"and vecnum"<<vecnum<<endl;
		
		//cout << endl<< endl<< endl<< endl<< endl<< endl;
		
		//location of tail (x y z)
		objectPoints.at<float>(0,0)=size*locationData.at<float>(vecnum,0); 
		objectPoints.at<float>(0,1)=size*locationData.at<float>(vecnum,1);
		objectPoints.at<float>(0,2)=size*locationData.at<float>(vecnum,2);
		
		
		float senx = SensorTvec.at<float>(0,0);
		float seny = SensorTvec.at<float>(0,1);
		float senz = SensorTvec.at<float>(0,2);
		
		float vecx = objectPoints.at<float>(0,0);
		float vecy = objectPoints.at<float>(0,1);
		float vecz = objectPoints.at<float>(0,2);
		
		
		// to make these lengths, must offset position of tail
		objectPoints.at<float>(1,0)=objectPoints.at<float>(0,0) + size*locationData.at<float>(vecnum,3); 
		objectPoints.at<float>(1,1)=objectPoints.at<float>(0,1) + size*locationData.at<float>(vecnum,4); 
		objectPoints.at<float>(1,2)=objectPoints.at<float>(0,2) + size*locationData.at<float>(vecnum,5); 

		vector<Point2f> imagePoints;
		cv::projectPoints( objectPoints, B.Rvec,B.Tvec, CP.CameraMatrix,CP.Distorsion,   imagePoints);

	// note syntax...
	// void line(Mat& img, Point pt1, Point pt2, const Scalar& color,        int thickness=1, int lineType=8,  int shift=0)
	// void arrowedLine(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int line_type=8, int shift=0, double tipLength=0.1)
		//cout <<100.0*objectPoints.at<float>(0,2)<<endl;
		int aThing = (int)100.0*objectPoints.at<float>(0,2);
		//cout <<"athing"<<aThing<<endl;
		
		
		if (sqrt(   (vecx - senx)*(vecx - senx) +
					(vecy - seny)*(vecy - seny) +
					(vecz - senz)*(vecz - senz) 
					)
					< 0.10){
			arrowedLine(Image,imagePoints[0],imagePoints[1],Scalar(0,0,255,255),1,//abs(aThing), //normally 1 for thickness
				CV_AA);
			}
			else
			{
				arrowedLine(Image,imagePoints[0],imagePoints[1],Scalar(255,0,0,255),1,//abs(aThing), //normally 1 for thickness
				CV_AA);
			}
			
			//MORNING//
			//BProbe.Tvec 
			
			//fillConvexPoly(Image,,4,Scalar(255,255,255,255)
		
		
		
		
		//putText(Image,"awesome", imagePoints[1],FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,255,255,255),2);
	}
}

bool readArguments ( int argc,char **argv )
{



    if (argc<5) {

        //cerr<<"Invalid number of arguments"<<endl;
        cerr<<"Usage: (in.avi|live) boardConfig.yml [intrinsics.yml] [size] [out]"<<endl;
        //return false;

        cerr << "Note from Jaime: using defaults" <<endl;
        string homeFolder = argv[1];

        string inputparamFileName = argv[2];

        FileStorage fsinputparams( inputparamFileName, FileStorage::READ);

        fsinputparams["TheInputVideo"] >> TheInputVideo;

        fsinputparams["TheBoardConfigFile"] >> TheBoardConfigFile;
        TheBoardConfigFile = homeFolder + TheBoardConfigFile;

        fsinputparams["TheBoardConfigFile2"] >> TheBoardConfigFile2;
        TheBoardConfigFile2 = homeFolder + TheBoardConfigFile2;

        fsinputparams["TheBoardConfigFileLab"] >> TheBoardConfigFileLab;
        TheBoardConfigFileLab = homeFolder + TheBoardConfigFileLab;

        fsinputparams["TheIntrinsicFile"] >> TheIntrinsicFile;
        TheIntrinsicFile = homeFolder + TheIntrinsicFile;

        fsinputparams["recording"] >> recording;

        fsinputparams["TheMarkerSize1"] >> TheMarkerSize1;

        fsinputparams["TheMarkerSizeLab"] >> TheMarkerSizeLab;

        cerr<<TheMarkerSizeLab<<endl;

        fsinputparams.release();

        return true;
    }
    else
    {




        TheInputVideo=argv[1];
        TheBoardConfigFile=argv[2];
        TheBoardConfigFile2=argv[3];
        TheBoardConfigFileLab=argv[4];
        if (argc>=6)
            TheIntrinsicFile=argv[5];
        //if (argc>=7)
            //useArduino=argv[6];
            if (atoi(argv[6]) == 1)
            {
                recording = true;

            }
            else if(atoi(argv[6]) == 0)
            {
                recording = false;
            }
            else
            {
                cerr<<"Neither recording nor visualizing...???"<<endl;
                return false;
            }




        if (argc>=8)
            TheMarkerSize1=atof(argv[7]);
        if (argc>=9)
            TheMarkerSizeLab=atof(argv[8]);

        //if (argc>=7)
        //    TheOutVideoFilePath=argv[6];


    //    if (argc==5)
    //        cerr<<"NOTE: You need makersize to see 3d info!!!!"<<endl;

        return true;
    }




}

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
        while ( key!=27 && TheVideoCapturer.grab())
        {
            TheVideoCapturer.retrieve( TheInputImage);
            TheInputImage.copyTo(TheInputImageCopy);
            index++; //number of images captured
            double tick = (double)getTickCount();//for checking the speed
            //Detection of the board
            float probDetect=TheBoardDetector.detect(TheInputImage);
            //TRYING TO SWITCH TO MARKERS//float probDetect2=TheBoardDetector2.detect(TheInputImage);
            float probDetectLab=TheBoardDetectorLab.detect(TheInputImage);
            //chekc the speed by calculating the mean speed of all iterations
            AvrgTime.first+=((double)getTickCount()-tick)/getTickFrequency();
            AvrgTime.second++;
            //uncomment to see detection time
            //cout<<"Time detection="<<1000*AvrgTime.first/AvrgTime.second<<" milliseconds"<<endl;
            
            //print marker borders
            //for (unsigned int i=0;i<TheBoardDetector.getDetectedMarkers().size();i++)
            //    TheBoardDetector.getDetectedMarkers()[i].draw(TheInputImageCopy,Scalar(0,0,255),1);

            //print board
            if (TheCameraParameters.isValid()) {
				
                if ( probDetect>0.2)   {
					cerr << "Board 1 in sight!" <<endl;
					//Mat oneVect = (Mat_<float>(6,1) << atof(argv[6]), atof(argv[7]), atof(argv[8]), atof(argv[9]), atof(argv[10]), atof(argv[11]) ); // Column vector
                    //CvDrawingUtils::draw3dAxis( TheInputImageCopy,TheBoardDetector.getDetectedBoard(),TheCameraParameters);
                    //drawVecAtPos(TheInputImageCopy,TheBoardDetector.getDetectedBoard(),TheCameraParameters,oneVect,"awesome");
                    //draw3dAxisBoardj(TheInputImageCopy,TheBoardDetector.getDetectedBoard(),TheCameraParameters);
                    //draw3dBoardCube( TheInputImageCopy,TheBoardDetected,TheIntriscCameraMatrix,TheDistorsionCameraParams);
                }
                else
                
                
                {if (recording == true) {cerr << "Can't see board 1.\n";}}
                
					

                //TRYING TO SWITCH TO MARKERS//if ( probDetect2>0.2)   {
					//TRYING TO SWITCH TO MARKERS//cerr << "Board 2 in sight!" <<endl;
                    //CvDrawingUtils::draw3dAxis( TheInputImageCopy,TheBoardDetector2.getDetectedBoard(),TheCameraParameters);
                    //draw3dAxisBoardj(TheInputImageCopy,TheBoardDetector2.getDetectedBoard(),TheCameraParameters);
                    //draw3dBoardCube( TheInputImageCopy,TheBoardDetected,TheIntriscCameraMatrix,TheDistorsionCameraParams);
                //TRYING TO SWITCH TO MARKERS//}
                //TRYING TO SWITCH TO MARKERS//else
                //TRYING TO SWITCH TO MARKERS//{
					//cerr << "Can't see board 2.\n";
				//TRYING TO SWITCH TO MARKERS//}
				
				
				
                if ( probDetectLab>0.2)   {  // If we detected the lab frame board 
					CvDrawingUtils::draw3dAxis( TheInputImageCopy,TheBoardDetectorLab.getDetectedBoard(),TheCameraParameters);
				}
				else
				{cerr << "Can't see board Lab.\n";}
				
				
				
				
				
				
				
				
				
				
				
				if(recording ==false && probDetectLab>0.2){
					Mat R33forLab;
					cv::Rodrigues(TheBoardDetectorLab.getDetectedBoard().Rvec,R33forLab); 

					Mat R33forSensorSideNumber1; //MORNING!// this needs to be the probe, not sensor side 1
					cv::Rodrigues(TheBoardDetector.getDetectedBoard().Rvec,R33forSensorSideNumber1);

					drawVecsAtPosTesting(TheInputImageCopy,TheBoardDetectorLab.getDetectedBoard(),TheCameraParameters,someVects,R33forLab,R33forSensorSideNumber1, TheBoardDetector.getDetectedBoard());
				}
				
				
				
				
				
				
				
				if ( probDetectLab>0.2  && probDetect>0.2 
							&& recording == true) // detected lab board and board1
				{
					Mat R33forLab;
					
					cv::Rodrigues(TheBoardDetectorLab.getDetectedBoard().Rvec,R33forLab); 
					// takes the 1 by 3 and stores it as a 3 by 3 in R33forLab
					
					//cout << "R33 for lab" << endl << R33forLab << endl;
					
										
					Mat R33forSensorSideNumber1;
					cv::Rodrigues(TheBoardDetector.getDetectedBoard().Rvec,R33forSensorSideNumber1);
					//cout << R33forSensorSideNumber1 << endl;
					
					string inFromArduino;
					if(arduino.isOpen())
					{
					
						inFromArduino = arduino.read();
						arduino.flush();
						
						
					}
					
					
					
					
					//cout << inFromArduino;
					
					Mat processedInFromArduino = (Mat_<float>(3,1) << 1024, 1024, 1024); // Column vector
					
					
					
					
					
					
					// as per http://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
					// SCREW YOU C++ FOR STUPID STRING MANAGEMENT >_<
					// but thank you internet for solutions
				   
					//std::string s = inFromArduino; // "scott>=tiger>=mushroom";
					//inFromArduino = "scott,tiger,mushroom";
					
					try
					{
						std::string delimiter = ","; //">=";
						
						size_t pos = 0;
						int stupidcounterforarduinostuff = 0;
						std::string token;
						while ((pos = inFromArduino.find(delimiter)) != std::string::npos) {
							token = inFromArduino.substr(0, pos);
							
							//std::cout << "token" << (stof(token)/10)<<endl;
							
							//cout << "Before "<< processedInFromArduino.at<float>(stupidcounterforarduinostuff,0)<<endl; // stupidcounterforarduinostuff,0) <<endl;
							
							
							processedInFromArduino.at<float>(stupidcounterforarduinostuff,0) = stof(token);
							
							
							//cout << "after " << processedInFromArduino.at<float>(stupidcounterforarduinostuff,0) << endl;
							
							stupidcounterforarduinostuff++;
							
							inFromArduino.erase(0, pos + delimiter.length());
						}
						processedInFromArduino.at<float>(stupidcounterforarduinostuff,0) = stof(inFromArduino);
						//std::cout << inFromArduino << std::endl;
						
						
						
						cerr<<"THIS"<<endl<<processedInFromArduino<<endl<<endl;
						Mat processedInFromArduinoCOPY = (processedInFromArduino*1.0) - 512.0;
						cerr<<"minus qui COPY"<<endl<<processedInFromArduino<<endl<<endl;
						processedInFromArduinoCOPY = scaleFactorForVectors*processedInFromArduinoCOPY;
						cerr<<"THIS"<<endl<<processedInFromArduinoCOPY<<endl<<endl;
						
						
						
						
						if (badflag == false){
						if (!(processedInFromArduino.at<float>(0,0) == 1024 || // if it does NOT still have ANY of its initial values
						     processedInFromArduino.at<float>(1,0) == 1024 ||
						       processedInFromArduino.at<float>(2,0) == 1024))
						       {
						
						//cout<<"THIS"<<endl<<processedInFromArduino<<endl<<endl;
						processedInFromArduino = (processedInFromArduino*1.0) - 512.0;
						//cout<<"minus qui"<<endl<<processedInFromArduino<<endl<<endl;
						processedInFromArduino = scaleFactorForVectors*processedInFromArduino;
						//cout<<"THIS"<<endl<<processedInFromArduino<<endl<<endl;
					
						Mat deriv = processedInFromArduinoLastTime - processedInFromArduino;
						if (fabs(deriv.at<float>(0,0)) < 0.05 && // if deriv is small
						     fabs(deriv.at<float>(1,0)) < 0.05 &&
						       fabs(deriv.at<float>(2,0)) < 0.05)
						       {
						
					
					
					
					Mat afterDouble; 
					afterDouble = R33forLab.inv() * (R33forSensorSideNumber1 * processedInFromArduino); //inversion method
					//WORKS!!!
					//YAY!!!
					
					
					//
					//Changed this morning and works
					//HARD CODED and works
					Mat initPositionThing = TheBoardDetector.getDetectedBoard().Tvec;
					Mat offsetForThing = (Mat_<float>(3,1) << 0, 0.05, 0.015); // Column vector
					initPositionThing = initPositionThing - (R33forSensorSideNumber1*offsetForThing); // changed this morning. plus what?
					
					Mat translationStuff = R33forLab.inv()*(initPositionThing - TheBoardDetectorLab.getDetectedBoard().Tvec);
					
					//cout << endl << endl << "Translation stuff" << translationStuff << endl << endl;
					
					//ACTUALLY PUT THIS IN HERE ----afterDouble
					Mat toShow = (Mat_<float>(1,6) <<   // First, X Y Z location
					translationStuff.at<float>(0,0), 
					 translationStuff.at<float>(0,1), 
					  translationStuff.at<float>(0,2),
					afterDouble.at<float>(0,0),
					 afterDouble.at<float>(0,1),
					  afterDouble.at<float>(0,2)); // Column vector for Bx By Bz
					
					cout <<
					 toShow.at<float>(0,0) << "," <<
					 toShow.at<float>(0,1) << "," <<
					 toShow.at<float>(0,2) << "," <<
					 toShow.at<float>(0,3) << "," <<
					 toShow.at<float>(0,4) << "," <<
					 toShow.at<float>(0,5)
					 <<endl;
					
					drawVecAtPos(TheInputImageCopy,TheBoardDetectorLab.getDetectedBoard(),TheCameraParameters,toShow, "recording data");
					} // done with if deriv small
					processedInFromArduinoLastTime = processedInFromArduino;
						
					}//done with "if not 1024
					else
					{
						cerr << "setting badflag true" <<endl;
						badflag = true;
					}
				}// done with if badflag == false
				else //so if it is not false, i.e. true, then make it false
				{
					badflag = false;
				}
				
					}
					catch (...)
					{
						cerr << "Recording error" <<endl;
						Mat toShow = (Mat_<float>(1,6) << 0,0,0 ,0,0,0);
						drawVecAtPos(TheInputImageCopy,TheBoardDetectorLab.getDetectedBoard(),TheCameraParameters,toShow, "ERROR");
					}
					
				}
				//END IF DETECTED LAB BOARD AND BOARD 1
				
				
					
			} // end if cam param valid
					
		
            
            
				
				
        
            //DONE! Easy, right?

            //show input with augmented information and  the thresholded image
            cv::imshow("in",TheInputImageCopy);
            cv::imshow("thres",TheBoardDetector.getMarkerDetector().getThresholdedImage());
            
            // Note: do we care?
            //cv::imshow("thres",TheBoardDetector.getMarkerDetector().getThresholdedImage());
            
            //write to video if required
            if (  TheOutVideoFilePath!="") {
                //create a beautiful compiosed image showing the thresholded
                //first create a small version of the thresholded image
                cv::Mat smallThres;
                cv::resize( TheBoardDetector.getMarkerDetector().getThresholdedImage(),smallThres,cvSize(TheInputImageCopy.cols/3,TheInputImageCopy.rows/3));
                cv::Mat small3C;
                cv::cvtColor(smallThres,small3C,CV_GRAY2BGR);
                cv::Mat roi=TheInputImageCopy(cv::Rect(0,0,TheInputImageCopy.cols/3,TheInputImageCopy.rows/3));
                small3C.copyTo(roi);
                VWriter<<TheInputImageCopy;
// 			 cv::imshow("TheInputImageCopy",TheInputImageCopy);

            }
            
            
            
            
            
            





			//std::this_thread::sleep_for (std::chrono::seconds(1));



            key=cv::waitKey(waitTime);//wait for key to be pressed
            processKey(key);
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



