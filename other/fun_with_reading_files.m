fid = fopen('test.txt');
dlmread(


x=[];
y=[];
z=[];
i=1;
while 1
line=fgetl(fid);
if line==-1 
fclose (fid);
break
end

[x(i),y(i),z(i)]=strread(line,'%f,%f,%f');
i=i+1;
end
