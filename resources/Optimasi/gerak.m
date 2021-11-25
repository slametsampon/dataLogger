% This program is composed by Heru Suwoyo
% Wednesday, July 4, 2018 Shanghai University
% Here are the code of the Extended Kalman Filter for the prediction step
% It return the new predicted state g, jacobian matrix wrt to the state G,
% jacobian matrix wrt to control V

function [g, Gg, Vv] = gerak(R,u,n)
if nargin==2
    n=[0;0];
end
w = 10;
x = R(1);
y = R(2);
a = R(3);

l = u(1)+n(1);
r = u(2)+n(2);

    if l ~= r
        alfa = (r-l)/w;
        R = l/alfa;
        xp=x+((R+(w/2))*(sin(a+alfa)-sin(a)));
        yp=y+(R+(w/2))*(-cos(a+alfa)+cos(a));
        ap=a+alfa;
        
        if ap > pi
            ap = ap - 2*pi;
        end
        if ap < -pi
            ap = ap + 2*pi;
        end
        
        g = [xp yp ap]';
        Gg = [1 0 (R+w/2)*(cos(a+alfa)-cos(a));0 1 (R+w/2)*(sin(a+alfa)-sin(a));0 0 1];
        Vv = [((w*r)/(r-l))*(sin(a+alfa)-sin(a))-((r+l)/2*(r-l))*cos(a+alfa) ((-w*l)/(r-l)^2)*(sin(a+alfa)-sin(a))+((r+l)/2*(r-l))*cos(a+alfa);...
            ((w*r)/(r-l))*(cos(a+alfa)-cos(a))-((r+l)/2*(r-l))*sin(a+alfa) ((-w*l)/(r-l)^2)*(-cos(a+alfa)+cos(a))+((r+l)/2*(r-l))*sin(a+alfa);...
            -1/w 1/w];
    end
    if r==l
        xp=x+(l*cos(a));
        yp=y+(l*sin(a));
        ap=a+0;
        
        if ap > pi
            ap = ap - 2*pi;
        end
        if ap < -pi
            ap = ap + 2*pi;
        end
        
        g = [xp yp ap]';
        Gg= [1 0 -l*sin(a);0 1 l*cos(a);0 0 1];
        Vv= [0.5*(cos(a)+((l/w)*sin(a))) 0.5*((-(l/w)*sin(a)+cos(a)));...
            0.5*(sin(a)-((l/w)*cos(a))) 0.5*(((l/w)*cos(a)+sin(a)));...
            -1/w 1/w];
    end
    
end
 



