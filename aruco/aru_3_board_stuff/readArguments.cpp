bool readArguments ( int argc,char **argv )
{



    if (argc<5) {

        //cerr<<"Invalid number of arguments"<<endl;
        cerr<<"Usage: (in.avi|live) boardConfig.yml [intrinsics.yml] [size] [out]"<<endl;
        //return false;

        cerr << "Note from Jaime: using defaults" <<endl;
        string homeFolder = argv[1];

        string inputparamFileName = argv[2];

        //recording = argv[3];

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
