%Random Topography Generation
clear; clc; close all
%the elevation range of Mars is 29 km (18 miles)
%You can generate N random numbers in the interval (a,b) 
% with the formula r = a + (b-a).*rand(N,1)

%grid
x = rand(100,1);
y = rand(100,1);
z = 1.0 + (1.0-0.0).*rand(100,1);
[x1,y1] = meshgrid(0:0.01:1);
z1 = griddata(x,y,z,x1,y1,'cubic');

avgz = (max(max(z1)) - min(min(z1)))/2;
maxz = max(max(z1));
z2 = -abs(z1-1.7);

h1 = figure; set(h1,'WindowStyle','docked');
plot3(x,y,z,'.r')
surface(x1,y1,z2,"edgecolor","none")

h2 = figure; set(h2,'WindowStyle','docked');
plot3(x,y,z,'.r')
surface(x1,y1,((z1/maxz).^2),"edgecolor","none")
zlim([-1, 2])

M = zeros(101*101,3);
for n = 1:101
    M(((n-1)*100 + n):(n*100 + n),1) = x1(:,n);
    M(((n-1)*100 + n):(n*100 + n),2) = y1(:,n);
    M(((n-1)*100 + n):(n*100 + n),3) = z1(:,n);
end
writematrix(M,'desert_topo.csv')
