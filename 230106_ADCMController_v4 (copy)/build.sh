# pc(VIEWER ON)
g++ ./src/*.cpp -I./inc -lpthread -I/usr/include/python3.8 -lpython3.8 -DMATPLOTLIBCPP -o ./output/adcm_pc_vieweron

# pc(VIEWER OFF)
g++ ./src/*.cpp -I./inc -lpthread -o ./output/adcm_pc_vieweroff

# target(VIEWER OFF)
aarch64-linux-gnu-g++ ./src/*.cpp -I./inc -lpthread -o ./output/adcm_target_vieweroff

