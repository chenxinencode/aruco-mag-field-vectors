alldat= dlmread('~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/intermediate_data_files/pos_data_done_with_python_to_do_octave.csv');

x=  alldat(:,1);
y=  alldat(:,2);
z=  alldat(:,3);
Bx= alldat(:,4);
By= alldat(:,5);
Bz= alldat(:,6);

quiver3AR(x, y, z, Bx, By, Bz);