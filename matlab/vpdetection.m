%% 
%  Copyright (c) 2011  Chen Feng (cforrest[at]umich[dot]edu)
%   and the University of Michigan
%
%   This program is free software; you can redistribute it and/or modify
%   it under the terms of the GNU General Public License as published by
%   the Free Software Foundation; either version 2 of the License, or
%   (at your option) any later version.
% 
%   This program is distributed in the hope that it will be useful,
%   but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%   GNU General Public License for more details.

function [lines,labels]=vpdetection(img)
%	using jlinkage+lsd to group vanishing lines, did not actually detect vanishing points here
%	but once you know the lines and labels, you can use any estimation methods you like to
%	tackle the problem
%INPUT
%	img <WxH> gray image
%OUTPUT
%	lines <Nx4> each row represent a line segments [x1 y1 x2 y2]
%	labels <Nx1> label start from 1, same label means same group
%AUTHOR
%	Chen Feng <www.umich.edu/~cforrest> <simbaforrest@gmail.com>
%REFERENCE
%	Tardif J.-P., 
%	Non-iterative Approach for Fast and Accurate Vanishing Point Detection, 
%	12th IEEE International Conference on Computer Vision, 
%	Kyoto, Japan, September 27 - October 4, 2009.

addpath('..\lsd-1.5\');

% get line segments from lsd
tic
lines = lsd(double(img));
t = toc;
disp(['[lsd] ',num2str(t),' seconds elapsed.']);

% save lines to a file for excutable vpdetection to use
save('lines.tmp', 'lines', '-ascii', '-tabs');

% vpdetection
[status, result] = system('vpdetection lines.tmp lines.out')

out = load('lines.out');
lines = out(:,1:4);
labels = uint8(out(:,5)+1); %make label subscripts starts from 1
end