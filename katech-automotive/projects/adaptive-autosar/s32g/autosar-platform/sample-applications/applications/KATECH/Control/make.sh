cd lib
./makelib.sh
cd ..
cd build

make -j8
#scp Control root@192.168.1.99:/opt/Control/bin/Control
cd ..
