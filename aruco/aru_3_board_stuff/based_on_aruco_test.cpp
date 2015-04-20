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
VideoCapture TheVideoCapturer;
Mat TheInputImage,TheInputImageCopy;
CameraParameters TheCameraParameters;
BoardConfiguration TheBoardConfig;
BoardConfiguration TheBoardConfig2;
BoardConfiguration TheBoardConfigLab;
BoardDetector TheBoardDetector;
BoardDetector TheBoardDetector2;
BoardDetector TheBoardDetectorLab;

string TheOutVideoFilePath;
cv::VideoWriter VWriter;

void cvTackBarEvents(int pos,void*);
pair<double,double> AvrgTime(0,0) ;//determines the average time required for detection
double ThresParam1,ThresParam2;
int iThresParam1,iThresParam2;
int waitTime=0;




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
    //cout << locationData.at<float>(0,0) << endl;
    //cout << locationData.at<float>(0,1) << endl;
    //cout << locationData.at<float>(0,2) << endl;
    //cout << locationData.at<float>(1,0) << endl;
    //cout << locationData.at<float>(1,1) << endl;
    //cout << locationData.at<float>(1,2) << endl;
    
    
    //cout << endl<< endl<< endl<< endl<< endl<< endl;
    
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
	
	float size=B[0].ssize*1;
    Mat objectPoints (2,3,CV_32FC1);
    //cout << locationData.at<float>(0,0) << endl;
    //cout << locationData.at<float>(0,1) << endl;
    //cout << locationData.at<float>(0,2) << endl;
    //cout << locationData.at<float>(1,0) << endl;
    //cout << locationData.at<float>(1,1) << endl;
    //cout << locationData.at<float>(1,2) << endl;
    
    
    //cout << endl<< endl<< endl<< endl<< endl<< endl;
    
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

bool readArguments ( int argc,char **argv )
{

    if (argc<3) {
        cerr<<"Invalid number of arguments"<<endl;
        cerr<<"Usage: (in.avi|live) boardConfig.yml [intrinsics.yml] [size] [out]"<<endl;
        return false;
    }
    TheInputVideo=argv[1];
    TheBoardConfigFile=argv[2];
    TheBoardConfigFile2=argv[3];
    TheBoardConfigFileLab=argv[4];
    if (argc>=6)
        TheIntrinsicFile=argv[5];
    if (argc>=7)
        TheMarkerSize1=atof(argv[6]);
    if (argc>=8)
        TheMarkerSizeLab=atof(argv[7]);
    
    //if (argc>=7)
    //    TheOutVideoFilePath=argv[6];


//    if (argc==5)
//        cerr<<"NOTE: You need makersize to see 3d info!!!!"<<endl;

    return true;
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
    try
    {
        if (  readArguments (argc,argv)==false) return 0;
//parse arguments
        TheBoardConfig.readFromFile(TheBoardConfigFile);
        TheBoardConfig2.readFromFile(TheBoardConfigFile2);
        TheBoardConfigLab.readFromFile(TheBoardConfigFileLab);
        //read from camera or from  file
        if (TheInputVideo=="live") {
            TheVideoCapturer.open(0);
            waitTime=10;
        }
        else if (TheInputVideo=="live0") {
            TheVideoCapturer.open(0);
            waitTime=10;
        }
        else if (TheInputVideo=="live1") {
            TheVideoCapturer.open(1);
            waitTime=10;
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
	    TheBoardDetector2.setParams(TheBoardConfig2,TheCameraParameters,TheMarkerSize2);
	    TheBoardDetector2.getMarkerDetector().getThresholdParams( ThresParam1,ThresParam2);
	    TheBoardDetector2.getMarkerDetector().enableErosion(true);//for chessboards
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
            float probDetect2=TheBoardDetector2.detect(TheInputImage);
            float probDetectLab=TheBoardDetectorLab.detect(TheInputImage);
            //chekc the speed by calculating the mean speed of all iterations
            AvrgTime.first+=((double)getTickCount()-tick)/getTickFrequency();
            AvrgTime.second++;
            cout<<"Time detection="<<1000*AvrgTime.first/AvrgTime.second<<" milliseconds"<<endl;
            //print marker borders
            //for (unsigned int i=0;i<TheBoardDetector.getDetectedMarkers().size();i++)
            //    TheBoardDetector.getDetectedMarkers()[i].draw(TheInputImageCopy,Scalar(0,0,255),1);

            //print board
             if (TheCameraParameters.isValid()) {
                if ( probDetect>0.2)   {
					cout << TheBoardDetector.getDetectedBoard().Rvec << endl;
					cout << TheBoardDetector.getDetectedBoard().Tvec << endl;
					//Mat oneVect = (Mat_<float>(6,1) << atof(argv[6]), atof(argv[7]), atof(argv[8]), atof(argv[9]), atof(argv[10]), atof(argv[11]) ); // Column vector
                    CvDrawingUtils::draw3dAxis( TheInputImageCopy,TheBoardDetector.getDetectedBoard(),TheCameraParameters);
                    //drawVecAtPos(TheInputImageCopy,TheBoardDetector.getDetectedBoard(),TheCameraParameters,oneVect,"awesome");
                    //draw3dAxisBoardj(TheInputImageCopy,TheBoardDetector.getDetectedBoard(),TheCameraParameters);
                    //draw3dBoardCube( TheInputImageCopy,TheBoardDetected,TheIntriscCameraMatrix,TheDistorsionCameraParams);
                }
                if ( probDetect2>0.2)   {
					cout << TheBoardDetector2.getDetectedBoard().Rvec << endl;
					cout << TheBoardDetector2.getDetectedBoard().Tvec << endl;
                    CvDrawingUtils::draw3dAxis( TheInputImageCopy,TheBoardDetector2.getDetectedBoard(),TheCameraParameters);
                    //draw3dAxisBoardj(TheInputImageCopy,TheBoardDetector2.getDetectedBoard(),TheCameraParameters);
                    //draw3dBoardCube( TheInputImageCopy,TheBoardDetected,TheIntriscCameraMatrix,TheDistorsionCameraParams);
                }
                if ( probDetectLab>0.2)   {  // If we detected the lab frame board 
					//cout << TheBoardDetectorLab.getDetectedBoard().Rvec << endl;
					cout << "LAB TVEC" << endl<< TheBoardDetectorLab.getDetectedBoard().Tvec << endl<< endl<< endl;
                    CvDrawingUtils::draw3dAxis( TheInputImageCopy,TheBoardDetectorLab.getDetectedBoard(),TheCameraParameters);
                    //draw3dAxisBoardj(TheInputImageCopy,TheBoardDetector2.getDetectedBoard(),TheCameraParameters);
                    //draw3dBoardCube( TheInputImageCopy,TheBoardDetected,TheIntriscCameraMatrix,TheDistorsionCameraParams);
                    
                    
					Mat R33forLab;
					
					cv::Rodrigues(TheBoardDetectorLab.getDetectedBoard().Rvec,R33forLab); 
					// takes the 1 by 3 and stores it as a 3 by 3 in R33forLab
					
					cout << "R33 for lab" << endl << R33forLab << endl;
					
					if ( probDetect>0.2)   { // if we detected the sensor board
						cout << TheBoardDetector.getDetectedBoard().Rvec << endl;
						cout << TheBoardDetector.getDetectedBoard().Tvec << endl;
						
						
						Mat R33forSensorSideNumber1;
						cv::Rodrigues(TheBoardDetector.getDetectedBoard().Rvec,R33forSensorSideNumber1);
						cout << R33forSensorSideNumber1 << endl;
						
						Mat blah = (Mat_<float>(3,1) << 0, 4, 0); // Column vector
						
						
						Mat afterDouble; 
						afterDouble = R33forLab.inv() * (R33forSensorSideNumber1 * blah); //inversion method
						//WORKS!!!
						//YAY!!!
						
						//version 1
						Mat translationStuff = TheBoardDetector.getDetectedBoard().Tvec - TheBoardDetectorLab.getDetectedBoard().Tvec;
						//version 2
						//Mat translationStuff = TheBoardDetector.getDetectedBoard().Tvec - TheBoardDetectorLab.getDetectedBoard().Tvec;
						
						cout << endl << endl << "Translation stuff" << translationStuff << endl << endl;
						
						//ACTUALLY PUT THIS IN HERE ----afterDouble
						Mat toShow = (Mat_<float>(1,6) <<   // First, X Y Z location
						translationStuff.at<float>(0,0), 
						 translationStuff.at<float>(0,1), 
						  translationStuff.at<float>(0,2),
						afterDouble.at<float>(0,0),
						 afterDouble.at<float>(0,1),
						  afterDouble.at<float>(0,2)); // Column vector for Bx By Bz
						
						drawVecAtPos(TheInputImageCopy,TheBoardDetectorLab.getDetectedBoard(),TheCameraParameters,toShow, "testing");
						
						
						if (0 == 1){
							//Mat oneVect = (Mat_<float>(6,1) << atof(argv[6]), atof(argv[7]), atof(argv[8]), atof(argv[9]), atof(argv[10]), atof(argv[11]) ); // Column vector
							CvDrawingUtils::draw3dAxis( TheInputImageCopy,TheBoardDetector.getDetectedBoard(),TheCameraParameters);
							//drawVecAtPos(TheInputImageCopy,TheBoardDetector.getDetectedBoard(),TheCameraParameters,oneVect,"awesome");
						}
						
						//draw3dAxisBoardj(TheInputImageCopy,TheBoardDetector.getDetectedBoard(),TheCameraParameters);
						//draw3dBoardCube( TheInputImageCopy,TheBoardDetected,TheIntriscCameraMatrix,TheDistorsionCameraParams);
					}
				}
					
            }
            
            
            //time date stuff, copied from project2
            //needs to have sub-second timing 
				std::time_t result = std::time(NULL); //nullptr);
				std::cout // << std::asctime(std::localtime(&result))
						  << result; // <<  " seconds since the Epoch\n";
						  
				// fileOutt << "time" << result ; //<< endl;

				cout<<endl; // <<endl<<endl;
				
				
				
				
        
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

            key=cv::waitKey(waitTime);//wait for key to be pressed
            processKey(key);
        }


    } catch (std::exception &ex)

    {
        cout<<"Exception :"<<ex.what()<<endl;
    }

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
    TheBoardDetector2.getMarkerDetector().setThresholdParams(ThresParam1,ThresParam2);
    TheBoardDetectorLab.getMarkerDetector().setThresholdParams(ThresParam1,ThresParam2);
//recompute
//Detection of the board
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
}



