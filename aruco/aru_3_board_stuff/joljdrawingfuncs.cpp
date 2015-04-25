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
