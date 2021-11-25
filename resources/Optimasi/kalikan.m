function g=kalikan(a,b)
g = [];
for i=1:size(a,1)
g(i,:) = a(i).*b(i,:);
end