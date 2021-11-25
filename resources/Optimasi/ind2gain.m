function [Kp,Ki,Kd]= ind2gain(individu,Npop)
x=individu(1:Npop,1:8  );
y=individu(1:Npop,9:16 );
z=individu(1:Npop,17:24);
x=bin2dec(num2str(x)).*1/255;
y=bin2dec(num2str(y)).*1/255;
z=bin2dec(num2str(z)).*1/255;
% .*repmat((batas atas-batas bawah),N,1)+repmat(batas bawah,N,1);
Kp = x.*repmat((2-0),Npop,1)+repmat(0,Npop,1);
Ki = y.*repmat((0-0),Npop,1)+repmat(0,Npop,1);
Kd = z.*repmat((1+1),Npop,1)+repmat(-0.5,Npop,1);
end