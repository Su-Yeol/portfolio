# # pc(VIEWER ON)
# g++ ./source/*.cpp -I./include -lpthread -I/usr/include/python3.8 -lpython3.8 -DMATPLOTLIBCPP -o ./output/Pedestrian_MinimumDistance
# # pc(VIEWER OFF)
# g++ ./source/*.cpp -lpthread -I./include -o ./output/Pedestrian_MinimumDistance
# target(VIEWER OFF)
aarch64-linux-gnu-g++ -g ./source/*.cpp -lpthread -I./include -o ./output/Pedestrian_MinimumDistance

scp ./output/Pedestrian_MinimumDistance root@192.168.100.99:/home/root/sykim/Pedestrian/output
scp ./config.ini root@192.168.100.99:/home/root/sykim/Pedestrian/output

