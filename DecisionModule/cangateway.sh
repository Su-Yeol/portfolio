aarch64-linux-gnu-g++ -g ./source_gateway/CANGateway.cpp -lpthread -I./include -o ./output/test
scp ./output/test root@192.168.10.98:/home/root/g80/CanGateway



