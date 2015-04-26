import numpy as np

with open('/home/elabbiglubu/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/intermediate_data_files/pos_data_from_aruco.csv','r') as infi:
   x,y,z,Bx,By,Bz = np.loadtxt(infi,delimiter=',',unpack=True)

scaleFactorForVectors = 0.001;

Bx = Bx/scaleFactorForVectors + 512.0;
By = By/scaleFactorForVectors + 512.0;
Bz = Bz/scaleFactorForVectors + 512.0;

Bx = Bx*(5.0/1023.0);
By = By*(5.0/1023.0);
Bz = Bz*(5.0/1023.0);

Bx = (0.0039*Bx)-0.0096;
By = (0.0039*By)-0.0096;
Bz = (0.0039*Bz)-0.0096;
#resulting units are millitesla	

#scale it a bit more ... log style
Bx = Bx*20.0 # log(Bx)
By = By*20.0 # log(By)
Bz = Bz*20.0 # log(Bz)

#remember that final result for Bx By Bz will be meters (length on screen)





# BEGIN INTERPOLATION STUFF.
grid_x, grid_y, grid_z = np.mgrid[min(x):max(x):6j,   min(y):max(y):6j,   min(z):max(z):6j]

points = zip(x,y,z) # np.random.rand(len(Bx), 3) 

from scipy.interpolate import griddata

grid_Bx_0 = griddata(points, Bx, (grid_x, grid_y, grid_z), method='linear')
grid_By_0 = griddata(points, By, (grid_x, grid_y, grid_z), method='linear')
grid_Bz_0 = griddata(points, Bz, (grid_x, grid_y, grid_z), method='linear')


#grid_z1 = griddata(points, values, (grid_x, grid_y), method='linear')
#grid_z2 = griddata(points, values, (grid_x, grid_y), method='cubic')

#import matplotlib.pyplot as plt
#plt.subplot(221)
#plt.imshow(func(grid_x, grid_y).T, extent=(0,1,0,1), origin='lower')
#plt.plot(points[:,0], points[:,1], 'k.', ms=1)
#plt.title('Original')
#plt.subplot(222)
#plt.imshow(grid_z0.T, extent=(0,1,0,1), origin='lower')
#plt.title('Nearest')
#plt.subplot(223)
#plt.imshow(grid_z1.T, extent=(0,1,0,1), origin='lower')
#plt.title('Linear')
#plt.subplot(224)
#plt.imshow(grid_z2.T, extent=(0,1,0,1), origin='lower')
#plt.title('Cubic')
#plt.gcf().set_size_inches(6, 6)
#plt.show()

xi=grid_x.flatten()
yi=grid_y.flatten()
zi=grid_z.flatten()
Bxi=grid_Bx_0.flatten()
Byi=grid_By_0.flatten()
Bzi=grid_Bz_0.flatten()

filepathThing = '/home/elabbiglubu/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/intermediate_data_files/pos_data_done_with_python_to_do_octave.csv'
np.savetxt(filepathThing, zip(xi,yi,zi,Bxi,Byi,Bzi),delimiter=",")


	
