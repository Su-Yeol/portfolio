#!/bin/bash

#sudo ifconfig enp0s3 down
#sudo ifconfig enp0s3 hw ether 00:11:22:33:44:99
#sudo ifconfig enp0s3 192.168.100.99
#ifconfig enp0s3

sudo ip -s -s neigh flush all
sudo arp -i enp0s3 -s 192.168.100.88 00:11:22:33:44:88
#sudo ifconfig enp0s3 up

arp -n
sleep 2

./output/adcm_pc_vieweron
