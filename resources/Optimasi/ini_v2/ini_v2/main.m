clear all, close all, clf
% wall-following robot sederhana
robot = [30 20 0]';
for i=1:200
    % baca sensor
    [jarak,posisi]=tampilkan_robot(robot,1);
    n = rand(2,1)*0;
    if jarak < 5 jarak=NaN; end
    % tampilkan jarak
	title(sprintf('jarak: %.f',jarak));
    % hentikan simulasi jika jarak dibawah 5
    if isnan(jarak)
        break;
    end
    % letakkan algoritma anda disini
    if jarak <= 20
        u = [3,12];
    elseif jarak < 18
        u = [0,18];
    elseif jarak>20 && jarak<35
        u = [8,7];
    else
        u = [12,5];
    end
    robot=gerak(robot,u,n);
    pause(0.01);
end