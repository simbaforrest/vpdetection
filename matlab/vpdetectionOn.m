function [im, lines, labels] = vpdetectionOn(imname)
im=imread(imname);
gim = rgb2gray(im);
[lines, labels]=vpdetection(gim);
drawVPGroup(gim,lines,labels);
end
