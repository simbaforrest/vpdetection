close all; clear all; clc;
im=imread('../data/indoor.jpg');
gim = rgb2gray(im);
[lines, labels]=vpdetection(gim);
drawVPGroup(gim,lines,labels);

disp('press any key to continue...');
pause;

im=imread('../data/outdoor.jpg');
gim = rgb2gray(im);
[lines, labels]=vpdetection(gim);
drawVPGroup(gim,lines,labels);
