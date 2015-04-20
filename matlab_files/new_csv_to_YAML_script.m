alldat= dlmread('~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/intermediate_data_files/pos_data_from_aruco.csv');

scaleFactorForVectors = 0.001;

x=  alldat(:,1);
y=  alldat(:,2);
z=  alldat(:,3);
Bx= alldat(:,4);
By= alldat(:,5);
Bz= alldat(:,6);

Bx = Bx/scaleFactorForVectors + 512;
By = By/scaleFactorForVectors + 512;
Bz = Bz/scaleFactorForVectors + 512;

Bx = Bx*(5/1023);
By = By*(5/1023);
Bz = Bz*(5/1023);

Bx = (0.0039*Bx)-0.0096;
By = (0.0039*By)-0.0096;
Bz = (0.0039*Bz)-0.0096;
%resulting units are millitesla	

%scale it down a bit more ... log style
Bx = Bx*2 % log(Bx)
By = By*2 % log(By)
Bz = Bz*2 % log(Bz)

%remember that final result for Bx By Bz will be meters

quiver3AR(x, y, z, Bx, By, Bz);
