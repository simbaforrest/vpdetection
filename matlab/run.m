close all; clear all; clc;

[im,lines,labels]=vpdetectionOn('../data/indoor.jpg');
print('../data/indoor-out','-dpng');

disp('press any key to continue...');
pause;

[im,lines,labels]=vpdetectionOn('../data/outdoor.jpg');
print('../data/outdoor-out','-dpng');