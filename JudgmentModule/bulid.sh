aarch64-linux-gnu-g++ -g ./source/*.cpp -lpthread -I./include -o ./output/Pedestrian_MinimumDistance

scp ./output/Pedestrian_MinimumDistance root@192.168.100.99:/home/root/sykim/Pedestrian/output
scp ./config.ini root@192.168.100.99:/home/root/sykim/Pedestrian/output