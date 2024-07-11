#!/bin/bash
###################################################################
# 88Q5192 port setup script
# 2024. 02. 11

# phytool write IF/Die/regOffset/Data
# IF - pfe0, pfe2, eth0
# Die0 - 0xf - pfe2 / Die1 - 0xe - pfe1
#
# phytool write clause 45
# e.g - IF pfe2, Die 0, Port 4, Dev 1, Reg 0x0834, Data 0xc001
# phytool write pfe2/0xf/0x19 0x0834 (Reg)
# phytool write pfe2/0xf/0x18 0x8
# phytool write pfe2/0xf/0x19 0xc001 (Data)
# phytool wrtie pfe2/0xf/0x18 0x8..
####################################################################

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
    echo "       Port 7 -> Tx / Port 9,19 -> 1Gbps"
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
                # write
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x19 0xC000
                phytool write pfe2/0xf/0x18 0x8421
                #LED Ctr 1
                phytool write pfe2/0xf/0x1 0x8053
                phytool write pfe2/0xf/0x0 0x9436
                # read
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x18 0x8821
                phytool read pfe2/0xf/0x19
            elif [ $speed -eq 1000 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x19 0xC001
                phytool write pfe2/0xf/0x18 0x8421
                
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x18 0x8821
                phytool read pfe2/0xf/0x19
            else
                echo "Invalid speed. Please specify either 100 or 1000."
                exit 1
            fi
        elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x19 0x8000
                phytool write pfe2/0xf/0x18 0x8421
                #LED Ctr 1
                phytool write pfe2/0xf/0x1 0x8053
                phytool write pfe2/0xf/0x0 0x9436

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x18 0x8821
                phytool read pfe2/0xf/0x19
            elif [ $speed -eq 1000 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x19 0x8001
                phytool write pfe2/0xf/0x18 0x8421

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8021
                phytool write pfe2/0xf/0x18 0x8821
                phytool read pfe2/0xf/0x19
            else
                echo "Invalid speed. Please specify either 100 or 1000."
                exit 1
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
                #LED Ctr 2
                phytool write pfe2/0xf/0x1 0x8053
                phytool write pfe2/0xf/0x0 0x9456

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x18 0x8841
                phytool read pfe2/0xf/0x19
            elif [ $speed -eq 1000 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x19 0xC001
                phytool write pfe2/0xf/0x18 0x8441
                
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x18 0x8841
                phytool read pfe2/0xf/0x19
            else
                echo "Invalid speed. Please specify either 100 or 1000."
                exit 1
            fi
        elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x19 0x8000
                phytool write pfe2/0xf/0x18 0x8441
                #LED Ctr 2
                phytool write pfe2/0xf/0x1 0x8053
                phytool write pfe2/0xf/0x0 0x9456

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x18 0x8841
                phytool read pfe2/0xf/0x19
            elif [ $speed -eq 1000 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x19 0x8001
                phytool write pfe2/0xf/0x18 0x8441

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8041
                phytool write pfe2/0xf/0x18 0x8841
                phytool read pfe2/0xf/0x19
            else
                echo "Invalid speed. Please specify either 100 or 1000."
                exit 1
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

                #LED Ctr 1
                phytool write pfe1/0xe/0x1 0x8053
                phytool write pfe1/0xe/0x0 0x9436
                
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x18 0x8821
                phytool read pfe1/0xe/0x19
            elif [ $speed -eq 1000 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x19 0xC001
                phytool write pfe1/0xe/0x18 0x8421
                
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x18 0x8821
                phytool read pfe1/0xe/0x19
            else
                echo "Invalid speed. Please specify either 100 or 1000."
                exit 1
            fi
        elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x19 0x8000
                phytool write pfe1/0xe/0x18 0x8421

                #LED Ctr 1
                phytool write pfe1/0xe/0x1 0x8053
                phytool write pfe1/0xe/0x0 0x9436

                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x18 0x8821
                phytool read pfe1/0xe/0x19
            elif [ $speed -eq 1000 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x19 0x8001
                phytool write pfe1/0xe/0x18 0x8421
                
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8021
                phytool write pfe1/0xe/0x18 0x8821
                phytool read pfe1/0xe/0x19
            else
                echo "Invalid speed. Please specify either 100 or 1000."
                exit 1
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

                #LED Ctr 2
                phytool write pfe1/0xe/0x1 0x8053
                phytool write pfe1/0xe/0x0 0x9456

                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x18 0x8841
                phytool read pfe1/0xe/0x19
            elif [ $speed -eq 1000 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x19 0xC001
                phytool write pfe1/0xe/0x18 0x8441
                
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x18 0x8841
                phytool read pfe1/0xe/0x19
            else
                echo "Invalid speed. Please specify either 100 or 1000."
                exit 1
            fi
        elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x19 0x8000
                phytool write pfe1/0xe/0x18 0x8441
                #LED Ctr 2
                phytool write pfe1/0xe/0x1 0x8053
                phytool write pfe1/0xe/0x0 0x9456
                
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x18 0x8841
                phytool read pfe1/0xe/0x19
            elif [ $speed -eq 1000 ]; then
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x19 0x8001
                phytool write pfe1/0xe/0x18 0x8441
                
                phytool write pfe1/0xe/0x19 0x0834
                phytool write pfe1/0xe/0x18 0x8041
                phytool write pfe1/0xe/0x18 0x8841
                phytool read pfe1/0xe/0x19
            else
                echo "Invalid speed. Please specify either 100 or 1000."
                exit 1
            fi
        else
            echo "Invalid phy mode. Please specify 'M' for Master or 'S' for Slave."
            exit 1
        fi
    ;;
    
    3)
        if [ $phy == "M" ]; then
            echo "Port $port, Phy Master, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8061
                phytool write pfe2/0xf/0x19 0xC000
                phytool write pfe2/0xf/0x18 0x8461

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8061
                phytool write pfe2/0xf/0x18 0x8861
                phytool read pfe2/0xf/0x19
            else
                echo "Invalid speed. Please specify either 100."
                exit 1
            fi
        elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8061
                phytool write pfe2/0xf/0x19 0x8000
                phytool write pfe2/0xf/0x18 0x8461

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8061
                phytool write pfe2/0xf/0x18 0x8861
                phytool read pfe2/0xf/0x19
            else
                echo "Invalid speed. Please specify either 100."
                exit 1
            fi
        else
            echo "Invalid phy mode. Please specify 'M' for Master or 'S' for Slave."
            exit 1
        fi
    ;;
    4)
        if [ $phy == "M" ]; then
            echo "Port $port, Phy Master, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8081
                phytool write pfe2/0xf/0x19 0xC000
                phytool write pfe2/0xf/0x18 0x8481

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8081
                phytool write pfe2/0xf/0x18 0x8881
                phytool read pfe2/0xf/0x19
            else
                echo "Invalid speed. Please specify either 100."
                exit 1
            fi
        elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8081
                phytool write pfe2/0xf/0x19 0x8000
                phytool write pfe2/0xf/0x18 0x8481

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x8081
                phytool write pfe2/0xf/0x18 0x8881
                phytool read pfe2/0xf/0x19
            else
                echo "Invalid speed. Please specify either 100."
                exit 1
            fi
        else
            echo "Invalid phy mode. Please specify 'M' for Master or 'S' for Slave."
            exit 1
        fi
    ;;
    5)
        if [ $phy == "M" ]; then
            echo "Port $port, Phy Master, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x80A1
                phytool write pfe2/0xf/0x19 0xC000
                phytool write pfe2/0xf/0x18 0x84A1

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x80A1
                phytool write pfe2/0xf/0x18 0x88A1
                phytool read pfe2/0xf/0x19
            else
                echo "Invalid speed. Please specify either 100."
                exit 1
            fi
        elif [ $phy == "S" ]; then
            echo "Port $port, Phy Slave, ${speed}Mbps"
            if [ $speed -eq 100 ]; then
                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x80A1
                phytool write pfe2/0xf/0x19 0x8000
                phytool write pfe2/0xf/0x18 0x84A1

                phytool write pfe2/0xf/0x19 0x0834
                phytool write pfe2/0xf/0x18 0x80A1
                phytool write pfe2/0xf/0x18 0x88A1
                phytool read pfe2/0xf/0x19
            else
                echo "Invalid speed. Please specify either 100."
                exit 1
            fi
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
        echo "Port $port -> 1Gbps"
    ;;
    *)
        echo "Invalid port number. Please specify a valid port number."
        exit 1
    ;;
esac
