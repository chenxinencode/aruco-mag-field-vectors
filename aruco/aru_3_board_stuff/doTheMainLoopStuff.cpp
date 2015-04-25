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


