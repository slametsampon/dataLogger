function c = jumlahkan(a,b)
c=[];
for i=1:size(a,1)
    c(i,:) = a(i,:)+b;
end
end

