fid = fopen('~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/intermediate_data_files/pos_data_from_aruco.csv');

x =[0];
y =[0];
z =[0];
Bx=[0];
By=[0];
Bz=[0];
i=1;
while 1
	line=fgetl(fid)
	if line==-1 
		fclose (fid);
		break
	end
	
	try
		[x(i),y(i),z(i),Bx(i),By(i),Bz(i)]=strread(line,'%f,%f,%f,%f,%f,%f')
		i=i+1;
	catch 
		fprintf('skipped one line\n')
	end

end





quiver3AR(x, y, z, Bx, By, Bz);
