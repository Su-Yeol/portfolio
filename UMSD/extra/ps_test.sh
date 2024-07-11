#!/bin/bash


port=$1
phy=$2
speed=$3

if [ $# -ne 3 ]; then
    echo "==========================================================="
    echo "Usage: $0 <Port> <Phy M/S> <Speed>"
    echo "       Eg) ./ps.sh 1 M 100"
    echo "       Port 1,2,11,12 -> 100/1000Mbps-t1"
    echo "       Port 3,4,5     -> 100Mbps-t1"
    echo "       Port 13,14,15  -> 10base-t1s"
    echo "       Port 6,16,17 -> MII / Port 0,8,10,18 -> Internal"
    echo "       Port 7 -> Tx / Port 9,19 -> 10Gbps"
    echo "==========================================================="
    exit 1
fi

valid_ports=(1 2 3 4 5 6 7 11 12 13 14 15 16 17)
if ! [[ " ${valid_ports[@]} " =~ " $port " ]]; then
    echo "Invalid port number. Please specify a valid port number."
    exit 1
    
fi

if [ $speed -ne 100 ] && [ $speed -ne 1000 ]; then
    echo "Invalid speed. Please specify either 100 or 1000."
    exit 1
fi

case $port in
    1)
        if [ $phy == "M" ]; then
            echo "Port $port, Phy Master, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x19 0xC000
                phytool write pfe2/0xf/0x18 0x8421
                elif [ $speed -eq 1000 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x19 0xC001
                phytool write pfe2/0xf/0x18 0x8421
            fi
            elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x19 0x8000
                phytool write pfe2/0xf/0x18 0x8421
                elif [ $speed -eq 1000 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x19 0x8001
                phytool write pfe2/0xf/0x18 0x8421
            fi
        else
            echo "Invalid phy mode. Please specify 'M' for Master or 'S' for Slave."
            exit 1
        fi
    ;;
    2)
        if [ $phy == "M" ]; then
            echo "Port $port, Phy Master, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x19 0xC000
                phytool write pfe2/0xf/0x18 0x8441
                elif [ $speed -eq 1000 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x19 0xC001
                phytool write pfe2/0xf/0x18 0x8441
            fi
            elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x19 0x8000
                phytool write pfe2/0xf/0x18 0x8441
                elif [ $speed -eq 1000 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x19 0x8001
                phytool write pfe2/0xf/0x18 0x8441
            fi
        else
            echo "Invalid phy mode. Please specify 'M' for Master or 'S' for Slave."
            exit 1
        fi
    ;;
    11)
        if [ $phy == "M" ]; then
            echo "Port $port, Phy Master, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x19 0xC000
                phytool write pfe1/0xe/0x18 0x8421
                elif [ $speed -eq 1000 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x19 0xC001
                phytool write pfe1/0xe/0x18 0x8421
            fi
            elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x19 0x8000
                phytool write pfe1/0xe/0x18 0x8421
                elif [ $speed -eq 1000 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x19 0x8001
                phytool write pfe1/0xe/0x18 0x8421
            fi
        else
            echo "Invalid phy mode. Please specify 'M' for Master or 'S' for Slave."
            exit 1
        fi
    ;;
    12)
        if [ $phy == "M" ]; then
            echo "Port $port, Phy Master, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x19 0xC000
                phytool write pfe1/0xe/0x18 0x8441
                elif [ $speed -eq 1000 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x19 0xC001
                phytool write pfe1/0xe/0x18 0x8441
            fi
            elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x19 0x8000
                phytool write pfe1/0xe/0x18 0x8441
                elif [ $speed -eq 1000 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x19 0x8001
                phytool write pfe1/0xe/0x18 0x8441
            fi
        else
            echo "Invalid phy mode. Please specify 'M' for Master or 'S' for Slave."
            exit 1
        fi
    ;;
    
    3)
        if [ $phy == "M" ]; then
            echo "Port $port, Phy Master, ${speed}Mbps"
	        phytool write pfe2/0xf/0x19 0x0834
        	phytool write pfe2/0xf/0x18 0x8061
           	phytool write pfe2/0xf/0x19 0xC000
           	phytool write pfe2/0xf/0x18 0x8461
	        elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            phytool write pfe2/0xf/0x19 0x0834
           	phytool write pfe2/0xf/0x18 0x8061
            phytool write pfe2/0xf/0x19 0x8000
            phytool write pfe2/0xf/0x18 0x8461
        else
            echo "Invalid phy mode. Please specify 'M' for Master or 'S' for Slave."
            exit 1
        fi
    ;;
    4)
        if [ $phy == "M" ]; then
            echo "Port $port, Phy Master, ${speed}Mbps"
            phytool write pfe2/0xf/0x19 0x0834
            phytool write pfe2/0xf/0x18 0x8081
            phytool write pfe2/0xf/0x19 0xC000
            phytool write pfe2/0xf/0x18 0x8481
            elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            phytool write pfe2/0xf/0x19 0x0834
            phytool write pfe2/0xf/0x18 0x8081
            phytool write pfe2/0xf/0x19 0x8000
            phytool write pfe2/0xf/0x18 0x8481
        else
            echo "Invalid phy mode. Please specify 'M' for Master or 'S' for Slave."
            exit 1
        fi
    ;;
    5)
        if [ $phy == "M" ]; then
            echo "Port $port, Phy Master, ${speed}Mbps"
            phytool write pfe2/0xf/0x19 0x0834
            phytool write pfe2/0xf/0x18 0x80A1
            phytool write pfe2/0xf/0x19 0xC000
            phytool write pfe2/0xf/0x18 0x84A1
            elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            phytool write pfe2/0xf/0x19 0x0834
            phytool write pfe2/0xf/0x18 0x80A1
            phytool write pfe2/0xf/0x19 0x8000
            phytool write pfe2/0xf/0x18 0x84A1
        else
            echo "Invalid phy mode. Please specify 'M' for Master or 'S' for Slave."
            exit 1
        fi
    ;;
    6|16|17)
        echo "Port $port -> MII"
    ;;
    7)
        echo "Port $port -> TX"
    ;;
    13|14|15)
        echo "Port $port -> 10base-t1s"
        echo "Please use the t1s.sh"
    ;;
    9|19)
        echo "Port $port -> 10Gbps"
    ;;
    *)
        echo "Invalid port number. Please specify a valid port number."
        exit 1
    ;;
esac
