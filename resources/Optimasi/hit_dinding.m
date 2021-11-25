function [distance,location] = hit_dinding(wall_awal,wall_akhir,sensor_vector,varargin)   
p = wall_awal;
q = sensor_vector(1,:);
r = wall_akhir - wall_awal;
s = sensor_vector(2,:)-sensor_vector(1,:); 
r_cross_s = crossProduct(r,s);
% q_minus_p =  q - p;
q_minus_p = kurangkan(q,p);
q_minus_p_cross_s = crossProduct(q_minus_p,s); 
q_minus_p_cross_r = crossProduct(q_minus_p,r); 

parallel_indices = find(0==r_cross_s);
if any(parallel_indices)
    r_cross_s(indices) = 1; 
end
t = q_minus_p_cross_s./r_cross_s;
u = q_minus_p_cross_r./r_cross_s;
t(parallel_indices) = inf;
u(parallel_indices) = inf;
intersection = NaN*ones(length(p(:,1)),2);
good_vector = ((0<t).*(1>t).*(0<u).*(1>u));
good_indices = find(good_vector>0);
if ~isempty(good_indices)
    result = p + kalikan(t,r);
%     result = p + t.*r
    intersection(good_indices,:) = result(good_indices,:);
end
ini=[];
for i=1:size(intersection,1)
    ini(i,:)=intersection(i,:) - sensor_vector(1,:);
end
distances_squared = sum((ini).^2,2);
[best,best_index] = min(distances_squared);

distance = best^0.5;
location = intersection(best_index,:);

function result = crossProduct(v,w)
result = v(:,1).*w(:,2)-v(:,2).*w(:,1);




