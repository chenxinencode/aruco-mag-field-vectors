/*****************************************************************************************
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
********************************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <aruco/aruco.h>
#include <aruco/cvdrawingutils.h>

// also, for more file io, as per http://stackoverflow.com/questions/11550021/converting-a-mat-file-from-matlab-into-cvmat-matrix-in-opencv
#include <string>
// #include <cv.h>
#include </usr/include/opencv/cv.h>
#include </usr/include/opencv/highgui.h>



#include <ctime>
 
//int main()
//{
//    std::time_t result = std::time(nullptr);
//    std::cout << std::asctime(std::localtime(&result))
//              << result << " seconds since the Epoch\n";
//}


using namespace cv;
using namespace aruco;

string TheInputVideo;
string TheIntrinsicFile;
string TheMatlabDataFile;
float TheMarkerSize=-1;
int ThePyrDownLevel;
MarkerDetector MDetector;
VideoCapture TheVideoCapturer;
vector<Marker> TheMarkers;
Mat TheInputImage,TheInputImageCopy;
CameraParameters TheCameraParameters;
void cvTackBarEvents(int pos,void*);
bool readCameraParameters(string TheIntrinsicFile,CameraParameters &CP,Size size);

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
bool readArguments ( int argc,char **argv )
{
    if (argc<2) {
        cerr<<"Invalid number of arguments"<<endl;
        cerr<<"Usage: (in.avi|live) [intrinsics.yml] [size] [vectorsToPlot.yml]"<<endl;
        return false;
    }
    TheInputVideo=argv[1];
    if (argc>=3)
        TheIntrinsicFile=argv[2];
    if (argc>=4)
        TheMarkerSize=atof(argv[3]);

	if (argc>=5)
		TheMatlabDataFile=argv[4];

    if (argc==3)
        cerr<<"NOTE: You need makersize to see 3d info!!!!"<<endl;
        
    
		
    return true;
}
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

// obviously modeled after CvDrawingUtils::draw3dAxis
void draw3dAxisj(cv::Mat &Image,Marker &m,const CameraParameters &CP)
{

  float size=m.ssize*1;
    Mat objectPoints (4,3,CV_32FC1);
    objectPoints.at<float>(0,0)=0;
    objectPoints.at<float>(0,1)=0;
    objectPoints.at<float>(0,2)=0;
    objectPoints.at<float>(1,0)=size;
    objectPoints.at<float>(1,1)=0;
    objectPoints.at<float>(1,2)=0;
    objectPoints.at<float>(2,0)=0;
    objectPoints.at<float>(2,1)=size;
    objectPoints.at<float>(2,2)=0;
    objectPoints.at<float>(3,0)=0;
    objectPoints.at<float>(3,1)=0;
    objectPoints.at<float>(3,2)=size;

    vector<Point2f> imagePoints;
    cv::projectPoints( objectPoints, m.Rvec,m.Tvec, CP.CameraMatrix,CP.Distorsion,   imagePoints);
//draw lines of different colours

// note syntax...
// void line(Mat& img, Point pt1, Point pt2, const Scalar& color,        int thickness=1, int lineType=8,  int shift=0)
// void arrowedLine(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int line_type=8, int shift=0, double tipLength=0.1)
    
    //cv::line(Image,imagePoints[0],imagePoints[1],Scalar(0,0,255,255),1,CV_AA);
    //cv::line(Image,imagePoints[0],imagePoints[2],Scalar(0,255,0,255),1,CV_AA);
    //cv::line(Image,imagePoints[0],imagePoints[3],Scalar(255,0,0,255),1,CV_AA);
    arrowedLine(Image,imagePoints[0],imagePoints[1],Scalar(0,0,255,255),1,CV_AA);
    arrowedLine(Image,imagePoints[0],imagePoints[2],Scalar(0,255,0,255),1,CV_AA);
    arrowedLine(Image,imagePoints[0],imagePoints[3],Scalar(255,0,0,255),1,CV_AA);
    putText(Image,"x", imagePoints[1],FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,0,255,255),2);
    putText(Image,"y", imagePoints[2],FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0,255,0,255),2);
    putText(Image,"z", imagePoints[3],FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,0,0,255),2);
}

void drawVecAtPos(cv::Mat &Image,Marker &m,const CameraParameters &CP,cv::Mat &locationData)
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

void drawVecsAtPosTesting(cv::Mat &Image,Marker &m,const CameraParameters &CP,cv::Mat &locationData)
{
  cout << locationData.rows<<endl;
  float size=m.ssize*1;
    Mat objectPoints (2,3,CV_32FC1);
    for (unsigned int vecnum=0; vecnum < 2; vecnum++) {
		//cout << locationData.at<float>(0,0) << endl;
		//cout << locationData.at<float>(0,1) << endl;
		//cout << locationData.at<float>(0,2) << endl;
		//cout << locationData.at<float>(1,0) << endl;
		//cout << locationData.at<float>(1,1) << endl;
		//cout << locationData.at<float>(1,2) << endl;
		
		
		//cout << endl<< endl<< endl<< endl<< endl<< endl;
		
		//location of tail (x y z)
		objectPoints.at<float>(0,0)=size*locationData.at<float>(vecnum,0); 
		objectPoints.at<float>(0,1)=size*locationData.at<float>(vecnum,1);
		objectPoints.at<float>(0,2)=size*locationData.at<float>(vecnum,2);
		
		
		// to make these lengths, must offset position of tail
		objectPoints.at<float>(1,0)=objectPoints.at<float>(0,0) + size*locationData.at<float>(vecnum,3); 
		objectPoints.at<float>(1,1)=objectPoints.at<float>(0,1) + size*locationData.at<float>(vecnum,4); 
		objectPoints.at<float>(1,2)=objectPoints.at<float>(0,2) + size*locationData.at<float>(vecnum,5); 

		vector<Point2f> imagePoints;
		cv::projectPoints( objectPoints, m.Rvec,m.Tvec, CP.CameraMatrix,CP.Distorsion,   imagePoints);

	// note syntax...
	// void line(Mat& img, Point pt1, Point pt2, const Scalar& color,        int thickness=1, int lineType=8,  int shift=0)
	// void arrowedLine(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int line_type=8, int shift=0, double tipLength=0.1)

		arrowedLine(Image,imagePoints[0],imagePoints[1],Scalar(255,255,255,255),1,CV_AA);
		putText(Image,"awesome", imagePoints[1],FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255,255,255,255),2);
	}
}

//						someVects.at<float>(vecnum,0);
//						someVects.at<float>(i,1);
//						someVects.at<float>(i,2);
//						someVects.at<float>(i,3);
//						someVects.at<float>(i,4);
//						someVects.at<float>(i,5);
//					}

 
int main(int argc,char **argv)
{
	
    
    try
    {
        if (readArguments (argc,argv)==false) {
            return 0;
        }
        //parse arguments
        ;
        //read from camera or from  file
        if (TheInputVideo=="live") {
            TheVideoCapturer.open(0);
            waitTime=10;
        }
        if (TheInputVideo=="live0") {
            TheVideoCapturer.open(0);
            waitTime=10;
        }
        if (TheInputVideo=="live1") {
            TheVideoCapturer.open(1);
            waitTime=10;
        }
        else  TheVideoCapturer.open(TheInputVideo);
        //check video is open
        if (!TheVideoCapturer.isOpened()) {
            cerr<<"Could not open video"<<endl;
            return -1;

        }
        

        //read first image to get the dimensions
        TheVideoCapturer>>TheInputImage;

        //read camera parameters if passed
        if (TheIntrinsicFile!="") {
            TheCameraParameters.readFromXMLFile(TheIntrinsicFile);
            TheCameraParameters.resize(TheInputImage.size());
        }
        //Configure other parameters
        if (ThePyrDownLevel>0)
            MDetector.pyrDown(ThePyrDownLevel);

        //begin copy-paste from http://stackoverflow.com/questions/11550021/converting-a-mat-file-from-matlab-into-cvmat-matrix-in-opencv
		Mat oneVect;
		Mat useVecLat;
		Mat someVects;
		Mat zeroYzero;

		

		string demoFile  = "demo.yml";

		FileStorage fsDemo( demoFile, FileStorage::READ);
		fsDemo["oneVect"] >> oneVect;
		
		fsDemo["oneVect"] >> useVecLat;
		
		
		fsDemo["oneVect"] >> zeroYzero;

		fsDemo["someVects"] >> someVects;

		
		cout << "Print the contents of oneVect:" << endl;
		cout << oneVect << endl;
		
		
		
		fsDemo.release(); //close the file
		
		//end copy-paste from http://stackoverflow.com/questions/11550021/converting-a-mat-file-from-matlab-into-cvmat-matrix-in-opencv
		
		Mat blah = (Mat_<float>(3,1) << 10, 20, 30); // Column vector

		Mat multTimes = (Mat_<float>(3,3) << 2,3,4,5,6,7,8,9,11);
		
		cout <<blah<<endl;
		cout <<multTimes<<endl;
		cout << multTimes*blah <<endl;
		
		
		
		cout << "an element oneVect:" << endl;
		cout << oneVect.at<float>(0,1) << endl;
		cout << oneVect.at<float>(1) << endl;
		
		
		
		// to access the 42 in this YAML:
		
		//oneVect: !!opencv-matrix
		//   rows: 1
		//   cols: 3
		//   dt: f
		//   data: [ 4, 3, 42, 55]
		
		//  do oneVect.at<float>(0,2)


		//end data input



        //Create gui

        cv::namedWindow("thres",1);
        cv::namedWindow("in",1);
        MDetector.getThresholdParams( ThresParam1,ThresParam2);
        MDetector.setCornerRefinementMethod(MarkerDetector::LINES);
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
            //copy image

            index++; //number of images captured
            double tick = (double)getTickCount();//for checking the speed
            //Detection of markers in the image passed

            MDetector.detect(TheInputImage,TheMarkers,TheCameraParameters,TheMarkerSize);
            //note that this function outputs the marker info, for some reason.
            
            
            //chekc the speed by calculating the mean speed of all iterations
            AvrgTime.first+=((double)getTickCount()-tick)/getTickFrequency();
            AvrgTime.second++;
            
            //cout<<"Time detection="<<1000*AvrgTime.first/AvrgTime.second<<" milliseconds"<<endl;
            
            
            //print marker info and draw the markers in image
            TheInputImage.copyTo(TheInputImageCopy);
            for (unsigned int i=0;i<TheMarkers.size();i++) {
//                cout<<TheMarkers[i]<<endl;
                if (TheMarkers[i].id == 605) {   
					Mat R33for605;
					
					cv::Rodrigues(TheMarkers[i].Rvec,R33for605); // takes the 1 by 3 and stores it as a 3 by 3 in R33
					cout << R33for605 << endl;
					
					cout << TheMarkers[i].id << endl;
					
					for (unsigned int maytr=0;maytr<TheMarkers.size();maytr++) {  //take 0 , 1 , 0, inverse transform first, then transform.
						if (TheMarkers[maytr].id == 500) {
							//NOTES APRIL 15 // make 1 by 4 [measured_x; measured_y; measured_z; 0];
							//NOTES APRIL 15 // Tlab * (Tsensor.inv() * sensorMeaurement) 
							// 4th element is nothing
							
							
							
							
							
							//zeroYzero.at<float>(0,0) = 0;
							//zeroYzero.at<float>(0,1) = 2;
							//zeroYzero.at<float>(0,2) = 0;
							//zeroYzero.at<float>(0,3) = 0;
							//zeroYzero.at<float>(0,4) = 0;
							//zeroYzero.at<float>(0,5) = 0;
							
							//cout << zeroYzero << endl;
							Mat R33for500;
							cout << TheMarkers[maytr].id<< "food"<< endl;
							cv::Rodrigues(TheMarkers[maytr].Rvec,R33for500);
							cout << R33for500 << endl;
							//cout << TheMarkers[i].id << endl;
						
							Mat blah = (Mat_<float>(3,1) << 0, 1, 0); // Column vector
							
							//R33for500 * R33.t() * zeroYzero; //transpose method
							
							Mat afterDouble; 
							afterDouble = R33for500.inv() * (R33for605 * blah); //inversion method
							//WORKS!!!
							//YAY!!!
							
							//ACTUALLY PUT THIS IN HERE ----afterDouble
							Mat toShow = (Mat_<float>(1,6) << 0, 0, 0, 
							    afterDouble.at<float>(0,0), afterDouble.at<float>(0,1), afterDouble.at<float>(0,2)); // Column vector
							
							drawVecAtPos(TheInputImageCopy,TheMarkers[maytr],TheCameraParameters,toShow);
							
							
							
							
							
							
							
							
							
							
							
							
							
							
							
							
							
							//USE THIS!
							//Tlab * (Tsensor.inv() * sensorMeaurement
							//R33for500 * (R33.inv() * zeroYzero); //inversion method
							
							
							
							
							//cout << "shelll"<< endl;
							//cout << afterDouble << endl;
							//useVecLat.at<float>(0,0) = 0;
							//useVecLat.at<float>(0,1) = 0;
							//useVecLat.at<float>(0,2) = 0;
							//useVecLat.at<float>(0,3) = afterDouble.at<float>(0,0);
							//useVecLat.at<float>(0,4) = afterDouble.at<float>(0,1);
							//useVecLat.at<float>(0,5) = afterDouble.at<float>(0,2);
							
							//drawVecAtPos(TheInputImageCopy,TheMarkers[maytr],TheCameraParameters,afterDouble); //oneVect);
						}
					}
				}
                
                TheMarkers[i].draw(TheInputImageCopy,Scalar(0,0,255),1);
            }
            
            //print other rectangles that contains no valid markers
       /**     for (unsigned int i=0;i<MDetector.getCandidates().size();i++) {
                aruco::Marker m( MDetector.getCandidates()[i],999);
                m.draw(TheInputImageCopy,cv::Scalar(255,0,0));
            }*/



            //draw a 3d cube in each marker if there is 3d info
            if (  TheCameraParameters.isValid())
                for (unsigned int i=0;i<TheMarkers.size();i++) {
                    
                    CvDrawingUtils::draw3dCube(TheInputImageCopy,TheMarkers[i],TheCameraParameters);
                    //Never use this; just reference // CvDrawingUtils::draw3dAxis(TheInputImageCopy,TheMarkers[i],TheCameraParameters);
                    
                    draw3dAxisj(TheInputImageCopy,TheMarkers[i],TheCameraParameters);
                    //drawVecAtPos(TheInputImageCopy,TheMarkers[i],TheCameraParameters,oneVect);
                    //drawVecsAtPosTesting(TheInputImageCopy,TheMarkers[i],TheCameraParameters,someVects);
                    						
                }
            //DONE! Easy, right?
            std::time_t result = std::time(NULL); //nullptr);
			std::cout << std::asctime(std::localtime(&result))
					  << result << " seconds since the Epoch\n";

            cout<<endl<<endl<<endl;
            //show input with augmented information and  the thresholded image
            cv::imshow("in",TheInputImageCopy);
            cv::imshow("thres",MDetector.getThresholdedImage());

            key=cv::waitKey(waitTime);//wait for key to be pressed
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
    MDetector.setThresholdParams(ThresParam1,ThresParam2);
//recompute
    MDetector.detect(TheInputImage,TheMarkers,TheCameraParameters);
    TheInputImage.copyTo(TheInputImageCopy);
    for (unsigned int i=0;i<TheMarkers.size();i++)	TheMarkers[i].draw(TheInputImageCopy,Scalar(0,0,255),1);
    //print other rectangles that contains no valid markers
    /*for (unsigned int i=0;i<MDetector.getCandidates().size();i++) {
        aruco::Marker m( MDetector.getCandidates()[i],999);
        m.draw(TheInputImageCopy,cv::Scalar(255,0,0));
    }*/

//draw a 3d cube in each marker if there is 3d info
    if (TheCameraParameters.isValid())
        for (unsigned int i=0;i<TheMarkers.size();i++)
            CvDrawingUtils::draw3dCube(TheInputImageCopy,TheMarkers[i],TheCameraParameters);

    cv::imshow("in",TheInputImageCopy);
    cv::imshow("thres",MDetector.getThresholdedImage());
}


