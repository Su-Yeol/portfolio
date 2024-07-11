#!/bin/bash

port=$1
plca=$2
nodeid=$3


if [ $# -ne 3 ]; then
    echo "==========================================================="
    echo "Usage: $0 <Port> <PLCA Enable> <Node ID> "
    echo "       Eg) ./10base-t1s 13 1 10"
    echo "       Port 13,14,15  -> 10base-t1s"
    echo "==========================================================="
    exit 1
fi

valid_ports=(13 14 15)
if ! [[ " ${valid_ports[@]} " =~ " $port " ]]; then
    echo "Invalid port number. Please specify a valid port number ( 13, 14, 15 )."
    exit 1
    
fi

if ! (( nodeid >= 0 && nodeid <= 255 )); then
    echo "Invalid Node id. Please specify a valid node id ( 0 ~ 255 )."
    exit 1
fi

if [ "$plca" != "0" ] && [ "$plca" != "1" ]; then
    echo "Invalid PLCA Enable. Please specify 0 (Disable) or 1 (Enable)."
    exit 1
fi

hex_nodeid=$(printf "%02X" $nodeid)
hex_data=${hex_nodeid: -2}

case $port in
    13)
        if [ $plca -eq 0 ]; then
            echo "port $port, PLCA Disable, Node ID $nodeid"
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x8063
            phytool write pfe1/0xe/0x19 0x3C$hex_data #data
            echo "nodeid $hex_data"
            phytool write pfe1/0xe/0x18 0x8463
            #read
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x8063
            phytool write pfe1/0xe/0x18 0x8863
            phytool read pfe1/0xe/0x19
            
        elif [ $plca -eq 1 ]; then
            echo "port $port, PLCA Enable, Node ID $nodeid"
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x8063
            phytool write pfe1/0xe/0x19 0x7C$hex_data #data
            echo "nodeid $hex_data"
            phytool write pfe1/0xe/0x18 0x8463
            
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x8063
            phytool write pfe1/0xe/0x18 0x8863
            phytool read pfe1/0xe/0x19
        fi
    ;;
    14)
        if [ $plca -eq 0 ]; then
            echo "port $port, PLCA Disable, Node ID $nodeid"
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x8083
            phytool write pfe1/0xe/0x19 0x3C$hex_data #data
            echo "nodeid $hex_data"
            phytool write pfe1/0xe/0x18 0x8483
            
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x8083
            phytool write pfe1/0xe/0x18 0x8883
            phytool read pfe1/0xe/0x19
            
        elif [ $plca -eq 1 ]; then
            echo "port $port, PLCA Enable, Node ID $nodeid"
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x8083
            phytool write pfe1/0xe/0x19 0x7C$hex_data #data
            echo "nodeid $hex_data"
            phytool write pfe1/0xe/0x18 0x8483
            
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x8083
            phytool write pfe1/0xe/0x18 0x8883
            phytool read pfe1/0xe/0x19
            
        fi
    ;;
    15)
        if [ $plca -eq 0 ]; then
            echo "port $port, PLCA Disable, Node ID $nodeid"
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x80A3
            phytool write pfe1/0xe/0x19 0x3C$hex_data #data
            echo "nodeid $hex_data"
            phytool write pfe1/0xe/0x18 0x84A3
            
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x80A3
            phytool write pfe1/0xe/0x18 0x88A3
            phytool read pfe1/0xe/0x19
            
        elif [ $plca -eq 1 ]; then
            echo "port $port, PLCA Enable, Node ID $nodeid"
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x80A3
            phytool write pfe1/0xe/0x19 0x7C$hex_data #data
            echo "nodeid $hex_data"
            phytool write pfe1/0xe/0x18 0x84A3
            
            phytool write pfe1/0xe/0x19 0xF300
            phytool write pfe1/0xe/0x18 0x80A3
            phytool write pfe1/0xe/0x18 0x88A3
            phytool read pfe1/0xe/0x19
            
        fi
    ;;
esac


