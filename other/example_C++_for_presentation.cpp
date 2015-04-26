
float probDetectLab=TheBoardDetectorLab.detect(TheInputImage);

if (probDetectLab>0.2)   {  // If we detected the lab frame board
	CvDrawingUtils::draw3dAxis( TheInputImageCopy,TheBoardDetectorLab.getDetectedBoard(),TheCameraParameters);
}

if(recording == false && probDetectLab>0.2){
	drawVecsAtPos(...);
}

if ( probDetectLab>0.2  && probDetect>0.2 && recording == true) // detected lab board and board1
{
	//read arduino
		string inFromArduino = arduino.read();
		processedInFromArduino = doMathAndProcesssing(inFromArduino);
	
	//rotation stuff
		Mat R33forLab;
		cv::Rodrigues(TheBoardDetectorLab.getDetectedBoard().Rvec,R33forLab);
		// takes the 1 by 3 and stores it as a 3 by 3 in R33forLab

		Mat R33forSensorSideNumber1;
		cv::Rodrigues(TheBoardDetector.getDetectedBoard().Rvec,R33forSensorSideNumber1);

		Mat rotatedMagFieldReading;
		rotatedMagFieldReading = R33forLab.inv() * (R33forSensorSideNumber1 * processedInFromArduino); 

	//translation stuff
		Mat locationOfSensorTag = TheBoardDetector.getDetectedBoard().Tvec;
		
		Mat offsetOfSensorFromSensorTag = (Mat_<float>(3,1) << 0, 0.05, 0.015); // Column vector
		
		distSensorToCamera = locationOfSensorTag - (R33forSensorSideNumber1*offsetOfSensorFromSensorTag); 
		
		Mat distSensorFromLabFrame = R33forLab.inv()*(distSensorToCamera - TheBoardDetectorLab.getDetectedBoard().Tvec);

	//View what we just measured for real-time checking if it's working
		drawVecAtPos(...);
	
}

