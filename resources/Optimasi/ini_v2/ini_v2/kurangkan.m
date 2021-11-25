function c = kurangkan(a,b)
c=[];
for i=1:size(b,1)
    c(i,:) = a-b(i,:);
end
end