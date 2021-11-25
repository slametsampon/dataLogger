function ini = pso_flc(mbf1,mbf2,mbf3)
lwbound=-100;
upbound=100;
a=newfis('FLC');
a.input(1).name='error';
a.input(1).range=[lwbound upbound];
a.input(1).mf(1).name='Negative';
a.input(1).mf(1).type='trimf';
a.input(1).mf(1).params=[-100 mbf1 0];
a.input(1).mf(2).name='Zero';
a.input(1).mf(2).type='trimf';
a.input(1).mf(2).params=[-60 mbf2 60];
a.input(1).mf(3).name='Possitive';
a.input(1).mf(3).type='trimf';
a.input(1).mf(3).params=[0 mbf3 100];

a.output(1).name='kecepatan';
a.output(1).range=[lwbound upbound];
a.output(1).mf(1).name='decrease';
a.output(1).mf(1).type='trimf';
a.output(1).mf(1).params=[-100 -0.1 0];
a.output(1).mf(2).name='no_act';
a.output(1).mf(2).type='trimf';
a.output(1).mf(2).params=[-10 0 10];
a.output(1).mf(3).name='increase';
a.output(1).mf(3).type='trimf';
a.output(1).mf(3).params=[0 1 100];

a.rule(1).antecedent=[1];
a.rule(1).consequent=[1];
a.rule(1).weight=1;
a.rule(1).connection=1;

a.rule(2).antecedent=[2];
a.rule(2).consequent=[2];
a.rule(2).weight=1;
a.rule(2).connection=1;

a.rule(3).antecedent=[3];
a.rule(3).consequent=[3];
a.rule(3).weight=1;
a.rule(3).connection=1;
% fuzzy(a) % tampilkan rancangan fuzzy

robot = [30 20 0]';
set_point=40;
store_error=[];
for i=1:500
    [jarak,posisi]=tampilkan_robot(robot,1);
    error=(set_point-jarak)/100;
    store_error(i)=error;
    u_set = evalfis(error,a)/10;
    itu=5;
    ini = itu+(u_set);
    u = [itu ini];
    n=rand(1,2).*0;
    robot=gerak(robot,u,n);
    pause(0.000001);
end
ini = sum(store_error.^2);
end