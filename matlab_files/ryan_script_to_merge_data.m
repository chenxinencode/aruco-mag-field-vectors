%if false %% CHANGE ME 

BFile=dlmread('mag_data');
bx=BFile(:,1);
by=BFile(:,2);
bz=BFile(:,3);
h=BFile(:,7);
m=BFile(:,8);
s=BFile(:,9);


bt=h*3600+m*60+s;



fid=fopen('sample_pos_data.txt');
x=[];
y=[];
z=[];
pt=[];

% kill the junk
for junk=1:4
fgetl(fid)
end

while 1
line=fgetl(fid);
if line ==-1
fclose(fid)

break
end


if line(1) ~='q'
[xte,yte,zte,ptte]=strread(line,'%f,%f,%f,%d');
x=[x xte];
y=[y yte];
z=[z zte];
pt=[pt ptte];
end
end


while pt > 24*60*60
pt=pt-24*60*60;
end


magx=[];
magy=[];
magz=[];
magt=[];
posx=[];
posy=[];
posz=[];

%pt=[77321 77324 77329];
l=length(bt);
for i=1:l
for j=1:length(pt)
if bt(i)==pt(j)
magxte=bx(i);
magyte=by(i);
magzte=bz(i);
magtte=bt(i);
xt=x(j);
yt=y(j);
zt=z(j);
magx=[magx magxte];
magy=[magy magyte];
magz=[magz magzte];
magt=[magt magtte];
posx=[posx xt];
posy=[posy yt];
posz=[posz xt];


end 
end
%end

end

%posx=[1 2 3 4 5 6 7];
%posy=[0 0 0 0 0 0 0];
%posz=[0 0 0 0 0 0 0];
%magx=(posx-3).^2;
%magy=[0 0 0 0 0 0 0];
%magz=[0 0 0 0 0 0 0];

quiver3AR(posx,posy,posz,magx,magy,magz);
