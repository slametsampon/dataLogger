clc, clear all, close all;
it=1;
maxit=10;
Npop=30;
individu=floor(rand(Npop,24)*2);
pbest = [];
gbest = [];
output = [];
for gint=1:maxit
    % fitness
    [Kp,Ki,Kd] = ind2gain(individu,Npop);
    parent = [Kp,Ki,Kd];
    pipi=parent;
    for i=1:Npop
        % fitness for all chromosome
        f(i,:)=ga_pid(parent(i,1),parent(i,2),parent(i,3));
    end
    [pbest,idx1] = min(f);
    gbest = parent(idx1,:);
    
    % 1/fitness
    overfit = 1./f;
    % sum(1/fitness);
    sumoverfit = sum(overfit);
    % relative fitness
    rel_fit = overfit./sumoverfit;
    %proof=sum(rel_fit);
    % expected count
    exp_count = rel_fit.*Npop;
    angle = rel_fit./sum(rel_fit)*360;
    cum_angle = cumsum(angle);
    % generate random prob
    [a1,a2,a3] = ind2gain(individu,Npop);
    % select chromosome with lowest fitness using roullete wheel selection
    % method
    for j=1:Npop
        rand_prob(i) = rand(1)*360;
        [val,idx] = min(abs(cum_angle-rand_prob(i)));
        if rand_prob(i)>cum_angle(idx)
            individu(j) = individu(idx+1);
        else
            individu(j)=individu(idx);
        end
    end
    [b1,b2,b3] = ind2gain(individu,Npop);
    child = individu;
    satisfy = 0;
    while satisfy<1
        m=floor(Npop/2);
        probCO=0.3; % Probabilitas Cross Over
        for i=1:m
            p=rand;
            if p<probCO
                r1=floor(24*rand)+1;
                r2=floor((24-r1)*rand)+r1;
                child(2*i-1,r1:r2)=child(2*i,r1:r2);
                child(2*i,r1:r2)=child(2*i-1,r1:r2);
            end
        end
        [c1,c2,c3] = ind2gain(child,Npop);
        probMut=0.3; %Probabilitas mutasi
        for i=1:Npop
            p=rand;
            if p<probMut
                r=floor(rand*24)+1;
                child(i,r)=1-child(i,r);
            end
        end
        individu = child;
        [Kp,Ki,Kd]=ind2gain(individu,Npop);
        parent = [Kp,Ki,Kd];
        popo2 = parent;
        for i=1:Npop
            % fitness for all chromosome
            f2(i,:)=ga_pid(parent(i,1),parent(i,2),parent(i,3));
        end
        [newpbest,idx2] = min(f2);
        newgbest = parent(idx2,:);
        if newpbest<pbest
            pbest = newpbest;
            gbest = newgbest;
            satisfy= 3;
        else
            pbest = pbest;
            gbest = gbest;
        end
        output(:,gint)=[gint;pbest;gbest'];
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
disp('Silahkan input nilai (best solution) pada PID-WFR');
disp('=========================');
