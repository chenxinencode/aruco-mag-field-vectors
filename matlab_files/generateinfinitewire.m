rangee = -.1:.03:.11;
xran = rangee;
yran = rangee(rangee>0);
zran = rangee;

[x, y, z] = meshgrid(xran, yran, zran); %(-1:1, -1:1, -1:1); (-.21:.1:.2, -.21:.1:.2, -.21:.1:.2);



Bx = -z./((x.^2+z.^2).^(3/2));
Bz = x./((x.^2+z.^2).^(3/2));
By = 0.*x;

%Bx = Bx./10000;
%Bz = Bz./10000;

%max(abs(Bx(:)))
%max(abs(Bz(:)))
%min(abs(Bx(:)))
%min(abs(Bz(:)))




% log scaling
logScaleFlag = true
if logScaleFlag

	Bx(Bx>1) = log(Bx(Bx>1));
	Bx(Bx<-1)  = -log(-Bx(Bx<-1));

	Bz(Bz>1) = log(Bz(Bz>1));
	Bz(Bz<-1)  = -log(-Bz(Bz<-1));
else
	Bx = Bx./100;
	Bz = Bz./100;
	
	threshforcenter = 0.05;

	%Bx((abs(xran)>threshforcenter) & (abs(zran)>threshforcenter)) = 0
	%Bz((abs(xran)>threshforcenter) & (abs(zran)>threshforcenter))=0

	%zran = zranoriginal((abs(xranoriginal)>threshforcenter) & (abs(zranoriginal)>threshforcenter))
	


end






%max(abs(Bx(:)))
%max(abs(Bz(:)))
%min(Bx(:))
%min(Bz(:))

Bx = Bx./100;
Bz = Bz./100;



%Bx(    abs(Bx)    >  (median(abs(Bx))*1))=0;
%Bz(abs(Bz)        >  (median(abs(Bz))*1))=0;


quiver3AR(x,y,z,Bx,By,Bz);


% Validation!
theFinalCountdown = zeros(length(x(:)),6);
theFinalCountdown(:,1) = x(:);
theFinalCountdown(:,2) = y(:);
theFinalCountdown(:,3) = z(:);
theFinalCountdown(:,4) = Bx(:);
theFinalCountdown(:,5) = By(:);
theFinalCountdown(:,6) = Bz(:);

paththing = '~/Desktop/GIT_USE_THIS/aruco-mag-field-vectors/intermediate_data_files'
csvwrite('~/pos_data_from_aruco.csv',theFinalCountdown)

