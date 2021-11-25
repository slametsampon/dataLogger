function [jarak_ke_titik,titik_pada_dinding] = tampilkan_robot(posisi_robot,kondisi)
% ubah arena disini sesuka anda
A=[0,0];
B=[400,0];
C=[400,400];
D=[0,400];
E=[200,0];
F=[200,150];
G=[300,150];
H=[300,250];
I=[400,250];
J=[200,250];
K=[200,400];
L=[0,200];
M=[120,200];
N=[80,200];
O=[80,120];
wall_awal=[A;B;C;D;E;F;H;J;L;N];
wall_akhir=[B;C;D;A;F;G;I;K;M;O];
if kondisi ~= 0
    walls_x = [wall_awal(:,1) wall_akhir(:,1) NaN*wall_awal(:,1)];
    walls_y = [wall_awal(:,2) wall_akhir(:,2) NaN*wall_awal(:,2)];
    N_walls=length(wall_awal(:,1));
    walls_x = reshape(walls_x',N_walls*3,1);
    walls_y = reshape(walls_y',N_walls*3,1);
%     plot(walls_x,walls_y,'k','Linewidth',5);
%     hold on
end
% semua sisi tubuh robot dilihat dari atas, mengacu pada pusat posisi robot
x=posisi_robot(1);
y=posisi_robot(2);
theta=posisi_robot(3);
% parameter robot
lebar_robot=10;
panjang_robot=12;
% titik untuk tubuh robot
depan = panjang_robot/2;
belakang = -panjang_robot/2;
kiri = -lebar_robot/2;
kanan = lebar_robot/2;
p1=[kiri depan];
p2=[kiri belakang];
p3=[kanan belakang];
p4=[kanan depan];
tubuh_robot=[p1;p2;p3;p4];
% kita gunakan rotation matrix
offset=pi/2;
R = [cos(theta-offset) sin(theta-offset);-sin(theta-offset) cos(theta-offset)];
% rotasi tubuh robot
tubuh_robot = jumlahkan(tubuh_robot*R,[x y]);
xdata=[tubuh_robot(:,1);tubuh_robot(1,1)];
ydata=[tubuh_robot(:,2);tubuh_robot(1,2)];
% plot(xdata,ydata,'r');
% hold on

% parameter roda robot
lebar_roda = 3;
panjang_roda=4;
spasi=-0.5*lebar_roda;
% titik pada roda kiri
wl1 = [kiri-lebar_roda-spasi 0];
wl2 = [kiri-lebar_roda-spasi -panjang_roda];
wl3 = [kiri-spasi -panjang_roda];
wl4 = [kiri-spasi 0];
roda_kiri = [wl1;wl2;wl3;wl4];
roda_kiri = jumlahkan(roda_kiri*R,[x,y]);
xdata_roda_kiri = [roda_kiri(:,1);roda_kiri(1,1)];
ydata_roda_kiri = [roda_kiri(:,2);roda_kiri(1,2)];
% plot(xdata_roda_kiri,ydata_roda_kiri,'b');
% hold on
% roda kanan
wr1 = [kanan+spasi 0];
wr2 = [kanan+spasi -panjang_roda];
wr3 = [kanan+spasi+lebar_roda -panjang_roda];
wr4 = [kanan+spasi+lebar_roda 0];
roda_kanan = [wr1;wr2;wr3;wr4];
roda_kanan = jumlahkan(roda_kanan*R,[x,y]);
xdata_roda_kanan = [roda_kanan(:,1);roda_kanan(1,1)];
ydata_roda_kanan = [roda_kanan(:,2);roda_kanan(1,2)];
% plot(xdata_roda_kanan,ydata_roda_kanan,'b');
% hold on
% axis equal
% axis square
% grid on

% parameter sensor
max_range=100;
lebar_sensor  = 0.5;
panjang_sensor= 1.5;
sensor_posisi = [2,4,pi/2-deg2rad(35)];
% titik-titik untuk sensor
kanan_ss = lebar_sensor/2;
belakang_ss = -panjang_sensor/2;
kiri_ss = -lebar_sensor/2;
depan_ss = panjang_sensor/2;
s1=[kiri_ss depan_ss];
s2=[kiri_ss belakang_ss];
s3=[kanan_ss belakang_ss];
s4=[kanan_ss depan_ss];
sensor = [s1;s2;s3;s4];
% rotasikan sensor agar hadapannya sesuai
offset=0;
Rs=[cos(sensor_posisi(3)-offset) sin(sensor_posisi(3)-offset);-sin(sensor_posisi(3)-offset) cos(sensor_posisi(3)-offset)];
sensor = jumlahkan(sensor*Rs,[sensor_posisi(1) sensor_posisi(2)]);
sensor = jumlahkan(sensor*R,[x,y]);
xdata_sensor = [sensor(:,1);sensor(1,1)];
ydata_sensor = [sensor(:,2);sensor(1,2)];
% plot(xdata_sensor,ydata_sensor,'m');
% hold on

% define
xo_sv = sensor_posisi(1);
yo_sv = sensor_posisi(2);
to_sv = sensor_posisi(3);
delta_y = max_range*sin(to_sv);
delta_x = max_range*cos(to_sv);
sensor_vector = [xo_sv yo_sv;delta_x+xo_sv delta_y+yo_sv];
sensor_vector = jumlahkan(sensor_vector*R,[x,y]);
%     sensor_vector = sensor_vector*Rs+[x,y];
[jarak_ke_titik,titik_pada_dinding] = hit_dinding(wall_awal,wall_akhir,sensor_vector);
if kondisi ~= 0 && jarak_ke_titik>0 && jarak_ke_titik<100
    sensor_vector(2,:)=titik_pada_dinding;
else
    jarak_ke_titik = 100;
end
xdata_sensor_vektor = [sensor_vector(:,1);sensor_vector(1,1)];
ydata_sensor_vektor = [sensor_vector(:,2);sensor_vector(1,2)];
% plot(xdata_sensor_vektor,ydata_sensor_vektor,'g');
% hold on

% xlim([0 400])
% ylim([0 400])
% hold on;
end
