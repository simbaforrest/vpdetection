function drawVPGroup(img, lines, labels)
close;
imshow(img);
hold on;

ngp = max(labels);
colors = rand(ngp,3);

nl = size(lines,1);
for i=1:nl
    plot(lines(i,1:2:4),lines(i,2:2:4),...
        '-','Color',colors(uint8(labels(i)),:), 'LineWidth',2);
end
end