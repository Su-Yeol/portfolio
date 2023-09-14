aarch64-linux-gnu-g++ -g ./source/*.cpp -lpthread -I./include -o ./output/PathSaveModule

scp ./output/PathSaveModule root@192.168.100.99:/home/root/sykim/PathSaveModule/
