%x = [1, 2, 3.5]
%y = [11, 12, 13.5]
%z = [21, 22, 23.5]

[x,y,z]=meshgrid(1:4,12:15,22:25);


xint = 3.5;
yint = 13.5;
zint = 23.5;

v=2*x;

griddata3(x,y,z,v,xint,yint,zint)
