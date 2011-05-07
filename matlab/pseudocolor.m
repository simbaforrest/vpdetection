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

function colors=pseudocolor(maxcolors)
% generate pseudo colors for visualize classification labels
% idea from "computer vision?algorithm and application" by Szeliski
%INPUT
%   maxcolors <1x1>: number of colors required, i.e. number of
%   groups/clusters
%OUTPUT
%   colors <Nx3>: color vectors, N=min(maxcolors,2^bits_total)
%AUTHOR
% by Chen Feng <simbaforrest@gmail.com>
bpc = (maxcolors>8) + (maxcolors>64) + 1;
bits_total = 3*bpc;
color_num = min(2^bits_total,maxcolors);
colors = zeros(color_num,3);
for c=0:color_num-1
    r=uint16(0);
    g=uint16(0);
    b=uint16(0);
    for k=0:3:bits_total-1
        b = bitshift(b,1) + ...
            bitand(bitshift(uint16(c), -k), uint16(1));
        g = bitshift(g,1) + ...
            bitand(bitshift(uint16(c), -k-1), uint16(1));
        r = bitshift(r,1) + ...
            bitand(bitshift(uint16(c), -k-2), uint16(1));
    end
    r = bitshift(r, (8 - bpc));
    g = bitshift(g, (8 - bpc));
    b = bitshift(b, (8 - bpc));
    colors(c+1,:) = double([r,g,b])/255;
end
end