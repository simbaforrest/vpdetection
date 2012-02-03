function drawVPGroup(img, lines, labels)
close;
imshow(img);
hold on;

colors = fixedcolor;
nc = length(colors);

nl = size(lines,1);
for i=1:nl
    plot(lines(i,1:2:4),lines(i,2:2:4),...
        '-','Color',colors(mod(uint8(labels(i)-1),nc)+1,:), 'LineWidth',2);
end
end