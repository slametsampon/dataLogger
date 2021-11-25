clc, clear all, close all
ini_pbest = [];
global population_size
population_size=2;
% maximum generasi
maxit=30;
w=rand(1);
scale=0.001;
dim=3;
N=population_size;
pbest = [];
gbest = [];
% .*repmat((batas atas-batas bawah),N,1)+repmat(batas bawah,N,1);
% Input Membership Function (error)
x1=rand(N,1).*repmat((0+100),N,1)+repmat(-100,N,1);
x2=rand(N,1).*repmat((60+60),N,1)+repmat(-60,N,1);
x3=rand(N,1).*repmat((100-0),N,1)+repmat(0,N,1);
x=[x1';x2';x3'];
v = rand(3,population_size)/50;
xmax = max(max(x));
xmin = min(min(x));
vmax = max(max(v));
vmin = min(min(v));
output=[];

for gint=1:maxit
    f=[];
    for i=1:population_size
        ac = x(:,i);
        f(i,:)=pso_flc(ac(1),ac(2),ac(3));
    end
    [pbest,it]=min(f);
    gbest=x(:,it);
    f2=[];
    % update velocity and location of all particles
    satisfy=0;
    while satisfy<1
        for k=1:population_size
            % update
            for d=1:3
                v(d,k)=w*v(d,k)+ scale*rand(1)*(pbest-x(d,k)) + scale*rand(1)*(gbest(d)-x(d,k));
                x(d,k)=x(d,k)+v(d,k);
            end
            % evaluasi
            ac = x(:,k);
            f2(k,:)=pso_flc(ac(1),ac(2),ac(3));
        end
        % obtain new pbest and gbest
        [newpbest,itn]=min(f2);
        newgbest = x(:,itn);
        if newpbest<pbest
            pbest = newpbest;
            gbest = newgbest;
            satisfy= 3;
        else
            pbest = pbest;
            gbest = gbest;
        end
        output(:,gint)=[gint;pbest;gbest];
        ini_pbest(gint) = pbest;
    end
    disp(['Process ' num2str(gint/maxit*100) '%'])
end
disp('==========DONE===========');
pbest = output(2,gint);
gbest = output(3:5,gint);
figure(1);
xlabel('Generation');
ylabel('Fitness');
plot(ini_pbest,'b','LineWidth',1.5)
disp(['best fitness = ' num2str(pbest)]);
disp(['best solution = ' num2str(gbest')]);
disp('=========================');
disp('Silahkan input nilai (best solution) pada Fuzzy Logic Controlled-WFR');
disp('=========================');