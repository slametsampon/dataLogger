function ini = pso_pid(Kp,Ki,Kd)
robot = [30 20 0]';
error_sum = 0;
jarak = 100;
jarak_lama = jarak;
store_error = [];
for i=1:500
    delta_t = 1;
    [jarak,posisi]=tampilkan_robot(robot,1);
    setpoint = 30;
    error = setpoint - jarak;
    store_error(i)=error;
    error_sum = error_sum + error*delta_t;
    error_dot = (jarak - jarak_lama)/delta_t;
    u_set = Kp*error + Ki*error_sum + Kd*error_dot;
%     title(sprintf('Jarak: %.f',jarak));
    a = 5-u_set;
    if a<5
        a=1;
    elseif a>13
        a=7;
    end
    u = [a 5];
    n=rand(1,2).*0;
    robot=gerak(robot,u,n);
%     pause(0.001);
    %     clf
end
ini = sum(store_error.^2)
end