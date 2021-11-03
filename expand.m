function [ ca, mean ] = expand(K,clusters,Nclusters)
% Input: K - K value
%        clusters - cell array of data points
%        Nclusters - array containing number of members in each cluster classes
% Output: ca - resulting cell array of partitioned classes
ca = clusters;
mean = zeros(1,2);
for i = 1:K
    % Repeat kmean if the column number > K
    if Nclusters(i) > 3*K
        K_new = floor(Nclusters(i)/10);
        Mtemp = cell2mat(cellfun(@(x) cell2mat(x), clusters(1:Nclusters(i),i),'un',0)); % matrix conversion
        [tempclusters tempKmeans tempNclusters tempmembership] = my_kmean(Mtemp,K_new,0);
        [row col] = size(clusters);
        temp = shape(tempclusters, row);
        mean = [mean; tempKmeans];
        ca(:,i) = expand(K_new,temp,tempNclusters);
    end
end
mean = [mean(2:end, 1) mean(2:end, 2)];

end
