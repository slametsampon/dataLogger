% buat path/trajectory berdasarkan posisi robot awal dan list of control
% command
u = [11 12;12 11;5 12;4 12];
robot_posisi = [10,10,pi/8];
storage=zeros(size(u,1),2);
for i=1:size(u,1)
    n = rand(2,1);
    robot_posisi = gerak(robot_posisi,u(i,:),n);
    tampilkan_robot(robot_posisi,0)
    storage(i,:)=robot_posisi(1:2);
end