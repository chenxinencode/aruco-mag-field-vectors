[X,Y] = meshgrid(-2:0.25:2,-1:0.2:1);
Z = X.* exp(-X.^2 - Y.^2);
[U,V,W] = surfnorm(X,Y,Z);

figure
quiver3(X,Y,Z,U,V,W)
quiver3AR(X,Y,Z,U,V,W)


hold on
% surf(X,Y,Z)
view(-35,45)
axis([-2 2 -1 1 -.6 .6])
hold off