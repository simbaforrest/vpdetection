%-----------------------------------------------------------
% Plot clustered Points
%
% Usage:
%
% showClusters(Points,T, minlength)
%
% Arguments:
%     Points           - Data set
%     T                - Clustered points Labels
%     minlength        - Minimum number of element for a cluster
%-----------------------------------------------------------
function showClusters(Points,T, minlength)


j = 1;
if(size(T,2) > 1 && size(T,1) == size(Points,2))
    T2 = ones(size(Points,2),1) * -1;
    for i=1:size(T,1)
        tmp = find(T(i,:));
        if(length(tmp) > 0)
            T2(i) = tmp(1); 
        end
    end
    T = T2;
end

% plot results
%hold off;
for i=1:max(T)
           
    
    res = ceil(sqrt(sqrt(max(T))));
    r = mod(i,res) / res;
    g = mod(i/res,res) / res;
    b = mod((i/res)/res,res) / res;
    
    color = [r g b];
        
    inCurrentModel = find(T == i);
    if(length(inCurrentModel) > minlength)
        if(size(Points,1) == 3)
            plot3(Points(1,inCurrentModel), Points(2,inCurrentModel), Points(3,inCurrentModel), '.', 'Color', color, 'MarkerSize', 5);
        end
        if(size(Points,1) == 2)
            plot(Points(1,inCurrentModel), Points(2,inCurrentModel), '.', 'Color', color, 'MarkerSize', 5);
        end
    else
        if(size(Points,1) == 3)
            plot3(Points(1,inCurrentModel), Points(2,inCurrentModel), Points(3,inCurrentModel), 'k+', 'MarkerSize', 5);
        end
        if(size(Points,1) == 2)
            plot(Points(1,inCurrentModel), Points(2,inCurrentModel), 'k+', 'MarkerSize', 5);
        end

    end
        hold on;

end


end